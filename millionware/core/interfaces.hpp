#pragma once

#include <cstdint>

namespace interfaces
{
  inline uintptr_t client = 0;
  inline uintptr_t entity_list = 0;
  inline uintptr_t engine_client = 0;
  inline uintptr_t input_system = 0;
  inline uintptr_t game_movement = 0;
  inline uintptr_t event_manager = 0;

  void initialize();
}
