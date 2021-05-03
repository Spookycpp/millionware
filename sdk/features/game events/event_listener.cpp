#include "game_events.h"

#include "../../core/interfaces/interfaces.h"

#include "../../engine/logging/logging.h"
#include "../../engine/security/xorstr.h"

// for game event list: https://wiki.alliedmods.net/Counter-Strike:_Global_Offensive_Events
// this also has list of the parameters used

c_event_listener::c_event_listener() {
    interfaces::game_events->add_listener(this, XORSTR("item_purchase"), false);
    interfaces::game_events->add_listener(this, XORSTR("bomb_beginplant"), false);
    interfaces::game_events->add_listener(this, XORSTR("player_death"), false);
}

c_event_listener::~c_event_listener() {
    interfaces::game_events->remove_listener(this);
}

void c_event_listener::on_fired_game_event(c_game_event *game_event) {

    if (std::strncmp(game_event->get_name(), XORSTR("item_purchase"), 14) == 0) {

        const item_purchase_data_t data{
            game_event->get_int(XORSTR("userid")),
            game_event->get_string(XORSTR("weapon")),
        };

        features::game_events::on_item_purchase(data);
    }
    else if (std::strncmp(game_event->get_name(), XORSTR("bomb_beginplant"), 16) == 0) {
        features::game_events::on_begin_plant(game_event);
    }
    else if (std::strncmp(game_event->get_name(), XORSTR("player_death"), 13) == 0) {
        features::game_events::on_player_death(game_event);
    }
}