#include "../../utils/render/render.hpp"
#include "../../core/interfaces.hpp"
#include "../../core/cheat.hpp"

#include "../../sdk/vector.hpp"

namespace features::visuals {
	struct box_t {
		int x, y, w, h;
	};

	// cringe but ye
	inline std::array<int, 65> m_bottom_offset;

	inline std::array<float, 65> m_alpha;

	bool setup_box( c_entity* entity, box_t& box );
	bool is_valid_entity( c_entity* entity );

	void draw_box( const box_t& box, c_player* player );
	void draw_name( const box_t& box, c_player* player );
	void draw_health( const box_t& box, c_player* player );
	void draw_armor( const box_t& box, c_player* player );
	void draw_ammo( const box_t& box, c_player* player );
	void draw_weapon( const box_t& box, c_player* player );
	void draw_flags( const box_t& box, c_player* player );
	void draw_skeleton( c_player* player );
	void draw_head_dot( c_player* player );

	void on_paint( );
}