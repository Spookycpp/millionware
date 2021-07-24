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

float util::get_total_latency() {

    c_net_channel_info *nci = interfaces::engine_client->get_net_channel_info();

    if (!nci)
        return 0.0f;

    return nci->get_latency(FLOW_OUTGOING) + nci->get_latency(FLOW_INCOMING);
}

float util::get_lerp_time() {

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

float util::get_random_float(const float min, const float max) {

    using random_float_t = float (*)(float, float);
    static random_float_t random_float_fn = (random_float_t) GetProcAddress(GetModuleHandleA(xs("vstdlib.dll")), xs("RandomFloat"));

    if (!random_float_fn)
        return 0.f;

    return random_float_fn(min, max);
}

bool util::run_frame() {
    return cheat::local_player != nullptr;
}

bool util::line_goes_through_smoke(const vector_t &src, const vector_t &dst) {

    using line_goes_through_smoke_t = bool (*)(vector_t, vector_t, int16_t);
    static auto line_goes_through_smoke_fn = (line_goes_through_smoke_t) patterns::line_goes_through_smoke;

    if (!line_goes_through_smoke_fn)
        return false;

    return line_goes_through_smoke_fn(src, dst, 1);
}

bool util::intersects_hitbox(const vector_t eye_pos, const vector_t end_pos, const vector_t min, const vector_t max, const float radius) {
    return math::dist_segment_to_segment(eye_pos, end_pos, min, max).length() < radius;
}

void util::auto_accept() {
    const auto set_local_player_ready_fn = reinterpret_cast<bool(__stdcall *)(const char *)>(patterns::get_accept_match());

    if (!set_local_player_ready_fn)
        return;

    set_local_player_ready_fn(xs(""));
}

void util::force_full_update() {

    static float update_time = 0.f;
    static bool should_update = false;

    if (force_update) {
        update_time = interfaces::global_vars->current_time + .1f;
        should_update = true;
        force_update = false;
    }

    if (should_update && interfaces::global_vars->current_time > update_time && interfaces::client_state->delta_tick != -1) {
        logging::info("Forcing game update");
        interfaces::client_state->delta_tick = -1;

        should_update = false;
    }
}

void util::set_random_seed(const int seed) {

    using random_seed_t = int(__cdecl *)(int);
    static random_seed_t random_seed_fn = (random_seed_t) GetProcAddress(GetModuleHandleA(xs("vstdlib.dll")), xs("RandomSeed"));

    if (!random_seed_fn)
        return;

    random_seed_fn(seed);
}

void util::set_skybox(const char *name) {
    features::miscellaneous::skybox_changer(settings.visuals.world.skybox);
}

void util::on_frame_stage_notify(const e_client_frame_stage frame_stage) {

    if (frame_stage != e_client_frame_stage::FRAME_STAGE_RENDER_START)
        return;

    features::miscellaneous::skybox_changer(settings.visuals.world.skybox);
}

void util::play_sound(const char *file_path, int volume) {

    if (volume == -1)
        volume = settings.global.sound_fx_volume;

    char buffer[256] = {};
    sprintf_s(buffer, xs("playvol \"%s\" \"%s\""), file_path, std::to_string(static_cast<float>(volume) / 100.0f).c_str());

    interfaces::engine_client->execute_command(buffer);
}

void util::movement_fix(const vector_t &old_angles, c_user_cmd *user_cmd) {
    vector_t view_angles = {0.0f, old_angles.y, 0.0f};
    vector_t aim_angles = {0.0f, user_cmd->view_angles.y, 0.0f};

    vector_t view_forward, view_side;
    vector_t aim_forward, aim_side;

    math::angle_vectors(view_angles, &view_forward, &view_side);
    math::angle_vectors(aim_angles, &aim_forward, &aim_side);

    view_forward.normalize();
    view_side.normalize();

    vector_t forward_norm = view_forward * user_cmd->forward_move;
    vector_t side_norm = view_side * user_cmd->side_move;

    float new_forward = forward_norm.dot(aim_forward) + side_norm.dot(aim_forward);
    float new_side = forward_norm.dot(aim_side) + side_norm.dot(aim_side);

    user_cmd->forward_move = new_forward;
    user_cmd->side_move = new_side;
}

void util::disable_model_occlusion() {
    const static auto r_drawallrenderables = interfaces::convar_system->find_convar(xs("r_drawallrenderables"));

    if (r_drawallrenderables->get_int() != 1) {
        r_drawallrenderables->callbacks.clear();
        r_drawallrenderables->set_value(1);
    }
}

point_t util::screen_transform(const vector_t &world) {

    const auto screen_transform = [&](const vector_t &in, point_t &out) -> bool {
        const static auto &matrix = *(matrix4x4_t *) patterns::get_view_matrix();

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

c_player_resource *util::get_player_resource() {

    for (int i = 65; i < interfaces::entity_list->get_highest_ent_index(); ++i) {
        auto ent = (c_entity *) interfaces::entity_list->get_entity(i);

        if (!ent)
            continue;

        const auto cc = ent->get_networkable()->get_client_class();

        if (cc && cc->class_id == CCSPlayerResource) {
            return reinterpret_cast<c_player_resource *>(ent);
        }
    }

    return nullptr;
}

uintptr_t *util::find_hud_element(const char *name) {
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

std::optional<vector_t> util::get_intersection(const vector_t &start, const vector_t &end, const vector_t &mins, const vector_t &maxs, float radius) {

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

std::string util::sanitize_string(const std::string &str) {

    std::string ret = str;

    for (auto &it : ret) {
        switch (it) {
        case '"':
        case '\\':
        case ';':
        case '\n':
            it = ' ';
            break;
        default:
            break;
        }
    }

    return ret;
}

void util::undo() {

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
