#ifndef __GAME_EVENTS_H__
#define __GAME_EVENTS_H__

#include "cards/card_fwd.h"

namespace banggame {
    enum class range_mod_type {
        range_mod,
        weapon_range,
        distance_mod
    };

    enum class escape_type {
        no_escape,
        escape_timer,
        escape_no_timer
    };
}

namespace banggame::event_type {

    struct on_game_setup {
        player_ptr first_player;
    };
    
    struct apply_sign_modifier {
        nullable_ref<card_sign> value;
    };

    struct apply_maxcards_modifier {
        const_player_ptr origin;
        nullable_ref<int> value;
    };

    struct apply_immunity_modifier {
        card_ptr origin_card;
        player_ptr origin;
        const_player_ptr target;
        effect_flags flags;
        nullable_ref<card_list> cards;
    };

    struct apply_escapable_modifier {
        card_ptr origin_card;
        player_ptr origin;
        const_player_ptr target;
        effect_flags flags;
        nullable_ref<escape_type> value;
    };
    
    struct count_range_mod {
        const_player_ptr origin;
        range_mod_type type;
        nullable_ref<int> value;
    };

    struct count_initial_cards {
        const_player_ptr origin;
        nullable_ref<int> value;
    };
    
    struct check_play_card {
        player_ptr origin;
        card_ptr origin_card;
        const effect_context &ctx;
        nullable_ref<game_string> out_error;
    };
    
    struct check_revivers {
        player_ptr origin;
    };
    
    struct on_equip_card {
        player_ptr origin;
        player_ptr target;
        card_ptr target_card;
        const effect_context &ctx;
    };

    struct on_discard_hand_card {
        player_ptr origin;
        card_ptr target_card;
        bool used;
    };

    struct on_turn_switch {
        player_ptr origin;
    };

    struct pre_turn_start {
        player_ptr origin;
    };

    struct on_turn_start {
        player_ptr origin;
    };
    
    struct on_turn_end {
        player_ptr origin;
        bool skipped;
    };
}

#endif