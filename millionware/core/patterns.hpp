#pragma once

#include <cstdint>

namespace patterns
{
  inline uintptr_t client_state = 0;
  inline uintptr_t d3d9_present = 0;
  inline uintptr_t d3d9_reset = 0;
  inline uintptr_t hud_system = 0;
  inline uintptr_t render_beams = 0;
  inline uintptr_t weapon_system = 0;

  void initialize();
}
