#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "../core/interfaces/interfaces.h"
#include "../engine/math/math.h"

#include "entity.h"
#include <vector>

c_entity *entity_handle_t::get() const
{
	if (this->handle == 0xFFFFFFFF)
		return nullptr;

	return interfaces::entity_list->get_entity_from_handle(this->handle);
}

bool entity_handle_t::operator ==(const entity_handle_t &other) const
{
	return handle == other.handle;
}

bool entity_handle_t::operator ==(c_entity *entity) const
{
	return get() == entity;
}

bool entity_handle_t::operator !=(const entity_handle_t &other) const
{
	return handle != other.handle;
}

bool entity_handle_t::operator !=(c_entity *entity) const
{
	return get() != entity;
}

CUtlVector<c_animation_layer>& c_player::animation_overlay() {
	const static auto offset = netvars::get(CRC_CT("DT_BaseAnimating:m_hLightingOrigin"));

	return *(CUtlVector<c_animation_layer>*)((uintptr_t)this + offset + 0x3c);
}

CUtlVector<matrix3x4_t>& c_player::get_cached_bone_data() {
	return *(CUtlVector<matrix3x4_t>*) ((uintptr_t)this + 0x290C + sizeof(void*));
}

void c_player::set_abs_angles(const vector_t& angle) {
	if (!this) //-V704
		return;

	using Fn = void(__thiscall*)(void*, const vector_t&);
	static auto fn = reinterpret_cast<Fn>(patterns::set_abs_angles);

	return fn(this, angle);
}

vector_t c_player::get_eye_pos() const {
	return get_vec_origin() + get_view_offset();
}

vector_t c_player::extrapolate_position(const vector_t& pos) {
	return pos + this->get_velocity() * TICK_INTERVAL();
}

vector_t c_player::get_hitbox_pos(const int idx) {

	if (idx == -1) {
		return {};
	}

	matrix3x4_t matrices[128];

	if (!get_renderable()->setup_bones(matrices, 128, 0x100, interfaces::global_vars->current_time)) {
		return {};
	}

	const auto model = get_renderable()->get_model();

	if (!model) {
		return {};
	}

	const auto studio_hdr = interfaces::model_info->get_studio_model(model);

	if (!studio_hdr) {
		return {};
	}

	const auto bbox = studio_hdr->get_hitbox(idx, 0);

	if (!bbox) {
		return {};
	}

	const matrix3x4_t& matrix = matrices[bbox->bone];
	const float modifier = bbox->radius != -1.0f ? bbox->radius : 0.0f;

	const auto min = math::vector_transform(bbox->mins - modifier, matrix);
	const auto max = math::vector_transform(bbox->maxs + modifier, matrix);

	return (min + max) * 0.5f;
}

float c_player::get_flash_time() {
	return *reinterpret_cast<float*>(uintptr_t(this) + patterns::get_flash_time());
}

int c_player::sequence_activity(int sequence) {
	const auto model = this->get_renderable()->get_model();
	if (!model) {
		return -1;
	}

	const auto hdr = interfaces::model_info->get_studio_model(model);
	if (!hdr) {
		return -1;
	}

	return reinterpret_cast<int(__fastcall*)(void*, studio_hdr_t*, int)>(patterns::get_sequence_activity)(this, hdr, sequence);
}

bool c_player::is_flashed() {
	return get_flash_time() > interfaces::global_vars->current_time;
}

bool c_player::can_shoot() {
	const auto weapon = (c_weapon*)get_active_weapon_handle().get();

	if (!weapon) {
		return false;
	}

	return can_shoot(weapon);
}

bool c_player::can_shoot(c_weapon* weapon) {
	const float server_time = float(this->get_tick_base()) * interfaces::global_vars->interval_per_tick;

	if (weapon->get_next_attack() > server_time) {
		return false;
	}

	if ((weapon->get_item_definition_index() == WEAPON_FAMAS || weapon->get_item_definition_index() == WEAPON_GLOCK) && weapon->get_is_burst_mode() && weapon->get_burst_shots_remaining() > 0) {
		return false;
	}

	if (weapon->get_next_primary_attack() > server_time) {
		return false;
	}

	if (weapon->get_item_definition_index() == WEAPON_REVOLVER && weapon->get_ready_time() > server_time) {
		return false;
	}

	return true;
}

bool c_player::is_alive() {
	return this->get_life_state() == LIFE_STATE_ALIVE;
}

bool c_player::is_visible(c_player* local, const vector_t& src, const vector_t& dst) {
	c_trace_filter filter;

	if (local) {
		filter.skip = local;
	}

	trace_t tr;
	interfaces::trace->trace_ray({ src, dst }, MASK_SHOT, &filter, &tr);

	return (tr.hit_ent == this || tr.fraction > 0.99f);
}

bool c_player::is_visible(c_player* local, const vector_t& src) {
	bool ret = false;

	matrix3x4_t matrices[128];

	if (!get_renderable()->setup_bones(matrices, 128, 0x100, interfaces::global_vars->current_time)) {
		return false;
	}

	const auto model = this->get_renderable()->get_model();

	if (!model) {
		return false;
	}

	const auto studio_hdr = interfaces::model_info->get_studio_model(model);

	if (!studio_hdr) {
		return false;
	}

	static std::vector< int > hitboxes = {
		HEAD,
		R_CALF, L_CALF,
		R_FOOT, L_FOOT,
		R_HAND, L_HAND,
		R_UPPERARM, R_FOREARM,
	};

	for (const auto& it : hitboxes) {
		const auto bbox = studio_hdr->get_hitbox(it, 0);

		if (!bbox) {
			continue;
		}

		const matrix3x4_t& matrix = matrices[bbox->bone];
		const float modifier = bbox->radius != -1.0f ? bbox->radius : 0.0f;

		const auto min = math::vector_transform(bbox->mins - modifier, matrix);
		const auto max = math::vector_transform(bbox->maxs + modifier, matrix);

		const vector_t dst = (min + max) * 0.5f;

		c_trace_filter filter;

		if (local) {
			filter.skip = local;
		}

		trace_t tr;
		interfaces::trace->trace_ray({ src, dst }, MASK_SHOT, &filter, &tr);

		if (tr.hit_ent == this || tr.fraction > 0.99f) {
			ret = true;
			break;
		}
	}

	return ret;
}

bool c_player::is_reloading() {
	const auto& reload_layer = animation_overlay().Element(1);
	if (reload_layer.owner) {
		const int activity = sequence_activity(reload_layer.sequence);

		return activity == 967 && reload_layer.weight != 0.f;
	}

	return false;
}

bool c_player::is_valid(const bool check_alive) {
	if (!this || this->get_networkable()->is_dormant() || !this->is_player()) {
		return false;
	}

	return check_alive ? this->is_alive() : !this->is_alive();
}