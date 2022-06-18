#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <windows.h>

using unique_handle = typename std::unique_ptr<void, decltype(&CloseHandle)>;

template <typename T>
inline void check_return(const T &return_value, const std::string &error_message) {
    if (return_value)
        return;

    MessageBoxA(nullptr, error_message.data(), "millionware", MB_OK);

    std::exit(EXIT_FAILURE);
}

inline bool enable_privilege(const std::string &privilege_name) {
    HANDLE token_handle = nullptr;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token_handle))
        return false;

    LUID luid{};

    if (!LookupPrivilegeValueA(nullptr, privilege_name.data(), &luid))
        return false;

    TOKEN_PRIVILEGES token_state{};

    token_state.PrivilegeCount = 1;
    token_state.Privileges[0].Luid = luid;
    token_state.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(token_handle, false, &token_state, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
        return false;

    CloseHandle(token_handle);

    return true;
}

template <typename F>
inline auto wait_for_object(const F &function, std::chrono::steady_clock::duration interval = std::chrono::milliseconds(250),
                            std::chrono::steady_clock::duration time_out = std::chrono::seconds(60)) -> decltype(function()) {
    const auto start_time = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - start_time < time_out) {
        if (const auto result = function(); result)
            return result;

        std::this_thread::sleep_for(interval);
    }

    return {};
}
