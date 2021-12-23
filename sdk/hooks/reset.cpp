#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"

#include "../engine/render/render.h"

long __stdcall hooks::reset(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *present_parameters) {
    render::undo();

    auto resolution = reset_hk.call_original<decltype(&reset)>(device, present_parameters);

    D3DDEVICE_CREATION_PARAMETERS creation_params;

    device->GetCreationParameters(&creation_params);

    render::init(creation_params.hFocusWindow, device);

    return resolution;
}