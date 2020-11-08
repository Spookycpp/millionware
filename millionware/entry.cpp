#include <Windows.h>

#include <chrono>
#include <thread>

#include "core/interfaces.hpp"

#ifdef _DEBUG
constexpr static bool IS_DEBUG = true;
#else
constexpr static bool IS_DEBUG = false;
#endif

unsigned long __stdcall initial_thread(const LPVOID dll_instance) {
  using namespace std::chrono_literals;

  interfaces::initialize();

  // @todo: initialize hooks

  if constexpr (IS_DEBUG) {
    while (!GetAsyncKeyState(VK_DELETE) && !GetAsyncKeyState(VK_END))
      std::this_thread::sleep_for(50ms);

    // @todo: uninitialize hooks

    FreeLibraryAndExitThread(static_cast<HMODULE>(dll_instance), 0);
  }

  return 0ul;
}

void handle_process_attach(const LPVOID dll_instance) {
  if constexpr (IS_DEBUG) {
    const auto thread_handle = CreateThread(nullptr, 0, initial_thread, dll_instance, 0, nullptr);

    if (thread_handle != nullptr)
      CloseHandle(thread_handle);
  }
  else {
    initial_thread(dll_instance);
  }
}

// ReSharper disable once CppInconsistentNaming
int __stdcall DllMain(const HINSTANCE dll_instance, const UINT call_reason, const LPVOID reserved) {
  if (call_reason == DLL_PROCESS_ATTACH)
    handle_process_attach(dll_instance);

  return TRUE;
}
