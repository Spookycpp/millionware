#include "../core/cheat/cheat.h"
#include "../core/interfaces/interfaces.h"
#include "../core/hooks/hooks.h"

#include "../engine/security/xorstr.h"

#include "../source engine/entity.h"
#include "../source engine/material.h"
#include "../source engine/material_handle.h"

void __fastcall hooks::draw_model_execute(uintptr_t ecx, uintptr_t edx, void* ctx, void* state, c_model_render_info* info, matrix3x4_t* matrix) {

	draw_model_execute_original(ecx, edx, ctx, state, info, matrix);
}