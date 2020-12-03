#include "../../core/interfaces.hpp"
#include "../../utils/render.hpp"
#include "esp.hpp"

static std::vector<std::optional<screen_bounding_box_t>> entity_bounding_box_lookup;

std::optional<screen_bounding_box_t> features::visuals::esp::get_entity_box(int index) {
	const auto entity = interfaces::entity_list->get_by_index(index);

	if (entity != nullptr) {
		// @todo: calculate the bounding box
	}

	return entity_bounding_box_lookup[index];
}

void features::visuals::esp::handle_player_esp(c_player* player) {
	const auto box = get_entity_box(player->networkable->index());
	const auto box_position = box->min;
	const auto box_size = box->max - box->min;

	render::rect(box_position, box_size, player->team_num() == TEAM_NUM_TERRORISTS ? color_t(255, 0, 0) : color_t(0, 0, 255));
}

void features::visuals::esp::handle_weapon_esp(c_weapon* weapon) {
	const auto box = get_entity_box(weapon->networkable->index());
	const auto box_position = box->min;
	const auto box_size = box->max - box->min;

	render::rect(box_position, box_size, color_t(255));
}

void features::visuals::esp::paint() {
	// make sure we have enough space to keep all entities
	entity_bounding_box_lookup.resize(interfaces::entity_list->get_highest_entity_index() + 1);

	// reset the stored bounding boxes and calculate new ones
	for (auto i = 0u; i < entity_bounding_box_lookup.size(); i++) {
		// reset the stored bounding box
		entity_bounding_box_lookup[i] = std::nullopt;

		const auto entity = interfaces::entity_list->get_by_index(i);

		if (entity == nullptr)
			continue;

		// @todo: see if the entity should be drawn

		// calculate the bounding box
		const auto box = get_entity_box(i);

		// check if the box was calculated properly
		if (!box.has_value())
			continue;

		// draw the esp
		if (entity->is_player()) {
			handle_player_esp(entity->as_player());
		}
		else if (entity->is_weapon()) {
			handle_weapon_esp(entity->as_weapon());
		}
	}
}
