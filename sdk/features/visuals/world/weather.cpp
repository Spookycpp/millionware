#include "weather.h"

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"

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
                    rain_entity->get_collideable()->get_mins() = {-16384.0f, -16384.0f, -16384.0f};
                    rain_entity->get_collideable()->get_maxs() = {16384.0f, 16384.0f, 16384.0f};

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
} // namespace features::visuals::weather