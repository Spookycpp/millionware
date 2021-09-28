#pragma once

#include "../core/patterns/patterns.h"

#include <array>
#include <cstdint>

#include "client_class.h"
#include "econ_item.h"
#include "macros.h"
#include "matrix.h"
#include "model_info.h"
#include "util_vector.h"
#include "vector.h"
#include "weapon_system.h"

#define INVALID_EHANDLE_INDEX 0xFFFFFFFF

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

struct animation_layer_t {
    char pad_0x8[0x8];
    uint32_t unk1;
    uint32_t unk2;
    uint32_t unk3;
    uint32_t order;          // 0x0014
    uint32_t sequence;       // 0x0018
    float prev_cycle;        // 0x001C
    float weight;            // 0x0020
    float weight_delta_rate; // 0x0024
    float playback_rate;     // 0x0028
    float cycle;             // 0x2C
    void *player;            // 0x30
    char pad_0x4[0x4];
};

struct animation_data_t {
    std::array<animation_layer_t, 13> anim_layers;
    std::array<animation_layer_t, 13> last_layers;
    std::array<float, 24> pose_params;
    std::array<matrix3x4_t, 128> bones;
    int activity;
    float sim_time;
    float last_sim_time;
    vector_t last_interp_origin;
    float fraction;
    vector_t interp_origin;
    vector_t last_origin;
    vector_t last_velocity;
    vector_t anim_velocity;
    bool valid;
    float last_duck;
    int prev_flags;
    int anim_flags;
    float adjust_rate;
    float adjust_cycle;
    bool is_stopping;
    float last_anim_time;
};

enum e_life_state {
    LIFE_STATE_ALIVE,
    LIFE_STATE_KILL_CAM,
    LIFE_STATE_DEAD,
};

enum observer_mode_t {
    OBS_MODE_NONE = 0,  // not in spectator mode
    OBS_MODE_DEATHCAM,  // special mode for death cam animation
    OBS_MODE_FREEZECAM, // zooms to a target, and freeze-frames on them
    OBS_MODE_FIXED,     // view from a fixed camera position
    OBS_MODE_IN_EYE,    // follow a player in first person view
    OBS_MODE_CHASE,     // follow a player in third person view
    OBS_MODE_POI, // PASSTIME point of interest - game objective, big fight, anything interesting; added in the middle of the enum due to
                  // tons of hard-coded "<ROAMING" enum compares
    OBS_MODE_ROAMING, // free roaming

    NUM_OBSERVER_MODES,
};

enum e_team_num {
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
    BONE_USED_BY_HITBOX = 0x00000100,     // bone (or child) is used by a hit box
    BONE_USED_BY_ATTACHMENT = 0x00000200, // bone (or child) is used by an attachment point
    BONE_USED_BY_VERTEX_MASK = 0x0003FC00,
    BONE_USED_BY_VERTEX_LOD0 = 0x00000400, // bone (or child) is used by the toplevel model via skinned vertex
    BONE_USED_BY_VERTEX_LOD1 = 0x00000800,
    BONE_USED_BY_VERTEX_LOD2 = 0x00001000,
    BONE_USED_BY_VERTEX_LOD3 = 0x00002000,
    BONE_USED_BY_VERTEX_LOD4 = 0x00004000,
    BONE_USED_BY_VERTEX_LOD5 = 0x00008000,
    BONE_USED_BY_VERTEX_LOD6 = 0x00010000,
    BONE_USED_BY_VERTEX_LOD7 = 0x00020000,
    BONE_USED_BY_BONE_MERGE = 0x00040000 // bone is available for bone merge to occur against it
};

// How many bits to use to encode an edict.
#define MAX_EDICT_BITS 11 // # of bits needed to represent max edicts
// Max # of edicts in a level
#define MAX_EDICTS (1 << MAX_EDICT_BITS)

// types of precipitation
enum precipitation_type_t {
    PRECIPITATION_TYPE_RAIN = 0,
    PRECIPITATION_TYPE_SNOW,
    PRECIPITATION_TYPE_ASH,
    PRECIPITATION_TYPE_SNOWFALL,
    PRECIPITATION_TYPE_PARTICLERAIN,
    PRECIPITATION_TYPE_PARTICLEASH,
    PRECIPITATION_TYPE_PARTICLERAINSTORM,
    PRECIPITATION_TYPE_PARTICLESNOW,
    NUM_PRECIPITATION_TYPES
};

class c_animation_layer {
public:
    bool client_blend;       // 0x0000
    float blend_in;          // 0x0004
    void *studio_hdr;        // 0x0008
    int dispatch_sequence;   // 0x000C
    int dispatch_sequence_2; // 0x0010
    int order;               // 0x0014
    int sequence;            // 0x0018
    float prev_cycle;        // 0x001C
    float weight;            // 0x0020
    float weight_delta_rate; // 0x0024
    float playback_rate;     // 0x0028
    float cycle;             // 0x002C
    void *owner;             // 0x0030
    char pad_0038[4];        // 0x0034
};

struct entity_handle_t {
    uintptr_t handle;

    constexpr entity_handle_t() : handle(0xFFFFFFFF) {
    }

    constexpr entity_handle_t(unsigned long handle) : handle(handle) {
    }

    class c_entity *get() const;

    bool operator==(const entity_handle_t &other) const;
    bool operator==(class c_entity *entity) const;

    bool operator!=(const entity_handle_t &other) const;
    bool operator!=(class c_entity *entity) const;
};

class c_collideable {
public:
    DECLARE_VFUNC(1, get_mins(), vector_t &(__thiscall *) (void *) )();
    DECLARE_VFUNC(2, get_maxs(), vector_t &(__thiscall *) (void *) )();
};

class c_networkable {
public:
    DECLARE_VFUNC(1, release(), void(__thiscall *)(void *))();
    DECLARE_VFUNC(2, get_client_class(), c_client_class *(__thiscall *) (void *) )();
    DECLARE_VFUNC(4, on_pre_data_changed(int type), void(__thiscall *)(void *, int))(type);
    DECLARE_VFUNC(5, on_data_changed(int type), void(__thiscall *)(void *, int))(type);
    DECLARE_VFUNC(6, pre_data_update(int type), void(__thiscall *)(void *, int))(type);
    DECLARE_VFUNC(7, post_data_update(int type), void(__thiscall *)(void *, int))(type);
    DECLARE_VFUNC(9, is_dormant(), bool(__thiscall *)(void *))();
    DECLARE_VFUNC(10, index(), int(__thiscall *)(void *))();
};

class c_renderable {
public:
    DECLARE_VFUNC(1, get_render_origin(), vector_t &(__thiscall *) (void *) )();
    DECLARE_VFUNC(2, get_render_angles(), vector_t &(__thiscall *) (void *) )();
    DECLARE_VFUNC(3, should_draw(), bool(__thiscall *)(void *))();
    DECLARE_VFUNC(8, get_model(), c_model *(__thiscall *) (void *) )();
    DECLARE_VFUNC(13, setup_bones(matrix3x4_t *bone_to_world_out, int max_bones, int bone_mask, float current_time),
                  bool(__thiscall *)(void *, matrix3x4_t *, int, int, float))
    (bone_to_world_out, max_bones, bone_mask, current_time);
};

class c_entity {
public:
    // networked variables
    DECLARE_OFFSET(get_renderable(), (c_renderable *) ((uintptr_t) this + 0x4));
    DECLARE_OFFSET(get_networkable(), (c_networkable *) ((uintptr_t) this + 0x8));

    DECLARE_VFUNC(3, get_collideable(), c_collideable *(__thiscall *) (void *) )();
    DECLARE_VFUNC(10, get_abs_origin(), vector_t &(__thiscall *) (void *) )();
    DECLARE_VFUNC(142, get_class_name(), const char *(__thiscall *) (void *) )();
    DECLARE_VFUNC(158, is_player(), bool(__thiscall *)(void *))(); // @xref: "effects/nightvision"
    DECLARE_VFUNC(166, is_weapon(), bool(__thiscall *)(void *))();

    DECLARE_NETVAR(float, simulation_time, "DT_BaseEntity", "m_flSimulationTime");
    DECLARE_NETVAR(float, c4_blow, "DT_PlantedC4", "m_flC4Blow");

    DECLARE_NETVAR(bool, is_spotted, "DT_BaseEntity", "m_bSpotted");

    DECLARE_NETVAR(bool, is_bomb_ticking, "DT_PlantedC4", "m_bBombTicking");
    DECLARE_NETVAR(bool, is_bomb_defused, "DT_PlantedC4", "m_bBombDefused");
    DECLARE_NETVAR(float, bomb_blow_time, "DT_PlantedC4", "m_flC4Blow");
    DECLARE_NETVAR(entity_handle_t, bomb_defuser, "DT_PlantedC4", "m_hBombDefuser");
    DECLARE_NETVAR(float, defuse_countdown, "DT_PlantedC4", "m_flDefuseCountDown");

    DECLARE_NETVAR(int, flags, "DT_BasePlayer", "m_fFlags");
    DECLARE_NETVAR(int, team_num, "DT_BaseEntity", "m_iTeamNum");

    DECLARE_NETVAR(vector_t, vec_origin, "DT_BaseEntity", "m_vecOrigin");
    DECLARE_NETVAR(vector_t, mins, "DT_CollisionProperty", "m_vecMins");
    DECLARE_NETVAR(vector_t, maxs, "DT_CollisionProperty", "m_vecMaxs");

    DECLARE_NETVAR(entity_handle_t, owner_handle, "DT_BaseEntity", "m_hOwnerEntity");
    DECLARE_NETVAR_OFFSET(matrix3x4_t, transformation_matrix, "DT_BaseEntity", "m_CollisionGroup", -48);

    DECLARE_NETVAR(int, fire_count, "DT_Inferno", "m_fireCount");
    DECLARE_NETVAR_PTR(bool, fire_is_burning, "DT_Inferno", "m_bFireIsBurning");
    DECLARE_NETVAR_PTR(int, fire_x_delta, "DT_Inferno", "m_fireXDelta");
    DECLARE_NETVAR_PTR(int, fire_y_delta, "DT_Inferno", "m_fireYDelta");
    DECLARE_NETVAR_PTR(int, fire_z_delta, "DT_Inferno", "m_fireZDelta");

    bool is_grenade();

	template <typename t>
    __forceinline t &get(size_t offset) {
        return *(t *) ((uintptr_t) this + offset);
    }

    template <typename t>
    __forceinline void set(size_t offset, const t &val) {
        *(t *) ((uintptr_t) this + offset) = val;
    }

    template <typename t>
    __forceinline t as() {
        return (t) this;
    }
};

class c_player : public c_entity {
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
    DECLARE_NETVAR(float, next_attack, "DT_BCCLocalPlayerExclusive", "m_flNextAttack");
    DECLARE_NETVAR(float, fall_velocity, "DT_Local", "m_flFallVelocity");
    DECLARE_NETVAR(int, shots_fired, "DT_CSLocalPlayerExclusive", "m_iShotsFired");
    DECLARE_NETVAR(int, armor, "DT_CSPlayer", "m_ArmorValue");
    DECLARE_NETVAR(int, health, "DT_BasePlayer", "m_iHealth");
    DECLARE_NETVAR(int, max_health_n, "DT_BasePlayer", "m_iMaxHealth");
    DECLARE_NETVAR(int, life_state, "DT_BasePlayer", "m_lifeState");
    DECLARE_NETVAR(int, tick_base, "DT_LocalPlayerExclusive", "m_nTickBase");
    DECLARE_NETVAR(int, collision_group, "DT_BasePlayer", "m_CollisionGroup");
    DECLARE_NETVAR(int, hitbox_set, "DT_BaseAnimating", "m_nHitboxSet");
    DECLARE_NETVAR(int, money, "DT_CSPlayer", "m_iAccount");
    DECLARE_NETVAR(int, observer_mode, "DT_BasePlayer", "m_iObserverMode");
    DECLARE_NETVAR(int, survival_team, "DT_BasePlayer", "m_nSurvivalTeam");
    DECLARE_NETVAR(vector_t, eye_angles, "DT_CSPlayer", "m_angEyeAngles[0]");
    DECLARE_NETVAR(vector_t, punch_angle, "DT_Local", "m_viewPunchAngle");
    DECLARE_NETVAR(vector_t, aim_punch_angle, "DT_Local", "m_aimPunchAngle");
    DECLARE_NETVAR(vector_t, velocity, "DT_LocalPlayerExclusive", "m_vecVelocity[0]");
    DECLARE_NETVAR(vector_t, view_offset, "DT_LocalPlayerExclusive", "m_vecViewOffset[0]");
    DECLARE_NETVAR(entity_handle_t, observer_target, "DT_BasePlayer", "m_hObserverTarget");
    DECLARE_NETVAR(entity_handle_t, active_weapon_handle, "DT_BaseCombatCharacter", "m_hActiveWeapon");
    DECLARE_NETVAR(entity_handle_t, view_model_handle, "DT_BasePlayer", "m_hViewModel[0]");
    DECLARE_NETVAR(entity_handle_t, ground_entity, "DT_BasePlayer", "m_hGroundEntity");
    DECLARE_NETVAR(entity_handle_t, my_weapons, "DT_BasePlayer", "m_hMyWeapons");
    DECLARE_NETVAR(entity_handle_t, my_wearables, "DT_BasePlayer", "m_hMyWearables");

    DECLARE_NETVAR_OFFSET(float, surface_friction, "DT_CSPlayer", "m_vecLadderNormal", -4);
    DECLARE_NETVAR_OFFSET(float, gravity, "DT_CSPlayer", "m_iTeamNum", -14);
    DECLARE_NETVAR_OFFSET(int, old_simulation_time, "DT_BaseEntity", "m_flSimulationTime", 4);
    DECLARE_NETVAR_OFFSET(int, move_type, "DT_BaseEntity", "m_nRenderMode", 1);
    DECLARE_NETVAR_OFFSET(int, glow_index, "DT_CSPlayer", "m_flFlashDuration", 24);

    CUtlVector<c_animation_layer> &animation_overlay();
    CUtlVector<matrix3x4_t> &get_cached_bone_data();

    void set_abs_angles(const vector_t &angle);
    void set_absolute_origin(const vector_t &new_origin);

    vector_t get_eye_pos() const;
    vector_t extrapolate_position(const vector_t &pos);
    vector_t get_hitbox_pos(int idx);

    float get_flash_time();

    int max_health();
    int sequence_activity(int sequence);

    bool is_flashed();
    bool can_shoot();
    bool can_shoot(class c_weapon *weapon);
    bool is_valid(bool check_alive = true);
    bool is_alive();
    bool is_enemy();
    bool is_visible(c_player *local, const vector_t &src, const vector_t &dst);
    bool is_visible(c_player *local, const vector_t &src);
    bool is_reloading();
    bool is_smoked();
    bool has_bomb();
};

class i_client_unknown {
public:
    DECLARE_VFUNC(6, get_base_entity(), c_entity *(__thiscall *) (i_client_unknown *) )();
};

class i_client_renderable {
public:
    DECLARE_VFUNC(0, get_i_client_unknown(), i_client_unknown *(__thiscall *) (i_client_renderable *) )();
};

class c_economy_item : public c_entity {
public:
    DECLARE_NETVAR(short, item_definition_index, "DT_ScriptCreatedItem", "m_iItemDefinitionIndex");
    DECLARE_NETVAR(bool, is_initialized, "DT_ScriptCreatedItem", "m_bInitialized");
    DECLARE_NETVAR(int, entity_level, "DT_ScriptCreatedItem", "m_iEntityLevel");
    DECLARE_NETVAR(int, account_id, "DT_ScriptCreatedItem", "m_iAccountID");
    DECLARE_NETVAR(int, item_id_low, "DT_ScriptCreatedItem", "m_iItemIDLow");
    DECLARE_NETVAR(int, item_id_high, "DT_ScriptCreatedItem", "m_iItemIDHigh");
    DECLARE_NETVAR(int, entity_quality, "DT_ScriptCreatedItem", "m_iEntityQuality");
    DECLARE_NETVAR(int, original_owner_xuid_low, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
    DECLARE_NETVAR(int, original_owner_xuid_high, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");

    c_econ_item_view *get_econ_item_view();
};

class c_grenade : public c_entity {
public:
    DECLARE_NETVAR(int, explode_effect_tick_begin, "DT_BaseCSGrenadeProjectile", "m_nExplodeEffectTickBegin");
    DECLARE_NETVAR(int, smoke_effect_tick_begin, "DT_BaseCSGrenadeProjectile", "m_nSmokeEffectTickBegin");
    DECLARE_NETVAR(vector_t, initial_velocity, "DT_BaseCSGrenadeProjectile", "m_vInitialVelocity");
    DECLARE_NETVAR_OFFSET(float, spawn_time, "DT_BaseCSGrenadeProjectile", "m_vecExplodeEffectOrigin", 12);
};

class c_weapon : public c_economy_item {
public:
    DECLARE_NETVAR(bool, is_burst_mode, "DT_WeaponCSBase", "m_bBurstMode");
    DECLARE_NETVAR(float, next_primary_attack, "DT_LocalActiveWeaponData", "m_flNextPrimaryAttack");
    DECLARE_NETVAR(float, next_secondary_attack, "DT_LocalActiveWeaponData", "m_flNextSecondaryAttack");
    DECLARE_NETVAR(float, ready_time, "DT_WeaponCSBase", "m_flPostponeFireReadyTime");
    DECLARE_NETVAR(float, recoil_index, "DT_WeaponCSBase", "m_flRecoilIndex");
    DECLARE_NETVAR(int, ammo1, "DT_BaseCombatWeapon", "m_iClip1");
    DECLARE_NETVAR(int, ammo2, "DT_BaseCombatWeapon", "m_iClip2");
    DECLARE_NETVAR(int, reserve_ammo_count, "DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount");
    DECLARE_NETVAR(int, burst_shots_remaining, "DT_WeaponCSBaseGun", "m_iBurstShotsRemaining");

    DECLARE_VFUNC(453, get_spread(), float(__thiscall *)(void *))();
    DECLARE_VFUNC(483, get_inaccuracy(), float(__thiscall *)(void *))();

    weapon_info_t *get_info();

    int get_weapon_type();
    bool has_scope();

    inline bool is_pistol() {
        return get_weapon_type() == WEAPON_TYPE_PISTOL;
    }
    inline bool is_heavy_pistol() {
        return get_item_definition_index() == WEAPON_DEAGLE || get_item_definition_index() == WEAPON_REVOLVER;
    }
    inline bool is_awp() {
        return get_item_definition_index() == WEAPON_AWP;
    }
    inline bool is_scout() {
        return get_item_definition_index() == WEAPON_SSG08;
    }
    inline bool is_auto() {
        return get_item_definition_index() == WEAPON_SCAR20 || get_item_definition_index() == WEAPON_G3SG1;
    }
    inline bool is_taser() {
        return this->get_weapon_type() == WEAPON_TYPE_TASER;
    }
    inline bool is_shotgun() {
        return this->get_weapon_type() == WEAPON_TYPE_SHOTGUN;
    }
    inline bool is_smg() {
        return this->get_weapon_type() == WEAPON_TYPE_SMG;
    }
    inline bool is_rifle() {
        return this->get_weapon_type() == WEAPON_TYPE_RIFLE;
    }
    inline bool is_mg() {
        return this->get_weapon_type() == WEAPON_TYPE_MG;
    }
    inline bool is_grenade() {
        return this->get_weapon_type() == WEAPON_TYPE_GRENADE;
    }
    inline bool is_knife() {
        return this->get_weapon_type() == WEAPON_TYPE_KNIFE;
    }

    bool is_valid(const bool check_clip = true);
};

class c_base_grenade : public c_weapon {
public:
    DECLARE_NETVAR(bool, pin_pulled, "DT_BaseCSGrenade", "m_bPinPulled");
    DECLARE_NETVAR(float, throw_time, "DT_BaseCSGrenade", "m_fThrowTime");
    DECLARE_NETVAR(float, throw_strength, "DT_BaseCSGrenade", "m_flThrowStrength");
};

class c_game_rules {
public:
    DECLARE_NETVAR(bool, freeze_period, "DT_CSGameRules", "m_bFreezePeriod");

    static c_game_rules *get() {
        return **(c_game_rules ***) (patterns::get_game_rules_proxy() + 1);
    }
};

class c_player_resource {
public:
    bool is_c4_carrier(int index) {
        const static auto offset = netvars::get(CRC_CT("DT_CSPlayerResource:m_iPlayerC4"));

        return index == *(int *) ((uintptr_t) this + offset);
    }

    int get_ping(int index) {
        const static auto offset = netvars::get(CRC_CT("DT_CSPlayerResource:m_iPing"));

        return *(int *) ((uintptr_t) this + offset + index * 4);
    }

    DECLARE_NETVAR(vector_t, bomb_site_center_a, "DT_CSPlayerResource", "m_bombsiteCenterA");
    DECLARE_NETVAR(vector_t, bomb_site_center_b, "DT_CSPlayerResource", "m_bombsiteCenterB");
};

class c_precipitation_entity : public c_entity {
public:
    DECLARE_NETVAR(int, precip_type, "DT_Precipitation", "m_nPrecipType");
};
