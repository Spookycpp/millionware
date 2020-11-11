#pragma once

#include "macros.hpp"
#include "network_channel.hpp"
#include "player_info.hpp"
#include "vector.hpp"

class c_engine_client
{
public:
  VIRTUAL_METHOD(void, get_screen_size, 5, (int& width, int& height), width, height);
  VIRTUAL_METHOD(void, server_command, 6, (const char* command, bool send_reliably = true), command, send_reliably);
  VIRTUAL_METHOD(void, client_command, 7, (const char* command), command);
  VIRTUAL_METHOD(bool, get_player_info, 8, (int index, player_info_t& buffer), index, buffer);
  VIRTUAL_METHOD(int, get_index_from_user_id, 9, (int user_id), user_id);
  VIRTUAL_METHOD(int, get_local_player, 12, ());
  VIRTUAL_METHOD(void, get_view_angles, 18, (vector3_t& view_angles), view_angles);
  VIRTUAL_METHOD(void, set_view_angles, 19, (const vector3_t& view_angles), view_angles);
  VIRTUAL_METHOD(bool, is_in_game, 26, ());
  VIRTUAL_METHOD(bool, is_connected, 27, ());
  VIRTUAL_METHOD(const char*, get_level_name, 52, ());
  VIRTUAL_METHOD(const char*, get_level_name_short, 53, ());
  VIRTUAL_METHOD(const char*, get_map_ground_name, 54, ());
  VIRTUAL_METHOD(c_net_channel_info*, get_net_channel_info, 78, ());
  VIRTUAL_METHOD(bool, is_playing_demo, 82, ());
  VIRTUAL_METHOD(bool, is_recording_demo, 83, ());
  VIRTUAL_METHOD(bool, is_taking_screenshot, 92, ());
};
