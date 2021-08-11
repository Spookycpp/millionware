#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <unordered_map>

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"
#include "../../../engine/input/input.h"
#include "../../../engine/logging/logging.h"
#include "../../../engine/math/math.h"
#include "../../../engine/render/render.h"
#include "../../../engine/render/render.h"
#include "../../../engine/security/xorstr.h"
#include "../world/world.h"
#include "esp.h"

using namespace features::visuals::esp;

// cringe but ye
static std::array<float, 65> m_bottom_offset;

static std::unordered_map<int, entity_esp_t> entity_esp_info;

static entity_esp_t &get_entity_info(int index) {
    if (entity_esp_info.find(index) == entity_esp_info.end())
        entity_esp_info.insert({ index, {} });

    return entity_esp_info[index];
}

static bool get_bounding_box(c_entity *entity, features::visuals::esp::bounding_box_t &out_box) {
    const auto collideable = entity->get_collideable();
    const auto mins = collideable->get_mins();
    const auto maxs = collideable->get_maxs();

    vector_t points[8] = { {mins.x, mins.y, mins.z}, {mins.x, maxs.y, mins.z}, {maxs.x, maxs.y, mins.z}, {maxs.x, mins.y, mins.z},
                          {maxs.x, maxs.y, maxs.z}, {mins.x, maxs.y, maxs.z}, {mins.x, mins.y, maxs.z}, {maxs.x, mins.y, maxs.z} };

    std::transform(std::begin(points), std::end(points), std::begin(points), [entity](const auto point) { return math::vector_transform(point, entity->get_transformation_matrix()); });

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

    out_box = { left, bottom, right - left, (top - bottom) };

    return true;
}

namespace features::visuals::esp {

    void frame() {

        if (!cheat::local_player || !interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected())
            return;

        for (auto i = 0; i < interfaces::entity_list->get_highest_ent_index(); i++) {
            const auto entity = interfaces::entity_list->get_entity(i);

            if (!entity)
                continue;

            // yandere dev moment
            if (entity->is_weapon() && !entity->get_networkable()->is_dormant())
                draw_dropped_weapon(entity);
            else if (entity->is_grenade() && !entity->get_networkable()->is_dormant())
                draw_thrown_utility(entity);
            else if (!entity->get_networkable()->is_dormant()) {
                draw_planted_bomb(entity);
                draw_defusal_kit(entity);
            }

            features::visuals::world::draw_world(entity);

            if (!entity->is_player())
                continue;

            auto &entity_info = get_entity_info(i);

            if (!entity->get_networkable()->is_dormant()) {
                entity_info.last_server_update = interfaces::global_vars->current_time;
                entity_info.position = entity->get_vec_origin();
            }

            const auto player = (c_player *)entity;

            if (player->get_life_state() != LIFE_STATE_ALIVE || player->get_health() <= 0)
                continue;

            if (player->get_team_num() == cheat::local_player->get_team_num())
                continue;

            const auto time_since_last_update = interfaces::global_vars->current_time - entity_info.last_server_update;

            if (time_since_last_update > 0.5f)
                continue;

            bounding_box_t entity_box;

            if (!get_bounding_box(entity, entity_box))
                continue;

            m_bottom_offset[i] = 0.f;

            if (settings.visuals.player.engine_radar)
                if (!player->get_is_spotted())
                    player->get_is_spotted() = true;

            const auto screen_size = render::get_screen_size();

            if (entity_box.x + entity_box.width - 1 < 0 || entity_box.x - 1 >= screen_size.x || entity_box.y + entity_box.height - 1 < 0 || entity_box.y - 1 >= screen_size.y)
                continue;

            draw_box(entity_box, player);
            draw_name(entity_box, player);
            draw_health(entity_box, player);
            draw_armor(entity_box, player);
            draw_ammo(entity_box, player);
            draw_weapon(entity_box, player);
            draw_flags(entity_box, player);
            draw_skeleton(player);
            draw_headspot(player);
            draw_barrel(player);
        }
    }

    void draw_box(const bounding_box_t &entity_box, c_player *player) {

        if (!settings.visuals.player.bounding_box)
            return;

        const auto box_position = point_t{ entity_box.x, entity_box.y };
        const auto box_size = point_t{ entity_box.width, entity_box.height };

        render::draw_rect(box_position - 1.0f, box_size + 2.0f, { 0, 0, 0, 210 });
        render::draw_rect(box_position + 1.0f, box_size - 2.0f, { 0, 0, 0, 210 });
        render::draw_rect(box_position, box_size, settings.visuals.player.bounding_box_color);
    }

    void draw_name(const bounding_box_t &entity_box, c_player *player) {

        if (!settings.visuals.player.player_name)
            return;

        player_info_t info;

        if (!interfaces::engine_client->get_player_info(player->get_networkable()->index(), info))
            return;

        const auto player_name = info.fake_player ? std::format(xs("BOT {}"), info.name) : info.name;

        const auto text_size = render::measure_text(player_name.c_str(), FONT_VERDANA_12_BOLD);
        render::draw_text({ entity_box.x + (entity_box.width / 2) - (text_size.x / 2) + 1, entity_box.y - text_size.y - 2 + 1 }, { 5, 5, 5, 220 }, player_name.c_str(), FONT_VERDANA_12_BOLD);
        render::draw_text({ entity_box.x + (entity_box.width / 2) - (text_size.x / 2), entity_box.y - text_size.y - 2 }, settings.visuals.player.player_name_color, player_name.c_str(), FONT_VERDANA_12_BOLD);
    }

    void draw_health(const bounding_box_t &entity_box, c_player *player) {

        if (!settings.visuals.player.health)
            return;

        const auto clamped_health = std::clamp(player->get_health(), 0, player->max_health());
        const auto bar_size = std::clamp((clamped_health * entity_box.height) / 100, 0.f, entity_box.height);
        const auto red = std::min((510 * (100 - clamped_health)) / 100, 255);
        const auto green = std::min((510 * clamped_health) / 100, 255);

        render::fill_rect({ entity_box.x - 6, entity_box.y - 1 }, { 4, entity_box.height + 2 }, { 0, 0, 0, 180 });
        render::fill_rect({ entity_box.x - 5, entity_box.y + (entity_box.height - bar_size) }, { 2, bar_size }, { red, green, 0, 210 });

        // don't use clamped health here, because if their health
        // is larger than 100 we want to draw the health amount
        if (player->get_health() <= 90 || player->get_health() > 100) {
            char health_text_buffer[8];

            sprintf_s(health_text_buffer, xs("%d"), player->get_health());

            const auto health_text_size = render::measure_text(health_text_buffer, FONT_SMALL_TEXT);

            render::draw_text_outlined({ entity_box.x - 4.0f - health_text_size.x * 0.5f, entity_box.y + (entity_box.height - bar_size) - 6.0f }, { 255, 255, 255, 220 }, { 5, 5, 5, 220 }, health_text_buffer, FONT_SMALL_TEXT);
        }
    }

    void draw_armor(const bounding_box_t &entity_box, c_player *player) {
        if (!settings.visuals.player.armor)
            return;

        const auto player_index = player->get_networkable()->index();
        const auto box_multiplier = player->get_armor() / 100.0f;
        const auto box_width = std::clamp(entity_box.width * box_multiplier, 0.0f, entity_box.width);

        render::fill_rect({ entity_box.x - 1.0f, entity_box.y + entity_box.height + m_bottom_offset[player_index] + 2.0f }, { entity_box.width + 2.0f, 4.0f }, { 0, 0, 0, 180 });
        render::fill_rect({ entity_box.x, entity_box.y + entity_box.height + m_bottom_offset[player_index] + 3.0f }, { box_width, 2.0f }, { 255, 255, 255 });

        if (player->get_armor() < 90) {
            char armor_text_buffer[8];

            sprintf_s(armor_text_buffer, xs("%d"), player->get_armor());

            const auto armor_text_size = render::measure_text(armor_text_buffer, FONT_SMALL_TEXT);

            render::draw_text_outlined({ entity_box.x + box_width - armor_text_size.x * 0.5f, entity_box.y + entity_box.height + m_bottom_offset[player_index] }, { 255, 255, 255, 220 }, { 5, 5, 5, 220 }, armor_text_buffer, FONT_SMALL_TEXT);
        }

        m_bottom_offset[player_index] += 6.0f;
    }

    void draw_ammo(const bounding_box_t &entity_box, c_player *player) {
        if (!settings.visuals.player.ammo)
            return;

        const auto weapon = (c_weapon *)player->get_active_weapon_handle().get();

        if (!weapon)
            return;

        const auto weapon_data = interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index());

        if (!weapon_data)
            return;

        if (weapon_data->max_clip_ammo < 1)
            return;

        const auto player_index = player->get_networkable()->index();
        const auto reload_layer = &player->animation_overlay().element(1);
        const auto box_multiplier = player->is_reloading() ? reload_layer->cycle : (weapon->get_ammo1() / (float)weapon_data->max_clip_ammo);
        const auto box_width = std::clamp(entity_box.width * box_multiplier, 0.0f, entity_box.width);

        render::fill_rect({ entity_box.x - 1.0f, entity_box.y + entity_box.height + m_bottom_offset[player_index] + 2.0f }, { entity_box.width + 2.0f, 4.0f }, { 0, 0, 0, 180 });
        render::fill_rect({ entity_box.x, entity_box.y + entity_box.height + m_bottom_offset[player_index] + 3.0f }, { box_width, 2.0f }, settings.visuals.player.ammo_color);

        if (weapon->get_ammo1() > 0 && weapon->get_ammo1() < weapon_data->max_clip_ammo && !player->is_reloading()) {
            char ammo_text_buffer[8];

            sprintf_s(ammo_text_buffer, xs("%d"), weapon->get_ammo1());

            const auto ammo_text_size = render::measure_text(ammo_text_buffer, FONT_SMALL_TEXT);

            render::draw_text_outlined({ entity_box.x + box_width - ammo_text_size.x * 0.5f, entity_box.y + entity_box.height + m_bottom_offset[player_index] - 1.0f }, { 255, 255, 255, 255 }, { 5, 5, 5, 220 }, ammo_text_buffer, FONT_SMALL_TEXT);
        }

        m_bottom_offset[player_index] += 5.0f;
    }

    void draw_weapon(const bounding_box_t &entity_box, c_player *player) {

        if (!settings.visuals.player.weapon)
            return;

        const auto weapon = (c_weapon *)player->get_active_weapon_handle().get();

        if (!weapon)
            return;

        const auto weapon_data = interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index());

        if (!weapon_data)
            return;

        const auto localized_name = interfaces::localize->find(weapon_data->hud_name);

        char localized_name_buffer[32];

        memset(localized_name_buffer, 0, sizeof(localized_name_buffer));

        const auto localized_name_length = WideCharToMultiByte(CP_UTF8, 0, localized_name, wcslen(localized_name), localized_name_buffer, sizeof(localized_name_buffer), nullptr, nullptr);

        for (auto i = 0; i < localized_name_length; i++)
            localized_name_buffer[i] = std::toupper(localized_name_buffer[i]);

        auto localized_name_size = render::measure_text(localized_name_buffer, FONT_SMALL_TEXT);

        localized_name_size += point_t{ 1.0f, 1.0f };

        render::draw_text_outlined({ entity_box.x + entity_box.width * 0.5f - localized_name_size.x * 0.5f, entity_box.y + entity_box.height + m_bottom_offset[player->get_networkable()->index()] + 1.0f }, 
            { 255, 255, 255, 255 }, { 5, 5, 5, 220 }, localized_name_buffer, FONT_SMALL_TEXT);

        m_bottom_offset[player->get_networkable()->index()] += localized_name_size.y;
    }

    void draw_flags(const bounding_box_t &entity_box, c_player *player) {

        auto draw_flag = [flag_offset = 0.0f, &entity_box, player](const char *flag_text, const color_t &flag_color) mutable {
            const auto flag_text_size = render::measure_text(flag_text, FONT_SMALL_TEXT);

            render::draw_text_outlined({ entity_box.x + entity_box.width + 3.0f, entity_box.y - 1.0f + flag_offset }, flag_color, { 5, 5, 5, 220 }, flag_text, FONT_SMALL_TEXT);

            flag_offset += flag_text_size.y;
        };

        if (settings.visuals.player.flags & (1 << 0))
            draw_flag(player->get_has_helmet() ? xs("HK") : xs("K"), { 255, 255, 255 });

        if (settings.visuals.player.flags & (1 << 1) && player->get_is_scoped())
            draw_flag(xs("SCOPE"), { 0, 150, 255, 200 });

        if (settings.visuals.player.flags & (1 << 2) && player->is_reloading())
            draw_flag(xs("RELOAD"), { 2, 106, 198, 200 });

        if (settings.visuals.player.flags & (1 << 3) && player->is_flashed())
            draw_flag(xs("FLASH"), { 255, 255, 255, 200 });

        if (settings.visuals.player.flags & (1 << 4) && player->has_bomb())
            draw_flag(xs("C4"), { 230, 50, 50, 200 });

        if (settings.visuals.player.flags & (1 << 5) && player->get_is_defusing())
            draw_flag(xs("DEFUSE"), { 255, 100, 0, 200 });

        if (settings.visuals.player.flags & (1 << 6) && player->is_smoked())
            draw_flag(xs("SMOKE"), { 255, 255, 255, 200 });

        if (settings.visuals.player.flags & (1 << 7) && player->get_health() == 1)
            draw_flag(xs("FLASH KILL"), { 125, 255, 248, 200 });

        if (settings.visuals.player.flags & (1 << 8)) {
            auto flag_string = std::format(xs("${}"), player->get_money());
            draw_flag(flag_string.c_str(), { 149, 184, 6, 200 });
        }
    }

    void draw_skeleton(c_player *player) {

        if (!settings.visuals.player.skeleton)
            return;

        const auto studio_hdr = interfaces::model_info->get_studio_model(player->get_renderable()->get_model());

        if (!studio_hdr)
            return;

        std::array<matrix3x4_t, 128> matrices = {};

        if (!player->get_renderable()->setup_bones(matrices.data(), matrices.size(), 0x100, interfaces::global_vars->current_time))
            return;

        for (int i = 0; i < studio_hdr->bones_count; ++i) {
            studio_bone_t *bone = studio_hdr->get_bone(i);

            if (!bone || !(bone->flags & 0x100) || bone->parent == -1)
                continue;

            vector_t child, parent;
            point_t child_screen, parent_screen;

            math::matrix_position(matrices.at(i), child);
            math::matrix_position(matrices.at(bone->parent), parent);

            // this is to fix the neck glitch
            static const int chest_bone_number = 6;
            vector_t chest_bone, upper_chest_bone;

            math::matrix_position(matrices.at(chest_bone_number + 1), upper_chest_bone);
            math::matrix_position(matrices.at(chest_bone_number), chest_bone);

            vector_t upper_direction = upper_chest_bone - chest_bone;
            vector_t breast_bone = chest_bone + upper_direction / 2.0f;

            vector_t child_delta = child - breast_bone;
            vector_t parent_delta = parent - breast_bone;

            if (parent_delta.length() < 9 && child_delta.length() < 9)
                parent = breast_bone;

            if (i == chest_bone_number - 1)
                child = breast_bone;

            if (abs(child_delta.z) < 5 && (parent_delta.length() < 5 && child_delta.length() < 5) || i == chest_bone_number)
                continue;

            if (!math::world_to_screen(child, child_screen) || !math::world_to_screen(parent, parent_screen))
                continue;

            render::draw_line(child_screen, parent_screen, settings.visuals.player.skeleton_color);
        }
    }

    void draw_headspot(c_player *player) {

        if (!settings.visuals.player.head_spot)
            return;

        point_t screen;

        if (!math::world_to_screen(player->get_hitbox_pos(HEAD), screen))
            return;

        render::fill_circle(screen, 2.f, settings.visuals.player.head_spot_color);
    }

    void draw_barrel(c_player *player) {

        if (!settings.visuals.player.barrel)
            return;

        const auto weapon = (c_weapon *)player->get_active_weapon_handle().get();

        if (!weapon)
            return;

        const auto weapon_info = interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index());

        if (!weapon_info)
            return;

        const vector_t eye_angles = player->get_eye_angles();

        vector_t f;
        math::angle_to_vector(eye_angles, f);
        f *= weapon_info->range;

        const vector_t src = player->get_hitbox_pos(HEAD);
        const vector_t dst = src + f;

        c_trace_filter filter;
        filter.skip = player;

        trace_t tr;
        interfaces::trace->trace_ray(ray_t{ src, dst }, MASK_SHOT, &filter, &tr);

        interfaces::debug_overlay->add_line(tr.start_pos, tr.end_pos, { 255, 255, 255 }, false, interfaces::global_vars->interval_per_tick);
        interfaces::debug_overlay->add_swept_box(tr.end_pos, tr.end_pos, -.9f, .9f, eye_angles, { 255, 255, 255 }, interfaces::global_vars->interval_per_tick);
    }

    void draw_planted_bomb(c_entity *entity) {

        if (!settings.visuals.world.planted_bomb)
            return;

        auto planted_bomb = (c_entity *)entity;

        if (planted_bomb->get_networkable()->is_dormant())
            return;

        c_client_class *client_class = planted_bomb->get_networkable()->get_client_class();

        if (!client_class)
            return;

        bounding_box_t entity_box;

        if (!get_bounding_box(entity, entity_box))
            return;

        if (client_class->class_id == CPlantedC4) {
            const auto text_size = render::measure_text(xs("planted c4"), FONT_TAHOMA_11);
            const auto text_pos = point_t{ entity_box.x + entity_box.width * 0.5f - text_size.x * 0.5f, entity_box.y + entity_box.height * 0.5f - text_size.y * 0.5f };

            render::draw_text(text_pos, settings.visuals.world.grenades_color, xs("planted c4"), FONT_TAHOMA_11);
        }
    }

    void draw_dropped_weapon(c_entity *entity) {

        auto weapon = (c_weapon *)entity;

        if (weapon->get_owner_handle() != entity_handle_t())
            return;

        if (weapon->get_networkable()->is_dormant())
            return;

        auto weapon_info = interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index());

        if (!weapon_info)
            return;

        bounding_box_t entity_box;

        if (!get_bounding_box(entity, entity_box))
            return;

        auto client_class = entity->get_networkable()->get_client_class();

        if (settings.visuals.world.weapon && weapon->get_item_definition_index() != WEAPON_C4) {

            const auto localized_name = interfaces::localize->find(weapon_info->hud_name);

            char localized_name_buffer[32];

            memset(localized_name_buffer, 0, sizeof(localized_name_buffer));

            const auto localized_name_length = WideCharToMultiByte(CP_UTF8, 0, localized_name, wcslen(localized_name), localized_name_buffer, sizeof(localized_name_buffer), nullptr, nullptr);

            for (auto i = 0; i < localized_name_length; i++)
                localized_name_buffer[i] = std::tolower(localized_name_buffer[i]);

            const auto text_size = render::measure_text(localized_name_buffer, FONT_TAHOMA_11);
            const auto text_pos = point_t{ entity_box.x + entity_box.width * 0.5f - text_size.x * 0.5f, entity_box.y + entity_box.height * 0.5f - text_size.y * 0.5f };

            render::draw_text(text_pos, settings.visuals.world.weapon_color, localized_name_buffer, FONT_TAHOMA_11);
        }
        else if (settings.visuals.world.dropped_bomb && weapon->get_item_definition_index() == WEAPON_C4) {

            // doing this instead of using localized
            // name because "c4 explosive" looks stupid
            const auto bomb_string = xs("dropped bomb");

            const auto text_size = render::measure_text(bomb_string, FONT_TAHOMA_11);
            const auto text_pos = point_t{ entity_box.x + entity_box.width * 0.5f - text_size.x * 0.5f, entity_box.y + entity_box.height * 0.5f - text_size.y * 0.5f };

            render::draw_text(text_pos, settings.visuals.world.dropped_bomb_color, bomb_string, FONT_TAHOMA_11);
        }
    }

    void draw_thrown_utility(c_entity *entity) {

        if (!settings.visuals.world.grenades)
            return;

        auto grenade = (c_weapon *)entity;

        if (grenade->get_networkable()->is_dormant())
            return;

        c_client_class *client_class = grenade->get_networkable()->get_client_class();

        if (!client_class)
            return;

        bounding_box_t entity_box;

        if (!get_bounding_box(entity, entity_box))
            return;

        const auto model = grenade->get_renderable()->get_model();

        if (!model)
            return;

        auto model_name = interfaces::model_info->get_model_name(model);

        if (client_class->class_id == CBaseCSGrenadeProjectile) {
            if (std::strstr(model_name, xs("fraggrenade")) && grenade->get_explode_effect_tick_begin() < 1) {
                const auto text_size = render::measure_text(xs("frag"), FONT_TAHOMA_11);
                const auto text_pos = point_t{ entity_box.x + entity_box.width * 0.5f - text_size.x * 0.5f, entity_box.y + entity_box.height * 0.5f - text_size.y * 0.5f };

                render::draw_text(text_pos, settings.visuals.world.grenades_color, xs("frag"), FONT_TAHOMA_11);
            }
            else if (std::strstr(model_name, xs("flashbang"))) {
                const auto text_size = render::measure_text(xs("flashbang"), FONT_TAHOMA_11);
                const auto text_pos = point_t{ entity_box.x + entity_box.width * 0.5f - text_size.x * 0.5f, entity_box.y + entity_box.height * 0.5f - text_size.y * 0.5f };

                render::draw_text(text_pos, settings.visuals.world.grenades_color, xs("flashbang"), FONT_TAHOMA_11);
            }
        }

        if (client_class->class_id == CSmokeGrenadeProjectile) {
            const auto text_size = render::measure_text(xs("smoke"), FONT_TAHOMA_11);
            const auto text_pos = point_t{ entity_box.x + entity_box.width * 0.5f - text_size.x * 0.5f, entity_box.y + entity_box.height * 0.5f - text_size.y * 0.5f };

            render::draw_text(text_pos, settings.visuals.world.grenades_color, xs("smoke"), FONT_TAHOMA_11);
        }

        if (client_class->class_id == CMolotovProjectile) {
            if (std::strstr(model_name, xs("molotov"))) {

                const auto text_size = render::measure_text(xs("molotov"), FONT_TAHOMA_11);
                const auto text_pos = point_t{ entity_box.x + entity_box.width * 0.5f - text_size.x * 0.5f, entity_box.y + entity_box.height * 0.5f - text_size.y * 0.5f };

                render::draw_text(text_pos, settings.visuals.world.grenades_color, xs("molotov"), FONT_TAHOMA_11);
            }
            else if (std::strstr(model_name, xs("incendiary"))) {

                const auto text_size = render::measure_text(xs("incendiary"), FONT_TAHOMA_11);
                const auto text_pos = point_t{ entity_box.x + entity_box.width * 0.5f - text_size.x * 0.5f, entity_box.y + entity_box.height * 0.5f - text_size.y * 0.5f };

                render::draw_text(text_pos, settings.visuals.world.grenades_color, xs("incendiary"), FONT_TAHOMA_11);
            }
        }

        if (client_class->class_id == CDecoyProjectile) {
            const auto text_size = render::measure_text(xs("decoy"), FONT_TAHOMA_11);
            const auto text_pos = point_t{ entity_box.x + entity_box.width * 0.5f - text_size.x * 0.5f, entity_box.y + entity_box.height * 0.5f - text_size.y * 0.5f };

            render::draw_text(text_pos, settings.visuals.world.grenades_color, xs("decoy"), FONT_TAHOMA_11);
        }
    }

    void draw_defusal_kit(c_entity *entity) {

        bounding_box_t entity_box;

        if (!get_bounding_box(entity, entity_box))
            return;

        auto client_class = entity->get_networkable()->get_client_class();

        if (!client_class)
            return;

        if (settings.visuals.world.defusal_kit && client_class->class_id == CEconEntity) {

            const auto defusal_kit_string = xs("defusal kit");

            const auto text_size = render::measure_text(defusal_kit_string, FONT_TAHOMA_11);
            const auto text_pos = point_t{ entity_box.x + entity_box.width * 0.5f - text_size.x * 0.5f, entity_box.y + entity_box.height * 0.5f - text_size.y * 0.5f };

            render::draw_text(text_pos, settings.visuals.world.defusal_kit_color, defusal_kit_string, FONT_TAHOMA_11);
        }
    }

    void update_dormant_pos(int index, const vector_t &position) {
        auto &entity_info = get_entity_info(index);

        entity_info.last_server_update = interfaces::global_vars->current_time;
        entity_info.position = position;
    }

    void update_dormant_pos(int index, int money) {
        auto &entity_info = get_entity_info(index);

        entity_info.predicted_money = money;
    }
} // namespace features::visuals::esp