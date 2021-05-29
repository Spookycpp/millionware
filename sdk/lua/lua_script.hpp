#pragma once

#include "lua_internal.hpp"

#include "lua_context.hpp"

namespace lua_internal {
	class script {
	private:
		context _ctx;

		std::string _path;
		bool _loaded;
		std::string _lua_string;
	
	public:
		script(const std::string& path);

		context ctx() const;
		std::string path() const;
        std::string filename() const;
        bool loaded() const;
		std::string lua_string() const;
	};
}