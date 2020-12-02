#pragma once

#include <cstdint>

namespace patterns {

  inline uintptr_t engine_vgui_start_drawing = 0;
  inline uintptr_t engine_vgui_finish_drawing = 0;
  inline uintptr_t player_has_bomb = 0;
  inline uintptr_t set_local_player_ready = 0;
  inline uintptr_t set_clantag = 0;

  void initialize();

}
