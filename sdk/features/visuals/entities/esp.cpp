// ReSharper disable CppClangTidyClangDiagnosticImplicitIntFloatConversion
// ReSharper disable CppClangTidyClangDiagnosticFloatConversion
// ReSharper disable CppClangTidyCppcoreguidelinesNarrowingConversions
#pragma warning(disable:4244)

#include <algorithm>
#include <array>
#include <cstdint>

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"
#include "../../../engine/input/input.h"
#include "../../../engine/logging/logging.h"
#include "../../../engine/math/math.h"
#include "../../../engine/render/render.h"
#include "../../../engine/security/xorstr.h"
#include "../../../core/util/util.h"
#include "../world/world.h"
#include "esp.h"

#include <imgui.h>

using namespace features::visuals::esp;

namespace features::visuals::esp {
    std::vector<entity_esp_t> entity_esp;

    void frame() {
        if (!cheat::local_player || !interfaces::engine_client->is_in_game() || !interfaces::engine_client->is_connected()) {
            return;
        }

        // update dormant positions
        update_positions();

        for (auto i = 0; i < interfaces::entity_list->get_highest_ent_index(); i++) {
            entity_esp.resize(interfaces::entity_list->get_highest_ent_index());

            c_entity *entity = interfaces::entity_list->get_entity(i);
            if (!entity) {
                continue;
            }

            const float dist_to_local = cheat::local_player->get_abs_origin().dist_2d(entity->get_abs_origin());

            if (!update_dormancy(i, entity, dist_to_local)) {
                continue;
            }
            
            draw_dropped_weapon(entity, dist_to_local);
            draw_defusal_kit(entity, dist_to_local);
            draw_thrown_utility(entity);         
            draw_planted_bomb(entity);

            world::draw_world(entity);
            draw_player(i, entity);
        }
    }

    void draw_player(const int idx, c_entity *entity) {
        if (!entity->is_player()) {
            return;
        }

        const auto player = reinterpret_cast<c_player *>(entity);
        if (player->get_life_state() != LIFE_STATE_ALIVE || player->get_health() <= 0) {
            return;
        }

        // TODO: team check option
        if (player->get_team_num() == cheat::local_player->get_team_num()) {
            return;
        }

        if (entity->get_networkable()->is_dormant()) {
            const vector_t pos = entity_esp.at(idx).position;
            player->set_absolute_origin({pos.x, pos.y, player->get_abs_origin().z}); // don't ask
        }

        bounding_box_t entity_box;
        if (!get_bounding_box(entity, entity_box)) {
            return;
        }

        entity_esp.at(idx).bottom_offset = 0.0f;

        const point_t screen_size = render::get_screen_size();
        if (entity_box.x + entity_box.width - 1 < 0 || entity_box.x - 1 >= screen_size.x 
            || entity_box.y + entity_box.height - 1 < 0 || entity_box.y - 1 >= screen_size.y) {
            return;
        }

        draw_box(entity_box, player);
        draw_name(entity_box, player);
        draw_health(entity_box, player);
        draw_armor(entity_box, player);
        draw_ammo(entity_box, player);
        draw_weapon(entity_box, player);
        draw_flags(entity_box, player);
        draw_skeleton(player);
        draw_headspot(player);
    }

    void draw_box(const bounding_box_t &entity_box, c_player *player) {
        if (!settings.visuals.player.bounding_box) {
            return;
        }

        const color_t col = get_color(player, settings.visuals.player.bounding_box_color);

        const auto box_position = point_t{ entity_box.x, entity_box.y };
        const auto box_size = point_t{ entity_box.width, entity_box.height };

        render::draw_rect(box_position - 1.0f, box_size + 2.0f, { 0, 0, 0, col.a });
        render::draw_rect(box_position + 1.0f, box_size - 2.0f, { 0, 0, 0, col.a });
        render::draw_rect(box_position, box_size, col);
    }

    void draw_name(const bounding_box_t &entity_box, c_player *player) {
        if (!settings.visuals.player.player_name) {
            return;
        }

        player_info_t info;
        if (!interfaces::engine_client->get_player_info(player->get_networkable()->index(), info)) {
            return;
        }

        const color_t col = get_color(player, settings.visuals.player.player_name_color);

        const std::string player_name = info.fake_player ? std::format(xs("BOT {}"), info.name) : info.name;
        const auto text_size = render::measure_text(player_name.c_str(), FONT_VERDANA_12_BOLD);

        render::draw_text({ entity_box.x + entity_box.width / 2 - text_size.x / 2 + 1, entity_box.y - text_size.y - 2 + 1 }, { 5, 5, 5, col.a }, player_name.c_str(), FONT_VERDANA_12_BOLD);
        render::draw_text({ entity_box.x + entity_box.width / 2 - text_size.x / 2, entity_box.y - text_size.y - 2 }, col, player_name.c_str(), FONT_VERDANA_12_BOLD);
    }

    void draw_health(const bounding_box_t &entity_box, c_player *player) {
        if (!settings.visuals.player.health) {
            return;
        }

        const auto clamped_health = std::clamp(player->get_health(), 0, player->max_health());
        const auto bar_size       = std::clamp(clamped_health * entity_box.height / 100, 0.0f, entity_box.height);
        const auto red            = std::min(510 * (100 - clamped_health) / 100, 255);
        const auto green          = std::min(510 * clamped_health / 100, 255);

        const color_t col = get_color(player, { red, green, 0, 210 });

        render::fill_rect({ entity_box.x - 6, entity_box.y - 1 }, { 4, entity_box.height + 2 }, { 0, 0, 0, col.a });
        render::fill_rect({ entity_box.x - 5, entity_box.y + (entity_box.height - bar_size) }, { 2, bar_size }, col);

        // don't use clamped health here, because if their health
        // is larger than 100 we want to draw the health amount
        if (player->get_health() <= 90 || player->get_health() > 100) {
            const std::string health_text  = std::format(xs("{}"), player->get_health());
            const point_t health_text_size = render::measure_text(health_text.c_str(), FONT_SMALL_TEXT);

            render::draw_text_outlined({ entity_box.x - 4.0f - health_text_size.x * 0.5f, entity_box.y + (entity_box.height - bar_size) - 6.0f }, { 255, 255, 255, col.a }, { 5, 5, 5, col.a }, health_text.c_str(), FONT_SMALL_TEXT);
        }
    }

    void draw_armor(const bounding_box_t &entity_box, c_player *player) {
        if (!settings.visuals.player.armor) {
            return;
        }

        const auto idx             = player->get_networkable()->index();
        const float box_multiplier = static_cast<float>(player->get_armor()) / 100.0f;
        const float box_width      = std::clamp(entity_box.width * box_multiplier, 0.0f, entity_box.width);

        const color_t col = get_color(player, { 255, 255, 255 });

        render::fill_rect({ entity_box.x - 1.0f, entity_box.y + entity_box.height + entity_esp.at(idx).bottom_offset + 2.0f }, { entity_box.width + 2.0f, 4.0f }, { 0, 0, 0, col.a });
        render::fill_rect({ entity_box.x, entity_box.y + entity_box.height + entity_esp.at(idx).bottom_offset + 3.0f }, { box_width, 2.0f }, { 255, 255, 255, col.a });

        if (player->get_armor() < 90) {
            const std::string armor_text = std::format(xs("{}"), player->get_armor());
            const point_t armor_text_size = render::measure_text(armor_text.c_str(), FONT_SMALL_TEXT);

            render::draw_text_outlined({ entity_box.x + box_width - armor_text_size.x * 0.5f, entity_box.y + entity_box.height + entity_esp.at(idx).bottom_offset }, { 255, 255, 255, col.a }, { 5, 5, 5, col.a }, armor_text.c_str(), FONT_SMALL_TEXT);
        }

        entity_esp.at(idx).bottom_offset += 6.0f;
    }

    void draw_ammo(const bounding_box_t &entity_box, c_player *player) {
        if (!settings.visuals.player.ammo) {
            return;
        }

        const auto weapon = reinterpret_cast<c_weapon *>(player->get_active_weapon_handle().get());
        if (!weapon) {
            return;
        }

        const weapon_info_t *weapon_data = interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index());
        if (!weapon_data) {
            return;
        }

        if (weapon_data->max_clip_ammo < 1) {
            return;
        }

        const color_t col = get_color(player, settings.visuals.player.ammo_color);

        const auto idx            = player->get_networkable()->index();
        const auto reload_layer   = &player->animation_overlay().element(1);
        const auto box_multiplier = player->is_reloading() ? reload_layer->cycle : static_cast<float>(weapon->get_ammo1()) / static_cast<float>(weapon_data->max_clip_ammo);
        const auto box_width      = std::clamp(entity_box.width * box_multiplier, 0.0f, entity_box.width);

        render::fill_rect({ entity_box.x - 1.0f, entity_box.y + entity_box.height + entity_esp.at(idx).bottom_offset + 2.0f }, { entity_box.width + 2.0f, 4.0f }, { 0, 0, 0, col.a });
        render::fill_rect({ entity_box.x, entity_box.y + entity_box.height + entity_esp.at(idx).bottom_offset + 3.0f }, { box_width, 2.0f }, col);

        if (weapon->get_ammo1() > 0 && weapon->get_ammo1() < weapon_data->max_clip_ammo && !player->is_reloading()) {
            char ammo_text_buffer[8];

            sprintf_s(ammo_text_buffer, xs("%d"), weapon->get_ammo1());

            const auto ammo_text_size = render::measure_text(ammo_text_buffer, FONT_SMALL_TEXT);

            render::draw_text_outlined({ entity_box.x + box_width - ammo_text_size.x * 0.5f, entity_box.y + entity_box.height + entity_esp.at(idx).bottom_offset - 1.0f }, { 255, 255, 255, col.a }, { 5, 5, 5, col.a }, ammo_text_buffer, FONT_SMALL_TEXT);
        }

        entity_esp.at(idx).bottom_offset += 5.0f;
    }

    void draw_weapon(const bounding_box_t &entity_box, c_player *player) {
        if (!settings.visuals.player.weapon) {
            return;
        }

        const auto weapon = reinterpret_cast<c_weapon *>(player->get_active_weapon_handle().get());
        if (!weapon) {
            return;
        }

        const weapon_info_t *weapon_data = interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index());
        if (!weapon_data) {
            return;
        }

        const wchar_t *localized_name = interfaces::localize->find(weapon_data->hud_name);

        std::vector<char> localized_name_buf(32);
        WideCharToMultiByte(CP_UTF8, 0, localized_name, std::wcslen(localized_name), localized_name_buf.data(), localized_name_buf.size(), nullptr, nullptr);

        point_t localized_name_size = render::measure_text(localized_name_buf.data(), FONT_SMALL_TEXT);
        localized_name_size += point_t{ 1.0f, 1.0f };

        const color_t col = get_color(player, { 255, 255, 255 });
        const int idx = player->get_networkable()->index();

        render::draw_text_outlined({ entity_box.x + entity_box.width * 0.5f - localized_name_size.x * 0.5f, entity_box.y + entity_box.height + entity_esp.at(idx).bottom_offset + 1.0f },
            col, { 5, 5, 5, col.a }, localized_name_buf.data(), FONT_SMALL_TEXT);

        entity_esp.at(idx).bottom_offset += localized_name_size.y;
    }

    void draw_flags(const bounding_box_t &entity_box, c_player *player) {
        auto draw_flag = [flag_offset = 0.0f, &entity_box, player](const char *flag_text, const color_t &flag_color) mutable {
            const color_t col = get_color(player, flag_color);

            const point_t flag_text_size = render::measure_text(flag_text, FONT_SMALL_TEXT);
            render::draw_text_outlined({ entity_box.x + entity_box.width + 3.0f, entity_box.y - 1.0f + flag_offset }, col, { 5, 5, 5, col.a }, flag_text, FONT_SMALL_TEXT);

            flag_offset += flag_text_size.y;
        };

        if (settings.visuals.player.flags & 1 << 0) {
            draw_flag(player->get_has_helmet() ? xs("HK") : xs("K"), { 255, 255, 255, 200 });
        }

        if (settings.visuals.player.flags & 1 << 1 && player->get_is_scoped()) {
            draw_flag(xs("SCOPE"), { 255, 255, 255, 200 });
        }

        if (settings.visuals.player.flags & 1 << 2 && player->is_reloading()) {
            draw_flag(xs("RELOAD"), { 80, 200, 240, 200 });
        }

        if (settings.visuals.player.flags & 1 << 3 && player->is_flashed()) {
            draw_flag(xs("FLASH"), { 255, 255, 255, 200 });
        }

        if (settings.visuals.player.flags & 1 << 4 && player->has_bomb()) {
            draw_flag(xs("C4"), { 255, 100, 100, 200 });
        }

        if (settings.visuals.player.flags & 1 << 5 && player->get_is_defusing()) {
            draw_flag(xs("DEFUSE"), { 255, 100, 100, 200 });
        }

        if (settings.visuals.player.flags & 1 << 6 && player->is_smoked()) {
            draw_flag(xs("SMOKE"), { 255, 255, 255, 200 });
        }

        if (settings.visuals.player.flags & 1 << 7 && player->get_health() == 1) {
            draw_flag(xs("FLASH KILL"), { 255, 255, 255, 200 });
        }

        if (settings.visuals.player.flags & 1 << 8) {
            const std::string flag_string = std::format(xs("${}"), player->get_money());
            draw_flag(flag_string.c_str(), { 135, 240, 60, 200 });
        }

        if (settings.visuals.player.flags & 1 << 9 && player->get_has_defuser()) {
            draw_flag(xs("KIT"), { 255, 255, 255, 200 });
        }
    }

    void draw_skeleton(c_player *player) {
        if (!settings.visuals.player.skeleton || player->get_networkable()->is_dormant()) {
            return;
        }

        const auto studio_hdr = interfaces::model_info->get_studio_model(player->get_renderable()->get_model());
        if (!studio_hdr) {
            return;
        }

        std::array<matrix3x4_t, 128> matrices = {};
        memcpy(matrices.data(), player->get_cached_bone_data().get_elements(), player->get_cached_bone_data().count() * sizeof(matrix3x4_t));

        /*if (!player->get_renderable()->setup_bones(matrices.data(), matrices.size(), 0x100, interfaces::global_vars->current_time)) {
            return;
        }*/

        for (int i = 0; i < studio_hdr->bones_count; ++i) {
            studio_bone_t *bone = studio_hdr->get_bone(i);
            if (!bone || !(bone->flags & 0x100) || bone->parent == -1) {
                continue;
            }

            vector_t child, parent;
            point_t child_screen, parent_screen;

            math::matrix_position(matrices.at(i), child);
            math::matrix_position(matrices.at(bone->parent), parent);

            if (!math::world_to_screen(child, child_screen) || !math::world_to_screen(parent, parent_screen)) {
                continue;
            }

            color_t col = get_color(player, settings.visuals.player.skeleton_color);
            render::draw_line(child_screen, parent_screen, col);
        }
    }

    void draw_headspot(c_player *player) {
        if (!settings.visuals.player.head_spot) {
            return;
        }

        point_t screen;
        if (!math::world_to_screen(player->get_hitbox_pos(HEAD), screen)) {
            return;
        }

        const color_t col = get_color(player, settings.visuals.player.head_spot_color);
        render::fill_circle(screen, 2.0f, col);
    }

    void draw_planted_bomb(c_entity *entity) {

        if (!settings.visuals.world.planted_bomb)
            return;

        auto planted_bomb = entity;

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

    void draw_dropped_weapon(c_entity *entity, const float dist_to_local) {
        if (!entity->is_weapon()) {
            return;
        }

        auto weapon = reinterpret_cast<c_weapon *>(entity);

        if (weapon->get_owner_handle() != entity_handle_t()) {
            return;
        }

        weapon_info_t *weapon_info = interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index());
        if (!weapon_info) {
            return;
        }

        bounding_box_t entity_box;
        if (!get_bounding_box(entity, entity_box)) {
            return;
        }

        const float radius = 0.5f * std::sqrt(entity_box.width * entity_box.width + entity_box.height * entity_box.height);

        if (settings.visuals.world.weapon && weapon->get_item_definition_index() != WEAPON_C4) {
            // text
            const wchar_t *localized_name = interfaces::localize->find(weapon_info->hud_name);

            std::vector<char> localized_name_buf(32);
            WideCharToMultiByte(CP_UTF8, 0, localized_name, std::wcslen(localized_name), localized_name_buf.data(), localized_name_buf.size(), nullptr, nullptr);

            const point_t text_size = render::measure_text(localized_name_buf.data(), FONT_SMALL_TEXT);
            const auto text_pos     = point_t{ entity_box.x + entity_box.width / 2.0f - text_size.x / 2.0f, entity_box.y + entity_box.height / 2.0f - text_size.y / 2.0f + 8.0f };

            // color
            color_t icon_color = settings.visuals.world.weapon_color;
            color_t text_color = { 255, 255, 255 };

            text_color.a *= std::clamp((1200.0f - (dist_to_local - 500.0f)) / 1200.0f, 0.0f, 1.0f);
            icon_color.a *= std::clamp((800.0f - (dist_to_local - 250.0f)) / 800.0f, 0.0f, 1.0f);

            //draw
            if (icon_color.a > 0) {
                IDirect3DTexture9 *texture = get_weapon_texture(weapon_info->weapon_name, 2.0f);
                
                if (texture) {
                    D3DSURFACE_DESC surface_desc;
                    texture->GetLevelDesc(0, &surface_desc);

                    const point_t size     = { static_cast<float>(surface_desc.Width) * 0.25f, static_cast<float>(surface_desc.Height) * 0.25f };
                    const point_t position = { entity_box.x + entity_box.width / 2.0f - size.x / 2.0f, entity_box.y + entity_box.height / 2.0f - size.y / 2.0f - 4.0f };
                    
                    render::draw_image(position + 1.0f, size, { 5, 5, 5, icon_color.a }, texture);
                    render::draw_image(position, size, icon_color, texture);
                }
            }

            if (text_color.a > 0) {
                render::draw_text_outlined(text_pos, text_color, { 5, 5, 5, text_color.a }, localized_name_buf.data(), FONT_SMALL_TEXT);
            }
        }
        else if (settings.visuals.world.dropped_bomb && weapon->get_item_definition_index() == WEAPON_C4) {

            const char *bomb_string = xs("BOMB");

            const auto text_size = render::measure_text(bomb_string, FONT_SMALL_TEXT);
            const auto text_pos = point_t{ entity_box.x + entity_box.width / 2.0f - text_size.x / 2.0f, entity_box.y + entity_box.height + radius / 2.0f - text_size.y / 2.0f };

            render::draw_text(text_pos, settings.visuals.world.dropped_bomb_color, bomb_string, FONT_SMALL_TEXT);
        }
    }

    void draw_thrown_utility(c_entity *entity) {
        auto draw_circle = [](const point_t pos, const float radius, const float progress = 1.0f, const float duration = 1.0f) {
            constexpr float min = math::deg_to_rad(270.0f);

            const float max = math::deg_to_rad(270.0f + 1.0f + 360.0f * (progress / duration));
            if (min > max) {
                return false;
            }

            const color_t color = settings.visuals.world.grenades_color;
            const color_t bg_color = color_t::blend({ 33, 33, 33, 255 }, color, 0.3f);
            
            render::fill_circle(pos, radius, { 5, 5, 5, 155 });

            ImGui::GetOverlayDrawList()->PathArcTo({ pos.x, pos.y }, radius + 1.0f, min, max);
            ImGui::GetOverlayDrawList()->PathStroke(IM_COL32(bg_color.r, bg_color.g, bg_color.b, 65), 0, 1);

            ImGui::GetOverlayDrawList()->PathArcTo({ pos.x, pos.y }, radius, math::deg_to_rad(0.0f), math::deg_to_rad(360.0f));
            ImGui::GetOverlayDrawList()->PathStroke(IM_COL32(bg_color.r, bg_color.g, bg_color.b, 185), 0, 2);

            ImGui::GetOverlayDrawList()->PathArcTo({ pos.x, pos.y }, radius - 2.0f, min, max);
            ImGui::GetOverlayDrawList()->PathStroke(IM_COL32(color.r, color.g, color.b, 255), 0, 2);

            return true;
        };

        if (!entity->is_grenade()) {
            return;
        }

        auto grenade = reinterpret_cast<c_grenade *>(entity);
        if (grenade->get_networkable()->is_dormant()) {
            return;
        }

        c_client_class *client_class = grenade->get_networkable()->get_client_class();
        if (!client_class) {
            return;
        }

        point_t screen;
        if (!math::world_to_screen(entity->get_abs_origin(), screen)) {
            return;
        }

        c_model *model = grenade->get_renderable()->get_model();
        if (!model) {
            return;
        }

        std::string model_name = interfaces::model_info->get_model_name(model);
        float seconds_until_detonation;

        // decoy
        if (client_class->class_id == CDecoyProjectile) {
            if (reinterpret_cast<c_player *>(entity)->get_velocity().length_2d() == 0.0f) {
                // stopped moving, draw the decoy from the decoy_started event instead
                return;
            }

            if (draw_circle(screen, 12.0f)) {
                static IDirect3DTexture9 *texture = nullptr;
                if (!texture) {
                    texture = util::load_texture_from_vpk(xs("materials/panorama/images/icons/equipment/decoy.svg"));
                }
                else {
                    render::draw_image({ screen.x - 5.0f, screen.y - 7.0f }, { 11.0f, 12.0f }, { 255, 255, 255 }, texture);
                }
            }
        }
        else if (client_class->class_id == CBaseCSGrenadeProjectile) {
            seconds_until_detonation = 1.5f - (interfaces::global_vars->current_time - grenade->get_spawn_time());
            if (seconds_until_detonation <= 0.0f) {
                return;
            }
            
            if (draw_circle(screen, 12.0f, seconds_until_detonation, 1.5f)) {
                // HE grenade
                if (model_name.find(xs("fraggrenade")) != std::string::npos) {
                    static IDirect3DTexture9 *texture = nullptr;
                    if (!texture) {
                        texture = util::load_texture_from_vpk(xs("materials/panorama/images/icons/equipment/hegrenade.svg"));
                    }
                    else {
                        render::draw_image({screen.x - 4.0f, screen.y - 7.0f}, {8.0f, 12.0f}, { 255, 255, 255 }, texture);
                    }
                }
                // flashbang
                else if (model_name.find(xs("flashbang")) != std::string::npos) {
                    static IDirect3DTexture9 *texture = nullptr;
                    if (!texture) {
                        texture = util::load_texture_from_vpk(xs("materials/panorama/images/icons/equipment/flashbang.svg"));
                    }
                    else {
                        render::draw_image({screen.x - 5.0f, screen.y - 7.0f}, {11.0f, 12.0f}, { 255, 255, 255 }, texture);
                    }
                }
            }
        }
        else if (client_class->class_id == CMolotovProjectile) {
            static c_convar *detonate_cvar = interfaces::convar_system->find_convar("molotov_throw_detonate_time");
            const float detonate_time      = detonate_cvar->get_float();

            seconds_until_detonation = detonate_time - (interfaces::global_vars->current_time - grenade->get_spawn_time());

            // molotov
            if (draw_circle(screen, 12.0f, seconds_until_detonation, detonate_time)) {
                static IDirect3DTexture9 *texture = nullptr;
                if (!texture) {
                    texture = util::load_texture_from_vpk(xs("materials/panorama/images/icons/equipment/molotov.svg"));
                }
                else {
                    render::draw_image({ screen.x - 5.0f, screen.y - 7.0f }, { 11.0f, 12.0f }, { 255, 255, 255 }, texture);
                }
            }
        }
        else if (client_class->class_id == CSmokeGrenadeProjectile) {
            if (reinterpret_cast<c_player *>(entity)->get_velocity().length_2d() == 0.0f) {
                // stopped moving, draw the smoke from the on_smokegrenade_detonate event instead
                return;
            }

            if (draw_circle(screen, 12.0f)) {
                static IDirect3DTexture9 *texture = nullptr;
                if (!texture) {
                    texture = util::load_texture_from_vpk(xs("materials/panorama/images/icons/equipment/smokegrenade.svg"));
                }

                if (texture) {
                    render::draw_image({ screen.x - 2.0f, screen.y - 7.0f }, { 5.0f, 12.0f }, { 255, 255, 255 }, texture);
                }
            }
        }
    }

    void draw_defusal_kit(c_entity *entity, const float dist_to_local) {
        static IDirect3DTexture9 *texture = nullptr;
        if (!texture) {
            texture = util::load_texture_from_vpk(xs("materials/panorama/images/icons/equipment/defuser.svg"), 4.0f);
        }

        c_client_class *client_class = entity->get_networkable()->get_client_class();
        if (!client_class) {
            return;
        }

        if (!settings.visuals.world.defusal_kit || client_class->class_id != CEconEntity) {
            return;
        }

        bounding_box_t entity_box;
        if (!get_bounding_box(entity, entity_box)) {
            return;
        }

        const float radius = 0.5f * std::sqrt(entity_box.width * entity_box.width + entity_box.height * entity_box.height) / 1.5f;
        
        // text
        const char *defusal_kit_string = xs("DEFUSER");
        const point_t text_size        = render::measure_text(defusal_kit_string, FONT_SMALL_TEXT);
        const auto text_pos            = point_t{ entity_box.x + entity_box.width / 2.0f - text_size.x / 2.0f, entity_box.y + entity_box.height + radius / 2.0f - text_size.y / 2.0f };

        // color
        color_t icon_color = settings.visuals.world.defusal_kit_color;
        color_t text_color = { 255, 255, 255 };
        if (dist_to_local > 250.0f) {
            icon_color.a *= std::clamp((300.0f - (dist_to_local - 250.0f)) / 300.0f, 0.0f, 1.0f);
            text_color.a *= std::clamp((600.0f - (dist_to_local - 250.0f)) / 600.0f, 0.0f, 1.0f);
        }

        // draw
        if (icon_color.a > 0) {
            const color_t bg_color = color_t::blend({ 33, 33, 33, icon_color.a / 2 }, icon_color, 0.3f);
            // render::fill_circle({ entity_box.x + entity_box.width / 2.0f, entity_box.y + entity_box.height / 2.0f }, radius, bg_color);

            if (texture) {
                render::draw_image({ entity_box.x + entity_box.width / 2.0f - radius / 2.0f + 1.0f, entity_box.y + entity_box.height / 2.0f - radius / 2.0f + 1.0f }, { radius, radius }, { 5, 5, 5, icon_color.a }, texture);
                render::draw_image({ entity_box.x + entity_box.width / 2.0f - radius / 2.0f, entity_box.y + entity_box.height / 2.0f - radius / 2.0f }, { radius, radius }, icon_color, texture);
            }
        }

        if (text_color.a > 0) {
            render::draw_text_outlined(text_pos, text_color, { 5, 5, 5, text_color.a / 6 }, defusal_kit_string, FONT_SMALL_TEXT);
        }
    }

    bool get_bounding_box(c_entity *entity, bounding_box_t &out_box) {
        c_collideable *collideable = entity->get_collideable();
        const vector_t mins = collideable->get_mins();
        const vector_t maxs = collideable->get_maxs();

        vector_t points[8] = {
            {mins.x, mins.y, mins.z}, {mins.x, maxs.y, mins.z},
            {maxs.x, maxs.y, mins.z}, {maxs.x, mins.y, mins.z},
            {maxs.x, maxs.y, maxs.z}, {mins.x, maxs.y, maxs.z},
            {mins.x, mins.y, maxs.z}, {maxs.x, mins.y, maxs.z}
        };

        std::ranges::transform(points, std::begin(points), [entity](const auto point) {
            return math::vector_transform(point, entity->get_transformation_matrix());
        });

        std::array<point_t, 8> screen_pos;

        for (int i = 0; i < 8; i++) {
            if (!math::world_to_screen(points[i], screen_pos[i])) {
                return false;
            }
        }

        float left = FLT_MAX;
        float top = FLT_MIN;
        float right = FLT_MIN;
        float bottom = FLT_MAX;

        for (auto &point : screen_pos) {
            left = std::min(left, point.x);
            top = std::max(top, point.y);
            right = std::max(right, point.x);
            bottom = std::min(bottom, point.y);
        }

        out_box = { left, bottom, right - left, (top - bottom) };

        return true;
    }

    IDirect3DTexture9 *get_weapon_texture(std::string weapon_name, const float scale) {
        static std::vector<std::pair<uint32_t, IDirect3DTexture9 *>> textures;

        const auto it = std::ranges::find_if(textures, [&](const std::pair<uint32_t, IDirect3DTexture9 *> &element) {
            return element.first == CRC(weapon_name.c_str());
        });

        if (it != textures.end()) {
            return it->second; // texture has already been created
        }

        const std::string path     = std::format(xs("materials/panorama/images/icons/equipment/{}.svg"), weapon_name.substr(7));
        IDirect3DTexture9 *texture = util::load_texture_from_vpk(path.c_str(), scale);

        if (texture) {
            textures.emplace_back(CRC(weapon_name.c_str()), texture);
        }

        return texture;
    }

    color_t get_color(c_entity *entity, color_t col) {
        const auto blend = [](const color_t in, const float progress) {
            static const color_t clr_gray = { 160, 160, 160, 255 };
            const int a = in.a;

            color_t ret = color_t::blend(clr_gray, in, 0.1f + progress * 0.9f);
            ret.a = a;

            return ret;
        };

        if (entity->get_networkable()->is_dormant()) {
            const float anim = entity_esp.at(entity->get_networkable()->index()).fade;

            col = blend(col, anim);
            col.a *= anim;
        }

        return col;
    }

    bool update_dormancy(const int idx, c_entity *entity, const float dist_to_local) {
        auto &[position, fade, spotted, bottom_offset] = entity_esp.at(idx);

        // dormancy fade
        constexpr float anim_rate = 1.0f / 0.5f;
        float rate = interfaces::global_vars->frame_time * anim_rate;

        if (!entity->get_networkable()->is_dormant()) {
            update_position(idx, entity->get_abs_origin());
            fade = fade > 0.0f ? std::clamp(fade + rate, 0.0f, 1.0f) : 0.5f;
            spotted = true;
        }
        else {
            if (fade < 0.3f && dist_to_local <= 2000.0f) {
                rate *= 0.02f;
            }

            fade = std::clamp(fade -= rate, 0.0f, 1.0f);

            if (fade <= 0.0f) {
                spotted = false;
                return false;
            }
        }

        return true;
    }

    void update_positions() {
        CUtlVector<c_snd_info> sound_info {};
        interfaces::engine_sound->get_active_sounds(sound_info);

        if (!sound_info.count()) {
            return;
        }

        for (int i = 0; i < sound_info.count(); ++i) {
            c_snd_info &snd = sound_info.get_elements()[i];

            if (!snd.origin) {
                continue;
            }

            const int idx = snd.sound_source;
            c_entity *ent = interfaces::entity_list->get_entity(idx);
            if (!ent) {
                continue;
            }

            if (!ent->is_player()) {
                continue;
            }

            if (ent->get_networkable()->is_dormant()) {
                update_position(idx, *snd.origin, true); // force update
            }
            else {
                static std::array<float, 65> last_time;

                if (std::abs(interfaces::global_vars->current_time - last_time.at(idx)) > 1.0f) {
                    last_time[idx] = interfaces::global_vars->current_time;
                    // store sounds for noise esp
                }
            }
        }
    }

    void update_position(const int idx, const vector_t &pos, const bool force_update) {
        auto &[position, fade, spotted, bottom_offset] = entity_esp.at(idx);

        position = pos;

        if (fade > 0.0f && fade <= 0.3f && spotted || force_update) {
            fade = 0.3f;
        }
    }

    void reset_position() {
        for (auto &[position, fade, spotted, bottom_offset] : entity_esp) {
            fade = 0.0f;
            spotted = false;
        }
    }
} // namespace features::visuals::esp