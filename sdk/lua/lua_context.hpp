#pragma once

#include "lua_internal.hpp"
#include "lua_callback.hpp"

namespace lua_internal {
    class context {
    private:
        std::vector<callback> _callbacks;
        std::string _path;

        bool _exiting = false;

        void new_state();
        void setup_tables();

        // namespace initializer
        void lua_client();
        void lua_draw();
        void entity();
        

    public:
        lua_State *l = nullptr;

        bool load(const std::string &path);
        void ffi(bool state = true) const;

        [[nodiscard]] bool run() const;
        void exit();
        [[nodiscard]] bool exiting() const;

        std::vector<callback> callbacks() const;
        void add_callback(const callback &cb);
        std::string path() const;
        std::string filename() const;
    };
}
