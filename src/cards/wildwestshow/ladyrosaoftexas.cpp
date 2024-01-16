#include "ladyrosaoftexas.h"

#include "cards/game_enums.h"

#include "game/game.h"
#include "game/bot_suggestion.h"

namespace banggame {

    static player *get_next_player(player *origin) {
        player_iterator it{origin};
        if (origin->m_game->check_flags(game_flags::invert_rotation)) {
            ++it;
        } else {
            --it;
        }
        return *it;
    }

    bool effect_ladyrosaoftexas::on_check_target(card *origin_card, player *origin) {
        return bot_suggestion::target_enemy{}.on_check_target(origin_card, origin, get_next_player(origin));
    }

    game_string effect_ladyrosaoftexas::get_error(card *origin_card, player *origin) {
        if (origin->m_game->num_alive() <= 2) {
            return {"ERROR_CANT_PLAY_CARD", origin_card};
        } else {
            return {};
        }
    }

    void effect_ladyrosaoftexas::on_play(card *origin_card, player *origin) {
        player *target = get_next_player(origin);
        target->add_player_flags(player_flags::skip_turn);
        std::iter_swap(
            rn::find(origin->m_game->m_players, origin),
            rn::find(origin->m_game->m_players, target));
        origin->m_game->add_update<game_update_type::player_order>(origin->m_game->make_player_order_update());
    }
}