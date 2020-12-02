#include "../core/config.hpp"
#include "../core/interfaces.hpp"
#include "../gui/gui.hpp"
#include "../utils/hash.hpp"
#include "../utils/xorstr.hpp"
#include "menu.hpp"

void menu::frame() {
	gui::window(XOR(L"millionware"), []() {
		gui::tab(e_texture::LEGIT_22, []() {
			const auto make_weapon_category = [](uint32_t weapon_hash) {
				return [weapon_hash]() {
					gui::group(XOR(L"Aimbot"), [weapon_hash]() {
						gui::checkbox(XOR(L"Enable"), FNV(weapon_hash, FNV_CT(".legit.aimbot.enable")));
						gui::slider(XOR(L"Slider 1"), FNV(weapon_hash, FNV_CT(".legit.aimbot.slider1")), 0, 100);
						gui::slider(XOR(L"Slider 2"), FNV(weapon_hash, FNV_CT(".legit.aimbot.slider2")), 0.0f, 45.0f);
					});
					gui::group(XOR(L"Triggerbot"), [weapon_hash]() {
						gui::checkbox(XOR(L"Enable"), FNV(weapon_hash, FNV_CT(".legit.trigger.enable")));
						gui::slider(XOR(L"Slider 1"), FNV(weapon_hash, FNV_CT(".legit.trigger.slider1")), 0, 100);
						gui::slider(XOR(L"Slider 2"), FNV(weapon_hash, FNV_CT(".legit.trigger.slider2")), 0.0f, 45.0f);
					});
					gui::group(XOR(L"Target selection"), [weapon_hash]() {
						gui::checkbox(XOR(L"Enable"), FNV(weapon_hash, FNV_CT(".legit.target.enable")));
						gui::slider(XOR(L"Slider 1"), FNV(weapon_hash, FNV_CT(".legit.target.slider1")), 0, 100);
						gui::slider(XOR(L"Slider 2"), FNV(weapon_hash, FNV_CT(".legit.target.slider2")), 0.0f, 45.0f);
					});
					gui::group(XOR(L"Backtracking"), [weapon_hash]() {
						gui::checkbox(XOR(L"Enable"), FNV(weapon_hash, FNV_CT(".legit.backtrack.enable")));
						gui::slider(XOR(L"Slider 1"), FNV(weapon_hash, FNV_CT(".legit.backtrack.slider1")), 0, 100);
						gui::slider(XOR(L"Slider 2"), FNV(weapon_hash, FNV_CT(".legit.backtrack.slider2")), 0.0f, 45.0f);
					});
				};
			};

			gui::category(XOR(L"Pistols"), make_weapon_category(FNV_CT("pistols")));
			gui::category(XOR(L"Shotguns"), make_weapon_category(FNV_CT("shotguns")));
			gui::category(XOR(L"SMG"), make_weapon_category(FNV_CT("smg")));
			gui::category(XOR(L"Rifles"), make_weapon_category(FNV_CT("rifles")));
			gui::category(XOR(L"AWP"), make_weapon_category(FNV_CT("awp")));
			gui::category(XOR(L"Scout"), make_weapon_category(FNV_CT("scout")));
			gui::category(XOR(L"Auto snipers"), make_weapon_category(FNV_CT("auto")));
		});
		gui::tab(e_texture::VISUALS_22, []() {
			gui::category(XOR(L"Enemies"), []() {
				gui::group(XOR(L"ESP"), []() {});
				gui::group(XOR(L"Models"), []() {});
			});
			gui::category(XOR(L"Allies"), []() {
				gui::group(XOR(L"ESP"), []() {});
				gui::group(XOR(L"Models"), []() {});
			});
			gui::category(XOR(L"Dropped items"), []() {
				gui::group(XOR(L"ESP"), []() {});
				gui::group(XOR(L"Models"), []() {});
			});
			gui::category(XOR(L"View model"), []() {
				gui::group(XOR(L"General"), []() {});
				gui::group(XOR(L"Models"), []() {});
			});
			gui::category(XOR(L"World"), []() {
				gui::group(XOR(L"Environment"), []() {});
				gui::group(XOR(L"Weather"), []() {});
				gui::group(XOR(L"Props and walls"), []() {});
			});
			gui::category(XOR(L"Removables"), []() {
				gui::group(XOR(L"World"), []() {});
				gui::group(XOR(L"Post-processing"), []() {});
				gui::group(XOR(L"View model"), []() {});
			});
		});
		gui::tab(e_texture::MISC_22, []() {
			gui::category(XOR(L"General"), []() {
				gui::group(XOR(L"Movement"), []() {
					gui::checkbox(XOR(L"Bunny hop"), FNV_CT("misc.movement.bunny_hop"));
					gui::checkbox(XOR(L"Infinite duck"), FNV_CT("misc.movement.infinite_duck"));
					gui::checkbox(XOR(L"Auto strafer"), FNV_CT("misc.movement.auto_strafer"));
				});
			});
		});
		gui::tab(e_texture::CHANGERS_22, []() {
			gui::category(XOR(L"Knife and gloves"), []() {
				gui::group(XOR(L"General"), []() {});
			});
			gui::category(XOR(L"Weapon skins"), []() {
				gui::group(XOR(L"General"), []() {});
			});
			gui::category(XOR(L"Agents"), []() {
				gui::group(XOR(L"General"), []() {});
			});
		});
		gui::tab(e_texture::PROFILE_22, []() {
			gui::category(XOR(L"User settings"), []() {
				gui::group(XOR(L"General"), []() {});
			});
			gui::category(XOR(L"Presets"), []() {
				gui::group(XOR(L"General"), []() {});
			});
			gui::category(XOR(L"Scripts"), []() {
				gui::group(XOR(L"General"), []() {});
			});
		});
	});
}
