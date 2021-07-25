#include <imgui.h>

#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"
#include "../../engine/hash/hash.h"
#include "../../engine/pe/pe.h"
#include "../../engine/render/render.h"
#include "../../engine/security/xorstr.h"
#include "logging.h"

constexpr auto LOG_MAX_BACKLOG = 32;
constexpr auto FADE_IN_OUT_GRACE_PERIOD = 0.5f;
constexpr auto DEFAULT_ENTRY_DURATION = 5.0f;

struct valve_color_t {
    int r : 8;
    int g : 8;
    int b : 8;
    int a : 8;
};

struct log_entry_t {
    int severity;

    float time_added;
    float duration;

    std::string message;
};

using con_color_msg_fn = void(__cdecl *)(const valve_color_t &, const char *, ...);

static std::vector<log_entry_t> messages;
static con_color_msg_fn con_color_msg;

void logging::init() {
    ImGui::CreateContext();

    con_color_msg = (con_color_msg_fn) pe::get_export(xs("tier0.dll"), xs("?ConColorMsg@@YAXABVColor@@PBDZZ"));
}

void logging::render() {
    const auto current_time = (float) ImGui::GetTime();
    const auto messages_to_remove = std::remove_if(messages.begin(), messages.end(), [current_time](const auto &message) {
        const auto displayed_for = current_time - message.time_added;

        return displayed_for >= message.duration;
    });

    messages.erase(messages_to_remove, messages.end());

    auto offset = 0.0f;

    for (const auto &message : messages) {
        const auto displayed_for = current_time - message.time_added;
        const auto text_size = render::measure_text(message.message.c_str(), FONT_TAHOMA_11);
        const auto message_increment = text_size.y + 1.0f;

        color_t color;
        point_t position = {4.0f, 4.0f + offset};

        if (message.severity == LOG_SEVERITY_DEBUG)
            color = {82, 235, 224};
        else if (message.severity == LOG_SEVERITY_INFO)
            color = settings.global.accent_color;
        else if (message.severity == LOG_SEVERITY_WARNING)
            color = {255, 213, 28};
        else if (message.severity == LOG_SEVERITY_ERROR)
            color = {255, 32, 28};

        if (displayed_for <= FADE_IN_OUT_GRACE_PERIOD || message.duration - displayed_for <= FADE_IN_OUT_GRACE_PERIOD) {
            const auto what1 = displayed_for <= FADE_IN_OUT_GRACE_PERIOD;
            const auto what2 = what1 ? displayed_for : message.duration - displayed_for;
            const auto what3 = std::clamp(what2, 0.0f, FADE_IN_OUT_GRACE_PERIOD) / FADE_IN_OUT_GRACE_PERIOD;

            color.a = std::clamp((int) (what3 * 255.0f), 0, 255);
            offset += message_increment * what3;

            if (what1)
                position.x -= (1.0f - what3) * 150.0f;
        }
        else {
            color.a = 255;
            offset += message_increment;
        }

        render::draw_text(position + 1, {10, 10, 10, 80}, message.message.c_str(), FONT_TAHOMA_11);
        render::draw_text(position, color, message.message.c_str(), FONT_TAHOMA_11);
    }
}

void logging::print(int severity, const std::string &message) {
    log_entry_t entry;

    entry.severity = severity;
    entry.time_added = ImGui::GetTime();
    entry.duration = DEFAULT_ENTRY_DURATION;
    entry.message = move(message);

    if (messages.size() >= LOG_MAX_BACKLOG)
        messages.erase(messages.begin());

    messages.push_back(entry);

    if (con_color_msg) {
        const auto accent = settings.global.accent_color;

        valve_color_t color;

        if (severity == LOG_SEVERITY_DEBUG)
            color = {82, 235, 224, 255};
        else if (severity == LOG_SEVERITY_INFO)
            color = {accent.r, accent.g, accent.b, 255};
        else if (severity == LOG_SEVERITY_WARNING)
            color = {255, 213, 28, 255};
        else if (severity == LOG_SEVERITY_ERROR)
            color = {255, 32, 28, 255};

        con_color_msg(color, xs("[millionware] "));
        con_color_msg({255, 255, 255, 255}, entry.message.c_str());
        con_color_msg({255, 255, 255, 255}, xs("\n"));
    }
}
