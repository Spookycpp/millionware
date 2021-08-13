#pragma once

#include <cstring>

#include "macros.h"

class c_ui_panel {
  public:
    DECLARE_VFUNC(9, get_panel_id(), const char *(__thiscall *) (void *) )();
    DECLARE_VFUNC(25, get_parent(), c_ui_panel *(__thiscall *) (void *) )();
    DECLARE_VFUNC(27, set_visible(bool visible), c_ui_panel *(__thiscall *) (void *, bool) )(visible);
};

class c_ui_engine {
  public:
    DECLARE_VFUNC(36, is_valid_panel_ptr(c_ui_panel *panel), bool(__thiscall *)(void *, c_ui_panel *))(panel);
    DECLARE_VFUNC(56, get_last_target_panel(), c_ui_panel *(__thiscall *) (void *) )();
    DECLARE_VFUNC(113, run_script(c_ui_panel *root_panel, const char *js_code, const char *layout_file, int a1, int a2, bool a3, bool a4),
                  void(__thiscall *)(void *, c_ui_panel *, const char *, const char *, int, int, bool, bool))
    (root_panel, js_code, layout_file, a1, a2, a3, a4);
};

class c_panorama_engine {
  public:
    DECLARE_VFUNC(11, access_ui_engine(), c_ui_engine *(__thiscall *) (void *) )();
};
