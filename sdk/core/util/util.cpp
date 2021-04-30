#include "util.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"
#include "../../core/patterns/patterns.h"
#include "../../core/settings/settings.h"

#include "../../engine/security/xorstr.h"
#include "../../engine/math/math.h"


#include "../../features/miscellaneous/miscellaneous.h"

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
	c_net_channel_info* nci = interfaces::engine_client->get_net_channel_info();

	if (!nci) {
		return 0.0f;
	}

	return nci->get_latency(FLOW_OUTGOING) + nci->get_latency(FLOW_INCOMING);
}

float util::get_lerp_time() {
	static auto cl_interpolate  = interfaces::convar_system->find_convar(XORSTR("cl_interpolate"));
	static auto cl_interp	    = interfaces::convar_system->find_convar(XORSTR("cl_interp"));
	static auto cl_updaterate   = interfaces::convar_system->find_convar(XORSTR("cl_updaterate"));
	static auto cl_interp_ratio = interfaces::convar_system->find_convar(XORSTR("cl_interp_ratio"));

	if (cl_interp && cl_interpolate && cl_updaterate && cl_interp_ratio) {
		if (cl_interpolate->get_int()) {
			return std::max(cl_interp->get_float(), cl_interp_ratio->get_float() / cl_updaterate->get_float());
		}
	}

	return 0.0f;
}

float util::get_random_float(const float min, const float max) {
	using  random_float_t = float(*)(float, float);
	static random_float_t random_float_fn = (random_float_t)GetProcAddress(GetModuleHandleA(XORSTR("vstdlib.dll")), XORSTR("RandomFloat"));

	if (!random_float_fn)
		return 0.f;

	return random_float_fn(min, max);
}

bool util::run_frame() {
	return cheat::local_player != nullptr;
}

bool util::line_goes_through_smoke(const vector_t& src, const vector_t& dst) {

	using       line_goes_through_smoke_t = bool(*) (vector_t, vector_t, int16_t);
	static auto line_goes_through_smoke_fn = (line_goes_through_smoke_t)patterns::line_goes_through_smoke;

	if (!line_goes_through_smoke_fn)
		return false;

	return line_goes_through_smoke_fn(src, dst, 1);
}

bool util::intersects_hitbox(const vector_t eye_pos, const vector_t end_pos, const vector_t min, const vector_t max, const float radius) {
	return math::dist_segment_to_segment(eye_pos, end_pos, min, max).length() < radius;
}

void util::set_random_seed(const int seed) {

	using  random_seed_t = int(__cdecl*)(int);
	static random_seed_t random_seed_fn = (random_seed_t)GetProcAddress(GetModuleHandleA(XORSTR("vstdlib.dll")), XORSTR("RandomSeed"));

	if (!random_seed_fn)
		return;

	random_seed_fn(seed);
}

void util::set_skybox(const char* name) {
	// get load_named_sky func address
	static auto load_named_sky_addr = patterns::load_named_sky;
	static auto load_named_sky_fn = reinterpret_cast<void(__fastcall*)(const char*)>(load_named_sky_addr);

	// set it.
	load_named_sky_fn(name);
}

void util::on_frame_stage_notify(const e_client_frame_stage frame_stage) {
	if (frame_stage != e_client_frame_stage::FRAME_STAGE_RENDER_START)
		return;

	features::miscellaneous::skybox_changer(settings.visuals.world.skybox);
}

void util::play_sound(const char* file_path, int volume) {

	if (volume == -1)
		volume = settings.global.sound_fx_volume;

	char buffer[256] = {};
	sprintf_s(buffer, XORSTR("playvol \"%s\" \"%s\""), file_path, std::to_string(static_cast<float>(volume) / 100.0f).c_str());

	interfaces::engine_client->execute_command(buffer);
}

c_player_resource* util::get_player_resource() {
	for (int i = 65; i < interfaces::entity_list->get_highest_ent_index(); ++i) {
		auto ent = (c_entity*)interfaces::entity_list->get_entity(i);

		if (!ent)
			continue;

		const auto cc = ent->get_networkable()->get_client_class();

		if (cc && cc->class_id == CCSPlayerResource) {
			return reinterpret_cast<c_player_resource*>(ent);
		}
	}

	return nullptr;
}

std::optional<vector_t> util::get_intersection(const vector_t& start, const vector_t& end, const vector_t& mins, const vector_t& maxs, float radius) {
	const auto sphere_ray_intersection = [start, end, radius](auto&& center) -> std::optional< vector_t > {
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

std::string util::sanitize_string(const std::string& str) {
	std::string ret = str;

	for (auto& it : ret) {
		switch (it) {
			case '"': case '\\': case ';': case '\n':
				it = ' ';
				break;
			default:
				break;
		}
	}

	return ret;
}
