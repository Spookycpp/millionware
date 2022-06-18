#include "movement.h"
#include "engine prediction/engine_prediction.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/patterns/patterns.h"
#include "../../core/settings/settings.h"
#include "../../core/util/util.h"

#include "../../engine/hash/hash.h"
#include "../../engine/input/input.h"
#include "../../engine/logging/logging.h"
#include "../../engine/math/math.h"
#include "../../engine/security/xorstr.h"

namespace features::movement {
    void pre_prediction(c_user_cmd *user_cmd) {
        edgebugged = false;

        if (settings.miscellaneous.movement.no_duck_cooldown) {
            user_cmd->buttons |= BUTTON_IN_BULLRUSH;
        }

        if (settings.miscellaneous.movement.bunny_hop) {
            if (cheat::local_player->get_move_type() == MOVE_TYPE_LADDER ||
                input::is_key_down(settings.miscellaneous.movement.jump_bug_hotkey))
                return;

            if (!(cheat::local_player->get_flags() & ENTITY_FLAG_ONGROUND))
                user_cmd->buttons &= ~BUTTON_IN_JUMP;
        }

        if (settings.miscellaneous.movement.air_duck) {
            if (!(cheat::local_player->get_flags() & ENTITY_FLAG_ONGROUND))
                user_cmd->buttons |= BUTTON_IN_DUCK;
        }
    }

    void post_prediction(c_user_cmd *user_cmd, int pre_flags, int post_flags) {

        if (settings.miscellaneous.movement.jump_bug && input::is_key_down(settings.miscellaneous.movement.jump_bug_hotkey)) {
            cheat::impact_sound = true;

            if (!(pre_flags & ENTITY_FLAG_ONGROUND) && post_flags & ENTITY_FLAG_ONGROUND) {
                user_cmd->buttons |= BUTTON_IN_DUCK;
            }

            if (post_flags & ENTITY_FLAG_ONGROUND) {
                user_cmd->buttons &= ~BUTTON_IN_JUMP;
            }
        }

        if (settings.miscellaneous.movement.edge_bug && input::is_key_down(settings.miscellaneous.movement.edge_bug_hotkey)) {
            if (!(pre_flags & ENTITY_FLAG_ONGROUND) && post_flags & ENTITY_FLAG_ONGROUND)
                user_cmd->buttons |= BUTTON_IN_DUCK;
        }

        if (settings.miscellaneous.movement.edge_jump && input::is_key_down(settings.miscellaneous.movement.edge_jump_hotkey)) {
            if (pre_flags & ENTITY_FLAG_ONGROUND && !(post_flags & ENTITY_FLAG_ONGROUND))
                user_cmd->buttons |= BUTTON_IN_JUMP;
        }

        static int64_t ej_lj_duck = -1;
        if (settings.miscellaneous.movement.edge_jump_duck && input::is_key_down(settings.miscellaneous.movement.edge_jump_duck_hotkey)) {
            if (ej_lj_duck == -1 && pre_flags & ENTITY_FLAG_ONGROUND && !(post_flags & ENTITY_FLAG_ONGROUND)) {
                user_cmd->buttons |= BUTTON_IN_JUMP | BUTTON_IN_DUCK;

                ej_lj_duck = interfaces::global_vars->tick_count;
            }

            if (ej_lj_duck != -1) {
                if (interfaces::global_vars->tick_count - ej_lj_duck < 2) {
                    user_cmd->buttons |= BUTTON_IN_DUCK;
                } else {
                    ej_lj_duck = -1;
                }
            }
        }

        static int64_t lj_tick = -1;
        if (settings.miscellaneous.movement.long_jump && input::is_key_down(settings.miscellaneous.movement.long_jump_hotkey)) {
            if (lj_tick == -1) {
                user_cmd->buttons |= BUTTON_IN_JUMP | BUTTON_IN_DUCK;

                lj_tick = interfaces::global_vars->tick_count;
            }

            if (lj_tick != -1) {
                if (interfaces::global_vars->tick_count - lj_tick < 2) {
                    user_cmd->buttons |= BUTTON_IN_DUCK;
                } else {
                    lj_tick = -1;
                }
            }
        }

        static int64_t ej_lj_tick = -1;
        if (settings.miscellaneous.movement.edge_jump_lj_bind &&
            input::is_key_down(settings.miscellaneous.movement.edge_jump_lj_bind_hotkey)) {
            if (ej_lj_tick == -1 && pre_flags & ENTITY_FLAG_ONGROUND && !(post_flags & ENTITY_FLAG_ONGROUND)) {
                user_cmd->buttons |= BUTTON_IN_JUMP | BUTTON_IN_DUCK;

                ej_lj_tick = interfaces::global_vars->tick_count;
            }

            if (ej_lj_tick != -1) {
                if (interfaces::global_vars->tick_count - ej_lj_tick < 2) {
                    user_cmd->buttons |= BUTTON_IN_DUCK;
                } else {
                    ej_lj_tick = -1;
                }
            }
        }

        edgebug_assist(user_cmd);
        edgebug_detection(user_cmd);
    }

    void predict_edgebug(c_user_cmd *user_cmd) {
        cheat::predicting = true;

        if ((int) roundf(cheat::local_player->get_velocity().z) == 0.0 || (cheat::unpredicted_flags & ENTITY_FLAG_ONGROUND) != 0) {
            predicted_successful = 0;
            prediction_failed = 1;
        } else if (cheat::unpredicted_velocity.z < -6.0 && cheat::local_player->get_velocity().z > cheat::unpredicted_velocity.z &&
                   cheat::local_player->get_velocity().z < -6.0 && (cheat::local_player->get_flags() & 1) == 0 &&
                   cheat::local_player->get_move_type() != MOVE_TYPE_NOCLIP && cheat::local_player->get_move_type() != MOVE_TYPE_LADDER) {
            const auto previous_velocity = cheat::local_player->get_velocity().z;

            engine_prediction::start_prediction();
            engine_prediction::end_prediction();

            static auto sv_gravity = interfaces::convar_system->find_convar(xs("sv_gravity"));
            const float gravity_velocity_constant =
                roundf((-sv_gravity->get_float()) * interfaces::global_vars->interval_per_tick + previous_velocity);

            if (gravity_velocity_constant == roundf(cheat::local_player->get_velocity().z)) {
                predicted_successful = 1;
                prediction_failed = 0;
            } else {
                predicted_successful = 0;
                prediction_failed = 1;
            }
        }
    }

    void edgebug_assist(c_user_cmd *user_cmd) {

        if (!input::is_key_down(settings.miscellaneous.movement.edge_bug_assist_hotkey))
            return;

        if (!predicted_successful) {

            cheat::predicting = true;

            should_duck = 0;

            for (auto prediction_ticks_ran = 0; prediction_ticks_ran < 2; ++prediction_ticks_ran) {

                if (prediction_ticks_ran == 1) {
                    should_duck = 1;
                    engine_prediction::apply_edgebug_data(user_cmd);
                }

                if (!prediction_ticks_ran)
                    engine_prediction::apply_edgebug_flags();

                for (auto radius_checked = 1; radius_checked <= settings.miscellaneous.movement.edge_bug_radius; ++radius_checked) {

                    if (prediction_ticks_ran == 1) {
                        engine_prediction::start_prediction();
                        engine_prediction::end_prediction();
                    }

                    if ((cheat::unpredicted_flags & 1) != 0 || cheat::unpredicted_velocity.z > 0.0) {
                        predicted_successful = 0;
                        break;
                    }

                    predict_edgebug(user_cmd);

                    if (predicted_successful) {
                        prediction_ticks = radius_checked;
                        prediction_timestamp = interfaces::global_vars->tick_count;
                        mouse_offset = abs(user_cmd->mouse_dx);
                        break;
                    }

                    if (prediction_failed) {
                        prediction_failed = 0;
                        break;
                    }

                    if (!prediction_ticks_ran) {
                        engine_prediction::start_prediction();
                        engine_prediction::end_prediction();
                    }
                }

                if (predicted_successful)
                    break;
            }
        }

        if (predicted_successful) {

            if (interfaces::global_vars->tick_count < prediction_ticks + prediction_timestamp) {
                user_cmd->forward_move = 0.0;
                user_cmd->side_move = 0.0;

                if (should_duck) {
                    user_cmd->buttons |= BUTTON_IN_DUCK;
                }

                edgebugging = true;
            } else {
                predicted_successful = 0;
                should_duck = 0;
            }
        }

        cheat::predicting = false;
    }

    void edgebug_detection(c_user_cmd *user_cmd) {

        if (!predicted_successful)
            return;

        if ((cheat::local_player->get_velocity().z >= -6.0 || cheat::local_player->get_move_type() == MOVE_TYPE_NOCLIP ||
             cheat::local_player->get_move_type() == MOVE_TYPE_LADDER) &&
            !edgebugged) {
            last_known_velocity = cheat::local_player->get_velocity();
            return;
        }

        if (!edgebugged) {
            if (cheat::local_player->get_velocity().length_2d() <= 250.0 ||
                roundf(cheat::local_player->get_velocity().length_2d()) > 250.0) {
                if (std::floor(last_known_velocity.z) < -7 && (cheat::local_player->get_velocity().z / last_known_velocity.z) <= 0.7) {
                    edgebugged = true;
                }
            }
        }

        last_known_velocity = cheat::local_player->get_velocity();
    }

    void slide_walk(c_user_cmd *user_cmd) {

        if (!settings.miscellaneous.movement.slide_walk)
            return;

        user_cmd->buttons &= ~BUTTON_IN_MOVE_RIGHT;
        user_cmd->buttons &= ~BUTTON_IN_MOVE_LEFT;
        user_cmd->buttons &= ~BUTTON_IN_LEFT;
        user_cmd->buttons &= ~BUTTON_IN_RIGHT;
        user_cmd->buttons &= ~BUTTON_IN_FORWARD;
        user_cmd->buttons &= ~BUTTON_IN_BACK;

        const auto move_type = cheat::local_player->get_move_type();

        if (move_type != MOVE_TYPE_NOCLIP && move_type != MOVE_TYPE_LADDER) {
            if (user_cmd->forward_move < 0.0f) {
                user_cmd->buttons |= BUTTON_IN_FORWARD;
            } else if (user_cmd->forward_move > 0.0f) {
                user_cmd->buttons |= BUTTON_IN_BACK;
            }

            if (user_cmd->side_move < 0.0f) {
                user_cmd->buttons |= BUTTON_IN_MOVE_RIGHT;
                user_cmd->buttons |= BUTTON_IN_RIGHT;
            } else if (user_cmd->side_move > 0.0f) {
                user_cmd->buttons |= BUTTON_IN_MOVE_LEFT;
                user_cmd->buttons |= BUTTON_IN_LEFT;
            }
        } else {
            if (user_cmd->forward_move > 0.0f) {
                user_cmd->buttons |= BUTTON_IN_FORWARD;
            } else if (user_cmd->forward_move < 0.0f) {
                user_cmd->buttons |= BUTTON_IN_BACK;
            }

            if (user_cmd->side_move > 0.0f) {
                user_cmd->buttons |= BUTTON_IN_MOVE_RIGHT;
                user_cmd->buttons |= BUTTON_IN_RIGHT;
            } else if (user_cmd->side_move < 0.0f) {
                user_cmd->buttons |= BUTTON_IN_MOVE_LEFT;
                user_cmd->buttons |= BUTTON_IN_LEFT;
            }
        }
    }

    void strafe_optimizer(c_user_cmd *user_cmd, int pre_flags, int post_flags) {
        if (!settings.miscellaneous.movement.strafe_optimizer)
            return;

        static float old_yaw = user_cmd->view_angles.y;

        if (input::is_key_down(settings.miscellaneous.movement.strafe_optimizer_key)) {
            static auto sensitivity = interfaces::convar_system->find_convar(xs("sensitivity"));
            vector_t velocity = cheat::local_player->get_velocity();

            float m_speed = velocity.length_2d();
            float m_ideal =
                (m_speed > 0.f)
                    ? math::rad_to_deg(std::asin(settings.miscellaneous.movement.strafe_optimizer_max_gain / 100.f) * 30.f) / m_speed
                    : 0.f;

            auto yaw_delta = math::strafe_opti_normalize_angle(user_cmd->view_angles.y - old_yaw, 180);

            auto absolute_yaw_delta = fabsf(yaw_delta);
            auto ideal_strafe = std::copysignf(m_ideal, yaw_delta);

            if (!(pre_flags & ENTITY_FLAG_ONGROUND) && !(post_flags & ENTITY_FLAG_ONGROUND) && fabsf(yaw_delta) > 0.f && m_speed > 150.f &&
                absolute_yaw_delta > (fabsf(m_ideal) / 10.f)) {

                if (m_speed >= settings.miscellaneous.movement.strafe_optimizer_max_velocity)
                    return;

                auto error_margin = ideal_strafe - yaw_delta;
                float target_angle =
                    old_yaw + yaw_delta + (error_margin * settings.miscellaneous.movement.strafe_optimizer_pull_amount / 100.f);
                yaw_delta = target_angle - old_yaw;
                auto moused_x = floor(floor((yaw_delta / sensitivity->get_float()) / 0.022));

                auto humanized_angle = moused_x * sensitivity->get_float() * 0.022;

                user_cmd->view_angles.y = humanized_angle + old_yaw;
            }
        }

        old_yaw = user_cmd->view_angles.y;
        interfaces::engine_client->set_view_angles(user_cmd->view_angles);
    }

    void blockbot(c_user_cmd *user_cmd) {
        if (!settings.miscellaneous.movement.blockbot || !input::is_key_down(settings.miscellaneous.movement.blockbot_key))
            return;

        vector_t local_pos = cheat::local_player->get_abs_origin();

        c_player *closest_teammate = nullptr;

        float dist = 250.f;

        for (int i = 1; i < interfaces::global_vars->max_clients; i++) {
            auto current_ent = reinterpret_cast<c_player *>(interfaces::entity_list->get_entity(i));

            if (!current_ent || current_ent == cheat::local_player)
                continue;

            if (!current_ent->is_alive() || current_ent->get_networkable()->is_dormant())
                continue;

            float current_dist = cheat::local_player->get_abs_origin().dist_2d(current_ent->get_abs_origin());
            if (current_dist < dist) {
                dist = current_dist;
                closest_teammate = current_ent;
            }
        }

        if (!closest_teammate)
            return;

        const vector_t velocity = closest_teammate->get_velocity();
        const auto speed = velocity.length_2d();

        if (closest_teammate) {
            vector_t ally_origin = closest_teammate->get_abs_origin();

            vector_t fixed_velocity = closest_teammate->get_velocity();
            fixed_velocity *= interfaces::global_vars->interval_per_tick;

            ally_origin += fixed_velocity;

            vector_t ang = math::calc_angle(local_pos, ally_origin);

            auto ground_entity = cheat::local_player->get_ground_entity().get();

            if (ground_entity && ground_entity->is_player()) {
                if (local_pos.dist_2d(ally_origin) > 1.0f) {

                    ally_origin += closest_teammate->get_view_offset();
                    vector_t angle = math::calc_angle(local_pos, ally_origin);
                    ally_origin += closest_teammate->get_velocity().length_2d() * interfaces::global_vars->interval_per_tick;

                    user_cmd->forward_move = 450.0f;
                    user_cmd->side_move = 0.0f;
                    util::movement_fix(angle, user_cmd);
                }
            } else {
                // blocking
                vector_t angle = math::calc_angle(cheat::local_player->get_abs_origin(), closest_teammate->get_abs_origin());

                angle.y -= cheat::local_player->get_eye_angles().y;
                angle.normalize();

                float distance_to_player = ally_origin.dist_2d(local_pos);
                vector_t angle_from_ally = math::calc_angle(ally_origin, local_pos);

                vector_t fixed_angle = math::align_with_world(angle_from_ally);

                vector_t fixed_pos = math::make_vector(fixed_angle);
                fixed_pos *= distance_to_player;
                fixed_pos += ally_origin;

                ally_origin += closest_teammate->get_velocity().length_2d() * interfaces::global_vars->interval_per_tick;

                if (local_pos.dist(fixed_pos) > 5.0f) {

                    if (angle.y < 0.0f)
                        user_cmd->side_move = 450.f;
                    else if (angle.y > 0.0f)
                        user_cmd->side_move = -450.f;
                }
            }
        }
    }

    void autostrafer(c_user_cmd *user_cmd) { // moneybonk
        if (!settings.miscellaneous.movement.auto_strafe)
            return;

        const int move_type = cheat::local_player->get_move_type();

        if (move_type == MOVE_TYPE_LADDER || move_type == MOVE_TYPE_NOCLIP)
            return;

        const auto rotate_movement = [&](float rot, const float forward, const float side) {
            rot *= static_cast<float>(math::pi_rad);

            const float cos_rot = std::cos(rot);
            const float sin_rot = std::sin(rot);

            user_cmd->forward_move = cos_rot * forward - sin_rot * side;
            user_cmd->side_move = sin_rot * forward + cos_rot * side;
        };

        const vector_t vel = cheat::local_player->get_velocity();
        const float speed = vel.length_2d();

        const auto on_ground = cheat::local_player->get_flags() & ENTITY_FLAG_ONGROUND;

        if (user_cmd->buttons & BUTTON_IN_JUMP && (speed >= 1.0f || settings.miscellaneous.movement.air_duck) && !on_ground) {
            if (user_cmd->forward_move == 0.0f && user_cmd->side_move == 0.0f) {
                if (!user_cmd->mouse_dx) {
                    const float ideal_rot = std::min(math::rad_to_deg(std::asin(30.0f / std::max(speed, FLT_EPSILON))) * 0.5f, 45.0f);
                    const float sign = user_cmd->command_number % 2 ? 1.0f : -1.0f;

                    rotate_movement((ideal_rot - 90.0f) * sign, 450.0f, 0.0f);
                } else {
                    user_cmd->side_move = user_cmd->mouse_dx < 0 ? -450.0f : 450.0f;
                }
            }
        }
    }
} // namespace features::movement