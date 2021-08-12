#pragma once

#include "lua_internal.hpp"

#include "lua_context.hpp"

namespace lua_internal {
	class handler {
	private:
        class script {
          public:
            context ctx;

            std::string _path;
            bool loaded;

          public:
            script(const std::string &script_path) : loaded(false) {
                _path = script_path;
                loaded = ctx.load(_path);
            }

            std::string filename() const {
                return std::filesystem::path(_path).stem().string();
            }
        };

	private:
		std::vector<std::unique_ptr<script>> scripts;

	public:
        void add_script(const std::string &path);
        void add_script(const std::vector<std::string> &paths);

		std::vector<context> loaded();
        void unload(const std::string &name);
        void unload();

        std::vector<callback> events(const std::string &name);
        std::vector<lua_internal::callback> events(uint32_t hash, const std::string &name = "");
        std::vector<callback> events();
    };
}
