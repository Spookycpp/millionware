#include "damage_logs.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"
#include "../../core/util/util.h"

#include "../../engine/logging/logging.h"
#include "../../engine/security/xorstr.h"

namespace features::damage_logs {
    std::vector<damage_log_data_t> damage_logs = {};

    void on_hurt_event(const damage_log_event_data_t &data) {

        if (!settings.visuals.player.damage_logs) 
            return;

        const int user_id = interfaces::engine_client->get_player_for_user_id(data.user_id);
        const auto user = (c_player *) interfaces::entity_list->get_entity(user_id);

        if (!user)
            return;

        player_info_t info;

        if (!interfaces::engine_client->get_player_info(user->get_networkable()->index(), info)) 
            return;        

        const int attacker_id = interfaces::engine_client->get_player_for_user_id(data.attacker_id);
        const auto attacker = (c_player *) interfaces::entity_list->get_entity(attacker_id);

        if (!attacker) 
            return;
        
        if (user == attacker || attacker != cheat::local_player) 
            return;
        
        damage_logs.emplace_back(damage_log_data_t{data, info.name, 255});

        const auto hit_group_to_str = [](int hit_group) -> std::string {
            switch (hit_group) {
            case HIT_GROUP_GENERIC:
                return XORSTR("body");
            case HIT_GROUP_HEAD:
                return XORSTR("head");
            case HIT_GROUP_CHEST:
                return XORSTR("chest");
            case HIT_GROUP_STOMACH:
                return XORSTR("stomach");
            case HIT_GROUP_LEFT_ARM:
                return XORSTR("left arm");
            case HIT_GROUP_RIGHT_ARM:
                return XORSTR("right arm");
            case HIT_GROUP_LEFT_LEG:
                return XORSTR("left leg");
            case HIT_GROUP_RIGHT_LEG:
                return XORSTR("right leg");
            case HIT_GROUP_GEAR:
                return XORSTR("gear");
            }

            return XORSTR("u missed dat nigga fr");
        };

        logging::info("hurt {} in the {} for {} damage | {} health remaining", info.name, hit_group_to_str(data.hit_group), data.dmg_health, data.health);
    }

    std::vector<damage_log_data_t> get_damage_logs() {
        return damage_logs;
    }

    void clear_damage_logs() {
        if (damage_logs.empty())
            return;
        
        for (size_t i = 0; i < damage_logs.size(); ++i) {
            auto &damage_log = damage_logs.at(i);

            if (damage_log.alpha <= 0) {
                damage_logs.erase(damage_logs.begin() + i);
                continue;
            }

            damage_log.alpha -= static_cast<int>(255.0f / 3.0f * interfaces::global_vars->frame_time);
        }
    }
} // namespace features::damage_logs