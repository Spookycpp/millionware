#pragma once

struct base_handle_t {
  unsigned long handle;

  explicit constexpr base_handle_t(unsigned long handle)
    : handle(handle) {}

  class c_entity* get() const;

  bool operator ==(const base_handle_t& other) const;
  bool operator ==(class c_entity* entity) const;

  bool operator !=(const base_handle_t& other) const;
  bool operator !=(class c_entity* entity) const;
};

constexpr static base_handle_t INVALID_HANDLE = base_handle_t(0xFFFFFFFF);
