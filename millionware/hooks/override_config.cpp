#include "../core/hooks.hpp"
#include "../core/config.hpp"
#include "../utils/hash.hpp"

bool __fastcall hooks::override_config_hook(uintptr_t ecx, uintptr_t edx, material_system_config_t* config, bool update) {
	const auto _ = std::lock_guard(hooks::override_config.call_mutex);

	return reinterpret_cast<decltype(&override_config_hook)>(hooks::override_config.original)(ecx, edx, config, update);
}