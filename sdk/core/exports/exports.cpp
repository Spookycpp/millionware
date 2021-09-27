#include "exports.h"

//__declspec(dllexport) interface_entry_t *g_interfaces;
//__declspec(dllexport) pattern_entry_t *g_patterns;
//__declspec(dllexport) netvar_table_entry_t *g_netvar_tables;
//
// uint32_t exports::get_interface(uint32_t hash)
//{
//	for (auto entry = g_interfaces; entry; entry = entry->next)
//	{
//		if (entry->hash == hash)
//			return entry->address;
//	}
//
//	return 0;
//}
//
// uint32_t exports::get_pattern(uint32_t hash)
//{
//	for (auto entry = g_patterns; entry; entry = entry->next)
//	{
//		if (entry->hash == hash)
//			return entry->address;
//	}
//
//	return 0;
//}
