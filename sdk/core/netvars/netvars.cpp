#include <fstream>
#include <unordered_map>

#include "../../engine/hash/hash.h"
#include "../../engine/security/xorstr.h"
#include "../exports/exports.h"
#include "../interfaces/interfaces.h"
#include "netvars.h"

static std::unordered_map<uint32_t, uint32_t> netvars_map;
static std::unordered_map<uint32_t, send_prop_type> prop_types_map;
static std::ofstream netvar_dump;

void dump_netvar_table(c_recv_table *table, int child_offset = 0) {
    auto join_string = (const char *) xs(":");

    for (auto i = 0; i < table->props_count; i++) {
        const auto current_prop = &table->props[i];
        const auto current_child = current_prop->data_table;

        if (current_child != nullptr && current_child->props_count > 0 &&
            *(short *) current_child->table_name == 0x5444 /* ASCII for "DT" */)
            dump_netvar_table(current_child, child_offset + current_prop->offset);

        const auto hash = CRC(current_prop->prop_name, CRC(join_string, CRC(table->table_name)));

        if (netvars_map.find(hash) == netvars_map.end()) {
#ifdef _DEBUG
            char buffer[128];

            sprintf_s(buffer, "%s:%s = 0x%04x \n", table->table_name, current_prop->prop_name, child_offset + current_prop->offset);

            netvar_dump << buffer;
#endif
            prop_types_map[hash] = current_prop->prop_type;
            netvars_map[hash] = child_offset + current_prop->offset;
        }
    }
}

void netvars::init() {
    //#ifdef _DEBUG
    // netvar_dump.open("./mw_netvars.txt");

    const auto client_data = interfaces::client_dll->get_all_classes();

    for (auto data = client_data; data != nullptr; data = data->next) {
        dump_netvar_table(data->table);
    }

    // netvar_dump.close();
    //#else
    // for (auto entry = g_netvar_tables; entry; entry = entry->next)
    //{
    //	dump_netvar_table((c_recv_table *) entry->offset);
    //}
    //#endif
}

send_prop_type netvars::get_type_from_netvar(const uint32_t hash) {
    const auto it = prop_types_map.find(hash);
    if (it == prop_types_map.end()) {
        return send_prop_type::DPT_NUMSendPropTypes; // invalid hash
    }

    return it->second;
}

uint32_t netvars::get(uint32_t hash) {
    return netvars_map[hash];
}
