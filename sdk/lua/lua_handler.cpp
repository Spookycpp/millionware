#include "lua_handler.hpp"
#include "lua_context.hpp"

#include "../engine/logging/logging.h"

void lua_internal::handler::add_script(const std::string &path) {
    scripts.push_back(std::make_unique<script>(path));
}

void lua_internal::handler::add_script(const std::vector<std::string> &paths) {
    for (auto &it : paths) {
        scripts.push_back(std::make_unique<script>(it));
    }
}

auto lua_internal::handler::loaded() -> std::vector<lua_internal::context> {
	std::vector<context> ret;

	for (auto &it : scripts) {
        if (it->ctx.exiting) {
            continue;
        }

		if (!it->loaded) {
            unload(it->filename());
			continue;
		}

		ret.push_back(it->ctx);
	}

	return ret;
}

void lua_internal::handler::unload(const std::string &name) {
	std::erase_if(scripts, [name](std::unique_ptr<script> &it) {
        if (it->filename() != name) {
            return false;
        }

        if (it->loaded) {
            it->ctx.exit();
        }

        return true;
    });
}

void lua_internal::handler::unload() {
    for (auto &it : scripts) {
        if (it->loaded) {
            it->ctx.exit();
        }
    }

	scripts.clear();
}

std::vector<lua_internal::callback> lua_internal::handler::events() {
	std::vector<callback> ret;

	for (auto& it : loaded()) {
        if (it.exiting) {
            continue;
        }

		for (auto& cb : it.callbacks) {
			ret.push_back(cb);
		}
	}

	return ret;
}

std::vector<lua_internal::callback> lua_internal::handler::events(const std::string &name) {
    std::vector<callback> ret;

    /*for (auto &it : loaded()) {
        if (it.exiting) {
            continue;
        }

        for (auto &cb : it.callbacks) {
            if (!cb.ref) {
                continue;
            }

            if (cb.name == name) {
                ret.push_back(cb);
            }
        }
    }*/

    return ret;
}

std::vector<lua_internal::callback> lua_internal::handler::events(const uint32_t hash, const std::string &name) {
    std::vector<callback> ret;

    /*for (auto &it : loaded()) {
        if (it.exiting) {
            continue;
        }

        for (auto &cb : it.callbacks) {
            if (!cb.ref) {
                continue;
            }

            if (cb.hash == hash || cb.name == name) {
                ret.push_back(cb);
            }
        }
    }*/

    return ret;
}