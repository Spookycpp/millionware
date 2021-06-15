#include "world.h"

#include <cstdio>
#include <string>
#include <windows.h>

#include <tlhelp32.h>
#include <psapi.h>
#include <algorithm>
#include <cstdint>
#include <unordered_map>

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"

#include "../../../engine/input/input.h"
#include "../../../engine/logging/logging.h"
#include "../../../engine/math/math.h"
#include "../../../engine/render/render.h"
#include "../../../engine/security/xorstr.h"

namespace features::visuals::world {

    void on_frame_stage_notify(e_client_frame_stage frame_stage) {
        switch (frame_stage) {
            case e_client_frame_stage::FRAME_STAGE_RENDER_START: {
                nightmode();
            }
        }
    }

    void indicators() {
        if (!cheat::local_player || cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
            return;

        const auto screen_center = render::get_screen_size() * 0.5f;

        auto draw_indicator = [screen_center, offset = 400.0f](const char *text, const color_t &color) mutable {
            const auto measure = render::measure_text(text, FONT_VERDANA_24);

            render::draw_text(screen_center - measure * 0.5f + point_t(0.0f, offset), color, text, FONT_VERDANA_24);

            offset += measure.y + 8.0f;
        };

       if (settings.visuals.local.indicators & (1 << 0)) {
            static auto tick_prev = 0;
            static auto last_velocity = 0;
            static auto take_off = 0;
            static auto take_off_time = 0.0f;
            static auto last_on_ground = false;

            const auto vel = (int) (cheat::local_player->get_velocity().length_2d() + 0.5f);
            const auto on_ground = (cheat::local_player->get_flags() & ENTITY_FLAG_ONGROUND);

            if (last_on_ground && !on_ground) {
                take_off = vel;
                take_off_time = interfaces::global_vars->current_time + 1.0f;
            }

            last_on_ground = on_ground;

            const auto color = vel == last_velocity ? color_t(255, 199, 89) : vel < last_velocity ? color_t(255, 119, 119) : color_t(30, 255, 109);
            const auto should_draw_takeoff = (!on_ground || (take_off_time > interfaces::global_vars->current_time)) && settings.visuals.local.indicators & (1 << 1);

            char buffer[32];

            if (should_draw_takeoff)
                sprintf_s(buffer, XORSTR("%i (%i)"), (int) vel, (int) take_off);
            else
                sprintf_s(buffer, XORSTR("%i"), (int) vel);

            draw_indicator(buffer, color);

            if (tick_prev + 5 < interfaces::global_vars->tick_count) {
                last_velocity = vel;
                tick_prev = interfaces::global_vars->tick_count;
            }
        }

        if (settings.visuals.local.indicators & (1 << 2) && settings.miscellaneous.movement.jump_bug && input::is_key_down(settings.miscellaneous.movement.jump_bug_hotkey))
            draw_indicator(XORSTR("jb"), {255, 255, 255, 220});

        if (settings.visuals.local.indicators & (1 << 4) && settings.miscellaneous.movement.edge_bug_assist && input::is_key_down(settings.miscellaneous.movement.edge_bug_assist_hotkey))
            draw_indicator(XORSTR("eb"), {255, 255, 255, 220});

        if (settings.visuals.local.indicators & (1 << 5) && settings.miscellaneous.movement.edge_jump && input::is_key_down(settings.miscellaneous.movement.edge_jump_hotkey))
            draw_indicator(XORSTR("ej"), {255, 255, 255, 220});

        if (settings.visuals.local.indicators & (1 << 6) && settings.miscellaneous.movement.fast_walk && input::is_key_down(settings.miscellaneous.movement.fast_walk_hotkey))
            draw_indicator(XORSTR("fw"), {255, 255, 255, 220});
    }

    void nightmode() {
        static bool toggled = false;
        static float last_darkness = 0.f;

        auto do_nightmode = settings.visuals.world.nightmode != toggled || last_darkness != settings.visuals.world.nightmode_darkness;

        if (!do_nightmode)
            return;

        toggled = settings.visuals.world.nightmode;
        last_darkness = settings.visuals.world.nightmode_darkness;

        if (!cheat::local_player || !interfaces::engine_client->is_in_game())
            return;

        const static auto draw_specific_static_prop = interfaces::convar_system->find_convar(XORSTR("r_DrawSpecificStaticProp"));

        const float world_darkness = settings.visuals.world.nightmode ? 1.f - (settings.visuals.world.nightmode_darkness / 100.f) : 1.f;
        const float prop_darkness = settings.visuals.world.nightmode ? 1.3f - (settings.visuals.world.nightmode_darkness / 100.f) : 1.f;

        draw_specific_static_prop->set_value(settings.visuals.world.nightmode ? 0 : -1);

        // iterate material handles.
        for (auto handle = interfaces::material_system->first_material(); handle != interfaces::material_system->invalid_material(); handle = interfaces::material_system->next_material(handle)) {

            // get material from handle.
            c_material *material = interfaces::material_system->get_material(handle);
            if (!material)
                continue;

            // if (material->is_error_material())
            //	continue;

            // modulate world materials.
            if (strncmp(material->get_group_name(), XORSTR("World textures"), 14) == 0)
                material->set_color(world_darkness, world_darkness, world_darkness);
            // modulate props materials.
            else if (strncmp(material->get_group_name(), XORSTR("StaticProp textures"), 19) == 0)
                material->set_color(prop_darkness, prop_darkness, prop_darkness);

            auto cl_csm_shadows = interfaces::convar_system->find_convar(XORSTR("cl_csm_shadows"));
            if (cl_csm_shadows->get_int() != 0)
                cl_csm_shadows->set_value(0);
        }
    }
} // namespace features::visuals::world
