#include <filesystem>
#include <fstream>
#include <ostream>

#include <nlohmann/json.hpp>

#include "../../engine/logging/logging.h"
#include "../../engine/security/xorstr.h"
#include "settings.h"

// clang-format off
#define MAKE_LEGIT_BOT(weapon)                                                                                                                                           \
    config_item_t{settings.##weapon.enabled, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("enabled")},                                                              \
    config_item_t{settings.##weapon.hotkey, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("hotkey")},                                                                 \
    config_item_t{settings.##weapon.check_visible, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("check_visible")},                                                  \
    config_item_t{settings.##weapon.check_team, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("check_team")},                                                        \
    config_item_t{settings.##weapon.check_flashed, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("check_flashed")},                                                  \
    config_item_t{settings.##weapon.check_smoked, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("check_smoked")},                                                    \
    config_item_t{settings.##weapon.hitbox_method, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("hitbox_method")},                                                   \
    config_item_t{settings.##weapon.hitbox, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("hitbox")},                                                                 \
    config_item_t{settings.##weapon.target_backtrack, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("target_backtrack")},                                            \
    config_item_t{settings.##weapon.start_bullets, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("start_bullets")},                                                   \
    config_item_t{settings.##weapon.fov, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("fov")},                                                                     \
    config_item_t{settings.##weapon.speed, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("speed")},                                                                 \
    config_item_t{settings.##weapon.speed_exponent, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("speed_exponent")},                                               \
    config_item_t{settings.##weapon.rcs_x, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("rcs_x")},                                                                 \
    config_item_t{settings.##weapon.rcs_y, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("rcs_y")},                                                                 \
    config_item_t{settings.##weapon.flick_bot.enabled, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("flick_bot"), xs("enabled")},                                    \
    config_item_t{settings.##weapon.flick_bot.fov, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("flick_bot"), xs("fov")},                                          \
    config_item_t{settings.##weapon.flick_bot.hit_chance, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("flick_bot"), xs("hit_chance")},                              \
    config_item_t{settings.##weapon.assist.enabled, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("assist"), xs("enabled")},                                         \
    config_item_t{settings.##weapon.assist.fov, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("assist"), xs("fov")},                                                \
    config_item_t{settings.##weapon.assist.strength, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("assist"), xs("strength")},                                      \
    config_item_t{settings.##weapon.backtrack.enabled, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("backtrack"), xs("enabled")},                                   \
    config_item_t{settings.##weapon.backtrack.fov, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("backtrack"), xs("fov")},                                          \
    config_item_t{settings.##weapon.backtrack.time, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("backtrack"), xs("time")},                                          \
    config_item_t{settings.##weapon.smoothing.enabled, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("smoothing"), xs("enabled")},                                   \
    config_item_t{settings.##weapon.smoothing.samples, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("smoothing"), xs("samples")},                                    \
    config_item_t{settings.##weapon.smoothing.factor, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("smoothing"), xs("factor")},                                    \
    config_item_t{settings.##weapon.rcs.enabled, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("rcs"), xs("enabled")},                                               \
    config_item_t{settings.##weapon.rcs.x, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("rcs"), xs("x")},                                                          \
    config_item_t{settings.##weapon.rcs.y, CONFIG_ITEM_FLOAT, xs("legitbot"), xs(#weapon), xs("rcs"), xs("y")},                                                          \
    config_item_t{settings.##weapon.triggerbot.enabled, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("triggerbot"), xs("enabled")},                                 \
    config_item_t{settings.##weapon.triggerbot.hotkey, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("triggerbot"), xs("hotkey")},                                    \
    config_item_t{settings.##weapon.triggerbot.check_team, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("triggerbot"), xs("check_team")},                           \
    config_item_t{settings.##weapon.triggerbot.check_flashed, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("triggerbot"), xs("check_flashed")},                     \
    config_item_t{settings.##weapon.triggerbot.check_smoked, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("triggerbot"), xs("check_smoked")},                       \
    config_item_t{settings.##weapon.triggerbot.hit_chance, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("triggerbot"), xs("hit_chance")},                            \
    config_item_t{settings.##weapon.triggerbot.delay, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("triggerbot"), xs("delay")},                                      \
    config_item_t{settings.##weapon.triggerbot.backtrack.enabled, CONFIG_ITEM_BOOL, xs("legitbot"), xs(#weapon), xs("triggerbot"), xs("backtrack"), xs("enabled")},      \
    config_item_t{settings.##weapon.triggerbot.backtrack.time, CONFIG_ITEM_INT, xs("legitbot"), xs(#weapon), xs("triggerbot"), xs("backtrack"), xs("time")}
// clang-format on

settings_t settings = {};
settings_t::legitbot_t *settings_lbot = &settings.lbot_global;

enum config_item_type_t {
    CONFIG_ITEM_BOOL,
    CONFIG_ITEM_INT,
    CONFIG_ITEM_FLOAT,
    CONFIG_ITEM_COLOR,
    CONFIG_ITEM_STRING,
};

struct config_item_t {
    std::string category;
    std::string second_category;
    std::string third_category;
    std::string fourth_category;
    std::string name;

    config_item_type_t type;

    void *pointer;

    template <typename T>
    config_item_t(T &pointer, config_item_type_t type, std::string category, std::string name) {
        this->pointer = (void *) &pointer;
        this->type = type;
        this->category = category;
        this->name = name;
    }

    template <typename T>
    config_item_t(T &pointer, config_item_type_t type, std::string category, std::string second_category, std::string name) {
        this->pointer = (void *) &pointer;
        this->type = type;
        this->category = category;
        this->second_category = second_category;
        this->name = name;
    }

    template <typename T>
    config_item_t(T &pointer, config_item_type_t type, std::string category, std::string second_category, std::string third_category,
                  std::string name) {
        this->pointer = (void *) &pointer;
        this->type = type;
        this->category = category;
        this->second_category = second_category;
        this->third_category = third_category;
        this->name = name;
    }

    template <typename T>
    config_item_t(T &pointer, config_item_type_t type, std::string category, std::string second_category, std::string third_category,
                  std::string fourth_category, std::string name) {
        this->pointer = (void *) &pointer;
        this->type = type;
        this->category = category;
        this->second_category = second_category;
        this->third_category = third_category;
        this->fourth_category = fourth_category;
        this->name = name;
    }
};

// clang-format off
static config_item_t items[] = {
    // Global
    config_item_t{settings.global.accent_color, CONFIG_ITEM_COLOR, xs("global"), xs("accent_color")},
    config_item_t{settings.global.weapon_groups, CONFIG_ITEM_BOOL, xs("global"), xs("weapon_groups")},
    config_item_t{settings.global.debug_overlay, CONFIG_ITEM_BOOL, xs("global"), xs("debug_overlay")},
    // Legit bot
    MAKE_LEGIT_BOT(lbot_global),
    MAKE_LEGIT_BOT(lbot_pistols),
    MAKE_LEGIT_BOT(lbot_hpistols),
    MAKE_LEGIT_BOT(lbot_rifles),
    MAKE_LEGIT_BOT(lbot_smg),
    MAKE_LEGIT_BOT(lbot_shotgun),
    MAKE_LEGIT_BOT(lbot_awp),
    MAKE_LEGIT_BOT(lbot_scout),
    MAKE_LEGIT_BOT(lbot_auto),
    MAKE_LEGIT_BOT(lbot_other),
    // Ragebot
    config_item_t{settings.ragebot.enabled, CONFIG_ITEM_BOOL, xs("ragebot"), xs("enabled")},
    config_item_t{settings.ragebot.hotkey, CONFIG_ITEM_INT, xs("ragebot"), xs("hotkey")},
    config_item_t{settings.ragebot.check_visible, CONFIG_ITEM_BOOL, xs("ragebot"), xs("check_visible")},
    config_item_t{settings.ragebot.check_team, CONFIG_ITEM_BOOL, xs("ragebot"), xs("check_team")},
    config_item_t{settings.ragebot.auto_aim, CONFIG_ITEM_BOOL, xs("ragebot"), xs("auto_aim")},
    config_item_t{settings.ragebot.auto_shoot, CONFIG_ITEM_BOOL, xs("ragebot"), xs("auto_shoot")},
    config_item_t{settings.ragebot.silent_aim, CONFIG_ITEM_BOOL, xs("ragebot"), xs("silent_aim")},
    config_item_t{settings.ragebot.no_recoil, CONFIG_ITEM_BOOL, xs("ragebot"), xs("no_recoil")},
    config_item_t{settings.ragebot.target_method, CONFIG_ITEM_INT, xs("ragebot"), xs("target_method")},
    config_item_t{settings.ragebot.hitbox_method, CONFIG_ITEM_INT, xs("ragebot"), xs("hitbox_method")},
    config_item_t{settings.ragebot.hitbox, CONFIG_ITEM_INT, xs("ragebot"), xs("hitbox")},
    config_item_t{settings.ragebot.fov, CONFIG_ITEM_FLOAT, xs("ragebot"), xs("fov")},
    // Visuals
    config_item_t{settings.visuals.stream_proof, CONFIG_ITEM_BOOL, xs("visuals"), xs("stream_proof")},
    // Visuals player
    config_item_t{settings.visuals.player.activation_type, CONFIG_ITEM_INT, xs("visuals"), xs("player"), xs("activation_type")},
    config_item_t{settings.visuals.player.draw_teammates, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("draw_teammates")},
    config_item_t{settings.visuals.player.hotkey, CONFIG_ITEM_INT, xs("visuals"), xs("player"), xs("hotkey")},
    config_item_t{settings.visuals.player.engine_radar, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("engine_radar")},
    config_item_t{settings.visuals.player.bounding_box, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("bounding_box")},
    config_item_t{settings.visuals.player.bounding_box_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("player"), xs("bounding_box_color")},
    config_item_t{settings.visuals.player.player_name, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("player_name")},
    config_item_t{settings.visuals.player.player_name_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("player"), xs("player_name_color")},
    config_item_t{settings.visuals.player.health, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("health")},
    config_item_t{settings.visuals.player.weapon, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("weapon")},
    config_item_t{settings.visuals.player.ammo, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("ammo")},
    config_item_t{settings.visuals.player.ammo_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("player"), xs("ammo_color")},
    config_item_t{settings.visuals.player.armor, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("armor")},
    config_item_t{settings.visuals.player.flags, CONFIG_ITEM_INT, xs("visuals"), xs("player"), xs("flags")},
    config_item_t{settings.visuals.player.skeleton, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("skeleton")},
    config_item_t{settings.visuals.player.skeleton_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("player"), xs("skeleton_color")},
    config_item_t{settings.visuals.player.head_spot, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("head_spot")},
    config_item_t{settings.visuals.player.head_spot_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("player"), xs("head_spot_color")},
    config_item_t{settings.visuals.player.glow, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("glow")},
    config_item_t{settings.visuals.player.glow_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("player"), xs("glow_color")},
    config_item_t{settings.visuals.player.damage_logs, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("damage_logs")},
    config_item_t{settings.visuals.player.outside_fov, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("outside_fov")},
    config_item_t{settings.visuals.player.outside_fov_radius, CONFIG_ITEM_FLOAT, xs("visuals"), xs("player"), xs("outside_fov_radius")},
    config_item_t{settings.visuals.player.outside_fov_size, CONFIG_ITEM_INT, xs("visuals"), xs("player"), xs("outside_fov_size")},
    config_item_t{settings.visuals.player.footsteps, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("footsteps")},
    config_item_t{settings.visuals.player.footsteps_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("player"), xs("footsteps_color")},
    // Visuals chams
    config_item_t{settings.visuals.player.chams.material, CONFIG_ITEM_INT, xs("visuals"), xs("player"), xs("chams"), xs("material")},
    config_item_t{settings.visuals.player.chams.visible, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("chams"), xs("visible")},
    config_item_t{settings.visuals.player.chams.visible_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("player"), xs("chams"), xs("visible_color")},
    config_item_t{settings.visuals.player.chams.invisible, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("chams"), xs("invisible")},
    config_item_t{settings.visuals.player.chams.invisible_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("player"), xs("chams"), xs("invisible_color")},
    config_item_t{settings.visuals.player.chams.smoke, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("chams"), xs("smoke")},
    config_item_t{settings.visuals.player.chams.backtrack, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("chams"), xs("backtrack")},
    config_item_t{settings.visuals.player.chams.glow, CONFIG_ITEM_BOOL, xs("visuals"), xs("player"), xs("chams"), xs("glow")},
    config_item_t{settings.visuals.player.chams.glow_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("player"), xs("chams"), xs("glow_color")},
    // Visuals world
    config_item_t{settings.visuals.world.skybox, CONFIG_ITEM_INT, xs("visuals"), xs("world"), xs("skybox")},
    config_item_t{settings.visuals.world.nightmode, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("nightmode")},
    config_item_t{settings.visuals.world.nightmode_darkness, CONFIG_ITEM_FLOAT, xs("visuals"), xs("world"), xs("nightmode_darkness")},
    config_item_t{settings.visuals.world.fullbright, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("fullbright")},
    config_item_t{settings.visuals.world.fog, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("fog")},
    config_item_t{settings.visuals.world.fog_length, CONFIG_ITEM_INT, xs("visuals"), xs("world"), xs("fog_length")},
    config_item_t{settings.visuals.world.fog_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("world"), xs("fog_color")},
    config_item_t{settings.visuals.world.weapon, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("weapon")},
    config_item_t{settings.visuals.world.weapon_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("world"), xs("weapon_color")},
    config_item_t{settings.visuals.world.grenades, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("grenades")},
    config_item_t{settings.visuals.world.grenades_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("world"), xs("grenades_color")},
    config_item_t{settings.visuals.world.molotov_spread, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("molotov_spread")},
    config_item_t{settings.visuals.world.molotov_spread_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("world"), xs("molotov_spread_color")},
    config_item_t{settings.visuals.world.planted_bomb, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("planted_bomb")},
    config_item_t{settings.visuals.world.planted_bomb_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("world"), xs("planted_bomb_color")},
    config_item_t{settings.visuals.world.dropped_bomb, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("dropped_bomb")},
    config_item_t{settings.visuals.world.dropped_bomb_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("world"), xs("dropped_bomb_color")},
    config_item_t{settings.visuals.world.defusal_kit, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("defusal_kit")},
    config_item_t{settings.visuals.world.defusal_kit_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("world"), xs("defusal_kit_color")},
    config_item_t{settings.visuals.world.chicken, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("chicken")},
    config_item_t{settings.visuals.world.remove_fog, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("remove_fog")},
    config_item_t{settings.visuals.world.weather, CONFIG_ITEM_BOOL, xs("visuals"), xs("world"), xs("weather")},
    // Visuals local
    config_item_t{settings.visuals.local.recoil_crosshair, CONFIG_ITEM_INT, xs("visuals"), xs("local"), xs("recoil_crosshair")},
    config_item_t{settings.visuals.local.penetration_crosshair, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("penetration_crosshair")},
    config_item_t{settings.visuals.local.sniper_crosshair, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("sniper_crosshair")},
    config_item_t{settings.visuals.local.grenade_prediction, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("grenade_prediction")},
    config_item_t{settings.visuals.local.grenade_prediction_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("local"), xs("grenade_prediction_color")},
    config_item_t{settings.visuals.local.spectator_list, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("spectator_list")},
    config_item_t{settings.visuals.local.kill_effect, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("kill_effect")},
    config_item_t{settings.visuals.local.viewmodel_offset, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("viewmodel_offset")},
    config_item_t{settings.visuals.local.disable_post_processing, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("disable_post_processing")},
    config_item_t{settings.visuals.local.disable_panorama_blur, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("disable_panorama_blur")},
    config_item_t{settings.visuals.local.indicators, CONFIG_ITEM_INT, xs("visuals"), xs("local"), xs("indicators")},
    config_item_t{settings.visuals.local.velocity_takeoff, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("velocity_takeoff")},
    config_item_t{settings.visuals.local.velocity_color_1, CONFIG_ITEM_COLOR, xs("visuals"), xs("local"), xs("velocity_color_1")},
    config_item_t{settings.visuals.local.velocity_color_2, CONFIG_ITEM_COLOR, xs("visuals"), xs("local"), xs("velocity_color_2")},
    config_item_t{settings.visuals.local.velocity_color_3, CONFIG_ITEM_COLOR, xs("visuals"), xs("local"), xs("velocity_color_3")},
    config_item_t{settings.visuals.local.jb_indicator, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("jb_indicator")},
    config_item_t{settings.visuals.local.eb_indicator, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("eb_indicator")},
    config_item_t{settings.visuals.local.ej_indicator, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("ej_indicator")},
    config_item_t{settings.visuals.local.mj_indicator, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("mj_indicator")},
    config_item_t{settings.visuals.local.flash_alpha, CONFIG_ITEM_INT, xs("visuals"), xs("local"), xs("flash_alpha")},
    config_item_t{settings.visuals.local.aspect_ratio, CONFIG_ITEM_FLOAT, xs("visuals"), xs("local"), xs("aspect_ratio")},
    config_item_t{settings.visuals.local.override_fov, CONFIG_ITEM_INT, xs("visuals"), xs("local"), xs("override_fov")},
    config_item_t{settings.visuals.local.viewmodel_offset_x, CONFIG_ITEM_FLOAT, xs("visuals"), xs("local"), xs("viewmodel_offset_x")},
    config_item_t{settings.visuals.local.viewmodel_offset_y, CONFIG_ITEM_FLOAT, xs("visuals"), xs("local"), xs("viewmodel_offset_y")},
    config_item_t{settings.visuals.local.viewmodel_offset_z, CONFIG_ITEM_FLOAT, xs("visuals"), xs("local"), xs("viewmodel_offset_z")},
    config_item_t{settings.visuals.local.viewmodel_offset_r, CONFIG_ITEM_FLOAT, xs("visuals"), xs("local"), xs("viewmodel_offset_r")},
    config_item_t{settings.visuals.local.feet_fx, CONFIG_ITEM_INT, xs("visuals"), xs("local"), xs("feet_fx")},
    config_item_t{settings.visuals.local.data_graph, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("data_graph")},
    config_item_t{settings.visuals.local.data_graph_info, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("data_graph_info")},
    config_item_t{settings.visuals.local.data_graph_background, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("data_graph_background")},
    config_item_t{settings.visuals.local.data_graph_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("local"), xs("data_graph_color")},
    config_item_t{settings.visuals.local.data_graph_bg_color, CONFIG_ITEM_COLOR, xs("visuals"), xs("local"), xs("data_graph_bg_color")},
    // Visuals local third person
    config_item_t{settings.visuals.local.third_person.enabled, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("third_person"), xs("enabled")},
    config_item_t{settings.visuals.local.third_person.first_person_on_nade, CONFIG_ITEM_BOOL, xs("visuals"), xs("local"), xs("third_person"), xs("first_person_on_nade")},
    config_item_t{settings.visuals.local.third_person.hotkey, CONFIG_ITEM_INT, xs("visuals"), xs("local"), xs("third_person"), xs("hotkey")},
    config_item_t{settings.visuals.local.third_person.distance, CONFIG_ITEM_FLOAT, xs("visuals"), xs("local"), xs("third_person"), xs("distance")},
    // Miscellaneous
    config_item_t{settings.miscellaneous.player_privacy, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("player_privacy")},
    config_item_t{settings.miscellaneous.auto_pistol, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("auto_pistol")},
    config_item_t{settings.miscellaneous.clantag, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("clantag")},
    config_item_t{settings.miscellaneous.rank_reveal, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("rank_reveal")},
    config_item_t{settings.miscellaneous.money_reveal, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("money_reveal")},
    config_item_t{settings.miscellaneous.preserve_killfeed, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("preserve_killfeed")},
    config_item_t{settings.miscellaneous.auto_accept, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("auto_accept")},
    config_item_t{settings.miscellaneous.reveal_overwatch_suspect, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("reveal_overwatch_suspect")},
    config_item_t{settings.miscellaneous.unlock_hidden_convars, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("unlock_hidden_convars")},
    config_item_t{settings.miscellaneous.unlock_inventory, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("unlock_inventory")},
    config_item_t{settings.miscellaneous.ragdoll_push, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("ragdoll_push")},
    config_item_t{settings.miscellaneous.ragdoll_float, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("ragdoll_float")},
    config_item_t{settings.miscellaneous.buy_log, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("buy_log")},
    config_item_t{settings.miscellaneous.bomb_log, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("bomb_log")},
    config_item_t{settings.miscellaneous.bomb_log_sounds, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("bomb_log_sounds")},
    config_item_t{settings.miscellaneous.bomb_holder_log, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("bomb_holder_log")},
    config_item_t{settings.miscellaneous.report_player_on_death, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("report_player_on_death")},
    config_item_t{settings.miscellaneous.vote_reveal, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("vote_reveal")},
    config_item_t{settings.miscellaneous.discord_rpc, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("discord_rpc")},
    config_item_t{settings.miscellaneous.kill_say, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("kill_say")},
    config_item_t{settings.miscellaneous.hit_sound, CONFIG_ITEM_INT, xs("miscellaneous"), xs("hit_sound")},
    config_item_t{settings.miscellaneous.kill_sound, CONFIG_ITEM_INT, xs("miscellaneous"), xs("kill_sound")},
    config_item_t{settings.miscellaneous.server_regions, CONFIG_ITEM_INT, xs("miscellaneous"), xs("server_regions")},
    config_item_t{settings.miscellaneous.hit_sound_custom, CONFIG_ITEM_STRING, xs("miscellaneous"), xs("hit_sound_custom")},
    config_item_t{settings.miscellaneous.kill_sound_custom, CONFIG_ITEM_STRING, xs("miscellaneous"), xs("kill_sound_custom")},
    // Miscellaneous movement
    config_item_t{settings.miscellaneous.movement.bunny_hop, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("bunny_hop")},
    config_item_t{settings.miscellaneous.movement.jump_bug, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("jump_bug")},
    config_item_t{settings.miscellaneous.movement.jump_bug_hotkey, CONFIG_ITEM_INT, xs("miscellaneous"), xs("movement"), xs("jump_bug_hotkey")},
    config_item_t{settings.miscellaneous.movement.edge_bug, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("edge_bug")},
    config_item_t{settings.miscellaneous.movement.edge_bug_hotkey, CONFIG_ITEM_INT, xs("miscellaneous"), xs("movement"), xs("edge_bug_hotkey")},
    config_item_t{settings.miscellaneous.movement.edge_jump, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("edge_jump")},
    config_item_t{settings.miscellaneous.movement.edge_jump_hotkey, CONFIG_ITEM_INT, xs("miscellaneous"), xs("movement"), xs("edge_jump_hotkey")},
    config_item_t{settings.miscellaneous.movement.edge_jump_duck, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("edge_jump_duck")},
    config_item_t{settings.miscellaneous.movement.edge_jump_duck_hotkey, CONFIG_ITEM_INT, xs("miscellaneous"), xs("movement"), xs("edge_jump_duck_hotkey")},
    config_item_t{settings.miscellaneous.movement.long_jump, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("long_jump")},
    config_item_t{settings.miscellaneous.movement.long_jump_hotkey, CONFIG_ITEM_INT, xs("miscellaneous"), xs("movement"), xs("long_jump_hotkey")},
    config_item_t{settings.miscellaneous.movement.edge_jump_lj_bind, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("edge_jump_lj_bind")},
    config_item_t{settings.miscellaneous.movement.edge_jump_lj_bind_hotkey, CONFIG_ITEM_INT, xs("miscellaneous"), xs("movement"), xs("edge_jump_lj_bind_hotkey")},
    config_item_t{settings.miscellaneous.movement.edge_bug_assist, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("edge_bug_assist")},
    config_item_t{settings.miscellaneous.movement.edge_bug_assist_hotkey, CONFIG_ITEM_INT, xs("miscellaneous"), xs("movement"), xs("edge_bug_assist_hotkey")},
    config_item_t{settings.miscellaneous.movement.edge_bug_radius, CONFIG_ITEM_INT, xs("miscellaneous"), xs("movement"), xs("edge_bug_radius")},
    config_item_t{settings.miscellaneous.movement.edgebug_rage_amount, CONFIG_ITEM_FLOAT, xs("miscellaneous"), xs("movement"), xs("edgebug_rage_amount")},
    config_item_t{settings.miscellaneous.movement.auto_strafe, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("auto_strafe")},
    config_item_t{settings.miscellaneous.movement.air_duck, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("air_duck")},
    config_item_t{settings.miscellaneous.movement.no_duck_cooldown, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("no_duck_cooldown")},
    config_item_t{settings.miscellaneous.movement.slide_walk, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("slide_walk")},
    config_item_t{settings.miscellaneous.movement.strafe_optimizer, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("strafe_optimizer")},
    config_item_t{settings.miscellaneous.movement.strafe_optimizer_key, CONFIG_ITEM_INT, xs("miscellaneous"), xs("movement"), xs("strafe_optimizer_key")},
    config_item_t{settings.miscellaneous.movement.strafe_optimizer_max_gain, CONFIG_ITEM_FLOAT, xs("miscellaneous"), xs("movement"), xs("strafe_optimizer_max_gain")},
    config_item_t{settings.miscellaneous.movement.strafe_optimizer_pull_amount, CONFIG_ITEM_FLOAT, xs("miscellaneous"), xs("movement"), xs("strafe_optimizer_pull_amount")},
    config_item_t{settings.miscellaneous.movement.strafe_optimizer_max_velocity, CONFIG_ITEM_INT, xs("miscellaneous"), xs("movement"), xs("strafe_optimizer_max_velocity")},
    config_item_t{settings.miscellaneous.movement.blockbot, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("movement"), xs("blockbot")},
    config_item_t{settings.miscellaneous.movement.blockbot_key, CONFIG_ITEM_INT, xs("miscellaneous"), xs("movement"), xs("blockbot_key")},
    // Miscellaneous fake lag
    config_item_t{settings.miscellaneous.fake_lag.type, CONFIG_ITEM_INT, xs("miscellaneous"), xs("fake_lag"), xs("type")},
    config_item_t{settings.miscellaneous.fake_lag.ticks, CONFIG_ITEM_INT, xs("miscellaneous"), xs("fake_lag"), xs("ticks")},
    config_item_t{settings.miscellaneous.fake_lag.fluctuate, CONFIG_ITEM_INT, xs("miscellaneous"), xs("fake_lag"), xs("fluctuate")},
    config_item_t{settings.miscellaneous.fake_lag.in_air, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("fake_lag"), xs("in_air")},
    config_item_t{settings.miscellaneous.fake_lag.on_peek, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("fake_lag"), xs("on_peek")},
    config_item_t{settings.miscellaneous.fake_lag.on_peek_ticks, CONFIG_ITEM_INT, xs("miscellaneous"), xs("fake_lag"), xs("on_peek_ticks")},
    config_item_t{settings.miscellaneous.fake_lag.on_peek_alert, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("fake_lag"), xs("on_peek_alert")},
    config_item_t{settings.miscellaneous.fake_lag.on_peek_minimum_speed, CONFIG_ITEM_FLOAT, xs("miscellaneous"), xs("fake_lag"), xs("on_peek_minimum_speed")},
    config_item_t{settings.miscellaneous.fake_lag.on_move, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("fake_lag"), xs("on_move")},
    config_item_t{settings.miscellaneous.fake_lag.on_attack, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("fake_lag"), xs("on_attack")},
    config_item_t{settings.miscellaneous.fake_lag.avoid_ground, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("fake_lag"), xs("avoid_ground")},
    config_item_t{settings.miscellaneous.fake_lag.jump_reset, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("fake_lag"), xs("jump_reset")},
    config_item_t{settings.miscellaneous.fake_lag.on_duck, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("fake_lag"), xs("on_duck")},
    config_item_t{settings.miscellaneous.fake_lag.choke_indicator, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("fake_lag"), xs("choke_indicator")},
    // Miscellaneous fake ping
    config_item_t{settings.miscellaneous.fake_ping.enabled, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("fake_ping"), xs("enabled")},
    config_item_t{settings.miscellaneous.fake_ping.max_ping, CONFIG_ITEM_FLOAT, xs("miscellaneous"), xs("fake_ping"), xs("max_ping")},
    // Miscellaneous legit AA
    config_item_t{settings.miscellaneous.legit_aa.enabled, CONFIG_ITEM_BOOL, xs("settings"), xs("miscellaneous"), xs("legit_aa"), xs("enabled")},
    config_item_t{settings.miscellaneous.legit_aa.balance, CONFIG_ITEM_BOOL, xs("settings"), xs("miscellaneous"), xs("legit_aa"), xs("balance")},
    config_item_t{settings.miscellaneous.legit_aa.swap_sides_hotkey, CONFIG_ITEM_INT, xs("settings"), xs("miscellaneous"), xs("legit_aa"), xs("swap_sides_hotkey")},
    config_item_t{settings.miscellaneous.legit_aa.range, CONFIG_ITEM_INT, xs("settings"), xs("miscellaneous"), xs("legit_aa"), xs("range")},
    config_item_t{settings.miscellaneous.legit_aa.desync_indicator, CONFIG_ITEM_BOOL, xs("settings"), xs("miscellaneous"), xs("legit_aa"), xs("desync_indicator")},
    // Miscellaneous rapid lag
    config_item_t{settings.miscellaneous.rapid_lag.enabled, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("rapid_lag"), xs("enabled")},
    config_item_t{settings.miscellaneous.rapid_lag.hotkey_hold, CONFIG_ITEM_INT, xs("miscellaneous"), xs("rapid_lag"), xs("hotkey_hold")},
    config_item_t{settings.miscellaneous.rapid_lag.hotkey_toggle, CONFIG_ITEM_INT, xs("miscellaneous"), xs("rapid_lag"), xs("hotkey_toggle")},
    config_item_t{settings.miscellaneous.rapid_lag.on_peek, CONFIG_ITEM_BOOL, xs("miscellaneous"), xs("rapid_lag"), xs("on_peek")},
    config_item_t{settings.miscellaneous.rapid_lag.tick_shift, CONFIG_ITEM_INT, xs("miscellaneous"), xs("rapid_lag"), xs("tick_shift")},
    config_item_t{settings.miscellaneous.rapid_lag.wait_ticks, CONFIG_ITEM_INT, xs("miscellaneous"), xs("rapid_lag"), xs("wait_ticks")},
};
// clang-format on

void settings_t::save() {
    auto config_path = std::format(xs(".\\mw\\configs\\{}.json"), settings.global.config_name);
    auto config_file = std::ofstream(config_path);

    if (!config_file.good()) {
        logging::error(xs("failed to open config file for writing"));

        return;
    }

    nlohmann::json config = {};

    for (const auto &item : items) {
        const bool category_exists = !item.category.empty();
        const bool second_category_exists = !item.second_category.empty();
        const bool third_category_exists = !item.third_category.empty();
        const bool fourth_category_exists = !item.fourth_category.empty();

        nlohmann::json value;

        if (category_exists && !second_category_exists && !third_category_exists && !fourth_category_exists) {
            value = config[item.category.c_str()][item.name.c_str()];
        } else if (category_exists && second_category_exists && !third_category_exists && !fourth_category_exists) {
            value = config[item.category.c_str()][item.second_category.c_str()][item.name.c_str()];
        } else if (category_exists && second_category_exists && third_category_exists && !fourth_category_exists) {
            value = config[item.category.c_str()][item.second_category.c_str()][item.third_category.c_str()][item.name.c_str()];
        } else if (category_exists && second_category_exists && third_category_exists && fourth_category_exists) {
            value = config[item.category.c_str()][item.second_category.c_str()][item.third_category.c_str()][item.fourth_category.c_str()]
                          [item.name.c_str()];
        } else {
            continue;
        }

        if (item.type == CONFIG_ITEM_BOOL) {
            value = *(bool *) item.pointer;
        } else if (item.type == CONFIG_ITEM_INT) {
            value = *(int *) item.pointer;
        } else if (item.type == CONFIG_ITEM_FLOAT) {
            value = *(float *) item.pointer;
        } else if (item.type == CONFIG_ITEM_COLOR) {
            auto color = (color_t *) item.pointer;

            value = std::format(xs("#{:02x}{:02x}{:02x}{:02x}"), color->r, color->g, color->b, color->a);
        } else if (item.type == CONFIG_ITEM_STRING) {
            value = *(std::string *) item.pointer;
        } else {
            logging::warning(xs("unknown config item type {} for {}"), (int) item.type, item.name);

            continue;
        }

        if (category_exists && !second_category_exists && !third_category_exists && !fourth_category_exists) {
            config[item.category.c_str()][item.name.c_str()] = value;
        } else if (category_exists && second_category_exists && !third_category_exists && !fourth_category_exists) {
            config[item.category.c_str()][item.second_category.c_str()][item.name.c_str()] = value;
        } else if (category_exists && second_category_exists && third_category_exists && !fourth_category_exists) {
            config[item.category.c_str()][item.second_category.c_str()][item.third_category.c_str()][item.name.c_str()] = value;
        } else if (category_exists && second_category_exists && third_category_exists && fourth_category_exists) {
            config[item.category.c_str()][item.second_category.c_str()][item.third_category.c_str()][item.fourth_category.c_str()]
                  [item.name.c_str()] = value;
        }
    }

    config_file << config.dump(2);
    config_file.close();

    logging::info(xs("config {} has been saved"), settings.global.config_name);
}

void settings_t::load() {
    auto config_path = std::format(xs(".\\mw\\configs\\{}.json"), settings.global.config_name);
    auto config_file = std::ifstream(config_path);

    if (!config_file.good()) {
        logging::error(xs("failed to open config file, make sure it's in the right place"));

        return;
    }

    nlohmann::json config;

    config_file >> config;

    for (const auto &item : items) {
        const bool category_exists = !item.category.empty();
        const bool second_category_exists = !item.second_category.empty();
        const bool third_category_exists = !item.third_category.empty();
        const bool fourth_category_exists = !item.fourth_category.empty();

        nlohmann::json value;

        if (category_exists && !second_category_exists && !third_category_exists && !fourth_category_exists) {
            value = config[item.category.c_str()][item.name.c_str()];
        } else if (category_exists && second_category_exists && !third_category_exists && !fourth_category_exists) {
            value = config[item.category.c_str()][item.second_category.c_str()][item.name.c_str()];
        } else if (category_exists && second_category_exists && third_category_exists && !fourth_category_exists) {
            value = config[item.category.c_str()][item.second_category.c_str()][item.third_category.c_str()][item.name.c_str()];
        } else if (category_exists && second_category_exists && third_category_exists && fourth_category_exists) {
            value = config[item.category.c_str()][item.second_category.c_str()][item.third_category.c_str()][item.fourth_category.c_str()]
                          [item.name.c_str()];
        } else {
            continue;
        }

        if (value.is_null())
            continue;
        
        if (item.type == CONFIG_ITEM_BOOL) {
            *(bool *) item.pointer = value.get<bool>();
        } else if (item.type == CONFIG_ITEM_INT) {
            *(int *) item.pointer = value.get<int>();
        } else if (item.type == CONFIG_ITEM_FLOAT) {
            *(float *) item.pointer = value.get<float>();
        } else if (item.type == CONFIG_ITEM_COLOR) {
            auto color = (color_t *) item.pointer;
            auto value_str = value.get<std::string>();

            if (value_str.size() != 9 && value_str.size() != 7) {
                logging::warning(xs("invalid color value {} for {}"), value_str, item.name);

                continue;
            }

            color->r = std::stoi(value_str.substr(1, 2), nullptr, 16);
            color->g = std::stoi(value_str.substr(3, 2), nullptr, 16);
            color->b = std::stoi(value_str.substr(5, 2), nullptr, 16);

            if (value_str.size() == 9)
                color->a = std::stoi(value_str.substr(7, 2), nullptr, 16);
        } else if (item.type == CONFIG_ITEM_STRING) {
            *(std::string *) item.pointer = value.get<std::string>();
        } else {
            logging::warning(xs("unknown config item type {} for {}"), (int) item.type, item.name);

            continue;
        }
    }

    config_file.close();

    logging::info(xs("config {} has been loaded"), settings.global.config_name);
}
