#include "../../core/interfaces/interfaces.h"
#include "../../engine/hash/hash.h"
#include "../../engine/pe/pe.h"
#include "../../engine/security/xorstr.h"
#include "logging.h"
#include "../../core/settings/settings.h"

struct valve_color_t
{
	unsigned char r, g, b, a;
};

using con_color_msg_fn = void(__cdecl *)(const valve_color_t &, const char *, ...);

static std::vector<std::pair<int, std::string>> log_buffer = {};
static con_color_msg_fn con_color_msg = nullptr;
static int min_severity = SEVERITY_INFO;

void logging::init(int severity)
{
	min_severity = SEVERITY_ERROR + 1;
	con_color_msg = (con_color_msg_fn) pe::get_export(XORSTR("tier0.dll"), XORSTR("?ConColorMsg@@YAXABVColor@@PBDZZ"));
	min_severity = severity;
}

bool logging::should_log(int severity)
{
	return severity >= min_severity;
}

void logging::print(int severity, const std::string &message)
{
	if (con_color_msg == nullptr)
	{
		const auto prev_severity = min_severity;

		min_severity = SEVERITY_ERROR + 1;
		con_color_msg = (con_color_msg_fn) pe::get_export(XORSTR("tier0.dll"), XORSTR("?ConColorMsg@@YAXABVColor@@PBDZZ"));
		min_severity = prev_severity;

		if (con_color_msg == nullptr)
		{
			log_buffer.push_back(std::make_pair(severity, message));

			return;
		}
	}

	if (!log_buffer.empty())
	{
		const auto log_buffer_copy = log_buffer;

		log_buffer.clear();

		for (const auto &[buffer_severity, buffer_message] : log_buffer_copy)
		{
			print(buffer_severity, buffer_message);
		}
	}

	const auto accent = settings.global.accent_color;

	switch (severity)
	{
	case SEVERITY_TRACE:
		con_color_msg({ 184, 184, 184, 255 }, XORSTR("[millionware] "));
		break;

	case SEVERITY_DEBUG:
		con_color_msg({ 82, 235, 224, 255 }, XORSTR("[millionware] "));
		break;

	case SEVERITY_INFO:
        con_color_msg({(uint8_t) accent.r, (uint8_t) accent.g, (uint8_t) accent.b, 255}, XORSTR("[millionware] "));
        break;

	case SEVERITY_WARNING:
		con_color_msg({ 255, 213, 28, 255 }, XORSTR("[millionware] "));
		break;

	case SEVERITY_ERROR:
		con_color_msg({ 255, 32, 28, 255 }, XORSTR("[millionware] "));
		break;
	}

	con_color_msg({ 255, 255, 255, 255 }, XORSTR("%s \n"), message.data());
}
