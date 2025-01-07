#ifndef __LEGENDS_CHUCK_ALUCK_H__
#define __LEGENDS_CHUCK_ALUCK_H__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_chuck_a_luck : event_equip {
        void on_enable(card_ptr target_card, player_ptr target);
    };

    DEFINE_EQUIP(chuck_a_luck, equip_chuck_a_luck)
}

#endif