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
						gui::dropdown(XOR(L"Dropdown 1"), FNV(weapon_hash, FNV_CT(".legit.aimbot.dropdown1")), { XOR(L"Item 1"), XOR(L"Item 2"), XOR(L"Item 3"), XOR(L"Item 4") });
						gui::dropdown(XOR(L"Dropdown 2"), FNV(weapon_hash, FNV_CT(".legit.aimbot.dropdown2")), { XOR(L"Item 1"), XOR(L"Item 2"), XOR(L"Item 3"), XOR(L"Item 4") });
					});
					gui::group(XOR(L"Triggerbot"), [weapon_hash]() {
						gui::checkbox(XOR(L"Enable"), FNV(weapon_hash, FNV_CT(".legit.trigger.enable")));
						gui::slider(XOR(L"Slider 1"), FNV(weapon_hash, FNV_CT(".legit.trigger.slider1")), 0, 100);
						gui::slider(XOR(L"Slider 2"), FNV(weapon_hash, FNV_CT(".legit.trigger.slider2")), 0.0f, 45.0f);
						gui::dropdown(XOR(L"Dropdown 1"), FNV(weapon_hash, FNV_CT(".legit.aimbot.dropdown1")), { XOR(L"Item 1"), XOR(L"Item 2"), XOR(L"Item 3"), XOR(L"Item 4") });
						gui::dropdown(XOR(L"Dropdown 2"), FNV(weapon_hash, FNV_CT(".legit.aimbot.dropdown2")), { XOR(L"Item 1"), XOR(L"Item 2"), XOR(L"Item 3"), XOR(L"Item 4") });
					});
					gui::group(XOR(L"Target selection"), [weapon_hash]() {
						gui::checkbox(XOR(L"Enable"), FNV(weapon_hash, FNV_CT(".legit.target.enable")));
						gui::slider(XOR(L"Slider 1"), FNV(weapon_hash, FNV_CT(".legit.target.slider1")), 0, 100);
						gui::slider(XOR(L"Slider 2"), FNV(weapon_hash, FNV_CT(".legit.target.slider2")), 0.0f, 45.0f);
						gui::dropdown(XOR(L"Dropdown 1"), FNV(weapon_hash, FNV_CT(".legit.aimbot.dropdown1")), { XOR(L"Item 1"), XOR(L"Item 2"), XOR(L"Item 3"), XOR(L"Item 4") });
						gui::dropdown(XOR(L"Dropdown 2"), FNV(weapon_hash, FNV_CT(".legit.aimbot.dropdown2")), { XOR(L"Item 1"), XOR(L"Item 2"), XOR(L"Item 3"), XOR(L"Item 4") });
					});
					gui::group(XOR(L"Backtracking"), [weapon_hash]() {
						gui::checkbox(XOR(L"Enable"), FNV(weapon_hash, FNV_CT(".legit.backtrack.enable")));
						gui::slider(XOR(L"Slider 1"), FNV(weapon_hash, FNV_CT(".legit.backtrack.slider1")), 0, 100);
						gui::slider(XOR(L"Slider 2"), FNV(weapon_hash, FNV_CT(".legit.backtrack.slider2")), 0.0f, 45.0f);
						gui::dropdown(XOR(L"Dropdown 1"), FNV(weapon_hash, FNV_CT(".legit.aimbot.dropdown1")), { XOR(L"Item 1"), XOR(L"Item 2"), XOR(L"Item 3"), XOR(L"Item 4") });
						gui::dropdown(XOR(L"Dropdown 2"), FNV(weapon_hash, FNV_CT(".legit.aimbot.dropdown2")), { XOR(L"Item 1"), XOR(L"Item 2"), XOR(L"Item 3"), XOR(L"Item 4") });
					});
				};
			};

			gui::category(e_texture::WEAPON_USP_SILENCER_24, 57, 24, make_weapon_category(FNV_CT("pistols")));
			gui::category(e_texture::WEAPON_DEAGLE_24, 39, 24, make_weapon_category(FNV_CT("heavy_pistols")));
			gui::category(e_texture::WEAPON_P90_24, 49, 24, make_weapon_category(FNV_CT("smg")));
			gui::category(e_texture::WEAPON_MAG7_24, 47, 24, make_weapon_category(FNV_CT("shotguns")));
			gui::category(e_texture::WEAPON_AK47_24, 67, 24, make_weapon_category(FNV_CT("rifles")));
			gui::category(e_texture::WEAPON_SSG08_24, 75, 24, make_weapon_category(FNV_CT("scout")));
			gui::category(e_texture::WEAPON_SCAR20_24, 68, 24, make_weapon_category(FNV_CT("auto_snipers")));
			gui::category(e_texture::WEAPON_AWP_24, 82, 24, make_weapon_category(FNV_CT("heavy_pistols")));
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
			gui::category(XOR(L"Environment"), []() {
				gui::group(XOR(L"World"), []() {
					gui::checkbox(XOR(L"Nightmode"), FNV_CT("misc.other.world.night_mode"));
					gui::checkbox(XOR(L"Fullbright"), FNV_CT("misc.other.world.full_bright"));
				});
				gui::group(XOR(L"Props and walls"), []() {});
			});
			gui::category(XOR(L"Effects"), []() {
				gui::group(XOR(L"Post-processing"), []() {
					gui::checkbox(XOR(L"Remove panorama blur"), FNV_CT("misc.other.remove_panorama_blur"));
					gui::checkbox(XOR(L"Remove post processing"), FNV_CT("misc.other.remove_post_processing"));
					gui::checkbox(XOR(L"Force crosshair"), FNV_CT("misc.other.force_crosshair"));
					gui::slider(XOR(L"Flash alpha"), FNV_CT("misc.other.flash_alpha"), 0.f, 255.f);
					gui::slider(XOR(L"Aspect ratio"), FNV_CT("misc.other.aspect_ratio"), 0.f, 5.f);
					gui::slider(XOR(L"Override FOV"), FNV_CT("misc.other.override_fov"), 50.f, 150.f);
				});
			});
		});
		gui::tab(e_texture::MISC_22, []() {
			gui::category(XOR(L"General"), []() {
				gui::group(XOR(L"Miscellaneous"), []() {
					gui::checkbox(XOR(L"Bunny hop"), FNV_CT("misc.movement.bunny_hop"));
					gui::checkbox(XOR(L"Infinite duck"), FNV_CT("misc.movement.infinite_duck"));
					gui::checkbox(XOR(L"Auto strafer"), FNV_CT("misc.movement.auto_strafer"));
					gui::checkbox(XOR(L"Clan tag"), FNV_CT("misc.other.clantag"));
					gui::checkbox(XOR(L"Name spam"), FNV_CT("misc.other.name_spam"));
					gui::checkbox(XOR(L"Reveal money"), FNV_CT("misc.other.reveal_money"));
					gui::slider(XOR(L"Auto pistol"), FNV_CT("misc.other.auto_pistol_delay"), 0.f, 100.f);
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
