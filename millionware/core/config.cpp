#include "../sdk/color.hpp"
#include "../utils/hash/hash.hpp"
#include "config.hpp"

void config::initialize() {
	get<color_t>(FNV_CT("ui.accent")) = color_t(222, 7, 208);
	get<float>(FNV_CT("misc.other.override_fov")) = 90.f;
}

void config::save_to(std::string_view name) {
	// @todo: implement
}

void config::load_from(std::string_view name) {
	// @todo: implement
}
