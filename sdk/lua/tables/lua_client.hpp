#pragma once

#include "../lua_internal.hpp"
#include "../lua_callback.hpp"

#include "../../core/interfaces/interfaces.h"
#include "../../core/patterns/patterns.h"

#include "../../engine/logging/logging.h"

#include "../../source engine/game_events.h"

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
}

inline void lua_internal::context::lua_client() {
    luabridge::getGlobalNamespace(l)
    .beginNamespace("mw")
        .addFunction("log", std::function([this]() { tables::client::log(l); }))
        .addFunction("exec", std::function([this]() { tables::client::exec(l); }))
        .addFunction("time", std::function([this]() { return tables::client::time(l); }))
        .addFunction("unix_time", std::function([this]() { return std::time(nullptr); }))
        .addFunction("find_pattern", std::function([this]() { return tables::client::find_pattern(l); }))

    .endNamespace();
}