#include "../core/hooks.hpp"
#include "../core/config.hpp"

bool __fastcall hooks::override_config_hook(uintptr_t ecx, uintptr_t edx, material_system_config_t* config, bool update) {
	return hooks::override_config.get_original<decltype(&override_config_hook)>()(ecx, edx, config, update);
}