#include "play_verify.h"

#include "cards/effect_list_zip.h"
#include "cards/effect_enums.h"
#include "cards/game_enums.h"
#include "cards/filters.h"

#include "cards/base/requests.h"

#include "utils/utils.h"

namespace banggame {

    struct duplicate_set_unique {
        std::set<player *> players;
        std::set<card *> cards;
        card_cube_count cubes;
    };

    static game_string merge_duplicate_sets(duplicate_set_unique &set, duplicate_set &&other) {
        for (player *p : other.players) {
            if (!set.players.insert(p).second) {
                return {"ERROR_DUPLICATE_PLAYER", p};
            }
        }
        for (card *c : other.cards) {
            if (!set.cards.insert(c).second) {
                return {"ERROR_DUPLICATE_CARD", c};
            }
        }
        for (auto &[card, ncubes] : other.cubes) {
            if ((set.cubes[card] += ncubes) > card->num_cubes) {
                return {"ERROR_NOT_ENOUGH_CUBES_ON", card};
            }
        }
        return {};
    }

    static game_string verify_target_list(player *origin, card *origin_card, bool is_response, const target_list &targets, effect_context &ctx, duplicate_set_unique &duplicates) {
        auto &effects = origin_card->get_effect_list(is_response);

        if (effects.empty()) {
            return "ERROR_EFFECT_LIST_EMPTY";
        }
        
        size_t diff = targets.size() - effects.size();
        if (auto repeatable = origin_card->get_tag_value(tag_type::repeatable)) {
            if (diff < 0 || diff % origin_card->optionals.size() != 0
                || (*repeatable > 0 && diff > (origin_card->optionals.size() * *repeatable)))
            {
                return "ERROR_INVALID_TARGETS";
            }
        } else if (diff != 0 && diff != origin_card->optionals.size()) {
            return "ERROR_INVALID_TARGETS";
        }

        effect_target_list mth_targets;
        for (const auto &[target, effect] : zip_card_targets(targets, origin_card, is_response)) {
            if (!target.is(effect.target)) {
                return "ERROR_INVALID_TARGET_TYPE";
            } else if (effect.type == effect_type::mth_add) {
                mth_targets.emplace_back(effect, target);
            } else if (effect.type == effect_type::ctx_add) {
                if (target.is(target_type::card)) {
                    origin_card->modifier.add_context(origin_card, origin, target.get<target_type::card>(), ctx);
                } else if (target.is(target_type::player)) {
                    origin_card->modifier.add_context(origin_card, origin, target.get<target_type::player>(), ctx);
                } else {
                    return "ERROR_INVALID_TARGET_TYPE";
                }
            }
            
            MAYBE_RETURN(enums::visit_indexed(
                [&]<target_type E>(enums::enum_tag_t<E>, auto && ... args) -> game_string {
                    return play_visitor<E>{origin, origin_card, effect}.get_error(ctx, FWD(args) ...);
                }, target));
        }

        MAYBE_RETURN(origin_card->get_mth(is_response).get_error(origin_card, origin, mth_targets, ctx));

        for (const auto &[target, effect] : zip_card_targets(targets, origin_card, is_response)) {
            MAYBE_RETURN(merge_duplicate_sets(duplicates, enums::visit_indexed([&]<target_type E>(enums::enum_tag_t<E>, auto && ... args) {
                return play_visitor<E>{origin, origin_card, effect}.duplicates(FWD(args) ... );
            }, target)));
        }

        if (ctx.repeat_card != origin_card) {
            switch (origin_card->pocket) {
            case pocket_type::player_hand:
            case pocket_type::player_table:
            case pocket_type::player_character:
                if (origin_card->owner != origin) {
                    return "ERROR_INVALID_CARD_OWNER";
                }
                break;
            case pocket_type::button_row:
            case pocket_type::shop_selection:
            case pocket_type::hidden_deck:
            case pocket_type::stations:
            case pocket_type::train:
                break;
            case pocket_type::scenario_card:
                if (origin_card != origin->m_game->m_scenario_cards.back()) {
                    return "ERROR_INVALID_SCENARIO_CARD";
                }
                break;
            case pocket_type::wws_scenario_card:
                if (origin_card != origin->m_game->m_wws_scenario_cards.back()) {
                    return "ERROR_INVALID_SCENARIO_CARD";
                }
                break;
            default:
                return "ERROR_INVALID_CARD_POCKET";
            }
        }

        return {};
    }
    
    static game_string verify_modifiers(player *origin, card *origin_card, bool is_response, const modifier_list &modifiers, effect_context &ctx, duplicate_set_unique &duplicates) {
        for (const auto &[mod_card, targets] : modifiers) {
            if (!mod_card->is_modifier()) {
                return "ERROR_CARD_IS_NOT_MODIFIER";
            }

            mod_card->modifier.add_context(mod_card, origin, ctx);

            MAYBE_RETURN(merge_duplicate_sets(duplicates, {.cards{ mod_card }}));
            MAYBE_RETURN(verify_target_list(origin, mod_card, is_response, targets, ctx, duplicates));
            MAYBE_RETURN(get_play_card_error(origin, mod_card, ctx));
        }

        for (size_t i=0; i<modifiers.size(); ++i) {
            const auto &[mod_card, targets] = modifiers[i];

            MAYBE_RETURN(mod_card->modifier.get_error(mod_card, origin, origin_card, ctx));
            for (size_t j=0; j<i; ++j) {
                card *mod_card_before = modifiers[j].card;
                MAYBE_RETURN(mod_card_before->modifier.get_error(mod_card_before, origin, mod_card, ctx));
            }
        }
        return {};
    }

    static game_string verify_equip_target(player *origin, card *origin_card, bool is_response, const target_list &targets, const effect_context &ctx) {
        if (is_response) {
            return "ERROR_CANNOT_EQUIP_AS_RESPONSE";
        }

        if (origin_card->pocket == pocket_type::player_hand && origin_card->owner != origin) {
            return "ERROR_INVALID_CARD_OWNER";
        }

        player *target = origin;
        if (origin_card->self_equippable()) {
            if (!targets.empty()) {
                return "ERROR_INVALID_EQUIP_TARGET";
            }
        } else {
            if (targets.size() != 1 || !targets.front().is(target_type::player)) {
                return "ERROR_INVALID_EQUIP_TARGET";
            }
            target = targets.front().get<target_type::player>();
        }
        
        return get_equip_error(origin, origin_card, target, ctx);
    }

    game_string get_play_card_error(player *origin, card *origin_card, const effect_context &ctx) {
        if (card *disabler = origin->m_game->get_disabler(origin_card)) {
            return {"ERROR_CARD_DISABLED_BY", origin_card, disabler};
        }
        if (origin_card->inactive) {
            return {"ERROR_CARD_INACTIVE", origin_card};
        }
        return origin->m_game->call_event<event_type::check_play_card>(origin, origin_card, ctx, game_string{});
    }

    game_string get_equip_error(player *origin, card *origin_card, player *target, const effect_context &ctx) {
        if (origin->m_game->check_flags(game_flags::disable_equipping)) {
            return "ERROR_CANT_EQUIP_CARDS";
        }
        if (origin_card->self_equippable()) {
            if (origin != target) {
                return "ERROR_INVALID_EQUIP_TARGET";
            }
        } else {
            MAYBE_RETURN(filters::check_player_filter(origin, origin_card->equip_target, target));
        }
        if (card *equipped = target->find_equipped_card(origin_card)) {
            return {"ERROR_DUPLICATED_CARD", equipped};
        }
        return {};
    }

    static game_string verify_card_targets(player *origin, card *origin_card, bool is_response, const target_list &targets, const modifier_list &modifiers, effect_context &ctx) {
        if (!is_response && origin->m_game->m_playing != origin) {
            return "ERROR_PLAYER_NOT_IN_TURN";
        }

        duplicate_set_unique duplicates;

        MAYBE_RETURN(verify_modifiers(origin, origin_card, is_response, modifiers, ctx, duplicates));

        if (filters::is_equip_card(origin_card)) {
            MAYBE_RETURN(verify_equip_target(origin, origin_card, is_response, targets, ctx));
        } else if (origin_card->is_modifier()) {
            return "ERROR_CARD_IS_MODIFIER";
        } else {
            MAYBE_RETURN(verify_target_list(origin, origin_card, is_response, targets, ctx, duplicates));
        }

        MAYBE_RETURN(get_play_card_error(origin, origin_card, ctx));
        
        if (origin->m_gold < filters::get_card_cost(origin_card, is_response, ctx)) {
            return "ERROR_NOT_ENOUGH_GOLD";
        }

        return {};
    }

    static game_string check_prompt(player *origin, card *origin_card, bool is_response, const target_list &targets, const effect_context &ctx) {
        effect_target_list mth_targets;
        for (const auto &[target, effect] : zip_card_targets(targets, origin_card, is_response)) {
            if (effect.type == effect_type::mth_add) {
                mth_targets.emplace_back(effect, target);
            }
            MAYBE_RETURN(enums::visit_indexed([&]<target_type E>(enums::enum_tag_t<E>, auto && ... args) {
                return play_visitor<E>{origin, origin_card, effect}.prompt(ctx, FWD(args) ... );
            }, target));
        }

        return origin_card->get_mth(is_response).on_prompt(origin_card, origin, mth_targets, ctx);
    }

    static game_string check_prompt_play(player *origin, card *origin_card, bool is_response, const target_list &targets, const modifier_list &modifiers, const effect_context &ctx) {
        for (const auto &[mod_card, mod_targets] : modifiers) {
            MAYBE_RETURN(mod_card->modifier.on_prompt(mod_card, origin, origin_card, ctx));
            MAYBE_RETURN(check_prompt(origin, mod_card, is_response, mod_targets, ctx));
        }
        if (filters::is_equip_card(origin_card)) {
            player *target = origin_card->self_equippable() ? origin
                : targets.front().get<target_type::player>().get();
            for (const auto &e : origin_card->equips) {
                MAYBE_RETURN(e.on_prompt(origin_card, origin, target));
            }
            return {};
        } else {
            return check_prompt(origin, origin_card, is_response, targets, ctx);
        }
    }

    static void log_played_card(card *origin_card, player *origin, bool is_response) {
        if (origin_card->has_tag(tag_type::skip_logs)) return;
        
        switch (origin_card->pocket) {
        case pocket_type::player_hand:
        case pocket_type::scenario_card:
        case pocket_type::hidden_deck:
            if (!origin_card->name.empty()) {
                if (is_response) {
                    origin->m_game->add_log("LOG_RESPONDED_WITH_CARD", origin_card, origin);
                } else {
                    origin->m_game->add_log("LOG_PLAYED_CARD", origin_card, origin);
                }
            }
            break;
        case pocket_type::player_table:
            if (is_response) {
                origin->m_game->add_log("LOG_RESPONDED_WITH_CARD", origin_card, origin);
            } else {
                origin->m_game->add_log("LOG_PLAYED_TABLE_CARD", origin_card, origin);
            }
            break;
        case pocket_type::player_character:
            if (is_response) {
                if (origin_card->has_tag(tag_type::drawing)) {
                    origin->m_game->add_log("LOG_DRAWN_WITH_CHARACTER", origin_card, origin);
                } else {
                    origin->m_game->add_log("LOG_RESPONDED_WITH_CHARACTER", origin_card, origin);
                }
            } else {
                origin->m_game->add_log("LOG_PLAYED_CHARACTER", origin_card, origin);
            }
            break;
        case pocket_type::shop_selection:
            origin->m_game->add_log("LOG_BOUGHT_CARD", origin_card, origin);
            break;
        case pocket_type::stations:
            origin->m_game->add_log("LOG_PAID_FOR_STATION", origin_card, origin);
            break;
        }
    }

    static void log_equipped_card(card *origin_card, player *origin, player *target) {
        if (origin_card->pocket == pocket_type::shop_selection) {
            if (origin == target) {
                origin->m_game->add_log("LOG_BOUGHT_EQUIP", origin_card, origin);
            } else {
                origin->m_game->add_log("LOG_BOUGHT_EQUIP_TO", origin_card, origin, target);
            }
        } else {
            if (origin == target) {
                origin->m_game->add_log("LOG_EQUIPPED_CARD", origin_card, origin);
            } else {
                origin->m_game->add_log("LOG_EQUIPPED_CARD_TO", origin_card, origin, target);
            }
        }
    }

    void apply_target_list(player *origin, card *origin_card, bool is_response, const target_list &targets, const effect_context &ctx) {
        log_played_card(origin_card, origin, is_response);

        if (origin_card != ctx.repeat_card && !origin_card->has_tag(tag_type::no_auto_discard)) {
            switch (origin_card->pocket) {
            case pocket_type::player_hand:
                origin->discard_used_card(origin_card);
                break;
            case pocket_type::player_table:
                if (origin_card->is_green()) {
                    origin->discard_card(origin_card);
                }
                break;
            case pocket_type::shop_selection:
                origin->m_game->move_card(origin_card, pocket_type::shop_discard);
                origin->m_game->queue_action([m_game=origin->m_game]{
                    if (m_game->m_shop_selection.size() < 3) {
                        m_game->draw_shop_card();
                    }
                }, -1);
            }
        }

        effect_target_list mth_targets;
        card_cube_count selected_cubes;

        for (const auto &[target, effect] : zip_card_targets(targets, origin_card, is_response)) {
            if (effect.type == effect_type::mth_add) {
                mth_targets.emplace_back(effect, target);
            } else if (effect.type == effect_type::pay_cube) {
                if (target.is(target_type::select_cubes)) {
                    for (card *c : target.get<target_type::select_cubes>()) {
                        ++selected_cubes[c];
                    }
                } else if (target.is(target_type::self_cubes)) {
                    selected_cubes[origin_card] += effect.target_value;
                }
            }
        }
        for (const auto &[c, ncubes] : selected_cubes) {
            origin->m_game->move_cubes(c, nullptr, ncubes);
        }
        for (const auto &[target, effect] : zip_card_targets(targets, origin_card, is_response)) {
            enums::visit_indexed([&]<target_type E>(enums::enum_tag_t<E>, auto && ... args) {
                play_visitor<E>{origin, origin_card, effect}.play(ctx, FWD(args) ... );
            }, target);
        }

        origin_card->get_mth(is_response).on_play(origin_card, origin, mth_targets, ctx);
    }

    void apply_equip(player *origin, card *origin_card, const target_list &targets, const effect_context &ctx) {
        player *target = origin_card->self_equippable() ? origin
            : targets.front().get<target_type::player>().get();
            
        origin->m_game->queue_action([=]{ 
            if (!origin->alive()) return;

            log_equipped_card(origin_card, origin, target);
            
            if (origin_card->pocket == pocket_type::player_hand) {
                origin->m_game->call_event<event_type::on_discard_hand_card>(origin, origin_card, true);
            }

            target->equip_card(origin_card);

            origin->m_game->call_event<event_type::on_equip_card>(origin, target, origin_card, ctx);
        });
    }

    game_message verify_and_pick(player *origin, const pick_card_args &args) {
        auto req = origin->m_game->top_request(origin);

        if (!req || !req->can_pick(args.card)) {
            return {enums::enum_tag<message_type::error>, "ERROR_INVALID_PICK"};
        }

        if (!args.bypass_prompt) {
            if (game_string prompt = req->pick_prompt(args.card)) {
                return {enums::enum_tag<message_type::prompt>, prompt};
            }
        }
        
        origin->m_game->send_request_status_clear();

        req->on_pick(args.card);
        req.reset();

        origin->m_game->update();
        return {};
    }

    game_message verify_and_play(player *origin, const play_card_args &args) {
        bool is_response = origin->m_game->pending_requests();

        effect_context ctx;

        if (game_string error = verify_card_targets(origin, args.card, is_response, args.targets, args.modifiers, ctx)) {
            return {enums::enum_tag<message_type::error>, error};
        }

        if (!args.bypass_prompt) {
            if (game_string prompt = check_prompt_play(origin, args.card, is_response, args.targets, args.modifiers, ctx)) {
                return {enums::enum_tag<message_type::prompt>, prompt};
            }
        }

        origin->m_game->send_request_status_clear();

        if (args.card->pocket != pocket_type::button_row) {
            origin->m_played_cards.emplace_back(args.card, args.modifiers, ctx);
        }

        origin->add_gold(-filters::get_card_cost(args.card, is_response, ctx));

        for (const auto &[mod_card, mod_targets] : args.modifiers) {
            apply_target_list(origin, mod_card, is_response, mod_targets, ctx);
        }

        if (filters::is_equip_card(args.card)) {
            apply_equip(origin, args.card, args.targets, ctx);
        } else {
            apply_target_list(origin, args.card, is_response, args.targets, ctx);
        }

        origin->m_game->update();

        return {};
    }
}