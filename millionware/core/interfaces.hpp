#pragma once

#include <cstdint>

#include "../sdk/client_dll.hpp"
#include "../sdk/engine_client.hpp"
#include "../sdk/engine_vgui.hpp"
#include "../sdk/entity_list.hpp"
#include "../sdk/vgui.hpp"

namespace interfaces
{
  inline c_base_client_dll* client = nullptr;
  inline c_entity_list* entity_list = nullptr;
  inline c_engine_client* engine_client = nullptr;
  inline c_engine_vgui* engine_vgui = nullptr;
  inline c_vgui_surface* vgui_surface = nullptr;
  inline uintptr_t input_system = 0;
  inline uintptr_t game_movement = 0;
  inline uintptr_t event_manager = 0;

  void initialize();
}
