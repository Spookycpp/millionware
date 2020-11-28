#include <stdio.h>
#include <Windows.h>

#include "../thirdparty/minhook/minhook.h"
#include "../thirdparty/xorstr/xorstr.hpp"
#include "../utils/error.hpp"
#include "../utils/hash.hpp"
#include "cheat.hpp"
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

template <typename T>
inline uintptr_t get_vfunc_address(T* base_interface, size_t index) {
  return reinterpret_cast<uintptr_t**>(base_interface)[0][index];
}

void hooks::initialize() {
  // initialize stuff initialized by hooks that aren't ran immediately
  if (interfaces::engine_client->is_in_game())
    cheat::local_player = interfaces::entity_list->get_by_index(interfaces::engine_client->get_local_player())->as_player();

  // initialize hooking engine
  if (MH_Initialize() != MH_OK)
    utils::error_and_exit(e_error_code::HOOKS, HASH_FNV_CT("minhook initialization"));

  // add hooks
  create_move.function = get_vfunc_address(interfaces::client_mode, 24);
  create_move.detour = reinterpret_cast<uintptr_t>(&create_move_hook);

  level_init_post_entity.function = get_vfunc_address(interfaces::client, 6);
  level_init_post_entity.detour = reinterpret_cast<uintptr_t>(&level_init_post_entity_hook);

  level_shutdown_pre_entity.function = get_vfunc_address(interfaces::client, 7);
  level_shutdown_pre_entity.detour = reinterpret_cast<uintptr_t>(&level_shutdown_pre_entity_hook);

  frame_stage_notify.function = get_vfunc_address(interfaces::client, 37);
  frame_stage_notify.detour = reinterpret_cast<uintptr_t>(&frame_stage_notify_hook);

  engine_paint.function = get_vfunc_address(interfaces::engine_vgui, 14);
  engine_paint.detour = reinterpret_cast<uintptr_t>(&engine_paint_hook);

  lock_cursor.function = get_vfunc_address(interfaces::vgui_surface, 67);
  lock_cursor.detour = reinterpret_cast<uintptr_t>(&lock_cursor_hook);

  screen_size_changed.function = get_vfunc_address(interfaces::vgui_surface, 116);
  screen_size_changed.detour = reinterpret_cast<uintptr_t>(&screen_size_changed_hook);

  ADD_HOOK(create_move);
  ADD_HOOK(level_init_post_entity);
  ADD_HOOK(level_shutdown_pre_entity);
  ADD_HOOK(frame_stage_notify);
  ADD_HOOK(engine_paint);
  ADD_HOOK(lock_cursor);
  ADD_HOOK(screen_size_changed);

  if (MH_EnableHook(nullptr) != MH_OK)
    utils::error_and_exit(e_error_code::HOOKS, HASH_FNV_CT("enable all hooks"));
}

void hooks::shutdown() {
  if (MH_DisableHook(nullptr) != MH_OK)
    utils::error_and_exit(e_error_code::HOOKS, HASH_FNV_CT("disable all hooks"));

  REMOVE_HOOK(create_move);
  REMOVE_HOOK(level_init_post_entity);
  REMOVE_HOOK(level_shutdown_pre_entity);
  REMOVE_HOOK(frame_stage_notify);
  REMOVE_HOOK(engine_paint);
  REMOVE_HOOK(lock_cursor);
  REMOVE_HOOK(screen_size_changed);

  // make sure we can hold the call guards, so that means the hook isnt running
  // and we can safely free the trampoline code and shit and exit out!
  const auto _1 = std::lock_guard(create_move.call_mutex);
  const auto _2 = std::lock_guard(level_init_post_entity.call_mutex);
  const auto _3 = std::lock_guard(level_shutdown_pre_entity.call_mutex);
  const auto _4 = std::lock_guard(frame_stage_notify.call_mutex);
  const auto _5 = std::lock_guard(engine_paint.call_mutex);
  const auto _6 = std::lock_guard(lock_cursor.call_mutex);
  const auto _7 = std::lock_guard(screen_size_changed.call_mutex);

  if (MH_Uninitialize() != MH_OK)
    utils::error_and_exit(e_error_code::HOOKS, HASH_FNV_CT("minhook uninitialization"));

  // revert what hooks might've possibly messed up
  interfaces::input_system->enable_input(true);
}
