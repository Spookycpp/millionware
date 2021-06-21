#include "damage logs/damage_logs.h"
#include "game_events.h"

#include "../../core/interfaces/interfaces.h"

#include "../../engine/logging/logging.h"
#include "../../engine/security/xorstr.h"

#include "../../core/cheat/cheat.h"
#include "../../lua/lua_game.hpp"

// for game event list: https://wiki.alliedmods.net/Counter-Strike:_Global_Offensive_Events
// this also has list of the parameters used

c_event_listener::c_event_listener() {
    interfaces::game_events->add_listener(this, XORSTR("player_footstep"), false);
    interfaces::game_events->add_listener(this, XORSTR("player_hurt"), false);
    interfaces::game_events->add_listener(this, XORSTR("item_purchase"), false);
    interfaces::game_events->add_listener(this, XORSTR("inferno_startburn"), false);
    interfaces::game_events->add_listener(this, XORSTR("inferno_extinguish"), false);
    interfaces::game_events->add_listener(this, XORSTR("smokegrenade_detonate"), false);
    interfaces::game_events->add_listener(this, XORSTR("bomb_beginplant"), false);
    interfaces::game_events->add_listener(this, XORSTR("bomb_begindefuse"), false);
    interfaces::game_events->add_listener(this, XORSTR("bomb_abortdefuse"), false);
    interfaces::game_events->add_listener(this, XORSTR("hostage_follows"), false);
    interfaces::game_events->add_listener(this, XORSTR("vote_cast"), false);
    interfaces::game_events->add_listener(this, XORSTR("player_death"), false);
    interfaces::game_events->add_listener(this, XORSTR("round_start"), false);
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
    else if (std::strncmp(game_event->get_name(), XORSTR("player_hurt"), 12) == 0) {
        features::game_events::on_player_hurt(game_event);

        // clang-format off
        const damage_log_event_data_t data{game_event->get_int(XORSTR("userid")),
            game_event->get_int(XORSTR("attacker")),
            game_event->get_int(XORSTR("health")),
            game_event->get_int(XORSTR("armor")),
            game_event->get_string(XORSTR("weapon")),
            game_event->get_int(XORSTR("dmg_health")),
            game_event->get_int(XORSTR("dmg_armor")),
            game_event->get_int(XORSTR("hitgroup"))};
        // clang-format on
        features::damage_logs::on_hurt_event(data);
    }
    else if (std::strncmp(game_event->get_name(), XORSTR("vote_cast"), 10) == 0) {
        features::game_events::on_vote_cast(game_event);
    }
    else if (std::strncmp(game_event->get_name(), XORSTR("round_start"), 10) == 0) {
        cheat::should_clear_death_notices = true;
    }

    lua::callbacks::run_events(game_event);
}

bool c_event_listener::exists(const char *listener) {
    return interfaces::game_events->find_listener(this, listener);
}

bool c_event_listener::add(const char *listener) {
    return interfaces::game_events->add_listener(this, listener, false);
}
