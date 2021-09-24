#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/util/util.h"

#include "../features/discord presence/rpc.h"
#include "../features/lagcompensation/lagcompensation.h"
#include "../features/miscellaneous/miscellaneous.h"
#include "../features/visuals/world/weather.h"
#include "../features/visuals/world/world.h"

void __fastcall hooks::frame_stage_notify(c_base_client_dll *ecx, uintptr_t edx, e_client_frame_stage frame_stage) {
	discord_rpc::update();
	features::miscellaneous::panorama_blur(); // main menu blur can be removed

	if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
		return frame_stage_notify_original(ecx, edx, frame_stage);

	if (frame_stage == e_client_frame_stage::FRAME_STAGE_START) {
		cheat::view_matrix = interfaces::engine_client->world_to_screen_matrix();
		cheat::local_player = static_cast<c_player *>(interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player()));
	}

	features::lag_compensation::on_frame_stage_notify(frame_stage);
	features::miscellaneous::on_frame_stage_notify(frame_stage);
	features::visuals::weather::on_frame_stage_notify(frame_stage);
	features::visuals::world::on_frame_stage_notify(frame_stage);
	util::on_frame_stage_notify(frame_stage);

	frame_stage_notify_original(ecx, edx, frame_stage);
}
