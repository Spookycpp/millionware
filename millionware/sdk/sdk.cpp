#include "../core/interfaces.hpp"
#include "../core/patterns.hpp"
#include "base_handle.hpp"

c_entity* c_base_handle::get() const {
  return interfaces::entity_list->get_by_handle(*this);
}

bool c_base_handle::operator ==(const c_base_handle& other) const {
  return handle == other.handle;
}

bool c_base_handle::operator ==(c_entity* entity) const {
  return get() == entity;
}

bool c_base_handle::operator !=(const c_base_handle& other) const {
  return handle != other.handle;
}

bool c_base_handle::operator !=(c_entity* entity) const {
  return get() != entity;
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
