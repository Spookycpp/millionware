#include "util.hpp"
#include "../core/config.hpp"

float TICK_INTERVAL() {
	return interfaces::global_vars->interval_per_tick;
}

int TIME_TO_TICKS(const float dt) {
	return static_cast<int>(0.5f + dt / TICK_INTERVAL());
}

float TICKS_TO_TIME(const int tick) {
	return tick * TICK_INTERVAL();
}

float util::get_total_latency()
{
	c_net_channel_info* nci = interfaces::engine_client->get_net_channel_info();

	if (!nci) {
		return 0.0f;
	}

	return nci->get_latency(FLOW_OUTGOING) + nci->get_latency(FLOW_INCOMING);
}

float util::get_lerp_time()
{
	static auto cl_interpolate	 = interfaces::convar_system->find(STR_ENC("cl_interpolate"));
	static auto cl_interp		 = interfaces::convar_system->find(STR_ENC("cl_interpolate"));
	static auto cl_updaterate	 = interfaces::convar_system->find(STR_ENC("cl_interpolate"));
	static auto cl_interp_ratio	 = interfaces::convar_system->find(STR_ENC("cl_interpolate"));

	if (cl_interp && cl_interpolate && cl_updaterate && cl_interp_ratio)
	{
		if (cl_interpolate->get_int()) {
			return std::max(cl_interp->get_float(), cl_interp_ratio->get_float() / cl_updaterate->get_float());
		}
	}

	return 0.0f;
}

float util::get_random_float(const float min, const float max)
{
	using  random_float_t = float(*)(float, float);
	static random_float_t random_float_fn = (random_float_t)GetProcAddress(GetModuleHandleA(STR_ENC("vstdlib.dll")), STR_ENC("RandomFloat"));

	if (!random_float_fn)
		return 0.f;

	return random_float_fn(min, max);
}

bool util::run_frame() {
	return !!cheat::local_player;
}

bool util::line_goes_through_smoke(const vector3_t& src, const vector3_t& dst) {

	using       line_goes_through_smoke_t  = bool(*) (vector3_t, vector3_t, int16_t);
	static auto line_goes_through_smoke_fn = (line_goes_through_smoke_t)patterns::line_goes_through_smoke;

	if (!line_goes_through_smoke_fn)
		return false;

	return line_goes_through_smoke_fn(src, dst, 1);
}

bool util::intersects_hitbox(const vector3_t eye_pos, const vector3_t end_pos, const vector3_t min, const vector3_t max, const float radius)
{
	return math::dist_segment_to_segment(eye_pos, end_pos, min, max).length() < radius;
}

void util::set_random_seed(const int seed) {

	using  random_seed_t = int(__cdecl*)(int);
	static random_seed_t random_seed_fn = (random_seed_t)GetProcAddress(GetModuleHandleA(STR_ENC("vstdlib.dll")), STR_ENC("RandomSeed"));

	if (!random_seed_fn)
		return;

	random_seed_fn(seed);
}

void util::set_night_mode() {
	for (int i = 1; i <= interfaces::entity_list->get_highest_entity_index(); ++i)
	{
		auto controller = (c_env_tonemap_controller*)interfaces::entity_list->get_by_index(i);

		if (!controller) {
			continue;
		}

		client_class_t* cc = controller->networkable()->get_client_class();

		if (!cc || cc->class_id != cenvtonemapcontroller) {
			continue;
		}

		controller->use_exposure_min() = true;
		controller->use_exposure_max() = true;
		controller->get_exposure_min() = config::get<float>(FNV_CT("visuals.world.nightmode_intensity"));
		controller->get_exposure_max() = config::get<float>(FNV_CT("visuals.world.nightmode_intensity"));
	}
}

std::optional<vector3_t> util::get_intersection(const vector3_t& start, const vector3_t& end, const vector3_t& mins, const vector3_t& maxs, float radius)
{
	const auto sphere_ray_intersection = [start, end, radius](auto&& center) -> std::optional< vector3_t >
	{
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

	for (size_t i = 0; i < std::floor(mins.dist(maxs)); ++i)
	{
		if (const auto intersection = sphere_ray_intersection(mins + delta * float(i)); intersection) {
			return intersection;
		}
	}

	if (const auto intersection = sphere_ray_intersection(maxs); intersection) {
		return intersection;
	}

	return {};
}
