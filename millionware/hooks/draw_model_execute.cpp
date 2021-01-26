#include "../core/cheat.hpp"
#include "../core/hooks.hpp"
#include "../core/interfaces.hpp"
#include "../utils/xorstr/xorstr.hpp"

void __fastcall hooks::draw_model_execute_hook(uintptr_t ecx, uintptr_t edx, void* ctx, void* state, c_model_render_info* info, matrix3x4_t* matrix) {
	// @todo: implement

	hooks::draw_model_execute.get_original<decltype(&draw_model_execute_hook)>()(ecx, edx, ctx, state, info, matrix);
}