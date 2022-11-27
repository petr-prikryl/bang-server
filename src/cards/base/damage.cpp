#include "damage.h"

#include "game/game.h"
#include "deathsave.h"

namespace banggame {
    
    game_string effect_damage::verify(card *origin_card, player *origin, effect_flags flags) {
        if (origin->m_hp <= damage) {
            return "ERROR_CANT_SELF_DAMAGE";
        }
        return {};
    }

    void effect_damage::on_play(card *origin_card, player *origin, player *target, effect_flags flags) {
        target->m_game->queue_request<request_damage>(origin_card, origin, target, damage, flags);
    }

    static constexpr auto damaging_allowed_flags = effect_flags::is_bang | effect_flags::play_as_bang | effect_flags::multi_target;

    request_damage::request_damage(card *origin_card, player *origin, player *target, int damage, effect_flags flags)
        : request_base(origin_card, origin, target, flags & damaging_allowed_flags)
        , damage(damage) {}
    
    std::vector<card *> request_damage::get_highlights() const {
        return target->m_backup_character;
    }

    bool request_damage::auto_resolve() {
        if (target->is_ghost()) {
            target->m_game->pop_request();
            return true;
        }
        if (!target->m_game->call_event<event_type::check_damage_response>(false)) {
            auto lock = target->m_game->lock_updates(true);
            on_finished();
            return true;
        }
        return false;
    }

    void request_damage::on_finished() {
        if (bool(flags & effect_flags::play_as_bang)) {
            if (bool(flags & effect_flags::multi_target)) {
                target->m_game->add_log("LOG_TAKEN_DAMAGE_AS_GATLING", origin_card, target);
            } else if (damage > 1) {
                target->m_game->add_log("LOG_TAKEN_DAMAGE_AS_BANG_PLURAL", origin_card, target, damage);
            } else {
                target->m_game->add_log("LOG_TAKEN_DAMAGE_AS_BANG", origin_card, target);
            }
        } else {
            if (damage > 1) {
                target->m_game->add_log("LOG_TAKEN_DAMAGE_PLURAL", origin_card, target, damage);
            } else {
                target->m_game->add_log("LOG_TAKEN_DAMAGE", origin_card, target);
            }
        }
        target->set_hp(target->m_hp - damage);
        target->m_game->call_event<event_type::before_hit>(origin_card, origin, target, damage, flags);
        if (target->m_hp <= 0) {
            target->m_game->queue_request<request_death>(origin_card, origin, target);
        }
        target->m_game->call_event<event_type::after_hit>(origin_card, origin, target, damage, flags);
    }

    game_string request_damage::status_text(player *owner) const {
        if (bool(flags & effect_flags::play_as_bang)) {
            if (bool(flags & effect_flags::multi_target)) {
                return {"STATUS_DAMAGING_AS_GATLING", target, origin_card};
            } else if (damage > 1) {
                return {"STATUS_DAMAGING_AS_BANG_PLURAL", target, origin_card, damage};
            } else {
                return {"STATUS_DAMAGING_AS_BANG", target, origin_card};
            }
        } else {
            if (damage > 1) {
                return {"STATUS_DAMAGING_PLURAL", target, origin_card, damage};
            } else {
                return {"STATUS_DAMAGING", target, origin_card};
            }
        }
    }

}