#include "cheat.hpp"

enum e_build_type
{
  BUILD_TYPE_STABLE,
  BUILD_TYPE_BETA,
  BUILD_TYPE_DEVELOPER,
};

struct cheat_info_t
{
  uint32_t inject_time;
  uint32_t sub_expire;

  char username[32];
  char access_token[32];
  char build_type;
};

__declspec(dllexport) cheat_info_t g_cheat_info;

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

std::string_view cheat::get_username() {
  return g_cheat_info.username;
}

std::string_view cheat::get_access_token() {
  return g_cheat_info.access_token;
}
