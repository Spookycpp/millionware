#include "../sdk/color.hpp"
#include "../utils/hash/hash.hpp"
#include "config.hpp"

void config::initialize() {
	get<color_t>(FNV_CT("ui.accent")) = color_t(222, 7, 208);

	get<color_t>(FNV_CT("visuals.enemy.health_color")) = color_t(145, 237, 59);
	get<color_t>(FNV_CT("visuals.enemy.health_text_color")) = color_t(255, 255, 255);

	get<color_t>(FNV_CT("visuals.enemy.chams.color")) = color_t(255, 102, 0);
	get<color_t>(FNV_CT("visuals.enemy.chams_hidden.color")) = color_t(222, 7, 208);

	get<int>(FNV_CT("legitbot.hitbox_method")) = 1;
	get<int>(FNV_CT("legitbot.flick_bot.enabled")) = 1;

	get<float>(FNV_CT("misc.other.override_fov")) = 90.f;
}

void config::save_to(std::string_view name) {
	// @todo: implement
}

void config::load_from(std::string_view name) {
	// @todo: implement
}
