#pragma once

#include <format>

#ifdef _DEBUG
#define MIN_SEVERITY LOG_SEVERITY_DEBUG
#else
#define MIN_SEVERITY LOG_SEVERITY_INFO
#endif

enum e_log_severity {
    LOG_SEVERITY_DEBUG,
    LOG_SEVERITY_INFO,
    LOG_SEVERITY_WARNING,
    LOG_SEVERITY_ERROR,
};

#define IMPL_LOGGING_LEVEL(level_name, level_severity)                                                                                                                                                 \
    template <typename... A> void level_name(const std::string &format_str, A &&...args) {                                                                                                             \
        if constexpr (level_severity >= MIN_SEVERITY)                                                                                                                                                            \
            print(level_severity, std::format(format_str, std::forward<A>(args)...));                                                                                                                  \
    }

namespace logging {

    void init();
    void render();

    void print(int severity, const std::string &message);

    IMPL_LOGGING_LEVEL(debug, LOG_SEVERITY_DEBUG);
    IMPL_LOGGING_LEVEL(info, LOG_SEVERITY_INFO);
    IMPL_LOGGING_LEVEL(warning, LOG_SEVERITY_WARNING);
    IMPL_LOGGING_LEVEL(error, LOG_SEVERITY_ERROR);

} // namespace logging
