#include "lagcompensation.h"
#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/settings/settings.h"
#include "../../core/util/util.h"

#include "../../source engine/client_dll.h"
#include "../../source engine/input.h"
#include "../../source engine/matrix.h"

#include "../../engine/math/math.h"

#include "../movement/engine prediction/engine_prediction.h"
#include <algorithm>

namespace features::lag_compensation {
    std::array<std::deque<render_record_t>, 65> records = {};

    std::array<float, 65> last_sim_time = {};

    void on_frame_stage_notify(const e_client_frame_stage frame_stage) {
        if (frame_stage != e_client_frame_stage::FRAME_STAGE_RENDER_END)
            return;

        store_visuals();
    }

    void store_visuals() {
        if (cheat::local_player->get_life_state() != LIFE_STATE_ALIVE)
            return;

        for (int i = 1; i < 65; ++i) {
            const auto ent = (c_player *) interfaces::entity_list->get_entity(i);

            if (ent == cheat::local_player)
                continue;

            auto &data = records[i];

            if (!ent || ent->get_has_gun_game_immunity()) {
                data.clear();
                continue;
            }

            if (std::abs(last_sim_time[i] - ent->get_simulation_time()) != 0.0f) {
                render_record_t new_record = {};
                new_record.simulation_time = ent->get_simulation_time() /*+ util::get_lerp_time( )*/;
                new_record.origin = ent->get_vec_origin();
                new_record.global_time = interfaces::global_vars->current_time /*- util::get_lerp_time( )*/;
                new_record.valid_time = 0.0f;

                std::array<matrix3x4_t, 128> matrices = {};
                ent->get_renderable()->setup_bones(matrices.data(), 128, BONE_USED_BY_ANYTHING, 0.0f);

                for (auto &matrix : matrices) {
                    const vector_t origin = ent->get_vec_origin();
                    vector_t delta = math::get_matrix_position(matrix) - origin;

                    math::set_matrix_position(delta + origin, matrix);
                }

                std::memcpy(new_record.matrices.data(), matrices.data(), sizeof(matrices));

                data.push_front(new_record);

                last_sim_time[i] = ent->get_simulation_time();
            }

            while (!data.empty() && int(data.size()) > TIME_TO_TICKS(1.0f))
                data.pop_back();
        }
    }

    bool get_render_record(const int idx, matrix3x4_t *out) {
        const auto is_time_valid = [&](const float time) {
            if (!settings_lbot)
                return false;

            const c_net_channel_info *nci = interfaces::engine_client->get_net_channel_info();

            if (!nci)
                return false;

            const float correct = std::clamp(util::get_total_latency(), 0.0f, 0.2f);
            const float delta = correct - (interfaces::global_vars->current_time - time);

            if (settings.miscellaneous.fake_ping.enabled)
                return std::abs(delta) < 0.150f;

            const float backtrack_time = settings_lbot->backtrack.enabled ? static_cast<float>(settings_lbot->backtrack.time)
                                                                          : static_cast<float>(settings_lbot->triggerbot.backtrack.time);

            return std::abs(delta) <= backtrack_time * 0.001f;
        };

        if (!cheat::local_player || cheat::local_player->get_life_state() != LIFE_STATE_DEAD)
            return false;

        auto ent = (c_player *) interfaces::entity_list->get_entity(idx);

        if (!ent)
            return false;

        auto &data = records[idx];

        for (auto it = data.rbegin(); it != data.rend(); ++it) {
            if (it + 1 != data.rend() && !is_time_valid(it->simulation_time) &&
                (it + 1)->simulation_time != ent->get_simulation_time() + util::get_lerp_time())
                continue;

            if (it->origin.dist(ent->get_vec_origin()) < 1.0f)
                return false;

            // if (features::engine_prediction::is_breaking_lc(ent->get_networkable()->index()))
            //	return false;

            const auto next_record = it + 1;

            if (static_cast<int>(std::abs(it->simulation_time - ent->get_simulation_time())) > TIME_TO_TICKS(16))
                continue;

            if (it->valid_time <= 0.0f)
                it->valid_time = interfaces::global_vars->current_time;

            const float choke = ent->get_simulation_time() - ent->get_old_simulation_time();

            float lerp_time = next_record == data.rend() ? choke : next_record->simulation_time - it->simulation_time;
            lerp_time = std::max(lerp_time, TICK_INTERVAL());

            float delta = (interfaces::global_vars->current_time - it->valid_time) / lerp_time;
            delta = std::clamp(delta, 0.0f, 1.0f);

            if (lerp_time > 0.075f)
                delta = math::ease_out(0.0f, 1.0f, delta);

            const vector_t next = it + 1 == data.rend() ? ent->get_vec_origin() : (it + 1)->origin;
            const vector_t lerp = math::lerp(it->origin, next, delta);

            std::array<matrix3x4_t, 128> ret = {};
            std::memcpy(ret.data(), it->matrices.data(), sizeof(ret));

            for (int i = 0; i < 128; ++i) {
                vector_t matrix_delta = math::get_matrix_position(it->matrices[i]) - it->origin;
                math::set_matrix_position(matrix_delta + lerp, ret[i]);
            }

            std::memcpy(out, ret.data(), sizeof(ret));

            return true;
        }

        return false;
    }

    std::deque<render_record_t> &get_raw_render_record(const int idx) {
        return records[idx];
    }
} // namespace features::lag_compensation