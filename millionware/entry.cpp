#include <Windows.h>

#include <chrono>
#include <thread>

#include "core/cheat.hpp"
#include "core/config.hpp"
#include "core/hooks.hpp"
#include "core/interfaces.hpp"
#include "core/netvars.hpp"
#include "core/patterns.hpp"
#include "features/hitchance/hitchance.hpp"
#include "gui/gui.hpp"
#include "menu/menu.hpp"
#include "utils/render/render.hpp"

unsigned long __stdcall initial_thread(LPVOID dll_instance) {
  using namespace std::chrono_literals;

  cheat::initialize();
  patterns::initialize();
  interfaces::initialize();
  config::initialize();
  netvars::initialize();
  gui::initialize();

  // lol
  features::hit_chance::initialize();

  hooks::initialize();

#ifdef _DEBUG
  while (!GetAsyncKeyState(VK_DELETE) || !GetAsyncKeyState(VK_END))
    std::this_thread::sleep_for(50ms);

  hooks::shutdown();

  std::this_thread::sleep_for(300ms);

  FreeLibraryAndExitThread(static_cast<HMODULE>(dll_instance), 0);
#endif

  return 0ul;
}

void handle_process_attach(LPVOID dll_instance) {
#ifdef _DEBUG
  const auto thread_handle = CreateThread(nullptr, 0, initial_thread, dll_instance, 0, nullptr);

  if (thread_handle != nullptr)
    CloseHandle(thread_handle);
#else
  initial_thread(dll_instance);
#endif
}

// ReSharper disable once CppInconsistentNaming
int __stdcall DllMain(HINSTANCE dll_instance, UINT call_reason, LPVOID reserved) {
  if (call_reason == DLL_PROCESS_ATTACH)
    handle_process_attach(dll_instance);

  return TRUE;
}
