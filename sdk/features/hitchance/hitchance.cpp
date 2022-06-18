#include "hitchance.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/util/util.h"

#include "../../engine/math/math.h"

#include <array>
#include <vector>

namespace features::hit_chance {

    std::array<hit_chance_data_t, 256> hit_chance_records = {};

    struct hitbox_data_t {
        hitbox_data_t(const vector_t &min, const vector_t &max, const float radius) {
            this->min = min;
            this->max = max;
            this->radius = radius;
        }

        vector_t min;
        vector_t max;
        float radius;
    };

    void initialize() {
        for (size_t i = 0; i < 256; ++i) {
            util::set_random_seed(i);

            const float rand_a = util::get_random_float(0.0f, 1.0f);
            const float rand_pi_a = util::get_random_float(0.0f, math::pi_2);
            const float rand_b = util::get_random_float(0.0f, 1.0f);
            const float rand_pi_b = util::get_random_float(0.0f, math::pi_2);

            hit_chance_records[i] = {std::make_pair(rand_a, rand_b), std::make_pair(std::cos(rand_pi_a), std::sin(rand_pi_b)),
                                     std::make_pair(std::cos(rand_pi_b), std::sin(rand_pi_a))};
        }
    }

    vector_t get_spread_direction(c_weapon *weapon, const vector_t &angles, int seed) {
        if (!weapon)
            return vector_t();

        const auto *data = &hit_chance_records[seed & 255];

        if (!data)
            return vector_t();

        auto [rand_a, rand_b] = data->random;

        if (weapon->get_item_definition_index() == WEAPON_NEGEV) {
            auto weapon_info = weapon ? interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index()) : nullptr;
            ;

            if (weapon_info && weapon_info->recoil_seed < 3) {
                rand_a = 1.0f - std::pow(rand_a, static_cast<float>(3 - weapon_info->recoil_seed + 1));
                rand_b = 1.0f - std::pow(rand_b, static_cast<float>(3 - weapon_info->recoil_seed + 1));
            }
        }

        const float rand_inaccuracy = rand_a * weapon->get_inaccuracy();
        const float rand_spread = rand_b * weapon->get_spread();

        const float spread_x = data->inaccuracy.first * rand_inaccuracy + data->spread.first * rand_spread;
        const float spread_y = data->inaccuracy.second * rand_inaccuracy + data->spread.second * rand_spread;

        vector_t forward, right, up;
        math::angle_to_vectors(angles, &forward, &right, &up);

        return forward + right * spread_x + up * spread_y;
    }

    std::vector<hitbox_data_t> get_hitbox_data(c_player *target, int hitbox) {
        std::vector<hitbox_data_t> hitbox_data;

        const auto model = target->get_renderable()->get_model();

        if (!model)
            return {};

        const auto hdr = interfaces::model_info->get_studio_model(model);

        if (!hdr)
            return {};

        const auto set = hdr->get_hitbox_set(target->get_hitbox_set());

        if (!set)
            return {};

        std::array<matrix3x4_t, 128> bone_matrix = {};
        std::memcpy(bone_matrix.data(), target->get_cached_bone_data().get_elements(),
                    target->get_cached_bone_data().count() * sizeof(matrix3x4_t));

        if (hitbox == -1) {
            for (int i = 0; i < set->hitbox_count; ++i) {
                const auto box = set->get_hitbox(i);

                if (!box)
                    continue;

                vector_t min = math::vector_transform(box->mins, bone_matrix[box->bone]);
                vector_t max = math::vector_transform(box->maxs, bone_matrix[box->bone]);

                float radius = box->radius;

                if (radius == -1.0f)

                    hitbox_data.emplace_back(hitbox_data_t{min, max, radius});
            }
        } else {
            const auto box = set->get_hitbox(hitbox);

            if (!box)
                return {};

            vector_t min = math::vector_transform(box->mins, bone_matrix[box->bone]);
            vector_t max = math::vector_transform(box->maxs, bone_matrix[box->bone]);

            float radius = box->radius;

            if (radius == -1.0f)
                radius = min.dist(max);

            hitbox_data.emplace_back(hitbox_data_t{min, max, radius});
        }

        return hitbox_data;
    }

    bool can_hit(c_player *target, c_weapon *weapon, const vector_t &angles, const int percentage, const int hitbox) {
        if (!target || !weapon)
            return false;

        auto weapon_info = weapon ? interfaces::weapon_system->get_weapon_info(weapon->get_item_definition_index()) : nullptr;
        ;

        if (!weapon_info)
            return false;

        const auto hitbox_data = get_hitbox_data(target, hitbox);

        if (hitbox_data.empty())
            return false;

        const vector_t eye_pos = cheat::local_player->get_eye_pos();

        const int hits_needed = (percentage * 256) / 100;
        int hits = 0;

        for (int i = 0; i < 256; ++i) {
            const vector_t end_pos = eye_pos + get_spread_direction(weapon, angles, i) * weapon_info->range;

            for (const auto &it : hitbox_data) {
                if (util::intersects_hitbox(eye_pos, end_pos, it.min, it.max, it.radius)) {
                    hits++;
                    break;
                }
            }

            if (hits >= hits_needed)
                return true;
        }

        return false;
    }

} // namespace features::hit_chance