#include "lua_game.hpp"

#include "../engine/logging/logging.h"

bool lua::init() {
    // create the main cheat directory and subdirectories for scripts and other stuff
    // i dont think you can use `std::filesystem::create_directories` to create nested directories
    // so we have to do it manually like this, fix this if you know a better way AND FUCKING TEST IT PLEASE
    if (!std::filesystem::exists(xs(".\\mw"))) {
        std::filesystem::create_directories(xs(".\\mw"));
        std::filesystem::create_directories(xs(".\\mw\\scripts"));
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
        for (auto &it : handler.events(xs("startup"))) {
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
        {xs("userid"), 1},
        {xs("health"), 1},
        {xs("armor"), 1},
        {xs("dmg_health"), 1},
        {xs("dmg_armor"), 1},
        {xs("userid"), 1},
        {xs("userid"), 1},
        {xs("userid"), 1},
        {xs("hitgroup"), 1},
        {xs("team"), 1},
        {xs("loadout"), 1},
        {xs("site"), 1},
        {xs("entindex"), 1},
        {xs("hostage"), 1},
        {xs("defindex"), 1},
        {xs("defindex"), 1},
        {xs("weptype"), 1},
        {xs("winner"), 1},
        {xs("player_count"), 1},
        {xs("entityid"), 1},
        {xs("player"), 1},
        {xs("quality"), 1},
        {xs("method"), 1},
        {xs("itemdef"), 1},
        {xs("itemid"), 1},
        {xs("vote_parameter"), 1},
        {xs("botid"), 1},
        {xs("toteam"), 1},
        {xs("priority"), 1},
        {xs("drone_dispatched"), 1},
        {xs("delivered"), 1},
        {xs("cargo"), 1},
        {xs("attacker"), 1},
        {xs("dominated"), 1},
        {xs("revenge"), 1},
        {xs("usepenetratedrid"), 1},
        {xs("x"), 1},
        {xs("y"), 1},
        {xs("z"), 1},

        // float
        {xs("damage"), 2},
        {xs("distance"), 2},

        // bool
        {xs("canbuy"), 3},
        {xs("isplanted"), 3},
        {xs("hasbomb"), 3},
        {xs("haskit"), 3},
        {xs("silenced"), 3},
        {xs("inrestart"), 3},
        {xs("success"), 3},
        {xs("assistedflash"), 3},
        {xs("noscope"), 3},
        {xs("thrusmoke"), 3},
        {xs("urgent"), 3},
        {xs("headshot"), 3},

        // string
        {xs("item"), 4},
        {xs("message"), 4},
        {xs("type"), 4},
        {xs("weapon"), 4},
        {xs("weapon_itemid"), 4},
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
        for (auto &it : handler.events(xs("run_command"))) {
            // push reference to lua func back onto the stack
            lua_rawgeti(it.l, LUA_REGISTRYINDEX, it.ref[1]);
            
            // create new table for user_cmd data
            it.ref[2] = luabridge::newTable(it.l);
            it.ref[xs("command_number")] = cmd->command_number;

            // push to stack
            it.ref.push();

            // call the lua function
            /*luabridge::LuaException::*/lua_pcall(it.l, 1, 0, 0);
        }
    }
    catch (luabridge::LuaException &ex) {
        logging::error(xs("{}"), ex.what());
    }
    LeaveCriticalSection(&mutex.critical_section);
}

void lua::callbacks::override_view(view_setup_t *view_setup) {
    if (!mutex.initialized) {
        return;
    }

    EnterCriticalSection(&mutex.critical_section);
    try {

        for (auto &it : handler.events(xs("override_view"))) {
            // push reference to lua func back onto the stack
            lua_rawgeti(it.l, LUA_REGISTRYINDEX, it.ref[1]);

            // create new table for user_cmd data
            it.ref[2] = luabridge::newTable(it.l);
            it.ref[xs("x")] = view_setup->x;
            it.ref[xs("y")] = view_setup->y;
            // TODO: missing z
            it.ref[xs("fov")] = view_setup->fov;

            // push to stack
            it.ref.push();

            // call the lua function
            /*luabridge::LuaException::*/lua_pcall(it.l, 1, 0, 0);

            view_setup->x = it.ref[xs("x")];
            view_setup->y = it.ref[xs("y")];
            view_setup->fov = it.ref[xs("fov")];
        }
    }
    catch (luabridge::LuaException &ex) {
        logging::error(xs("{}"), ex.what());
    }
    LeaveCriticalSection(&mutex.critical_section);
}

void lua::callbacks::draw() {
    if (!mutex.initialized) {
        return;
    }

    EnterCriticalSection(&mutex.critical_section);
    try {
        for (auto &it : handler.events(xs("draw"))) {
            // push reference to lua func back onto the stack
            lua_rawgeti(it.l, LUA_REGISTRYINDEX, it.ref[1]);

            // call the lua function
            /*luabridge::LuaException::*/lua_pcall(it.l, 0, 0, 0);
        }
    }
    catch (luabridge::LuaException &ex) {
        logging::error(xs("{}"), ex.what());
    }
    LeaveCriticalSection(&mutex.critical_section);
}
