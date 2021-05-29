#include "lua_context.hpp"
#include "lua_callback.hpp"

// types
#include "types/lua_vec2d.hpp"
#include "types/lua_vec3d.hpp"
#include "types/lua_color.hpp"

// tables
#include "tables/lua_client.hpp"
#include "tables/lua_draw.hpp"
#include "tables/lua_entity.hpp"

#include "../engine/logging/logging.h"
#include "../engine/security/xorstr.h"

std::array<luaL_Reg, 7> libs = {{
	{"", luaopen_base},
	{LUA_MATHLIBNAME, luaopen_math},
	{LUA_STRLIBNAME, luaopen_string},
	{LUA_TABLIBNAME, luaopen_table},
	//{LUA_OSLIBNAME, luaopen_os},
	{LUA_LOADLIBNAME, luaopen_package},
	//{LUA_DBLIBNAME, luaopen_debug},
	{LUA_BITLIBNAME, luaopen_bit},
	{LUA_JITLIBNAME, luaopen_jit},
	//{LUA_FFILIBNAME, luaopen_ffi},
}};

void hook(lua_State *l, lua_Debug *dbg) {
    
}

void lua_internal::context::new_state() {
	l = luaL_newstate();
    lua_sethook(l, hook, LUA_MASKCOUNT, 1);

	luabridge::enableExceptions(l);

	// load standard libraries
	for (auto& [name, func] : libs) {
		lua_pushcfunction(l, func);
		lua_pushstring(l, name);
		lua_call(l, 1, 0);
	}

	setup_tables();
}

void lua_internal::context::setup_tables() {
	// init mw table
	luabridge::getGlobalNamespace(l)
	.beginNamespace("mw")
		.addFunction("register_callback", std::function([this]() {
			size_t len;

			// first argument is the event name
            const char *event_name = luaL_checklstring(l, -2, &len);

			// ensure this callback hasn't already been registered
			/*if (const auto it = std::ranges::find_if(callbacks, [func_name](const callback& cb) {
				return cb.name == func_name;
			}); it != callbacks.end()) {
				return 0;
			}*/

			const callback cb(l, event_name);
			
			// save function ref to push back onto the stack later
			cb.ref[1] = luaL_ref(l, LUA_REGISTRYINDEX);
			
			callbacks.push_back(cb);
		}))
		.addFunction("remove_callback", std::function([this]() {
			size_t len;

			// first argument is the event name
			const char* event_name = luaL_checklstring(l, -2, &len);

			// TODO: FINISH THIS, need to figure out a unique identifier for each callback
			// compare name, unref, and erase
            callbacks.erase(std::ranges::remove_if(callbacks, [this, event_name](const callback &cb) {
			    // luaL_unref()
				return false;
			}).begin(), callbacks.end());
		}))
	    .addFunction("register_event_callback", std::function([this]() {
			size_t len;

			// first argument is the event name
            const char *event_name = luaL_checklstring(l, -2, &len);

            if (!interfaces::_event_listener->exists(event_name)) {
				// event is not already registered by us
                if (!interfaces::_event_listener->add(event_name)) {
                    logging::error(XORSTR("Failed to register event: {}"), event_name);
                    return;
                }
			}

            const callback cb(l, event_name, true);

			// save function ref to push back onto the stack later
            cb.ref[1] = luaL_ref(l, LUA_REGISTRYINDEX);

			callbacks.push_back(cb);
		}))
	.endNamespace();

	// types
	lua_vec2d(l);
    lua_vec3d(l);
    lua_color(l);

	// tables
    lua_client();
    lua_draw();
    entity();
}

void lua_internal::context::ffi(const bool state) const {
	if (state) {
		lua_pushcfunction(l, luaopen_ffi);
		lua_pushstring(l, LUA_FFILIBNAME);
		lua_call(l, 1, 0);
	}
	else {
		lua_getglobal(l, "ffi");
		lua_pushnil(l);
		lua_pop(l, 1);
	}
}

bool lua_internal::context::load(const std::string& path) {
    script_path = path;
	new_state();

	if (luaL_loadfile(l, script_path.c_str())) {
        logging::error(std::string(lua_tostring(l, -1)));
		lua_pop(l, 1);
		return false;
	}

	return true;
}

bool lua_internal::context::run() const {
	if (/*luabridge::LuaException::*/lua_pcall(l, 0, 0, 0)) {
        logging::error(std::string(lua_tostring(l, -1)));
		lua_pop(l, 1);
		return false;
	}

	return true;
}

void lua_internal::context::exit() {
    callbacks.clear();
    exiting = true;
}

std::string lua_internal::context::filename() const {
    return std::filesystem::path(script_path).stem().string();
}