#pragma once

#include "../lua_internal.hpp"
#include "../lua_callback.hpp"

#include "lua_entity.hpp"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/patterns/patterns.h"

#include "../../engine/logging/logging.h"

#include "../../source engine/game_events.h"

#include "../../ui/ui.h"

#include "../../features/movement/movement.h"

namespace lua_internal::tables::client {
    inline void log(lua_State *l) {
        size_t len;
        logging::info(luaL_checklstring(l, 1, &len));
    }

    inline void exec(lua_State *l) {
        size_t len;
        interfaces::engine_client->execute_command(luaL_checklstring(l, 1, &len));
    }

    inline std::string time(lua_State *l) {
        size_t len;

        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);

        std::stringstream ss;
        ss << std::put_time(&tm, luaL_checklstring(l, 1, &len));

        return ss.str();
    }

    inline int find_pattern(lua_State *l) {
        size_t len;
        return patterns::get_pattern(luaL_checklstring(l, 1, &len), luaL_checklstring(l, 2, &len));
    }

    inline entity userid_to_entity(lua_State *l) {
        const int user_id = interfaces::engine_client->get_player_for_user_id(luaL_checkinteger(l, 1));

        auto user = interfaces::entity_list->get_entity(user_id);
        if (!user) {
            return entity(nullptr, 0);
        }

        return entity(user, user->get_networkable()->index());
    }

    inline color accent_color(lua_State *l) {
        const color_t &col = ui::get_accent_color();
        return {col.r, col.g, col.b, col.a};
    }

    inline bool in_edgebug(lua_State *l) {
        return features::movement::predicted_successful && interfaces::global_vars->tick_count < features::movement::prediction_ticks + features::movement::prediction_timestamp;
    }

    inline bool edgebugged(lua_State *l) {
        return features::movement::edgebugged;
    }

    inline void print_to_chat(lua_State *l) {
        size_t len;
        interfaces::client_mode->chat->chat_printf(0, 0, luaL_checklstring(l, 1, &len));
    }

    inline void print_to_chat_html(lua_State *l) {
        size_t len;

        cheat::notice_text = luaL_checklstring(l, 1, &len);

        const std::string str = "##" + cheat::notice_text;
        interfaces::client_mode->chat->chat_printf(0, 0, str.c_str());
    }
}

inline void lua_internal::context::lua_client() {
    luabridge::getGlobalNamespace(l)
    .beginNamespace(xs("mw"))
        .addFunction(xs("log"), std::function([this]() { tables::client::log(l); }))
        .addFunction(xs("exec"), std::function([this]() { tables::client::exec(l); }))
        .addFunction(xs("time"), std::function([this]() { return tables::client::time(l); }))
        .addFunction(xs("unix_time"), std::function([this]() { return std::time(nullptr); }))
        .addFunction(xs("userid_to_entity"), std::function([this]() { return tables::client::userid_to_entity(l); }))
        .addFunction(xs("find_pattern"), std::function([this]() { return tables::client::find_pattern(l); }))
        .addFunction(xs("accent_color"), std::function([this]() { return tables::client::accent_color(l); }))
        .addFunction(xs("in_edgebug"), std::function([this]() { return tables::client::in_edgebug(l); }))
        .addFunction(xs("edgebugged"), std::function([this]() { return tables::client::edgebugged(l); }))
        .addFunction(xs("print_to_chat"), std::function([this]() { tables::client::print_to_chat(l); }))
        .addFunction(xs("print_to_chat_html"), std::function([this]() { tables::client::print_to_chat_html(l); }))

    .endNamespace();
}