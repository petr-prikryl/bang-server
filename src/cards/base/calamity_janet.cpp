#include "calamity_janet.h"

#include "cards/game_enums.h"

#include "game/game.h"

namespace banggame {

    void equip_calamity_janet::on_enable(card *origin_card, player *p) {
        p->add_player_flags(player_flags::treat_missed_as_bang);
    }

    void equip_calamity_janet::on_disable(card *origin_card, player *p) {
        p->remove_player_flags(player_flags::treat_missed_as_bang);
    }
}