#include <Windows.h>

#include <d3dx9.h>
#include <stdio.h>

#include "../thirdparty/minhook/minhook.h"
#include "../thirdparty/xorstr/xorstr.hpp"
#include "../utils/hash.hpp"
#include "hooks.hpp"
#include "patterns.hpp"

extern long __fastcall present_hook(uintptr_t ecx, uintptr_t edx, LPDIRECT3DDEVICE9 device, const RECT* source_rect, const RECT* dest_rect, HWND dest_window_override, const RGNDATA* dirty_region);
extern long __fastcall reset_hook(uintptr_t ecx, uintptr_t edx, const D3DPRESENT_PARAMETERS* presentation_parameters);

inline void error_and_quit(const uint32_t error_code) {
  char buffer[128];

  sprintf_s(buffer, sizeof buffer, XORSTR(
    "Fatal error during initialization. \n"
    "Please reach out to us via a ticket. \n"
    "Additional info: E03 %08X"
  ), error_code);

  MessageBoxA(nullptr, buffer, nullptr, MB_OK | MB_ICONERROR);
  TerminateProcess(GetCurrentProcess(), -1);
}

inline uintptr_t get_vfunc_address(const uintptr_t base_address, const size_t index) {
  return reinterpret_cast<uintptr_t**>(base_address)[0][index];
}

void hooks::initialize() {
  if (MH_Initialize() != MH_OK)
    error_and_quit(HASH_FNV_CT("minhook initialization"));

  if (MH_CreateHook(reinterpret_cast<LPVOID>(patterns::d3d9_present), static_cast<LPVOID>(present_hook), reinterpret_cast<LPVOID*>(&present.original)) != MH_OK)
    error_and_quit(HASH_FNV_CT("create d3d9 present"));

  if (MH_CreateHook(reinterpret_cast<LPVOID>(patterns::d3d9_reset), static_cast<LPVOID>(reset_hook), reinterpret_cast<LPVOID*>(&reset.original)) != MH_OK)
    error_and_quit(HASH_FNV_CT("create d3d9 reset"));

  if (MH_EnableHook(nullptr) != MH_OK)
    error_and_quit(HASH_FNV_CT("enable all hooks"));
}

void hooks::shutdown() {
  if (MH_DisableHook(nullptr) != MH_OK)
    error_and_quit(HASH_FNV_CT("disable all hooks"));

  if (MH_RemoveHook(reinterpret_cast<LPVOID>(patterns::d3d9_present)) != MH_OK)
    error_and_quit(HASH_FNV_CT("remove d3d9 present"));

  if (MH_RemoveHook(reinterpret_cast<LPVOID>(patterns::d3d9_reset)) != MH_OK)
    error_and_quit(HASH_FNV_CT("remove d3d9 reset"));

  // make sure we can hold the call guards, so that means the hook isnt running
  // and we can safely free the trampoline code and shit and exit out!
  const auto _1 = std::lock_guard<std::mutex>(present.call_mutex);
  const auto _2 = std::lock_guard<std::mutex>(reset.call_mutex);

  if (MH_Uninitialize() != MH_OK)
    error_and_quit(HASH_FNV_CT("minhook uninitialization"));
}
