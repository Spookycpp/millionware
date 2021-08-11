#pragma once

#include <format>
#include "../../source engine/color.h"

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

struct valve_color_t {
    int r : 8;
    int g : 8;
    int b : 8;
    int a : 8;
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

    void console(valve_color_t color, const char *message);
    void print(const color_t color, const std::string &tag, const std::string &message);

    IMPL_LOGGING_LEVEL(debug, LOG_SEVERITY_DEBUG);
    IMPL_LOGGING_LEVEL(info, LOG_SEVERITY_INFO);
    IMPL_LOGGING_LEVEL(warning, LOG_SEVERITY_WARNING);
    IMPL_LOGGING_LEVEL(error, LOG_SEVERITY_ERROR);

} // namespace logging
