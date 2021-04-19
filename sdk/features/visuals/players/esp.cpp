#include <array>
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <cctype>

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"
#include "../../engine/input/input.h"
#include "../../engine/math/math.h"
#include "../../engine/render/render.h"
#include "../../engine/security/xorstr.h"
#include "esp.h"
#include "../../engine/logging/logging.h"

// cringe but ye
static std::array<float, 65> m_bottom_offset;

struct entity_esp_t {
	int predicted_money;
	float last_server_update;
	vector_t position;
};

static std::unordered_map<int, entity_esp_t> entity_esp_info;

static entity_esp_t& get_entity_info(int index) {
	if (entity_esp_info.find(index) == entity_esp_info.end())
		entity_esp_info.insert({ index, {} });

	return entity_esp_info[index];
}

static bool get_bounding_box(const entity_esp_t& entity_info, c_entity* entity, features::visuals::esp::bounding_box_t& out_box) {
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

	std::transform(std::begin(points), std::end(points), std::begin(points), [entity](const auto point) {
		return math::vector_transform(point, entity->get_transformation_matrix());
	});

	point_t screen_pos[8];

	for (auto i = 0; i < 8; i++) {
		if (!math::world_to_screen(points[i], screen_pos[i]))
			return false;
	}

	auto left = FLT_MAX;
	auto top = FLT_MIN;
	auto right = FLT_MIN;
	auto bottom = FLT_MAX;

	for (auto i = 0; i < 8; i++) {
		left = std::min(left, screen_pos[i].x);
		top = std::max(top, screen_pos[i].y);
		right = std::max(right, screen_pos[i].x);
		bottom = std::min(bottom, screen_pos[i].y);
	}

	out_box = { left, bottom, right - left, (top - bottom) + 4.0f };

	return true;
}

void features::visuals::esp::frame() {
	if (!interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
		return;

	for (auto i = 0; i < interfaces::entity_list->get_highest_ent_index(); i++) {
		const auto entity = interfaces::entity_list->get_entity(i);

		if (!entity || !entity->is_player())
			continue;

		auto& entity_info = get_entity_info(i);

		if (!entity->get_networkable()->is_dormant()) {
			entity_info.last_server_update = interfaces::global_vars->current_time;
			entity_info.position = entity->get_vec_origin();
		}

		const auto player = (c_player*)entity;

		if (player->get_life_state() != LIFE_STATE_ALIVE || player->get_health() <= 0)
			continue;

		if (player->get_team_num() == cheat::local_player->get_team_num())
			continue;

		const auto time_since_last_update = interfaces::global_vars->current_time - entity_info.last_server_update;

		if (time_since_last_update > 3.0f)
			continue;

		bounding_box_t entity_box;

		if (!get_bounding_box(entity_info, entity, entity_box))
			continue;

		m_bottom_offset[i] = 0.f;

		draw_box(entity_box, player);
		draw_name(entity_box, player);
		draw_health(entity_box, player);
		draw_armor(entity_box, player);
		draw_ammo(entity_box, player);
		draw_weapon(entity_box, player);
		draw_flags(entity_box, player);
	}
}

void features::visuals::esp::draw_box(const bounding_box_t& entity_box, c_player* player) {

	if (!settings.visuals.player.bounding_box)
		return;

	const auto box_position = point_t{ entity_box.x, entity_box.y };
	const auto box_size = point_t{ entity_box.width, entity_box.height };

	render::draw_rect(box_position - 1.0f, box_size + 2.0f, { 0, 0, 0, 100 });
	render::draw_rect(box_position + 1.0f, box_size - 2.0f, { 0, 0, 0, 100 });
	render::draw_rect(box_position, box_size, { 255, 255, 255, 255 });
}

void features::visuals::esp::draw_name(const bounding_box_t& entity_box, c_player* player) {

	if (!settings.visuals.player.player_name)
		return;

	player_info_t info;

	if (!interfaces::engine_client->get_player_info(player->get_networkable()->index(), info))
		return;

	const auto text_size = render::measure_text(info.name, FONT_VERDANA_12);
	render::draw_text({ entity_box.x + (entity_box.width / 2) - (text_size.x / 2), entity_box.y - text_size.y }, { 255, 255, 255, 255 }, info.name, FONT_VERDANA_12);
}

void features::visuals::esp::draw_health(const bounding_box_t& entity_box, c_player* player) {

	if (!settings.visuals.player.health)
		return;

	const auto clamped_health = std::clamp(player->get_health(), 0, 100);
	const auto bar_size = std::clamp((clamped_health * entity_box.height) / 100.0f, 0.0f, entity_box.height);
	const auto red = std::min((510 * (100 - clamped_health)) / 100, 255);
	const auto green = std::min((510 * clamped_health) / 100, 255);

	render::fill_rect({ entity_box.x - 6.0f, entity_box.y - 1.0f }, { 4.0f, entity_box.height + 2.0f }, { 0, 0, 0, 180 });
	render::fill_rect({ entity_box.x - 5.0f, entity_box.y + entity_box.height - bar_size }, { 2.0f, bar_size }, { red, green, 0, 210 });

	// don't use clamped health here, because if their health
	// is larger than 100 we want to draw the health amount
	if (player->get_health() <= 90 || player->get_health() > 100) {
		char health_text_buffer[8];

		sprintf_s(health_text_buffer, XORSTR("%d"), player->get_health());

		const auto health_text_size = render::measure_text(health_text_buffer, FONT_SMALL_TEXT);

		render::draw_text({ entity_box.x - 5.0f - health_text_size.x * 0.5f, entity_box.y + (entity_box.height - bar_size) - 7.0f }, { 255, 255, 255, 210 }, health_text_buffer, FONT_SMALL_TEXT);
	}
}

void features::visuals::esp::draw_armor(const bounding_box_t& entity_box, c_player* player) {
	if (!settings.visuals.player.armor)
		return;

	const auto player_index = player->get_networkable()->index();
	const auto box_multiplier = player->get_armor() / 100.0f;
	const auto box_width = std::clamp(entity_box.width * box_multiplier, 0.0f, entity_box.width);

	render::fill_rect({ entity_box.x - 1.0f, entity_box.y + entity_box.height + m_bottom_offset[player_index] + 2.0f }, { entity_box.width + 2.0f, 4.0f }, { 0, 0, 0, 180 });
	render::fill_rect({ entity_box.x, entity_box.y + entity_box.height + m_bottom_offset[player_index] + 3.0f }, { box_width, 2.0f }, { 255, 255, 255, 210 });

	if (player->get_armor() < 90) {
		char armor_text_buffer[8];

		sprintf_s(armor_text_buffer, XORSTR("%d"), player->get_armor());

		const auto armor_text_size = render::measure_text(armor_text_buffer, FONT_SMALL_TEXT);

		render::draw_text({ entity_box.x + box_width - armor_text_size.x * 0.5f, entity_box.y + entity_box.height + m_bottom_offset[player_index] }, { 255, 255, 255, 210 }, armor_text_buffer, FONT_SMALL_TEXT);
	}

	m_bottom_offset[player_index] += 6.0f;
}

void features::visuals::esp::draw_ammo(const bounding_box_t& entity_box, c_player* player) {
	if (!settings.visuals.player.ammo)
		return;

	const auto weapon = (c_weapon*)player->get_active_weapon_handle().get();

	if (!weapon)
		return;

	const auto weapon_data = interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index());

	if (!weapon_data)
		return;

	if (weapon_data->max_clip_ammo < 1)
		return;

	const auto player_index = player->get_networkable()->index();
	const auto reload_layer = &player->animation_overlay().Element(1);
	const auto box_multiplier = player->is_reloading() ? reload_layer->cycle : (weapon->get_ammo1() / (float)weapon_data->max_clip_ammo);
	const auto box_width = std::clamp(entity_box.width * box_multiplier, 0.0f, entity_box.width);

	render::fill_rect({ entity_box.x - 1.0f, entity_box.y + entity_box.height + m_bottom_offset[player_index] + 1.0f }, { entity_box.width + 2.0f, 4.0f }, { 0, 0, 0, 180 });
	render::fill_rect({ entity_box.x, entity_box.y + entity_box.height + m_bottom_offset[player_index] + 2.0f }, { box_width, 2.0f }, { 0, 150, 255, 210 });

	if (weapon->get_ammo1() > 0 && weapon->get_ammo1() < weapon_data->max_clip_ammo && !player->is_reloading()) {
		char ammo_text_buffer[8];

		sprintf_s(ammo_text_buffer, XORSTR("%d"), weapon->get_ammo1());

		const auto ammo_text_size = render::measure_text(ammo_text_buffer, FONT_SMALL_TEXT);

		render::draw_text({ entity_box.x + box_width - ammo_text_size.x * 0.5f, entity_box.y + entity_box.height + m_bottom_offset[player_index] - 1.0f }, { 255, 255, 255, 210 }, ammo_text_buffer, FONT_SMALL_TEXT);
	}

	m_bottom_offset[player_index] += 6.0f;
}

void features::visuals::esp::draw_weapon(const bounding_box_t& entity_box, c_player* player) {

	if (!settings.visuals.player.weapon_text)
		return;

	const auto weapon = (c_weapon*)player->get_active_weapon_handle().get();

	if (!weapon)
		return;

	const auto weapon_data = interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index());

	if (!weapon_data)
		return;

	const auto localized_name = interfaces::localize->find(weapon_data->hud_name);

	char localized_name_buffer[32];

	memset(localized_name_buffer, 0, sizeof(localized_name_buffer));

	const auto localized_name_length = WideCharToMultiByte(CP_UTF8, 0, localized_name, wcslen(localized_name),
		localized_name_buffer, sizeof(localized_name_buffer), nullptr, nullptr);

	for (auto i = 0; i < localized_name_length; i++)
		localized_name_buffer[i] = std::toupper(localized_name_buffer[i]);

	const auto localized_name_size = render::measure_text(localized_name_buffer, FONT_SMALL_TEXT);

	render::draw_text(
		{ entity_box.x + entity_box.width * 0.5f - localized_name_size.x * 0.5f, entity_box.y + entity_box.height + m_bottom_offset[player->get_networkable()->index()] },
		{ 255, 255, 255, 210 }, localized_name_buffer, FONT_SMALL_TEXT);

	m_bottom_offset[player->get_networkable()->index()] += localized_name_size.y;
}

void features::visuals::esp::draw_flags(const bounding_box_t& entity_box, c_player* player) {

	if (!settings.visuals.player.flags)
		return;

	auto draw_flag = [flag_offset = 0.0f, &entity_box, player](const char* flag_text, const color_t& flag_color) mutable
	{
		const auto flag_text_size = render::measure_text(flag_text, FONT_SMALL_TEXT);

		render::draw_text({ entity_box.x + entity_box.width + 2.0f, entity_box.y - 1.0f + flag_offset }, flag_color, flag_text, FONT_SMALL_TEXT);

		flag_offset += flag_text_size.y;
	};

	if (player->get_armor() > 0)
		draw_flag(player->get_has_helmet() ? XORSTR("HK") : XORSTR("K"), { 255, 255, 255 });

	if (player->get_is_scoped())
		draw_flag(XORSTR("SCOPED"), { 0, 150, 255 });

	if (player->is_reloading())
		draw_flag(XORSTR("RELOADING"), { 2, 106, 198 });
}

void features::visuals::esp::update_dormant_pos(int index, const vector_t& position) {
	auto& entity_info = get_entity_info(index);

	entity_info.last_server_update = interfaces::global_vars->current_time;
	entity_info.position = position;
}

void features::visuals::esp::update_predicted_money(int index, int money) {
	auto& entity_info = get_entity_info(index);

	entity_info.predicted_money = money;
}
