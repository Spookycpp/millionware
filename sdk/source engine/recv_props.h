#pragma once

class c_recv_prop;
class c_recv_table;

using recv_var_proxy_fn = void (*)(void *, void *, void *);
using array_length_recv_proxy_fn = void (*)(void *, int, int);
using data_table_recv_var_proxy_fn = void (*)(c_recv_prop *, void **, void *, int);

enum class send_prop_type {
    DPT_Int = 0,
    DPT_Float,
    DPT_Vector,
    DPT_VectorXY, // Only encodes the XY of a vector, ignores Z
    DPT_String,
    DPT_Array, // An array of the base types (can't be of datatables).
    DPT_DataTable,
    DPT_Int64,
    DPT_NUMSendPropTypes
};

class c_recv_prop {
  public:
    const char *prop_name;
    send_prop_type prop_type;
    int prop_flags;
    int buffer_size;
    int inside_of_array;
    void *extra_data_ptr;
    c_recv_prop *array_prop;
    array_length_recv_proxy_fn array_length_proxy;
    recv_var_proxy_fn proxy;
    data_table_recv_var_proxy_fn data_table_proxy;
    c_recv_table *data_table;
    int offset;
    int element_stride;
    int elements_count;
    const char *parent_array_prop_name;
};

class c_recv_table {
  public:
    c_recv_prop *props;
    int props_count;
    void *decoder_ptr;
    const char *table_name;
    bool initialized;
    bool in_main_list;
};
