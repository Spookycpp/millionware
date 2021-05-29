#include "lua_callback.hpp"

lua_internal::callback::callback(lua_State* l, std::string name, const bool is_game_event)
    : l(l), ref(luabridge::newTable(l)), name(std::move(name)), is_game_event(is_game_event) {

}
