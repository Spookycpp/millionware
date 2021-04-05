#include "../resources/csgo_icons.h"
#include "../resources/font_awesome.h"
#include "../engine/input/input.h"
#include "../engine/render/render.h"
#include "ui.h"

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
static std::string text_input_value1;

void ui::init()
{
	main_window = std::make_shared<c_window>(point_t(64.0f, 64.0f), point_t(1000.0f, 704.0f));

	const auto prepare_tab = [] (const auto &tab)
	{
		for (auto i = 0; i < 2; i++)
		{
			const auto group = tab->new_group("Group");

			group->new_checkbox("Example checkbox", checkbox_value1)
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
				->add_key_bind(key_bind_value2);
		}
	};

	if (const auto aim_category = main_window->new_category("Aim Assistance"))
	{
		const auto legit_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_CROSSHAIRS, "Legit");
		const auto rage_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_SKULL, "Rage");
		const auto anti_aim_tab = aim_category->new_tab(FONT_FA_SOLID_32, ICON_FA_REDO, "Anti-aim");

		prepare_tab(legit_tab);
		prepare_tab(rage_tab);
		prepare_tab(anti_aim_tab);
	}

	if (const auto visual_category = main_window->new_category("Visualizations"))
	{
		const auto players_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_USER, "Players");
		const auto weapons_tab = visual_category->new_tab(FONT_WEAPONS_32, ICON_WEAPON_FIVESEVEN, "Weapons");
		const auto world_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_GLOBE_AMERICAS, "World");
		const auto view_tab = visual_category->new_tab(FONT_FA_SOLID_32, ICON_FA_EYE, "View");

		prepare_tab(players_tab);
		prepare_tab(weapons_tab);
		prepare_tab(world_tab);
		prepare_tab(view_tab);
	}

	if (const auto misc_category = main_window->new_category("Miscellaneous"))
	{
		const auto main_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_TOOLS, "Main");
		const auto presets_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_COGS, "Presets");
		const auto scripts_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_FILE_CODE, "Scripts");
		const auto inventory_tab = misc_category->new_tab(FONT_FA_SOLID_32, ICON_FA_WALLET, "Inventory");

		prepare_tab(main_tab);
		prepare_tab(presets_tab);
		prepare_tab(scripts_tab);
		prepare_tab(inventory_tab);

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
