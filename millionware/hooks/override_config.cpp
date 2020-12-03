#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../core/config.hpp"

#include "../utils/render.hpp"

bool __fastcall hooks::override_config_hook(uintptr_t ecx, uintptr_t edx, c_material_system_config* config, bool update) {
	const auto _ = std::lock_guard(hooks::override_config.call_mutex);

	config->full_bright = config::get<bool>(FNV_CT("misc.other.world.full_bright"));

	return reinterpret_cast<decltype(&override_config_hook)>(hooks::override_config.original)(ecx, edx, config, update);
}