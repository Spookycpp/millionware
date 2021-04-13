#include <algorithm>
#include <cstdint>
#include <unordered_map>

#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"
#include "../../engine/math/math.h"
#include "../../engine/render/render.h"
#include "esp.h"

struct entity_esp_t
{
	int predicted_money;
	float last_server_update;
	vector_t position;
};

struct bounding_box_t
{
	float x;
	float y;
	float width;
	float height;
};

static std::unordered_map<int, entity_esp_t> entity_esp_info;

static entity_esp_t &get_entity_info(int index)
{
	if (entity_esp_info.find(index) == entity_esp_info.end())
		entity_esp_info.insert({ index, {} });

	return entity_esp_info[index];
}

static bool get_bounding_box(const entity_esp_t &entity_info, c_entity *entity, bounding_box_t &out_box)
{
	const auto collideable = entity->get_collideable();
	const auto mins = collideable->get_mins();
	const auto maxs = collideable->get_maxs();

	vector_t points[8] =
	{
		{ mins.x, mins.y, mins.z },
		{ mins.x, maxs.y, mins.z },
		{ maxs.x, maxs.y, mins.z },
		{ maxs.x, mins.y, mins.z },
		{ maxs.x, maxs.y, maxs.z },
		{ mins.x, maxs.y, maxs.z },
		{ mins.x, mins.y, maxs.z },
		{ maxs.x, mins.y, maxs.z }
	};

	std::transform(std::begin(points), std::end(points), std::begin(points), [entity] (const auto point)
	{
		return math::vector_transform(point, entity->get_transformation_matrix());
	});

	point_t screen_pos[8];

	for (auto i = 0; i < 8; i++)
	{
		if (!math::world_to_screen(points[i], screen_pos[i]))
			return false;
	}

	auto left = FLT_MAX;
	auto top = FLT_MIN;
	auto right = FLT_MIN;
	auto bottom = FLT_MAX;

	for (auto i = 0; i < 8; i++)
	{
		left = std::min(left, screen_pos[i].x);
		top = std::max(top, screen_pos[i].y);
		right = std::max(right, screen_pos[i].x);
		bottom = std::min(bottom, screen_pos[i].y);
	}

	out_box = { left, bottom, right - left, (top - bottom) + 4.0f };

	return true;
}

void features::visuals::esp::frame()
{
	if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
		return;

	for (auto i = 0; i < interfaces::entity_list->get_highest_ent_index(); i++)
	{
		const auto entity = interfaces::entity_list->get_entity(i);

		if (!entity || !entity->is_player())
			continue;

		auto &entity_info = get_entity_info(i);

		if (!entity->get_networkable()->is_dormant())
		{
			entity_info.last_server_update = interfaces::global_vars->current_time;
			entity_info.position = entity->get_vec_origin();
		}

		const auto player = (c_player *) entity;

		if (player->get_life_state() != LIFE_STATE_ALIVE || player->get_health() <= 0)
			continue;

		const auto time_since_last_update = interfaces::global_vars->current_time - entity_info.last_server_update;
		
		if (time_since_last_update > 3.0f)
			continue;

		bounding_box_t entity_box;

		if (!get_bounding_box(entity_info, entity, entity_box))
			continue;

		const auto box_position = point_t { entity_box.x, entity_box.y };
		const auto box_size = point_t { entity_box.width, entity_box.height };

		render::draw_rect(box_position - 1.0f, box_size + 2.0f, { 0, 0, 0, 100 }, 2.0f);
		render::draw_rect(box_position + 1.0f, box_size - 2.0f, { 0, 0, 0, 100 }, 2.0f);
		render::draw_rect(box_position, box_size, { 255, 255, 255, 200 }, 2.0f);
	}
}

void features::visuals::esp::update_dormant_pos(int index, const vector_t &position)
{
	auto &entity_info = get_entity_info(index);

	entity_info.last_server_update = interfaces::global_vars->current_time;
	entity_info.position = position;
}

void features::visuals::esp::update_predicted_money(int index, int money)
{
	auto &entity_info = get_entity_info(index);

	entity_info.predicted_money = money;
}
