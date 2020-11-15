#pragma once

#include <cstdint>

namespace patterns
{
  inline uintptr_t global_vars_base = 0;
  inline uintptr_t engine_vgui_start_drawing = 0;
  inline uintptr_t engine_vgui_finish_drawing = 0;
  inline uintptr_t player_has_bomb = 0;

  void initialize();
}
