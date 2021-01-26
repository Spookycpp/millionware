#include "../core/config.hpp"
#include "../core/interfaces.hpp"
#include "../gui/gui.hpp"
#include "../utils/hash/hash.hpp"
#include "../utils/xorstr/xorstr.hpp"
#include "menu.hpp"

void menu::frame() {
	const auto test_tab = []() {
		const auto test_sub_tab = []() {
			const auto test_group = []() {
				// gui::attach_color_picker(STR_ENC(L"Color picker #1"), config::get<color_t>(FNV_CT("ui.accent")), true);
				// gui::attach_color_picker(STR_ENC(L"Color picker #2"), config::get<color_t>(FNV_CT("ui.accent")), true);
				// gui::attach_color_picker(STR_ENC(L"Color picker #3"), config::get<color_t>(FNV_CT("ui.accent")), true);
				gui::checkbox(STR_ENC(L"Test checkbox"), config::get<bool>(FNV_CT("checkbox")));

				//gui::slider(STR_ENC(L"Trigger bot delay"), config::get<int>(FNV_CT("tbot.delay")), 0, 250, STR_ENC(L"{}ms"));
				//gui::slider(STR_ENC(L"Field of view override"), config::get<float>(FNV_CT("visual.fov")), 45.0f, 120.0f, STR_ENC(L"{:.0f}°"));
				//gui::slider(STR_ENC(L"Wall transparency"), config::get<int>(FNV_CT("visual.wt")), 0, 100, STR_ENC(L"{}%"));

				gui::end_group();
			};

			if (gui::begin_group(STR_ENC(L"Test group 1"))) test_group();
			if (gui::begin_group(STR_ENC(L"Test group 2"))) test_group();
			if (gui::begin_group(STR_ENC(L"Test group 3"))) test_group();
			if (gui::begin_group(STR_ENC(L"Test group 4"))) test_group();

			gui::end_sub_tab();
		};

		if (gui::begin_sub_tab(STR_ENC(L"Sub-tab 1"))) test_sub_tab();
		if (gui::begin_sub_tab(STR_ENC(L"Sub-tab 2"))) test_sub_tab();
		if (gui::begin_sub_tab(STR_ENC(L"Sub-tab 3"))) test_sub_tab();
		if (gui::begin_sub_tab(STR_ENC(L"Sub-tab 4"))) test_sub_tab();
		if (gui::begin_sub_tab(STR_ENC(L"Sub-tab 5"))) test_sub_tab();
	};

	
	if (gui::begin_window()) {

		if (gui::tab(STR_ENC(L"Aimbot"))) {
			if (gui::begin_sub_tab(STR_ENC(L"Pistols"))) {

				if (gui::begin_group(STR_ENC(L"Legitbot"))) {
					gui::checkbox(STR_ENC(L"Enabled"), config::get<bool>(FNV_CT("aimbot.legit.enabled")));
					//@note: once hotkeys are added use them here :D
					gui::slider(STR_ENC(L"Field of view"), config::get<int>(FNV_CT("aimbot.legit.fov")), 0, 180, STR_ENC(L"{}°"));
					gui::slider(STR_ENC(L"Strength"), config::get<int>(FNV_CT("aimbot.legit.strength")), 0, 100, STR_ENC(L"{}%"));
					gui::end_group();
				}

				if (gui::begin_group(STR_ENC(L"Backtrack"))) {
					gui::checkbox(STR_ENC(L"Enabled"), config::get<bool>(FNV_CT("aimbot.backtrack.enabled")));
					gui::slider(STR_ENC(L"Field of view"), config::get<int>(FNV_CT("aimbot.backtrack.fov")), 0, 100, STR_ENC(L"{}°"));
					gui::slider(STR_ENC(L"Max time"), config::get<int>(FNV_CT("aimbot.backtrack.max_time")), 0, 200, STR_ENC(L"{}ms"));
					gui::end_group();
				}

				if (gui::begin_group(STR_ENC(L"Triggerbot"))) {
					gui::checkbox(STR_ENC(L"Enabled"), config::get<bool>(FNV_CT("aimbot.triggerbot.enabled")));
					//@note: once hotkeys are added use them here :D
					gui::checkbox(STR_ENC(L"Target teammates"), config::get<bool>(FNV_CT("aimbot.triggerbot.teammates")));
					gui::checkbox(STR_ENC(L"Ignore behind smoke"), config::get<bool>(FNV_CT("aimbot.triggerbot.behind_smoke")));
					gui::checkbox(STR_ENC(L"Ignore while flashed"), config::get<bool>(FNV_CT("aimbot.triggerbot.flash_check")));
					gui::slider(STR_ENC(L"Hitchance"), config::get<int>(FNV_CT("aimbot.triggerbot.hitchance")), 0, 100, STR_ENC(L"{}"));
					gui::slider(STR_ENC(L"Delay"), config::get<int>(FNV_CT("aimbot.triggerbot.delay")), 0, 1000, STR_ENC(L"{}ms"));
					gui::end_group();
				}
			}
		}

		if (gui::tab(STR_ENC(L"Visuals"))) {
			if (gui::begin_sub_tab(STR_ENC(L"Local"))) {
			}

			if (gui::begin_sub_tab(STR_ENC(L"Enemy"))) {
			}

			if (gui::begin_sub_tab(STR_ENC(L"Friendly"))) {
			}
		}

		if (gui::tab(STR_ENC(L"Miscellaneous"))) {
			if (gui::begin_sub_tab(STR_ENC(L"General"))) {
				if (gui::begin_group(STR_ENC(L"Movement"))) {
					gui::checkbox(STR_ENC(L"Bunny hop"), config::get<bool>(FNV_CT("misc.movement.bunny_hop")));
					gui::checkbox(STR_ENC(L"Infinite duck"), config::get<bool>(FNV_CT("misc.movement.infinite_duck")));
					gui::checkbox(STR_ENC(L"Jump bug"), config::get<bool>(FNV_CT("misc.movement.jump_bug")));
					gui::checkbox(STR_ENC(L"Edge bug"), config::get<bool>(FNV_CT("misc.movement.edge_bug")));
					gui::checkbox(STR_ENC(L"Edge jump"), config::get<bool>(FNV_CT("misc.movement.edge_jump")));
					gui::end_group();
				}

				if (gui::begin_group(STR_ENC(L"Miscellaneous"))) {
					gui::slider(STR_ENC(L"Field of view override"), config::get<float>(FNV_CT("misc.other.override_fov")), 45.0f, 120.0f, STR_ENC(L"{:.0f}°"));
					gui::checkbox(STR_ENC(L"Auto accept"), config::get<bool>(FNV_CT("misc.other.auto_accept")));
					gui::checkbox(STR_ENC(L"Auto pistol"), config::get<bool>(FNV_CT("misc.other.auto_pistol")));
					gui::checkbox(STR_ENC(L"Rank reveal"), config::get<bool>(FNV_CT("misc.other.rank_reveal")));
					gui::checkbox(STR_ENC(L"Ragdoll push"), config::get<bool>(FNV_CT("misc.other.ragdoll_push")));
					gui::checkbox(STR_ENC(L"Ragdoll float"), config::get<bool>(FNV_CT("misc.other.ragdoll_float")));
					gui::end_group();
				}
			}
		}

		if (gui::tab(STR_ENC(L"Changers"))) {
			if (gui::begin_sub_tab(STR_ENC(L"Knives"))) {
			}

			if (gui::begin_sub_tab(STR_ENC(L"Skins"))) {
			}

			if (gui::begin_sub_tab(STR_ENC(L"Gloves"))) {
			}

			if (gui::begin_sub_tab(STR_ENC(L"Models"))) {
			}
		}

		if (gui::tab(STR_ENC(L"Settings"))) {
		}

		gui::end_window();
	}
}
