#ifndef __MANAGER_H__
#define __MANAGER_H__

#include "lobby.h"
#include "chat_commands.h"

#include <random>

namespace banggame {

template<server_message_type E>
server_message make_message(auto && ... args) {
    return server_message{enums::enum_tag<E>, FWD(args) ...};
}

using send_message_function = std::function<void(client_handle, server_message)>;
using kick_client_function = std::function<void(client_handle, const std::string &message)>;
using client_ip_function = std::function<std::string(client_handle)>;

struct server_options {
    bool enable_cheats = false;
    bool verbose = false;
};

class game_manager {
public:
    game_manager();

    template<std::invocable<client_handle, server_message> Function>
    void set_send_message_function(Function &&fun) {
        m_send_message = [this, fun=std::forward<Function>(fun)](client_handle hdl, server_message msg) {
            if (m_options.verbose) {
                std::cout << m_get_client_ip(hdl) << ": Sent " << json::serialize(msg) << std::endl;
            }
            std::invoke(fun, hdl, std::move(msg));
        };
    }

    void set_kick_client_function(kick_client_function &&fun) {
        m_kick_client = std::move(fun);
    }

    void set_client_ip_function(client_ip_function &&fun) {
        m_get_client_ip = std::move(fun);
    }

    void on_receive_message(client_handle client, const client_message &msg);

    void client_disconnected(client_handle client);
    
    bool client_validated(client_handle client) const;

    void tick();

    void send_lobby_update(const lobby &lobby);

    template<server_message_type E>
    void send_message(client_handle client, auto && ... args) {
        m_send_message(client, make_message<E>(FWD(args) ... ));
    }

    template<server_message_type E>
    void broadcast_message(auto && ... args) {
        auto msg = make_message<E>(FWD(args) ... );
        for (client_handle client : users | std::views::keys) {
            m_send_message(client, msg);
        }
    }

    template<server_message_type E>
    void broadcast_message_lobby(const lobby &lobby, auto && ... args) {
        auto msg = make_message<E>(FWD(args) ... );
        for (auto [team, it] : lobby.users) {
            m_send_message(it->first, msg);
        }
    }

    void kick_client(client_handle client, const std::string &message) {
        if (m_kick_client) {
            m_kick_client(client, message);
        }
    }

    server_options &options() { return m_options; }

private:
    void kick_user_from_lobby(user_ptr user);

    std::string handle_message(MSG_TAG(connect),        client_handle client, const connect_args &value);
    std::string handle_message(MSG_TAG(pong),           user_ptr user);
    std::string handle_message(MSG_TAG(user_edit),      user_ptr user, const user_info &value);
    std::string handle_message(MSG_TAG(lobby_list),     user_ptr user);
    std::string handle_message(MSG_TAG(lobby_make),     user_ptr user, const lobby_info &value);
    std::string handle_message(MSG_TAG(lobby_edit),     user_ptr user, const lobby_info &args);
    std::string handle_message(MSG_TAG(lobby_join),     user_ptr user, const lobby_id_args &value);
    std::string handle_message(MSG_TAG(lobby_leave),    user_ptr user);
    std::string handle_message(MSG_TAG(lobby_chat),     user_ptr user, const lobby_chat_client_args &value);
    std::string handle_message(MSG_TAG(lobby_return),   user_ptr user);
    std::string handle_message(MSG_TAG(game_start),     user_ptr user);
    std::string handle_message(MSG_TAG(game_action),    user_ptr user, const json::json &value);

    std::string handle_chat_command(user_ptr user, const std::string &command);

    std::string command_print_help(user_ptr user);
    std::string command_print_users(user_ptr user);
    std::string command_kick_user(user_ptr user, std::string_view userid);
    std::string command_get_game_options(user_ptr user);
    std::string command_set_game_option(user_ptr user, std::string_view name, std::string_view value);
    std::string command_give_card(user_ptr user, std::string_view name);
    std::string command_set_team(user_ptr user, std::string_view value);
    std::string command_get_rng_seed(user_ptr user);

private:
    user_map users;
    lobby_list m_lobbies;

    server_options m_options;

    int generate_lobby_id();
    int generate_user_id(int user_id);

    std::default_random_engine m_rng;
    
    send_message_function m_send_message;
    kick_client_function m_kick_client;
    client_ip_function m_get_client_ip;

    friend struct lobby;
    friend class chat_command;
};

}

#endif