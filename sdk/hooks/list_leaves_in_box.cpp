#include "../source engine/leaf_system.h"
#include "../core/hooks/hooks.h"
#include "../core/patterns/patterns.h"

int __fastcall hooks::list_leaves_in_box(uintptr_t ecx, uintptr_t edx, const vector_t &mins, const vector_t &maxs, void *leaves_list, int max_leaves) {
	// if ( ( uint32_t ) _ReturnAddress( ) != patterns::get_insert_into_tree_list_leaves_in_box_call( ) )
	// 	return hook->get_original<decltype( &list_leaves_in_box )>( )( ecx, edx, mins, maxs, leaves_list, max_leaves );

	// const auto info = *( renderable_info_t ** ) ( ( uint32_t ) _AddressOfReturnAddress( ) + 0x14 );

	// if ( !info || !info->renderable )
	// 	return hook->get_original<decltype( &list_leaves_in_box )>( )( ecx, edx, mins, maxs, leaves_list, max_leaves );

	// @todo: Check renderable entity

	// info->flags &= ~0x100;
	// info->render_group |= 0xc0;

	// const auto map_min = vector_t { -16384.f, -16384.f, -16384.f };
	// const auto map_max = vector_t { 16384.f, 16384.f, 16384.f };

	return list_leaves_in_box_original(ecx, edx, mins, maxs, leaves_list, max_leaves);
}
