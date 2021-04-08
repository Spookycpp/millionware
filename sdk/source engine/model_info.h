#pragma once

#include <cstdint>

#include "macros.h"
#include "studio.h"
#include "vector.h"
#include "matrix.h"

struct c_model
{
	uintptr_t handle;

	char name[255];
};

struct c_model_render_info {
	vector_t origin;
	vector_t angles;
	uintptr_t pad[0x4];
	uintptr_t renderable;
	c_model *model;
	matrix3x4_t *model_to_world;
	matrix3x4_t *lighting_offset;
	vector_t *lighting_origin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	short instance;
};

class c_model_info {
public:
	DECLARE_VFUNC(1, get_model(int index), c_model *(__thiscall *)(void *, int))(index);
	DECLARE_VFUNC(2, get_model_index(const char *filename), int(__thiscall *)(void *, const char *))(filename);
	DECLARE_VFUNC(2, get_model_name(c_model *model), const char *(__thiscall *)(void *, c_model *))(model);
	DECLARE_VFUNC(32, get_studio_model(c_model *model), studio_hdr_t *(__thiscall *)(void *, c_model *))(model);
};