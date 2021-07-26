#include "weather.h"

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"
#include <format>

namespace features::visuals::weather {

    void update_weather() {

        static bool last_state = false;
        static bool created_rain = false;

        static c_precipitation_entity *rain_entity = nullptr;
        static c_client_class *precipitation = nullptr;

        if (!precipitation) {
            for (auto client_class = interfaces::client_dll->get_all_classes(); client_class && !precipitation; client_class = client_class->next) {
                if (client_class->class_id == CPrecipitation)
                    precipitation = client_class;
            }
        }

        if (!precipitation)
            return;

        if (last_state != settings.visuals.world.weather) {
            if (settings.visuals.world.weather) {
                if (precipitation->create && precipitation->create(MAX_EDICTS - 1, 0)) {
                    rain_entity = (c_precipitation_entity *) interfaces::entity_list->get_entity(MAX_EDICTS - 1);

                    rain_entity->get_networkable()->pre_data_update(0);
                    rain_entity->get_networkable()->on_pre_data_changed(0);

                    rain_entity->get_precip_type() = precipitation_type_t::PRECIPITATION_TYPE_SNOW;
                    rain_entity->get_mins() = {-16384.0f, -16384.0f, -16384.0f};
                    rain_entity->get_maxs() = {16384.0f, 16384.0f, 16384.0f};

                    rain_entity->get_networkable()->on_data_changed(0);
                    rain_entity->get_networkable()->post_data_update(0);
                }
            }
            else {
                const auto networkable = rain_entity ? rain_entity->get_networkable() : nullptr;

                if (networkable)
                    networkable->release();

                rain_entity = nullptr;
            }

            last_state = settings.visuals.world.weather;
        }
    }

    void do_fog() {

        if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
            return;

        static auto fog_override   = interfaces::convar_system->find_convar(xs("fog_override"));
        static auto fog_start      = interfaces::convar_system->find_convar(xs("fog_start"));
        static auto fog_end        = interfaces::convar_system->find_convar(xs("fog_end"));
        static auto fog_maxdensity = interfaces::convar_system->find_convar(xs("fog_maxdensity"));
        static auto fog_color_cvar = interfaces::convar_system->find_convar(xs("fog_color"));

        const auto fog_enable = settings.visuals.world.fog;
        const auto fog_length = settings.visuals.world.fog_length;
        const auto fog_color  = settings.visuals.world.fog_color;

        static bool old_enable = false;
        static int old_length = 0;
        static color_t old_color;

        if (fog_enable != old_enable || fog_length != old_length || fog_color != old_color) {
            fog_override->set_value(fog_enable);
            fog_start->set_value(0);
            fog_end->set_value(fog_length);
            fog_maxdensity->set_value(fog_color.a / 255.f);
            fog_color_cvar->set_value(std::format(xs("{} {} {}"), fog_color.r, fog_color.g, fog_color.b).data());
            old_enable = fog_enable;
            old_length = fog_length;
            old_color  = fog_color;
        }
    }
} // namespace features::visuals::weather