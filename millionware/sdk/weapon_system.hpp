#pragma once

#include "macros.hpp"
#include "weapon_info.hpp"

enum e_weapon_type {
    WEAPON_TYPE_KNIFE,
    WEAPON_TYPE_PISTOL,
    WEAPON_TYPE_SUBMACHINEGUN,
    WEAPON_TYPE_RIFLE,
    WEAPON_TYPE_SHOTGUN,
    WEAPON_TYPE_SNIPER_RIFLE,
    WEAPON_TYPE_MACHINEGUN,
    WEAPON_TYPE_C4,
    WEAPON_TYPE_PLACEHOLDER,
    WEAPON_TYPE_GRENADE,
    WEAPON_TYPE_UNKNOWN
};

class c_weapon_system {
public:
	VIRTUAL_METHOD(weapon_info_t*, get_weapon_info, 2, (int item_definition_index), item_definition_index);
};