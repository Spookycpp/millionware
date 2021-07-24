#include "weather.h"

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"
#include <format>

namespace features::visuals::weather {

    void update_weather() {

        static bool last_state = false;
        static void *rain_networkable = nullptr;
        c_precipitation_entity *rain_ent = (c_precipitation_entity *) interfaces::entity_list->get_entity(MAX_EDICTS - 1);
        static c_client_class *precipitation_client_class = nullptr;

        if (!settings.visuals.world.weather)
            return;

        if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected()) {
            rain_networkable = rain_ent = nullptr;
            return;
        }

        if (!cheat::local_player)
            return;

        if (!precipitation_client_class) {
            for (auto pclass = interfaces::client_dll->get_all_classes(); pclass && !precipitation_client_class; pclass = pclass->next)
                if (strstr(pclass->class_name, XORSTR("CPrecipitation"))) {

                    precipitation_client_class = pclass;
                }
        }

        else {
            if (!rain_ent && precipitation_client_class && precipitation_client_class->create) {

                rain_networkable = ((void *(*) (int, int) ) precipitation_client_class->create)(MAX_EDICTS - 1, 0);

                if (rain_networkable) {
                    rain_ent = (c_precipitation_entity *) interfaces::entity_list->get_entity(MAX_EDICTS - 1);

                    rain_ent->get_precip_type() = (precipitation_type_t::PRECIPITATION_TYPE_SNOW);

                    rain_ent->get_networkable()->pre_data_update(0);
                    rain_ent->get_networkable()->on_pre_data_changed(0);

                    rain_ent->get_mins() = vector_t(-32767.0f, -32767.0f, -32767.0f);
                    rain_ent->get_maxs() = vector_t(32767.0f, 32767.0f, 32767.0f);

                    rain_ent->get_networkable()->on_data_changed(0);
                    rain_ent->get_networkable()->post_data_update(0);
                }
            }
        }
    }

    void do_fog() {

        if (!settings.visuals.world.fog)
            return;

        if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
            return;

        static auto fog_enable           = interfaces::convar_system->find_convar(XORSTR("fog_enable"));
        static auto fog_enableskybox     = interfaces::convar_system->find_convar(XORSTR("fog_enableskybox"));
        static auto fog_override         = interfaces::convar_system->find_convar(XORSTR("fog_override"));
        static auto fog_color            = interfaces::convar_system->find_convar(XORSTR("fog_color"));
        static auto fog_colorskybox      = interfaces::convar_system->find_convar(XORSTR("fog_colorskybox"));
        static auto fog_maxdensity       = interfaces::convar_system->find_convar(XORSTR("fog_maxdensity"));
        static auto fog_maxdensityskybox = interfaces::convar_system->find_convar(XORSTR("fog_maxdensityskybox"));
        static auto fog_start            = interfaces::convar_system->find_convar(XORSTR("fog_start"));
        static auto fog_end              = interfaces::convar_system->find_convar(XORSTR("fog_end"));
        static auto fog_endskybox        = interfaces::convar_system->find_convar(XORSTR("fog_endskybox"));

        auto override_fog_color = settings.visuals.world.fog_color;

        fog_enable->callbacks.clear();
        fog_enableskybox->callbacks.clear();
        fog_override->callbacks.clear();
        fog_color->callbacks.clear();
        fog_colorskybox->callbacks.clear();

        fog_start->set_value(0);
        fog_override->set_value(settings.visuals.world.fog);
        fog_end->set_value(settings.visuals.world.fog_length);
        fog_endskybox->set_value(settings.visuals.world.fog_length);
        fog_maxdensity->set_value(override_fog_color.a / 255.f);
        fog_maxdensityskybox->set_value(override_fog_color.a / 255.f);

        fog_color->set_value(std::format(XORSTR("{} {} {}"), override_fog_color.r, override_fog_color.g, override_fog_color.b).data());
        fog_colorskybox->set_value(std::format(XORSTR("{} {} {}"), override_fog_color.r, override_fog_color.g, override_fog_color.b).data());
    }
} // namespace features::visuals::weather