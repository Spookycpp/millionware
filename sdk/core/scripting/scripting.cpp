#include "scripting.h"
#include "../../engine/security/xorstr.h"
#include "../interfaces/interfaces.h"

static c_ui_panel *root_panel;

void scripting::init() {
    const auto ui_engine = interfaces::panorama->access_ui_engine();

    auto iter = ui_engine->get_last_target_panel();

    while (iter != nullptr && ui_engine->is_valid_panel_ptr(iter)) {
        if (strcmp(iter->get_panel_id(), xs("CSGOMainMenu")) == 0)
            break;

        iter = iter->get_parent();
    }

    root_panel = iter;
}

void scripting::run_script(const std::string &script) {
    const auto ui_engine = interfaces::panorama->access_ui_engine();

    ui_engine->run_script(root_panel, script.data(), xs("panorama/layout/base_mainmenu.xml"), 8, 10, false, false);
}
