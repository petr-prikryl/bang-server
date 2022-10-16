#ifndef __VALLEYOFSHADOWS_SNAKE_H__
#define __VALLEYOFSHADOWS_SNAKE_H__

#include "../card_effect.h"

namespace banggame {

    struct effect_snake : event_based_effect {
        void on_enable(card *target_card, player *target);
    };
}

#endif