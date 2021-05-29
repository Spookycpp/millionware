#include "lua_handler.hpp"
#include "lua_context.hpp"

#include "../engine/logging/logging.h"

auto lua_internal::handler::loaded() -> std::vector<lua_internal::context> {
	std::vector<context> ret;

	for (auto &it : _scripts) {
        if (it->ctx().exiting()) {
            continue;
        }

		if (!it->loaded()) {
            unload(it->filename());
			continue;
		}

		ret.push_back(it->ctx());
	}

	return ret;
}

void lua_internal::handler::unload(const std::string &name) {
	std::erase_if(_scripts, [name](std::unique_ptr<script> &it) {
        if (it->filename() != name) {
            return false;
        }

        if (it->loaded()) {
            it->ctx().exit();
        }

        return true;
    });
}

void lua_internal::handler::unload() {
    for (auto &it : _scripts) {
        if (it->loaded()) {
            it->ctx().exit();
        }
    }

	_scripts.clear();
}

auto lua_internal::handler::events() -> std::vector<lua_internal::callback> {
	std::vector<callback> ret;

	for (auto& it : loaded()) {
        if (it.exiting()) {
            continue;
        }

		for (auto& cb : it.callbacks()) {
			ret.push_back(cb);
		}
	}

	return ret;
}

auto lua_internal::handler::events(const std::string &name) -> std::vector<lua_internal::callback> {
    std::vector<callback> ret;

    for (auto &it : loaded()) {
        if (it.exiting()) {
            continue;
        }

        for (auto &cb : it.callbacks()) {
            if (!cb.m_ref) {
                continue;
            }

            if (cb.m_name == name) {
                ret.push_back(cb);
            }
        }
    }

    return ret;
}