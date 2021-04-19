#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../engine/render/render.h"
#include "../engine/security/xorstr.h"
#include "../features/visuals/players/esp.h"
#include "../features/visuals/world/indicators.h"
#include "../ui/ui.h"

static int active_tab = 0;

long __stdcall hooks::present(IDirect3DDevice9 *device, RECT *source_rect, RECT *dest_rect, HWND dest_window_override, RGNDATA *dirty_region)
{
	render::begin();

	features::visuals::esp::frame();
	features::visuals::indicators::indicators();

	ui::frame();

	render::finish();

	return present_original(device, source_rect, dest_rect, dest_window_override, dirty_region);
}
