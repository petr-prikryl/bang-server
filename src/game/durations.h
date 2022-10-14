#ifndef __DURATIONS_H__
#define __DURATIONS_H__

#include "net_options.h"

#include <chrono>

namespace banggame {

    using namespace std::chrono_literals;

    template<std::integral T>
    using ticks_t = std::chrono::duration<T, std::ratio<1, server_tickrate>>;
    
    using ticks = ticks_t<int>;

    static constexpr ticks default_timer_duration = 2s;

    static constexpr ticks auto_confirm_duration = 5s;

    static constexpr ticks lobby_lifetime = 10s;
}

#endif