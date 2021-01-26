#include <minhook.h>
#include <mutex>

#include "../error/error.hpp"
#include "hook.hpp"

static std::once_flag minhook_initialised;

c_hook::c_hook(uintptr_t target, uintptr_t detour) {
	std::call_once(minhook_initialised, []() {
		MH_Initialize();
		});

	target_ = target;
	detour_ = detour;
	original_ = 0;
}

void c_hook::enable() {
	MH_CreateHook((LPVOID)target_, (LPVOID)detour_, (LPVOID*)&original_);
	MH_EnableHook((LPVOID)target_);
}

void c_hook::disable() {
	MH_DisableHook((LPVOID)target_);
	MH_RemoveHook((LPVOID)target_);
}