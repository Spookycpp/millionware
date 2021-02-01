#include "../core/config.hpp"
#include "../core/interfaces.hpp"
#include "../gui/gui.hpp"
#include "../utils/hash/hash.hpp"
#include "../utils/xorstr/xorstr.hpp"
#include "menu.hpp"

void menu::frame() {
	if (gui::begin_window()) {

		if (gui::tab(STR_ENC(L"Aimbot"))) {
			if (gui::begin_sub_tab(STR_ENC(L"Pistols"))) {

				if (gui::begin_group(STR_ENC(L"Weapon groups"))) {
					gui::checkbox(STR_ENC(L"Enabled"), config::get<bool>(FNV_CT("legitbot.weapon_groups")));
					// @Note: Dropdowns yet to be added so once it's implemented, add functionality for:
					// Pistols, Rifiles, Snipers, & Other (smg, shotgun, etc).
					gui::end_group();
				}

				if (gui::begin_group(STR_ENC(L"Backtrack"))) {

					if (gui::checkbox(STR_ENC(L"Enabled"), config::get<bool>(FNV_CT("legitbot.backtrack.enabled")))) {
						gui::slider(STR_ENC(L"Field of view"), config::get<float>(FNV_CT("legitbot.backtrack.fov")), 0.f, 180.0f, STR_ENC(L"{:.0f}°"));
						gui::slider(STR_ENC(L"Max time"), config::get<int>(FNV_CT("legitbot.backtrack.time")), 10, 200, STR_ENC(L"{}ms"));
					}

					gui::end_group();
				}

				if (gui::begin_group(STR_ENC(L"General"))) {
					// Hotkey here, make functionality for "on hotkey, off hotkey, toggle & always on".
					gui::checkbox(STR_ENC(L"Enabled"), config::get<bool>(FNV_CT("legitbot.enabled")));

					if (gui::checkbox(STR_ENC(L"Aim assist"), config::get<bool>(FNV_CT("legitbot.assist.enabled")))) {
						gui::slider(STR_ENC(L"Field of view"), config::get<float>(FNV_CT("legitbot.assist.fov")), 0.f, 180.0f, STR_ENC(L"{:.0f}°"));
						gui::slider(STR_ENC(L"Strength"), config::get<float>(FNV_CT("legitbot.assist.strength")), 0.f, 100.f, STR_ENC(L"{:.0f}%"));
					}

					if (gui::checkbox(STR_ENC(L"Flick bot"), config::get<bool>(FNV_CT("legitbot.flick_bot.enabled")))) {
						gui::slider(STR_ENC(L"Field of view "), config::get<float>(FNV_CT("legitbot.flick_bot.fov")), 0.f, 180.0f, STR_ENC(L"{:.0f}°"));
						gui::slider(STR_ENC(L"Strength "), config::get<float>(FNV_CT("legitbot.flick_bot.hit_chance")), 0.f, 100.f, STR_ENC(L"{:.0f}%"));
					}

					gui::end_group();
				}
				
				if (gui::begin_group(STR_ENC(L"Triggerbot"))) {
					if (gui::checkbox(STR_ENC(L"Triggerbot"), config::get<bool>(FNV_CT("legitbot.triggerbot.enabled")))) {
						gui::checkbox(STR_ENC(L"Friendly fire"), config::get<bool>(FNV_CT("legitbot.triggerbot.check_team")));
						gui::checkbox(STR_ENC(L"Smoke check"), config::get<bool>(FNV_CT("legitbot.triggerbot.check_smoked")));
						gui::checkbox(STR_ENC(L"Flash check"), config::get<bool>(FNV_CT("legitbot.triggerbot.check_flashed")));
						gui::slider(STR_ENC(L"Hit chance"), config::get<int>(FNV_CT("legitbot.triggerbot.hit_chance")), 0, 100, STR_ENC(L"{}%"));
						gui::slider(STR_ENC(L"Delay"), config::get<int>(FNV_CT("legitbot.triggerbot.delay")), 0, 200, STR_ENC(L"{}ms"));

					}
					gui::end_group();
				}
			}
		}

		if (gui::tab(STR_ENC(L"Visuals"))) {
			if (gui::begin_sub_tab(STR_ENC(L"Enemy"))) {
				if (gui::begin_group(STR_ENC(L"ESP"))) {
					gui::checkbox(STR_ENC(L"Enabled"), config::get<bool>(FNV_CT("visuals.enemy.enabled")));
					gui::checkbox(STR_ENC(L"Bounding box"), config::get<bool>(FNV_CT("visuals.enemy.bounding_box")));
					gui::checkbox(STR_ENC(L"Name"), config::get<bool>(FNV_CT("visuals.enemy.name")));
					gui::checkbox(STR_ENC(L"Health"), config::get<bool>(FNV_CT("visuals.enemy.health")));
					gui::checkbox(STR_ENC(L"Armor"), config::get<bool>(FNV_CT("visuals.enemy.armor")));
					gui::checkbox(STR_ENC(L"Weapon text"), config::get<bool>(FNV_CT("visuals.enemy.weapon_text")));
					gui::checkbox(STR_ENC(L"Weapon icon"), config::get<bool>(FNV_CT("visuals.enemy.weapon_icon")));
					gui::checkbox(STR_ENC(L"Ammo"), config::get<bool>(FNV_CT("visuals.enemy.ammo")));
					gui::checkbox(STR_ENC(L"Skeleton"), config::get<bool>(FNV_CT("visuals.enemy.skeleton")));
					gui::checkbox(STR_ENC(L"Head dot"), config::get<bool>(FNV_CT("visuals.enemy.head_dot")));
					gui::checkbox(STR_ENC(L"Line of sight"), config::get<bool>(FNV_CT("visuals.enemy.line_of_sight")));
					gui::end_group();
				}

				if (gui::begin_group(STR_ENC(L"Chams"))) {
					gui::attach_color_picker(STR_ENC(L"Visible color"), config::get<color_t>(FNV_CT("visuals.enemy.chams.color")), true);
					gui::checkbox(STR_ENC(L"Player"), config::get<bool>(FNV_CT("visuals.enemy.chams")));
					gui::attach_color_picker(STR_ENC(L"Hidden color"), config::get<color_t>(FNV_CT("visuals.enemy.chams_hidden.color")), true);
					gui::checkbox(STR_ENC(L"Player (hidden)"), config::get<bool>(FNV_CT("visuals.enemy.chams_hidden")));
					gui::end_group();

				}
			}

			if (gui::begin_sub_tab(STR_ENC(L"Friendly"))) {
				if (gui::begin_group(STR_ENC(L"ESP"))) {
					gui::checkbox(STR_ENC(L"test"), config::get<bool>(FNV_CT("test")));
					gui::end_group();
				}
			}

			if (gui::begin_sub_tab(STR_ENC(L"Local"))) {
				if (gui::begin_group(STR_ENC(L"ESP"))) {
					gui::checkbox(STR_ENC(L"test2"), config::get<bool>(FNV_CT("test2")));
					gui::end_group();
				}
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

				if (gui::begin_group(STR_ENC(L"Other"))) {
					gui::checkbox(STR_ENC(L"Remove post processing"), config::get<bool>(FNV_CT("visuals.other.general.post_processing")));
					gui::checkbox(STR_ENC(L"Remove panorama blur"), config::get<bool>(FNV_CT("visuals.other.general.panorama_blur")));
					gui::checkbox(STR_ENC(L"Force crosshair"), config::get<bool>(FNV_CT("visuals.other.general.force_crosshair")));
					gui::slider(STR_ENC(L"Flash alpha"), config::get<float>(FNV_CT("visuals.other.general.flash_alpha")), 0.0f, 255.0f, STR_ENC(L"{:.0f}"));
					gui::slider(STR_ENC(L"Aspect ratio"), config::get<float>(FNV_CT("visuals.other.general.aspect_ratio")), 0.f, 5.f);

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
