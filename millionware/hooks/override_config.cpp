#include "../core/hooks.hpp"
#include "../core/config.hpp"
#include "../utils/hash.hpp"

bool __fastcall hooks::override_config_hook(uintptr_t ecx, uintptr_t edx, material_system_config_t* config, bool update) {
	const auto _ = std::lock_guard(hooks::override_config.call_mutex);

	config->full_bright = config::get<bool>(FNV_CT("misc.other.world.full_bright"));
	config->draw_gray= config::get<bool>(FNV_CT("misc.other.world.draw_grey"));
	config->show_low_res_image = config::get<bool>(FNV_CT("misc.other.world.low_resolution"));
	config->show_mip_levels = config::get<bool>(FNV_CT("misc.other.world.rainbow"));

	return reinterpret_cast<decltype(&override_config_hook)>(hooks::override_config.original)(ecx, edx, config, update);
}