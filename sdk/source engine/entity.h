#pragma once

#include "../core/patterns/patterns.h"
#include "../core/util/util.h"

#include <cstdint>

#include "client_class.h"
#include "macros.h"
#include "model_info.h"
#include "util_vector.h"
#include "vector.h"
#include "weapon_system.h"
#include "matrix.h"

enum e_item_definition_index
{
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
	WEAPON_KNIFE_BAYONET = 500,
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
	WEAPON_GLOVE_STUDDED_BLOODHOUND = 5027,
	WEAPON_GLOVE_T_SIDE = 5028,
	WEAPON_GLOVE_CT_SIDE = 5029,
	WEAPON_GLOVE_SPORTY = 5030,
	WEAPON_GLOVE_SLICK = 5031,
	WEAPON_GLOVE_LEATHER_WRAP = 5032,
	WEAPON_GLOVE_MOTORCYCLE = 5033,
	WEAPON_GLOVE_SPECIALIST = 5034,
	WEAPON_GLOVE_HYDRA = 5035
};

enum e_life_state
{
	LIFE_STATE_ALIVE,
	LIFE_STATE_KILL_CAM,
	LIFE_STATE_DEAD,
};

enum e_team_num
{
	TEAM_NUM_NONE,
	TEAM_NUM_SPECTATOR,
	TEAM_NUM_TERRORISTS,
	TEAM_NUM_COUNTER_TERRORISTS,
};

enum e_player_hitboxes : int {
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

enum e_bone_mask_t {
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

struct entity_handle_t
{
	uintptr_t handle;

	constexpr entity_handle_t(unsigned long handle) : handle(handle)
	{
	}

	class c_entity *get() const;

	bool operator ==(const entity_handle_t &other) const;
	bool operator ==(class c_entity *entity) const;

	bool operator !=(const entity_handle_t &other) const;
	bool operator !=(class c_entity *entity) const;
};

class c_collideable
{
public:
	DECLARE_VFUNC(1, get_mins(), vector_t &(__thiscall *)(void *))();
	DECLARE_VFUNC(2, get_maxs(), vector_t &(__thiscall *)(void *))();
};

class c_networkable
{
public:
	DECLARE_VFUNC(2, get_client_class(), c_client_class *(__thiscall *)(void *))();
	DECLARE_VFUNC(9, is_dormant(), bool(__thiscall *)(void *))();
	DECLARE_VFUNC(10, index(), int(__thiscall *)(void *))();
};

class c_renderable
{
public:
	DECLARE_VFUNC(1, get_render_origin(), vector_t &(__thiscall *)(void *))();
	DECLARE_VFUNC(2, get_render_angles(), vector_t &(__thiscall *)(void *))();
	DECLARE_VFUNC(3, should_draw(), bool(__thiscall *)(void *))();
	DECLARE_VFUNC(8, get_model(), c_model *(__thiscall *)(void *))();
	DECLARE_VFUNC(13, setup_bones(matrix3x4_t *bone_to_world_out, int max_bones, int bone_mask, float current_time), bool(__thiscall *)(void *, matrix3x4_t *, int, int, float))(bone_to_world_out, max_bones, bone_mask, current_time);
};

class c_entity
{
public:
	// networked variables
	DECLARE_OFFSET(get_renderable(), (c_renderable *) ((uintptr_t) this + 0x4));
	DECLARE_OFFSET(get_networkable(), (c_networkable *) ((uintptr_t) this + 0x8));

	DECLARE_VFUNC(3, get_collideable(), c_collideable *(__thiscall *)(void *))();
	DECLARE_VFUNC(10, get_abs_origin(), vector_t &(__thiscall *)(void *))();
	DECLARE_VFUNC(157, is_player(), bool(__thiscall *)(void *))();
	DECLARE_VFUNC(165, is_weapon(), bool(__thiscall *)(void *))();

	DECLARE_NETVAR(float, simulation_time, "DT_BaseEntity", "m_flSimulationTime");
	DECLARE_NETVAR(bool, is_spotted, "DT_BaseEntity", "m_bSpotted");
	DECLARE_NETVAR(int, flags, "DT_BasePlayer", "m_fFlags");
	DECLARE_NETVAR(int, team_num, "DT_BaseEntity", "m_iTeamNum");
	DECLARE_NETVAR(vector_t, origin, "DT_BaseEntity", "m_vecOrigin");
	DECLARE_NETVAR(entity_handle_t, owner_handle, "DT_BaseEntity", "m_hOwnerEntity");

	DECLARE_NETVAR_OFFSET(matrix3x4_t, transformation_matrix, "DT_BaseEntity", "m_CollisionGroup", -48);
};

class c_player : public c_entity
{
public:
	DECLARE_NETVAR(bool, has_defuser, "DT_CSPlayer", "m_bHasDefuser");
	DECLARE_NETVAR(bool, has_gun_game_immunity, "DT_CSPlayer", "m_bGunGameImmunity");
	DECLARE_NETVAR(bool, has_helmet, "DT_CSPlayer", "m_bHasHelmet");
	DECLARE_NETVAR(bool, is_scoped, "DT_CSPlayer", "m_bIsScoped");
	DECLARE_NETVAR(bool, is_defusing, "DT_CSPlayer", "m_bIsDefusing");
	DECLARE_NETVAR(bool, has_heavy_armor, "DT_CSPlayer", "m_bHasHeavyArmor");
	DECLARE_NETVAR(float, flash_duration, "DT_CSPlayer", "m_flFlashDuration");
	DECLARE_NETVAR(float, flash_alpha, "DT_CSPlayer", "m_flFlashMaxAlpha");
	DECLARE_NETVAR(float, lower_body_yaw, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	DECLARE_NETVAR(float, health_shot_boost_time, "DT_CSPlayer", "m_flHealthShotBoostExpirationTime");
	DECLARE_NETVAR(int, shots_fired, "DT_CSLocalPlayerExclusive", "m_iShotsFired");
	DECLARE_NETVAR(int, armor, "DT_CSPlayer", "m_ArmorValue");
	DECLARE_NETVAR(int, health, "DT_BasePlayer", "m_iHealth");
	DECLARE_NETVAR(int, life_state, "DT_BasePlayer", "m_lifeState");
	DECLARE_NETVAR(int, tick_base, "DT_LocalPlayerExclusive", "m_nTickBase");
	DECLARE_NETVAR(int, hitbox_set, "DT_BaseAnimating", "m_nHitboxSet");
	DECLARE_NETVAR(int, money, "DT_CSPlayer", "m_iAccount");
	DECLARE_NETVAR(int, observer_mode, "DT_BasePlayer", "m_iObserverMode");
	DECLARE_NETVAR(vector_t, eye_angles, "DT_CSPlayer", "m_angEyeAngles[0]");
	DECLARE_NETVAR(vector_t, punch_angle, "DT_Local", "m_viewPunchAngle");
	DECLARE_NETVAR(vector_t, aim_punch_angle, "DT_Local", "m_aimPunchAngle");
	DECLARE_NETVAR(vector_t, velocity, "DT_LocalPlayerExclusive", "m_vecVelocity[0]");
	DECLARE_NETVAR(vector_t, view_offset, "DT_LocalPlayerExclusive", "m_vecViewOffset[0]");
	DECLARE_NETVAR(entity_handle_t, observer_target, "DT_BasePlayer", "m_hObserverTarget");
	DECLARE_NETVAR(entity_handle_t, active_weapon_handle, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	DECLARE_NETVAR(entity_handle_t, view_model_handle, "DT_BasePlayer", "m_hViewModel[0]");
	DECLARE_NETVAR(entity_handle_t, ground_entity, "DT_BasePlayer", "m_hGroundEntity");

	DECLARE_NETVAR_OFFSET(int, old_simulation_time, "DT_BaseEntity", "m_flSimulationTime", 4);
	DECLARE_NETVAR_OFFSET(int, move_type, "DT_BaseEntity", "m_nRenderMode", 1);

	CUtlVector<matrix3x4_t>& get_cached_bone_data();

	vector_t get_eye_pos( ) const;

	vector_t extrapolate_position(const vector_t& pos);
	
	float get_flash_time();

	bool is_flashed();

	vector_t get_hitbox_pos(int idx);
	
	bool can_shoot();
	bool can_shoot(class c_weapon* weapon);
	bool is_valid(bool check_alive = true);
	bool is_alive();
	bool is_visible(c_player* local, const vector_t& src, const vector_t& dst);
	bool is_visible(c_player* local, const vector_t& src);
};

class c_economy_item : public c_entity
{
public:
	DECLARE_NETVAR(short, item_definition_index, "DT_ScriptCreatedItem", "m_iItemDefinitionIndex");
	DECLARE_NETVAR(bool, is_initialized, "DT_ScriptCreatedItem", "m_bInitialized");
	DECLARE_NETVAR(int, entity_level, "DT_ScriptCreatedItem", "m_iEntityLevel");
	DECLARE_NETVAR(int, account_id, "DT_ScriptCreatedItem", "m_iAccountID");
	DECLARE_NETVAR(int, item_id_low, "DT_ScriptCreatedItem", "m_iItemIDLow");
	DECLARE_NETVAR(int, item_id_high, "DT_ScriptCreatedItem", "m_iItemIDHigh");
	DECLARE_NETVAR(int, entity_quality, "DT_ScriptCreatedItem", "m_iEntityQuality");
};

class c_weapon : public c_economy_item
{
public:
	DECLARE_NETVAR(bool, is_burst_mode, "DT_WeaponCSBase", "m_bBurstMode");
	DECLARE_NETVAR(float, next_primary_attack, "DT_LocalActiveWeaponData", "m_flNextPrimaryAttack");
	DECLARE_NETVAR(float, next_secondary_attack, "DT_LocalActiveWeaponData", "m_flNextSecondaryAttack");
	DECLARE_NETVAR(float, ready_time, "DT_WeaponCSBase", "m_flPostponeFireReadyTime");
	DECLARE_NETVAR(float, next_attack, "DT_BCCLocalPlayerExclusive", "m_flNextAttack");
	DECLARE_NETVAR(float, recoil_index, "DT_WeaponCSBase", "m_flRecoilIndex");
	DECLARE_NETVAR(int, ammo1, "DT_BaseCombatWeapon", "m_iClip1");
	DECLARE_NETVAR(int, ammo2, "DT_BaseCombatWeapon", "m_iClip2");
	DECLARE_NETVAR(int, reserve_ammo_count, "DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount");
	DECLARE_NETVAR(int, burst_shots_remaining, "DT_WeaponCSBaseGun", "m_iBurstShotsRemaining");

	DECLARE_VFUNC(452, get_spread(), float(__thiscall *)(void *))();
	DECLARE_VFUNC(482, get_inaccuracy(), float(__thiscall *)(void *))();

	int get_weapon_type() {
		switch (this->get_item_definition_index()) {
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
			case WEAPON_KNIFE_BAYONET:         return WEAPON_TYPE_KNIFE;
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

	bool has_scope() {
		const int weapon_index = this->get_item_definition_index();

		return weapon_index == WEAPON_G3SG1
			|| weapon_index == WEAPON_SCAR20
			|| weapon_index == WEAPON_AWP
			|| weapon_index == WEAPON_AUG
			|| weapon_index == WEAPON_SG556
			|| weapon_index == WEAPON_SSG08;
	}

	inline bool is_pistol() { return get_weapon_type() == WEAPON_TYPE_PISTOL; }
	inline bool is_heavy_pistol() { return get_item_definition_index() == WEAPON_DEAGLE || get_item_definition_index() == WEAPON_REVOLVER; }
	inline bool is_awp() { return get_item_definition_index() == WEAPON_AWP; }
	inline bool is_scout() { return get_weapon_type() == WEAPON_SSG08; }
	inline bool is_auto() { return get_item_definition_index() == WEAPON_SCAR20 || get_item_definition_index() == WEAPON_G3SG1; }
	inline bool is_taser() { return this->get_weapon_type() == WEAPON_TYPE_TASER; }
	inline bool is_shotgun() { return this->get_weapon_type() == WEAPON_TYPE_SHOTGUN; }
	inline bool is_smg() { return this->get_weapon_type() == WEAPON_TYPE_SMG; }
	inline bool is_rifle() { return this->get_weapon_type() == WEAPON_TYPE_RIFLE; }
	inline bool is_mg() { return this->get_weapon_type() == WEAPON_TYPE_MG; }
	inline bool is_grenade() { return this->get_weapon_type() == WEAPON_TYPE_GRENADE; }
	inline bool is_knife() { return this->get_weapon_type() == WEAPON_TYPE_KNIFE; }

	bool is_valid(const bool check_clip = true) {
		switch (this->get_weapon_type()) {
			case WEAPON_TYPE_PISTOL:  break;
			case WEAPON_TYPE_SHOTGUN: break;
			case WEAPON_TYPE_SMG:     break;
			case WEAPON_TYPE_RIFLE:   break;
			case WEAPON_TYPE_MG:      break;
			case WEAPON_TYPE_SNIPER:  break;
			default:                  return false;
		}

		return check_clip ? this->get_ammo1() > 0 : true;
	}
};

class c_base_grenade : public c_entity
{
public:
	DECLARE_NETVAR(bool, pin_pulled, "DT_BaseCSGrenade", "m_bPinPulled");
	DECLARE_NETVAR(float, throw_time, "DT_BaseCSGrenade", "m_fThrowTime");
	DECLARE_NETVAR(float, throw_strength, "DT_BaseCSGrenade", "m_flThrowStrength");
};
