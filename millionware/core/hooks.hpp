#pragma once

#include <mutex>

struct hook_storage_t
{
  uintptr_t original;

  std::mutex call_mutex;
};

namespace hooks
{
  inline hook_storage_t present;
  inline hook_storage_t reset;

  void initialize();
  void shutdown();
}
