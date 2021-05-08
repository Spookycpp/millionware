#include "world.h"

#include <algorithm>
#include <cstdint>
#include <unordered_map>

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"

#include "../../engine/input/input.h"
#include "../../engine/logging/logging.h"
#include "../../engine/math/math.h"
#include "../../engine/render/render.h"
#include "../../engine/security/xorstr.h"

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

            static auto last_velocity = 0.f;
            static auto take_off_velocity = 0.f;
            static auto take_off = false;
            static auto take_off_tick = 0;

            const auto velocity = cheat::local_player->get_velocity().length_2d();
            const auto velocity_difference = (int) velocity - (int) last_velocity;
            const auto on_ground = cheat::local_player->get_flags() & ENTITY_FLAG_ONGROUND;

            if (take_off && !on_ground) {
                take_off_velocity = velocity;
                take_off_tick = interfaces::global_vars->tick_count;
            }

            take_off = on_ground;

            const auto color = velocity_difference < 0 ? color_t(255, 199, 89) : (velocity_difference > 0 ? color_t(30, 255, 109) : color_t(30, 255, 109));
            const auto should_draw_take_off = (!on_ground || take_off_tick > interfaces::global_vars->tick_count) && settings.visuals.local.indicators & (1 << 1);

            char buffer[32];

            if (should_draw_take_off)
                sprintf_s(buffer, XORSTR("%.0f (%.0f)"), velocity, take_off_velocity);
            else
                sprintf_s(buffer, XORSTR("%.0f"), velocity);

            draw_indicator(buffer, color);

            if (interfaces::global_vars->tick_count % 8 == 0)
                last_velocity = velocity;
        }

        if (settings.visuals.local.indicators & (1 << 2) && settings.miscellaneous.movement.jump_bug && input::is_key_down(settings.miscellaneous.movement.jump_bug_hotkey))
            draw_indicator(XORSTR("jb"), {255, 255, 255, 220});

        if (settings.visuals.local.indicators & (1 << 3) && settings.miscellaneous.movement.edge_bug && input::is_key_down(settings.miscellaneous.movement.edge_bug_hotkey))
            draw_indicator(XORSTR("eb"), {255, 255, 255, 220});

        if (settings.visuals.local.indicators & (1 << 4) && settings.miscellaneous.movement.edge_bug_assist && input::is_key_down(settings.miscellaneous.movement.edge_bug_assist_hotkey))
            draw_indicator(XORSTR("eba"), {255, 255, 255, 220});

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
        }
    }
} // namespace features::visuals::world
