#include "request_queue.h"

#include "game.h"

#include "utils/type_name.h"

namespace banggame {
    
    static ticks get_total_update_time(game *game) {
        return ranges::max(game->m_players | ranges::views::transform([&](player *p) {
            return ranges::accumulate(game->m_updates | ranges::views::transform([&](const game_update_tuple &tup) {
                if (tup.duration >= ticks{0} && tup.target.matches(p)) {
                    return tup.duration;
                }
                return ticks{0};
            }), ticks{0});
        }));
    }

    request_queue::state_t request_queue::invoke_update() {
        if (m_game->is_game_over()) {
            return state_done{};
        } else if (auto req = top_request()) {
#ifndef NDEBUG
            fmt::print("{: >5}: {}\n", req->priority, utils::demangle(typeid(*req).name()));
            fflush(stdout);
#endif

            req->on_update();
            req->live = true;
            
            if (top_request() != req) {
                return state_next{};
            }
            if (auto *timer = req->timer()) {
                if (timer->get_duration() <= ticks{0}) {
                    pop_request();
                    timer->on_finished();
                    return state_next{};
                }
                timer->start(get_total_update_time(m_game));
            }
            m_game->send_request_update();
        } else if (m_game->m_playing && !m_game->send_request_status_ready()) {
            return state_next{};
        }
        if (std::ranges::any_of(m_game->m_players, &player::is_bot)) {
            if (m_game->m_options.bot_play_timer > game_duration{0}) {
                return state_bot_play{ get_total_update_time(m_game) + clamp_ticks(m_game->m_options.bot_play_timer) };
            } else if (m_game->request_bot_play()) {
                return state_next{};
            }
        }
        return state_done{};
    }

    request_queue::state_t request_queue::invoke_tick_update() {
        if (m_game->is_game_over()) {
            return state_done{};
        } else if (auto req = top_request()) {
            if (auto *timer = req->timer()) {
                timer->tick();
                if (timer->finished()) {
                    m_game->send_request_status_clear();
                    pop_request();
                    timer->on_finished();
                    return state_next{};
                }
            }
        }

        return std::visit(overloaded{
            [](const auto &state) -> state_t { return state; },
            [](const state_waiting &state) -> state_t {
                if (state.timer > ticks{}) {
                    return state_waiting{ state.timer - ticks{1} };
                } else {
                    return state_next{};
                }
            },
            [&](const state_bot_play &state) -> state_t {
                if (state.timer > ticks{}) {
                    return state_bot_play{ state.timer - ticks{1} };
                } else if (m_game->request_bot_play()) {
                    return state_next{};
                } else {
                    return state_done{};
                }
            }
        }, m_state);
    }
    
    void request_queue::tick() {
        m_state = invoke_tick_update();

        if (std::holds_alternative<state_next>(m_state)) {
            commit_updates();
        }
    }

    static constexpr ticks max_update_timer_duration = 10s;
    static constexpr int max_update_count = 30;
    
    void request_queue::commit_updates() {
        int count = 0;
        do {
            auto timer = get_total_update_time(m_game);
            if (timer > max_update_timer_duration || count > max_update_count) {
                m_state = state_waiting{ timer };
            } else {
                m_state = invoke_update();
                ++count;
            }
        } while (std::holds_alternative<state_next>(m_state));
    }
}