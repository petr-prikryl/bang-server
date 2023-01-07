#include "sacagaway.h"

#include "game/game.h"

namespace banggame {
    
    void equip_sacagaway::on_enable(card *target_card, player *target) {
        target->m_game->add_game_flags(game_flags::hands_shown);

        for (player *p : target->m_game->m_players) {
            for (card *c : p->m_hand) {
                target->m_game->set_card_visibility(c, nullptr);
            }
        }
    }

    void equip_sacagaway::on_disable(card *target_card, player *target) {
        target->m_game->remove_game_flags(game_flags::hands_shown);

        for (player *p : target->m_game->m_players) {
            for (card *c : p->m_hand) {
                target->m_game->set_card_visibility(c, p);
            }
        }
    }
}