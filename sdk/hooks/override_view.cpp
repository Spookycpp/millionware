#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/settings/settings.h"
#include "../core/util/util.h"

#include "../features/miscellaneous/miscellaneous.h"

int __fastcall hooks::override_view(c_client_mode *ecx, uintptr_t edx, view_setup_t *view_setup)
{
	if (!view_setup)
		return override_view_original(ecx, edx, view_setup);

	if (view_setup) 
		features::miscellaneous::on_override_view(view_setup);

	if (interfaces::engine_client->is_in_game()) {
		auto view_model = (c_player*)cheat::local_player->get_view_model_handle().get();

		if (view_model) {
			auto eye_angles = view_setup->angles;

			if (settings.visuals.local.viewmodel_offset)
				eye_angles.z -= settings.visuals.local.viewmodel_offset_r;

			view_model->set_abs_angles(eye_angles);
		}
	}

	return override_view_original(ecx, edx, view_setup);
}
