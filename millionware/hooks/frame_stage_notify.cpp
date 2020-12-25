#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/render.hpp"

void __fastcall hooks::frame_stage_notify_hook(uintptr_t ecx, uintptr_t edx, int frame_stage) {
	const auto _ = std::lock_guard(hooks::frame_stage_notify.call_mutex);

	reinterpret_cast<decltype(&frame_stage_notify_hook)>(hooks::frame_stage_notify.original)(ecx, edx, frame_stage);
}
