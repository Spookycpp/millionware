#pragma once

#include "../utils/hash.hpp"
#include "base_handle.hpp"
#include "client_class.hpp"
#include "macros.hpp"
#include "matrix.hpp"
#include "vector.hpp"

enum e_life_state {
	LIFE_STATE_ALIVE,
	LIFE_STATE_KILL_CAM,
	LIFE_STATE_DEAD,
};

enum e_team_num {
	TEAM_NUM_NONE,
	TEAM_NUM_SPECTATOR,
	TEAM_NUM_TERRORISTS,
	TEAM_NUM_COUNTER_TERRORISTS,
};

enum e_entity_flag {
	ENTITY_FLAG_ONGROUND = 1 << 0,
	ENTITY_FLAG_DUCKING = 1 << 1,
	ENTITY_FLAG_WATERJUMP = 1 << 2,
	ENTITY_FLAG_ONTRAIN = 1 << 3,
	ENTITY_FLAG_INRAIN = 1 << 4,
	ENTITY_FLAG_FROZEN = 1 << 5,
	ENTITY_FLAG_ATCONTROLS = 1 << 6,
	ENTITY_FLAG_CLIENT = 1 << 7,
	ENTITY_FLAG_FAKECLIENT = 1 << 8,
	ENTITY_FLAG_INWATER = 1 << 9,
	ENTITY_FLAG_FLY = 1 << 10,
	ENTITY_FLAG_SWIM = 1 << 11,
	ENTITY_FLAG_CONVEYOR = 1 << 12,
	ENTITY_FLAG_NPC = 1 << 13,
	ENTITY_FLAG_GODMODE = 1 << 14,
	ENTITY_FLAG_NOTARGET = 1 << 15,
	ENTITY_FLAG_AIMTARGET = 1 << 16,
	ENTITY_FLAG_PARTIALGROUND = 1 << 17,
	ENTITY_FLAG_STATICPROP = 1 << 18,
	ENTITY_FLAG_GRAPHED = 1 << 19,
	ENTITY_FLAG_GRENADE = 1 << 20,
	ENTITY_FLAG_STEPMOVEMENT = 1 << 21,
	ENTITY_FLAG_DONTTOUCH = 1 << 22,
	ENTITY_FLAG_BASEVELOCITY = 1 << 23,
	ENTITY_FLAG_WORLDBRUSH = 1 << 24,
	ENTITY_FLAG_OBJECT = 1 << 25,
	ENTITY_FLAG_KILLME = 1 << 26,
	ENTITY_FLAG_ONFIRE = 1 << 27,
	ENTITY_FLAG_DISSOLVING = 1 << 28,
	ENTITY_FLAG_TRANSRAGDOLL = 1 << 29,
	ENTITY_FLAG_UNBLOCKABLE_BY_PLAYER = 1 << 30,
};

enum e_move_type {
	MOVE_TYPE_NONE,
	MOVE_TYPE_ISOMETRIC,
	MOVE_TYPE_WALK,
	MOVE_TYPE_STEP,
	MOVE_TYPE_FLY,
	MOVE_TYPE_FLYGRAVITY,
	MOVE_TYPE_VPHYSICS,
	MOVE_TYPE_PUSH,
	MOVE_TYPE_NOCLIP,
	MOVE_TYPE_LADDER,
	MOVE_TYPE_OBSERVER,
	MOVE_TYPE_CUSTOM,
};

class c_collideable {
public:
	VIRTUAL_METHOD(vector3_t&, get_mins, 1, ());
	VIRTUAL_METHOD(vector3_t&, get_maxs, 2, ());
};

class c_client_networkable {
public:
	VIRTUAL_METHOD(client_class_t*, get_client_class, 2, ());
	VIRTUAL_METHOD(bool, is_dormant, 9, ());
	VIRTUAL_METHOD(int, index, 10, ());
};

class c_client_renderable {
public:
	VIRTUAL_METHOD(vector3_t&, get_render_origin, 1, ());
	VIRTUAL_METHOD(vector3_t&, get_render_angles, 2, ());
	VIRTUAL_METHOD(bool, should_draw, 3, ());
	VIRTUAL_METHOD(bool, setup_bones, 13, (matrix3x4_t* bone_to_world_out, const int max_bones, const int bone_mask, const float current_time), bone_to_world_out, max_bones, bone_mask, current_time);
};

class c_entity {
	uintptr_t unknown_;

public:
	c_client_renderable* renderable() {
		return reinterpret_cast<c_client_renderable*>(reinterpret_cast<uintptr_t>(this) + 0x4);
	}

	c_client_networkable* networkable() {
		return reinterpret_cast<c_client_networkable*>(reinterpret_cast<uintptr_t>(this) + 0x8);
	}

	NETVAR_DEFINITION(bool, is_spotted, FNV_CT("DT_BaseEntity"), FNV_CT("m_bSpotted"));
	NETVAR_DEFINITION(int, flags, FNV_CT("DT_BasePlayer"), FNV_CT("m_fFlags"));
	NETVAR_DEFINITION(int, team_num, FNV_CT("DT_BaseEntity"), FNV_CT("m_iTeamNum"));
	NETVAR_DEFINITION(vector3_t, origin, FNV_CT("DT_BaseEntity"), FNV_CT("m_vecOrigin"));
	NETVAR_DEFINITION(vector3_t, view_offset, FNV_CT("DT_CSPlayer"), FNV_CT("m_vecViewOffset[0]"));
	NETVAR_DEFINITION(base_handle_t, owner_handle, FNV_CT("DT_BaseEntity"), FNV_CT("m_hOwnerEntity"));
	NETVAR_DEFINITION_OFFSET(matrix3x4_t, entity_to_world_matrix, -48, FNV_CT("DT_BaseEntity"), FNV_CT("m_CollisionGroup"));

	VIRTUAL_METHOD(c_collideable*, get_collideable, 3, ());
	VIRTUAL_METHOD(bool, is_player, 157, ());
	VIRTUAL_METHOD(bool, is_weapon, 165, ());

	class c_player* as_player();
	class c_weapon* as_weapon();
};

class c_player : public c_entity {
public:
	NETVAR_DEFINITION(bool, has_defuser, FNV_CT("DT_CSPlayer"), FNV_CT("m_bHasDefuser"));
	NETVAR_DEFINITION(bool, has_gun_game_immunity, FNV_CT("DT_CSPlayer"), FNV_CT("m_bGunGameImmunity"));
	NETVAR_DEFINITION(bool, has_helmet, FNV_CT("DT_CSPlayer"), FNV_CT("m_bHasHelmet"));
	NETVAR_DEFINITION(bool, is_scoped, FNV_CT("DT_CSPlayer"), FNV_CT("m_bIsScoped"));;
	NETVAR_DEFINITION(bool, is_defusing, FNV_CT("DT_CSPlayer"), FNV_CT("m_bIsDefusing"));;
	NETVAR_DEFINITION(float, flash_duration, FNV_CT("DT_CSPlayer"), FNV_CT("m_flFlashDuration"));
	NETVAR_DEFINITION(float, flash_alpha, FNV_CT("DT_CSPlayer"), FNV_CT("m_flFlashMaxAlpha"));
	NETVAR_DEFINITION(int, shots_fired, FNV_CT("DT_CSPlayer"), FNV_CT("m_iShotsFired"));
	NETVAR_DEFINITION(int, armor, FNV_CT("DT_CSPlayer"), FNV_CT("m_ArmorValue"));
	NETVAR_DEFINITION(int, health, FNV_CT("DT_BasePlayer"), FNV_CT("m_iHealth"));
	NETVAR_DEFINITION(int, life_state, FNV_CT("DT_BasePlayer"), FNV_CT("m_lifeState"));
	NETVAR_DEFINITION(int, tick_base, FNV_CT("DT_BasePlayer"), FNV_CT("m_nTickBase"));
	NETVAR_DEFINITION(vector3_t, eye_angles, FNV_CT("DT_CSPlayer"), FNV_CT("m_angEyeAngles[0]"));
	NETVAR_DEFINITION(vector3_t, punch_angle, FNV_CT("DT_BasePlayer"), FNV_CT("m_viewPunchAngle"));
	NETVAR_DEFINITION(vector3_t, aim_punch_angle, FNV_CT("DT_BasePlayer"), FNV_CT("m_aimPunchAngle"));
	NETVAR_DEFINITION(vector3_t, velocity, FNV_CT("DT_BasePlayer"), FNV_CT("m_vecVelocity[0]"));
	NETVAR_DEFINITION(base_handle_t, observer_target, FNV_CT("DT_BasePlayer"), FNV_CT("m_hObserverTarget"));
	NETVAR_DEFINITION(base_handle_t, active_weapon_handle, FNV_CT("DT_BaseCombatCharacter"), FNV_CT("m_hActiveWeapon"));

	c_weapon* get_active_weapon() {
		return interfaces::entity_list->get< c_weapon >(active_weapon_handle());
	}

	bool has_bomb() const;
};

class c_economy_item : public c_entity {
public:
	NETVAR_DEFINITION(bool, is_initialized, FNV_CT("DT_BaseAttributableItem"), FNV_CT("m_AttributeManager"), FNV_CT("m_Item"), FNV_CT("m_bInitialized"));
	NETVAR_DEFINITION(int16_t, item_definition_index, FNV_CT("DT_BaseAttributableItem"), FNV_CT("m_AttributeManager"), FNV_CT("m_Item"), FNV_CT("m_iItemDefinitionIndex"));
	NETVAR_DEFINITION(int, entity_level, FNV_CT("DT_BaseAttributableItem"), FNV_CT("m_AttributeManager"), FNV_CT("m_Item"), FNV_CT("m_iEntityLevel"));
	NETVAR_DEFINITION(int, account_id, FNV_CT("DT_BaseAttributableItem"), FNV_CT("m_AttributeManager"), FNV_CT("m_Item"), FNV_CT("m_iAccountID"));
	NETVAR_DEFINITION(int, item_id_low, FNV_CT("DT_BaseAttributableItem"), FNV_CT("m_AttributeManager"), FNV_CT("m_Item"), FNV_CT("m_iItemIDLow"));
	NETVAR_DEFINITION(int, item_id_high, FNV_CT("DT_BaseAttributableItem"), FNV_CT("m_AttributeManager"), FNV_CT("m_Item"), FNV_CT("m_iItemIDHigh"));
	NETVAR_DEFINITION(int, entity_quality, FNV_CT("DT_BaseAttributableItem"), FNV_CT("m_AttributeManager"), FNV_CT("m_Item"), FNV_CT("m_iEntityQuality"));
};

class c_weapon : public c_economy_item {
public:
	NETVAR_DEFINITION(float, next_primary_attack, FNV_CT("DT_BaseCombatWeapon"), FNV_CT("m_flNextPrimaryAttack"));
	NETVAR_DEFINITION(float, next_secondary_attack, FNV_CT("DT_BaseCombatWeapon"), FNV_CT("m_flNextSecondaryAttack"));
	NETVAR_DEFINITION(int, ammo1, FNV_CT("DT_BaseCombatWeapon"), FNV_CT("m_iClip1"));
	NETVAR_DEFINITION(int, ammo2, FNV_CT("DT_BaseCombatWeapon"), FNV_CT("m_iClip2"));
	NETVAR_DEFINITION(int, reserve_ammo_count, FNV_CT("DT_BaseCombatWeapon"), FNV_CT("m_iPrimaryReserveAmmoCount"));
	NETVAR_DEFINITION(float, recoil_index, FNV_CT("DT_WeaponCSBase"), FNV_CT("m_flRecoilIndex"));
};
