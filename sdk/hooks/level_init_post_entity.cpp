#include "../core/cheat/cheat.h"
#include "../core/hooks/hooks.h"
#include "../core/interfaces/interfaces.h"
#include "../core/util/util.h"

#include "../engine/logging/logging.h"
#include "../engine/security/xorstr.h"

void __fastcall hooks::level_init_post_entity(c_base_client_dll *ecx, uintptr_t edx) {

    level_init_post_entity_original(ecx, edx);

    logging::debug(xs("Detected level initialization"));

    cheat::local_player = (c_player *) interfaces::entity_list->get_entity(interfaces::engine_client->get_local_player());
    util::on_map_load();
}