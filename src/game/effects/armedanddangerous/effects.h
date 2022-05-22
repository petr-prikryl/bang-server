#ifndef __ARMEDANDDANGEROUS_EFFECTS_H__
#define __ARMEDANDDANGEROUS_EFFECTS_H__

#include "../card_effect.h"

namespace banggame {

    struct handler_draw_atend {
        void on_play(card *origin_card, player *origin, const target_list &targets);
    };

    struct handler_heal_multi {
        opt_fmt_str on_prompt(card *origin_card, player *origin, const target_list &targets) const;
        void on_play(card *origin_card, player *origin, const target_list &targets);
    };

    struct effect_select_cube {
        opt_error verify(card *origin_card, player *origin, card *target) const;
        void on_play(card *origin_card, player *origin, card *target);
    };

    struct effect_pay_cube {
        int ncubes;
        effect_pay_cube(int value) : ncubes(std::max(1, value)) {}
        
        bool can_respond(card *origin_card, player *origin) const;
        opt_error verify(card *origin_card, player *origin) const;
        void on_play(card *origin_card, player *origin);
    };

    struct effect_add_cube {
        int ncubes;
        effect_add_cube(int value) : ncubes(std::max(1, value)) {}

        void on_play(card *origin_card, player *origin, card *target);
    };
    
    struct effect_reload {
        void on_play(card *origin_card, player *origin);
    };

    struct effect_rust {
        void on_play(card *origin_card, player *origin, player *target, effect_flags flags = {});
        void on_resolve(card *origin_card, player *origin, player *target);
    };

    struct effect_doublebarrel {
        void on_play(card *origin_card, player *origin);
    };

    struct effect_thunderer {
        void on_play(card *origin_card, player *origin);
    };

    struct effect_buntlinespecial {
        void on_play(card *origin_card, player *origin);
    };

    struct effect_bigfifty {
        void on_play(card *origin_card, player *origin);
    };

    struct handler_flintlock {
        void on_play(card *origin_card, player *origin, player *target, std::optional<target_none_t> paid_cubes);
    };

    struct effect_bandolier : effect_empty {
        opt_error verify(card *origin_card, player *origin) const;
    };

    struct handler_duck {
        void on_play(card *origin_card, player *origin, std::optional<target_none_t> paid_cubes);
    };

    struct handler_squaw {
        opt_error verify(card *origin_card, player *origin, card *discarded_card, std::optional<target_cubes_t> paid_cubes) const;
        void on_play(card *origin_card, player *origin, card *discarded_card, std::optional<target_cubes_t> paid_cubes);
    };

    struct effect_move_bomb : effect_empty {
        bool can_respond(card *origin_card, player *origin) const;
    };

    struct handler_move_bomb {
        opt_fmt_str on_prompt(card *origin_card, player *origin, player *target) const;
        opt_error verify(card *origin_card, player *origin, player *target) const;
        void on_play(card *origin_card, player *origin, player *target);
    };
}

#endif