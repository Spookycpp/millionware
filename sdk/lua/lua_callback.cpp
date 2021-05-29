#include "lua_callback.hpp"

lua_internal::callback::callback(lua_State* l, std::string name, const bool is_game_event)
    : l(l), m_ref(luabridge::newTable(l)), m_name(std::move(name)), m_is_game_event(is_game_event) {

}

void lua_internal::callback::dump_stack() const {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 13);
	std::cout << "-------- stack --------\n";
	const int top = lua_gettop(l);
	for (int i = 1; i <= top; i++)
	{
		printf("%d\t%s\t", i, luaL_typename(l, i));
		switch (lua_type(l, i))
		{
		case LUA_TNUMBER:
			printf("%g\n", lua_tonumber(l, i));
			break;
		case LUA_TSTRING:
			printf("%s\n", lua_tostring(l, i));
			break;
		case LUA_TBOOLEAN:
			printf("%s\n", lua_toboolean(l, i) ? "true" : "false");
			break;
		case LUA_TNIL:
			printf("%s\n", "nil");
			break;
		default:
			printf("%p\n", lua_topointer(l, i));
			break;
		}
	}
	std::cout << "-----------------------\n";
	SetConsoleTextAttribute(hConsole, 15);
}
