#include "doc_holyday.h"

#include "cards/base/bang.h"
#include "cards/base/steal_destroy.h"

#include "game/game.h"

namespace banggame {

    void handler_doc_holyday::on_play(card *origin_card, player *origin, tagged_value<target_type::cards> target_cards, player *target) {
        for (card *c : target_cards.value) {
            effect_discard{}.on_play(origin_card, origin, c);
        }
        if (!rn::all_of(target_cards.value, [&](card *target_card) {
            return target->immune_to(target_card, origin, {}, true);
        })) {
            effect_bang{}.on_play(origin_card, origin, target);
        } else {
            for (card *target_card : target_cards.value) {
                target->immune_to(target_card, origin, {});
            }
        }
    }
}