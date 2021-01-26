#include <array>
#include <tuple>
#include "fmt/format.h"

#include "visuals.hpp"

#include "../../core/config.hpp"
#include "../../core/cheat.hpp"
#include "../../core/interfaces.hpp"

#include "../../utils/math/math.hpp"
#include "../../utils/render/render.hpp"

constexpr auto health_bar_margin = 3;
constexpr auto health_bar_size = 2;

point_t rotate_point(const point_t point, const point_t center, const float angle)
{
    const auto xf = static_cast<float>(point.x - center.x);
    const auto yf = static_cast<float>(point.y - center.y);

    const auto rad = math::deg_to_rad(angle);
    const auto cos = std::cos(rad);
    const auto sin = std::sin(rad);

    return point_t(
        static_cast<int32_t>(xf * cos - yf * sin) + center.x,
        static_cast<int32_t>(xf * sin + yf * cos) + center.y
    );
}

std::tuple<point_t, point_t, point_t> rotate_triangle(const point_t point1, const point_t point2, const point_t point3, const float angle)
{
    const auto center = (point1 + point2 + point3) / 3;

    return std::make_tuple(rotate_point(point1, center, angle), rotate_point(point2, center, angle), rotate_point(point3, center, angle));
}

std::optional<features::visuals::bounding_box_t> features::visuals::calculate_box(c_entity* entity)
{
    const auto collideable = entity->get_collideable();

    if (!collideable)
        return std::nullopt;

    const auto& matrix = entity->entity_to_world_matrix();
    const auto& mins = collideable->get_mins();
    const auto& maxs = collideable->get_maxs();

    const vector3_t points[8] = {
      vector3_t(mins.x, mins.y, mins.z),
      vector3_t(mins.x, maxs.y, mins.z),
      vector3_t(maxs.x, maxs.y, mins.z),
      vector3_t(maxs.x, mins.y, mins.z),
      vector3_t(maxs.x, maxs.y, maxs.z),
      vector3_t(mins.x, maxs.y, maxs.z),
      vector3_t(mins.x, mins.y, maxs.z),
      vector3_t(maxs.x, mins.y, maxs.z),
    };

    const vector3_t points_transformed[8] = {
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

    for (auto i = 0; i < 8; i++)
    {
        const auto screen_pos = interfaces::debug_overlay->screen_position(points_transformed[i]);

        if (screen_pos.x == -1 && screen_pos.y == -1)
            return std::nullopt;

        screen_points[i] = screen_pos;
    }

    auto left = screen_points[0].x;
    auto top = screen_points[0].y;
    auto right = screen_points[0].x;
    auto bottom = screen_points[0].y;

    for (auto i = 1; i < 8; i++)
    {
        left = std::min(left, screen_points[i].x);
        top = std::max(top, screen_points[i].y);
        right = std::max(right, screen_points[i].x);
        bottom = std::min(bottom, screen_points[i].y);
    }

    return bounding_box_t(left, bottom, right, top);
}

void features::visuals::draw_esp_box(const bounding_box_t& box, const int32_t box_type, const color_t color)
{
    const auto box_size = box.max - box.min;
    const auto outline_alpha = static_cast<int32_t>(150.0f * (static_cast<float>(color.a) / 255.0f));
    const auto outline_color = color_t(0, outline_alpha);

    switch (box_type)
    {
    case 0: // regular
        render::rect(box.min - 1, box_size + 2, outline_color);
        render::rect(box.min + 1, box_size - 2, outline_color);
        render::rect(box.min, box_size, color);

        break;

    case 1: // corners
    {
        const auto longer_axis = std::max(box_size.x, box_size.y);
        const auto shorter_axis = std::min(box_size.x, box_size.y);

        auto corner_size = static_cast<int32_t>(static_cast<float>(longer_axis) * 0.2f);

        if (corner_size * 2 >= shorter_axis - 8)
            corner_size = (shorter_axis - 8) / 2;

        const auto top_left = box.min;
        const auto top_right = point_t(box.max.x, box.min.y);
        const auto bottom_left = point_t(box.min.x, box.max.y);
        const auto bottom_right = box.max;

        // top left outline
        render::line(top_left.x - 1, top_left.y - 1, top_left.x + corner_size + 1, top_left.y - 1, outline_color);
        render::line(top_left.x - 1, top_left.y, top_left.x + corner_size + 1, top_left.y, outline_color);
        render::line(top_left.x - 1, top_left.y + 1, top_left.x + corner_size + 1, top_left.y + 1, outline_color);
        render::line(top_left.x - 1, top_left.y + 2, top_left.x - 1, top_left.y + corner_size + 1, outline_color);
        render::line(top_left.x, top_left.y + 2, top_left.x, top_left.y + corner_size + 1, outline_color);
        render::line(top_left.x + 1, top_left.y + 2, top_left.x + 1, top_left.y + corner_size + 1, outline_color);

        // top right outline
        render::line(top_right.x + 1, top_right.y - 1, top_right.x - corner_size - 1, top_right.y - 1, outline_color);
        render::line(top_right.x + 1, top_right.y, top_right.x - corner_size - 1, top_right.y, outline_color);
        render::line(top_right.x + 1, top_right.y + 1, top_right.x - corner_size - 1, top_right.y + 1, outline_color);
        render::line(top_right.x - 1, top_right.y + 2, top_right.x - 1, top_right.y + corner_size + 1, outline_color);
        render::line(top_right.x, top_right.y + 2, top_right.x, top_right.y + corner_size + 1, outline_color);
        render::line(top_right.x + 1, top_right.y + 2, top_right.x + 1, top_right.y + corner_size + 1, outline_color);

        // bottom left outline
        render::line(bottom_left.x - 1, bottom_left.y + 1, bottom_left.x + corner_size + 1, bottom_left.y + 1, outline_color);
        render::line(bottom_left.x - 1, bottom_left.y, bottom_left.x + corner_size + 1, bottom_left.y, outline_color);
        render::line(bottom_left.x - 1, bottom_left.y - 1, bottom_left.x + corner_size + 1, bottom_left.y - 1, outline_color);
        render::line(bottom_left.x - 1, bottom_left.y - 2, bottom_left.x - 1, bottom_left.y - corner_size - 1, outline_color);
        render::line(bottom_left.x, bottom_left.y - 2, bottom_left.x, bottom_left.y - corner_size - 1, outline_color);
        render::line(bottom_left.x + 1, bottom_left.y - 2, bottom_left.x + 1, bottom_left.y - corner_size - 1, outline_color);

        // bottom right outline
        render::line(bottom_right.x + 1, bottom_right.y + 1, bottom_right.x - corner_size - 1, bottom_left.y + 1, outline_color);
        render::line(bottom_right.x + 1, bottom_right.y, bottom_right.x - corner_size - 1, bottom_left.y, outline_color);
        render::line(bottom_right.x + 1, bottom_right.y - 1, bottom_right.x - corner_size - 1, bottom_left.y - 1, outline_color);
        render::line(bottom_right.x + 1, bottom_right.y - 2, bottom_right.x + 1, bottom_left.y - corner_size - 1, outline_color);
        render::line(bottom_right.x, bottom_right.y - 2, bottom_right.x, bottom_left.y - corner_size - 1, outline_color);
        render::line(bottom_right.x - 1, bottom_right.y - 2, bottom_right.x - 1, bottom_left.y - corner_size - 1, outline_color);

        // top left
        render::line(top_left.x, top_left.y, top_left.x + corner_size, top_left.y, color);
        render::line(top_left.x, top_left.y + 1, top_left.x, top_left.y + corner_size, color);

        // top right
        render::line(top_right.x, top_right.y, top_right.x - corner_size, top_right.y, color);
        render::line(top_right.x, top_right.y + 1, top_right.x, top_right.y + corner_size, color);

        // bottom left
        render::line(bottom_left.x, bottom_left.y, bottom_left.x + corner_size, bottom_left.y, color);
        render::line(bottom_left.x, bottom_left.y - 1, bottom_left.x, bottom_left.y - corner_size, color);

        // bottom right
        render::line(bottom_right.x, bottom_right.y, bottom_right.x - corner_size, bottom_left.y, color);
        render::line(bottom_right.x, bottom_right.y - 1, bottom_right.x, bottom_left.y - corner_size, color);

        break;
    }

    default:
        throw std::runtime_error(fmt::format("invalid value for esp box type: {}", box_type));
    }
}

void features::visuals::draw_esp_name(const bounding_box_t& box, c_entity* entity, color_t color) {

    const auto box_size = box.max - box.min;

    if (entity->is_weapon())
    {
        const auto weapon = reinterpret_cast<c_weapon*>(entity);
        const auto weapon_info = interfaces::weapon_system->get_weapon_info(weapon->item_definition_index());
        const auto name_size = render::measure_text(e_font::UI_11, weapon_info->weapon_name);

        render::text(box.min.x + box_size.x / 2 - name_size.x / 2, box.min.y - name_size.y,
            e_font::UI_11, color, weapon_info->weapon_name);
    }
    else
    {
        player_info_t player_info;

        if (!interfaces::engine_client->get_player_info(entity->networkable()->index(), player_info))
            return;

        const auto name_size = render::measure_text(e_font::UI_11, player_info.name);

        render::text(box.min.x + box_size.x / 2 - name_size.x / 2, box.min.y - name_size.y,
            e_font::UI_11, color, player_info.name);
    }
}

void features::visuals::draw_player_esp(c_player* local_player, c_player* player)
{

    const auto entity_box_opt = calculate_box(player);
    const auto is_friendly = player->team_num() == local_player->team_num();
    const auto is_local = player == local_player;

    if (entity_box_opt.has_value()) {
        const auto& entity_box = entity_box_opt.value();

        if (config::get<bool>(FNV_CT("visuals.local.bounding_box")) && is_local) {
            draw_esp_box(entity_box, 0, { 255, 255, 255, 255 });
        }
        else if (config::get<bool>(FNV_CT("visuals.enemy.bounding_box")) && !is_friendly) {
            draw_esp_box(entity_box, 0, { 255, 0, 0, 255 });
        }
        else if (config::get<bool>(FNV_CT("visuals.team.bounding_box")) && is_friendly) {
            draw_esp_box(entity_box, 0, { 0, 255, 0, 255 });
        }

        if (config::get<bool>(FNV_CT("visuals.local.name")) && is_local) {
            draw_esp_name(entity_box, player, { 255, 255, 255, 255 });
        }        
        else if (config::get<bool>(FNV_CT("visuals.enemy.name")) && !is_friendly) {
            draw_esp_name(entity_box, player, { 255, 255, 255, 255 });
        }        
        else if (config::get<bool>(FNV_CT("visuals.team.name")) && is_friendly) {
            draw_esp_name(entity_box, player, { 255, 255, 255, 255 });
        }
    }
}

void features::visuals::render() {
    if (!interfaces::engine_client->is_in_game())
        return;

    if (cheat::local_player == nullptr)
        return;

    const auto self_enabled = config::get<bool>(FNV_CT("visuals.self.enabled"));
    const auto friendlies_enabled = config::get<bool>(FNV_CT("visuals.friendly.enabled"));
    const auto enemies_enabled = config::get<bool>(FNV_CT("visuals.enemy.enabled"));

    if (!self_enabled && !friendlies_enabled && !enemies_enabled)
        return;

    for (auto i = 1; i < interfaces::entity_list->get_highest_entity_index(); i++)
    {
        const auto entity = interfaces::entity_list->get_by_index(i);

        if (entity == nullptr)
            continue;

        if (entity->networkable()->is_dormant())
            continue;

        if (entity->is_player())
        {
            const auto player = reinterpret_cast<c_player*>(entity);

            if (player->health() <= 0)
                continue;

            draw_player_esp(cheat::local_player, player);
        }
    }
}