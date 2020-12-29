#include "../core/config.hpp"
#include "../core/interfaces.hpp"
#include "../gui/gui.hpp"
#include "../utils/hash.hpp"
#include "../utils/xorstr.hpp"
#include "menu.hpp"

void menu::frame() {
	const auto test_tab = []() {
		const auto test_sub_tab = []() {
			const auto test_group = []() {
				// gui::attach_color_picker(XOR(L"Color picker #1"), config::get<color_t>(FNV_CT("ui.accent")), true);
				// gui::attach_color_picker(XOR(L"Color picker #2"), config::get<color_t>(FNV_CT("ui.accent")), true);
				// gui::attach_color_picker(XOR(L"Color picker #3"), config::get<color_t>(FNV_CT("ui.accent")), true);
				gui::checkbox(XOR(L"Test checkbox"), config::get<bool>(FNV_CT("checkbox")));

				gui::slider(XOR(L"Trigger bot delay"), config::get<int>(FNV_CT("tbot.delay")), 0, 250, XOR(L"{}ms"));
				gui::slider(XOR(L"Field of view override"), config::get<float>(FNV_CT("visual.fov")), 45.0f, 120.0f, XOR(L"{:.0f}°"));
				gui::slider(XOR(L"Wall transparency"), config::get<int>(FNV_CT("visual.wt")), 0, 100, XOR(L"{}%"));

				gui::end_group();
			};

			if (gui::begin_group(XOR(L"Test group 1"))) test_group();
			if (gui::begin_group(XOR(L"Test group 2"))) test_group();
			if (gui::begin_group(XOR(L"Test group 3"))) test_group();
			if (gui::begin_group(XOR(L"Test group 4"))) test_group();

			gui::end_sub_tab();
		};

		if (gui::begin_sub_tab(XOR(L"Sub-tab 1"))) test_sub_tab();
		if (gui::begin_sub_tab(XOR(L"Sub-tab 2"))) test_sub_tab();
		if (gui::begin_sub_tab(XOR(L"Sub-tab 3"))) test_sub_tab();
		if (gui::begin_sub_tab(XOR(L"Sub-tab 4"))) test_sub_tab();
		if (gui::begin_sub_tab(XOR(L"Sub-tab 5"))) test_sub_tab();
	};

	
	if (gui::begin_window()) {

		if (gui::tab(XOR(L"Aimbot"))) {
			if (gui::begin_sub_tab(XOR(L"Pistols"))) {

				if (gui::begin_group(XOR(L"Legitbot"))) {
					gui::checkbox(XOR(L"Enabled"), config::get<bool>(FNV_CT("aimbot.legit.enabled")));
					//@note: once hotkeys are added use them here :D
					gui::slider(XOR(L"Field of view"), config::get<int>(FNV_CT("aimbot.legit.fov")), 0, 180, XOR(L"{}°"));
					gui::slider(XOR(L"Strength"), config::get<int>(FNV_CT("aimbot.legit.strength")), 0, 100, XOR(L"{}%"));
					gui::end_group();
				}

				if (gui::begin_group(XOR(L"Backtrack"))) {
					gui::checkbox(XOR(L"Enabled"), config::get<bool>(FNV_CT("aimbot.backtrack.enabled")));
					gui::slider(XOR(L"Field of view"), config::get<int>(FNV_CT("aimbot.backtrack.fov")), 0, 100, XOR(L"{}°"));
					gui::slider(XOR(L"Max time"), config::get<int>(FNV_CT("aimbot.backtrack.max_time")), 0, 200, XOR(L"{}ms"));
					gui::end_group();
				}

				if (gui::begin_group(XOR(L"Triggerbot"))) {
					gui::checkbox(XOR(L"Enabled"), config::get<bool>(FNV_CT("aimbot.triggerbot.enabled")));
					//@note: once hotkeys are added use them here :D
					gui::checkbox(XOR(L"Target teammates"), config::get<bool>(FNV_CT("aimbot.triggerbot.teammates")));
					gui::checkbox(XOR(L"Ignore behind smoke"), config::get<bool>(FNV_CT("aimbot.triggerbot.behind_smoke")));
					gui::checkbox(XOR(L"Ignore while flashed"), config::get<bool>(FNV_CT("aimbot.triggerbot.flash_check")));
					gui::slider(XOR(L"Hitchance"), config::get<int>(FNV_CT("aimbot.triggerbot.hitchance")), 0, 100, XOR(L"{}"));
					gui::slider(XOR(L"Delay"), config::get<int>(FNV_CT("aimbot.triggerbot.delay")), 0, 1000, XOR(L"{}ms"));
					gui::end_group();
				}
			}
		}

		if (gui::tab(XOR(L"Visuals"))) {
			if (gui::begin_sub_tab(XOR(L"Local"))) {
			}

			if (gui::begin_sub_tab(XOR(L"Enemy"))) {
			}

			if (gui::begin_sub_tab(XOR(L"Friendly"))) {
			}
		}

		if (gui::tab(XOR(L"Miscellaneous"))) {
			if (gui::begin_sub_tab(XOR(L"General"))) {
				if (gui::begin_group(XOR(L"Movement"))) {
					gui::checkbox(XOR(L"Bunny hop"), config::get<bool>(FNV_CT("misc.movement.bunny_hop")));
					gui::checkbox(XOR(L"Infinite duck"), config::get<bool>(FNV_CT("misc.movement.infinite_duck")));
					gui::end_group();
				}
			}
		}

		if (gui::tab(XOR(L"Changers"))) {
			if (gui::begin_sub_tab(XOR(L"Knives"))) {
			}

			if (gui::begin_sub_tab(XOR(L"Skins"))) {
			}

			if (gui::begin_sub_tab(XOR(L"Gloves"))) {
			}

			if (gui::begin_sub_tab(XOR(L"Models"))) {
			}
		}

		if (gui::tab(XOR(L"Settings"))) {
		}

		gui::end_window();
	}
}
