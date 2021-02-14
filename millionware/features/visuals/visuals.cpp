#include "visuals.hpp"
#include "../../core/config.hpp"

bool features::visuals::setup_box( c_entity* entity, box_t& box ) {
	vector3_t pos = entity->abs_origin( ); // abs_origin
	vector3_t top = pos + vector3_t( 0, 0, entity->get_collideable( )->get_maxs( ).z );

	const auto pos_screen = interfaces::debug_overlay->screen_position( pos );
	const auto top_screen = interfaces::debug_overlay->screen_position( top );

	// homo
	if( ( pos_screen.x + pos_screen.y == 0 ) || ( top_screen.x + top_screen.y == 0 ) )
		return false;

	box.x = static_cast< int >( top_screen.x - ( ( pos_screen.y - top_screen.y ) / 2 ) / 2 );
	box.y = static_cast< int >( top_screen.y );

	box.w = static_cast< int >( ( ( pos_screen.y - top_screen.y ) ) / 2 );
	box.h = static_cast< int >( ( pos_screen.y - top_screen.y ) );

	bool out_of_bounds = pos_screen.x + box.w + 20 < 0 || pos_screen.x - box.w - 20 > cheat::screen_size.x || pos_screen.y + 20 < 0 || pos_screen.y - box.h - 20 > cheat::screen_size.y;
	return !out_of_bounds;
}

bool features::visuals::is_valid_entity( c_entity* entity ) {
	if( !entity ) {
		return false;
	}

	const auto local = ( c_entity* )interfaces::entity_list->get_by_index( interfaces::engine_client->get_local_player( ) );
	if( !local ) {
		return false;
	}

	if( entity->is_player( ) ) {

		// could be made into a one-liner via ternary operators 
		// but fuck that i like readability LoL
		if( !entity->networkable( )->is_dormant( ) ) {
			m_alpha.at( entity->networkable( )->index( ) ) += 5.f * interfaces::global_vars->frame_time;
		}
		else {
			m_alpha.at( entity->networkable( )->index( ) ) -= 1.f * interfaces::global_vars->frame_time;
		}

		m_alpha.at( entity->networkable( )->index( ) ) = std::clamp( m_alpha.at( entity->networkable( )->index( ) ), 0.f, 1.f );

		if( entity->team_num( ) == local->team_num( ) ) {
			return false;
		}

		if( m_alpha.at( entity->networkable( )->index( ) ) <= 0.f ) {
			return false;
		}

		return entity->as_player( )->life_state( ) == LIFE_STATE_ALIVE;
	}

	return !entity->networkable( )->is_dormant( );
}

void features::visuals::draw_box( const box_t& box, c_player* player ) {
	if( !config::get<bool>( FNV_CT( "visuals.enemy.bounding_box" ) ) ) {
		return;
	}

	render::rect( box.x, box.y, box.w, box.h, color_t( 255, 255, 255, 210 ) );
	render::rect( box.x - 1, box.y - 1, box.w + 2, box.h + 2, color_t( 0, 0, 0, 180 ) );
	render::rect( box.x + 1, box.y + 1, box.w - 2, box.h - 2, color_t( 0, 0, 0, 180 ) );
}

void features::visuals::draw_name( const box_t& box, c_player* player ) {
	if( !config::get<bool>( FNV_CT( "visuals.enemy.name" ) ) ) {
		return;
	}

	player_info_t info;
	if( !interfaces::engine_client->get_player_info( player->entity_index( ), info ) ) {
		return;
	}

	const auto text_size = render::measure_text( e_font::VERDANA12, info.name );
	render::text( box.x + ( box.w / 2 ) - ( text_size.x / 2 ), box.y - text_size.y, e_font::VERDANA12, color_t( 255, 255, 255, 210 ), info.name );
}

void features::visuals::draw_health( const box_t& box, c_player* player ) {
	if( !config::get<bool>( FNV_CT( "visuals.enemy.health" ) ) ) {
		return;
	}

	const int clamped_health = std::clamp( player->health( ), 0, 100 );

	const int bar_size = std::clamp( ( clamped_health * box.h ) / 100, 0, box.h );
	const int red = std::min( ( 510 * ( 100 - clamped_health ) ) / 100, 255 );
	const int green = std::min( ( 510 * clamped_health ) / 100, 255 );

	render::fill_rect( box.x - 6, box.y - 1, 4, box.h + 2, color_t( 0, 0, 0, 180 ) );
	render::fill_rect( box.x - 5, box.y + ( box.h - bar_size ), 2, bar_size, color_t( red, green, 0, 210 ) );

	// don't use clamped health here, because if their 
	// health is larger than 100 we want to draw the health amt 
	if( player->health( ) <= 90 || player->health( ) > 100 ) {
		render::text( box.x - 5 - ( ( render::measure_text( e_font::ESP_SMALL_TEXT, fmt::format( STR_ENC( "{}" ), player->health( ) ) ).x / 2 ) ),
			box.y + ( box.h - bar_size ) - 7, e_font::ESP_SMALL_TEXT, color_t( 255, 255, 255, 210 ), fmt::format( STR_ENC( "{}" ), player->health( ) ) );
	}
}

void features::visuals::draw_armor( const box_t& box, c_player* player ) {
	if( !config::get<bool>( FNV_CT( "visuals.enemy.armor" ) ) ) {
		return;
	}

	const float box_multiplier = static_cast< float >( player->armor( ) ) / 100;
	const int box_width = std::clamp<int>( box.w * box_multiplier, 0, box.w );

	render::fill_rect( box.x - 1, box.y + box.h + m_bottom_offset.at( player->entity_index( ) ) + 2, box.w + 2, 4, color_t( 0, 0, 0, 180 ) );
	render::fill_rect( box.x, box.y + box.h + m_bottom_offset.at( player->entity_index( ) ) + 3, box_width, 2, color_t( 255, 255, 255, 210 ) );

	if( player->armor( ) < 90 ) {
		render::text( box.x + box_width - ( render::measure_text( e_font::ESP_SMALL_TEXT, fmt::format( STR_ENC( "{}" ), player->armor( ) ) ).x / 2 ),
			( box.y + box.h + m_bottom_offset.at( player->entity_index( ) ) ), e_font::ESP_SMALL_TEXT, color_t( 255, 255, 255, 210 ),
			fmt::format( STR_ENC( "{}" ), player->armor( ) ) );
	}

	m_bottom_offset.at( player->entity_index( ) ) += 6;
}

void features::visuals::draw_ammo( const box_t& box, c_player* player ) {
	if( !config::get<bool>( FNV_CT( "visuals.enemy.ammo" ) ) ) {
		return;
	}

	const auto weapon = player->active_weapon_handle( ).get( ) ? player->active_weapon_handle( ).get( )->as_weapon( ) : nullptr;
	if( !weapon ) {
		return;
	}

	const auto weapon_data = interfaces::weapon_system->get_weapon_info( weapon->item_definition_index( ) );
	if( !weapon_data ) {
		return;
	}

	// don't render ammo bar on nades, c4, zeus, knifes, etc.
	if( weapon_data->max_clip_ammo > 1 ) {
		float box_multiplier = static_cast< float >( weapon->ammo1( ) ) / weapon_data->max_clip_ammo;

		const auto& reload_layer = player->anim_overlay( ).Element( 1 );
		if( player->reloading( ) ) {
			box_multiplier = reload_layer.m_cycle;
		}

		const int box_width = std::clamp<int>( box.w * box_multiplier, 0, box.w );

		render::fill_rect( box.x - 1, box.y + box.h + m_bottom_offset.at( player->entity_index( ) ) + 1, box.w + 2, 4, color_t( 0, 0, 0, 180 ) );
		render::fill_rect( box.x, box.y + box.h + m_bottom_offset.at( player->entity_index( ) ) + 2, box_width, 2, color_t( 0, 150, 255, 210 ) );

		if( weapon->ammo1( ) > 0 && weapon->ammo1( ) < weapon_data->max_clip_ammo ) {
			render::text( box.x + box_width - ( render::measure_text( e_font::ESP_SMALL_TEXT, fmt::format( STR_ENC( "{}" ), weapon->ammo1( ) ) ).x / 2 ),
				( box.y + box.h + m_bottom_offset.at( player->entity_index( ) ) ) - 1, e_font::ESP_SMALL_TEXT, color_t( 255, 255, 255, 210 ),
				fmt::format( STR_ENC( "{}" ), weapon->ammo1( ) ) );
		}

		m_bottom_offset.at( player->entity_index( ) ) += 6;
	}
}

void features::visuals::draw_weapon( const box_t& box, c_player* player ) {
	if( !config::get<bool>( FNV_CT( "visuals.enemy.weapon_text" ) ) ) {
		return;
	}

	const auto weapon = player->active_weapon_handle( ).get( ) ? player->active_weapon_handle( ).get( )->as_weapon( ) : nullptr;
	if( !weapon ) {
		return;
	}

	const auto weapon_data = interfaces::weapon_system->get_weapon_info( weapon->item_definition_index( ) );
	if( !weapon_data ) {
		return;
	}

	// forgive me
	std::wstring localized = interfaces::localize->find( weapon_data->hud_name );
	std::string name( localized.begin( ), localized.end( ) );
	std::transform( name.begin( ), name.end( ), name.begin( ), ::toupper );

	const auto text_size = render::measure_text( e_font::ESP_SMALL_TEXT, name.data( ) );
	render::text( box.x + ( box.w / 2 ) - ( text_size.x / 2 ), box.y + box.h + m_bottom_offset.at( player->entity_index( ) ), e_font::ESP_SMALL_TEXT,
		color_t( 255, 255, 255, 210 ), name.data( ) );

	m_bottom_offset.at( player->entity_index( ) ) += text_size.y;
}

void features::visuals::draw_flags( const box_t& box, c_player* player ) {
	if( !config::get<bool>( FNV_CT( "visuals.enemy.flags" ) ) ) {
		return;
	}

	std::vector<std::pair<std::string, color_t>> flags;
	if( player->reloading( ) ) {
		flags.push_back( { STR_ENC( "R" ), color_t( 255, 0, 0 ) } );
	}

	if( player->is_scoped( ) ) {
		flags.push_back( { STR_ENC( "ZOOM" ), color_t( 0, 150, 255 ) } );
	}

	if( player->armor( ) ) {
		if( player->has_helmet( ) ) {
			flags.push_back( { STR_ENC( "HK" ), color_t( 255, 255, 255 ) } );
		}
		else {
			flags.push_back( { STR_ENC( "K" ), color_t( 255, 255, 255 ) } );
		}
	}

	if( flags.empty( ) ) {
		return;
	}

	for( int i = 0; i < flags.size( ); ++i ) {
		const auto str = flags[ i ].first;
		const auto str_size = render::measure_text( e_font::ESP_SMALL_TEXT, str.data( ) ).y;

		render::text( box.x + box.w + 2, box.y - 1 + ( ( str_size )*i ), e_font::ESP_SMALL_TEXT, flags[ i ].second.adjust_alpha( 210 ), str.data( ) );
	}
}

void features::visuals::draw_skeleton( c_player* player ) {
	if( !config::get<bool>( FNV_CT( "visuals.enemy.skeleton" ) ) ) {
		return;
	}

	// todo
}

void features::visuals::draw_head_dot( c_player* player ) {
	if( !config::get<bool>( FNV_CT( "visuals.enemy.head_dot" ) ) ) {
		return;
	}

	// todo
}

void features::visuals::on_paint( ) {
	for( int i = 1; i <= interfaces::entity_list->get_highest_entity_index( ); ++i ) {
		const auto entity = ( c_entity* )interfaces::entity_list->get_by_index( i );

		if( !is_valid_entity( entity ) ) {
			continue;
		}

		box_t box;
		if( !setup_box( entity, box ) ) {
			continue;
		}

		// player visuals
		if( entity->is_player( ) && config::get<bool>( FNV_CT( "visuals.enemy.enabled" ) ) ) {
			const auto player = entity->as_player( );

			m_bottom_offset.at( i ) = 0;

			const float alpha_modifier = interfaces::vgui_surface->get_alpha_multiplier( );
			interfaces::vgui_surface->set_alpha_multiplier( m_alpha.at( i ) );

			draw_box( box, player );
			draw_name( box, player );
			draw_health( box, player );
			draw_armor( box, player );
			draw_ammo( box, player );
			draw_weapon( box, player );
			draw_flags( box, player );
			draw_skeleton( player );
			draw_head_dot( player );

			interfaces::vgui_surface->set_alpha_multiplier( alpha_modifier );
		}
		// other entities (dropped weapons, nades etc..)
		else {

		}
	}

}