#include "can_play_card.h"

#include "game/game.h"

namespace banggame {

    void request_can_play_card::on_resolve() {
        target->m_game->pop_request();
    }
    
    game_string request_can_play_card::status_text(player_ptr owner) const {
        if (target == owner) {
            return {"STATUS_CAN_PLAY_CARD", origin_card};
        } else {
            return {"STATUS_CAN_PLAY_CARD_OTHER", target, origin_card};
        }
    }

    bool effect_can_play_card::can_play(card_ptr origin_card, player_ptr origin) {
        auto req = origin->m_game->top_request<request_can_play_card>(origin);
        return req && req->origin_card == origin_card;
    }

    void effect_can_play_card::on_play(card_ptr origin_card, player_ptr origin) {
        origin->m_game->pop_request();
    }
}
