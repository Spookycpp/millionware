#pragma once
#include "matrix.hpp"
#include "vector.hpp"

struct c_model {
	void* handle;
	char name[255];
};

struct c_model_render_info {
	vector3_t origin;
	vector3_t angles;
	char pad[0x4];
	void* renderable;
	const c_model* model;
	const matrix3x4_t* model_to_world;
	const matrix3x4_t* lighting_offset;
	const vector3_t* lighting_origin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	unsigned short instance;
};