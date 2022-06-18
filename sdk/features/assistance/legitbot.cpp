#include "legitbot.h"
#include "lagcompensation/lag_comp.h"

#include "../hitchance/hitchance.h"

#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"

#include "../../engine/input/input.h"
#include "../../engine/math/math.h"
#include "../../engine/security/xorstr.h"

#include "../../ui/ui.h"

namespace features::legitbot {
    vector_t local_angs = vector_t();

    float delta_time = 0.0f;
    float cur_time = 0.0f;

    bool aiming = false;
    bool flicked = false;

    std::deque<aim_record_t> angle_samples = {};

    void on_override_mouse_input(float *x, float *y) {
        interfaces::engine_client->get_view_angles(local_angs);

        if (local_angs.empty())
            return;

        aimbot(x, y);
    }

    void on_create_move(c_user_cmd *user_cmd, c_weapon *weapon) {
        interfaces::engine_client->get_view_angles(local_angs);

        if (local_angs.empty())
            return;

        flick_bot(user_cmd, weapon);
        standalone_rcs(user_cmd, weapon);
    }

    void aimbot(float *x, float *y) {
        aiming = false;

        if (!settings_lbot->enabled || settings.ragebot.enabled)
            return;

        const auto weapon = (c_weapon *) cheat::local_player->get_active_weapon_handle().get();

        if (!weapon || !update_settings(weapon))
            return;

        static float old_time = interfaces::global_vars->current_time;
        const float time = interfaces::global_vars->current_time;

        delta_time = time - old_time;
        old_time = time;

        if (settings_lbot->smoothing.enabled) {
            const float avg_delta = get_average_delta() * settings_lbot->smoothing.factor;

            if (avg_delta > delta_time)
                delta_time = avg_delta;
        }

        const auto target = get_target(settings_lbot->hitbox_method, settings_lbot->fov);
        const auto target_idx = std::get<0>(target);

        if (target_idx != -1) {
            if (should_activate()) {
                aiming = true;

                if (!flicked)
                    aim_at_target(target, x, y);
            }

            assist((c_player *) interfaces::entity_list->get_entity(target_idx), x, y);
        } else {
            cur_time = 0.0f;
        }
    }

    void assist(c_player *target, float *x, float *y) {
        if (!settings_lbot->assist.enabled)
            return;

        const int hitbox = get_bone(target, 1, settings_lbot->assist.fov);

        if (hitbox == -1)
            return;

        const vector_t local_pos = cheat::local_player->get_eye_pos();
        const vector_t enemy_pos = target->get_hitbox_pos(hitbox);

        const vector_t cur_angs = local_angs + cheat::local_player->get_aim_punch_angle() * 2.0f;

        vector_t aim_angs = math::vector_angles(local_pos, enemy_pos);

        if (!math::normalize_angles(aim_angs))
            return;

        aim_angs.x -= cheat::local_player->get_aim_punch_angle().x * 2.0f * (settings_lbot->rcs_x / 100.0f);
        aim_angs.y -= cheat::local_player->get_aim_punch_angle().y * 2.0f * (settings_lbot->rcs_y / 100.0f);

        if (!math::normalize_angles(aim_angs))
            return;

        vector_t view_delta = aim_angs - cur_angs;

        if (!math::normalize_angles(view_delta) || !math::clamp_angles(view_delta))
            return;

        vector_t move_angs = pixels_to_angles(point_t(*x, *y));
        move_angs *= settings_lbot->assist.strength;

        const float delta_x = std::abs(move_angs.x);
        const float delta_y = std::abs(move_angs.y);

        vector_t delta;
        delta.x = std::clamp(view_delta.x, -delta_x, delta_x);
        delta.y = std::clamp(view_delta.y, -delta_y, delta_y);

        const point_t pixels = angles_to_pixels(delta);
        *x += pixels.x;
        *y += pixels.y;
    }

    void flick_bot(c_user_cmd *user_cmd, c_weapon *weapon) {
        flicked = false;

        if (!settings_lbot->enabled)
            return;

        if (!settings_lbot->flick_bot.enabled)
            return;

        if (settings_lbot->check_flashed && cheat::local_player->is_flashed())
            return;

        const auto target = get_target(settings_lbot->hitbox_method, settings_lbot->flick_bot.fov);
        const auto target_idx = std::get<0>(target);

        if (target_idx != -1) {
            if (user_cmd->buttons & BUTTON_IN_ATTACK && cheat::local_player->get_shots_fired() == 0 &&
                cheat::local_player->can_shoot(weapon)) {
                flick_to_target(target, user_cmd, weapon, settings_lbot->flick_bot.enabled == 2);
                flicked = true;
            }
        }
    }

    void standalone_rcs(c_user_cmd *user_cmd, c_weapon *weapon) {
        if (weapon->is_awp() || weapon->is_scout() || weapon->is_auto() || weapon->is_shotgun() || weapon->is_pistol())
            return;

        if (!settings_lbot->rcs.enabled || settings_lbot->rcs.x <= 0.0f && settings_lbot->rcs.y <= 0.0f)
            return;

        static vector_t old_aim_punch;

        const int shots_fired = cheat::local_player->get_shots_fired();

        if (user_cmd->buttons & BUTTON_IN_ATTACK) {
            const vector_t aim_punch = cheat::local_player->get_aim_punch_angle();
            const float length = aim_punch.length_2d();

            if (length <= 0.0f || length >= 10.0f) {
                old_aim_punch = aim_punch;
                return;
            }

            vector_t compensated_angs = {
                settings_lbot->rcs.x > 0.0f ? (aim_punch.x - old_aim_punch.x) * (2.0f * (settings_lbot->rcs.x / 100.0f)) : 0.0f,
                settings_lbot->rcs.y > 0.0f ? (aim_punch.y - old_aim_punch.y) * (2.0f * (settings_lbot->rcs.y / 100.0f)) : 0.0f, 0.0f};

            if (!math::normalize_angles(compensated_angs)) {
                old_aim_punch = aim_punch;
                return;
            }

            if (shots_fired > 2) {
                vector_t view_angs = interfaces::engine_client->get_view_angles();

                if (!math::normalize_angles(view_angs)) {
                    old_aim_punch = aim_punch;
                    return;
                }

                vector_t aim_angs = (view_angs - compensated_angs).clamp();

                if (!math::normalize_angles(aim_angs) || !math::clamp_angles(aim_angs)) {
                    old_aim_punch = aim_punch;
                    return;
                }

                interfaces::engine_client->set_view_angles(aim_angs);
            }

            old_aim_punch = aim_punch;
        } else {
            old_aim_punch.init(0.0f, 0.0f, 0.0f);
        }
    }

    bool update_settings(c_weapon *weapon) {
        if (weapon->is_grenade() || weapon->is_knife() || weapon->get_ammo1() <= 0)
            return false;

        if (settings.global.weapon_groups) {
            if (weapon->is_heavy_pistol()) {
                settings_lbot = &settings.lbot_hpistols;
            } else if (weapon->is_pistol()) {
                settings_lbot = &settings.lbot_pistols;
            } else if (weapon->is_rifle()) {
                settings_lbot = &settings.lbot_rifles;
            } else if (weapon->is_awp()) {
                settings_lbot = &settings.lbot_awp;
            } else if (weapon->is_scout()) {
                settings_lbot = &settings.lbot_scout;
            } else if (weapon->is_auto()) {
                settings_lbot = &settings.lbot_auto;
            } else {
                settings_lbot = &settings.lbot_other;
            }
        } else {
            settings_lbot = &settings.lbot_global;
        }

        return settings_lbot != nullptr;
    }

    void aim_at_target(const target_t &data, float *x, float *y) {
        const auto ent = (c_player *) interfaces::entity_list->get_entity(std::get<0>(data));

        if (!is_valid_target(ent))
            return;

        vector_t aim_angs = math::vector_angles(cheat::local_player->get_eye_pos(), ent->extrapolate_position(std::get<1>(data)));

        if (!math::normalize_angles(aim_angs))
            return;

        aim_angs.x -= cheat::local_player->get_aim_punch_angle().x * 2.0f * (settings_lbot->rcs_x / 100.0f);
        aim_angs.y -= cheat::local_player->get_aim_punch_angle().y * 2.0f * (settings_lbot->rcs_y / 100.0f);

        if (!math::normalize_angles(aim_angs))
            return;

        vector_t delta = aim_angs - local_angs;

        if (!math::normalize_angles(delta) || !math::clamp_angles(delta))
            return;

        const float delta_length = delta.length();

        if (delta_length > 0.0f) {
            cur_time += delta_time;

            if (settings_lbot->speed != 0) {
                const float final_time = delta_length / (settings_lbot->speed / 75.0f);

                if (cur_time > final_time)
                    cur_time = final_time;

                float aim_progress = cur_time / final_time;
                aim_progress = math::ease_in(0.0f, 1.0f, aim_progress, settings_lbot->speed_exponent);

                delta *= aim_progress;
            }

            const point_t pixels = angles_to_pixels(delta);
            *x += pixels.x;
            *y += pixels.y;
        }
    }

    void flick_to_target(const target_t &data, c_user_cmd *user_cmd, c_weapon *weapon, const bool silent) {
        const auto ent = (c_player *) interfaces::entity_list->get_entity(std::get<0>(data));

        if (!is_valid_target(ent))
            return;

        vector_t aim_angs = math::vector_angles(cheat::local_player->get_eye_pos(), ent->extrapolate_position(std::get<1>(data)));

        if (!math::normalize_angles(aim_angs))
            return;

        aim_angs.x -= cheat::local_player->get_aim_punch_angle().x * 2.0f * (settings_lbot->rcs_x / 100.0f);
        aim_angs.y -= cheat::local_player->get_aim_punch_angle().y * 2.0f * (settings_lbot->rcs_y / 100.0f);

        if (!math::normalize_angles(aim_angs) || !math::clamp_angles(aim_angs))
            return;

        if (settings_lbot->flick_bot.hit_chance > 0) {
            if (!hit_chance::can_hit(ent, weapon, aim_angs, settings_lbot->flick_bot.hit_chance, std::get<2>(data))) {
                return;
            }
        }

        user_cmd->view_angles = aim_angs;

        if (!silent)
            interfaces::engine_client->set_view_angles(user_cmd->view_angles);
    }

    target_t get_target(const int method, const float fov) {
        target_t best_target{-1, vector_t(), -1};
        float best_fov = fov;

        const vector_t local_pos = cheat::local_player->get_eye_pos();
        const vector_t cur_angs = local_angs + cheat::local_player->get_aim_punch_angle() * 2.0f;

        for (int i = 1; i < 65; ++i) {
            const auto ent = (c_player *) interfaces::entity_list->get_entity(i);

            if (!is_valid_target(ent))
                continue;

            const int hitbox = get_bone(ent, method, fov);

            if (hitbox == -1)
                continue;

            vector_t aim_pos;

            if (!settings_lbot->target_backtrack)
                aim_pos = ent->get_hitbox_pos(hitbox);
            else
                aim_pos = lag_comp::get_backtracked_position(i);

            if (aim_pos.empty()) {
                aim_pos = ent->get_hitbox_pos(hitbox);

                if (aim_pos.empty())
                    continue;
            }

            if (!settings_lbot->check_visible) {
                if (!ent->is_visible(cheat::local_player, cheat::local_player->get_eye_pos(), aim_pos)) {
                    continue;
                }
            }

            if (settings_lbot->check_smoked) {
                if (util::line_goes_through_smoke(cheat::local_player->get_eye_pos(), aim_pos)) {
                    continue;
                }
            }

            vector_t aim_angs = math::vector_angles(local_pos, aim_pos);

            if (!math::normalize_angles(aim_angs))
                continue;

            const float cur_fov = math::get_fov(cur_angs, aim_angs);

            if (!std::isfinite(cur_fov)) {
                best_target = std::make_tuple(i, aim_pos, hitbox);
                break;
            }

            if (cur_fov < best_fov) {
                best_target = std::make_tuple(i, aim_pos, hitbox);
                best_fov = cur_fov;
            }
        }

        return best_target;
    }

    int get_bone(c_player *target, const int method, const float fov) {
        int best_hitbox = -1;
        float best_fov = fov;

        if (method == 0) {
            switch (settings_lbot->hitbox) {
            case 0: best_hitbox = HEAD; break;
            case 1: best_hitbox = NECK; break;
            case 2: best_hitbox = U_CHEST; break;
            case 3: best_hitbox = L_CHEST; break;
            case 4: best_hitbox = STOMACH; break;
            default: return -1;
            }
        } else if (method == 1) {
            static std::vector<int> hitboxes = {HEAD, NECK, PELVIS, L_CHEST, U_CHEST};

            const vector_t local_pos = cheat::local_player->get_eye_pos();
            const vector_t cur_angs = local_angs + cheat::local_player->get_aim_punch_angle() * 2.0f;

            for (auto &idx : hitboxes) {
                vector_t aim_pos = target->get_hitbox_pos(idx);

                if (aim_pos.empty())
                    continue;

                vector_t aim_angs = math::vector_angles(local_pos, aim_pos);

                if (!math::normalize_angles(aim_angs))
                    continue;

                const float cur_fov = math::get_fov(cur_angs, aim_angs);

                if (!std::isfinite(cur_fov)) {
                    best_hitbox = idx;
                    break;
                }

                if (cur_fov < best_fov) {
                    best_hitbox = idx;
                    best_fov = cur_fov;
                }
            }
        }

        return best_hitbox;
    }

    bool should_activate(const bool check_hotkey_only) {

        if (ui::is_active())
            return false;

        if (!input::is_key_down(settings_lbot->hotkey))
            return false;

        if (settings_lbot->check_flashed && cheat::local_player->is_flashed())
            return false;

        if (cheat::local_player->get_shots_fired() < settings_lbot->start_bullets)
            return false;

        // can't really use this, find better solution.
        // if (!cheat::local_player->can_shoot())
        //    return false;

        return true;
    }

    bool is_valid_target(c_player *target) {
        if (!target || target == cheat::local_player)
            return false;

        if (!target->is_valid())
            return false;

        if (target->get_has_gun_game_immunity())
            return false;

        const static auto mp_teammates_are_enemies = interfaces::convar_system->find_convar(xs("mp_teammates_are_enemies"));

        if (!settings_lbot->check_team) {
            if (mp_teammates_are_enemies->get_int() == 0 && target->get_team_num() == cheat::local_player->get_team_num())
                return false;
        }

        return true;
    }

    void sample_angle_data(const vector_t &angles) {
        const float time = interfaces::global_vars->current_time;

        if (!aiming)
            angle_samples.push_front({angles, time});

        while (static_cast<int>(angle_samples.size()) > settings_lbot->smoothing.samples)
            angle_samples.pop_back();
    }

    float get_average_delta() {
        if (angle_samples.empty()) {
            return 0.0f;
        }

        float avg_delta = 0.0f;

        for (auto &it : angle_samples) {
            static vector_t last_angs = it.view_angles;

            const float time_delta = interfaces::global_vars->current_time - it.time;
            const float delta_diff = delta_time / (time_delta * 2.0f);

            vector_t delta = (last_angs - it.view_angles).clamp();
            const float delta_length = delta.length() * delta_diff;

            avg_delta += delta_length;
            last_angs = it.view_angles;
        }

        return avg_delta / float(settings_lbot->smoothing.samples);
    }

    point_t angles_to_pixels(const vector_t &angles) {
        static auto m_yaw = interfaces::convar_system->find_convar(xs("m_yaw"));
        static auto m_pitch = interfaces::convar_system->find_convar(xs("m_pitch"));

        const float x = angles.x / m_pitch->get_float();
        const float y = angles.y / m_yaw->get_float();

        return point_t(-y, x);
    }

    vector_t pixels_to_angles(const point_t &pixels) {
        static auto m_yaw = interfaces::convar_system->find_convar(xs("m_yaw"));
        static auto m_pitch = interfaces::convar_system->find_convar(xs("m_pitch"));

        const float x = pixels.x * m_pitch->get_float();
        const float y = pixels.y * m_yaw->get_float();

        return vector_t(-y, x, 0.0f).clamp();
    }
} // namespace features::legitbot