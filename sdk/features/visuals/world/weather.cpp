#include "weather.h"

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"

namespace features::visuals::weather {
    void on_frame_stage_notify(e_client_frame_stage frame_stage) {

        switch (frame_stage) {

            case e_client_frame_stage::FRAME_STAGE_RENDER_START: {
               
            }
        }
    }

    void reset_weather() {
        if (!cheat::created_rain)
            return;

        for (int i = 0; i <= interfaces::entity_list->get_highest_ent_index(); i++) {
            c_entity *ent = (c_entity *) interfaces::entity_list->get_entity(i);

            if (!ent)
                continue;

            const c_client_class *client_class = ent->get_networkable()->get_client_class();

            if (!client_class)
                continue;

            if (client_class->class_id == CPrecipitation) {
                if (ent->get_networkable())
                    ent->get_networkable()->release();
            }
        }
    }

    void update_weather() {
        static auto rain_entity = (c_precipitation_entity *) nullptr;

        if (!settings.visuals.world.weather)
            return;

        if (cheat::created_rain)
            return;

        static c_client_class *precipitation = nullptr;

        if (!precipitation) {

			for (auto client_class = interfaces::client_dll->get_all_classes(); client_class && !precipitation; client_class = client_class->next) {
                if (client_class->class_id == CPrecipitation) 
                    precipitation = client_class;
            }
        }

        if (precipitation && precipitation->create) {

            c_client_networkable *rain_networkable = ((c_client_networkable * (*) (int, int) ) precipitation->create)(MAX_EDICTS - 1, 0);
            if (!rain_networkable)
                return;

            if (rain_networkable)
                rain_entity = (c_precipitation_entity *) interfaces::entity_list->get_entity(MAX_EDICTS - 1);

            //c_entity *rain_unknown = ((c_renderable *) rain_networkable)->GetIClientUnknown();
            // if (!rain_unknown) 
            //    return;

            if (!rain_entity->get_networkable())
                return;

            rain_networkable->pre_data_update(0);
            rain_networkable->on_data_changed(0);

            // null da callbacks
            static const auto r_RainRadius = interfaces::convar_system->find_convar(XORSTR("r_RainRadius"));
            r_RainRadius->callbacks.clear();

            // limit the render distance of da rain
            if (r_RainRadius->get_float() != 1000.f)
                r_RainRadius->set_value(1000.f);

            // only PRECIPITATION_TYPE_RAIN and PRECIPITATION_TYPE_SNOW work..?
            rain_entity->get_precip_type() = precipitation_type_t::PRECIPITATION_TYPE_SNOW;
            rain_entity->get_collideable()->get_mins() = vector_t(-32768.0f, -32768.0f, -32768.0f);
            rain_entity->get_collideable()->get_maxs() = vector_t(32768.0f, 32768.0f, 32768.0f);

            rain_entity->get_networkable()->on_post_data_change(0);
            rain_entity->get_networkable()->post_data_update(0);

            cheat::created_rain = true;
        }
    }
}