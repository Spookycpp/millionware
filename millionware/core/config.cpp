#include "../sdk/color.hpp"
#include "../utils/hash.hpp"
#include "config.hpp"

void config::initialize() {
	// @hint: define default config there
	get<color_t>(FNV_CT("ui.accent")) = color_t(222, 7, 208);
	get<float>(FNV_CT("misc.other.flash_alpha")) = 255.f;
}

void config::save_to(std::string_view name) {
	// @todo: implement
}

void config::load_from(std::string_view name) {
	// @todo: implement
}
