#include "lounge_car.h"

#include "game/game.h"

#include "cards/game_enums.h"

namespace banggame {

    struct request_lounge_car : request_base {
        request_lounge_car(card *origin_card, player *origin)
            : request_base(origin_card, origin, origin, effect_flags::auto_respond) {}
        
        void on_update() override {
            if (!sent) {
                for (int i=0; i<2 && !origin->m_game->m_train_deck.empty(); ++i) {
                    origin->m_game->move_card(origin->m_game->m_train_deck.front(), pocket_type::selection, origin);
                }
                if (origin->m_game->m_selection.size() <= 1) {
                    origin->m_game->pop_request();
                    while (!origin->m_game->m_selection.empty()) {
                        origin->equip_card(origin->m_game->m_selection.front());
                    }
                }
            }
        }

        game_string status_text(player *owner) const override {
            if (owner == target) {
                return {"STATUS_LOUNGE_CAR", origin_card};
            } else {
                return {"STATUS_LOUNGE_CAR_OTHER", target, origin_card};
            }
        }
    };

    game_string effect_lounge_car::on_prompt(card *origin_card, player *origin) {
        if (origin->m_game->m_train_deck.empty()) {
            return {"PROMPT_CARD_NO_EFFECT", origin_card};
        } else {
            return {};
        }
    }

    void effect_lounge_car::on_play(card *origin_card, player *origin) {
        origin->m_game->queue_request<request_lounge_car>(origin_card, origin);
    }

    game_string handler_lounge_car::get_error(card *origin_card, player *origin, card *target_card, player *target_player) {
        if (origin->m_game->top_request<request_lounge_car>(origin) == nullptr) {
            return "ERROR_INVALID_RESPONSE";
        }
        return {};
    }

    void handler_lounge_car::on_play(card *origin_card, player *origin, card *target_card, player *target_player) {
        origin->m_game->invoke_action([&]{
            origin->m_game->pop_request();

            target_player->equip_card(target_card);

            while (!origin->m_game->m_selection.empty()) {
                origin->equip_card(origin->m_game->m_selection.front());
            }
        });
    }
}