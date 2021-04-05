#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../ui/gui/gui.h"

void __fastcall hooks::lock_cursor(c_vgui_surface *ecx, uintptr_t edx)
{
	if (gui::is_active())
		return interfaces::surface->unlock_cursor();

	return lock_cursor_original(ecx, edx);
}
