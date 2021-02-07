#include "../sdk/color.hpp"
#include "../utils/hash/hash.hpp"
#include "../utils/input/input.hpp"
#include "config.hpp"

void config::initialize() {
	get<color_t>(FNV_CT("ui.accent")) = color_t(222, 7, 208);

	get<color_t>(FNV_CT("visuals.enemy.health_color")) = color_t(145, 237, 59);
	get<color_t>(FNV_CT("visuals.enemy.health_text_color")) = color_t(255, 255, 255);

	get<color_t>(FNV_CT("visuals.enemy.chams.color")) = color_t(255, 102, 0);
	get<color_t>(FNV_CT("visuals.enemy.chams_hidden.color")) = color_t(222, 7, 208);

	get<int>(FNV_CT("legitbot.hitbox_method")) = 0;
	get<int>(FNV_CT("legitbot.hitbox")) = 0;

	get<float>(FNV_CT("misc.other.override_fov")) = 90.f;
	get<float>(FNV_CT("visuals.other.general.flash_alpha")) = 255.f;
	get<float>(FNV_CT("visuals.world.nightmode_intensity")) = 0.075f;


	input::get_hotkey(FNV_CT("legitbot.hotkey")).type = HOTKEY_PRESSED;
	input::get_hotkey(FNV_CT("legitbot.hotkey")).key = VK_XBUTTON2;

	input::get_hotkey(FNV_CT("triggerbot.hotkey")).type = HOTKEY_PRESSED;
	input::get_hotkey(FNV_CT("triggerbot.hotkey")).key = VK_XBUTTON1;
}

void config::save_to(std::string_view name) {
	// @todo: implement
}

void config::load_from(std::string_view name) {
	// @todo: implement
}

void config::uninitialize() {
	get<float>(FNV_CT("visuals.world.nightmode_intensity")) = 1.f;
}
