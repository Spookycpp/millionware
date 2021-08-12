#pragma once

#include "lua_internal.hpp"

namespace lua_internal {
	class callback {
	private:
	public:
		lua_State* l;
		luabridge::LuaRef ref;

		std::string name;
		uint32_t hash;

        bool is_game_event;

		callback(lua_State *l, std::string name, bool is_game_event = false);
		callback(lua_State *l, std::string name, uint32_t hash, bool is_game_event = false);
	};
}