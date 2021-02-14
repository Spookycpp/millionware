#pragma once

#include "macros.hpp"
#include "vector.hpp"

class c_debug_overlay {
	VIRTUAL_METHOD( int, screen_position, 13, ( const vector3_t& in, vector3_t& out ), &in, &out );

public:
	point_t screen_position( const vector3_t& world_pos ) {
		vector3_t screen_pos;

		if( screen_position( world_pos, screen_pos ) != 1 )
			return point_t( static_cast< int >( screen_pos.x ), static_cast< int >( screen_pos.y ) );

		return point_t( 0, 0 );
	}

};
