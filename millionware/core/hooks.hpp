#pragma once

#include <mutex>

struct hook_storage_t
{
  uintptr_t function = 0;
  uintptr_t detour = 0;
  uintptr_t original = 0;

  std::mutex call_mutex;
};

namespace hooks
{
  inline hook_storage_t engine_paint;
  inline hook_storage_t screen_size_changed;

  void initialize();
  void shutdown();
}
