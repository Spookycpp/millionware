#include "../core/interfaces/interfaces.h"
#include "../core/hooks/hooks.h"
#include "../core/util/util.h"

#include "../features/lagcompensation/lagcompensation.h"
#include "../features/miscellaneous/miscellaneous.h"

void __fastcall hooks::frame_stage_notify(c_base_client_dll *ecx, uintptr_t edx, int frame_stage)
{
	if (!interfaces::engine_client->is_in_game() || !util::run_frame())
		return frame_stage_notify_original(ecx, edx, frame_stage);

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

	frame_stage_notify_original(ecx, edx, frame_stage);
}
