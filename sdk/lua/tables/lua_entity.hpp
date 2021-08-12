#pragma once

#include "../lua_internal.hpp"

#include "../../engine/hash/hash.h"
#include "../../source engine/entity.h"

#include "../../core/cheat/cheat.h"
#include "../../core/interfaces/interfaces.h"

namespace lua_internal::tables {
    class entity {
      private:
        c_entity *m_entity;
        int m_idx;

      public:
        entity(c_entity *entity, const int idx)
            : m_entity(entity), m_idx(idx) { }

        int index() const {
            if (!m_entity) {
                return 0;
            }

            return m_idx;
        }

        bool dormant() const {
            if (!m_entity) {
                return false;
            }

            return m_entity->get_networkable()->is_dormant();
        }

        bool alive() const {
            if (!m_entity) {
                return false;
            }

            return reinterpret_cast<c_player *>(m_entity)->is_alive();
        }

        vec3d origin(lua_State *l) {
            if (!m_entity) {
                return {};
            }

            const vector_t origin = m_entity->get_abs_origin();
            return {origin.x, origin.y, origin.z};
        }

        std::string name() const {
            if (!m_entity) {
                return {};
            }

            player_info_t info{};

            if (!interfaces::engine_client->get_player_info(m_idx, info)) {
                return {};
            }

            return info.name;
        }

        luabridge::LuaRef get_prop(lua_State *l) {
            luabridge::LuaRef table = luabridge::newTable(l);

            if (!m_entity) {
                return table;
            }

            size_t len;

            const uint32_t table_prop_hash = CRC(luaL_checklstring(l, 3, &len), CRC(":", CRC(luaL_checklstring(l, 2, &len))));
            const size_t offset            = netvars::get(table_prop_hash);
            const send_prop_type type      = netvars::get_type_from_netvar(table_prop_hash);

            switch (type)
            {
            case send_prop_type::DPT_Int: {
                table = m_entity->get<int>(offset);
            } break;

            case send_prop_type::DPT_Float: {
                table = m_entity->get<float>(offset);
            } break;

            case send_prop_type::DPT_Vector: {
                const auto v = m_entity->get<vector_t>(offset);
                table = vec3d{ v.x, v.y, v.z };
            } break;

            case send_prop_type::DPT_VectorXY: {
                const auto v = m_entity->get<point_t>(offset);
                table = vec2d{ v.x, v.y };
            } break;

            case send_prop_type::DPT_String: {
                table = m_entity->get_str(offset);
            } break;

            // do these ever get used?
            case send_prop_type::DPT_DataTable:
            case send_prop_type::DPT_Array: {
                // not sure if this will cause issues for DPT_Array,
                // but should be fine for DPT_DataTable as it's just a pointer
                table = m_entity->get<uint32_t>(offset);
            } break;

            case send_prop_type::DPT_Int64: {
                table = m_entity->get<int64_t>(offset);
            } break;

            default: break;
            }

            table.push();
            return table;
        }
    };

    class entity_list {
      private:

      public:
        static luabridge::LuaRef get_all(lua_State *l) {
            size_t len;
            const char *class_name = luaL_checklstring(l, 1, &len);

            luabridge::LuaRef table = luabridge::newTable(l);

            for (auto i = 0; i < interfaces::entity_list->get_highest_ent_index(); i++) {
               c_entity *entity = interfaces::entity_list->get_entity(i);
                if (!entity) {
                    continue;
                }

                c_client_class *client_class = entity->get_networkable()->get_client_class();
                if (!client_class) {
                    continue;
                }

                if (strcmp(client_class->class_name, class_name) != 0) {
                    continue;
                }

                table[i] = tables::entity(entity, i);
            }

            table.push();
            return table;
        }

        static entity get_local_player(lua_State *l) {
            if (!cheat::local_player) {
                return entity(nullptr, 0);
            }

            return entity(cheat::local_player, cheat::local_player->get_networkable()->index());
        }
    };
}

inline void lua_internal::context::entity() {
    luabridge::getGlobalNamespace(l)
    .beginClass<tables::entity>("entity")
        .addConstructor<void (*)(c_entity *entity, int idx)>()
        .addFunction("get_index", &tables::entity::index)
        .addFunction("is_alive", &tables::entity::alive)
        .addFunction("is_dormant", &tables::entity::dormant)
        .addFunction("get_origin", &tables::entity::origin)
        .addFunction("get_name", &tables::entity::name)
        .addFunction("get_prop", &tables::entity::get_prop)
    .endClass();

    luabridge::getGlobalNamespace(l)
    .beginNamespace("entity_list")
        .addFunction("get_all", std::function([this]() {
            return tables::entity_list::get_all(l);
        }))
        .addFunction("get_local_player", std::function([this]() {
            return tables::entity_list::get_local_player(l);
        }))
    .endNamespace();
}