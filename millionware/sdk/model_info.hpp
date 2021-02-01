#pragma once
#include "matrix.hpp"
#include "vector.hpp"
#include "macros.hpp"

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

struct studio_hdr_t;
class c_model_info {
public:
	VIRTUAL_METHOD( c_model*, get_model, 1, ( int index ), index );
	VIRTUAL_METHOD( int, get_model_index, 2, ( const char* filename ), filename );
	VIRTUAL_METHOD( const char*, get_model_name, 2, ( const c_model* model ), model );
	VIRTUAL_METHOD( studio_hdr_t*, get_studio_model, 32, ( const c_model* model ), model );
};