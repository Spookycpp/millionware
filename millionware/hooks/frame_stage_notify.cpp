#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"

#include "../features/lag compensation/lag_compensation.hpp"
#include "../features/miscellaneous/miscellaneous.hpp"

#include "../utils/util.hpp"

void __fastcall hooks::frame_stage_notify_hook(uintptr_t ecx, uintptr_t edx, int frame_stage) {

	if (!interfaces::engine_client->is_in_game() || !util::run_frame())
		return hooks::frame_stage_notify.get_original<decltype(&frame_stage_notify_hook)>()(ecx, edx, frame_stage);

	features::lag_compensation::on_frame_stage_notify(frame_stage);

	if (frame_stage == FRAME_STAGE_START) {
		features::miscellaneous::panorama_blur();
		features::miscellaneous::force_crosshair();
		features::miscellaneous::flash_alpha();

		features::miscellaneous::ragdoll_float();
		features::miscellaneous::ragdoll_push();
	}

	if (frame_stage == FRAME_STAGE_NET_UPDATE_POSTDATAUPDATE_START) {
		features::miscellaneous::post_processing();
	}

	hooks::frame_stage_notify.get_original<decltype(&frame_stage_notify_hook)>()(ecx, edx, frame_stage);
}
