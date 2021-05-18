#include "engine_prediction.h"
#include "../movement.h"

#include "../../../core/cheat/cheat.h"

#include "../../../source engine/client_prediction.h"

#include <array>
#include "../../../engine/security/xorstr.h"
#include "../../../core/util/util.h"
#include "../../../engine/math/math.h"

namespace features::engine_prediction {
    int predicted_flags = 0;

    std::array<player_data_t, 65> players;

    void on_create_move(c_user_cmd *user_cmd) {
        run_command(user_cmd);
    }

    void on_frame_stage_notify(const e_client_frame_stage current_stage) {
        // shit crashes
        /*for ( int i = 1; i < 65; ++i ) {
            players [ i ].update( i );
        }*/
    }

    void player_data_t::update(const int ent_idx) {
        auto player = (c_player*)interfaces::entity_list->get_entity(ent_idx);

        if (!player || player == cheat::local_player || !player->is_valid()) {
            valid = false;
            return;
        }

        const float sim_time = player->get_simulation_time();

        if (std::abs(sim_time - simulation_time) != 0.0f) {
            const float delta = std::abs(simulation_time - sim_time);

            if (!valid) {
                old_velocity = player->get_velocity();
            }
            else {
                last_choke = TIME_TO_TICKS(delta);
                old_velocity = velocity;
                breaking_lc = player->get_vec_origin().dist(position) > 64;
            }

            velocity = player->get_anim_data().last_velocity;
            position = player->get_vec_origin();

            valid = true;

            if (breaking_lc) {
                records.push_front({velocity, last_choke});
            }
        }

        simulation_time = sim_time;

        while (records.size() > 32) {
            records.pop_back();
        }
    }

    int get_predicted_choke(const int ent_idx) {
        auto &data = players[ent_idx];

        int adaptive_dtc = 0;
        int static_dtc = 0;
        int min_choke = 16;
        int max_choke = 0;

        if (!data.records.empty()) {
            int prev_choke = data.last_choke;

            float last_dist = 0.0f;

            for (auto &it : data.records) {
                if (it.tick == prev_choke) {
                    static_dtc++;
                }

                const float speed = it.velocity.length_2d();
                const float dist = speed * it.tick * TICK_INTERVAL();

                if (dist > 62.0f && std::abs(last_dist - dist) < 12.0f) {
                    adaptive_dtc++;
                }

                prev_choke = it.tick;

                min_choke = math::min(it.tick, min_choke);
                max_choke = math::max(it.tick, max_choke);

                last_dist = dist;
            }
        }

        if (adaptive_dtc > 10) {
            return TIME_TO_TICKS(64.0f / data.velocity.length_2d()) + 1;
        }

        if (static_dtc > 16 || data.records.empty()) {
            return data.last_choke;
        }

        const float factor = util::get_random_float_range(0.0f, 1.0f);

        return int((1.0f - factor) * float(min_choke) + factor * float(max_choke));
    }

    vector_t aimware_extrapolate(c_player *ent, const vector_t &origin, vector_t &velocity) {
        static auto sv_jump_impulse = interfaces::convar_system->find_convar(XORSTR("sv_jump_impulse"));
        static auto sv_gravity = interfaces::convar_system->find_convar(XORSTR("sv_gravity"));

        const vector_t mins = ent->get_mins();
        const vector_t maxs = ent->get_maxs();

        const vector_t start = origin;
        vector_t end = start + velocity * TICK_INTERVAL();

        ray_t ray;
        c_trace_filter filter;
        trace_t tr;

        filter.skip = ent;

        ray.init(start, end, mins, maxs);
        interfaces::trace->trace_ray(ray, MASK_PLAYERSOLID, &filter, &tr);

        if (tr.fraction != 1.0f) {
            for (int i = 0; i < 2; ++i) {
                velocity -= tr.plane.normal * velocity.dot(tr.plane.normal);

                float dot = velocity.dot(tr.plane.normal);

                if (dot < 0.0f) {
                    velocity -= tr.plane.normal * dot;
                }

                end = tr.end_pos + velocity * TICK_INTERVAL() * (1.f - tr.fraction);

                ray.init(tr.end_pos, end, mins, maxs);
                interfaces::trace->trace_ray(ray, MASK_PLAYERSOLID, &filter, &tr);

                if (tr.fraction == 1.0f) {
                    break;
                }
            }
        }

        end = tr.end_pos;
        end.z -= 2.0f;

        ray.init(tr.end_pos, end, mins, maxs);
        interfaces::trace->trace_ray(ray, MASK_PLAYERSOLID, &filter, &tr);

        if (tr.fraction != 1.0f && tr.plane.normal.z > 0.7f) {
            velocity.z = sv_jump_impulse->get_float();
        }
        else {
            velocity.z -= sv_gravity->get_float() * TICK_INTERVAL();
        }

        return tr.end_pos;
    }

    vector_t full_walk_move(c_player *player, const int ticks) {
        const auto data = get_player_data(player->get_networkable()->index());

        vector_t origin = data.position;
        vector_t velocity = data.velocity;
        vector_t old_velocity = data.velocity;
        vector_t acceleration = vector_t();

        bool is_on_ground = player->get_flags() & ENTITY_FLAG_ONGROUND;

        if (!data.valid) {
            return origin;
        }

        float velocity_dir = math::rad_to_deg(std::atan2(velocity.y, velocity.x));
        float angle_dir = velocity_dir - math::rad_to_deg(std::atan2(data.old_velocity.y, data.old_velocity.x));

        angle_dir *= TICKS_TO_TIME(data.last_choke);

        if (velocity_dir <= 180.0f) {
            if (velocity_dir < -180.0f) {
                velocity_dir += 360.0f;
            }
        }
        else {
            velocity_dir -= 360.0f;
        }

        const float length = velocity.length_2d();

        for (int i = 0; i < ticks; ++i) {
            const float extrapolated_dir = velocity_dir + angle_dir;

            velocity.x = std::cos(math::deg_to_rad(extrapolated_dir)) * length;
            velocity.y = std::sin(math::deg_to_rad(extrapolated_dir)) * length; // hey.... fix please In Fucking Correct

            start_gravity(player, origin, velocity);

            if (is_on_ground) {
                check_jump_button(player, origin, velocity);
                friction(player, velocity);
            }

            check_velocity(player, origin, velocity);

            // fuck walking
            if (!is_on_ground) {
                air_move(velocity, old_velocity, acceleration);
            }

            try_player_move(player, origin, velocity);

            is_on_ground = categorize_position(player, origin, velocity);

            check_velocity(player, origin, velocity);

            finish_gravity(player, origin, velocity);

            if (is_on_ground) {
                velocity.z = 0.0f;
            }

            old_velocity = velocity;
            velocity_dir = extrapolated_dir;
        }

        return origin;
    }

    void check_jump_button(c_player *player, vector_t &origin, vector_t &velocity) {
        static auto sv_jump_impulse = interfaces::convar_system->find_convar(XORSTR("sv_jump_impulse"));

        float ground_factor = 1.0f;

        vector_t ground_point = origin;
        ground_point.z -= 2.0f;

        trace_t tr;
        trace_player_bbox(player, origin, ground_point, &tr);

       //if (tr.hit_ent) {
       //    const auto surface_data = c_game::instance().get_physics_surface()->get_surface_data(tr.surface.props);
       //
       //    if (surface_data) {
       //        ground_factor = surface_data->game.jump_factor;
       //    }
       //}
       //
       //if (ground_factor == 0.0f) {
       //    ground_factor = 1.0f;
       //}

        velocity.z = ground_factor * sv_jump_impulse->get_float();

        finish_gravity(player, origin, velocity);
    }

    void start_gravity(c_player *player, vector_t &origin, vector_t &velocity) {
        static auto sv_gravity = interfaces::convar_system->find_convar(XORSTR("sv_gravity"));

        float gravity = player->get_gravity();

        // if( !gravity ) {
        gravity = 1.0f;
        //}

        velocity.z -= (gravity * sv_gravity->get_float() * 0.5f * TICK_INTERVAL());

        check_velocity(player, origin, velocity);
    }

    void finish_gravity(c_player *player, vector_t &origin, vector_t &velocity) {
        static auto sv_gravity = interfaces::convar_system->find_convar(XORSTR("sv_gravity"));

        float gravity = player->get_gravity();

        // if( !gravity ) {
        gravity = 1.0f;
        //}

        velocity.z -= (gravity * sv_gravity->get_float() * 0.5f * TICK_INTERVAL());

        check_velocity(player, origin, velocity);
    }

    void friction(c_player *player, vector_t &velocity) {
        static auto sv_friction = interfaces::convar_system->find_convar(XORSTR("sv_friction"));
        static auto sv_stopspeed = interfaces::convar_system->find_convar(XORSTR("sv_stopspeed"));

        const float surface_friction = player->get_surface_friction();

        const float speed = velocity.length();

        if (speed < 0.1f) {
            return;
        }

        const float friction = sv_friction->get_float() * surface_friction;
        const float control = speed < sv_stopspeed->get_float() ? sv_stopspeed->get_float() : speed;
        const float drop = control * friction * TICK_INTERVAL();

        float new_speed = speed - drop;

        if (new_speed < 0.0f) {
            new_speed = 0.0f;
        }

        if (new_speed != speed) {
            new_speed /= speed;
            velocity *= new_speed;
        }
    }

    void air_move(vector_t &velocity, vector_t &old_velocity, vector_t &acceleration) {
        vector_t wish_vel = velocity;
        wish_vel[2] = 0.f;

        vector_t wish_dir = wish_vel;
        wish_dir.normalize_in_place();

        const float wish_speed = acceleration.length(); // probably wrong

        air_accelerate(old_velocity, wish_dir, wish_speed);
    }

    void air_accelerate(vector_t &velocity, const vector_t &wish_dir, float wish_speed) {
        static auto sv_airaccelerate = interfaces::convar_system->find_convar(XORSTR("sv_airaccelerate"));

        wish_speed = std::max(wish_speed, 30.0f);

        const float cur_speed = velocity.dot(wish_dir);
        const float delta_speed = wish_speed - cur_speed;

        if (delta_speed <= 0.0f) {
            return;
        }

        float accel_speed = sv_airaccelerate->get_float() * wish_speed * TICK_INTERVAL();

        if (accel_speed > delta_speed) {
            accel_speed = delta_speed;
        }

        for (int i = 0; i < 2; ++i) {
            velocity[i] += accel_speed * wish_dir[i];
        }
    }

    void try_player_move(c_player *player, vector_t &origin, vector_t &velocity) {
        vector_t end_pos = origin + velocity * TICK_INTERVAL();

        trace_t tr;
        trace_player_bbox(player, origin, end_pos, &tr);

        if (tr.fraction != 1.0f) {
            end_pos = tr.end_pos;
        }

        origin = end_pos;
    }

    // there are supposed to be some ladder checks etc here, but we're really only supposed to be doing this when people are breaking lag comp etc
    // also ugly code, please fix :(
    bool categorize_position(c_player *player, vector_t &origin, vector_t &velocity) {
        constexpr float NON_JUMP_VELOCITY = 140.0f;

        vector_t ground_point = origin;
        ground_point.z -= 2.0f;

        if (velocity.z > NON_JUMP_VELOCITY) {
            return false;
        }

        trace_t tr;
        trace_player_bbox(player, origin, ground_point, &tr);

        if (!tr.hit_ent || tr.plane.normal.z < 0.7f) {
            try_touch_ground_in_quadrants(player, origin, ground_point, &tr);

            if (!tr.hit_ent || tr.plane.normal.z < 0.7f) {
                return false;
            }
        }

        return true;
    }

    void check_velocity(c_player *player, vector_t &origin, vector_t &velocity) {
        static auto sv_max_velocity = interfaces::convar_system->find_convar(XORSTR("sv_maxvelocity"));
        const float max_velocity = sv_max_velocity->get_float();

        for (int i = 0; i < 3; ++i) {
            if (!std::isfinite(velocity[i])) {
                velocity[i] = 0.0f;
            }

            if (!std::isfinite(origin[i])) {
                origin[i] = 0.0f;
            }

            velocity[i] = std::clamp(velocity[i], -max_velocity, max_velocity);
        }
    }

    // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/gamemovement.cpp#L2025
    vector_t extrapolate_player(c_player *player, const int ticks) {
        const int index = player->get_networkable()->index();
        const auto data = get_player_data(index);

        vector_t predicted(data.position);
        vector_t velocity(data.velocity);

        if (!data.valid) {
            return predicted;
        }

        float velocity_dir = math::rad_to_deg(std::atan2(velocity.y, velocity.x));
        float angle_dir = velocity_dir - math::rad_to_deg(std::atan2(data.old_velocity.y, data.old_velocity.x));

        angle_dir *= TICKS_TO_TIME(data.last_choke);

        if (velocity_dir <= 180.0f) {
            if (velocity_dir < -180.0f) {
                velocity_dir += 360.f;
            }
        }
        else {
            velocity_dir -= 360.0f;
        }

        const float length = velocity.length_2d();

        for (int i = 0; i < ticks; ++i) {
            const float extrapolated_dir = velocity_dir + angle_dir;

            velocity.x = std::cos(math::deg_to_rad(extrapolated_dir)) * length;
            velocity.y = std::sin(math::deg_to_rad(extrapolated_dir)) * length;

            predicted = aimware_extrapolate(player, predicted, velocity);

            velocity_dir = extrapolated_dir;
        }

        return predicted;
    }

    void trace_player_bbox(c_player *player, const vector_t &start, const vector_t &end, trace_t *tr) {
        ray_t ray;
        ray.init(start, end, player->get_mins(), player->get_maxs());

        c_trace_filter filter;
        filter.skip = player;

        interfaces::trace->trace_ray(ray, MASK_SOLID & ~CONTENTS_MONSTER, &filter, tr);
    }

    void try_touch_ground(c_player *player, const vector_t &start, const vector_t &end, const vector_t &mins, const vector_t &maxs, trace_t *tr) {
        ray_t ray;
        ray.init(start, end, mins, maxs);

        c_trace_filter filter;
        filter.skip = player;

        interfaces::trace->trace_ray(ray, MASK_SOLID & ~CONTENTS_MONSTER, &filter, tr);
    }

    void try_touch_ground_in_quadrants(c_player *player, const vector_t &start, const vector_t &end, trace_t *tr) {
        const vector_t mins_src = player->get_mins();
        const vector_t maxs_src = player->get_maxs();

        const float fraction = tr->fraction;
        const vector_t end_pos = tr->end_pos;

        vector_t mins = mins_src;
        vector_t maxs = vector_t(std::min(0.0f, maxs_src.x), std::min(0.0f, maxs_src.y), maxs_src.z);

        try_touch_ground(player, start, end, mins, maxs, tr);

        if (tr->hit_ent && tr->plane.normal.z >= 0.7f) {
            tr->fraction = fraction;
            tr->end_pos = end_pos;
            return;
        }

        mins = vector_t(std::max(0.0f, mins_src.x), std::max(0.0f, mins_src.y), mins_src.z);
        maxs = maxs_src;

        try_touch_ground(player, start, end, mins, maxs, tr);

        if (tr->hit_ent && tr->plane.normal.z >= 0.7f) {
            tr->fraction = fraction;
            tr->end_pos = end_pos;
            return;
        }

        mins = vector_t(mins_src.x, std::max(0.0f, mins_src.y), mins_src.z);
        maxs = vector_t(std::min(0.0f, maxs_src.x), maxs_src.y, maxs_src.z);

        try_touch_ground(player, start, end, mins, maxs, tr);

        if (tr->hit_ent && tr->plane.normal.z >= 0.7f) {
            tr->fraction = fraction;
            tr->end_pos = end_pos;
            return;
        }

        mins = vector_t(std::max(0.0f, mins_src.x), mins_src.y, mins_src.z);
        maxs = vector_t(maxs_src.x, std::min(0.0f, maxs_src.y), maxs_src.z);

        try_touch_ground(player, start, end, mins, maxs, tr);

        if (tr->hit_ent && tr->plane.normal.z >= 0.7f) {
            tr->fraction = fraction;
            tr->end_pos = end_pos;
            return;
        }

        tr->fraction = fraction;
        tr->end_pos = end_pos;
    }

    void run_command(c_user_cmd *user_cmd) {

        if (!interfaces::move_helper)
            return;

        const int backup_buttons = user_cmd->buttons;
        const float backup_forward_move = user_cmd->forward_move;
        const float backup_side_move = user_cmd->side_move;

        user_cmd->forward_move = user_cmd->side_move = 0.0f;
        user_cmd->buttons &= ~(BUTTON_IN_BACK | BUTTON_IN_FORWARD | BUTTON_IN_MOVE_LEFT | BUTTON_IN_MOVE_RIGHT);

        movedata_t move_data = {};
        memset(&move_data, 0, sizeof(movedata_t));

        // backup data
        const int old_buttons = user_cmd->buttons;
        const float old_current_time = interfaces::global_vars->current_time;
        const float old_frame_time = interfaces::global_vars->frame_time;
        const int old_tick_base = cheat::local_player->get_tick_base();
        const int old_flags = cheat::local_player->get_flags();
        const int old_move_type = cheat::local_player->get_move_type();
        const vector_t old_velocity = cheat::local_player->get_velocity();

        // set globals
        interfaces::global_vars->current_time = interfaces::global_vars->interval_per_tick * float(old_tick_base);
        interfaces::global_vars->frame_time = interfaces::global_vars->interval_per_tick;

        // random seed is already being calculated and set in CreateMove
        **reinterpret_cast<uintptr_t **>(cheat::run_command + 0x3E) = user_cmd->random_seed;          // prediction seed
        **reinterpret_cast<uintptr_t **>(cheat::run_command + 0x54) = uintptr_t(cheat::local_player); // prediction player
        
        // start prediction
        interfaces::move_helper->set_host(cheat::local_player);
        interfaces::game_movement->start_track_prediction_errors(cheat::local_player);

        // run prediction
        interfaces::prediction->setup_move(cheat::local_player, user_cmd, interfaces::move_helper, &move_data);
        interfaces::game_movement->process_movement(cheat::local_player, &move_data);
        interfaces::prediction->finish_move(cheat::local_player, user_cmd, &move_data);

        // finish prediction
        interfaces::game_movement->finish_track_prediction_errors(cheat::local_player);
        interfaces::move_helper->set_host(nullptr);

        **reinterpret_cast<uintptr_t **>(cheat::run_command + 0x3E) = 0xffffffff;
        **reinterpret_cast<uintptr_t ***>(cheat::run_command + 0x54) = nullptr;

        // good to have, can be used for edge jump and such
        predicted_flags = cheat::local_player->get_flags();

        // restore
        user_cmd->buttons = old_buttons;
        interfaces::global_vars->current_time = old_current_time;
        interfaces::global_vars->frame_time = old_frame_time;
        // cheat::local_player->get_tick_base() = old_tick_base;
        cheat::local_player->get_flags() = old_flags;
        cheat::local_player->get_move_type() = old_move_type;
        cheat::local_player->get_velocity() = old_velocity;

        user_cmd->forward_move = backup_forward_move;
        user_cmd->side_move = backup_side_move;
        user_cmd->buttons = backup_buttons;

        // reset move
        interfaces::game_movement->reset();
    }

    int get_predicted_flags() {
        return predicted_flags;
    }

    bool is_breaking_lc(const int ent_idx) {
        return players[ent_idx].breaking_lc;
    }

    player_data_t get_player_data(const int ent_idx) {
        return players[ent_idx];
    }

    void create_edgebug_entry(c_user_cmd* user_cmd) {
        /* credits: clarity.tk */

        features::movement::edgebug_container::prediction::fall_velocity = cheat::local_player->get_fall_velocity();
        features::movement::edgebug_container::prediction::flags = cheat::local_player->get_flags();
        features::movement::edgebug_container::prediction::origin = cheat::local_player->get_vec_origin();
        features::movement::edgebug_container::prediction::absolute_origin = cheat::local_player->get_abs_origin();
        features::movement::edgebug_container::prediction::ground_ent = cheat::local_player->get_ground_entity();
        memset(&features::movement::edgebug_container::prediction::movement_data, 0, sizeof(movedata_t));

        interfaces::move_helper->set_host(cheat::local_player);
        interfaces::prediction->setup_move(cheat::local_player, user_cmd, interfaces::move_helper, &features::movement::edgebug_container::prediction::movement_data);
    }

    void apply_edgebug_data(c_user_cmd *user_cmd) {
        /* credits: clarity.tk */

        interfaces::prediction->finish_move(cheat::local_player, user_cmd, &features::movement::edgebug_container::prediction::movement_data);
        interfaces::move_helper->set_host(nullptr);
        features::movement::edgebug_container::prediction::flags |= 2;
        features::movement::edgebug_container::prediction::movement_data.m_nButtons |= 4;
        cheat::local_player->get_flags() = features::movement::edgebug_container::prediction::flags;
        cheat::local_player->get_fall_velocity() = features::movement::edgebug_container::prediction::fall_velocity;
        cheat::local_player->set_absolute_origin(features::movement::edgebug_container::prediction::absolute_origin);
        cheat::local_player->get_vec_origin() = features::movement::edgebug_container::prediction::origin;
        cheat::local_player->get_ground_entity() = features::movement::edgebug_container::prediction::ground_ent;
    }

    void apply_edgebug_flags() {
        /* credits: clarity.tk */

        cheat::local_player->get_flags() = features::movement::edgebug_container::prediction::flags;
    }

} // namespace features::engine_prediction