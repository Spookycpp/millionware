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


//namespace lua_internal::entity {
//    inline c_entity *get_entity(const int idx, lua_State *l) {
//        return interfaces::entity_list->get_entity(idx);
//    }
//
//    inline int get_local_player(lua_State *l) {
//        if (!cheat::local_player) {
//            return 0;
//        }
//
//        return interfaces::engine_client->get_local_player();
//    }
//
//    inline int get_prop(lua_State *l) {
//        auto starts_with = [](const std::string &str, const std::string &match) {
//            return str.find(match) == 0;
//        };
//
//        if (!get_local_player(l)) {
//            lua_pushnil(l);
//            return 0;
//        }
//
//        c_entity *ent = get_entity(luaL_checkinteger(l, 1), l);
//        if (!ent) {
//            lua_pushnil(l);
//            return 0;
//        }
//
//        size_t len;
//        const char *table = luaL_checklstring(l, 2, &len);
//        const char *prop = luaL_checklstring(l, 3, &len);
//
//        const size_t offset = netvars::get(CRC(table, CRC(":", CRC(prop))));
//
//        if (starts_with(prop, XORSTR("m_b"))) {
//            lua_pushboolean(l, ent->get<bool>(offset));
//            return 1;
//        }
//
//        if (starts_with(prop, XORSTR("m_fl"))) {
//            lua_pushnumber(l, ent->get<float>(offset));
//            return 1;
//        }
//
//        lua_pushnil(l);
//        return 0;
//    }
//}

inline void lua_internal::context::entity() {
    luabridge::getGlobalNamespace(l)
    .beginClass<tables::entity>("entity")
        .addConstructor<void (*)(c_entity *entity, int idx)>()
        .addFunction("get_index", &tables::entity::index)
        .addFunction("is_alive", &tables::entity::alive)
        .addFunction("is_dormant", &tables::entity::dormant)
        .addFunction("get_origin", &tables::entity::origin)
        .addFunction("get_name", &tables::entity::name)
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