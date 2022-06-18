#include "nade_prediction.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"
#include "../../core/util/util.h"
#include "../../engine/math/math.h"
#include "../../engine/render/render.h"

namespace features::nade_prediction {

    std::vector<nade_point_t> grenade_point = {};

    int grenade_type = 0;
    bool hit_glass_already = false;

    void on_paint_traverse() {

        if (!settings.visuals.local.grenade_prediction)
            return;

        if (!grenade_type || grenade_point.empty())
            return;

        if (cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        auto weapon = (c_weapon *) cheat::local_player->get_active_weapon_handle().get();

        if (!weapon || !weapon->is_grenade())
            return;

        auto grenade = static_cast<c_base_grenade *>(weapon);

        if (!grenade->get_pin_pulled())
            return;

        point_t start, end;

        nade_point_t prev_nade_point = grenade_point.at(0);

        for (auto &it : grenade_point) {

            if (!math::world_to_screen(prev_nade_point.end_position, start) || !math::world_to_screen(it.end_position, end))
                continue;

            render::draw_line({start.x, start.y}, {end.x, end.y}, settings.visuals.local.grenade_prediction_color);

            if (it.detonate || it.plane) {

                // render::draw_rect({end.x - 2.0f, end.y - 2.0f}, 5.0f, 5.0f, {it.detonate ? color_t(255, 0, 255) : color_t(255, 255,
                // 255)});

                if (it.detonate) {
                    constexpr float fill = 0.25f * 180.0f;
                    constexpr float radius = 70.0f;

                    const float cur_time = interfaces::global_vars->current_time;

                    vector_t last_pos;

                    for (float rot = -fill; rot <= fill; rot += 3.0f) {
                        float rotation = rot + cur_time * 250.0f;

                        while (rotation > 360.0f)
                            rotation -= 360.0f;

                        vector_t rotated_pos = it.end_position;
                        rotated_pos.x += std::cos(math::deg_to_rad(rotation)) * radius;
                        rotated_pos.y += std::sin(math::deg_to_rad(rotation)) * radius;

                        if (rot != -fill) {
                            const float alpha = 1.0f - std::abs(rot) / fill;

                            color_t col = {200, 128, 255, 255};

                            const point_t new_screen = util::screen_transform(rotated_pos);
                            const point_t old_screen = util::screen_transform(last_pos);

                            // render::draw_line({old_screen.x - 1.0f, old_screen.y - 1.0f}, {new_screen.x - 1.0f, new_screen.y - 1.0f},
                            // {33, 33, 33, static_cast<int>(col.Value.w * 200.0f)}); render::draw_line({old_screen.x + 1.0f, old_screen.y
                            // + 1.0f}, {new_screen.x + 1.0f, new_screen.y + 1.0f}, {33, 33, 33, static_cast<int>(col.Value.w * 200.0f)});
                            // render::draw_line({old_screen.x, old_screen.y}, {new_screen.x, new_screen.y}, col);
                        }

                        last_pos = rotated_pos;
                    }
                }
            }

            prev_nade_point = it;
        }

        // draw( );
    }

    void on_create_move(c_user_cmd *user_cmd, c_weapon *weapon) {

        if (!settings.visuals.local.grenade_prediction)
            return;

        if (!weapon->is_grenade())
            return;

        auto grenade = static_cast<c_base_grenade *>(weapon);

        if (grenade->get_pin_pulled()) {
            grenade_type = weapon->get_item_definition_index();
            hit_glass_already = false;

            simulate(grenade, user_cmd->view_angles);
        } else {
            grenade_type = 0;
        }
    }

    void setup(c_base_grenade *grenade, vector_t &src, vector_t &dst, const vector_t &view) {

        if (!cheat::local_player || !cheat::local_player->is_valid(true))
            return;

        vector_t thrown_angle = view;

        float pitch = thrown_angle.x;

        if (pitch <= 90.0f) {
            if (pitch < -90.0f) {
                pitch += 360.0f;
            }
        } else {
            pitch -= 360.0f;
        }

        const float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
        thrown_angle.x = a;

        float velocity = 750.0f * 0.9f;
        float b = grenade->get_throw_strength();

        b = b * 0.7f;
        b = b + 0.3f;
        velocity *= b;

        vector_t forward, right, up;
        math::angle_to_vectors(thrown_angle, &forward, &right, &up);

        src = cheat::local_player->get_eye_pos();
        src.z += grenade->get_throw_strength() * 12.0f - 12.0f;

        vector_t dest = src;
        dest += forward * 22.0f;

        trace_t tr;
        trace_hull(src, dest, tr);

        vector_t back = forward;
        back *= 6.0f;

        src = tr.end_pos;
        src -= back;

        dst = cheat::local_player->get_velocity();
        dst *= 1.25f;
        dst += forward * velocity;
    }

    void trace_hull(const vector_t &src, const vector_t &end, trace_t &tr, bool *hit_glass) {
        ray_t ray;
        ray.init(src, end, vector_t(-2.0f, -2.0f, -2.0f), vector_t(2.0f, 2.0f, 2.0f));

        c_trace_filter filter;
        filter.skip = cheat::local_player;

        if (hit_glass) {

            interfaces::trace->trace_ray(ray, CONTENTS_WINDOW, &filter, &tr);

            if (tr.fraction != 1.0f)
                *hit_glass = true;
        }

        int flag = MASK_SOLID;
        flag &= ~CONTENTS_WINDOW;

        interfaces::trace->trace_ray(ray, flag, &filter, &tr);
    }

    bool detonated(const vector_t &thrown_angle, const trace_t &tr, const int tick, const float interval) {
        if (!grenade_type) {
            return false;
        }

        switch (grenade_type) {
        case WEAPON_SMOKEGRENADE:
        case WEAPON_DECOY: {
            if (thrown_angle.length_2d() < 0.1f) {
                return !(tick % static_cast<int>(0.2f / interval));
            }

            return false;
        }
        case WEAPON_MOLOTOV:
        case WEAPON_INCGRENADE: {
            if (tr.fraction != 1.0f && tr.plane.normal.z > 0.7f) {
                return true;
            }

            return static_cast<float>(tick) * interval > 2.1f && !(tick % static_cast<int>(0.050f / interval));
        }
        case WEAPON_FLASHBANG:
        case WEAPON_HEGRENADE: return static_cast<float>(tick) * interval > 1.5f && !(tick % static_cast<int>(0.1f / interval));
        default: return true;
        }
    }

    void resolve_fly_collision(trace_t &tr, vector_t &velocity, const float interval) {
        const auto physics_clip_velocity = [](const vector_t &in, const vector_t &normal, vector_t &out, const float overbounce) {
            constexpr float STOP_EPSILON = 0.1f;

            const float backoff = in.dot(normal) * overbounce;

            for (int i = 0; i < 3; i++) {
                const float change = normal[i] * backoff;
                out[i] = in[i] - change;

                if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON) {
                    out[i] = 0.0f;
                }
            }
        };

        float surface_elasticity = 1.0f;

        /*if ( tr.hit_ent && static_cast< C_CSPlayer * >( tr.hit_ent )->is_player( ) ) {
            surface_elasticity = 0.3f;
        }*/

        float total_elasticity = 0.45f * surface_elasticity;
        math::clamp(total_elasticity, 0.0f, 0.9f);

        vector_t abs_velocity;
        physics_clip_velocity(velocity, tr.plane.normal, abs_velocity, 2.0f);

        abs_velocity *= total_elasticity;

        const float speed_sqr = abs_velocity.length_sqr();
        constexpr float min_speed_sqr = 20.0f * 20.0f;

        if (speed_sqr < min_speed_sqr) {
            abs_velocity.x = 0.0f;
            abs_velocity.y = 0.0f;
            abs_velocity.z = 0.0f;
        }

        if (tr.plane.normal.z > 0.7f) {
            velocity = abs_velocity;
            abs_velocity *= (1.0f - tr.fraction) * interval;

            vector_t abs_end = tr.end_pos;
            abs_end += abs_velocity;

            trace_hull(tr.end_pos, abs_end, tr);
        } else {
            velocity = abs_velocity;
        }
    }

    int step(vector_t &src, vector_t &thrown_angle, bool &bounced, const int tick, const float interval) {
        const auto add_gravity_move = [](vector_t &move, vector_t &vel, const float frame_time, const bool on_ground) {
            const vector_t base_vel;

            move.x = (vel.x + base_vel.x) * frame_time;
            move.y = (vel.y + base_vel.y) * frame_time;

            if (on_ground) {
                move.z = (vel.z + base_vel.z) * frame_time;
            } else {
                const float gravity = 800.0f * 0.4f;

                const float new_z = vel.z - gravity * frame_time;
                move.z = ((vel.z + new_z) / 2.0f + base_vel.z) * frame_time;

                vel.z = new_z;
            }
        };

        bool traced_glass = false;

        vector_t move;
        add_gravity_move(move, thrown_angle, interval, false);

        vector_t abs_end = src;
        abs_end += move;

        trace_t tr;
        trace_hull(src, abs_end, tr, &traced_glass);

        if (traced_glass && !hit_glass_already) {
            thrown_angle *= 0.4f;
            hit_glass_already = true;
        }

        int result = 0;

        if (detonated(thrown_angle, tr, tick, interval)) {
            result |= 1;
        }

        if (tr.fraction != 1.0f) {
            bounced = true;
            result |= 2;

            resolve_fly_collision(tr, thrown_angle, interval);
        }

        src = tr.end_pos;

        return result;
    }

    void simulate(c_base_grenade *grenade, const vector_t &view) {
        vector_t src, thrown_angle;
        setup(grenade, src, thrown_angle, view);

        const float tick_interval = TICK_INTERVAL();

        const int log_step = static_cast<int>(0.025f / tick_interval);
        int log_timer = 0;

        static bool bounced = false;
        bool detonated = false;

        grenade_point.clear();

        for (size_t i = 0; i < grenade_point.max_size() - 1; ++i) {
            if (!log_timer) {
                grenade_point.emplace_back(nade_point_t{src, hit_glass_already, bounced, detonated});
                bounced = false;
            }

            const int s = step(src, thrown_angle, bounced, static_cast<int>(i), tick_interval);

            if (s & 1) {
                detonated = true;
                break;
            }

            if (log_timer >= log_step) {
                log_timer = 0;
            } else if (s & 2) {
                log_timer = 0;
            } else {
                ++log_timer;
            }
        }

        grenade_point.emplace_back(nade_point_t{src, hit_glass_already, bounced, detonated});
    }
}; // namespace features::nade_prediction