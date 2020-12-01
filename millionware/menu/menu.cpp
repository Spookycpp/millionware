#include "../core/config.hpp"
#include "../core/interfaces.hpp"
#include "../gui/gui.hpp"
#include "../utils/hash.hpp"
#include "../utils/xorstr.hpp"
#include "menu.hpp"

void menu::frame() {
	gui::window(XORSTR(L"millionware"), []() {
		gui::tab(e_texture::LEGIT_22, []() {
			const auto category_cb = [](uint32_t weapon_hash) {
				return [weapon_hash]() {
					gui::group(XORSTR(L"Aimbot"), [weapon_hash]() {
						gui::checkbox(XORSTR(L"Enable"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.aimbot.enable")));
						gui::slider(XORSTR(L"Slider 1"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.aimbot.slider1")), 0, 100);
						gui::slider(XORSTR(L"Slider 2"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.aimbot.slider2")), 0.0f, 45.0f);
					});
					gui::group(XORSTR(L"Triggerbot"), [weapon_hash]() {
						gui::checkbox(XORSTR(L"Enable"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.trigger.enable")));
						gui::slider(XORSTR(L"Slider 1"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.trigger.slider1")), 0, 100);
						gui::slider(XORSTR(L"Slider 2"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.trigger.slider2")), 0.0f, 45.0f);
					});
					gui::group(XORSTR(L"Target selection"), [weapon_hash]() {
						gui::checkbox(XORSTR(L"Enable"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.target.enable")));
						gui::slider(XORSTR(L"Slider 1"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.target.slider1")), 0, 100);
						gui::slider(XORSTR(L"Slider 2"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.target.slider2")), 0.0f, 45.0f);
					});
					gui::group(XORSTR(L"Backtracking"), [weapon_hash]() {
						gui::checkbox(XORSTR(L"Enable"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.backtrack.enable")));
						gui::slider(XORSTR(L"Slider 1"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.backtrack.slider1")), 0, 100);
						gui::slider(XORSTR(L"Slider 2"), HASH_FNV(weapon_hash, HASH_FNV_CT(".legit.backtrack.slider2")), 0.0f, 45.0f);
					});
				};
			};

			gui::category(XORSTR(L"Pistols"), category_cb(HASH_FNV_CT("pistols")));
			gui::category(XORSTR(L"Shotguns"), category_cb(HASH_FNV_CT("shotguns")));
			gui::category(XORSTR(L"SMG"), category_cb(HASH_FNV_CT("smg")));
			gui::category(XORSTR(L"Rifles"), category_cb(HASH_FNV_CT("rifles")));
			gui::category(XORSTR(L"AWP"), category_cb(HASH_FNV_CT("awp")));
			gui::category(XORSTR(L"Scout"), category_cb(HASH_FNV_CT("scout")));
			gui::category(XORSTR(L"Auto snipers"), category_cb(HASH_FNV_CT("auto")));
		});
		gui::tab(e_texture::VISUALS_22, []() {
			const auto player_category_cb = []() {
				gui::group(XORSTR(L"ESP"), []() {});
				gui::group(XORSTR(L"Models"), []() {});
			};
			const auto items_category_cb = []() {
				gui::group(XORSTR(L"ESP"), []() {});
				gui::group(XORSTR(L"Models"), []() {});
			};
			const auto viewmodel_category_cb = []() {
				gui::group(XORSTR(L"General"), []() {});
				gui::group(XORSTR(L"Models"), []() {});
			};
			const auto world_category_cb = []() {
				gui::group(XORSTR(L"Environment"), []() {});
				gui::group(XORSTR(L"Weather"), []() {});
				gui::group(XORSTR(L"Props and walls"), []() {});
			};
			const auto removables_category_cb = []() {
				gui::group(XORSTR(L"World"), []() {});
				gui::group(XORSTR(L"Post-processing"), []() {});
				gui::group(XORSTR(L"View model"), []() {});
			};

			gui::category(XORSTR(L"Enemies"), player_category_cb);
			gui::category(XORSTR(L"Allies"), player_category_cb);
			gui::category(XORSTR(L"Dropped items"), items_category_cb);
			gui::category(XORSTR(L"View model"), viewmodel_category_cb);
			gui::category(XORSTR(L"World"), world_category_cb);
			gui::category(XORSTR(L"Removables"), removables_category_cb);
		});
		gui::tab(e_texture::MISC_22, []() {
			const auto category_cb = []() {
				gui::group(XORSTR(L"General"), []() {});
			};

			gui::category(XORSTR(L"Matchmaking"), category_cb);
			gui::category(XORSTR(L"Movement"), category_cb);
		});
		gui::tab(e_texture::CHANGERS_22, []() {
			const auto category_cb = []() {
				gui::group(XORSTR(L"General"), []() {});
			};

			gui::category(XORSTR(L"Knife and gloves"), category_cb);
			gui::category(XORSTR(L"Weapon skins"), category_cb);
			gui::category(XORSTR(L"Agents"), category_cb);
		});
		gui::tab(e_texture::PROFILE_22, []() {
			const auto category_cb = []() {
				gui::group(XORSTR(L"General"), []() {});
			};

			gui::category(XORSTR(L"User settings"), category_cb);
			gui::category(XORSTR(L"Presets"), category_cb);
			gui::category(XORSTR(L"Scripts"), category_cb);
		});
	});
}
