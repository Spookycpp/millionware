#include <d3dx9.h>
#include <stdio.h>
#include <Windows.h>

#include "../thirdparty/minhook/minhook.h"
#include "../thirdparty/xorstr/xorstr.hpp"
#include "../utils/error.hpp"
#include "../utils/hash.hpp"
#include "hooks.hpp"
#include "interfaces.hpp"
#include "patterns.hpp"

#define ADD_HOOK(storage)                                                                                 \
  if (MH_CreateHook(reinterpret_cast<LPVOID>(storage.function), reinterpret_cast<LPVOID>(storage.detour), \
                    reinterpret_cast<LPVOID*>(&storage.original)) != MH_OK)                               \
  {                                                                                                       \
    utils::error_and_exit(e_error_code::HOOKS, HASH_FNV_CT("set up " #storage));                          \
  }

#define REMOVE_HOOK(storage)                                                      \
  if (MH_RemoveHook(reinterpret_cast<LPVOID>(storage.function)) != MH_OK)         \
    utils::error_and_exit(e_error_code::HOOKS, HASH_FNV_CT("remove " #storage));

extern void __fastcall engine_paint_hook(uintptr_t, uintptr_t, int);
extern void __fastcall screen_size_changed_hook(uintptr_t, uintptr_t, int, int);

template <typename T>
inline uintptr_t get_vfunc_address(T* base_interface, size_t index) {
  return reinterpret_cast<uintptr_t**>(base_interface)[0][index];
}

void hooks::initialize() {
  if (MH_Initialize() != MH_OK)
    utils::error_and_exit(e_error_code::HOOKS, HASH_FNV_CT("minhook initialization"));

  engine_paint.function = get_vfunc_address(interfaces::engine_vgui, 14);
  engine_paint.detour = reinterpret_cast<uintptr_t>(&engine_paint_hook);

  screen_size_changed.function = get_vfunc_address(interfaces::vgui_surface, 116);
  screen_size_changed.detour = reinterpret_cast<uintptr_t>(&screen_size_changed_hook);

  ADD_HOOK(engine_paint);
  ADD_HOOK(screen_size_changed);

  if (MH_EnableHook(nullptr) != MH_OK)
    utils::error_and_exit(e_error_code::HOOKS, HASH_FNV_CT("enable all hooks"));
}

void hooks::shutdown() {
  if (MH_DisableHook(nullptr) != MH_OK)
    utils::error_and_exit(e_error_code::HOOKS, HASH_FNV_CT("disable all hooks"));

  REMOVE_HOOK(engine_paint);
  REMOVE_HOOK(screen_size_changed);

  // make sure we can hold the call guards, so that means the hook isnt running
  // and we can safely free the trampoline code and shit and exit out!
  const auto _1 = std::lock_guard(engine_paint.call_mutex);
  const auto _2 = std::lock_guard(screen_size_changed.call_mutex);

  if (MH_Uninitialize() != MH_OK)
    utils::error_and_exit(e_error_code::HOOKS, HASH_FNV_CT("minhook uninitialization"));
}
