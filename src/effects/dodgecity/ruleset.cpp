#include "ruleset.h"

#include "game/game.h"

namespace banggame {

    void ruleset_dodgecity::on_apply(game *game) {
        game->add_listener<event_type::on_equip_card>({nullptr, 5}, [](player_ptr origin, player_ptr target, card_ptr target_card, const effect_context &ctx) {
            if (target_card->is_green()) {
                target_card->set_inactive(true);
            }
        });

        game->add_listener<event_type::on_turn_end>({nullptr, 5}, [](player_ptr origin, bool skipped) {
            for (card_ptr target_card : origin->m_table) {
                target_card->set_inactive(false);
            }
        });

        game->add_listener<event_type::check_play_card>(nullptr, [](player_ptr origin, card_ptr target_card, const effect_context &ctx, game_string &out_error) {
            if (target_card->is_green() && target_card->inactive) {
                out_error = {"ERROR_CARD_INACTIVE", target_card};
            }
        });
    }
}