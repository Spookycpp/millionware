#include "../core/interfaces.hpp"
#include "../core/patterns.hpp"
#include "base_handle.hpp"
#include "material_handle.hpp"

c_entity* base_handle_t::get() const {
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
