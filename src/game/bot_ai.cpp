#include "game.h"
#include "play_verify.h"
#include "possible_to_play.h"
#include "net/bot_info.h"

#include "cards/effect_enums.h"
#include "cards/filters.h"

namespace banggame {

    struct bot_error : std::exception {
        game_string message;
        bot_error(game_string message): message{message} {}
    };

    template<rn::range Range, typename Rng>
    decltype(auto) random_element(Range &&range, Rng &rng) {
        rn::range_value_t<Range> ret;
        if (rn::sample(std::forward<Range>(range), &ret, 1, rng).out == &ret) {
            throw bot_error{"EMPTY_RANGE_IN_RANDOM_ELEMENT"};
        }
        return ret;
    }

    struct random_target_visitor {
        player *origin;
        card *origin_card;
        const effect_holder &holder;
        const effect_context &ctx;

        player *operator()(enums::enum_tag_t<target_type::player>) const {
            return random_element(make_player_target_set(origin, origin_card, holder, ctx), origin->m_game->rng);
        }

        player *operator()(enums::enum_tag_t<target_type::conditional_player>) const {
            auto targets = rn::to<std::vector>(make_player_target_set(origin, origin_card, holder, ctx));
            if (targets.empty()) {
                return nullptr;
            } else {
                return random_element(targets, origin->m_game->rng);
            }
        }

        serial::player_list operator()(enums::enum_tag_t<target_type::adjacent_players>) const {
            auto targets = rn::to<std::vector>(make_adjacent_players_target_set(origin, origin_card, ctx));
            auto [target1, target2] = random_element(targets, origin->m_game->rng);
            return {target1, target2};
        }

        card *operator()(enums::enum_tag_t<target_type::card>) const {
            auto targets = rn::to<std::vector>(make_card_target_set(origin, origin_card, holder, ctx));
            return random_element(targets, origin->m_game->rng);
        }

        card *operator()(enums::enum_tag_t<target_type::extra_card> tag) const {
            if (ctx.repeat_card) {
                return nullptr;
            } else {
                auto targets = rn::to<std::vector>(make_card_target_set(origin, origin_card, holder, ctx));
                return random_element(targets, origin->m_game->rng);
            }
        }

        auto operator()(enums::enum_tag_t<target_type::cards> tag) const {
            auto targets = rn::to<std::vector>(make_card_target_set(origin, origin_card, holder, ctx));
            return targets
                | rv::sample(holder.target_value, origin->m_game->rng)
                | rn::to<serial::card_list>;
        }

        auto operator()(enums::enum_tag_t<target_type::cards_other_players>) const {
            serial::card_list ret;
            for (player *target : range_other_players(origin)) {
                if (auto targets = rv::concat(
                    target->m_table | rv::remove_if(&card::is_black),
                    target->m_hand | rv::take(1)
                )) {
                    ret.push_back(random_element(targets, origin->m_game->rng));
                }
            }
            return ret;
        }

        auto operator()(enums::enum_tag_t<target_type::select_cubes>) const {
            auto cubes = origin->cube_slots()
                | rv::for_each([](card *slot) {
                    return rv::repeat_n(slot, slot->num_cubes);
                })
                | rn::to<std::vector>;
            return cubes
                | rv::sample(holder.target_value, origin->m_game->rng)
                | rn::to<serial::card_list>;
        }
    };

    static play_card_target generate_random_target(player *origin, card *origin_card, const effect_holder &holder, const effect_context &ctx) {
        return enums::visit_enum([&]<target_type E>(enums::enum_tag_t<E> tag) -> play_card_target {
            if constexpr (play_card_target::has_type<E>) {
                return {tag, random_target_visitor{origin, origin_card, holder, ctx}(tag)};
            } else {
                return tag;
            }
        }, holder.target);
    }

    static play_card_args generate_random_play(player *origin, const card_modifier_node &node, bool is_response) {
        play_card_args ret;
        effect_context ctx;

        const card_modifier_node *cur_node = &node;
        card *playing_card = nullptr;
        while (!playing_card) {
            if (cur_node->branches.empty()) {
                playing_card = cur_node->card;
                if (filters::is_equip_card(playing_card)) {
                    if (!playing_card->self_equippable()) {
                        ret.targets.emplace_back(enums::enum_tag<target_type::player>,
                            random_element(make_equip_set(origin, playing_card, ctx), origin->m_game->rng));
                    }
                } else {
                    for (const effect_holder &holder : playing_card->get_effect_list(is_response)) {
                        ret.targets.push_back(generate_random_target(origin, playing_card, holder, ctx));
                    }
                    if (is_possible_to_play_effects(origin, playing_card, playing_card->optionals, ctx)) {
                        for (const effect_holder &holder : playing_card->optionals) {
                            ret.targets.push_back(generate_random_target(origin, playing_card, holder, ctx));
                        }
                    }
                }
            } else {
                card *origin_card = cur_node->card;
                auto &targets = ret.modifiers.emplace_back(origin_card).targets;

                origin_card->modifier.add_context(origin_card, origin, ctx);
                for (const effect_holder &holder : origin_card->get_effect_list(is_response)) {
                    const auto &target = targets.emplace_back(generate_random_target(origin, origin_card, holder, ctx));
                    if (holder.type == effect_type::ctx_add) {
                        if (target.is(target_type::card)) {
                            origin_card->modifier.add_context(origin_card, origin, target.get<target_type::card>(), ctx);
                        } else if (target.is(target_type::player)) {
                            origin_card->modifier.add_context(origin_card, origin, target.get<target_type::player>(), ctx);
                        }
                    }
                }

                cur_node = random_element(cur_node->branches
                    | rv::transform([](const card_modifier_node &node) { return &node; }),
                    origin->m_game->rng);
            }
        }
        ret.card = playing_card;
        return ret;
    }

    struct compare_card_node {
        bool operator ()(const card_modifier_node &lhs, const card_modifier_node &rhs) const {
            return lhs.card == rhs.card
                ? rn::lexicographical_compare(lhs.branches, rhs.branches, compare_card_node{})
                : get_card_order(lhs.card) < get_card_order(rhs.card);
        }
    };

    struct play_card_node {
        const card_modifier_node *node;
        
        auto operator < (const play_card_node &other) const {
            return compare_card_node{}(*node, *other.node);
        }
    };

    struct pick_card_node {
        card *target_card;
        
        auto operator < (const pick_card_node &other) const {
            return get_card_order(target_card) < get_card_order(other.target_card);
        }
    };

    using play_or_pick_node = std::variant<std::monostate, play_card_node, pick_card_node>;
    using play_card_node_set = std::set<play_or_pick_node>;

    play_card_node_set make_node_set(const card_modifier_tree &play_cards, const serial::card_list &pick_cards = {}, bool add_empty_action = false) {
        play_card_node_set ret;
        for (const card_modifier_node &node : play_cards) {
            ret.emplace(play_card_node{ &node });
        }
        for (card *target_card : pick_cards) {
            ret.emplace(pick_card_node{ target_card });
        }
        if (add_empty_action) {
            ret.emplace(std::monostate{});
        }
        return ret;
    }

    static bool execute_random_play(player *origin, bool is_response, std::optional<timer_id_t> timer_id, const play_card_node_set &in_node_set) {
        auto &pockets = is_response ? bot_info.settings.response_pockets : bot_info.settings.in_play_pockets;
        
        for (int i=0; i < bot_info.settings.max_random_tries; ++i) {
            auto node_set = in_node_set;
            
            while (!node_set.empty()) {
                play_or_pick_node selected_node = [&]{
                    for (pocket_type pocket : pockets) {
                        if (auto filter = node_set
                            | rv::filter([&](const play_or_pick_node &node) {
                                if (auto *play_card = std::get_if<play_card_node>(&node)) {
                                    return play_card->node->card->pocket == pocket;
                                }
                                return false;
                            }))
                        {
                            return random_element(filter, origin->m_game->rng);
                        }
                    }
                    return random_element(node_set, origin->m_game->rng);
                }();

                node_set.erase(selected_node);

                // maybe add random variation?
                bool bypass_prompt = node_set.empty() && i >= bot_info.settings.bypass_prompt_after;
                if (auto [message, done] = std::visit(overloaded{
                    [](std::monostate) {
                        if (bot_info.settings.allow_timer_no_action) {
                            return std::pair{game_message{}, false};
                        } else {
                            return std::pair{game_message{enums::enum_tag<message_type::error>, "NO_EMPTY_ACTION"}, false};
                        }
                    },
                    [&](const play_card_node &node) {
                        try {
                            auto args = generate_random_play(origin, *(node.node), is_response);
                            args.bypass_prompt = bypass_prompt;
                            args.timer_id = timer_id;
                            return std::pair{verify_and_play(origin, args), true};
                        } catch (const bot_error &error) {
                            return std::pair{game_message{enums::enum_tag<message_type::error>, error.message}, false};
                        }
                    },
                    [&](const pick_card_node &node) {
                        return std::pair{verify_and_pick(origin, { node.target_card, bypass_prompt, timer_id }), true};
                    }
                }, selected_node); message.is(message_type::ok)) {
                    return done;
                }
            }
        }

        // softlock
        fmt::print(stderr, "BOT ERROR: could not find card in execute_random_play()\n");

        return false;
    }

    bool game::request_bot_play() {
        if (pending_requests()) {
            for (player *origin : m_players | rv::filter(&player::is_bot)) {
                auto update = make_request_update(origin);
                
                if (!update.pick_cards.empty() || !update.respond_cards.empty()) {
                    std::optional<timer_id_t> timer_id;
                    if (update.timer) timer_id = update.timer->timer_id;

                    if (execute_random_play(origin, true, timer_id, make_node_set(update.respond_cards, update.pick_cards, timer_id.has_value()))) {
                        return true;
                    }
                }
            }
        } else if (m_playing && m_playing->is_bot()) {
            auto update = make_status_ready_update(m_playing);

            return execute_random_play(m_playing, false, std::nullopt, make_node_set(update.play_cards));
        }
        return false;
    }

    
}