#pragma once

#include "../utils/hash.hpp"
#include "base_handle.hpp"
#include "client_class.hpp"
#include "macros.hpp"
#include "matrix.hpp"
#include "vector.hpp"

class c_collideable
{
public:
	VIRTUAL_METHOD(vector3_t&, get_mins, 1, ());
	VIRTUAL_METHOD(vector3_t&, get_maxs, 2, ());
};

class c_client_networkable
{
public:
	VIRTUAL_METHOD(client_class_t*, get_client_class, 2, ());
	VIRTUAL_METHOD(bool, is_dormant, 9, ());
	VIRTUAL_METHOD(int, index, 10, ());
};

class c_client_renderable
{
public:
	VIRTUAL_METHOD(vector3_t&, get_render_origin, 1, ());
	VIRTUAL_METHOD(vector3_t&, get_render_angles, 2, ());
	VIRTUAL_METHOD(bool, should_draw, 3, ());
	VIRTUAL_METHOD(bool, setup_bones, 13, (matrix3x4_t* bone_to_world_out, const int max_bones, const int bone_mask, const float current_time), bone_to_world_out, max_bones, bone_mask, current_time);
};

class c_entity
{
	uintptr_t unknown_;

public:
	c_client_renderable* renderable;
	c_client_networkable* networkable;

	NETVAR_DEFINITION(bool, is_spotted, HASH_FNV_CT("DT_BaseEntity"), HASH_FNV_CT("m_bSpotted"));
	NETVAR_DEFINITION(int, flags, HASH_FNV_CT("DT_BasePlayer"), HASH_FNV_CT("m_fFlags"));
	NETVAR_DEFINITION(int, team_num, HASH_FNV_CT("DT_BaseEntity"), HASH_FNV_CT("m_iTeamNum"));
	NETVAR_DEFINITION(vector3_t, origin, HASH_FNV_CT("DT_BaseEntity"), HASH_FNV_CT("m_vecOrigin"));
	NETVAR_DEFINITION(vector3_t, view_offset, HASH_FNV_CT("DT_CSPlayer"), HASH_FNV_CT("m_vecViewOffset[0]"));
	NETVAR_DEFINITION(matrix3x4_t, entity_to_world_matrix, -48, HASH_FNV_CT("DT_BaseEntity"), HASH_FNV_CT("m_CollisionGroup"));
	NETVAR_DEFINITION(c_base_handle, owner_handle, HASH_FNV_CT("DT_BaseEntity"), HASH_FNV_CT("m_hOwnerEntity"));

  VIRTUAL_METHOD(c_collideable*, get_collideable, 3, ());
  VIRTUAL_METHOD(bool, is_player, 157, ());
  VIRTUAL_METHOD(bool, is_weapon, 165, ());

  class c_player* as_player();
  class c_weapon* as_weapon();
};

class c_player : public c_entity
{
public:
  NETVAR_DEFINITION(bool, has_defuser, HASH_FNV_CT("DT_CSPlayer"), HASH_FNV_CT("m_bHasDefuser"));
  NETVAR_DEFINITION(bool, has_gun_game_immunity, HASH_FNV_CT("DT_CSPlayer"), HASH_FNV_CT("m_bGunGameImmunity"));
  NETVAR_DEFINITION(bool, has_helmet, HASH_FNV_CT("DT_CSPlayer"), HASH_FNV_CT("m_bHasHelmet"));
  NETVAR_DEFINITION(bool, is_scoped, HASH_FNV_CT("DT_CSPlayer"), HASH_FNV_CT("m_bIsScoped"));;
  NETVAR_DEFINITION(bool, is_defusing, HASH_FNV_CT("DT_CSPlayer"), HASH_FNV_CT("m_bIsDefusing"));;
  NETVAR_DEFINITION(float, flash_duration, HASH_FNV_CT("DT_CSPlayer"), HASH_FNV_CT("m_flFlashDuration"));
  NETVAR_DEFINITION(float, flash_alpha, HASH_FNV_CT("DT_CSPlayer"), HASH_FNV_CT("m_flFlashMaxAlpha"));
  NETVAR_DEFINITION(int, shots_fired, HASH_FNV_CT("DT_CSPlayer"), HASH_FNV_CT("m_iShotsFired"));
  NETVAR_DEFINITION(int, armor, HASH_FNV_CT("DT_CSPlayer"), HASH_FNV_CT("m_ArmorValue"));
  NETVAR_DEFINITION(int, health, HASH_FNV_CT("DT_BasePlayer"), HASH_FNV_CT("m_iHealth"));
  NETVAR_DEFINITION(int, life_state, HASH_FNV_CT("DT_BasePlayer"), HASH_FNV_CT("m_lifeState"));
  NETVAR_DEFINITION(int, tick_base, HASH_FNV_CT("DT_BasePlayer"), HASH_FNV_CT("m_nTickBase"));
  NETVAR_DEFINITION(vector3_t, eye_angles, HASH_FNV_CT("DT_CSPlayer"), HASH_FNV_CT("m_angEyeAngles[0]"));
  NETVAR_DEFINITION(vector3_t, punch_angle, HASH_FNV_CT("DT_BasePlayer"), HASH_FNV_CT("m_viewPunchAngle"));
  NETVAR_DEFINITION(vector3_t, aim_punch_angle, HASH_FNV_CT("DT_BasePlayer"), HASH_FNV_CT("m_aimPunchAngle"));
  NETVAR_DEFINITION(vector3_t, velocity, HASH_FNV_CT("DT_BasePlayer"), HASH_FNV_CT("m_vecVelocity[0]"));
  NETVAR_DEFINITION(c_base_handle, observer_target, HASH_FNV_CT("DT_BasePlayer"), HASH_FNV_CT("m_hObserverTarget"));
  NETVAR_DEFINITION(c_base_handle, active_weapon_handle, HASH_FNV_CT("DT_BaseCombatCharacter"), HASH_FNV_CT("m_hActiveWeapon"));

  bool has_bomb() const;
};

class c_economy_item : public c_entity
{
public:
  NETVAR_DEFINITION(bool, is_initialized, HASH_FNV_CT("DT_BaseAttributableItem"), HASH_FNV_CT("m_AttributeManager"), HASH_FNV_CT("m_Item"), HASH_FNV_CT("m_bInitialized"));
  NETVAR_DEFINITION(int16_t, item_definition_index, HASH_FNV_CT("DT_BaseAttributableItem"), HASH_FNV_CT("m_AttributeManager"), HASH_FNV_CT("m_Item"), HASH_FNV_CT("m_iItemDefinitionIndex"));
  NETVAR_DEFINITION(int, entity_level, HASH_FNV_CT("DT_BaseAttributableItem"), HASH_FNV_CT("m_AttributeManager"), HASH_FNV_CT("m_Item"), HASH_FNV_CT("m_iEntityLevel"));
  NETVAR_DEFINITION(int, account_id, HASH_FNV_CT("DT_BaseAttributableItem"), HASH_FNV_CT("m_AttributeManager"), HASH_FNV_CT("m_Item"), HASH_FNV_CT("m_iAccountID"));
  NETVAR_DEFINITION(int, item_id_low, HASH_FNV_CT("DT_BaseAttributableItem"), HASH_FNV_CT("m_AttributeManager"), HASH_FNV_CT("m_Item"), HASH_FNV_CT("m_iItemIDLow"));
  NETVAR_DEFINITION(int, item_id_high, HASH_FNV_CT("DT_BaseAttributableItem"), HASH_FNV_CT("m_AttributeManager"), HASH_FNV_CT("m_Item"), HASH_FNV_CT("m_iItemIDHigh"));
  NETVAR_DEFINITION(int, entity_quality, HASH_FNV_CT("DT_BaseAttributableItem"), HASH_FNV_CT("m_AttributeManager"), HASH_FNV_CT("m_Item"), HASH_FNV_CT("m_iEntityQuality"));
};

class c_weapon : public c_economy_item
{
public:
  NETVAR_DEFINITION(float, next_primary_attack, HASH_FNV_CT("DT_BaseCombatWeapon"), HASH_FNV_CT("m_flNextPrimaryAttack"));
  NETVAR_DEFINITION(float, next_secondary_attack, HASH_FNV_CT("DT_BaseCombatWeapon"), HASH_FNV_CT("m_flNextSecondaryAttack"));
  NETVAR_DEFINITION(int, ammo1, HASH_FNV_CT("DT_BaseCombatWeapon"), HASH_FNV_CT("m_iClip1"));
  NETVAR_DEFINITION(int, ammo2, HASH_FNV_CT("DT_BaseCombatWeapon"), HASH_FNV_CT("m_iClip2"));
  NETVAR_DEFINITION(int, reserve_ammo_count, HASH_FNV_CT("DT_BaseCombatWeapon"), HASH_FNV_CT("m_iPrimaryReserveAmmoCount"));
  NETVAR_DEFINITION(float, recoil_index, HASH_FNV_CT("DT_WeaponCSBase"), HASH_FNV_CT("m_flRecoilIndex"));
};
