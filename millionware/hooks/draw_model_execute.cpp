#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/xorstr.hpp"

void __fastcall hooks::draw_model_execute_hook(uintptr_t ecx, uintptr_t edx, void* ctx, void* state, c_model_render_info* info, matrix3x4_t* matrix) {
	const auto _ = std::lock_guard(hooks::draw_model_execute.call_mutex);

	// @todo: implement

	reinterpret_cast<decltype(&draw_model_execute_hook)>(hooks::draw_model_execute.original)(ecx, edx, ctx, state, info, matrix);
}