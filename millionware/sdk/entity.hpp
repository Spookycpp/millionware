#pragma once

#include <array>

#include "../utils/math/math.hpp"

#include "../utils/hash/hash.hpp"

#include "weapon_system.hpp"
#include "base_handle.hpp"
#include "client_class.hpp"
#include "macros.hpp"
#include "matrix.hpp"
#include "vector.hpp"
#include "studiobbox.hpp"
#include "studio.hpp"
#include "model_info.hpp"
#include "util_vector.hpp"

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

enum e_item_definition_index {
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SHIELD = 37,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFEGG = 41,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS = 69,
	WEAPON_BREACHCHARGE = 70,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE = 75,
	WEAPON_HAMMER = 76,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB = 81,
	WEAPON_DIVERSION = 82,
	WEAPON_FRAG_GRENADE = 83,
	WEAPON_SNOWBALL = 84,
	WEAPON_BUMPMINE = 85,
	WEAPON_BAYONET = 500,
	WEAPON_KNIFE_CSS = 503,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516,
	WEAPON_KNIFE_CORD = 517,
	WEAPON_KNIFE_CANIS = 518,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE = 520,
	WEAPON_KNIFE_OUTDOOR = 521,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER = 523,
	WEAPON_KNIFE_SKELETON = 525,
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T_SIDE = 5028,
	GLOVE_CT_SIDE = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_WRAP = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034,
	GLOVE_HYDRA = 5035
};

enum player_hitboxes : int {
	HEAD = 0,
	NECK,
	PELVIS,
	STOMACH,
	THORAX,
	L_CHEST,
	U_CHEST,
	R_THIGH,
	L_THIGH,
	R_CALF,
	L_CALF,
	R_FOOT,
	L_FOOT,
	R_HAND,
	L_HAND,
	R_UPPERARM,
	R_FOREARM,
	L_UPPERARM,
	L_FOREARM,
	MAX_HITBOX,
};

enum hit_groups {
	HIT_GROUP_INVALID = -1,
	HIT_GROUP_GENERIC,
	HIT_GROUP_HEAD,
	HIT_GROUP_CHEST,
	HIT_GROUP_STOMACH,
	HIT_GROUP_LEFTARM,
	HIT_GROUP_RIGHTARM,
	HIT_GROUP_LEFTLEG,
	HIT_GROUP_RIGHTLEG,
	HIT_GROUP_GEAR = 10
};

enum bone_mask_t {
	BONE_USED_MASK = 0x0007FF00,
	BONE_USED_BY_ANYTHING = 0x0007FF00,
	BONE_USED_BY_HITBOX = 0x00000100,	// bone (or child) is used by a hit box
	BONE_USED_BY_ATTACHMENT = 0x00000200,	// bone (or child) is used by an attachment point
	BONE_USED_BY_VERTEX_MASK = 0x0003FC00,
	BONE_USED_BY_VERTEX_LOD0 = 0x00000400,	// bone (or child) is used by the toplevel model via skinned vertex
	BONE_USED_BY_VERTEX_LOD1 = 0x00000800,
	BONE_USED_BY_VERTEX_LOD2 = 0x00001000,
	BONE_USED_BY_VERTEX_LOD3 = 0x00002000,
	BONE_USED_BY_VERTEX_LOD4 = 0x00004000,
	BONE_USED_BY_VERTEX_LOD5 = 0x00008000,
	BONE_USED_BY_VERTEX_LOD6 = 0x00010000,
	BONE_USED_BY_VERTEX_LOD7 = 0x00020000,
	BONE_USED_BY_BONE_MERGE = 0x00040000	// bone is available for bone merge to occur against it
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
	VIRTUAL_METHOD(c_model*, get_model, 8, ());
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
	NETVAR_DEFINITION(base_handle_t, owner_handle, FNV_CT("DT_BaseEntity"), FNV_CT("m_hOwnerEntity"));
	NETVAR_DEFINITION(vector3_t, get_mins, FNV_CT("DT_BaseEntity"), FNV_CT("m_vecMins"));
	NETVAR_DEFINITION(vector3_t, get_maxs, FNV_CT("DT_BaseEntity"), FNV_CT("m_vecMaxs"));
	NETVAR_DEFINITION(float, get_simulation_time, FNV_CT("DT_BaseEntity"), FNV_CT("m_flSimulationTime"));

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
	NETVAR_DEFINITION(vector3_t, punch_angle, FNV_CT("DT_BasePlayer"), FNV_CT("m_Local"), FNV_CT("m_viewPunchAngle"));
	NETVAR_DEFINITION(vector3_t, aim_punch_angle, FNV_CT("DT_BasePlayer"), FNV_CT("m_Local"), FNV_CT("m_aimPunchAngle"));
	NETVAR_DEFINITION(vector3_t, velocity, FNV_CT("DT_BasePlayer"), FNV_CT("m_vecVelocity[0]"));
	NETVAR_DEFINITION(vector3_t, view_offset, FNV_CT("DT_BasePlayer"), FNV_CT("m_vecViewOffset[0]"));
	NETVAR_DEFINITION(base_handle_t, observer_target, FNV_CT("DT_BasePlayer"), FNV_CT("m_hObserverTarget"));
	NETVAR_DEFINITION(base_handle_t, active_weapon_handle, FNV_CT("DT_BaseCombatCharacter"), FNV_CT("m_hActiveWeapon"));
	NETVAR_DEFINITION(int, hitbox_set, FNV_CT("DT_BaseAnimating"), FNV_CT("m_nHitboxSet"));

	CUtlVector<matrix3x4_t>& get_cached_bone_data() {
		return *(CUtlVector<matrix3x4_t>*) ((uintptr_t)this + 0x290C + sizeof(void*));
	}

	NETVAR_DEFINITION_OFFSET(int, move_type, 1, FNV_CT("DT_BaseEntity"), FNV_CT("m_nRenderMode"));

	VIRTUAL_METHOD(float, get_spread, 452, ());
	VIRTUAL_METHOD(float, get_inaccuracy, 482, ());

	bool has_bomb() const;

	bool is_alive();

	bool is_enemy() const;

	bool is_valid(const bool check_alive = true);

	vector3_t get_eye_origin();

	vector3_t get_hitbox_pos(const int idx);

	float get_flash_time();

	bool is_flashed();

	bool can_shoot();

	bool can_shoot(c_weapon* wpn);

	vector3_t extrapolate_position(const vector3_t& pos);

	bool is_visible(c_player* local, const vector3_t& src, const vector3_t& dst);

	NETVAR_DEFINITION_OFFSET(int, get_old_simulation_time, 4, FNV_CT("DT_BaseEntity"), FNV_CT("m_flSimulationTime"));
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
	NETVAR_DEFINITION(bool, is_burst_mode, FNV_CT("DT_WeaponCSBase"), FNV_CT("m_bBurstMode"));
	NETVAR_DEFINITION(int, burst_shots_remaining, FNV_CT("DT_WeaponCSBaseGun"), FNV_CT("m_iBurstShotsRemaining"));
	NETVAR_DEFINITION(float, ready_time, FNV_CT("DT_WeaponCSBase"), FNV_CT("m_flPostponeFireReadyTime"));
	NETVAR_DEFINITION(float, next_attack, FNV_CT("DT_BaseCombatCharacter"), FNV_CT("m_flNextAttack"));

	int get_weapon_type()
	{
		switch (this->item_definition_index())
		{
		case WEAPON_DEAGLE:                return WEAPON_TYPE_PISTOL;
		case WEAPON_ELITE:                 return WEAPON_TYPE_PISTOL;
		case WEAPON_FIVESEVEN:             return WEAPON_TYPE_PISTOL;
		case WEAPON_GLOCK:                 return WEAPON_TYPE_PISTOL;
		case WEAPON_AK47:                  return WEAPON_TYPE_RIFLE;
		case WEAPON_AUG:                   return WEAPON_TYPE_RIFLE;
		case WEAPON_AWP:                   return WEAPON_TYPE_SNIPER;
		case WEAPON_FAMAS:                 return WEAPON_TYPE_RIFLE;
		case WEAPON_G3SG1:                 return WEAPON_TYPE_SNIPER;
		case WEAPON_GALILAR:               return WEAPON_TYPE_RIFLE;
		case WEAPON_M249:                  return WEAPON_TYPE_MG;
		case WEAPON_M4A1:                  return WEAPON_TYPE_RIFLE;
		case WEAPON_MAC10:                 return WEAPON_TYPE_SMG;
		case WEAPON_P90:                   return WEAPON_TYPE_SMG;
		case WEAPON_UMP45:                 return WEAPON_TYPE_SMG;
		case WEAPON_XM1014:                return WEAPON_TYPE_SHOTGUN;
		case WEAPON_BIZON:                 return WEAPON_TYPE_SMG;
		case WEAPON_MAG7:                  return WEAPON_TYPE_SHOTGUN;
		case WEAPON_NEGEV:                 return WEAPON_TYPE_MG;
		case WEAPON_SAWEDOFF:              return WEAPON_TYPE_SHOTGUN;
		case WEAPON_TEC9:                  return WEAPON_TYPE_PISTOL;
		case WEAPON_TASER:                 return WEAPON_TYPE_TASER;
		case WEAPON_HKP2000:               return WEAPON_TYPE_PISTOL;
		case WEAPON_MP7:                   return WEAPON_TYPE_SMG;
		case WEAPON_MP9:                   return WEAPON_TYPE_SMG;
		case WEAPON_NOVA:                  return WEAPON_TYPE_SHOTGUN;
		case WEAPON_P250:                  return WEAPON_TYPE_PISTOL;
		case WEAPON_SCAR20:                return WEAPON_TYPE_SNIPER;
		case WEAPON_SG556:                 return WEAPON_TYPE_RIFLE;
		case WEAPON_SSG08:                 return WEAPON_TYPE_SNIPER;
		case WEAPON_KNIFE:                 return WEAPON_TYPE_KNIFE;
		case WEAPON_FLASHBANG:             return WEAPON_TYPE_GRENADE;
		case WEAPON_HEGRENADE:             return WEAPON_TYPE_GRENADE;
		case WEAPON_SMOKEGRENADE:          return WEAPON_TYPE_GRENADE;
		case WEAPON_MOLOTOV:               return WEAPON_TYPE_GRENADE;
		case WEAPON_DECOY:                 return WEAPON_TYPE_GRENADE;
		case WEAPON_INCGRENADE:            return WEAPON_TYPE_GRENADE;
		case WEAPON_C4:                    return WEAPON_TYPE_INVALID;
		case WEAPON_KNIFE_T:               return WEAPON_TYPE_KNIFE;
		case WEAPON_M4A1_SILENCER:         return WEAPON_TYPE_RIFLE;
		case WEAPON_USP_SILENCER:          return WEAPON_TYPE_PISTOL;
		case WEAPON_CZ75A:                 return WEAPON_TYPE_PISTOL;
		case WEAPON_REVOLVER:              return WEAPON_TYPE_PISTOL;
		case WEAPON_BAYONET:               return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_CSS:             return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_FLIP:            return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_GUT:             return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_KARAMBIT:        return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_M9_BAYONET:      return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_TACTICAL:        return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_FALCHION:        return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_SURVIVAL_BOWIE:  return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_BUTTERFLY:       return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_PUSH:            return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_CORD:            return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_CANIS:           return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_URSUS:           return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_GYPSY_JACKKNIFE: return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_OUTDOOR:         return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_STILETTO:        return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_WIDOWMAKER:      return WEAPON_TYPE_KNIFE;
		case WEAPON_KNIFE_SKELETON:        return WEAPON_TYPE_KNIFE;
		default:                           return WEAPON_TYPE_INVALID;
		}
	}

	bool has_scope()
	{
		const int weapon_index = this->item_definition_index();

		return weapon_index == WEAPON_G3SG1
			|| weapon_index == WEAPON_SCAR20
			|| weapon_index == WEAPON_AWP
			|| weapon_index == WEAPON_AUG
			|| weapon_index == WEAPON_SG556
			|| weapon_index == WEAPON_SSG08;
	}

	bool is_pistol() { return this->get_weapon_type() == WEAPON_TYPE_PISTOL; }

	bool is_taser() { return this->get_weapon_type() == WEAPON_TYPE_TASER; }

	bool is_shotgun() { return this->get_weapon_type() == WEAPON_TYPE_SHOTGUN; }

	bool is_smg() { return this->get_weapon_type() == WEAPON_TYPE_SMG; }

	bool is_rifle() { return this->get_weapon_type() == WEAPON_TYPE_RIFLE; }

	bool is_mg() { return this->get_weapon_type() == WEAPON_TYPE_MG; }

	bool is_sniper() { return this->get_weapon_type() == WEAPON_TYPE_SNIPER; }

	bool is_grenade() { return this->get_weapon_type() == WEAPON_TYPE_GRENADE; }

	bool is_knife() { return this->get_weapon_type() == WEAPON_TYPE_KNIFE; }

	bool is_valid(const bool check_clip = true)
	{
		switch (this->get_weapon_type())
		{
		case WEAPON_TYPE_PISTOL:  break;
		case WEAPON_TYPE_SHOTGUN: break;
		case WEAPON_TYPE_SMG:     break;
		case WEAPON_TYPE_RIFLE:   break;
		case WEAPON_TYPE_MG:      break;
		case WEAPON_TYPE_SNIPER:  break;
		default:                  return false;
		}

		return check_clip ? this->ammo1() > 0 : true;
	}
};

class c_base_grenade : public c_entity {
public:
	// networked variables
	NETVAR_DEFINITION(bool, pin_pulled, FNV_CT("DT_BaseCSGrenade"), FNV_CT("m_bPinPulled"));
	NETVAR_DEFINITION(float, throw_time, FNV_CT("DT_BaseCSGrenade"), FNV_CT("m_fThrowTime"));
	NETVAR_DEFINITION(float, throw_strength, FNV_CT("DT_BaseCSGrenade"), FNV_CT("m_flThrowStrength"));
};