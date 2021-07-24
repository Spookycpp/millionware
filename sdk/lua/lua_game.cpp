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
    static std::array<std::pair<const char *, int>, 58> keys = {{
        // integer, byte
        {XORSTR("userid"), 1},
        {XORSTR("health"), 1},
        {XORSTR("armor"), 1},
        {XORSTR("dmg_health"), 1},
        {XORSTR("dmg_armor"), 1},
        {XORSTR("userid"), 1},
        {XORSTR("userid"), 1},
        {XORSTR("userid"), 1},
        {XORSTR("hitgroup"), 1},
        {XORSTR("team"), 1},
        {XORSTR("loadout"), 1},
        {XORSTR("site"), 1},
        {XORSTR("entindex"), 1},
        {XORSTR("hostage"), 1},
        {XORSTR("defindex"), 1},
        {XORSTR("defindex"), 1},
        {XORSTR("weptype"), 1},
        {XORSTR("winner"), 1},
        {XORSTR("player_count"), 1},
        {XORSTR("entityid"), 1},
        {XORSTR("player"), 1},
        {XORSTR("quality"), 1},
        {XORSTR("method"), 1},
        {XORSTR("itemdef"), 1},
        {XORSTR("itemid"), 1},
        {XORSTR("vote_parameter"), 1},
        {XORSTR("botid"), 1},
        {XORSTR("toteam"), 1},
        {XORSTR("priority"), 1},
        {XORSTR("drone_dispatched"), 1},
        {XORSTR("delivered"), 1},
        {XORSTR("cargo"), 1},
        {XORSTR("attacker"), 1},
        {XORSTR("dominated"), 1},
        {XORSTR("revenge"), 1},
        {XORSTR("usepenetratedrid"), 1},
        {XORSTR("x"), 1},
        {XORSTR("y"), 1},
        {XORSTR("z"), 1},

        // float
        {XORSTR("damage"), 2},
        {XORSTR("distance"), 2},

        // bool
        {XORSTR("canbuy"), 3},
        {XORSTR("isplanted"), 3},
        {XORSTR("hasbomb"), 3},
        {XORSTR("haskit"), 3},
        {XORSTR("silenced"), 3},
        {XORSTR("inrestart"), 3},
        {XORSTR("success"), 3},
        {XORSTR("assistedflash"), 3},
        {XORSTR("noscope"), 3},
        {XORSTR("thrusmoke"), 3},
        {XORSTR("urgent"), 3},
        {XORSTR("headshot"), 3},

        // string
        {XORSTR("item"), 4},
        {XORSTR("message"), 4},
        {XORSTR("type"), 4},
        {XORSTR("weapon"), 4},
        {XORSTR("weapon_itemid"), 4},
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
        for (auto &it : handler.events(XORSTR("run_command"))) {
            // push reference to lua func back onto the stack
            lua_rawgeti(it.l, LUA_REGISTRYINDEX, it.ref[1]);
            
            // create new table for user_cmd data
            it.ref[2] = luabridge::newTable(it.l);
            it.ref[XORSTR("command_number")] = cmd->command_number;

            // push to stack
            it.ref.push();

            // call the lua function
            /*luabridge::LuaException::*/lua_pcall(it.l, 1, 0, 0);
        }
    }
    catch (luabridge::LuaException &ex) {
        logging::error(XORSTR("{}"), ex.what());
    }
    LeaveCriticalSection(&mutex.critical_section);
}

void lua::callbacks::override_view(view_setup_t *view_setup) {
    if (!mutex.initialized) {
        return;
    }

    EnterCriticalSection(&mutex.critical_section);
    try {

        for (auto &it : handler.events(XORSTR("override_view"))) {
            // push reference to lua func back onto the stack
            lua_rawgeti(it.l, LUA_REGISTRYINDEX, it.ref[1]);

            // create new table for user_cmd data
            it.ref[2] = luabridge::newTable(it.l);
            it.ref[XORSTR("x")] = view_setup->x;
            it.ref[XORSTR("y")] = view_setup->y;
            // TODO: missing z
            it.ref[XORSTR("fov")] = view_setup->fov;

            // push to stack
            it.ref.push();

            // call the lua function
            /*luabridge::LuaException::*/lua_pcall(it.l, 1, 0, 0);

            view_setup->x = it.ref[XORSTR("x")];
            view_setup->y = it.ref[XORSTR("y")];
            view_setup->fov = it.ref[XORSTR("fov")];
        }
    }
    catch (luabridge::LuaException &ex) {
        logging::error(XORSTR("{}"), ex.what());
    }
    LeaveCriticalSection(&mutex.critical_section);
}

void lua::callbacks::draw() {
    if (!mutex.initialized) {
        return;
    }

    EnterCriticalSection(&mutex.critical_section);
    try {
        for (auto &it : handler.events(XORSTR("draw"))) {
            // push reference to lua func back onto the stack
            lua_rawgeti(it.l, LUA_REGISTRYINDEX, it.ref[1]);

            // call the lua function
            /*luabridge::LuaException::*/lua_pcall(it.l, 0, 0, 0);
        }
    }
    catch (luabridge::LuaException &ex) {
        logging::error(XORSTR("{}"), ex.what());
    }
    LeaveCriticalSection(&mutex.critical_section);
}
