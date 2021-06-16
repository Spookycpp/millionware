#include "lua_game.hpp"

#include "../engine/logging/logging.h"

bool lua::init() {
    // create the main cheat directory and subdirectories for scripts and other stuff
    // i dont think you can use `std::filesystem::create_directories` to create nested directories
    // so we have to do it manually like this, fix this if you know a better way AND FUCKING TEST IT PLEASE
    if (!std::filesystem::exists(XORSTR(".\\mw"))) {
        std::filesystem::create_directories(XORSTR(".\\mw"));
        std::filesystem::create_directories(XORSTR(".\\mw\\scripts"));
    }

    try {
        handler.add_script(lua_internal::get_script_paths());
    }
    catch (std::filesystem::filesystem_error &e) {
        if (e.code() == std::errc::no_such_file_or_directory) {
            return false;
        }
    }

    for (auto &script : handler.loaded()) {
        // enable ffi for this script
        script.ffi();

        if (!script.run()) {
            // script failed to run, e.g runtime error
            lua_close(script.l);
        }
    }

    if (!mutex.initialized) {
        InitializeCriticalSection(&mutex.critical_section);
        mutex.initialized = true;
    }

    // only ran on successful script execution
    callbacks::startup();

    return true;
}

void lua::reload() {
    handler.unload();
    init();
}

void lua::callbacks::startup() {
    if (!mutex.initialized) {
        return;
    }

    EnterCriticalSection(&mutex.critical_section);
    try {
        for (auto &it : handler.events(XORSTR("startup"))) {
            // push reference to lua func back onto the stack
            lua_rawgeti(it.l, LUA_REGISTRYINDEX, it.ref[1]);

            // call the lua function
            lua_pcall(it.l, 0, 0, 0);
        }
    }
    catch (luabridge::LuaException &ex) {
        logging::error(ex.what());
    }
    LeaveCriticalSection(&mutex.critical_section);
}

void lua::callbacks::run_events(c_game_event *game_event) {
    constexpr std::array<std::pair<const char *, int>, 58> keys = {{
        // integer, byte
        {"userid", 1},
        {"health", 1},
        {"armor", 1},
        {"dmg_health", 1},
        {"dmg_armor", 1},
        {"userid", 1},
        {"userid", 1},
        {"userid", 1},
        {"hitgroup", 1},
        {"team", 1},
        {"loadout", 1},
        {"site", 1},
        {"entindex", 1},
        {"hostage", 1},
        {"defindex", 1},
        {"defindex", 1},
        {"weptype", 1},
        {"winner", 1},
        {"player_count", 1},
        {"entityid", 1},
        {"player", 1},
        {"quality", 1},
        {"method", 1},
        {"itemdef", 1},
        {"itemid", 1},
        {"vote_parameter", 1},
        {"botid", 1},
        {"toteam", 1},
        {"priority", 1},
        {"drone_dispatched", 1},
        {"delivered", 1},
        {"cargo", 1},
        {"attacker", 1},
        {"dominated", 1},
        {"revenge", 1},
        {"usepenetratedrid", 1},
        {"x", 1},
        {"y", 1},
        {"z", 1},

        // float
        {"damage", 2},
        {"distance", 2},

        // bool
        {"canbuy", 3},
        {"isplanted", 3},
        {"hasbomb", 3},
        {"haskit", 3},
        {"silenced", 3},
        {"inrestart", 3},
        {"success", 3},
        {"assistedflash", 3},
        {"noscope", 3},
        {"thrusmoke", 3},
        {"urgent", 3},
        {"headshot", 3},

        // string
        {"item", 4},
        {"message", 4},
        {"type", 4},
        {"weapon", 4},
        {"weapon_itemid", 4},
    }};

    if (!mutex.initialized) {
        return;
    }

    EnterCriticalSection(&mutex.critical_section);
    try {
        for (auto &it : handler.events()) {
            if (!it.is_game_event) {
                continue;
            }

            if (strcmp(game_event->get_name(), it.name.c_str()) == 0) {
                lua_rawgeti(it.l, LUA_REGISTRYINDEX, it.ref[1]);
                
                // create new table for event data
                it.ref[2] = luabridge::newTable(it.l);

                for (const auto &[key_name, type] : keys) {
                    if (game_event->is_empty(key_name)) {
                        // key does not exist for the event
                        continue;
                    }

                    switch (type) {
                        // integer, byte
                        case 1: it.ref[key_name] = game_event->get_int(key_name); break;
                        // float
                        case 2: it.ref[key_name] = game_event->get_float(key_name); break;
                        // bool
                        case 3: it.ref[key_name] = game_event->get_bool(key_name); break;
                        // string
                        case 4: it.ref[key_name] = game_event->get_string(key_name); break;
                        default: break;
                    }
                }

                // push to stack
                it.ref.push();

                // call the lua function
                /*luabridge::LuaException::*/ lua_pcall(it.l, 1, 0, 0);
            }
        }
    }
    catch (luabridge::LuaException &ex) {
        logging::error(ex.what());
    }
    LeaveCriticalSection(&mutex.critical_section);
}

void lua::callbacks::run_command(c_user_cmd *cmd) {
    if (!mutex.initialized) {
        return;
    }

    EnterCriticalSection(&mutex.critical_section);
    try {
        for (auto &it : handler.events("run_command")) {
            // push reference to lua func back onto the stack
            lua_rawgeti(it.l, LUA_REGISTRYINDEX, it.ref[1]);
            
            // create new table for user_cmd data
            it.ref[2] = luabridge::newTable(it.l);
            it.ref["command_number"] = cmd->command_number;

            // push to stack
            it.ref.push();

            // call the lua function
            /*luabridge::LuaException::*/lua_pcall(it.l, 1, 0, 0);
        }
    }
    catch (luabridge::LuaException &ex) {
        logging::error("{}", ex.what());
    }
    LeaveCriticalSection(&mutex.critical_section);
}

void lua::callbacks::override_view(view_setup_t *view_setup) {
    if (!mutex.initialized) {
        return;
    }

    EnterCriticalSection(&mutex.critical_section);
    try {

        for (auto &it : handler.events("override_view")) {
            // push reference to lua func back onto the stack
            lua_rawgeti(it.l, LUA_REGISTRYINDEX, it.ref[1]);

            // create new table for user_cmd data
            it.ref[2] = luabridge::newTable(it.l);
            it.ref["x"] = view_setup->x;
            it.ref["y"] = view_setup->y;
            // TODO: missing z
            it.ref["fov"] = view_setup->fov;

            // push to stack
            it.ref.push();

            // call the lua function
            /*luabridge::LuaException::*/lua_pcall(it.l, 1, 0, 0);

            view_setup->x = it.ref["x"];
            view_setup->y = it.ref["y"];
            view_setup->fov = it.ref["fov"];
        }
    }
    catch (luabridge::LuaException &ex) {
        logging::error("{}", ex.what());
    }
    LeaveCriticalSection(&mutex.critical_section);
}

void lua::callbacks::draw() {
    if (!mutex.initialized) {
        return;
    }

    EnterCriticalSection(&mutex.critical_section);
    try {
        for (auto &it : handler.events("draw")) {
            // push reference to lua func back onto the stack
            lua_rawgeti(it.l, LUA_REGISTRYINDEX, it.ref[1]);

            // call the lua function
            /*luabridge::LuaException::*/lua_pcall(it.l, 0, 0, 0);
        }
    }
    catch (luabridge::LuaException &ex) {
        logging::error("{}", ex.what());
    }
    LeaveCriticalSection(&mutex.critical_section);
}
