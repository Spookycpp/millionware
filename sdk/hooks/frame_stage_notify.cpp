#include "../core/interfaces/interfaces.h"
#include "../core/hooks/hooks.h"
#include "../core/util/util.h"

#include "../features/lagcompensation/lagcompensation.h"
#include "../features/miscellaneous/miscellaneous.h"

void __fastcall hooks::frame_stage_notify(c_base_client_dll *ecx, uintptr_t edx, e_client_frame_stage frame_stage)
{
	if (!interfaces::engine_client->is_in_game() || !util::run_frame())
		return frame_stage_notify_original(ecx, edx, frame_stage);

	features::miscellaneous::on_frame_stage_notify(frame_stage);
	features::lag_compensation::on_frame_stage_notify(frame_stage);
	util::on_frame_stage_notify(frame_stage);

	frame_stage_notify_original(ecx, edx, frame_stage);
}
