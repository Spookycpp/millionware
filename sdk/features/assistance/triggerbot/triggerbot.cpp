#include "triggerbot.h"
#include "../../hitchance/hitchance.h"
#include "../lagcompensation/lag_comp.h"

#include "../../../core/cheat/cheat.h"
#include "../../../core/interfaces/interfaces.h"
#include "../../../core/settings/settings.h"
#include "../../../core/util/util.h"

#include "../../../engine/input/input.h"
#include "../../../engine/math/math.h"
#include "../../../engine/security/xorstr.h"

#include "../../../ui/ui.h"

#include <chrono>

namespace features::legitbot::triggerbot {
    int64_t current_time = 0;

    void on_create_move(c_user_cmd *user_cmd, c_weapon *weapon) {
        if (!settings_lbot->triggerbot.enabled)
            return;

        if (!weapon->is_valid() && weapon->get_item_definition_index() != WEAPON_TASER)
            return;

        // update timer
        {
            const auto now = std::chrono::system_clock::now();
            const auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

            current_time = now_ms.time_since_epoch().count();
        }

        if (should_activate())
            think(user_cmd, weapon);
    }

    void think(c_user_cmd *user_cmd, c_weapon *weapon) {
        const auto send_attack = [&]() -> void {
            user_cmd->buttons |= weapon->get_item_definition_index() != WEAPON_REVOLVER ? BUTTON_IN_ATTACK : BUTTON_IN_ATTACK2;
        };

        const auto unsend_attack = [&]() -> void {
            user_cmd->buttons &= weapon->get_item_definition_index() != WEAPON_REVOLVER ? ~BUTTON_IN_ATTACK : ~BUTTON_IN_ATTACK2;
        };

        const auto weapon_info = weapon ? interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index()) : nullptr;
        ;

        if (!weapon_info)
            return;

        vector_t view_angs;
        interfaces::engine_client->get_view_angles(view_angs);

        const vector_t fwd = math::angle_to_vector(view_angs + cheat::local_player->get_aim_punch_angle() * 2.0f) * weapon_info->range;

        const vector_t src = cheat::local_player->get_eye_pos();
        const vector_t dst = src + fwd;

        if (settings_lbot->triggerbot.check_smoked)
            if (util::line_goes_through_smoke(src, dst))
                return;

        if (!cheat::local_player->can_shoot(weapon)) {
            unsend_attack();
            return;
        }

        const bool should_shoot = !settings_lbot->triggerbot.backtrack.enabled ? trace_to_target(weapon, src, dst)
                                                                               : trace_to_backtracked_target(user_cmd, weapon, src, dst);

        if (should_shoot) {
            if (settings_lbot->triggerbot.delay > 0) {
                static int64_t delayed_time = 0;

                if (current_time > delayed_time) {
                    delayed_time = current_time + settings_lbot->triggerbot.delay;
                    send_attack();
                }
            } else {
                send_attack();
            }
        }
    }

    bool trace_to_target(c_weapon *weapon, const vector_t &start_pos, const vector_t &end_pos) {
        c_trace_filter filter;
        filter.skip = cheat::local_player;

        trace_t tr;
        interfaces::trace->trace_ray(ray_t{start_pos, end_pos}, MASK_SHOT, &filter, &tr);

        if (!is_valid_target(tr.hit_ent))
            return false;

        if (settings_lbot->triggerbot.hit_chance > 0) {
            vector_t aim_angs = math::vector_angles(start_pos, end_pos);

            if (!math::normalize_angles(aim_angs))
                return false;

            if (!hit_chance::can_hit(static_cast<c_player *>(tr.hit_ent), weapon, aim_angs, settings_lbot->triggerbot.hit_chance,
                                     tr.m_hitbox))
                return false;
        }

        return tr.hit_group <= HIT_GROUP_RIGHT_LEG && tr.hit_group > HIT_GROUP_GENERIC;
    }

    bool trace_to_backtracked_target(c_user_cmd *user_cmd, c_weapon *weapon, const vector_t &start_pos, const vector_t &end_pos) {
        float max_latency;

        if (settings.miscellaneous.fake_ping.enabled)
            max_latency = 0.300f + interfaces::engine_client->get_net_channel_info()->get_latency(FLOW_OUTGOING);
        else
            max_latency = static_cast<float>(settings_lbot->triggerbot.backtrack.time) * 0.001f +
                          interfaces::engine_client->get_net_channel_info()->get_latency(FLOW_OUTGOING);

        vector_t aim_angs = math::vector_angles(start_pos, end_pos);

        if (!math::normalize_angles(aim_angs))
            return false;

        const int cur_tick_count = interfaces::global_vars->tick_count;

        for (int i = 1; i < 65; ++i) {
            const auto ent = (c_player *) interfaces::entity_list->get_entity(i);

            if (!is_valid_target(ent))
                continue;

            for (auto &it : features::legitbot::lag_comp::get_record(i)) {
                if (!it.is_valid())
                    continue;

                if (std::abs(cur_tick_count - it.tick_count) > TIME_TO_TICKS(max_latency))
                    continue;

                const auto model = ent->get_renderable()->get_model();
                if (!model)
                    continue;

                const auto hdr = interfaces::model_info->get_studio_model(model);

                if (!hdr)
                    continue;

                const auto set = hdr->get_hitbox_set(ent->get_hitbox_set());

                if (!set)
                    continue;

                for (int j = 0; j < set->hitbox_count; ++j) {
                    const auto box = set->get_hitbox(j);

                    if (!box || box->radius == -1.0f)
                        continue;

                    const vector_t min = math::vector_transform(box->mins, it.matrices[box->bone]);
                    const vector_t max = math::vector_transform(box->maxs, it.matrices[box->bone]);

                    const float radius = box->radius;

                    if (auto intersection = util::get_intersection(start_pos, end_pos, min, max, radius); intersection) {
                        c_trace_filter filter;
                        filter.skip = cheat::local_player;

                        trace_t tr;
                        interfaces::trace->trace_ray(ray_t{start_pos, *intersection}, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

                        if (settings_lbot->triggerbot.hit_chance > 0) {
                            auto hit_ent = static_cast<c_player *>(tr.hit_ent);

                            if (hit_ent->is_valid() &&
                                !hit_chance::can_hit(hit_ent, weapon, aim_angs, settings_lbot->triggerbot.hit_chance, tr.m_hitbox))
                                continue;
                        }

                        if (tr.fraction <= 0.97f)
                            continue;

                        features::legitbot::lag_comp::backtrack_entity(user_cmd, i);

                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool is_valid_target(c_entity *target) {
        if (!target || target == cheat::local_player)
            return false;

        if (!static_cast<c_player *>(target)->is_player())
            return false;

        auto player = static_cast<c_player *>(target);

        if (!player->is_valid() || player->get_has_gun_game_immunity())
            return false;

        const static auto mp_teammates_are_enemies = interfaces::convar_system->find_convar(xs("mp_teammates_are_enemies"));

        if (!settings_lbot->triggerbot.check_team) {

            if (mp_teammates_are_enemies->get_int() == 0 && target->get_team_num() == cheat::local_player->get_team_num())
                return false;
        }

        return true;
    }

    bool should_activate() {

        if (settings_lbot->triggerbot.check_flashed) {
            if (cheat::local_player->is_flashed()) {
                return false;
            }
        }

        if (ui::is_active())
            return false;

        if (!input::is_key_down(settings_lbot->triggerbot.hotkey))
            return false;

        return true;
    }
} // namespace features::legitbot::triggerbot
