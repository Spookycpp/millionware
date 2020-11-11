#pragma once

#include <cstdint>

#include "typedefs.hpp"

enum class e_send_prop_type : int
{
  INT32 = 0,
  FLOAT,
  VECTOR3,
  VECTOR2,
  STRING,
  ARRAY,
  DATA_TABLE,
  INT64,
};

struct recv_prop_t
{
  const char* var_name;
  int recv_type;
  int flags;
  int string_buffer_size;
  bool inside_array;
  const void* extra_data;
  recv_prop_t* array_prop;
  array_length_recv_proxy_fn array_length_proxy;
  recv_var_proxy_fn proxy_fn;
  data_table_recv_var_proxy_fn data_table_proxy_fn;
  struct recv_table_t* data_table;
  int offset;
  int element_stride;
  int elements;
  const char* parent_array_prop_name;
};

struct recv_table_t
{
  recv_prop_t* props;
  int prop_count;
  uintptr_t decoder;
  const char* net_table_name;
  bool initialized;
  bool in_main_list;
};
