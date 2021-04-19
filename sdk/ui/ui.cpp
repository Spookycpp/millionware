#include "ui.h"

#include "../resources/csgo_icons.h"
#include "../resources/font_awesome.h"

#include "../core/settings/settings.h"

#include "../engine/input/input.h"
#include "../engine/render/render.h"
#include "../engine/security/xorstr.h"

#include "../features/miscellaneous/miscellaneous.h"

static bool is_menu_active = true;
static bool new_blocking = false;

static std::shared_ptr<c_window> main_window;
static std::shared_ptr<c_tab> active_tab_ptr;
static std::shared_ptr<c_element> blocking_ptr;

static std::shared_ptr<c_tab> next_active_tab_ptr;
static std::shared_ptr<c_element> next_blocking_ptr;

static color_t accent_color = { 222, 102, 122 };

color_t& ui::get_accent_color() {
	return accent_color;
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
static color_t color_picker_value1 = { 180, 30, 30 };
static color_t color_picker_value2 = { 30, 180, 30 };
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
			settings_t::legitbot_t* legitbot_settings = &settings.lbot_pistols;

			if (const auto group = legit_tab->new_group(XORSTR("Weapon groups"))) {
				group->new_checkbox(XORSTR("Enabled"), settings.global.weapon_groups);
				group->new_select(XORSTR("Group"), weapon_group, { XORSTR("Pistols"), XORSTR("Heavy pistol"), XORSTR("Rifles"), XORSTR("AWP"), XORSTR("Scout"), XORSTR("Auto"), XORSTR("Other") });
			}

			if (const auto group = legit_tab->new_group("Prerequisites")) {
				group->new_slider(XORSTR("Field of view"), legitbot_settings->fov, 0.0f, 180.0f, "{:.1f}°");
				group->new_slider(XORSTR("Start bullets"), legitbot_settings->start_bullets, 0, 10, "{}");
				group->new_select("Hitbox method", legitbot_settings->hitbox_method, { XORSTR("Static"), XORSTR("Nearest") });
				group->new_select("Target hitbox", legitbot_settings->hitbox, { XORSTR("Head"), XORSTR("Neck"), XORSTR("Upper chest"), XORSTR("Lower chest"), XORSTR("Stomach") });
				group->new_checkbox(XORSTR("Target backtrack"), legitbot_settings->target_backtrack);
			}

			if (const auto group = legit_tab->new_group("General")) {
				group->new_checkbox(XORSTR("Enabled"), legitbot_settings->enabled)
					->add_key_bind(legitbot_settings->hotkey);

				group->new_select(XORSTR("Flickbot"), legitbot_settings->flick_bot.enabled, { XORSTR("Disabled"), XORSTR("Normal"), XORSTR("Silent") });
				group->new_slider(XORSTR("Field of view"), legitbot_settings->flick_bot.fov, 0.1f, 180.0f, XORSTR("{:.1f}°"));
				group->new_slider(XORSTR("Hitchance"), legitbot_settings->flick_bot.hit_chance, 0, 100, XORSTR("{}%"));

				group->new_checkbox("Aim assist", legitbot_settings->assist.enabled);
				group->new_slider(XORSTR("Field of view"), legitbot_settings->assist.fov, 0.1f, 180.0f, XORSTR("{:.1f}°"));
				group->new_slider(XORSTR("Strength"), legitbot_settings->assist.strength, 0.1f, 1.f, XORSTR("{:.1f}"));

				group->new_checkbox("Backtracking", legitbot_settings->backtrack.enabled);
				group->new_slider(XORSTR("Field of view"), legitbot_settings->backtrack.fov, 0.1f, 180.0f, XORSTR("{:.1f}°"));
				group->new_slider(XORSTR("Max time"), legitbot_settings->backtrack.time, 0, 200, XORSTR("{}ms"));
			}

			if (const auto group = legit_tab->new_group("Aim options")) {
				group->new_slider(XORSTR("Speed"), legitbot_settings->speed, 0.0f, 100.0f, XORSTR("{:.1f}"));
				group->new_slider(XORSTR("Speed exponent"), legitbot_settings->speed_exponent, 1.0f, 2.5f, XORSTR("{:.1f}"));
				group->new_slider(XORSTR("RCS X"), legitbot_settings->rcs_x, 0.0f, 100.0f, XORSTR("{:.1f}"));
				group->new_slider(XORSTR("RCS Y"), legitbot_settings->rcs_y, 0.0f, 100.0f, XORSTR("{:.1f}"));

				if (group->new_checkbox(XORSTR("Adaptive"), legitbot_settings->smoothing.enabled)) {
					group->new_slider(XORSTR("Smoothing Samples"), legitbot_settings->smoothing.samples, 2, 28, XORSTR("{}"));
					group->new_slider(XORSTR("Smoothing Factor"), legitbot_settings->smoothing.factor, 0.1f, 2.0f, XORSTR("{}"));
				}
			}

			if (const auto group = legit_tab->new_group("Filters")) {
				group->new_checkbox(XORSTR("Target invisible"), legitbot_settings->check_visible);
				group->new_checkbox(XORSTR("Target teammates"), legitbot_settings->check_team);
				group->new_checkbox(XORSTR("Smoke check"), legitbot_settings->check_smoked);
				group->new_checkbox(XORSTR("Flash check"), legitbot_settings->check_flashed);
			}

			if (const auto group = legit_tab->new_group("Standalone RCS")) {
				group->new_checkbox(XORSTR("Enabled"), legitbot_settings->rcs.enabled);
				group->new_slider(XORSTR("X"), legitbot_settings->rcs.x, 0.0f, 100.0f, XORSTR("{:.1f}"));
				group->new_slider(XORSTR("Y"), legitbot_settings->rcs.y, 0.0f, 100.0f, XORSTR("{:.1f}"));
			}

			if (const auto group = legit_tab->new_group("Triggerbot")) {
				group->new_checkbox(XORSTR("Enabled"), legitbot_settings->triggerbot.enabled)
					->add_key_bind(legitbot_settings->triggerbot.hotkey);

				group->new_checkbox(XORSTR("Target teammates"), legitbot_settings->triggerbot.check_team);
				group->new_checkbox(XORSTR("Smoke check"), legitbot_settings->triggerbot.check_smoked);
				group->new_checkbox(XORSTR("Flash check"), legitbot_settings->triggerbot.check_flashed);
				group->new_slider(XORSTR("Hitchance"), legitbot_settings->triggerbot.hit_chance, 0, 100, "{}");
				group->new_slider(XORSTR("Delay"), legitbot_settings->triggerbot.delay, 0, 1000, XORSTR("{}ms"));

				if (group->new_checkbox(XORSTR("Backtrack"), legitbot_settings->triggerbot.backtrack.enabled))
					group->new_slider(XORSTR("Backtracking max time"), legitbot_settings->triggerbot.backtrack.time, 0, 200, XORSTR("{}ms"));
			}
		}

		if (const auto rage_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_SKULL, XORSTR("Rage"))) {
			if (const auto group = rage_tab->new_group(XORSTR("Aimbot"))) {
				// group->new_text(XORSTR("trolled nigger");
			}
		}

		if (const auto anti_aim_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_REDO, XORSTR("Anti-aim"))) {
			if (const auto group = anti_aim_tab->new_group(XORSTR("Angles"))) {

			}
		}
	}

	if (const auto visual_category = main_window->new_category(XORSTR("Visualizations"))) {
		if (const auto players_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_USER, XORSTR("Players"))) {
			if (const auto group = players_tab->new_group(XORSTR("Player ESP"))) {
				group->new_checkbox(XORSTR("Bounding box"), settings.visuals.player.bounding_box);
				group->new_checkbox(XORSTR("Name"), settings.visuals.player.player_name);
				group->new_checkbox(XORSTR("Health"), settings.visuals.player.health);
				group->new_checkbox(XORSTR("Armor"), settings.visuals.player.armor);
				group->new_checkbox(XORSTR("Weapon"), settings.visuals.player.weapon_text);
				group->new_checkbox(XORSTR("Ammo"), settings.visuals.player.ammo);
				group->new_checkbox(XORSTR("Flags"), settings.visuals.player.flags);
				group->new_checkbox(XORSTR("Skeleton"), settings.visuals.player.skeleton);
				group->new_checkbox(XORSTR("Headspot"), settings.visuals.player.head_spot);
				group->new_checkbox(XORSTR("Barrel"), settings.visuals.player.barrel);
				group->new_checkbox(XORSTR("Glow"), settings.visuals.player.glow);
				if (group->new_checkbox(XORSTR("Outside of FOV"), settings.visuals.player.outside_fov)) {
					group->new_slider(XORSTR("Radius"), settings.visuals.player.outside_fov_radius, 0.f, 2.0f, XORSTR("{:.1f}"));
					group->new_slider(XORSTR("Size"), settings.visuals.player.outside_fov_size, 0, 30, XORSTR("{}"));
				}

			}

			if (const auto group = players_tab->new_group(XORSTR("Model"))) {
				group->new_checkbox(XORSTR("Player"), settings.miscellaneous.movement.bunny_hop);
				group->new_checkbox(XORSTR("Player (behind walls)"), settings.miscellaneous.movement.bunny_hop);
				group->new_checkbox(XORSTR("Render chams in smoke"), settings.miscellaneous.movement.bunny_hop);
				group->new_checkbox(XORSTR("Visualize backtrack"), settings.miscellaneous.movement.bunny_hop);
			}
		}

		const auto weapons_tab = visual_category->new_tab(FONT_WEAPONS_32, ICON_WEAPON_FIVESEVEN, XORSTR("Weapons"));

		if (const auto world_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_GLOBE_AMERICAS, XORSTR("World"))) {
			if (const auto group = world_tab->new_group(XORSTR("Main"))) {
				// nightmode
				// fullbright
				// weather (rain, snow & wtv else we can do)
				group->new_select("Skybox", settings.visuals.world.skybox, { XORSTR("Default"), XORSTR("Tibet"), XORSTR("Baggage"), XORSTR("Monastery"), XORSTR("Italy"),
					XORSTR("Aztec"), XORSTR("Vertigo"), XORSTR("Daylight"), XORSTR("Daylight 2"), XORSTR("Clouds"), XORSTR("Clouds 2"), XORSTR("Gray"),
					XORSTR("Clear"), XORSTR("Canals"), XORSTR("Cobblestone"), XORSTR("Assault"), XORSTR("Clouds Dark"), XORSTR("Night"), XORSTR("Night 2"),
					XORSTR("Night Flat"), XORSTR("Dusty"), XORSTR("Rainy"), XORSTR("Custom") });

				// asus walls
				// asus props
				// removals (smoke, fog, blood, teammates, ragdolls, weapons
			}

			if (const auto group = world_tab->new_group(XORSTR("Other"))) {
				group->new_checkbox(XORSTR("Rank reveal"), settings.miscellaneous.rank_reveal);
				//group->new_checkbox(XORSTR("Money reveal"), settings.miscellaneous.reveal_money);
				//group->new_checkbox(XORSTR("Money reveal"), settings.miscellaneous.preserve_killfeed);
			}

			if (const auto group = world_tab->new_group(XORSTR("Fog"))) {
				// enable fog
				// start dist
				// end dist
				// densisty
			}
		}

		if (const auto view_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_EYE, XORSTR("View"))) {

			if (const auto group = view_tab->new_group(XORSTR("Local"))) {
				group->new_slider(XORSTR("Field of view"), settings.visuals.local.override_fov, 50.f, 130.f, XORSTR("{:.1f}°"));
				group->new_slider(XORSTR("Aspect ratio"), settings.visuals.local.aspect_ratio, 0.f, 5.f, XORSTR("{:.1f}%"));
				group->new_slider(XORSTR("Flash alpha"), settings.visuals.local.flash_alpha, 0, 100, XORSTR("{}%"));
				group->new_checkbox(XORSTR("Recoil crosshair"), settings.visuals.local.recoil_crosshair);
				group->new_checkbox(XORSTR("Sniper crosshair"), settings.visuals.local.sniper_crosshair);
				group->new_checkbox(XORSTR("Grenade prediction"), settings.visuals.local.grenade_prediction);
				group->new_checkbox(XORSTR("Spectator list"), settings.visuals.local.spectator_list);
				group->new_checkbox(XORSTR("Kill effect"), settings.visuals.local.kill_effect);

				if (group->new_checkbox(XORSTR("Viewmodel offset"), settings.visuals.local.viewmodel_offset)) {
					group->new_slider(XORSTR("X"), settings.visuals.local.viewmodel_offset_x, -10.f, 10.f, XORSTR("{:.1f}°"));
					group->new_slider(XORSTR("Y"), settings.visuals.local.viewmodel_offset_y, -10.f, 10.f, XORSTR("{:.1f}°"));
					group->new_slider(XORSTR("Z"), settings.visuals.local.viewmodel_offset_z, -10.f, 10.f, XORSTR("{:.1f}°"));
					group->new_slider(XORSTR("R"), settings.visuals.local.viewmodel_offset_r, 0.f, 360.f, XORSTR("{:.1f}°"));
				}
			}

			if (const auto group = view_tab->new_group("Local removals")) {
				group->new_checkbox(XORSTR("Disable post processing"), settings.visuals.local.disable_post_processing);
				group->new_checkbox(XORSTR("Disable panorama blur"), settings.visuals.local.disable_panorama_blur);
			}

			if (const auto group = view_tab->new_group(XORSTR("Model"))) {
				group->new_checkbox(XORSTR("Ragdoll push"), settings.miscellaneous.ragdoll_push);
				group->new_checkbox(XORSTR("Ragdoll float"), settings.miscellaneous.ragdoll_float);
				group->new_select(XORSTR("Feet fx"), settings.visuals.local.feet_fx, { XORSTR("None"), XORSTR("Sparks"), XORSTR("Dust"), XORSTR("Energy splash") });
			}
		}
	}

	if (const auto misc_category = main_window->new_category(XORSTR("Miscellaneous"))) {
		if (const auto main_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_TOOLS, XORSTR("Main"))) {

			if (const auto group = main_tab->new_group(XORSTR("Movement"))) {
				group->new_checkbox(XORSTR("Indicators"), settings.visuals.local.indicators);

				group->new_checkbox(XORSTR("Bunnyhop"), settings.miscellaneous.movement.bunny_hop);
				group->new_checkbox(XORSTR("Infinite duck"), settings.miscellaneous.movement.no_duck_cooldown);

				group->new_checkbox(XORSTR("Jumpbug"), settings.miscellaneous.movement.jump_bug)
					->add_key_bind(settings.miscellaneous.movement.jump_bug_hotkey);
					
				group->new_checkbox(XORSTR("Edgebug"), settings.miscellaneous.movement.edge_bug)
					->add_key_bind(settings.miscellaneous.movement.edge_bug_hotkey);

				if (group->new_checkbox(XORSTR("Edgebug assist"), settings.miscellaneous.movement.edge_bug_assist)) {
					group->add_key_bind(settings.miscellaneous.movement.edge_bug_assist_hotkey);

					group->new_slider(XORSTR("Edgebug units"), settings.miscellaneous.movement.edge_bug_radius, 0, 32, XORSTR("{}"));
					group->new_slider(XORSTR("Edgebug pull amount"), settings.miscellaneous.movement.edgebug_rage_amount, 0.f, 10.0f, XORSTR("{:.1f}"));
					group->new_checkbox(XORSTR("Crouch on edgebug"), settings.miscellaneous.movement.edge_bug_crouch);
					group->new_checkbox(XORSTR("Stop movement"), settings.miscellaneous.movement.edge_bug_movement);
					group->new_checkbox(XORSTR("Stop mouse"), settings.miscellaneous.movement.edge_bug_mouse);
				}

				group->new_checkbox(XORSTR("Edge jump"), settings.miscellaneous.movement.edge_jump)
					->add_key_bind(settings.miscellaneous.movement.edge_jump_hotkey);

				group->new_checkbox(XORSTR("Air duck"), settings.miscellaneous.movement.air_duck);

				group->new_checkbox(XORSTR("Fast walk"), settings.miscellaneous.movement.fast_walk)
					->add_key_bind(settings.miscellaneous.movement.fast_walk_hotkey);

				group->new_checkbox(XORSTR("Slide walk"), settings.miscellaneous.movement.slide_walk);

			}

		}

		if (const auto presets_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_COGS, XORSTR("Settings"))) {
			if (const auto group = presets_tab->new_group(XORSTR("YADA"))) {
				static bool test;
				// add raw text.
				group->new_checkbox("niggers", test)
					->add_color_picker(ui::get_accent_color(), false);
			}

			if (const auto group = presets_tab->new_group(XORSTR("BADA"))) {
			}

			set_active_tab(presets_tab);
		}

		if (const auto scripts_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_FILE_CODE, XORSTR("Scripts"))) {
			if (const auto group = scripts_tab->new_group(XORSTR("A"))) {
			}

			if (const auto group = scripts_tab->new_group(XORSTR("B"))) {
			}
		}

		const auto inventory_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_WALLET, XORSTR("Inventory"));

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
