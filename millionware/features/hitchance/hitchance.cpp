#include "hitchance.hpp"

#include "../../utils/util.hpp"

namespace features::hit_chance {

	std::array<hit_chance_data_t, 256> hit_chance_records = {};

	struct hitbox_data_t
	{
		hitbox_data_t(const vector3_t& min, const vector3_t& max, const float radis) {
			this->min = max;
			this->max = max;
			this->radius = radius;
		}

		vector3_t min;
		vector3_t max;
		float radius;
	};

	void initialize() {
		for (size_t i = 0; i < 256; ++i) {
			util::set_random_seed(i);

			const float rand_a = util::get_random_float(0.0f, 1.0f);
			const float rand_pi_a = util::get_random_float(0.0f, math::pi_2);
			const float rand_b = util::get_random_float(0.0f, 1.0f);
			const float rand_pi_b = util::get_random_float(0.0f, math::pi_2);

			hit_chance_records[i] = {
				std::make_pair(rand_a, rand_b),
				std::make_pair(std::cos(rand_pi_a), std::sin(rand_pi_b)),
				std::make_pair(std::cos(rand_pi_b), std::sin(rand_pi_a))
			};
		}
	}

	vector3_t get_spread_direction(c_weapon* wpn, const vector3_t& angles, int seed) {
		if (!wpn) {
			return vector3_t();
		}

		const auto* data = &hit_chance_records[seed & 255];

		if (!data) {
			return vector3_t();
		}

		float rand_a = data->random.first;
		float rand_b = data->random.second;

		if (wpn->item_definition_index() == WEAPON_NEGEV)
		{
			auto wpn_info = wpn ? interfaces::weapon_system->get_weapon_info(wpn->item_definition_index()) : nullptr;;

			if (wpn_info)
			{
				const int recoil_seed = *reinterpret_cast<int*>(uintptr_t(wpn_info) + 0x184); // not sure if correct

				if (recoil_seed < 3)
				{
					rand_a = 1.0f - std::pow(rand_a, static_cast<float>(3 - recoil_seed + 1));
					rand_b = 1.0f - std::pow(rand_b, static_cast<float>(3 - recoil_seed + 1));
				}
			}
		}

		const float rand_inaccuracy = rand_a * cheat::local_player->get_inaccuracy();
		const float rand_spread		= rand_b * cheat::local_player->get_spread();

		const float spread_x = data->inaccuracy.first * rand_inaccuracy + data->spread.first * rand_spread;
		const float spread_y = data->inaccuracy.second * rand_inaccuracy + data->spread.second * rand_spread;

		vector3_t forward, right, up;
		math::angle_to_vectors(angles, &forward, &right, &up);

		return forward + right * spread_x + up * spread_y;
	}

	float calculate_minimum_accuracy()
	{
		// calculate spread inaccuracy
		return 0.0f;
	}

	std::vector< hitbox_data_t > get_hitbox_data(c_player* target, int hitbox)
	{
		std::vector< hitbox_data_t > hitbox_data;

		const auto model = target->renderable()->get_model();

		if (!model) {
			return {};
		}

		const auto hdr = interfaces::model_info->get_studio_model(model);

		if (!hdr) {
			return {};
		}

		const auto set = hdr->hitbox_set(target->hitbox_set());

		if (!set) {
			return {};
		}

		std::array< matrix3x4_t, 128 > bone_matrix = {};
		std::memcpy(bone_matrix.data(), target->get_cached_bone_data().get_elements(), target->get_cached_bone_data().count() * sizeof(matrix3x4_t));

		if (hitbox == -1)
		{
			for (int i = 0; i < set->hitbox_count; ++i)
			{
				const auto box = set->hitbox(i);

				if (!box) {
					continue;
				}

				vector3_t min = math::vector_transform(box->mins, bone_matrix[box->bone]);
				vector3_t max = math::vector_transform(box->maxs, bone_matrix[box->bone]);

				float radius = box->radius;

				if (radius == -1.0f) {
					radius = min.dist(max);
				}

				hitbox_data.emplace_back(min, max, radius);
			}
		}
		else
		{
			const auto box = set->hitbox(hitbox);

			if (!box) {
				return {};
			}

			vector3_t min = math::vector_transform(box->mins, bone_matrix[box->bone]);
			vector3_t max = math::vector_transform(box->maxs, bone_matrix[box->bone]);

			float radius = box->radius;

			if (radius == -1.0f) {
				radius = min.dist(max);
			}

			hitbox_data.emplace_back(min, max, radius);
		}

		return hitbox_data;
	}

	bool can_hit(c_player* target, c_weapon* wpn, const vector3_t& angles, const int percentage, const int hitbox)
	{
		if (!target || !wpn) {
			return false;
		}

		auto wpn_info = wpn ? interfaces::weapon_system->get_weapon_info(wpn->item_definition_index()) : nullptr;;

		if (!wpn_info) {
			return false;
		}

		const auto hitbox_data = get_hitbox_data(target, hitbox);

		if (hitbox_data.empty()) {
			return false;
		}

		const vector3_t eye_pos = cheat::local_player->eye_angles();

		const int hits_needed = percentage * 256 / 100;
		int       hits = 0;

		for (int i = 0; i < 256; ++i)
		{
			const vector3_t end_pos = eye_pos + get_spread_direction(wpn, angles, i) * wpn_info->range;

			for (const auto& it : hitbox_data)
			{
				if (util::intersects_hitbox(eye_pos, end_pos, it.min, it.max, it.radius))
				{
					hits++;
					break;
				}
			}

			if (hits >= hits_needed) {
				return true;
			}
		}

		return false;
	}

}