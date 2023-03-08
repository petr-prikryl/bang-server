#ifndef __GREATTRAINROBBERY_GHOST_CAR_H__
#define __GREATTRAINROBBERY_GHOST_CAR_H__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_ghost_car {
        void on_equip(card *origin_card, player *origin);
        void on_unequip(card *origin_card, player *origin);
    };
}

#endif