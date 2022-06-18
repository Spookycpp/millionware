#include "game_events.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"

#include "../../core/util/util.h"
#include "../../engine/logging/logging.h"
#include "../../engine/security/xorstr.h"

namespace features::game_events {
    void on_player_hurt(c_game_event *game_event) {
        const int user_id = interfaces::engine_client->get_player_for_user_id(game_event->get_int(xs("userid")));
        const auto user = (c_player *) interfaces::entity_list->get_entity(user_id);

        if (!user)
            return;

        const int attacker_id = interfaces::engine_client->get_player_for_user_id(game_event->get_int(xs("attacker")));
        const auto attacker = (c_player *) interfaces::entity_list->get_entity(attacker_id);

        if (!attacker)
            return;

        if (user != attacker && attacker == cheat::local_player) {
            switch (settings.miscellaneous.hit_sound) {
            case 1: util::play_sound(xs("survival\\money_collect_05.wav")); break;
            case 2: util::play_sound(xs("buttons\\arena_switch_press_02.wav")); break;
            default: break;
            }
        }
    }

    void on_item_purchase(const item_purchase_data_t &purchase_data) {

        if (!settings.miscellaneous.buy_log)
            return;

        const int ent_id = interfaces::engine_client->get_player_for_user_id(purchase_data.user_id);
        const auto ent = (c_player *) interfaces::entity_list->get_entity(ent_id);

        if (!ent || ent == cheat::local_player || ent->get_team_num() == cheat::local_player->get_team_num())
            return;

        player_info_t info;

        if (!interfaces::engine_client->get_player_info(ent->get_networkable()->index(), info))
            return;

        std::string item = purchase_data.item;

        if (item.find(xs("weapon_")) != std::string::npos)
            item.erase(0, 7);
        else if (item.find(xs("item_")) != std::string::npos)
            item.erase(0, 5);

        logging::info(xs("{} purchased {}"), info.name, item);
    }

    void on_begin_plant(c_game_event *game_event) {

        if (!settings.miscellaneous.bomb_log)
            return;

        const int user_id = interfaces::engine_client->get_player_for_user_id(game_event->get_int(xs("userid")));
        const auto user = (c_player *) interfaces::entity_list->get_entity(user_id);

        if (!user || user == cheat::local_player)
            return;

        player_info_t info;

        if (!interfaces::engine_client->get_player_info(user->get_networkable()->index(), info)) {
            return;
        }

        vector_t origin;

        for (int i = 65; i < interfaces::entity_list->get_highest_ent_index(); ++i) {
            const auto bomb = (c_entity *) interfaces::entity_list->get_entity(i);

            if (!bomb || bomb->get_networkable()->is_dormant())
                continue;

            c_client_class *client_class = bomb->get_networkable()->get_client_class();

            if (client_class && client_class->class_id == CC4) {
                origin = user->get_renderable()->get_render_origin();
                break;
            }
        }

        if (origin.empty())
            return;

        auto player_resource = util::get_player_resource();

        const vector_t bomb_site_center_a = player_resource ? player_resource->get_bomb_site_center_a() : vector_t();
        const vector_t bomb_site_center_b = player_resource ? player_resource->get_bomb_site_center_b() : vector_t();

        if (bomb_site_center_a.empty() || bomb_site_center_b.empty())
            return;

        std::string bomb_site;

        if ((bomb_site_center_a - origin).length() < (bomb_site_center_b - origin).length())
            bomb_site = xs("A");
        else
            bomb_site = xs("B");

        logging::info(xs("{} planting at bombsite {}"), info.name, bomb_site);

        if (settings.miscellaneous.bomb_log_sounds) {
            util::play_sound(xs("weapons\\c4\\c4_initiate.wav"));
        }
    }

    void on_vote_cast(c_game_event *game_event) {
        if (!settings.miscellaneous.vote_reveal)
            return;

        int vote = game_event->get_int(xs("vote_option"));
        int id = game_event->get_int(xs("entityid"));

        player_info_t player_info;
        interfaces::engine_client->get_player_info(id, player_info);

        logging::info(xs("{} voted {}"), player_info.name, std::string(vote == 0 ? xs("yes") : xs("no")));
    }

    void on_player_death(c_game_event *game_event) {
        const int user_id = interfaces::engine_client->get_player_for_user_id(game_event->get_int(xs("userid")));
        const auto user = (c_player *) interfaces::entity_list->get_entity(user_id);

        if (!user)
            return;

        const int attacker_id = interfaces::engine_client->get_player_for_user_id(game_event->get_int(xs("attacker")));
        const auto attacker = (c_player *) interfaces::entity_list->get_entity(attacker_id);

        if (!attacker)
            return;

        if (user != attacker && attacker == cheat::local_player) {

            if (settings.miscellaneous.kill_say) {
                interfaces::engine_client->execute_command(xs("say god i wish i had millionware $$$$"));
            }

            if (settings.visuals.local.kill_effect) {
                cheat::local_player->get_health_shot_boost_time() = interfaces::global_vars->current_time + 0.8f;
            }

            switch (settings.miscellaneous.kill_sound) {
            case 1: util::play_sound(xs("survival\\money_collect_05.wav")); break;
            case 2: util::play_sound(xs("buttons\\arena_switch_press_02.wav")); break;
            default: break;
            }
        }

        if (!settings.miscellaneous.report_player_on_death)
            return;

        if (user != cheat::local_player)
            return;

        if (attacker->get_team_num() == cheat::local_player->get_team_num())
            return;

        player_info_t info;

        if (!interfaces::engine_client->get_player_info(attacker->get_networkable()->index(), info))
            return;

        // xref "textabuse"
        static auto report_player_addr = patterns::report_player;
        static auto report_player_fn = reinterpret_cast<int(__stdcall *)(const char *, const char *)>(report_player_addr);

        if (!report_player_fn)
            return;

        report_player_fn(std::to_string(info.xuid).c_str(), xs("aimbot,wallhack,"));
    }

    void on_player_given_c4(c_game_event *game_event) {

        if (!settings.miscellaneous.bomb_holder_log)
            return;

        player_info_t info;

        const int user_id = interfaces::engine_client->get_player_for_user_id(game_event->get_int(xs("userid")));
        const auto user = (c_player *) interfaces::entity_list->get_entity(user_id);

        if (user == cheat::local_player)
            return;

        if (!interfaces::engine_client->get_player_info(user->get_networkable()->index(), info))
            return;

        logging::info(xs("{} received the bomb"), info.name);
    }

} // namespace features::game_events
