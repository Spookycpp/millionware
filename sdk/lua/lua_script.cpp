#include "lua_script.hpp"

lua_internal::script::script(const std::string& path) : _loaded(false) {
	_loaded = _ctx.load(path);
    _path = path;
}

lua_internal::context lua_internal::script::ctx() const {
	return _ctx;
}

std::string lua_internal::script::path() const {
	return _path;
}

std::string lua_internal::script::filename() const {
    return std::filesystem::path(_path).stem().string();
}

bool lua_internal::script::loaded() const {
	return _loaded;
}

std::string lua_internal::script::lua_string() const {
	return _lua_string;
}
