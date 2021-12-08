#include "util.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/patterns/patterns.h"
#include "../../core/settings/settings.h"

#include "../../engine/math/math.h"
#include "../../engine/security/xorstr.h"

#include "../../source engine/cvar.h"

#include "../../engine/logging/logging.h"
#include "../../engine/render/render.h"
#include "../../features/miscellaneous/miscellaneous.h"

static bool force_update = false;

float TICK_INTERVAL() {
    return interfaces::global_vars->interval_per_tick;
}

int TIME_TO_TICKS(const float dt) {
    return static_cast<int>(0.5f + dt / TICK_INTERVAL());
}

float TICKS_TO_TIME(const int tick) {
    return tick * TICK_INTERVAL();
}

namespace util {

    float get_total_latency() {

        c_net_channel_info *nci = interfaces::engine_client->get_net_channel_info();

        if (!nci)
            return 0.0f;

        return nci->get_latency(FLOW_OUTGOING) + nci->get_latency(FLOW_INCOMING);
    }

    float get_lerp_time() {

        static auto cl_interpolate = interfaces::convar_system->find_convar(xs("cl_interpolate"));
        static auto cl_interp = interfaces::convar_system->find_convar(xs("cl_interp"));
        static auto cl_updaterate = interfaces::convar_system->find_convar(xs("cl_updaterate"));
        static auto cl_interp_ratio = interfaces::convar_system->find_convar(xs("cl_interp_ratio"));

        if (cl_interp && cl_interpolate && cl_updaterate && cl_interp_ratio) {
            if (cl_interpolate->get_int()) {
                return std::max(cl_interp->get_float(), cl_interp_ratio->get_float() / cl_updaterate->get_float());
            }
        }

        return 0.0f;
    }

    float get_random_float(const float min, const float max) {

        using random_float_t = float (*)(float, float);
        static random_float_t random_float_fn = (random_float_t) GetProcAddress(GetModuleHandleA(xs("vstdlib.dll")), xs("RandomFloat"));

        if (!random_float_fn)
            return 0.f;

        return random_float_fn(min, max);
    }

    bool run_frame() {
        return !!cheat::local_player;
    }

    bool line_goes_through_smoke(const vector_t &src, const vector_t &dst) {

        using line_goes_through_smoke_t = bool (*)(vector_t, vector_t, int16_t);
        static auto line_goes_through_smoke_fn = (line_goes_through_smoke_t) patterns::line_goes_through_smoke;

        if (!line_goes_through_smoke_fn)
            return false;

        return line_goes_through_smoke_fn(src, dst, 1);
    }

    bool intersects_hitbox(const vector_t eye_pos, const vector_t end_pos, const vector_t min, const vector_t max, const float radius) {
        return math::dist_segment_to_segment(eye_pos, end_pos, min, max).length() < radius;
    }

    void auto_accept() {
        const auto set_local_player_ready_fn = reinterpret_cast<bool(__stdcall *)(const char *)>(patterns::get_accept_match());

        if (!set_local_player_ready_fn)
            return;

        set_local_player_ready_fn(xs(""));
    }

    void force_full_update() {

        static float update_time = 0.f;
        static bool should_update = false;

        if (force_update) {
            update_time = interfaces::global_vars->current_time + .1f;
            should_update = true;
            force_update = false;
        }

        if (should_update && interfaces::global_vars->current_time > update_time && interfaces::client_state->delta_tick != -1) {
            logging::debug(xs("Forcing game update"));
            interfaces::client_state->delta_tick = -1;

            should_update = false;
        }
    }

    void set_random_seed(const int seed) {

        using random_seed_t = int(__cdecl *)(int);
        static random_seed_t random_seed_fn = (random_seed_t) GetProcAddress(GetModuleHandleA(xs("vstdlib.dll")), xs("RandomSeed"));

        if (!random_seed_fn)
            return;

        random_seed_fn(seed);
    }

    void play_sound(const char *file_path, int volume) {

        if (volume == -1)
            volume = 100;

        char buffer[256] = {};
        sprintf_s(buffer, xs("playvol \"%s\" \"%s\""), file_path, std::to_string(static_cast<float>(volume) / 100.0f).c_str());

        interfaces::engine_client->execute_command(buffer);
    }

    void movement_fix(c_user_cmd *user_cmd) {

        vector_t wish_fwd, wish_right, wish_up;
        math::angle_to_vectors(cheat::original_angles, &wish_fwd, &wish_right, &wish_up);

        vector_t view_fwd, view_right, view_up;
        math::angle_to_vectors(user_cmd->view_angles, &view_fwd, &view_right, &view_up);

        const float v8 = std::sqrt(wish_fwd.x * wish_fwd.x + wish_fwd.y * wish_fwd.y);
        const float v10 = std::sqrt(wish_right.x * wish_right.x + wish_right.y * wish_right.y);
        const float v12 = std::sqrt(wish_up.z * wish_up.z);

        const vector_t norm_wish_fwd(1.0f / v8 * wish_fwd.x, 1.0f / v8 * wish_fwd.y, 0.0f);
        const vector_t norm_wish_right(1.0f / v10 * wish_right.x, 1.0f / v10 * wish_right.y, 0.0f);
        const vector_t norm_wish_up(0.0f, 0.0f, 1.0f / v12 * wish_up.z);

        const float v14 = std::sqrt(view_fwd.x * view_fwd.x + view_fwd.y * view_fwd.y);
        const float v16 = std::sqrt(view_right.x * view_right.x + view_right.y * view_right.y);
        const float v18 = std::sqrt(view_up.z * view_up.z);

        const vector_t norm_view_fwd(1.0f / v14 * view_fwd.x, 1.0f / v14 * view_fwd.y, 0.0f);
        const vector_t norm_view_right(1.0f / v16 * view_right.x, 1.0f / v16 * view_right.y, 0.0f);
        const vector_t norm_view_up(0.0f, 0.0f, 1.0f / v18 * view_up.z);

        const float v22 = norm_wish_fwd.x * user_cmd->forward_move;
        const float v26 = norm_wish_fwd.y * user_cmd->forward_move;
        const float v28 = norm_wish_fwd.z * user_cmd->forward_move;

        const float v24 = norm_wish_right.x * user_cmd->side_move;
        const float v23 = norm_wish_right.y * user_cmd->side_move;
        const float v25 = norm_wish_right.z * user_cmd->side_move;

        const float v30 = norm_wish_up.x * user_cmd->up_move;
        const float v27 = norm_wish_up.z * user_cmd->up_move;
        const float v29 = norm_wish_up.y * user_cmd->up_move;

        user_cmd->forward_move = norm_view_fwd.x * v24 + norm_view_fwd.y * v23 + norm_view_fwd.z * v25 +
                                 (norm_view_fwd.x * v22 + norm_view_fwd.y * v26 + norm_view_fwd.z * v28) +
                                 (norm_view_fwd.y * v30 + norm_view_fwd.x * v29 + norm_view_fwd.z * v27);

        user_cmd->side_move = norm_view_right.x * v24 + norm_view_right.y * v23 + norm_view_right.z * v25 +
                              (norm_view_right.x * v22 + norm_view_right.y * v26 + norm_view_right.z * v28) +
                              (norm_view_right.x * v29 + norm_view_right.y * v30 + norm_view_right.z * v27);

        user_cmd->up_move = norm_view_up.x * v23 + norm_view_up.y * v24 + norm_view_up.z * v25 +
                            (norm_view_up.x * v26 + norm_view_up.y * v22 + norm_view_up.z * v28) +
                            (norm_view_up.x * v30 + norm_view_up.y * v29 + norm_view_up.z * v27);

        user_cmd->forward_move = std::clamp(user_cmd->forward_move, -450.0f, 450.0f);
        user_cmd->side_move = std::clamp(user_cmd->side_move, -450.0f, 450.0f);
        user_cmd->up_move = std::clamp(user_cmd->up_move, -320.0f, 320.0f);
    }

    void movement_fix(const vector_t &old_angles, c_user_cmd *user_cmd) {
        const vector_t view_angles = {0.0f, old_angles.y, 0.0f};
        const vector_t aim_angles = {0.0f, user_cmd->view_angles.y, 0.0f};

        vector_t view_forward, view_side;
        vector_t aim_forward, aim_side;

        math::angle_vectors(view_angles, &view_forward, &view_side);
        math::angle_vectors(aim_angles, &aim_forward, &aim_side);

        view_forward.normalize();
        view_side.normalize();

        const vector_t forward_norm = view_forward * user_cmd->forward_move;
        const vector_t side_norm = view_side * user_cmd->side_move;

        const float new_forward = forward_norm.dot(aim_forward) + side_norm.dot(aim_forward);
        const float new_side = forward_norm.dot(aim_side) + side_norm.dot(aim_side);

        user_cmd->forward_move = new_forward;
        user_cmd->side_move = new_side;
    }

    void disable_model_occlusion() {
        const static auto r_drawallrenderables = interfaces::convar_system->find_convar(xs("r_drawallrenderables"));

        if (r_drawallrenderables->get_int() != 1) {
            r_drawallrenderables->callbacks.clear();
            r_drawallrenderables->set_value(1);
        }
    }

    void open_settings_folder() {
        const std::string path = xs(".\\mw\\configs");
        ShellExecuteA(NULL, NULL, path.data(), NULL, NULL, SW_SHOWNORMAL);
    }

    void open_lua_folder() {
        const std::string path = xs(".\\mw\\scripts");
        ShellExecuteA(NULL, NULL, path.data(), NULL, NULL, SW_SHOWNORMAL);
    }

    point_t screen_transform(const vector_t &world) {

        const auto screen_transform = [&](const vector_t &in, point_t &out) -> bool {
            // const static auto &matrix = *(matrix4x4_t *) patterns::get_view_matrix();
            const static auto &matrix = cheat::view_matrix;
            out.x = matrix[0][0] * in.x + matrix[0][1] * in.y + matrix[0][2] * in.z + matrix[0][3];
            out.y = matrix[1][0] * in.x + matrix[1][1] * in.y + matrix[1][2] * in.z + matrix[1][3];

            const auto w = matrix[3][0] * in.x + matrix[3][1] * in.y + matrix[3][2] * in.z + matrix[3][3];

            if (w < 0.001f) {
                return true;
            }

            const auto inv = 1.0f / w;
            out.x *= inv;
            out.y *= inv;

            return true;
        };

        point_t screen;

        if (!screen_transform(world, screen))
            return {};

        const auto screen_size = render::get_screen_size();

        screen.x = screen_size.x * 0.5f + screen.x * screen_size.x * 0.5f;
        screen.y = screen_size.y * 0.5f - screen.y * screen_size.y * 0.5f;

        return screen;
    }

    c_player_resource *get_player_resource() {

        for (int i = 65; i < interfaces::entity_list->get_highest_ent_index(); ++i) {
            const auto ent = (c_entity *) interfaces::entity_list->get_entity(i);

            if (!ent)
                continue;

            const auto cc = ent->get_networkable()->get_client_class();

            if (cc && cc->class_id == CCSPlayerResource) {
                return reinterpret_cast<c_player_resource *>(ent);
            }
        }

        return nullptr;
    }

    uintptr_t *find_hud_element(const char *name) {
        static uintptr_t find_hud_element_addr_1 = patterns::get_find_hud_element_addr_1();

        if (!find_hud_element_addr_1)
            return nullptr;

        static auto this_ptr = *reinterpret_cast<uintptr_t **>(find_hud_element_addr_1 + 1);

        if (!this_ptr)
            return nullptr;

        static uintptr_t find_hud_element_addr_2 = patterns::get_find_hud_element_addr_2();

        if (!find_hud_element_addr_2)
            return nullptr;

        static auto find_hud_element_fn = reinterpret_cast<uint32_t(__thiscall *)(void *, const char *)>(find_hud_element_addr_2);

        if (!find_hud_element_fn)
            return nullptr;

        return reinterpret_cast<uintptr_t *>(find_hud_element_fn(this_ptr, name));
    }

    std::optional<vector_t> get_intersection(const vector_t &start, const vector_t &end, const vector_t &mins, const vector_t &maxs,
                                             float radius) {

        const auto sphere_ray_intersection = [start, end, radius](auto &&center) -> std::optional<vector_t> {
            auto direction = end - start;
            direction /= direction.length();

            auto q = center - start;
            auto v = q.dot(direction);
            auto d = radius * radius - (q.length_sqr() - v * v);

            if (d < FLT_EPSILON) {
                return {};
            }

            return start + direction * (v - std::sqrt(d));
        };

        auto delta = maxs - mins;

        delta /= delta.length();

        for (size_t i = 0; i < std::floor(mins.dist(maxs)); ++i) {
            if (const auto intersection = sphere_ray_intersection(mins + delta * float(i)); intersection) {
                return intersection;
            }
        }

        if (const auto intersection = sphere_ray_intersection(maxs); intersection) {
            return intersection;
        }

        return {};
    }

    std::string sanitize_string(const std::string &str) {

        std::string ret = str;

        for (auto &it : ret) {
            switch (it) {
            case '"':
            case '\\':
            case ';':
            case '\n': it = ' '; break;
            default: break;
            }
        }

        return ret;
    }

    IDirect3DTexture9 *load_texture_from_vpk(const char *file_path, float scale) {
        void *file = interfaces::file_system->open(file_path, xs("r"), xs("GAME"));
        if (!file) {
            return nullptr;
        }

        std::vector<char> buf(interfaces::file_system->size(file));

        interfaces::file_system->read(buf.data(), buf.size(), file);
        interfaces::file_system->close(file);

        IDirect3DTexture9 *texture = render::rasterize_vector(buf.data(), scale);
        if (!texture) {
            return nullptr;
        }

        return texture;
    }

    void undo() {

        interfaces::convar_system->find_convar(xs("weapon_debug_spread_show"))->set_value(0);
        interfaces::convar_system->find_convar(xs("cl_crosshair_recoil"))->set_value(0);
        interfaces::convar_system->find_convar(xs("mat_postprocess_enable"))->set_value(1);
        interfaces::convar_system->find_convar(xs("@panorama_disable_blur"))->set_value(0);
        interfaces::convar_system->find_convar(xs("phys_pushscale"))->set_value(1);
        interfaces::convar_system->find_convar(xs("cl_ragdoll_gravity"))->set_value(600);

        if (interfaces::engine_client->is_in_game()) {
            cheat::local_player->get_flash_alpha() = 255.0f;
            features::miscellaneous::skybox_changer(0);
        }
    }
} // namespace util