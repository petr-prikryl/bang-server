#include "holders.h"

#include <stdexcept>

#include "cards/effects.h"

#include "game.h"
#include "mth_unwrapper.h"

namespace banggame {

    template<card_expansion_type E>
    inline void do_apply_ruleset(game *game, enums::enum_tag_t<E>) {
        if constexpr (enums::value_with_type<E>) {
            if (bool(game->m_options.expansions & E)) {
                using type = enums::enum_type_t<E>;
                type{}.on_apply(game);
            }
        }
    }

    void game::apply_rulesets() {
        [&]<card_expansion_type ... Es>(enums::enum_sequence<Es ...>) {
            (do_apply_ruleset(this, enums::enum_tag<Es>), ...);
        }(enums::make_enum_sequence<card_expansion_type>());
    }

    template<typename Holder, typename Function>
    static auto visit_effect(Function &&fun, Holder &holder) {
        return enums::visit_enum([&]<decltype(Holder::type) E>(enums::enum_tag_t<E>) {
            if constexpr (enums::value_with_type<E>) {
                using type = enums::enum_type_t<E>;
                if constexpr (requires { type{holder.effect_value}; }) {
                    return fun(type{holder.effect_value});
                } else {
                    return fun(type{});
                }
            } else {
                return fun(std::monostate{});
            }
        }, holder.type);
    }

    void effect_holder::add_context(card *origin_card, player *origin, effect_context &ctx) const {
        visit_effect([&](auto &&value) {
            if constexpr (requires { value.add_context(origin_card, origin, ctx); }) {
                value.add_context(origin_card, origin, ctx);
            }
        }, *this);
    }

    game_string effect_holder::verify(card *origin_card, player *origin, const effect_context &ctx) const {
        return visit_effect([&](auto &&value) -> game_string {
            if constexpr (requires { value.verify(origin_card, origin, ctx); }) {
                return value.verify(origin_card, origin, ctx);
            } else if constexpr (requires { value.verify(origin_card, origin); }) {
                return value.verify(origin_card, origin);
            } else if constexpr (requires { value.can_respond(origin_card, origin); }) {
                if (!value.can_respond(origin_card, origin)) {
                    return "ERROR_INVALID_RESPONSE";
                }
            }
            return {};
        }, *this);
    }

    game_string effect_holder::on_prompt(card *origin_card, player *origin) const {
        return visit_effect([&](auto &&value) -> game_string {
            if constexpr (requires { value.on_check_target(origin_card, origin); }) {
                if (origin->is_bot() && !value.on_check_target(origin_card, origin)) {
                    return "BOT_BAD_PLAY";
                }
            }
            if constexpr (requires { value.on_prompt(origin_card, origin); }) {
                return value.on_prompt(origin_card, origin);
            }
            return {};
        }, *this);
    }

    void effect_holder::on_play(card *origin_card, player *origin, effect_flags flags) const {
        visit_effect([&](auto &&value) {
            if constexpr (requires { value.on_play(origin_card, origin, flags); }) {
                value.on_play(origin_card, origin, flags);
            } else if constexpr (requires { value.on_play(origin_card, origin); }) {
                value.on_play(origin_card, origin);
            }
        }, *this);
    }

    void effect_holder::add_context(card *origin_card, player *origin, player *target, effect_context &ctx) const {
        visit_effect([&](auto &&value) {
            if constexpr (requires { value.add_context(origin_card, origin, target, ctx); }) {
                value.add_context(origin_card, origin, target, ctx);
            }
        }, *this);
    }

    game_string effect_holder::verify(card *origin_card, player *origin, player *target, const effect_context &ctx) const {
        return visit_effect([&](auto &&value) -> game_string {
            if constexpr (requires { value.verify(origin_card, origin, target, ctx); }) {
                return value.verify(origin_card, origin, target, ctx);
            } else if constexpr (requires { value.verify(origin_card, origin, target); }) {
                return value.verify(origin_card, origin, target);
            }
            return {};
        }, *this);
    }

    game_string effect_holder::on_prompt(card *origin_card, player *origin, player *target) const {
        return visit_effect([&](auto &&value) -> game_string {
            if constexpr (requires { value.on_check_target(origin_card, origin, target); }) {
                if (origin->is_bot() && !value.on_check_target(origin_card, origin, target)) {
                    return "BOT_BAD_PLAY";
                }
            }
            if constexpr (requires { value.on_prompt(origin_card, origin, target); }) {
                return value.on_prompt(origin_card, origin, target);
            }
            return {};
        }, *this);
    }

    void effect_holder::on_play(card *origin_card, player *origin, player *target, effect_flags flags) const {
        visit_effect([&](auto &&value) {
            if constexpr (requires { value.on_play(origin_card, origin, target, flags); }) {
                value.on_play(origin_card, origin, target, flags);
            } else if constexpr (requires { value.on_play(origin_card, origin, target); }) {
                value.on_play(origin_card, origin, target);
            }
        }, *this);
    }

    void effect_holder::add_context(card *origin_card, player *origin, card *target, effect_context &ctx) const {
        visit_effect([&](auto &&value) {
            if constexpr (requires { value.add_context(origin_card, origin, target, ctx); }) {
                value.add_context(origin_card, origin, target, ctx);
            }
        }, *this);
    }

    game_string effect_holder::verify(card *origin_card, player *origin, card *target, const effect_context &ctx) const {
        return visit_effect([&](auto &&value) -> game_string {
            if constexpr (requires { value.verify(origin_card, origin, target, ctx); }) {
                return value.verify(origin_card, origin, target, ctx);
            } else if constexpr (requires { value.verify(origin_card, origin, target); }) {
                return value.verify(origin_card, origin, target);
            }
            return {};
        }, *this);
    }

    game_string effect_holder::on_prompt(card *origin_card, player *origin, card *target) const {
        return visit_effect([&](auto &&value) -> game_string {
            if constexpr (requires { value.on_check_target(origin_card, origin, target); }) {
                if (origin->is_bot() && !value.on_check_target(origin_card, origin, target)) {
                    return "BOT_BAD_PLAY";
                }
            }
            if constexpr (requires { value.on_prompt(origin_card, origin, target); }) {
                return value.on_prompt(origin_card, origin, target);
            }
            return {};
        }, *this);
    }

    void effect_holder::on_play(card *origin_card, player *origin, card *target, effect_flags flags) const {
        visit_effect([&](auto &&value) {
            if constexpr (requires { value.on_play(origin_card, origin, target, flags); }) {
                value.on_play(origin_card, origin, target, flags);
            } else if constexpr (requires { value.on_play(origin_card, origin, target); }) {
                value.on_play(origin_card, origin, target);
            }
        }, *this);
    }

    game_string equip_holder::on_prompt(card *origin_card, player *origin, player *target) const {
        return visit_effect([&](auto &&value) -> game_string {
            if constexpr (requires { value.on_check_target(origin_card, origin, target); }) {
                if (origin->is_bot() && !value.on_check_target(origin_card, origin, target)) {
                    return "BOT_BAD_PLAY";
                }
            }
            if constexpr (requires { value.on_prompt(origin_card, origin, target); }) {
                return value.on_prompt(origin_card, origin, target);
            }
            return {};
        }, *this);
    }

    void equip_holder::on_equip(card *target_card, player *target) const {
        visit_effect([&](auto &&value) {
            if constexpr (requires { value.on_equip(target_card, target); }) {
                value.on_equip(target_card, target);
            }
        }, *this);
    }

    void equip_holder::on_enable(card *target_card, player *target) const {
        visit_effect([&](auto &&value) {
            if constexpr (requires { value.on_enable(target_card, target); }) {
                value.on_enable(target_card, target);
            }
        }, *this);
    }

    void equip_holder::on_disable(card *target_card, player *target) const {
        visit_effect([&](auto &&value) {
            if constexpr (requires { value.on_disable(target_card, target); }) {
                value.on_disable(target_card, target);
            }
        }, *this);
    }

    void equip_holder::on_unequip(card *target_card, player *target) const {
        visit_effect([&](auto &&value) {
            if constexpr (requires { value.on_unequip(target_card, target); }) {
                value.on_unequip(target_card, target);
            }
        }, *this);
    }

    game_string modifier_holder::on_prompt(card *origin_card, player *origin, card *playing_card) const {
        return enums::visit_enum([&]<card_modifier_type E>(enums::enum_tag_t<E>) -> game_string {
            if constexpr (enums::value_with_type<E>) {
                enums::enum_type_t<E> handler;
                if constexpr (requires { handler.on_prompt(origin_card, origin, playing_card); }) {
                    return handler.on_prompt(origin_card, origin, playing_card);
                }
            }
            return {};
        }, type);
    }

    void modifier_holder::add_context(card *origin_card, player *origin, card *playing_card, effect_context &ctx) const {
        enums::visit_enum([&]<card_modifier_type E>(enums::enum_tag_t<E>) {
            if constexpr (enums::value_with_type<E>) {
                enums::enum_type_t<E> handler;
                if constexpr (requires { handler.add_context(origin_card, origin, playing_card, ctx); }) {
                    handler.add_context(origin_card, origin, playing_card, ctx);
                }
            }
        }, type);
    }

    game_string modifier_holder::verify(card *origin_card, player *origin, card *playing_card, const effect_context &ctx) const {
        return enums::visit_enum([&]<card_modifier_type E>(enums::enum_tag_t<E>) -> game_string {
            if constexpr (enums::value_with_type<E>) {
                enums::enum_type_t<E> handler;
                if constexpr (requires { handler.verify(origin_card, origin, playing_card, ctx); }) {
                    return handler.verify(origin_card, origin, playing_card, ctx);
                } else if constexpr (requires { handler.verify(origin_card, origin, playing_card); }) {
                    return handler.verify(origin_card, origin, playing_card);
                }
            }
            return {};
        }, type);
    }

    void mth_holder::add_context(card *origin_card, player *origin, const target_list &targets, effect_context &ctx) const {
        enums::visit_enum([&]<mth_type E>(enums::enum_tag_t<E>) {
            if constexpr (enums::value_with_type<E>) {
                using handler_type = enums::enum_type_t<E>;
                if constexpr (requires { mth_unwrapper{&handler_type::add_context}; }) {
                    mth_unwrapper{&handler_type::add_context}(origin_card, origin, targets, ctx);
                }
            }
        }, type);
    }

    game_string mth_holder::verify(card *origin_card, player *origin, const target_list &targets, const effect_context &ctx) const {
        return enums::visit_enum([&]<mth_type E>(enums::enum_tag_t<E>) -> game_string {
            if constexpr (enums::value_with_type<E>) {
                using handler_type = enums::enum_type_t<E>;
                if constexpr (requires { mth_unwrapper{&handler_type::verify}; }) {
                    return mth_unwrapper{&handler_type::verify}(origin_card, origin, targets, ctx);
                }
            }
            return {};
        }, type);
    }

    game_string mth_holder::on_prompt(card *origin_card, player *origin, const target_list &targets, const effect_context &ctx) const {
        return enums::visit_enum([&]<mth_type E>(enums::enum_tag_t<E>) -> game_string {
            if constexpr (enums::value_with_type<E>) {
                using handler_type = enums::enum_type_t<E>;
                if constexpr (requires { mth_unwrapper{&handler_type::on_check_target}; }) {
                    if (origin->is_bot() && !mth_unwrapper{&handler_type::on_check_target}(origin_card, origin, targets, ctx)) {
                        return "BOT_BAD_PLAY";
                    }
                }
                if constexpr (requires { mth_unwrapper{&handler_type::on_prompt}; }) {
                    return mth_unwrapper{&handler_type::on_prompt}(origin_card, origin, targets, ctx);
                }
            }
            return {};
        }, type);
    }
    
    void mth_holder::on_play(card *origin_card, player *origin, const target_list &targets, const effect_context &ctx) const {
        enums::visit_enum([&]<mth_type E>(enums::enum_tag_t<E>) {
            if constexpr (enums::value_with_type<E>) {
                using handler_type = enums::enum_type_t<E>;
                return mth_unwrapper{&handler_type::on_play}(origin_card, origin, targets, ctx);
            }
        }, type);
    }

}