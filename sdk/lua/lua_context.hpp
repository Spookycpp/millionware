#pragma once

#include "lua_internal.hpp"
#include "lua_callback.hpp"

namespace lua_internal {
    class context {
    private:
        void new_state();
        void setup_tables();

        // tables
        void lua_client();
        void lua_draw();
        void entity();

    public:
        lua_State *l = nullptr;

        std::vector<callback> callbacks;
        std::string script_path;

        bool exiting = false;

    public:
        bool load(const std::string &path);
        void ffi(bool state = true) const;

        bool run() const;
        void exit();

        std::string filename() const;
    };
}
