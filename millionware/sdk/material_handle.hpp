#pragma once

struct material_handle_t {
  unsigned short handle;

  explicit constexpr material_handle_t(unsigned short handle)
    : handle(handle) {}

  struct material_t* get() const;

  bool operator ==(const material_handle_t& other) const;
  bool operator ==(struct material_t* material) const;

  bool operator !=(const material_handle_t& other) const;
  bool operator !=(struct material_t* material) const;
};
