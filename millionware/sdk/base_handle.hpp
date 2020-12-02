#pragma once

class c_entity;

class c_base_handle {
public:
  explicit constexpr c_base_handle(unsigned long handle)
    : handle(handle) {}

  unsigned long handle;

  c_entity* get() const;

  bool operator ==(const c_base_handle& other) const;
  bool operator ==(c_entity* entity) const;

  bool operator !=(const c_base_handle& other) const;
  bool operator !=(c_entity* entity) const;
};

constexpr static c_base_handle INVALID_HANDLE = c_base_handle(0xFFFFFFFF);
