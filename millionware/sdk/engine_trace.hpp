#pragma once

#include "entity.hpp"

enum class trace_type_t {
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};

class i_trace_filter {
public:
	virtual bool should_hit_entity(c_entity* pEntity, int contentsMask) = 0;
	virtual trace_type_t get_trace_type() const = 0;
};

class c_trace_filter : public i_trace_filter {
public:
	void* skip;
	char* ignore = nullptr;

	bool should_hit_entity(c_entity* pEntityHandle, int /*contentsMask*/) override {
		const auto ent_cc = pEntityHandle->networkable()->get_client_class();
		if (ent_cc && ignore && strcmp(ignore, "") != 0) {
			if (ent_cc->network_name == ignore)
				return false;
		}

		return !(pEntityHandle == skip);
	}

	trace_type_t get_trace_type() const override {
		return trace_type_t::TRACE_EVERYTHING;
	}

	void set_ignore_class(char* cc) {
		ignore = cc;
	}
};

class c_trace_filter_skip_entity : public c_trace_filter {
public:
	void* skip;

	c_trace_filter_skip_entity(c_entity* pEntityHandle) {
		skip = pEntityHandle;
	}

	bool should_hit_entity(c_entity* pEntityHandle, int /*contentsMask*/) override {
		return !(pEntityHandle == skip);
	}

	trace_type_t get_trace_type() const override {
		return trace_type_t::TRACE_EVERYTHING;
	}
};

class c_trace_filter_entities_only : public c_trace_filter {
public:
	bool should_hit_entity(c_entity* pEntityHandle, int /*contentsMask*/) override {
		return true;
	}

	trace_type_t get_trace_type() const override {
		return trace_type_t::TRACE_ENTITIES_ONLY;
	}
};

class c_trace_filter_world_only : public c_trace_filter {
public:
	bool should_hit_entity(c_entity* /*pServerEntity*/, int /*contentsMask*/) override {
		return false;
	}

	trace_type_t get_trace_type() const override {
		return trace_type_t::TRACE_WORLD_ONLY;
	}
};

class c_trace_filter_world_and_props_only : public c_trace_filter {
public:
	bool should_hit_entity(c_entity* /*pServerEntity*/, int /*contentsMask*/) override {
		return false;
	}

	trace_type_t get_trace_type() const override {
		return trace_type_t::TRACE_EVERYTHING;
	}
};

class c_trace_filter_players_only_skip_one : public c_trace_filter {
public:
	c_trace_filter_players_only_skip_one(c_entity* ent) {
		pEnt = ent;
	}

	bool should_hit_entity(c_entity* pEntityHandle, int /*contentsMask*/) override {
		return pEntityHandle != pEnt && pEntityHandle->networkable()->get_client_class()->class_id == ccsplayer;
	}

	trace_type_t get_trace_type() const override {
		return trace_type_t::TRACE_ENTITIES_ONLY;
	}

private:
	c_entity* pEnt;
};

typedef bool (*should_hit_func_t)(c_entity* pHandleEntity, int contentsMask);

class c_trace_filter_simple : public c_trace_filter {
public:
	// It does have a base, but we'll never network anything below here..
	c_trace_filter_simple(const c_entity* passentity, int collisionGroup,
		should_hit_func_t pExtraShouldHitCheckFn = NULL);
	virtual bool should_hit_entity(c_entity* pHandleEntity, int contentsMask);

	virtual void SetPassEntity(const c_entity* pPassEntity) {
		m_pPassEnt = pPassEntity;
	}

	virtual void SetCollisionGroup(int iCollisionGroup) {
		m_collisionGroup = iCollisionGroup;
	}

	const c_entity* GetPassEntity(void) {
		return m_pPassEnt;
	}

private:
	const c_entity* m_pPassEnt;
	int m_collisionGroup;
	should_hit_func_t m_pExtraShouldHitCheckFunction;
};

class c_trace_filter_skip_two_entities : public c_trace_filter {
private:
	c_entity* m_ent1;
	c_entity* m_ent2;

public:
	c_trace_filter_skip_two_entities(c_entity* ent1, c_entity* ent2) {
		m_ent1 = ent1;
		m_ent2 = ent2;
	}

	bool should_hit_entity(c_entity* pEntityHandle, int /*contentsMask*/) override {
		return !(pEntityHandle == m_ent1 || pEntityHandle == m_ent2);
	}

	trace_type_t get_trace_type() const override {
		return trace_type_t::TRACE_EVERYTHING;
	}
};

class c_trace_filter_hit_all : public c_trace_filter {
public:
	bool should_hit_entity(c_entity* /*pServerEntity*/, int /*contentsMask*/) override {
		return true;
	}
};

struct ray_t {
private:
public:
	vector3_t m_start;
	vector3_t m_delta;
	vector3_t m_start_offset;
	vector3_t m_extents;
	const matrix3x4_t* m_world_axis_transform;
	bool m_is_ray;
	bool m_is_swept;

	ray_t() : m_world_axis_transform(nullptr), m_is_ray{ false }, m_is_swept{ false } { }

	ray_t(vector3_t const& start, vector3_t const& end) {
		init(start, end);
	}

	ray_t(vector3_t const& start, vector3_t const& end, vector3_t const& mins, vector3_t const& maxs) :
		m_world_axis_transform(nullptr), m_is_ray{ false }, m_is_swept{ false } {
		m_delta = end - start;

		m_world_axis_transform = nullptr;
		m_is_swept = m_delta.length() != 0;

		m_extents = maxs - mins;
		m_extents *= 0.5f;
		m_is_ray = m_extents.length_sqr() < 1e-6;

		m_start_offset = maxs + mins;
		m_start_offset *= 0.5f;
		m_start = start + m_start_offset;
		m_start_offset *= -1.0f;
	}

	void init(vector3_t const& start, vector3_t const& end) {
		m_delta = end - start;

		m_is_swept = m_delta.length_sqr() != 0;

		m_extents = { 0.f, 0.f, 0.f };

		m_world_axis_transform = nullptr;
		m_is_ray = true;

		m_start_offset = { 0.0f, 0.0f, 0.0f };
		m_start = start;
	}

	void init(vector3_t const& start, vector3_t const& end, vector3_t const& mins, vector3_t const& maxs) {
		m_delta = end - start;

		m_world_axis_transform = nullptr;
		m_is_swept = m_delta.length() != 0;

		m_extents = maxs - mins;
		m_extents *= 0.5f;
		m_is_ray = m_extents.length_sqr() < 1e-6;

		m_start_offset = maxs + mins;
		m_start_offset *= 0.5f;
		m_start = start + m_start_offset;
		m_start_offset *= -1.0f;
	}
};

class c_base_trace {
public:
	bool is_disp_surface(void) {
		return ((m_disp_flags & DISPSURF_FLAG_SURFACE) != 0);
	}

	bool is_disp_surface_walkable(void) {
		return ((m_disp_flags & DISPSURF_FLAG_WALKABLE) != 0);
	}

	bool is_disp_surface_buildable(void) {
		return ((m_disp_flags & DISPSURF_FLAG_BUILDABLE) != 0);
	}

	bool is_disp_surface_prop1(void) {
		return ((m_disp_flags & DISPSURF_FLAG_SURFPROP1) != 0);
	}

	bool is_disp_surface_prop2(void) {
		return ((m_disp_flags & DISPSURF_FLAG_SURFPROP2) != 0);
	}

public:

	// these members are aligned!!
	vector3_t start_pos; // start position
	vector3_t end_pos; // final position
	cplane_t plane; // surface normal at impact

	float fraction; // time completed, 1.0 = didn't hit anything

	int contents; // contents on other side of surface hit
	unsigned short m_disp_flags; // displacement flags for marking surfaces with data

	bool m_allsolid; // if true, plane is not valid
	bool start_solid; // if true, the initial point was in a solid area

	c_base_trace() {}
};

class c_game_trace : public c_base_trace {
public:
	int get_entity_index() const { }

	bool did_hit() const {
		return fraction < 1 || m_allsolid || start_solid;
	}

	bool is_visible() const {
		return fraction > 0.97f;
	}

public:
	float m_fractionleftsolid; // time we left a solid, only valid if we started in solid
	csurface_t surface; // surface hit (impact surface)
	int hit_group; // 0 == generic, non-zero is specific body part
	short m_physicsbone; // physics bone hit by trace in studio
	unsigned short m_world_surface_index; // Index of the msurface2_t, if applicable
	c_entity* hit_ent;
	int m_hitbox; // box hit by trace in studio

	c_game_trace() {}

private:
	// No copy constructors allowed
	c_game_trace(const c_game_trace& other) :
		m_fractionleftsolid(other.m_fractionleftsolid),
		surface(other.surface),
		hit_group(other.hit_group),
		m_physicsbone(other.m_physicsbone),
		m_world_surface_index(other.m_world_surface_index),
		hit_ent(other.hit_ent),
		m_hitbox(other.m_hitbox) {
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

class i_engine_trace {
public:
	virtual int get_point_contents(const vector3_t& vecAbsPosition, int contentsMask = MASK_ALL, c_entity** ppEntity = nullptr) = 0;
	virtual int get_point_contents_world_only(const vector3_t& vecAbsPosition, int contentsMask = MASK_ALL) = 0;
	virtual int get_point_contents_collideable(c_collideable* pCollide, const vector3_t& vecAbsPosition) = 0;
	virtual void clip_ray_to_entity(const ray_t& ray, unsigned int fMask, c_entity* pEnt, c_base_trace* pTrace) = 0;
	virtual void clip_ray_to_collideable(const ray_t& ray, unsigned int fMask, c_collideable* pCollide, trace_t* pTrace) = 0;
	virtual void trace_ray(const ray_t& ray, unsigned int fMask, i_trace_filter* pTraceFilter, trace_t* pTrace) = 0;
};