#pragma once

#include "macros.hpp"
#include "weapon_info.hpp"

enum e_weapon_type {
	WEAPON_TYPE_INVALID,
	WEAPON_TYPE_KNIFE,
	WEAPON_TYPE_PISTOL,
	WEAPON_TYPE_TASER,
	WEAPON_TYPE_SHOTGUN,
	WEAPON_TYPE_SMG,
	WEAPON_TYPE_RIFLE,
	WEAPON_TYPE_MG,
	WEAPON_TYPE_SNIPER,
	WEAPON_TYPE_GRENADE,
};

class c_weapon_system {
public:
    VIRTUAL_METHOD(weapon_info_t*, get_weapon_info, 2, (int item_definition_index), item_definition_index);
};