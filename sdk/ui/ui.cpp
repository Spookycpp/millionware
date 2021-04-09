#include "../resources/csgo_icons.h"
#include "../resources/font_awesome.h"
#include "../core/settings/settings.h"
#include "../engine/input/input.h"
#include "../engine/render/render.h"
#include "ui.h"
#include "../engine/security/xorstr.h"

static bool is_menu_active = true;
static bool new_blocking = false;

static std::shared_ptr<c_window> main_window;
static std::shared_ptr<c_tab> active_tab_ptr;
static std::shared_ptr<c_element> blocking_ptr;

static std::shared_ptr<c_tab> next_active_tab_ptr;
static std::shared_ptr<c_element> next_blocking_ptr;

static color_t accent_color = { 222, 102, 122 };

color_t &ui::get_accent_color()
{
	return accent_color;
}

std::shared_ptr<c_tab> ui::get_active_tab()
{
	return active_tab_ptr;
}

std::shared_ptr<c_element> ui::get_blocking()
{
	return blocking_ptr;
}

void ui::set_active_tab(std::shared_ptr<c_tab> active_tab)
{
	next_active_tab_ptr = active_tab;
}

void ui::set_blocking(std::shared_ptr<c_element> blocking)
{
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

void ui::init()
{
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

	if (const auto aim_category = main_window->new_category(XORSTR("Aim Assistance")))
	{
		if (const auto legit_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_CROSSHAIRS, "Legit")) {
			settings_t::legitbot_t* legitbot_settings = &settings.lbot_pistols;

			if (const auto group = legit_tab->new_group("Weapon groups")) {
				group->new_checkbox("Enabled", settings.global.weapon_groups);
				group->new_select("Group", weapon_group, { "Pistols", "Heavy pistol", "Rifles", "AWP", "Scout", "Auto", "Other" });
			}

			if (const auto group = legit_tab->new_group("Prerequisites")) {
				group->new_slider(XORSTR("Field of view"), legitbot_settings->fov, 0.0f, 180.0f, "{:.1f}");
				group->new_slider(XORSTR("Start bullets"), legitbot_settings->start_bullets, 0, 10, "{}");
				group->new_select("Hitbox method", legitbot_settings->hitbox_method, { "Static", "Nearest" });
				group->new_select("Target hitbox", legitbot_settings->hitbox, { "Head", "Neck", "Upper chest", "Lower chest", "Stomach" });
				group->new_checkbox(XORSTR("Target backtrack"), legitbot_settings->target_backtrack);
			}

			if (const auto group = legit_tab->new_group("General")) {
				group->new_checkbox(XORSTR("Enabled"), legitbot_settings->enabled)
					->add_key_bind(legitbot_settings->hotkey);

				group->new_select("Flickbot", legitbot_settings->flick_bot.enabled, { "Disabled", "Normal", "Silent" });
				group->new_slider(XORSTR("Field of view"), legitbot_settings->flick_bot.fov, 0.1f, 180.0f, "{:.1f}");
				group->new_slider(XORSTR("Hitchance"), legitbot_settings->flick_bot.hit_chance, 0, 100, "{}%");

				group->new_checkbox("Aim assist", legitbot_settings->assist.enabled);
				group->new_slider(XORSTR("Field of view"), legitbot_settings->assist.fov, 0.1f, 180.0f, "{:.1f}");
				group->new_slider(XORSTR("Strength"), legitbot_settings->assist.strength, 0.1f, 1.f, "{:.1f}");

				group->new_checkbox("Backtracking", legitbot_settings->backtrack.enabled);
				group->new_slider(XORSTR("Field of view"), legitbot_settings->backtrack.fov, 0.1f, 180.0f, "{:.1f}");
				group->new_slider(XORSTR("Max time"), legitbot_settings->backtrack.time, 0, 200, "{}ms");
			}

			if (const auto group = legit_tab->new_group("Aim options")) {
				group->new_slider(XORSTR("Speed"), legitbot_settings->speed, 0.0f, 100.0f, "{:.1f}");
				group->new_slider(XORSTR("Speed exponent"), legitbot_settings->speed_exponent, 1.0f, 2.5f, "{:.1f}");
				group->new_slider(XORSTR("RCS X"), legitbot_settings->rcs_x, 0.0f, 100.0f, "{:.1f}");
				group->new_slider(XORSTR("RCS Y"), legitbot_settings->rcs_y, 0.0f, 100.0f, "{:.1f}");
				group->new_checkbox(XORSTR("Adaptive"), legitbot_settings->smoothing.enabled);
				
				// check if the adaptive bool returns true, if so show these options @czapek
				group->new_slider(XORSTR("Smoothing Samples"), legitbot_settings->smoothing.samples, 2, 28, "{}");
				group->new_slider(XORSTR("Smoothing Factor"), legitbot_settings->smoothing.factor, 0.1f, 2.0f, "{}");
			}

			if (const auto group = legit_tab->new_group("Filters")) {
				group->new_checkbox(XORSTR("Target invisible"), legitbot_settings->check_visible);
				group->new_checkbox(XORSTR("Target teammates"), legitbot_settings->check_team);
				group->new_checkbox(XORSTR("Smoke check"), legitbot_settings->check_smoked);
				group->new_checkbox(XORSTR("Flash check"), legitbot_settings->check_flashed);
			}

			if (const auto group = legit_tab->new_group("Standalone RCS")) {
				group->new_checkbox(XORSTR("Enabled"), legitbot_settings->rcs.enabled);
				group->new_slider(XORSTR("X"), legitbot_settings->rcs.x, 0.0f, 100.0f, "{:.1f}");
				group->new_slider(XORSTR("Y"), legitbot_settings->rcs.y, 0.0f, 100.0f, "{:.1f}");
			}

			if (const auto group = legit_tab->new_group("Triggerbot")) {
				group->new_checkbox(XORSTR("Enabled"), legitbot_settings->triggerbot.enabled)
				->add_key_bind(legitbot_settings->triggerbot.hotkey);

				group->new_checkbox(XORSTR("Target teammates"), legitbot_settings->triggerbot.check_team);
				group->new_checkbox(XORSTR("Smoke check"), legitbot_settings->triggerbot.check_smoked);
				group->new_checkbox(XORSTR("Flash check"), legitbot_settings->triggerbot.check_flashed);
				group->new_slider(XORSTR("Hitchance"), legitbot_settings->triggerbot.hit_chance, 0, 100, "{}");
				group->new_slider(XORSTR("Delay"), legitbot_settings->triggerbot.delay, 0, 1000, "{}ms");

				group->new_checkbox(XORSTR("Backtrack"), legitbot_settings->triggerbot.backtrack.enabled);

				// check if the adaptive bool returns true, if so show this option @czapek
				group->new_slider(XORSTR("Backtracking max time"), legitbot_settings->triggerbot.backtrack.time, 0, 200, "{}ms");
				
			}
		}

		const auto rage_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_SKULL, "Rage");
		const auto anti_aim_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_REDO, "Anti-aim");
	}

	if (const auto visual_category = main_window->new_category("Visualizations"))
	{
		if (const auto players_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_USER, "Players"))
		{
			if (const auto group = players_tab->new_group("1")) {
				//group->new_checkbox(XORSTR("Bounding box"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Name"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Health"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Armor"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Weapon"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Ammo"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Skeleton"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Headspot"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Barrel"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Glow"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Outside of FOV"), settings.miscellaneous.movement.bunny_hop);
					// fov radius & size
			}

			if (const auto group = players_tab->new_group("Model")) {
				//group->new_checkbox(XORSTR("Player"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Player (behind walls)"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Render chams in smoke"), settings.miscellaneous.movement.bunny_hop);
				//group->new_checkbox(XORSTR("Visualize backtrack"), settings.miscellaneous.movement.bunny_hop);
			}
		}

		const auto weapons_tab = visual_category->new_tab(FONT_WEAPONS_32, ICON_WEAPON_FIVESEVEN, "Weapons");
		const auto world_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_GLOBE_AMERICAS, "World");
		const auto view_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_EYE, "View");
	}

	if (const auto misc_category = main_window->new_category("Miscellaneous"))
	{
		if (const auto main_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_TOOLS, "Main")) {

			if (const auto group = main_tab->new_group("Movement")) {
				group->new_checkbox(XORSTR("Bunnyhop"), settings.miscellaneous.movement.bunny_hop);
				group->new_checkbox(XORSTR("Infinite duck"), settings.miscellaneous.movement.no_duck_cooldown);

				group->new_checkbox(XORSTR("Jumpbug"), settings.miscellaneous.movement.jump_bug)
				->add_key_bind(settings.miscellaneous.movement.jump_bug_hotkey);

				group->new_checkbox(XORSTR("Edgebug"), settings.miscellaneous.movement.edge_bug)
				->add_key_bind(settings.miscellaneous.movement.edge_bug_hotkey);

				group->new_checkbox(XORSTR("Edgebug assist"), settings.miscellaneous.movement.edge_bug_assist)
					->add_key_bind(settings.miscellaneous.movement.edge_bug_assist_hotkey);

				group->new_slider(XORSTR("Edgebug units"), settings.miscellaneous.movement.edge_bug_radius, 0, 32, "{}");
				group->new_checkbox(XORSTR("Crouch on edgebug"), settings.miscellaneous.movement.edge_bug_crouch);
				group->new_checkbox(XORSTR("Stop movement"), settings.miscellaneous.movement.edge_bug_movement);
				group->new_checkbox(XORSTR("Stop mouse"), settings.miscellaneous.movement.edge_bug_mouse);
				group->new_slider(XORSTR("Edgebug pull amount"), settings.miscellaneous.movement.edgebug_rage_amount, 0.f, 10.0f, "{:.1f}");

				group->new_checkbox(XORSTR("Edge jump"), settings.miscellaneous.movement.edge_jump)
					->add_key_bind(settings.miscellaneous.movement.edge_jump_hotkey);
			}
		}
		const auto presets_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_COGS, "Presets");
		const auto scripts_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_FILE_CODE, "Scripts");
		const auto inventory_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_WALLET, "Inventory");

		set_active_tab(presets_tab);
	}
}

void ui::frame()
{
	if (input::is_key_pressed(VK_INSERT))
	{
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

	if (next_active_tab_ptr != nullptr)
	{
		active_tab_ptr = next_active_tab_ptr;
		next_active_tab_ptr = nullptr;
	}

	if (new_blocking)
	{
		new_blocking = false;
		blocking_ptr = next_blocking_ptr;
	}
}

bool ui::is_active()
{
	return is_menu_active;
}
