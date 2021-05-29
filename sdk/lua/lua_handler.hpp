#pragma once

#include "lua_internal.hpp"

#include "lua_context.hpp"
#include "lua_script.hpp"

namespace lua_internal {
	class handler {
	private:
		std::vector<std::unique_ptr<script>> _scripts;

	public:
		void add_script(const std::string& path) {
			_scripts.push_back(std::make_unique<script>(path));
		}

		void add_script(const std::vector<std::string>& paths) {
			for (auto& it : paths) {
				_scripts.push_back(std::make_unique<script>(it));
			}
		}

		std::vector<context> loaded();
        void unload(const std::string &name);
        void unload();
        std::vector<lua_internal::callback> events(const std::string &name);
        std::vector<callback> events();
    };
}
