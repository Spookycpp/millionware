#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/render.hpp"

#include "../features/misc/misc.hpp"

void __fastcall hooks::frame_stage_notify_hook(uintptr_t ecx, uintptr_t edx, int frame_stage) {
	const auto _ = std::lock_guard(hooks::frame_stage_notify.call_mutex);

	if (frame_stage == FRAME_STAGE_START) {
		features::misc::remove_panorama_blur();
		features::misc::remove_post_processing();
		features::misc::clantag();
		features::misc::name_spam();
		features::misc::flash_alpha();
		features::misc::force_crosshair();
	}

	reinterpret_cast<decltype(&frame_stage_notify_hook)>(hooks::frame_stage_notify.original)(ecx, edx, frame_stage);
}
