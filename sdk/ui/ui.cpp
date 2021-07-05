#include "ui.h"

#include "../resources/csgo_icons.h"
#include "../resources/font_awesome.h"

#include "../core/settings/settings.h"

#include "../engine/input/input.h"
#include "../engine/render/render.h"
#include "../engine/security/xorstr.h"

#include "../features/miscellaneous/miscellaneous.h"

#include "../lua/lua_game.hpp"

static bool is_menu_active = true;
static bool new_blocking = false;

static std::shared_ptr<c_window> main_window;
static std::shared_ptr<c_tab> active_tab_ptr;
static std::shared_ptr<c_element> blocking_ptr;

static std::shared_ptr<c_tab> next_active_tab_ptr;
static std::shared_ptr<c_element> next_blocking_ptr;

color_t &ui::get_accent_color() {
    return settings.global.accent_color;
}

std::shared_ptr<c_tab> ui::get_active_tab() {
    return active_tab_ptr;
}

std::shared_ptr<c_element> ui::get_blocking() {
    return blocking_ptr;
}

void ui::set_active_tab(std::shared_ptr<c_tab> active_tab) {
    next_active_tab_ptr = active_tab;
}

void ui::set_blocking(std::shared_ptr<c_element> blocking) {
    new_blocking = true;
    next_blocking_ptr = blocking;
}

static bool checkbox_value1 = true;
static bool checkbox_value2 = false;
static int select_value1 = 0;
static int multi_select_value1 = 0;
static int slider_int_value1 = 25;
static float slider_float_value1 = 25.0f;
static color_t color_picker_value1 = {180, 30, 30};
static color_t color_picker_value2 = {30, 180, 30};
static int key_bind_value1 = 0x2E;
static int key_bind_value2 = 0x2D;
static int weapon_group = 0;
static std::string text_input_value1;

void ui::init() {
    main_window = std::make_shared<c_window>(point_t(64.0f, 64.0f), point_t(1000.0f, 704.0f));

    /*group->new_checkbox("Example checkbox", checkbox_value1)
        ->add_color_picker(ui::get_accent_color(), false)
        ->add_key_bind(key_bind_value1);

    group->new_checkbox("Example checkbox", checkbox_value2)
        ->add_color_picker(color_picker_value1)
        ->add_key_bind(key_bind_value2);

    group->new_select("Example select", select_value1, { "Test option 1", "Test option 2", "Test option 3", "Test option 4", "Test option 5" })
        ->add_color_picker(color_picker_value1)
        ->add_key_bind(key_bind_value1);

    group->new_select("Example multi select", multi_select_value1, { "Test oasfddddption 1", "Test option 2", "Test option 3", "Test option 4", "Testdfhgderherwsdf option 5" }, true)
        ->add_color_picker(color_picker_value1)
        ->add_key_bind(key_bind_value2);

    group->new_slider("Example float slider", slider_float_value1, -50.0f, 50.0f, "{:.1f}")
        ->add_color_picker(color_picker_value1)
        ->add_key_bind(key_bind_value1);

    group->new_slider("Example int slider", slider_int_value1, 0, 50, "{}")
        ->add_key_bind(key_bind_value2)
        ->add_key_bind(key_bind_value2);

    group->new_button("Example button", [] () { printf("Nigger 1 \n"); });
    group->new_button("Example button", [] () { printf("Nigger 2 \n"); }, ICON_FA_TAG, FONT_FA_SOLID_32)
        ->add_key_bind(key_bind_value1);

    group->new_text_input("Example text input", text_input_value1)
        ->add_color_picker(color_picker_value1)
        ->add_key_bind(key_bind_value2);*/

    if (const auto aim_category = main_window->new_category(XORSTR("Aim Assistance"))) {
        if (const auto legit_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_CROSSHAIRS, XORSTR("Legit"))) {
            settings_t::legitbot_t *legitbot_settings = &settings.lbot_pistols;

            if (const auto group = legit_tab->new_group(XORSTR("Weapon groups"))) {
                group->new_checkbox(XORSTR("Enabled"), settings.global.weapon_groups);
                group->new_select(XORSTR("Group"), weapon_group,
                                  {XORSTR("Global"), XORSTR("Pistols"), XORSTR("Heavy pistol"), XORSTR("Rifles"), XORSTR("AWP"), XORSTR("Scout"), XORSTR("Auto"), XORSTR("Other")});
            }

            const auto make_weapon_group_ui = [&legit_tab](int weapon_group_index, settings_t::legitbot_t *group_settings) {
                const auto group_dependency = [weapon_group_index](const auto _) { return weapon_group == weapon_group_index; };
                if (const auto group = legit_tab->new_group("Prerequisites")) {
                    group->new_slider(XORSTR("Field of view"), group_settings->fov, 0.0f, 180.0f, "{:.1f}");
                    group->new_slider(XORSTR("Start bullets"), group_settings->start_bullets, 0, 10, "{}");
                    group->new_select(XORSTR("Hitbox method"), group_settings->hitbox_method, {XORSTR("Static"), XORSTR("Nearest")});
                    group->new_select(XORSTR("Target hitbox"), group_settings->hitbox, {XORSTR("Head"), XORSTR("Neck"), XORSTR("Upper chest"), XORSTR("Lower chest"), XORSTR("Stomach")});
                    group->new_checkbox(XORSTR("Target backtrack"), group_settings->target_backtrack);
                    group->add_dependency(group_dependency);
                }
                if (const auto group = legit_tab->new_group(XORSTR("General"))) {
                    const auto enabled_check = [group_settings](const auto _) { return group_settings->enabled; };
                    const auto flick_bot_check = [group_settings](const auto _) { return group_settings->flick_bot.enabled > 0; };
                    const auto aim_assist_check = [group_settings](const auto _) { return group_settings->assist.enabled; };
                    const auto backtrack_check = [group_settings](const auto _) { return group_settings->backtrack.enabled; };

                    group->new_checkbox(XORSTR("Enabled"), group_settings->enabled)->add_key_bind(group_settings->hotkey);
                    group->new_select(XORSTR("Flickbot"), group_settings->flick_bot.enabled, {XORSTR("Disabled"), XORSTR("Normal"), XORSTR("Silent")});
                    group->new_slider(XORSTR("Field of view"), group_settings->flick_bot.fov, 0.1f, 180.0f, XORSTR("{:.1f}"))->add_dependencies(enabled_check, flick_bot_check);
                    group->new_slider(XORSTR("Hitchance"), group_settings->flick_bot.hit_chance, 0, 100, XORSTR("{}%"))->add_dependencies(enabled_check, flick_bot_check);

                    group->new_checkbox(XORSTR("Aim assist"), group_settings->assist.enabled);
                    group->new_slider(XORSTR("Field of view"), group_settings->assist.fov, 0.1f, 5.f, XORSTR("{:.1f}"))->add_dependencies(enabled_check, aim_assist_check);
                    group->new_slider(XORSTR("Strength"), group_settings->assist.strength, 0.1f, 1.f, XORSTR("{:.1f}"))->add_dependencies(enabled_check, aim_assist_check);

                    group->new_checkbox("Backtracking", group_settings->backtrack.enabled);
                    group->new_slider(XORSTR("Field of view"), group_settings->backtrack.fov, 0.1f, 180.0f, XORSTR("{:.1f}"))->add_dependencies(enabled_check, backtrack_check);
                    group->new_slider(XORSTR("Max time"), group_settings->backtrack.time, 0, 200, XORSTR("{}ms"))->add_dependencies(enabled_check, backtrack_check);

                    group->add_dependency(group_dependency);
                }
                if (const auto group = legit_tab->new_group(XORSTR("Aim options"))) {
                    const auto smoothing_check = [group_settings](const auto _) { return group_settings->smoothing.enabled; };

                    group->new_slider(XORSTR("Speed"), group_settings->speed, 0.0f, 100.0f, XORSTR("{:.1f}"));
                    group->new_slider(XORSTR("Speed exponent"), group_settings->speed_exponent, 1.0f, 2.5f, XORSTR("{:.1f}"));
                    group->new_slider(XORSTR("RCS X"), group_settings->rcs_x, 0.0f, 100.0f, XORSTR("{:.1f}"));
                    group->new_slider(XORSTR("RCS Y"), group_settings->rcs_y, 0.0f, 100.0f, XORSTR("{:.1f}"));

                    group->new_checkbox(XORSTR("Adaptive"), group_settings->smoothing.enabled);
                    group->new_slider(XORSTR("Smoothing Samples"), group_settings->smoothing.samples, 2, 28, XORSTR("{}"))->add_dependency(smoothing_check);
                    group->new_slider(XORSTR("Smoothing Factor"), group_settings->smoothing.factor, 0.1f, 2.0f, XORSTR("{}"))->add_dependency(smoothing_check);

                    group->add_dependency(group_dependency);
                }
                if (const auto group = legit_tab->new_group(XORSTR("Filters"))) {
                    group->new_checkbox(XORSTR("Ignore visible check"), group_settings->check_visible);
                    group->new_checkbox(XORSTR("Target teammates"), group_settings->check_team);
                    group->new_checkbox(XORSTR("Smoke check"), group_settings->check_smoked);
                    group->new_checkbox(XORSTR("Flash check"), group_settings->check_flashed);

                    group->add_dependency(group_dependency);
                }
                if (const auto group = legit_tab->new_group(XORSTR("Standalone RCS"))) {
                    const auto rcs_check = [group_settings](const auto _) { return group_settings->rcs.enabled; };

                    group->new_checkbox(XORSTR("Enabled"), group_settings->rcs.enabled);
                    group->new_slider(XORSTR("X"), group_settings->rcs.x, 0.0f, 100.0f, XORSTR("{:.1f}"))->add_dependency(rcs_check);
                    group->new_slider(XORSTR("Y"), group_settings->rcs.y, 0.0f, 100.0f, XORSTR("{:.1f}"))->add_dependency(rcs_check);

                    group->add_dependency(group_dependency);
                }
                if (const auto group = legit_tab->new_group(XORSTR("Triggerbot"))) {
                    const auto triggerbot_check = [group_settings](const auto _) { return group_settings->triggerbot.enabled; };
                    const auto backtrack_check = [group_settings](const auto _) { return group_settings->triggerbot.backtrack.enabled; };

                    group->new_checkbox(XORSTR("Enabled"), group_settings->triggerbot.enabled)->add_key_bind(group_settings->triggerbot.hotkey);
                    group->new_checkbox(XORSTR("Target teammates"), group_settings->triggerbot.check_team)->add_dependency(triggerbot_check);
                    group->new_checkbox(XORSTR("Smoke check"), group_settings->triggerbot.check_smoked)->add_dependency(triggerbot_check);
                    group->new_checkbox(XORSTR("Flash check"), group_settings->triggerbot.check_flashed)->add_dependency(triggerbot_check);
                    group->new_slider(XORSTR("Hitchance"), group_settings->triggerbot.hit_chance, 0, 100, "{}")->add_dependency(triggerbot_check);
                    group->new_slider(XORSTR("Delay"), group_settings->triggerbot.delay, 0, 1000, XORSTR("{}ms"))->add_dependency(triggerbot_check);

                    group->new_checkbox(XORSTR("Backtrack"), group_settings->triggerbot.backtrack.enabled)->add_dependency(triggerbot_check);
                    group->new_slider(XORSTR("Backtracking max time"), group_settings->triggerbot.backtrack.time, 0, 200, XORSTR("{}ms"))->add_dependencies(triggerbot_check, backtrack_check);

                    group->add_dependency(group_dependency);
                }
            };

            make_weapon_group_ui(0, &settings.lbot_global);
            make_weapon_group_ui(1, &settings.lbot_pistols);
            make_weapon_group_ui(2, &settings.lbot_hpistols);
            make_weapon_group_ui(3, &settings.lbot_rifles);
            make_weapon_group_ui(4, &settings.lbot_awp);
            make_weapon_group_ui(5, &settings.lbot_scout);
            make_weapon_group_ui(6, &settings.lbot_auto);
            make_weapon_group_ui(7, &settings.lbot_other);
        }

        if (const auto rage_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_SKULL, XORSTR("Ragebot"))) {
            if (const auto group = rage_tab->new_group(XORSTR("Placeholder"))) {
            }    

            if (const auto group = rage_tab->new_group(XORSTR("Placeholder 2"))) {
            }
        }

        if (const auto anti_aim_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_REDO, XORSTR("Anti-aim"))) {
            if (const auto group = anti_aim_tab->new_group(XORSTR("Placeholder"))) {
            }

            if (const auto group = anti_aim_tab->new_group(XORSTR("Placeholder 2"))) {
            }
        }
    }

    if (const auto visual_category = main_window->new_category(XORSTR("Visualizations"))) {
        if (const auto players_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_USER, XORSTR("Players"))) {
            if (const auto group = players_tab->new_group(XORSTR("Player ESP"))) {
                group->new_checkbox(XORSTR("Bounding box"), settings.visuals.player.bounding_box)->add_color_picker(settings.visuals.player.bounding_box_color);
                group->new_checkbox(XORSTR("Name"), settings.visuals.player.player_name)->add_color_picker(settings.visuals.player.player_name_color);
                group->new_checkbox(XORSTR("Health"), settings.visuals.player.health);
                group->new_checkbox(XORSTR("Weapon"), settings.visuals.player.weapon);
                group->new_checkbox(XORSTR("Ammo"), settings.visuals.player.ammo)->add_color_picker(settings.visuals.player.ammo_color);
                group->new_checkbox(XORSTR("Armor"), settings.visuals.player.armor);

                group->new_select(XORSTR("Flags"), settings.visuals.player.flags,
                                  {XORSTR("Armor"), XORSTR("Scoped"), XORSTR("Reloading"), XORSTR("Flashed"), XORSTR("Bomb"), XORSTR("Defusing"), XORSTR("Smoked"), XORSTR("Flash kill")}, true);

                group->new_checkbox(XORSTR("Skeleton"), settings.visuals.player.skeleton)->add_color_picker(settings.visuals.player.skeleton_color);
                group->new_checkbox(XORSTR("Headspot"), settings.visuals.player.head_spot)->add_color_picker(settings.visuals.player.head_spot_color);
                group->new_checkbox(XORSTR("Barrel"), settings.visuals.player.barrel);
                group->new_checkbox(XORSTR("Glow"), settings.visuals.player.glow);

                group->new_checkbox(XORSTR("Outside of FOV"), settings.visuals.player.outside_fov);
                group->new_slider(XORSTR("Radius"), settings.visuals.player.outside_fov_radius, 0.f, 2.0f, XORSTR("{:.1f}"))->add_dependency(settings.visuals.player.outside_fov);
                group->new_slider(XORSTR("Size"), settings.visuals.player.outside_fov_size, 0, 30, XORSTR("{}"))->add_dependency(settings.visuals.player.outside_fov);
            }

            if (const auto group = players_tab->new_group(XORSTR("Model"))) {
                group->new_select(XORSTR("Material"), settings.visuals.player.chams.material, {XORSTR("Textured"), XORSTR("Flat")});
                group->new_checkbox(XORSTR("Player"), settings.visuals.player.chams.visible)->add_color_picker(settings.visuals.player.chams.visible_color, false);
                group->new_checkbox(XORSTR("Player (behind walls)"), settings.visuals.player.chams.invisible)->add_color_picker(settings.visuals.player.chams.invisible_color, false);
                group->new_checkbox(XORSTR("Visualize backtrack"), settings.visuals.player.chams.backtrack);
            }
        }

        if (const auto weapons_tab = visual_category->new_tab(FONT_WEAPONS_32, ICON_WEAPON_FIVESEVEN, XORSTR("Weapons"))) {
            if (const auto group = weapons_tab->new_group(XORSTR("Main"))) {
            }
        }

        if (const auto world_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_GLOBE_AMERICAS, XORSTR("World"))) {
            if (const auto group = world_tab->new_group(XORSTR("Main"))) {
                group->new_checkbox(XORSTR("Rain"), settings.visuals.world.weather);
                group->new_checkbox(XORSTR("Nightmode"), settings.visuals.world.nightmode)->add_color_picker(settings.visuals.world.nightmode_color);
                group->new_slider(XORSTR("Intensity"), settings.visuals.world.nightmode_darkness, 0.f, 100.f, XORSTR("{:.0f}%"))->add_dependency(settings.visuals.world.nightmode);
                group->new_checkbox(XORSTR("Fullbright"), settings.visuals.world.fullbright);

                // weather (rain, snow & wtv else we can do)
                group->new_select(XORSTR("Skybox"), settings.visuals.world.skybox,
                                  {XORSTR("Default"),     XORSTR("Tibet"),  XORSTR("Baggage"),  XORSTR("Monastery"),  XORSTR("Italy"), XORSTR("Aztec"),  XORSTR("Vertigo"),     XORSTR("Daylight"),
                                   XORSTR("Daylight 2"),  XORSTR("Clouds"), XORSTR("Clouds 2"), XORSTR("Gray"),       XORSTR("Clear"), XORSTR("Canals"), XORSTR("Cobblestone"), XORSTR("Assault"),
                                   XORSTR("Clouds Dark"), XORSTR("Night"),  XORSTR("Night 2"),  XORSTR("Night Flat"), XORSTR("Dusty"), XORSTR("Rainy"),  XORSTR("Custom")});

                // removals (smoke, fog, blood, teammates, ragdolls, weapons
            }

            if (const auto group = world_tab->new_group(XORSTR("Other"))) {
                group->new_checkbox(XORSTR("Rank reveal"), settings.miscellaneous.rank_reveal);
                group->new_checkbox(XORSTR("Preseve killfeed"), settings.miscellaneous.preserve_killfeed);
                group->new_checkbox(XORSTR("Damage log"), settings.visuals.player.damage_logs);
                group->new_checkbox(XORSTR("Purchase log"), settings.miscellaneous.buy_log);
                group->new_checkbox(XORSTR("Money reveal"), settings.miscellaneous.money_reveal);
            }
        }

        if (const auto view_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_EYE, XORSTR("View"))) {

            if (const auto group = view_tab->new_group(XORSTR("Local"))) {
                group->new_slider(XORSTR("Field of view"), settings.visuals.local.override_fov, 50, 130, XORSTR("{}°"));
                group->new_slider(XORSTR("Aspect ratio"), settings.visuals.local.aspect_ratio, 0.f, 5.f, XORSTR("{:.1f}%"));
                group->new_slider(XORSTR("Flash alpha"), settings.visuals.local.flash_alpha, 0, 100, XORSTR("{}%"));
                group->new_checkbox(XORSTR("Recoil crosshair"), settings.visuals.local.recoil_crosshair);
                group->new_checkbox(XORSTR("Sniper crosshair"), settings.visuals.local.sniper_crosshair);
                group->new_checkbox(XORSTR("Grenade prediction"), settings.visuals.local.grenade_prediction)->add_color_picker(settings.visuals.local.grenade_prediction_color);
                group->new_checkbox(XORSTR("Spectator list"), settings.visuals.local.spectator_list);
                group->new_checkbox(XORSTR("Kill effect"), settings.visuals.local.kill_effect);

                if (group->new_checkbox(XORSTR("Viewmodel offset"), settings.visuals.local.viewmodel_offset)) {
                    group->new_slider(XORSTR("X"), settings.visuals.local.viewmodel_offset_x, -10.f, 10.f, XORSTR("{:.1f}"))->add_dependency(settings.visuals.local.viewmodel_offset);
                    group->new_slider(XORSTR("Y"), settings.visuals.local.viewmodel_offset_y, -10.f, 10.f, XORSTR("{:.1f}"))->add_dependency(settings.visuals.local.viewmodel_offset);
                    group->new_slider(XORSTR("Z"), settings.visuals.local.viewmodel_offset_z, -10.f, 10.f, XORSTR("{:.1f}"))->add_dependency(settings.visuals.local.viewmodel_offset);
                    group->new_slider(XORSTR("R"), settings.visuals.local.viewmodel_offset_r, 0.f, 360.f, XORSTR("{:.1f}"))->add_dependency(settings.visuals.local.viewmodel_offset);
                }
            }

            if (const auto group = view_tab->new_group(XORSTR("Local removals"))) {
                group->new_checkbox(XORSTR("Disable post processing"), settings.visuals.local.disable_post_processing);
                group->new_checkbox(XORSTR("Disable panorama blur"), settings.visuals.local.disable_panorama_blur);
                group->new_checkbox(XORSTR("Remove fog"), settings.visuals.world.remove_fog);
            }

            if (const auto group = view_tab->new_group(XORSTR("Model"))) {
                group->new_checkbox(XORSTR("Ragdoll push"), settings.miscellaneous.ragdoll_push);
                group->new_checkbox(XORSTR("Ragdoll float"), settings.miscellaneous.ragdoll_float);
                group->new_select(XORSTR("Feet fx"), settings.visuals.local.feet_fx, {XORSTR("None"), XORSTR("Sparks"), XORSTR("Dust"), XORSTR("Energy splash")});

                group->new_checkbox(XORSTR("Feet trails"), settings.visuals.local.foot_trail)->add_color_picker(settings.visuals.local.trail_color);
                group->new_slider(XORSTR("Time"), settings.visuals.local.trail_time, 0.f, 10.f, XORSTR("{:.1f}"))->add_dependency(settings.visuals.local.foot_trail);
                group->new_slider(XORSTR("Size"), settings.visuals.local.trail_size, 0.f, 10.f, XORSTR("{:.1f}"))->add_dependency(settings.visuals.local.foot_trail);
            }
        }
    }

    if (const auto misc_category = main_window->new_category(XORSTR("Miscellaneous"))) {
        if (const auto main_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_TOOLS, XORSTR("Main"))) {

            if (const auto group = main_tab->new_group(XORSTR("Movement"))) {

                group->new_select(XORSTR("Indicators"), settings.visuals.local.indicators,
                    { XORSTR("Velocity"), XORSTR("Takeoff velocity"), XORSTR("Jumpbug"), XORSTR("Edgebug"), XORSTR("Edgebug assist"), XORSTR("Edge jump") }, true );

                group->new_checkbox(XORSTR("Bunnyhop"), settings.miscellaneous.movement.bunny_hop);
                group->new_checkbox(XORSTR("Infinite duck"), settings.miscellaneous.movement.no_duck_cooldown);

                group->new_checkbox(XORSTR("Jumpbug"), settings.miscellaneous.movement.jump_bug)->add_key_bind(settings.miscellaneous.movement.jump_bug_hotkey);

                group->new_checkbox(XORSTR("Edgebug"), settings.miscellaneous.movement.edge_bug_assist)->add_key_bind(settings.miscellaneous.movement.edge_bug_assist_hotkey);
                group->new_slider(XORSTR("Edgebug units"), settings.miscellaneous.movement.edge_bug_radius, 0, 32, XORSTR("{}"))->add_dependency(settings.miscellaneous.movement.edge_bug_assist);
                group->new_slider(XORSTR("Edgebug pull amount"), settings.miscellaneous.movement.edgebug_rage_amount, 0.f, 10.0f, XORSTR("{:.1f}"))->add_dependency(settings.miscellaneous.movement.edge_bug_assist);

                group->new_checkbox(XORSTR("Strafe optimizer"), settings.miscellaneous.movement.strafe_optimizer)->add_key_bind(settings.miscellaneous.movement.strafe_optimizer_key);
                group->new_slider(XORSTR("Strafe pull amount"), settings.miscellaneous.movement.strafe_optimizer_pull_amount, 0.f, 100.f, XORSTR("{:.1f}"))->add_dependency(settings.miscellaneous.movement.strafe_optimizer);
                group->new_slider(XORSTR("Strafe max gain"), settings.miscellaneous.movement.strafe_optimizer_max_gain, 0.f, 100.f, XORSTR("{:.1f}"))->add_dependency(settings.miscellaneous.movement.strafe_optimizer);
                group->new_slider(XORSTR("Strafe max velocity"), settings.miscellaneous.movement.strafe_optimizer_max_velocity, 0, 2000, XORSTR("{}"))->add_dependency(settings.miscellaneous.movement.strafe_optimizer);

                group->new_checkbox(XORSTR("Block bot"), settings.miscellaneous.movement.blockbot)->add_key_bind(settings.miscellaneous.movement.blockbot_key);

                group->new_checkbox(XORSTR("Edge jump"), settings.miscellaneous.movement.edge_jump)->add_key_bind(settings.miscellaneous.movement.edge_jump_hotkey);

                group->new_checkbox(XORSTR("Air duck"), settings.miscellaneous.movement.air_duck);

                group->new_checkbox(XORSTR("Slide walk"), settings.miscellaneous.movement.slide_walk);
            }

            if (const auto group = main_tab->new_group(XORSTR("Other"))) {
                group->new_checkbox(XORSTR("Discord rich presence"), settings.miscellaneous.discord_rpc);
                group->new_checkbox(XORSTR("Clan tag"), settings.miscellaneous.clantag);
                group->new_checkbox(XORSTR("Auto accept"), settings.miscellaneous.auto_accept);
                group->new_checkbox(XORSTR("Fake latency"), settings.miscellaneous.fake_ping.enabled);
                group->new_checkbox(XORSTR("Player privacy"), settings.miscellaneous.player_privacy);
                group->new_checkbox(XORSTR("Auto pistol"), settings.miscellaneous.auto_pistol);
                group->new_checkbox(XORSTR("Log weapon purchases"), settings.miscellaneous.buy_log);

                group->new_checkbox(XORSTR("Log bomb plants"), settings.miscellaneous.bomb_log);
                group->new_checkbox(XORSTR("Bomb plant sounds"), settings.miscellaneous.bomb_log_sounds)->add_dependency(settings.miscellaneous.bomb_log);

                group->new_checkbox(XORSTR("Vote revealer"), settings.miscellaneous.vote_reveal);

                group->new_checkbox(XORSTR("Report player on death"), settings.miscellaneous.report_player_on_death);

                const auto custom_hit_sound_dependency = [](const auto _) { return settings.miscellaneous.hit_sound == 3; };
                const auto custom_kill_sound_dependency = [](const auto _) { return settings.miscellaneous.kill_sound == 3; };

                group->new_select(XORSTR("Hit sound"), settings.miscellaneous.hit_sound, {XORSTR("None"), XORSTR("Money"), XORSTR("Arena switch press"), XORSTR("Custom")});
                group->new_text_input(XORSTR("Input"), settings.miscellaneous.hit_sound_custom, false)->add_dependency(custom_hit_sound_dependency);

                group->new_select(XORSTR("Kill sound"), settings.miscellaneous.kill_sound, {XORSTR("None"), XORSTR("Money"), XORSTR("Arena switch press"), XORSTR("Custom")});
                group->new_text_input(XORSTR("Input"), settings.miscellaneous.hit_sound_custom, false)->add_dependency(custom_kill_sound_dependency);
                
                group->new_button(XORSTR("Name spam"), features::miscellaneous::name_spam);
                group->new_button(XORSTR("Unlock hidden convars"), features::miscellaneous::unlock_hidden_convars);
                group->new_text_input("Example text input", text_input_value1);
            }
        }

        if (const auto scripts_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_FILE_CODE, XORSTR("Lua"))) {
            if (const auto group = scripts_tab->new_group(XORSTR("A"))) {
            }

            if (const auto group = scripts_tab->new_group(XORSTR("Lua"))) {
                group->new_button(XORSTR("Reload active scripts"), lua::reload);
            }
        }

        const auto inventory_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_WALLET, XORSTR("Inventory"));

        if (const auto presets_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_COGS, XORSTR("Settings"))) {
            if (const auto group = presets_tab->new_group(XORSTR("YADA"))) {
                static bool test;
                // add raw text.
                group->new_checkbox(XORSTR("niggers"), test)->add_color_picker(ui::get_accent_color(), false);
            }

            if (const auto group = presets_tab->new_group(XORSTR("BADA"))) {
            }

            set_active_tab(presets_tab);
        }
    }
}

void ui::frame() {
    if (input::is_key_pressed(VK_INSERT)) {
        is_menu_active ^= true;

        new_blocking = false;
        next_blocking_ptr = blocking_ptr = nullptr;
    }

    input::set_can_change_cursor(is_menu_active);

    if (!is_menu_active)
        return;

    main_window->render();

    if (blocking_ptr != nullptr)
        blocking_ptr->render();

    if (next_active_tab_ptr != nullptr) {
        active_tab_ptr = next_active_tab_ptr;
        next_active_tab_ptr = nullptr;
    }

    if (new_blocking) {
        new_blocking = false;
        blocking_ptr = next_blocking_ptr;
    }
}

bool ui::is_active() {
    return is_menu_active;
}
