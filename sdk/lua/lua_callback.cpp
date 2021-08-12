#include "lua_callback.hpp"

lua_internal::callback::callback(lua_State* l, std::string name, const bool is_game_event)
    : l(l), ref(luabridge::newTable(l)), name(std::move(name)), hash(0), is_game_event(is_game_event) {

}

lua_internal::callback::callback(lua_State *l, std::string name, const uint32_t hash, const bool is_game_event)
    : l(l), ref(luabridge::newTable(l)), name(std::move(name)), hash(hash), is_game_event(is_game_event) {

}
