#include <Windows.h>

#include <cstdint>
#include <d3dx9.h>
#include <mutex>

#include "../core/hooks.hpp"
#include "../utils/render.hpp"

static std::once_flag initialize_renderer;

// ReSharper disable CppParameterMayBeConst
long __fastcall present_hook(const uintptr_t ecx, const uintptr_t edx, LPDIRECT3DDEVICE9 device, const RECT* source_rect, const RECT* dest_rect, HWND dest_window_override, const RGNDATA* dirty_region) {
  const auto _ = std::lock_guard<std::mutex>(hooks::present.call_mutex);

  std::call_once(initialize_renderer, [device]() {
    render::initialize(device);
  });

  render::begin_frame();
  render::rect();
  render::finish_frame();

  return reinterpret_cast<decltype(&present_hook)>(hooks::present.original)(ecx, edx, device, source_rect, dest_rect, dest_window_override, dirty_region);
}
