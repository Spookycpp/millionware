#include "ui.h"

#include "../resources/csgo_icons.h"
#include "../resources/font_awesome.h"

#include "../core/settings/settings.h"
#include "../core/util/util.h"

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

    if (const auto aim_category = main_window->new_category(xs("Aim Assistance"))) {
        if (const auto legit_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_CROSSHAIRS, xs("Legit"))) {
            settings_t::legitbot_t *legitbot_settings = &settings.lbot_pistols;

            if (const auto group = legit_tab->new_group(xs("Weapon groups"))) {
                group->new_checkbox(xs("Enabled"), settings.global.weapon_groups);
                group
                    ->new_select(
                        xs("Group"), weapon_group,
                        {xs("Global"), xs("Pistols"), xs("Heavy pistol"), xs("Rifles"), xs("AWP"), xs("Scout"), xs("Auto"), xs("Other")})
                    ->add_dependency(settings.global.weapon_groups);
            }

            const auto make_weapon_group_ui = [&legit_tab](int weapon_group_index, settings_t::legitbot_t *group_settings) {
                const auto group_dependency = [weapon_group_index](const auto _) { return weapon_group == weapon_group_index; };
                if (const auto group = legit_tab->new_group("Prerequisites")) {
                    group->new_slider(xs("Field of view"), group_settings->fov, 0.0f, 180.0f, "{:.1f}");
                    group->new_slider(xs("Start bullets"), group_settings->start_bullets, 0, 10, "{}");
                    group->new_select(xs("Hitbox method"), group_settings->hitbox_method, {xs("Static"), xs("Nearest")});
                    group->new_select(xs("Target hitbox"), group_settings->hitbox,
                                      {xs("Head"), xs("Neck"), xs("Upper chest"), xs("Lower chest"), xs("Stomach")});
                    group->new_checkbox(xs("Target backtrack"), group_settings->target_backtrack);
                    group->add_dependency(group_dependency);
                }
                if (const auto group = legit_tab->new_group(xs("General"))) {
                    const auto enabled_check = [group_settings](const auto _) { return group_settings->enabled; };
                    const auto flick_bot_check = [group_settings](const auto _) { return group_settings->flick_bot.enabled > 0; };
                    const auto aim_assist_check = [group_settings](const auto _) { return group_settings->assist.enabled; };
                    const auto backtrack_check = [group_settings](const auto _) { return group_settings->backtrack.enabled; };

                    group->new_checkbox(xs("Enabled"), group_settings->enabled)->add_key_bind(group_settings->hotkey);
                    group->new_select(xs("Flickbot"), group_settings->flick_bot.enabled, {xs("Disabled"), xs("Normal"), xs("Silent")});
                    group->new_slider(xs("Field of view"), group_settings->flick_bot.fov, 0.1f, 180.0f, xs("{:.1f}"))
                        ->add_dependencies(enabled_check, flick_bot_check);
                    group->new_slider(xs("Hitchance"), group_settings->flick_bot.hit_chance, 0, 100, xs("{}%"))
                        ->add_dependencies(enabled_check, flick_bot_check);

                    group->new_checkbox(xs("Aim assist"), group_settings->assist.enabled);
                    group->new_slider(xs("Field of view"), group_settings->assist.fov, 0.1f, 5.f, xs("{:.1f}"))
                        ->add_dependencies(enabled_check, aim_assist_check);
                    group->new_slider(xs("Strength"), group_settings->assist.strength, 0.1f, 1.f, xs("{:.1f}"))
                        ->add_dependencies(enabled_check, aim_assist_check);

                    group->new_checkbox("Backtracking", group_settings->backtrack.enabled);
                    group->new_slider(xs("Field of view"), group_settings->backtrack.fov, 0.1f, 180.0f, xs("{:.1f}"))
                        ->add_dependencies(enabled_check, backtrack_check);
                    group->new_slider(xs("Max time"), group_settings->backtrack.time, 0, 200, xs("{}ms"))
                        ->add_dependencies(enabled_check, backtrack_check);

                    group->add_dependency(group_dependency);
                }
                if (const auto group = legit_tab->new_group(xs("Aim options"))) {
                    const auto smoothing_check = [group_settings](const auto _) { return group_settings->smoothing.enabled; };

                    group->new_slider(xs("Speed"), group_settings->speed, 0.0f, 100.0f, xs("{:.1f}"));
                    group->new_slider(xs("Speed exponent"), group_settings->speed_exponent, 1.0f, 2.5f, xs("{:.1f}"));
                    group->new_slider(xs("RCS X"), group_settings->rcs_x, 0.0f, 100.0f, xs("{:.1f}"));
                    group->new_slider(xs("RCS Y"), group_settings->rcs_y, 0.0f, 100.0f, xs("{:.1f}"));

                    group->new_checkbox(xs("Adaptive"), group_settings->smoothing.enabled);
                    group->new_slider(xs("Smoothing Samples"), group_settings->smoothing.samples, 2, 28, xs("{}"))
                        ->add_dependency(smoothing_check);
                    group->new_slider(xs("Smoothing Factor"), group_settings->smoothing.factor, 0.1f, 2.0f, xs("{}"))
                        ->add_dependency(smoothing_check);

                    group->add_dependency(group_dependency);
                }
                if (const auto group = legit_tab->new_group(xs("Filters"))) {
                    group->new_checkbox(xs("Ignore visible check"), group_settings->check_visible);
                    group->new_checkbox(xs("Target teammates"), group_settings->check_team);
                    group->new_checkbox(xs("Smoke check"), group_settings->check_smoked);
                    group->new_checkbox(xs("Flash check"), group_settings->check_flashed);

                    group->add_dependency(group_dependency);
                }
                if (const auto group = legit_tab->new_group(xs("Standalone RCS"))) {
                    const auto rcs_check = [group_settings](const auto _) { return group_settings->rcs.enabled; };

                    group->new_checkbox(xs("Enabled"), group_settings->rcs.enabled);
                    group->new_slider(xs("X"), group_settings->rcs.x, 0.0f, 100.0f, xs("{:.1f}"))->add_dependency(rcs_check);
                    group->new_slider(xs("Y"), group_settings->rcs.y, 0.0f, 100.0f, xs("{:.1f}"))->add_dependency(rcs_check);

                    group->add_dependency(group_dependency);
                }
                if (const auto group = legit_tab->new_group(xs("Triggerbot"))) {
                    const auto triggerbot_check = [group_settings](const auto _) { return group_settings->triggerbot.enabled; };
                    const auto backtrack_check = [group_settings](const auto _) { return group_settings->triggerbot.backtrack.enabled; };

                    group->new_checkbox(xs("Enabled"), group_settings->triggerbot.enabled)->add_key_bind(group_settings->triggerbot.hotkey);
                    group->new_checkbox(xs("Target teammates"), group_settings->triggerbot.check_team)->add_dependency(triggerbot_check);
                    group->new_checkbox(xs("Smoke check"), group_settings->triggerbot.check_smoked)->add_dependency(triggerbot_check);
                    group->new_checkbox(xs("Flash check"), group_settings->triggerbot.check_flashed)->add_dependency(triggerbot_check);
                    group->new_slider(xs("Hitchance"), group_settings->triggerbot.hit_chance, 0, 100, "{}")
                        ->add_dependency(triggerbot_check);
                    group->new_slider(xs("Delay"), group_settings->triggerbot.delay, 0, 1000, xs("{}ms"))->add_dependency(triggerbot_check);

                    group->new_checkbox(xs("Backtrack"), group_settings->triggerbot.backtrack.enabled)->add_dependency(triggerbot_check);
                    group->new_slider(xs("Backtracking max time"), group_settings->triggerbot.backtrack.time, 0, 200, xs("{}ms"))
                        ->add_dependencies(triggerbot_check, backtrack_check);

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

        if (const auto rage_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_SKULL, xs("Ragebot"))) {
            if (const auto group = rage_tab->new_group(xs("Placeholder"))) {
            }

            if (const auto group = rage_tab->new_group(xs("Placeholder 2"))) {
            }
        }

        if (const auto anti_aim_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_REDO, xs("Anti-aim"))) {
            if (const auto group = anti_aim_tab->new_group(xs("Placeholder"))) {
            }

            if (const auto group = anti_aim_tab->new_group(xs("Placeholder 2"))) {
            }
        }
    }

    if (const auto visual_category = main_window->new_category(xs("Visualizations"))) {
        if (const auto players_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_USER, xs("Players"))) {
            if (const auto group = players_tab->new_group(xs("Player ESP"))) {
                group->new_checkbox(xs("Draw teammates"), settings.visuals.player.draw_teammates);
                group->new_checkbox(xs("Bounding box"), settings.visuals.player.bounding_box)
                    ->add_color_picker(settings.visuals.player.bounding_box_color);
                group->new_checkbox(xs("Name"), settings.visuals.player.player_name)
                    ->add_color_picker(settings.visuals.player.player_name_color);
                group->new_checkbox(xs("Health"), settings.visuals.player.health);
                group->new_checkbox(xs("Weapon"), settings.visuals.player.weapon);
                group->new_checkbox(xs("Ammo"), settings.visuals.player.ammo)->add_color_picker(settings.visuals.player.ammo_color);
                // group->new_checkbox(xs("Armor"), settings.visuals.player.armor);

                group->new_select(xs("Flags"), settings.visuals.player.flags,
                                  {xs("Armor"), xs("Scoped"), xs("Reloading"), xs("Flashed"), xs("Bomb"), xs("Defusing"), xs("Smoked"),
                                   xs("Flash kill"), xs("Money"), xs("Defuser")},
                                  true);

                group->new_checkbox(xs("Skeleton"), settings.visuals.player.skeleton)
                    ->add_color_picker(settings.visuals.player.skeleton_color);
                group->new_checkbox(xs("Headspot"), settings.visuals.player.head_spot)
                    ->add_color_picker(settings.visuals.player.head_spot_color);

                group->new_checkbox(xs("Glow"), settings.visuals.player.glow)->add_color_picker(settings.visuals.player.glow_color);

                group->new_checkbox(xs("Outside of FOV"), settings.visuals.player.outside_fov);
                group->new_slider(xs("Radius"), settings.visuals.player.outside_fov_radius, 0.f, 2.0f, xs("{:.1f}"))
                    ->add_dependency(settings.visuals.player.outside_fov);
                group->new_slider(xs("Size"), settings.visuals.player.outside_fov_size, 0, 30, xs("{}"))
                    ->add_dependency(settings.visuals.player.outside_fov);

                group->new_checkbox("Footsteps", settings.visuals.player.footsteps)
                    ->add_color_picker(settings.visuals.player.footsteps_color);

                group->new_checkbox(xs("Engine radar"), settings.visuals.player.engine_radar);
            }

            if (const auto group = players_tab->new_group(xs("Model"))) {
                group->new_select(xs("Material"), settings.visuals.player.chams.material, {xs("Textured"), xs("Flat")});
                group->new_checkbox(xs("Player"), settings.visuals.player.chams.visible)
                    ->add_color_picker(settings.visuals.player.chams.visible_color);

                group->new_checkbox(xs("Player (behind walls)"), settings.visuals.player.chams.invisible)
                    ->add_color_picker(settings.visuals.player.chams.invisible_color);

                group->new_checkbox(xs("Add glow"), settings.visuals.player.chams.glow)
                    ->add_color_picker(settings.visuals.player.chams.glow_color);
            }

            if (const auto group = players_tab->new_group(xs("Viewmodel"))) {
                group->new_select(xs("Arm material"), settings.visuals.local.chams.arms_material, {xs("Textured"), xs("Flat")});
                group->new_checkbox(xs("Arm chams"), settings.visuals.local.chams.arms)
                    ->add_color_picker(settings.visuals.local.chams.arms_color);

                group->new_select(xs("Weapon material"), settings.visuals.local.chams.weapon_material, {xs("Textured"), xs("Flat")});
                group->new_checkbox(xs("Weapon chams"), settings.visuals.local.chams.weapon)
                    ->add_color_picker(settings.visuals.local.chams.weapon_color);

                group->new_select(xs("Sleeve material"), settings.visuals.local.chams.sleeve_material, {xs("Textured"), xs("Flat")});
                group->new_checkbox(xs("Sleeve chams"), settings.visuals.local.chams.sleeve)
                    ->add_color_picker(settings.visuals.local.chams.sleeve_color);
            }
        }

        if (const auto weapons_tab = visual_category->new_tab(FONT_WEAPONS_32, ICON_WEAPON_FIVESEVEN, xs("Weapons"))) {
            if (const auto group = weapons_tab->new_group(xs("Main"))) {
                group->new_checkbox(xs("Weapons"), settings.visuals.world.weapon)->add_color_picker(settings.visuals.world.weapon_color);
                group->new_checkbox(xs("Bomb"), settings.visuals.world.dropped_bomb)
                    ->add_color_picker(settings.visuals.world.dropped_bomb_color);
                group->new_checkbox(xs("Defusal kits"), settings.visuals.world.defusal_kit)
                    ->add_color_picker(settings.visuals.world.defusal_kit_color);
                group->new_checkbox(xs("Grenades"), settings.visuals.world.grenades)
                    ->add_color_picker(settings.visuals.world.grenades_color);
                // group->new_checkbox(xs("Fire spread"),
                // settings.visuals.world.molotov_spread)->add_color_picker(settings.visuals.world.molotov_spread_color);
            }
        }

        if (const auto world_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_GLOBE_AMERICAS, xs("World"))) {
            if (const auto group = world_tab->new_group(xs("Main"))) {
                group->new_checkbox(xs("Nightmode"), settings.visuals.world.nightmode);
                group->new_slider(xs("Intensity"), settings.visuals.world.nightmode_darkness, 0.f, 100.f, xs("{:.0f}%"))
                    ->add_dependency(settings.visuals.world.nightmode);

                group->new_checkbox(xs("Fullbright"), settings.visuals.world.fullbright);
                group->new_checkbox(xs("Weather"), settings.visuals.world.weather);

                group->new_checkbox(xs("Fog"), settings.visuals.world.fog)->add_color_picker(settings.visuals.world.fog_color);
                group->new_slider(xs("Distance"), settings.visuals.world.fog_length, 1, 100, xs("{}"))
                    ->add_dependency(settings.visuals.world.fog);

                // weather (rain, snow & wtv else we can do)
                group->new_select(xs("Skybox"), settings.visuals.world.skybox,
                                  {xs("Default"), xs("Tibet"),      xs("Baggage"),     xs("Monastery"), xs("Italy"),       xs("Aztec"),
                                   xs("Vertigo"), xs("Daylight"),   xs("Daylight 2"),  xs("Clouds"),    xs("Clouds 2"),    xs("Gray"),
                                   xs("Clear"),   xs("Canals"),     xs("Cobblestone"), xs("Assault"),   xs("Clouds Dark"), xs("Night"),
                                   xs("Night 2"), xs("Night Flat"), xs("Dusty"),       xs("Rainy"),     xs("Custom")});

                // removals (smoke, fog, blood, teammates, ragdolls, weapons
            }

            if (const auto group = world_tab->new_group(xs("Other"))) {
                group->new_checkbox(xs("Rank reveal"), settings.miscellaneous.rank_reveal);
                group->new_checkbox(xs("Preseve killfeed"), settings.miscellaneous.preserve_killfeed);
                group->new_checkbox(xs("Damage log"), settings.visuals.player.damage_logs);
                group->new_checkbox(xs("Purchase log"), settings.miscellaneous.buy_log);
                group->new_checkbox(xs("Money reveal"), settings.miscellaneous.money_reveal);
                group->new_checkbox(xs("Bomb timer"), settings.visuals.world.planted_bomb);
                group->new_checkbox(xs("Log bomb plants"), settings.miscellaneous.bomb_log);
                group->new_checkbox(xs("Bomb plant sounds"), settings.miscellaneous.bomb_log_sounds)
                    ->add_dependency(settings.miscellaneous.bomb_log);
                group->new_checkbox(xs("Bomb holder"), settings.miscellaneous.bomb_holder_log)
                    ->add_dependency(settings.miscellaneous.bomb_log);
            }
        }

        if (const auto view_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_EYE, xs("View"))) {

            if (const auto group = view_tab->new_group(xs("Local"))) {
                group->new_slider(xs("Field of view"), settings.visuals.local.override_fov, 50, 130, xs("{}°"));
                group->new_slider(xs("Aspect ratio"), settings.visuals.local.aspect_ratio, 0.f, 5.f, xs("{:.1f}%"));
                group->new_slider(xs("Flash alpha"), settings.visuals.local.flash_alpha, 0, 100, xs("{}%"));
                group->new_select(xs("Recoil crosshair"), settings.visuals.local.recoil_crosshair,
                                  {xs("Disabled"), xs("Engine"), xs("Overlay")});
                group->new_checkbox(xs("Sniper crosshair"), settings.visuals.local.sniper_crosshair);
                group->new_checkbox(xs("Grenade prediction"), settings.visuals.local.grenade_prediction)
                    ->add_color_picker(settings.visuals.local.grenade_prediction_color);
                group->new_checkbox(xs("Spectator list"), settings.visuals.local.spectator_list);
                group->new_checkbox(xs("Kill effect"), settings.visuals.local.kill_effect);

                if (group->new_checkbox(xs("Viewmodel offset"), settings.visuals.local.viewmodel_offset)) {
                    group->new_slider(xs("X"), settings.visuals.local.viewmodel_offset_x, -10.f, 10.f, xs("{:.1f}"))
                        ->add_dependency(settings.visuals.local.viewmodel_offset);
                    group->new_slider(xs("Y"), settings.visuals.local.viewmodel_offset_y, -10.f, 10.f, xs("{:.1f}"))
                        ->add_dependency(settings.visuals.local.viewmodel_offset);
                    group->new_slider(xs("Z"), settings.visuals.local.viewmodel_offset_z, -10.f, 10.f, xs("{:.1f}"))
                        ->add_dependency(settings.visuals.local.viewmodel_offset);
                    group->new_slider(xs("R"), settings.visuals.local.viewmodel_offset_r, 0.f, 360.f, xs("{:.1f}"))
                        ->add_dependency(settings.visuals.local.viewmodel_offset);
                }
            }

            if (const auto group = view_tab->new_group(xs("Local removals"))) {
                group->new_checkbox(xs("Disable post processing"), settings.visuals.local.disable_post_processing);
                group->new_checkbox(xs("Disable panorama blur"), settings.visuals.local.disable_panorama_blur);
                group->new_checkbox(xs("Remove fog"), settings.visuals.world.remove_fog);
                group->new_select(xs("Smoke"), settings.visuals.world.smoke_type, {xs("None"), xs("Wireframe"), xs("Remove")});
            }

            if (const auto group = view_tab->new_group(xs("Model"))) {
                group->new_checkbox(xs("Ragdoll push"), settings.miscellaneous.ragdoll_push);
                group->new_checkbox(xs("Ragdoll float"), settings.miscellaneous.ragdoll_float);
                group->new_select(xs("Feet fx"), settings.visuals.local.feet_fx,
                                  {xs("None"), xs("Sparks"), xs("Dust"), xs("Energy splash")});

                // group->new_checkbox(xs("Feet trails"),
                // settings.visuals.local.foot_trail)->add_color_picker(settings.visuals.local.trail_color);
                // group->new_slider(xs("Time"), settings.visuals.local.trail_time, 0.f, 10.f,
                // xs("{:.1f}"))->add_dependency(settings.visuals.local.foot_trail); group->new_slider(xs("Size"),
                // settings.visuals.local.trail_size, 0.f, 10.f, xs("{:.1f}"))->add_dependency(settings.visuals.local.foot_trail);
            }
        }
    }

    if (const auto misc_category = main_window->new_category(xs("Miscellaneous"))) {
        if (const auto main_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_TOOLS, xs("Main"))) {

            if (const auto group = main_tab->new_group(xs("Movement"))) {

                group
                    ->new_select(xs("Indicators"), settings.visuals.local.indicators,
                                 {xs("Velocity"), xs("Takeoff velocity"), xs("Jumpbug"), xs("Edgebug"), xs("Edgebug assist"),
                                  xs("Edge jump"), xs("Long jump")},
                                 true)
                    ->add_color_picker(settings.visuals.local.velocity_color_3)
                    ->add_color_picker(settings.visuals.local.velocity_color_2)
                    ->add_color_picker(settings.visuals.local.velocity_color_1);

                group->new_checkbox(xs("Bunnyhop"), settings.miscellaneous.movement.bunny_hop);
                group->new_checkbox(xs("Instant crouch"), settings.miscellaneous.movement.no_duck_cooldown);

                group->new_checkbox(xs("Autostrafe"), settings.miscellaneous.movement.auto_strafe);

                group->new_checkbox(xs("Jumpbug"), settings.miscellaneous.movement.jump_bug)
                    ->add_key_bind(settings.miscellaneous.movement.jump_bug_hotkey);

                group->new_checkbox(xs("Edgebug"), settings.miscellaneous.movement.edge_bug_assist)
                    ->add_key_bind(settings.miscellaneous.movement.edge_bug_assist_hotkey);
                group->new_slider(xs("Edgebug units"), settings.miscellaneous.movement.edge_bug_radius, 0, 32, xs("{}"))
                    ->add_dependency(settings.miscellaneous.movement.edge_bug_assist);
                group->new_slider(xs("Edgebug pull amount"), settings.miscellaneous.movement.edgebug_rage_amount, 0.f, 10.0f, xs("{:.1f}"))
                    ->add_dependency(settings.miscellaneous.movement.edge_bug_assist);

                group->new_checkbox(xs("Strafe optimizer"), settings.miscellaneous.movement.strafe_optimizer)
                    ->add_key_bind(settings.miscellaneous.movement.strafe_optimizer_key);
                group
                    ->new_slider(xs("Strafe pull amount"), settings.miscellaneous.movement.strafe_optimizer_pull_amount, 0.f, 100.f,
                                 xs("{:.1f}"))
                    ->add_dependency(settings.miscellaneous.movement.strafe_optimizer);
                group
                    ->new_slider(xs("Strafe max gain"), settings.miscellaneous.movement.strafe_optimizer_max_gain, 0.f, 100.f, xs("{:.1f}"))
                    ->add_dependency(settings.miscellaneous.movement.strafe_optimizer);
                group
                    ->new_slider(xs("Strafe max velocity"), settings.miscellaneous.movement.strafe_optimizer_max_velocity, 0, 2000,
                                 xs("{}"))
                    ->add_dependency(settings.miscellaneous.movement.strafe_optimizer);

                group->new_checkbox(xs("Block bot"), settings.miscellaneous.movement.blockbot)
                    ->add_key_bind(settings.miscellaneous.movement.blockbot_key);

                group->new_checkbox(xs("Long jump"), settings.miscellaneous.movement.long_jump)
                    ->add_key_bind(settings.miscellaneous.movement.long_jump_hotkey);

                group->new_checkbox(xs("Edge jump"), settings.miscellaneous.movement.edge_jump)
                    ->add_key_bind(settings.miscellaneous.movement.edge_jump_hotkey);
                group->new_checkbox(xs("Edge jump (Duck after)"), settings.miscellaneous.movement.edge_jump_duck)
                    ->add_key_bind(settings.miscellaneous.movement.edge_jump_duck_hotkey);
                group->new_checkbox(xs("Edge jump (LJ bind)"), settings.miscellaneous.movement.edge_jump_lj_bind)
                    ->add_key_bind(settings.miscellaneous.movement.edge_jump_lj_bind_hotkey);

                group->new_checkbox(xs("Air duck"), settings.miscellaneous.movement.air_duck);

                group->new_checkbox(xs("Slide walk"), settings.miscellaneous.movement.slide_walk);
            }

            if (const auto group = main_tab->new_group(xs("Other"))) {
                group->new_checkbox(xs("Discord rich presence"), settings.miscellaneous.discord_rpc);
                group->new_checkbox(xs("Clan tag"), settings.miscellaneous.clantag);
                group->new_checkbox(xs("Auto accept"), settings.miscellaneous.auto_accept);

                group->new_checkbox(xs("Fake latency"), settings.miscellaneous.fake_ping.enabled);
                group->new_slider(xs("Desired ping"), settings.miscellaneous.fake_ping.max_ping, 0.f, 200.f, xs("{:.0f}"))
                    ->add_dependency(settings.miscellaneous.fake_ping.enabled);

                group->new_checkbox(xs("Player privacy"), settings.miscellaneous.player_privacy);
                group->new_checkbox(xs("Auto pistol"), settings.miscellaneous.auto_pistol);
                group->new_checkbox(xs("Vote revealer"), settings.miscellaneous.vote_reveal);
                group->new_checkbox(xs("Inventory unlocker"), settings.miscellaneous.unlock_inventory);
                group->new_checkbox(xs("Custom netgraph"), settings.miscellaneous.custom_netgraph);
                group->new_checkbox(xs("Reveal overwatch suspect"), settings.miscellaneous.reveal_overwatch_suspect);
                group->new_checkbox(xs("Kill say"), settings.miscellaneous.kill_say);
                group->new_checkbox(xs("Report player on death"), settings.miscellaneous.report_player_on_death);

                const auto custom_hit_sound_dependency = [](const auto _) { return settings.miscellaneous.hit_sound == 3; };
                const auto custom_kill_sound_dependency = [](const auto _) { return settings.miscellaneous.kill_sound == 3; };

                group->new_select(xs("Hit sound"), settings.miscellaneous.hit_sound,
                                  {xs("None"), xs("Money"), xs("Arena switch press"), xs("Custom")});
                group->new_text_input(xs("Input"), settings.miscellaneous.hit_sound_custom, false)
                    ->add_dependency(custom_hit_sound_dependency);

                group->new_select(xs("Kill sound"), settings.miscellaneous.kill_sound,
                                  {xs("None"), xs("Money"), xs("Arena switch press"), xs("Custom")});
                group->new_text_input(xs("Input"), settings.miscellaneous.hit_sound_custom, false)
                    ->add_dependency(custom_kill_sound_dependency);

                group->new_button(xs("Name spam"), features::miscellaneous::name_spam);
                group->new_button(xs("Unlock hidden convars"), features::miscellaneous::unlock_hidden_convars);

#ifdef MW_DEBUG
                group->new_checkbox(xs("Show debug overlay"), settings.global.debug_overlay);
#endif
                // group->new_text_input("Example text input", text_input_value1);
            }
        }

        if (const auto scripts_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_FILE_CODE, xs("Lua"))) {
            if (const auto group = scripts_tab->new_group(xs("A"))) {
                // clang-format off
                group->new_select(xs("Region selector"), settings.miscellaneous.server_regions, {
                    xs("Disabled"), xs("Australia"), xs("Austria"), xs("Brazil"), xs("Chile"), xs("Dubai"), xs("France"), xs("Germany"), xs("Hong Kong"),
                    xs("India"), xs("Chennai"), xs("India (Mumbai)"), xs("Japan"), xs("Luxembourg"), xs("Netherlands"), xs("Peru"), xs("Philipines"),
                    xs("Poland"), xs("Singapore"), xs("South Africa"), xs("Spain"), xs("Sweden"), xs("United kingdom"), xs("USA (Atlanta)"),
                    xs("USA (Chicago)"), xs("USA (Los Angeles)"), xs("USA (Moses Lake)"), xs("USA (Oklahoma)"), xs("USA (Seattle)"), xs("USA (Washington DC)"),
                });
                // clang-format on
            }

            if (const auto group = scripts_tab->new_group(xs("Lua"))) {
                group->new_button(xs("Open lua folder"), util::open_lua_folder);
                group->new_button(xs("Reload active scripts"), lua::reload);
            }
        }

        const auto inventory_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_WALLET, xs("Inventory"));

        if (const auto presets_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_COGS, xs("Settings"))) {
            if (const auto group = presets_tab->new_group(xs("General"))) {
                group->new_label(xs("Accent color"))->add_color_picker(ui::get_accent_color(), false);
                group->new_text_input(xs("Config name"), settings.global.config_name);

                group->new_button(xs("Open settings folder"), util::open_settings_folder);
                group->new_button(xs("Save"), []() { settings.save(); });
                group->new_button(xs("Load"), []() { settings.load(); });

                // static bool test;
                // add raw text.
                // group->new_checkbox(xs("niggers"), test)->add_color_picker(ui::get_accent_color(), false);
            }

            // if (const auto group = presets_tab->new_group(xs("BADA"))) {
            // }

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
