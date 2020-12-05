#include "../../core/cheat.hpp"
#include "../../core/interfaces.hpp"
#include "../../utils/math.hpp"
#include "../../utils/render.hpp"
#include "esp.hpp"

static std::vector<std::optional<screen_bounding_box_t>> entity_bounding_box_lookup;

std::optional<screen_bounding_box_t> features::visuals::esp::get_entity_box(int index) {
	const auto entity = interfaces::entity_list->get_by_index(index);

	if (entity != nullptr && !entity_bounding_box_lookup[index].has_value()) {
		const auto collideable = entity->get_collideable();

		if (!collideable)
			return std::nullopt;

		const auto& matrix = entity->entity_to_world_matrix();
		const auto& mins = collideable->get_mins();
		const auto& maxs = collideable->get_maxs();

		const vector3_t points[] = {
			vector3_t(mins.x, mins.y, mins.z),
			vector3_t(mins.x, maxs.y, mins.z),
			vector3_t(maxs.x, maxs.y, mins.z),
			vector3_t(maxs.x, mins.y, mins.z),
			vector3_t(maxs.x, maxs.y, maxs.z),
			vector3_t(mins.x, maxs.y, maxs.z),
			vector3_t(mins.x, mins.y, maxs.z),
			vector3_t(maxs.x, mins.y, maxs.z),
		};

		const vector3_t points_transformed[] = {
			math::transform_vector(points[0], matrix),
			math::transform_vector(points[1], matrix),
			math::transform_vector(points[2], matrix),
			math::transform_vector(points[3], matrix),
			math::transform_vector(points[4], matrix),
			math::transform_vector(points[5], matrix),
			math::transform_vector(points[6], matrix),
			math::transform_vector(points[7], matrix),
		};

		point_t screen_points[8] = {};

		for (auto i = 0; i < 8; i++) {
			const auto screen_pos = interfaces::debug_overlay->screen_position(points_transformed[i]);

			if (screen_pos.x == -1 && screen_pos.y == -1)
				return std::nullopt;

			screen_points[i] = screen_pos;
		}

		auto left = screen_points[0].x;
		auto top = screen_points[0].y;
		auto right = screen_points[0].x;
		auto bottom = screen_points[0].y;

		for (auto i = 1; i < 8; i++) {
			left = std::min(left, screen_points[i].x);
			top = std::min(top, screen_points[i].y);
			right = std::max(right, screen_points[i].x);
			bottom = std::max(bottom, screen_points[i].y);
		}

		entity_bounding_box_lookup[index] = { point_t(left, top), point_t(right, bottom) };
	}

	return entity_bounding_box_lookup[index];
}

void features::visuals::esp::handle_player_esp(const screen_bounding_box_t& box, c_player* player) {
	const auto box_position = box.min;
	const auto box_size = box.max - box.min;

	render::rect(box_position, box_size, player->team_num() == TEAM_NUM_TERRORISTS ? color_t(255, 0, 0) : color_t(0, 0, 255));
}

void features::visuals::esp::handle_weapon_esp(const screen_bounding_box_t& box, c_weapon* weapon) {
	const auto box_position = box.min;
	const auto box_size = box.max - box.min;

	render::rect(box_position, box_size, color_t(255));
}

void features::visuals::esp::paint() {
	// le epic in game check :^)
	if (cheat::local_player == nullptr)
		return;

	// make sure we have enough space to keep all entities
	entity_bounding_box_lookup.resize(interfaces::entity_list->get_highest_entity_index() + 1);

	// reset the stored bounding boxes and calculate new ones
	for (auto i = 0u; i < entity_bounding_box_lookup.size(); i++) {
		// reset the stored bounding box
		entity_bounding_box_lookup[i] = std::nullopt;

		const auto entity = interfaces::entity_list->get_by_index(i);

		// some sanity checks
		if (entity == nullptr)
			continue;

		if (entity->networkable()->is_dormant())
			continue;

		// check if the entity should be drawn
		if (entity->is_player()) {
			const auto player = entity->as_player();

			// filter out dead players
			if (player->life_state() != LIFE_STATE_ALIVE)
				continue;
		}
		else if (entity->is_weapon()) {
			const auto weapon = entity->as_weapon();

			// filter out non dropped weapons
			if (weapon->owner_handle() != INVALID_HANDLE)
				continue;
		}

		// calculate the bounding box
		const auto box = get_entity_box(i);

		// check if the box was calculated properly
		if (!box.has_value())
			continue;

		// handle the esp
		if (entity->is_player()) {
			handle_player_esp(box.value(), entity->as_player());
		}
		else if (entity->is_weapon()) {
			handle_weapon_esp(box.value(), entity->as_weapon());
		}
	}
}
