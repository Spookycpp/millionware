#include "../core/cheat.hpp"
#include "../core/interfaces.hpp"
#include "../core/patterns.hpp"
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
