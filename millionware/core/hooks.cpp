#include <Windows.h>

#include <d3dx9.h>

#include "hooks.hpp"

extern long __fastcall present_hook(uintptr_t ecx, uintptr_t edx, const RECT* source_rect, const RECT* dest_rect, HWND dest_window_override, const RGNDATA* dirty_region);
extern long __fastcall reset_hook(uintptr_t ecx, uintptr_t edx, const D3DPRESENT_PARAMETERS* presentation_parameters);

void hooks::initialize() {
  // @todo: do hooks
}

void hooks::shutdown() {
  // @todo: undo hooks
}
