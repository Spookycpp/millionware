#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"

#include "../features/miscellaneous/miscellaneous.hpp"

void __fastcall hooks::frame_stage_notify_hook(uintptr_t ecx, uintptr_t edx, int frame_stage) {

	if (frame_stage == FRAME_STAGE_START) {
		features::miscellaneous::panorama_blur();
		features::miscellaneous::post_processing();
		features::miscellaneous::force_crosshair();
		features::miscellaneous::ragdoll_float();
		features::miscellaneous::ragdoll_push();
	}

	hooks::frame_stage_notify.get_original<decltype(&frame_stage_notify_hook)>()(ecx, edx, frame_stage);
}
