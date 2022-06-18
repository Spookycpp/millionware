#include <cstdio>
#include <windows.h>

#include "../../engine/hash/hash.h"
#include "../../engine/logging/logging.h"
#include "../../engine/pe/pe.h"
#include "../../engine/security/xorstr.h"
#include "../exports/exports.h"
#include "../patterns/patterns.h"
#include "interfaces.h"

uint32_t get_interface_internal(std::string_view module_name, std::string_view interface_name) {
    //#ifdef _DEBUG
    using create_interface_fn = uint32_t (*)(const char *, int *);

    const auto create_interface = (create_interface_fn) pe::get_export(module_name, xs("CreateInterface"));

    if (!create_interface)
        return 0u;

    return create_interface(interface_name.data(), nullptr);
    //#else
    // char interface_hash_buffer[80];
    //
    // sprintf_s(interface_hash_buffer, XORSTR("%s:%s"), module_name.data(), interface_name.data());
    //
    // return exports::get_interface(CRC(interface_hash_buffer));
    //#endif
}

inline uint32_t get_interface(std::string_view module_name, std::string_view interface_name) {
    const auto result = get_interface_internal(module_name, interface_name);

    if (result != 0u)
        logging::debug(xs("found interface '{}' in '{}' at 0x{:08x}"), interface_name.data(), module_name.data(), result);
    else
        logging::error(xs("couldn't find interface '{}' in '{}'"), interface_name.data(), module_name.data());

    return result;
}

bool interfaces::init() {
    if ((client_dll = (c_base_client_dll *) get_interface(xs("client.dll"), xs("VClient018"))) == nullptr)
        return false;

    if ((game_movement = (c_game_movement *) get_interface(xs("client.dll"), xs("GameMovement001"))) == nullptr)
        return false;

    if ((entity_list = (c_entity_list *) get_interface(xs("client.dll"), xs("VClientEntityList003"))) == nullptr)
        return false;

    if ((effects = (c_effects *) get_interface(xs("client.dll"), xs("IEffects001"))) == nullptr)
        return false;

    if ((engine_sound = (c_engine_sound *) get_interface(xs("engine.dll"), xs("IEngineSoundClient003"))) == nullptr)
        return false;

    if ((input_system = (c_input_system *) get_interface(xs("inputsystem.dll"), xs("InputSystemVersion001"))) == nullptr)
        return false;

    if ((engine_client = (c_engine_client *) get_interface(xs("engine.dll"), xs("VEngineClient014"))) == nullptr)
        return false;

    if ((trace = (c_engine_trace *) get_interface(xs("engine.dll"), xs("EngineTraceClient004"))) == nullptr)
        return false;

    if ((debug_overlay = (c_debug_overlay *) get_interface(xs("engine.dll"), xs("VDebugOverlay004"))) == nullptr)
        return false;

    if ((game_events = (c_game_event_manager2 *) get_interface(xs("engine.dll"), xs("GAMEEVENTSMANAGER002"))) == nullptr) {
        return false;
    } else {
        _event_listener = std::make_unique<c_event_listener>();
    }

    if ((game_types = (c_game_types *) get_interface(xs("matchmaking.dll"), xs("VENGINE_GAMETYPES_VERSION002"))) == nullptr)
        return false;

    if ((panorama = (c_panorama_engine *) get_interface(xs("panorama.dll"), xs("PanoramaUIEngine001"))) == nullptr)
        return false;

    if ((model_info = (c_model_info *) get_interface(xs("engine.dll"), xs("VModelInfoClient004"))) == nullptr)
        return false;

    if ((model_render = (c_model_render *) get_interface(xs("engine.dll"), xs("VEngineModel016"))) == nullptr)
        return false;

    if ((material_system = (c_material_system *) get_interface(xs("materialsystem.dll"), xs("VMaterialSystem080"))) == nullptr)
        return false;

    if ((vgui_engine = (void *) get_interface(xs("engine.dll"), xs("VEngineVGui001"))) == nullptr)
        return false;

    if ((d3d9_device = (IDirect3DDevice9 *) patterns::get_d3d9_device()) == nullptr)
        logging::error(xs("failed to find d3d9 device pattern"));

    if ((surface = (c_vgui_surface *) get_interface(xs("vguimatsurface.dll"), xs("VGUI_Surface031"))) == nullptr)
        return false;

    if ((convar_system = (c_cvar *) get_interface(xs("vstdlib.dll"), xs("VEngineCvar007"))) == nullptr)
        return false;

    if ((prediction = (c_prediction *) get_interface(xs("client.dll"), xs("VClientPrediction001"))) == nullptr)
        return false;

    if ((localize = (c_localize *) get_interface(xs("localize.dll"), xs("Localize_001"))) == nullptr)
        return false;

    if ((file_system = (c_base_file_system *) get_interface(xs("filesystem_stdio.dll"), xs("VBaseFileSystem011"))) == nullptr)
        return false;

    if ((d3d9_device = (IDirect3DDevice9 *) patterns::get_d3d9_device()) == nullptr) {
        logging::error(xs("failed to find d3d9 device pattern"));

        return false;
    }

    if ((client_mode = (c_client_mode *) patterns::get_client_mode()) == nullptr) {
        logging::error(xs("failed to find client mode pattern"));

        return false;
    }

    if ((global_vars = (c_global_vars *) patterns::get_global_vars()) == nullptr) {
        logging::error(xs("failed to find global variables pattern"));

        return false;
    }

    if ((client_state = (c_client_state *) patterns::get_client_state()) == nullptr) {
        logging::error(xs("failed to find client state pattern"));

        return false;
    }

    if ((weapon_system = (c_weapon_system *) patterns::get_weapon_system()) == nullptr) {
        logging::error(xs("failed to find weapon system pattern"));

        return false;
    }

    if ((input = (c_input *) patterns::get_input()) == nullptr) {
        logging::error(xs("failed to find input pattern"));

        return false;
    }

    if ((move_helper = (c_move_helper *) patterns::get_move_helper()) == nullptr) {
        logging::error(xs("failed to find move helper pattern"));

        return false;
    }

    if ((render_beams = (c_view_render_beams *) patterns::get_render_beams()) == nullptr) {
        logging::error(xs("failed to find render beams pattern"));

        return false;
    }

    return true;
}