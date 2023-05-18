#include "bigfifty.h"

#include "game/game.h"
#include "cards/effect_context.h"

namespace banggame {

    void effect_bigfifty::on_play(card *origin_card, player *p) {
        p->m_game->add_listener<event_type::apply_bang_modifier>(origin_card, [=](player *origin, request_bang *req) {
            if (origin == p) {
                origin->m_game->add_disabler(origin_card, [target = req->target](card *c) {
                    return (c->pocket == pocket_type::player_table
                        || c->pocket == pocket_type::player_character)
                        && c->owner == target;
                });
                req->on_cleanup([=]{
                    p->m_game->remove_disablers(origin_card);
                });
                origin->m_game->remove_listeners(origin_card);
            }
        });
    }

    void modifier_bigfifty::add_context(card *origin_card, player *origin, effect_context &ctx) {
        ctx.disable_bang_checks = true;
    }
}