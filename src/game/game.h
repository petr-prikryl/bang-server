#ifndef __GAME_H__
#define __GAME_H__

#include "events.h"
#include "game_table.h"
#include "request_queue.h"
#include "player_iterator.h"
#include "utils/utils.h"
#include "utils/generator.h"

namespace banggame {

    struct game : game_table, listener_map, request_queue {
        player *m_playing = nullptr;

        game() : request_queue(this) {}

        util::generator<json::json> get_rejoin_updates(player *target);

        card_sign get_card_sign(card *c);

        void add_players(std::span<int> user_ids);
        void start_game(const game_options &options);

        request_status_args make_request_update(player *p);
        status_ready_args make_status_ready_update(player *p);
        player_order_update make_player_order_update(bool instant = false);

        void send_request_status_clear();
        bool send_request_status_ready();
        void send_request_update();

        void start_next_turn();

        void draw_check_then(player *origin, card *origin_card, draw_check_condition condition, draw_check_function fun);

        void handle_player_death(player *killer, player *target, discard_all_reason reason);
    };

}

#endif