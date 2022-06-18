#include "weather.h"

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"

#include "../../../engine/logging/logging.h"

#include <format>

namespace features::visuals::weather {

    static bool last_state = false;
    static bool created_rain = false;

    static c_precipitation_entity *rain_entity = nullptr;
    static c_client_class *precipitation = nullptr;

    void on_frame_stage_notify(const e_client_frame_stage frame_stage) {
        switch (frame_stage) {
        case e_client_frame_stage::FRAME_STAGE_RENDER_START: {
            update_weather();
            do_fog();
        }
        default:;
        }
    }

    void reset_weather(const bool cleanup) {
        created_rain = false;

        if (rain_entity && !IsBadReadPtr(rain_entity, 4) && cleanup) {

            if (const auto networkable = rain_entity->get_networkable())
                networkable->release();
        }

        rain_entity = nullptr;
    }

    void update_weather() {

        if (!precipitation) {
            for (auto client_class = interfaces::client_dll->get_all_classes(); client_class && !precipitation;
                 client_class = client_class->next) {
                if (client_class->class_id == CPrecipitation)
                    precipitation = client_class;
            }
        }

        if (!precipitation)
            return;

        if (!created_rain && settings.visuals.world.weather || settings.visuals.world.weather && !last_state) {
            if (created_rain && rain_entity)
                reset_weather();

            if (precipitation->create && precipitation->create(MAX_EDICTS - 1, 0)) {
                rain_entity = reinterpret_cast<c_precipitation_entity *>(interfaces::entity_list->get_entity(MAX_EDICTS - 1));

                rain_entity->get_networkable()->pre_data_update(0);
                rain_entity->get_networkable()->on_pre_data_changed(0);

                rain_entity->get_precip_type() = precipitation_type_t::PRECIPITATION_TYPE_SNOW;
                rain_entity->get_mins() = {-16384.0f, -16384.0f, -16384.0f};
                rain_entity->get_maxs() = {16384.0f, 16384.0f, 16384.0f};

                rain_entity->get_networkable()->on_data_changed(0);
                rain_entity->get_networkable()->post_data_update(0);

                created_rain = true;
            }
        } else if (created_rain && !settings.visuals.world.weather && last_state) {
            reset_weather();
        }

        last_state = settings.visuals.world.weather;
    }

    void do_fog() {

        if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
            return;

        const auto fog_enable = settings.visuals.world.fog;
        const auto fog_length = settings.visuals.world.fog_length;
        const auto fog_color = settings.visuals.world.fog_color;

        static bool old_enable = false;
        static int old_length = 0;
        static color_t old_color;

        for (int i = 1; i < interfaces::entity_list->get_highest_ent_index(); i++) {
            auto fog = reinterpret_cast<c_fog_controller *>(interfaces::entity_list->get_entity(i));

            if (!fog) {
                continue;
            }

            if (fog->get_networkable()->get_client_class()->class_id != CFogController) {
                continue;
            }

            if (fog_enable != old_enable || fog_length != old_length || fog_color != old_color) { // https://i.imgur.com/xD24aJu.jpg
                fog->get_fog_enable() = fog_enable;
                fog->get_fog_start() = 0.f;
                fog->get_fog_end() = float(fog_length) * 100.f;
                fog->get_fog_max_density() = fog_color.a / 100.f;

                fog->get_fog_color_primary() = fog_color.to_u32();
                fog->get_fog_color_secondary() = fog_color.to_u32();

                old_enable = fog_enable;
                old_length = fog_length;
                old_color = fog_color;
            }
        }
    }
} // namespace features::visuals::weather