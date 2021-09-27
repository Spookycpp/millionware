#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/util/util.h"

#include "../engine/logging/logging.h"
#include "../engine/security/xorstr.h"

#include "../features/visuals/world/weather.h"

void __fastcall hooks::level_shutdown_pre_entity(c_base_client_dll *ecx, uintptr_t edx) {
    level_shutdown_pre_entity_original(ecx, edx);

    logging::debug(xs("Detected level shutdown"));

    cheat::local_player = nullptr;
    features::visuals::weather::reset_weather(false);
}
