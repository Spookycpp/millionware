#pragma once

#include <cstdint>

struct player_info_t
{
  uint64_t version;
  uint64_t xuid;
  char name[128];
  int user_id;
  char guid[33];
  int friends_id;
  char friends_name[128];
  bool is_bot;
  bool is_hltv_proxy;

private:
  char padding_[25];
};
