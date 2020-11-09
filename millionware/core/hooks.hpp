#pragma once

namespace hooks
{
  inline uintptr_t original_present = 0;
  inline uintptr_t original_reset = 0;

  void initialize();
  void shutdown();
}
