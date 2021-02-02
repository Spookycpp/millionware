#include "../core/cheat.hpp"
#include "../core/interfaces.hpp"
#include "../core/patterns.hpp"

#include "../utils/util.hpp"

#include "base_handle.hpp"
#include "material_handle.hpp"

c_entity* base_handle_t::get() const {
	if (*this == INVALID_HANDLE)
		return nullptr;

	return interfaces::entity_list->get_by_handle(*this);
}

bool base_handle_t::operator ==(const base_handle_t& other) const {
	return handle == other.handle;
}

bool base_handle_t::operator ==(c_entity* entity) const {
	return get() == entity;
}

bool base_handle_t::operator !=(const base_handle_t& other) const {
	return handle != other.handle;
}

bool base_handle_t::operator !=(c_entity* entity) const {
	return get() != entity;
}

material_t* material_handle_t::get() const {
	return interfaces::material_system->get_material(*this);
}

bool material_handle_t::operator ==(const material_handle_t& other) const {
	return handle == other.handle;
}

bool material_handle_t::operator ==(struct material_t* material) const {
	return get() == material;
}

bool material_handle_t::operator !=(const material_handle_t& other) const {
	return handle != other.handle;
}

bool material_handle_t::operator !=(struct material_t* material) const {
	return get() != material;
}

c_player* c_entity::as_player() {
	return reinterpret_cast<c_player*>(this);
}

c_weapon* c_entity::as_weapon() {
	return reinterpret_cast<c_weapon*>(this);
}

bool c_player::has_bomb() const {
	using has_bomb_fn = bool(__thiscall*)(uintptr_t);

	return reinterpret_cast<has_bomb_fn>(patterns::player_has_bomb)(reinterpret_cast<uintptr_t>(this));
}

bool c_player::is_alive()
{
	return this->life_state() == LIFE_STATE_ALIVE;
}

bool c_player::is_enemy() const
{
	if (!this)
		return false;

	c_entity* player = (c_entity*)this;

	const static auto mp_teammates_are_enemies = interfaces::convar_system->find(STR_ENC("mp_teammates_are_enemies"));

	if (!mp_teammates_are_enemies)
		return player->team_num() != cheat::local_player->team_num();

	if (mp_teammates_are_enemies->get_int())
		return this != cheat::local_player;

	return player->team_num() != cheat::local_player->team_num();
}

bool c_player::is_valid(const bool check_alive)
{
	if (!this || this->networkable()->is_dormant() || !this->is_player()) {
		return false;
	}

	return check_alive ? this->is_alive() : !this->is_alive();
}

vector3_t c_player::get_eye_origin()
{
	return this->origin() + this->view_offset();
}

vector3_t c_player::get_hitbox_pos(const int idx)
{
	if (idx == -1) {
		return {};
	}

	std::array< matrix3x4_t, 128 > matrices = {};

	if (!this->renderable()->setup_bones(matrices.data(), matrices.size(), 0x100, interfaces::global_vars->current_time)) {
		return {};
	}

	const auto model = this->renderable()->get_model();

	if (!model) {
		return {};
	}

	const auto studio_hdr = interfaces::model_info->get_studio_model(model);

	if (!studio_hdr) {
		return {};
	}

	mstudiobbox_t* bbox = (mstudiobbox_t*)studio_hdr->hitbox(idx, 0);

	if (!bbox) {
		return {};
	}

	const matrix3x4_t& matrix = matrices[bbox->bone_index];
	const float modifier = bbox->radius != -1.0f ? bbox->radius : 0.0f;

	vector3_t min, max;
	math::vector_transform(bbox->bb_min - modifier, matrix, min);
	math::vector_transform(bbox->bb_max + modifier, matrix, max);

	return (min + max) * 0.5f;
}

float c_player::get_flash_time()
{
	static auto m_flFlashBangTime = *reinterpret_cast<uint32_t*>(patterns::flashbang_time + 4);

	if (!m_flFlashBangTime)
		return 0.f;

	return *reinterpret_cast<float*>(uintptr_t(this) + m_flFlashBangTime);
}

bool c_player::is_flashed()
{
	return get_flash_time() > interfaces::global_vars->current_time;
}

bool c_player::can_shoot()
{
	const auto wpn = (c_weapon*)active_weapon_handle().get();

	if (!wpn) {
		return false;
	}

	return can_shoot(wpn);
}

bool c_player::can_shoot(c_weapon* wpn)
{
	const float server_time = float(this->tick_base()) * interfaces::global_vars->interval_per_tick;

	if (wpn->next_attack() > server_time) {
		return false;
	}

	if ((wpn->item_definition_index() == WEAPON_FAMAS || wpn->item_definition_index() == WEAPON_GLOCK) && wpn->is_burst_mode() && wpn->burst_shots_remaining() > 0) {
		return false;
	}

	if (wpn->next_primary_attack() > server_time) {
		return false;
	}

	if (wpn->item_definition_index() == WEAPON_REVOLVER && wpn->ready_time() > server_time) {
		return false;
	}

	return true;
}

vector3_t c_player::extrapolate_position(const vector3_t& pos)
{
	return pos + this->velocity() * TICK_INTERVAL();
}

bool c_player::is_visible(c_player* local, const vector3_t& src, const vector3_t& dst)
{
	c_trace_filter filter;

	if (local) {
		filter.skip = local;
	}

	trace_t tr;
	interfaces::engine_trace->trace_ray({ src, dst }, MASK_SHOT, &filter, &tr);

	return (tr.hit_ent == this || tr.fraction > 0.99f);
}