#include <Windows.h>

#include "cheat.hpp"

struct cheat_info_t {
	uint64_t inject_time;
	uint64_t sub_expire;

	wchar_t username[32];
	char access_token[32];
};

__declspec(dllexport) cheat_info_t g_cheat_info;

void cheat::initialize() {
#ifdef _DEBUG

	g_cheat_info.inject_time = 0;
	g_cheat_info.sub_expire = std::numeric_limits<uint64_t>::max();

	memset(g_cheat_info.username, 0, sizeof g_cheat_info.username);
	memset(g_cheat_info.access_token, 0, sizeof g_cheat_info.access_token);

	unsigned long username_length = sizeof g_cheat_info.username / sizeof(wchar_t);

	GetUserNameW(g_cheat_info.username, &username_length);

#endif
}

system_clock_time_point cheat::get_inject_time() {
	const auto epoch = system_clock_time_point();
	const auto since_epoch = std::chrono::seconds(g_cheat_info.inject_time);

	return epoch + since_epoch;
}

system_clock_time_point cheat::get_sub_expire() {
	const auto epoch = system_clock_time_point();
	const auto since_epoch = std::chrono::seconds(g_cheat_info.sub_expire);

	return epoch + since_epoch;
}

std::wstring_view cheat::get_username() {
	return g_cheat_info.username;
}

std::string_view cheat::get_access_token() {
	return g_cheat_info.access_token;
}
