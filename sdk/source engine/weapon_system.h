#pragma once

#include "macros.h"
#include "weapon_info.h"

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
    DECLARE_VFUNC(2, get_weapon_info(int item_definition_index), weapon_info_t *(__thiscall *) (void *, int) )(item_definition_index);
};
