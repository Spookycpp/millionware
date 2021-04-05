#include <windows.h>

#include "../../engine/input/input.h"
#include "../../engine/logging/logging.h"
#include "../../engine/render/render.h"
#include "../../engine/security/xorstr.h"
#include "../hooks/hooks.h"
#include "../interfaces/interfaces.h"
#include "../netvars/netvars.h"
#include "../patterns/patterns.h"
#include "../scripting/scripting.h"
#include "cheat.h"

bool cheat::init()
{
	if (!patterns::init())
		return false;

	if (!interfaces::init())
		return false;

	netvars::init();

	const auto device = (IDirect3DDevice9 *) patterns::get_d3d9_device();

	D3DDEVICE_CREATION_PARAMETERS creation_params;

	device->GetCreationParameters(&creation_params);

	render::init(creation_params.hFocusWindow, device);

	input::init(creation_params.hFocusWindow);

	if (!hooks::init())
		return false;

	logging::info(XORSTR("successfully initialized"));

	return true;
}

bool cheat::undo()
{
	if (!hooks::undo())
		return false;

	Sleep(1000);

	render::undo();

	input::undo();

	logging::info(XORSTR("successfully uninitialized"));

	return true;
}
