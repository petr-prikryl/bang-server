#include "game/play_verify.h"

#include "game/filters.h"
#include "cards/filter_enums.h"
#include "cards/game_enums.h"

namespace banggame {

    using visit_player = play_visitor<"player">;

    template<> bool visit_player::possible(const effect_context &ctx) {
        return bool(make_player_target_set(origin, origin_card, effect, ctx));
    }

    template<> player *visit_player::random_target(const effect_context &ctx) {
        return random_element(make_player_target_set(origin, origin_card, effect, ctx), origin->m_game->bot_rng);
    }

    template<> game_string visit_player::get_error(const effect_context &ctx, player *target) {
        MAYBE_RETURN(filters::check_player_filter(origin, effect.player_filter, target, ctx));
        return effect.get_error(origin_card, origin, target, ctx);
    }

    template<> game_string visit_player::prompt(const effect_context &ctx, player *target) {
        return effect.on_prompt(origin_card, origin, target, ctx);
    }

    template<> void visit_player::add_context(effect_context &ctx, player *target) {
        ctx.selected_players.push_back(target);
        effect.add_context(origin_card, origin, target, ctx);
    }

    template<> void visit_player::play(const effect_context &ctx, player *target) {
        effect_flags flags = effect_flag::single_target;
        if (origin_card->is_brown()) {
            flags.add(effect_flag::escapable);
        }
        effect.on_play(origin_card, origin, target, flags, ctx);
    }

}