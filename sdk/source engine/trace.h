#pragma once

#include "entity.h"
#include "macros.h"
#include "vector.h"

enum e_trace_type {
    TRACE_TYPE_EVERYTHING,
    TRACE_TYPE_WORLD_ONLY,
    TRACE_TYPE_ENTITIES_ONLY,
    TRACE_TYPE_EVERYTHING_FILTER_PROPS,
};

enum e_contents {
    CONTENTS_EMPTY = 0,
    CONTENTS_SOLID = 0x1,
    CONTENTS_WINDOW = 0x2,
    CONTENTS_AUX = 0x4,
    CONTENTS_GRATE = 0x8,
    CONTENTS_SLIME = 0x10,
    CONTENTS_WATER = 0x20,
    CONTENTS_BLOCKLOS = 0x40,
    CONTENTS_OPAQUE = 0x80,
    CONTENTS_TESTFOGVOLUME = 0x100,
    CONTENTS_UNUSED = 0x200,
    CONTENTS_BLOCKLIGHT = 0x400,
    CONTENTS_TEAM1 = 0x800,
    CONTENTS_TEAM2 = 0x1000,
    CONTENTS_IGNORE_NODRAW_OPAQUE = 0x2000,
    CONTENTS_MOVEABLE = 0x4000,
    CONTENTS_AREAPORTAL = 0x8000,
    CONTENTS_PLAYERCLIP = 0x10000,
    CONTENTS_MONSTERCLIP = 0x20000,
    CONTENTS_CURRENT_0 = 0x40000,
    CONTENTS_GRENADECLIP = 0x80000,
    CONTENTS_CURRENT_180 = 0x100000,
    CONTENTS_CURRENT_270 = 0x200000,
    CONTENTS_CURRENT_UP = 0x400000,
    CONTENTS_CURRENT_DOWN = 0x800000,
    CONTENTS_ORIGIN = 0x1000000,
    CONTENTS_MONSTER = 0x2000000,
    CONTENTS_DEBRIS = 0x4000000,
    CONTENTS_DETAIL = 0x8000000,
    CONTENTS_TRANSLUCENT = 0x10000000,
    CONTENTS_LADDER = 0x20000000,
    CONTENTS_HITBOX = 0x40000000,
};

enum e_collision_group {
    COLLISION_GROUP_NONE = 0,
    COLLISION_GROUP_DEBRIS,
    COLLISION_GROUP_DEBRIS_TRIGGER,
    COLLISION_GROUP_INTERACTIVE_DEBRIS,
    COLLISION_GROUP_INTERACTIVE,
    COLLISION_GROUP_PLAYER,
    COLLISION_GROUP_BREAKABLE_GLASS,
    COLLISION_GROUP_VEHICLE,
    COLLISION_GROUP_PLAYER_MOVEMENT,
    COLLISION_GROUP_NPC,
    COLLISION_GROUP_IN_VEHICLE,
    COLLISION_GROUP_WEAPON,
    COLLISION_GROUP_VEHICLE_CLIP,
    COLLISION_GROUP_PROJECTILE,
    COLLISION_GROUP_DOOR_BLOCKER,
    COLLISION_GROUP_PASSABLE_DOOR,
    COLLISION_GROUP_DISSOLVING,
    COLLISION_GROUP_PUSHAWAY,
    COLLISION_GROUP_NPC_ACTOR,
    COLLISION_GROUP_NPC_SCRIPTED,
    COLLISION_GROUP_PZ_CLIP,
    COLLISION_GROUP_DEBRIS_BLOCK_PROJECTILE,
    LAST_SHARED_COLLISION_GROUP
};

enum e_contents_mask {
    MASK_ALL = 0xFFFFFFFF,
    MASK_SOLID = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE),
    MASK_PLAYERSOLID = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE),
    MASK_NPCSOLID = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE),
    MASK_NPCFLUID = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER),
    MASK_WATER = (CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_SLIME),
    MASK_OPAQUE = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_OPAQUE),
    MASK_OPAQUE_AND_NPCS = (MASK_OPAQUE | CONTENTS_MONSTER),
    MASK_BLOCKLOS = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_BLOCKLOS),
    MASK_BLOCKLOS_AND_NPCS = (MASK_BLOCKLOS | CONTENTS_MONSTER),
    MASK_VISIBLE = (MASK_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE),
    MASK_VISIBLE_AND_NPCS = (MASK_OPAQUE_AND_NPCS | CONTENTS_IGNORE_NODRAW_OPAQUE),
    MASK_SHOT = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_HITBOX),
    MASK_SHOT_BRUSHONLY = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_DEBRIS),
    MASK_SHOT_HULL = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE),
    MASK_SHOT_PORTAL = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER),
    MASK_SOLID_BRUSHONLY = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_GRATE),
    MASK_PLAYERSOLID_BRUSHONLY = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_PLAYERCLIP | CONTENTS_GRATE),
    MASK_NPCSOLID_BRUSHONLY = (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE),
    MASK_NPCWORLDSTATIC = (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE),
    MASK_NPCWORLDSTATIC_FLUID = (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP),
    MASK_SPLITAREAPORTAL = (CONTENTS_WATER | CONTENTS_SLIME),
    MASK_CURRENT = (CONTENTS_CURRENT_0 | CONTENTS_GRENADECLIP | CONTENTS_CURRENT_180 | CONTENTS_CURRENT_270 | CONTENTS_CURRENT_UP | CONTENTS_CURRENT_DOWN),
    MASK_DEADSOLID = (CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_GRATE),
};

class c_base_trace_filter {
  public:
    virtual bool should_hit_entity(c_entity *entity, int contents_mask) = 0;
    virtual int get_trace_type() const = 0;
};

class c_trace_filter : public c_base_trace_filter {
  public:
    void *skip;
    char *ignore = nullptr;

    bool should_hit_entity(c_entity *entity, int contents_mask) override {
        const auto ent_cc = entity->get_networkable()->get_client_class();

        if (ent_cc && ignore && strcmp(ignore, "") != 0) {
            if (ent_cc->class_name == ignore)
                return false;
        }

        return entity != skip;
    }

    int get_trace_type() const override {
        return TRACE_TYPE_EVERYTHING;
    }

    void set_ignore_class(char *cc) {
        ignore = cc;
    }
};

class c_trace_filter_skip_entity : public c_trace_filter {
  public:
    void *skip;

    c_trace_filter_skip_entity(c_entity *entity) {
        skip = entity;
    }

    bool should_hit_entity(c_entity *entity, int contents_mask) override {
        return !(entity == skip);
    }

    int get_trace_type() const override {
        return TRACE_TYPE_EVERYTHING;
    }
};

class c_trace_filter_entities_only : public c_trace_filter {
  public:
    bool should_hit_entity(c_entity *entity, int contents_mask) override {
        return true;
    }

    int get_trace_type() const override {
        return TRACE_TYPE_ENTITIES_ONLY;
    }
};

class c_trace_filter_world_only : public c_trace_filter {
  public:
    bool should_hit_entity(c_entity *entity, int contents_mask) override {
        return false;
    }

    int get_trace_type() const override {
        return TRACE_TYPE_WORLD_ONLY;
    }
};

class c_trace_filter_world_and_props_only : public c_trace_filter {
  public:
    bool should_hit_entity(c_entity *entity, int contents_mask) override {
        return false;
    }

    int get_trace_type() const override {
        return TRACE_TYPE_EVERYTHING;
    }
};

class c_trace_filter_players_only_skip_one : public c_trace_filter {
  public:
    c_trace_filter_players_only_skip_one(c_entity *ent) {
        pEnt = ent;
    }

    bool should_hit_entity(c_entity *entity, int contents_mask) override {
        return entity != pEnt && entity->get_networkable()->get_client_class()->class_id == CCSPlayer;
    }

    int get_trace_type() const override {
        return TRACE_TYPE_ENTITIES_ONLY;
    }

  private:
    c_entity *pEnt;
};

typedef bool (*should_hit_func_t)(c_entity *entity, int contents_mask);

class c_trace_filter_simple : public c_trace_filter {
  public:
    c_trace_filter_simple(const c_entity *passentity, int collisionGroup, should_hit_func_t pExtraShouldHitCheckFn = NULL) {
        m_pPassEnt = passentity;
        m_collisionGroup = collisionGroup;
    }

    virtual bool should_hit_entity(c_entity *entity, int contents_mask) {
        return entity != m_pPassEnt;
    }

    virtual void SetPassEntity(const c_entity *pPassEntity) {
        m_pPassEnt = pPassEntity;
    }

    virtual void SetCollisionGroup(int iCollisionGroup) {
        m_collisionGroup = iCollisionGroup;
    }

    const c_entity *GetPassEntity(void) {
        return m_pPassEnt;
    }

  private:
    const c_entity *m_pPassEnt;
    int m_collisionGroup;
    should_hit_func_t m_pExtraShouldHitCheckFunction;
};

class c_trace_filter_skip_two_entities : public c_trace_filter {
  private:
    c_entity *m_ent1;
    c_entity *m_ent2;

  public:
    c_trace_filter_skip_two_entities(c_entity *ent1, c_entity *ent2) {
        m_ent1 = ent1;
        m_ent2 = ent2;
    }

    bool should_hit_entity(c_entity *entity, int contents_mask) override {
        return !(entity == m_ent1 || entity == m_ent2);
    }

    int get_trace_type() const override {
        return TRACE_TYPE_EVERYTHING;
    }
};

class c_trace_filter_hit_all : public c_trace_filter {
  public:
    bool should_hit_entity(c_entity *entity, int contents_mask) override {
        return true;
    }
};

struct __declspec(align(16)) vector_aligned_t : vector_t {
    inline vector_aligned_t() {}

    inline vector_aligned_t(float X, float Y, float Z) {
        x = X;
        y = Y;
        z = Z;
    }

    explicit vector_aligned_t(const vector_t &v) {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    vector_aligned_t &operator=(const vector_t &v) {
        x = v.x;
        y = v.y;
        z = v.z;

        return *this;
    }

    vector_aligned_t &operator=(const vector_aligned_t &v) {
        x = v.x;
        y = v.y;
        z = v.z;

        return *this;
    }

    float w;
};

struct ray_t {
  private:
  public:
    vector_aligned_t start;
    vector_aligned_t delta;
    vector_aligned_t start_offset;
    vector_aligned_t extents;
    matrix3x4_t *world_axis_transform;
    bool is_ray;
    bool is_swept;

    ray_t() : world_axis_transform(nullptr), is_ray{false}, is_swept{false} {}

    ray_t(const vector_t &start, const vector_t &end) {
        init(start, end);
    }

    ray_t(const vector_t &start, const vector_t &end, const vector_t &mins, const vector_t &maxs) : world_axis_transform(nullptr), is_ray{false}, is_swept{false} {
        delta = end - start;
        world_axis_transform = nullptr;
        is_swept = delta.length() != 0.0f;
        extents = maxs - mins;
        extents *= 0.5f;
        is_ray = extents.length_square() < 1e-6;
        start_offset = maxs + mins;
        start_offset *= 0.5f;
        this->start = start + start_offset;
        start_offset *= -1.0f;
    }

    void init(const vector_t &start, const vector_t &end) {
        delta = end - start;
        is_swept = delta.length_square() != 0.0f;
        extents = vector_t{0.0f, 0.0f, 0.0f};
        world_axis_transform = nullptr;
        is_ray = true;
        start_offset = vector_t{0.0f, 0.0f, 0.0f};
        this->start = start;
    }

    void init(const vector_t &start, const vector_t &end, const vector_t &mins, const vector_t &maxs) {
        delta = end - start;
        world_axis_transform = nullptr;
        is_swept = delta.length() != 0;
        extents = maxs - mins;
        extents *= 0.5f;
        is_ray = extents.length_square() < 1e-6;
        start_offset = maxs + mins;
        start_offset *= 0.5f;
        this->start = start + start_offset;
        start_offset *= -1.0f;
    }
};

struct cplane_t {
    vector_t normal;
    float m_dist;
    char m_type;
    char m_sign_bits;
    char m_pad[2];
};

struct csurface_t {
    const char *name;
    int16_t props;
    uint16_t flags;
};

class c_base_trace {
  public:
    // these members are aligned!!
    vector_t start_pos; // start position
    vector_t end_pos;   // final position
    cplane_t plane;     // surface normal at impact

    float fraction; // time completed, 1.0 = didn't hit anything

    int contents;                // contents on other side of surface hit
    unsigned short m_disp_flags; // displacement flags for marking surfaces with data

    bool m_allsolid;  // if true, plane is not valid
    bool start_solid; // if true, the initial point was in a solid area

    c_base_trace() {}
};

class c_game_trace : public c_base_trace {
  public:
    int get_entity_index() const {}

    bool did_hit() const {
        return fraction < 1 || m_allsolid || start_solid;
    }

    bool is_visible() const {
        return fraction > 0.97f;
    }

  public:
    float m_fractionleftsolid;            // time we left a solid, only valid if we started in solid
    csurface_t surface;                   // surface hit (impact surface)
    int hit_group;                        // 0 == generic, non-zero is specific body part
    short m_physicsbone;                  // physics bone hit by trace in studio
    unsigned short m_world_surface_index; // Index of the msurface2_t, if applicable
    c_entity *hit_ent;
    int m_hitbox; // box hit by trace in studio

    c_game_trace() {}

  private:
    // No copy constructors allowed
    c_game_trace(const c_game_trace &other)
        : m_fractionleftsolid(other.m_fractionleftsolid), surface(other.surface), hit_group(other.hit_group), m_physicsbone(other.m_physicsbone), m_world_surface_index(other.m_world_surface_index),
          hit_ent(other.hit_ent), m_hitbox(other.m_hitbox) {
        start_pos = other.start_pos;
        end_pos = other.end_pos;
        plane = other.plane;
        fraction = other.fraction;
        contents = other.contents;
        m_disp_flags = other.m_disp_flags;
        m_allsolid = other.m_allsolid;
        start_solid = other.start_solid;
    }
};

typedef c_game_trace trace_t;

class c_engine_trace {
  public:
    DECLARE_VFUNC(0, get_point_contents(const vector_t &position, int contents_mask = MASK_ALL, c_entity **entity = nullptr), int(__thiscall *)(void *, const vector_t &, int, c_entity **))
    (position, contents_mask, entity);
    DECLARE_VFUNC(1, get_point_contents_world_only(const vector_t &position, int contents_mask = MASK_ALL), int(__thiscall *)(void *, const vector_t &, int))(position, contents_mask);
    DECLARE_VFUNC(2, get_point_contents_collideable(c_collideable *collideable, const vector_t &position), int(__thiscall *)(void *, c_collideable *, const vector_t &))(collideable, position);
    DECLARE_VFUNC(3, clip_ray_to_entity(const ray_t &ray, int mask, c_entity *entity, c_base_trace *trace), void(__thiscall *)(void *, const ray_t &, int, c_entity *, c_base_trace *))
    (ray, mask, entity, trace);
    DECLARE_VFUNC(4, clip_ray_to_collideable(const ray_t &ray, int mask, vector_t *collideable, c_base_trace *trace), void(__thiscall *)(void *, const ray_t &, int, vector_t *, c_base_trace *))
    (ray, mask, collideable, trace);
    DECLARE_VFUNC(5, trace_ray(const ray_t &ray, int mask, c_base_trace_filter *filter, c_game_trace *trace), void(__thiscall *)(void *, const ray_t &, int, c_base_trace_filter *, c_game_trace *))
    (ray, mask, filter, trace);
};
