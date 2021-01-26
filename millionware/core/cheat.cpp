#include <windows.h>

#include "../utils/xorstr/xorstr.hpp"
#include "cheat.hpp"

struct cheat_info_t {
	uint64_t inject_time;
	uint64_t sub_expire;

	char username[32];
};

__declspec(dllexport) cheat_info_t g_cheat_info;

void cheat::initialize() {
#ifdef _DEBUG

	const auto username = STR_ENC("ur mom");

	g_cheat_info.inject_time = 0;
	g_cheat_info.sub_expire = std::numeric_limits<uint64_t>::max();

	memset(g_cheat_info.username, 0, sizeof g_cheat_info.username);
	memcpy(g_cheat_info.username, username, strlen(username));

#endif
}

uint64_t cheat::get_inject_time() {
	return g_cheat_info.inject_time;
}

uint64_t cheat::get_sub_expire() {
	return g_cheat_info.sub_expire;
}

std::string_view cheat::get_username() {
	return g_cheat_info.username;
}