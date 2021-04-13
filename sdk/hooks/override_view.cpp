#include "../core/hooks/hooks.h"

#include "../features/miscellaneous/miscellaneous.h"

int __fastcall hooks::override_view(c_client_mode *ecx, uintptr_t edx, view_setup_t *view_setup)
{
	if (!view_setup)
		return override_view_original(ecx, edx, view_setup);

	if (util::run_frame() && view_setup) 
		features::miscellaneous::on_override_view(view_setup);

	return override_view_original(ecx, edx, view_setup);
}
