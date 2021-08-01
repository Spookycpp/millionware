#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../engine/hash/hash.h"

void __fastcall hooks::paint_traverse(c_vgui_panel *ecx, uintptr_t edx, int panel_id, bool force_repaint, bool allow_force)
{
	paint_traverse_original(ecx, edx, panel_id, force_repaint, allow_force);
}
