#pragma once

#include <luajit/lua.hpp>
#include <LuaBridge/LuaBridge.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <utility>

#include "../engine/security/xorstr.h"

namespace lua_internal {
    inline const char *default_script_path = ("mw\\scripts\\");

    inline std::vector<std::string> get_script_paths(const std::string &path = default_script_path) {
        std::vector<std::string> scripts;

        for (const auto &it : std::filesystem::recursive_directory_iterator(path)) {
            if (it.is_directory()) {
                continue;
            }

            if (it.path().extension() == (".lua") || it.path().extension() == (".ljbc")) {
                scripts.push_back(it.path().string());
            }
        }

        std::ranges::sort(scripts);

        // erase duplicates
        scripts.erase(std::unique(scripts.begin(), scripts.end()), scripts.end());

        return scripts;
    }

    inline std::string read_file_contents(const std::string &path) {
        const std::ifstream fstream(path);

        std::stringstream ss;
        ss << fstream.rdbuf();

        return ss.str();
    }
} // namespace lua_internal