#ifndef __BASE_DAMAGE_H__
#define __BASE_DAMAGE_H__

#include "../card_effect.h"

namespace banggame {
    
    struct effect_damage {
        int amount;
        effect_damage(int amount) : amount(std::max(1, amount)) {}

        void on_play(card *origin_card, player *origin, player *target, effect_flags flags = {});
        void on_resolve(card *origin_card, player *origin, player *target, effect_flags flags = {});
    };

    struct effect_self_damage {
        game_string verify(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin);
    };

    struct timer_damaging : timer_request, cleanup_request {
        timer_damaging(card *origin_card, player *origin, player *target, int damage, effect_flags flags = {})
            : timer_request(origin_card, origin, target, flags)
            , damage(damage) {}
        
        int damage;

        std::vector<card *> get_highlights() const override;
        void on_finished() override;
        game_string status_text(player *owner) const override;
    };
}

#endif