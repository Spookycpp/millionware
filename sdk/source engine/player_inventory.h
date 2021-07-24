#pragma once

#include <vector>

#include "../engine/logging/logging.h"
#include "../engine/security/xorstr.h"

#include "../features/inventory changer/item_definitions.h"

#include "econ_item.h"
#include "macros.h"
#include "util_vector.h"

class CSharedObjectTypeCache {
  public:
    DECLARE_VFUNC(1, add_object(void *object), void(__thiscall *)(void *, void *))(object);
    DECLARE_VFUNC(3, remove_object(void *object), void(__thiscall *)(void *, void *))(object);

    std::vector<c_econ_item *> get_econ_items() {
        std::vector<c_econ_item *> ret;

        const uintptr_t *data = *reinterpret_cast<uintptr_t **>(this + 0x4);

        if (data) {
            const int size = *reinterpret_cast<int *>(this + 0x18);

            for (int i = 0; i < size; ++i) {
                ret.push_back(reinterpret_cast<c_econ_item *>(data[i]));
            }
        }

        return ret;
    }
};

class c_player_inventory;

class c_inventory_manager {
  public:
    /*c_player_inventory *get_local_player_inventory( )
    {
        static uintptr_t offset = pattern::find( fnv_( "client.dll" ), xorstr_( "8B 8B ? ? ? ? E8 ? ? ? ? 89 44 24 18" ) );

        if ( !offset ) {
            return nullptr;
        }

        static auto local_inventory_offset = *reinterpret_cast< uintptr_t * >( offset + 0x2 );

        if ( !local_inventory_offset ) {
            return nullptr;
        }

        return *reinterpret_cast< c_player_inventory ** >( uintptr_t( this ) + local_inventory_offset );
    }*/

    bool equip_item_in_loadout(const int team, const int slot, const uint64_t id) {
        static auto equip_item_in_loadout_addr = patterns::get_equip_item_in_loadout();
        static auto equip_item_in_loadout_fn = reinterpret_cast<bool(__thiscall *)(void *, int, int, uint64_t, bool)>(equip_item_in_loadout_addr);

        if (!equip_item_in_loadout_fn) {
            logging::info(xs("Failed to get EquipItemInLoadout"));
            return false;
        }

        return equip_item_in_loadout_fn(this, team, slot, id, true);
    }

    c_econ_item_view *find_or_create_reference_econ_item(const uint64_t id) {
        static auto find_or_create_reference_econ_addr = patterns::get_find_or_create_reference_econ_item();
        static auto find_or_create_reference_econ_item = reinterpret_cast<c_econ_item_view *(__thiscall *) (void *, int64_t)>(find_or_create_reference_econ_addr);

        if (!find_or_create_reference_econ_item) {
            logging::info(xs("Failed to get FindOrCreateReferenceEconItem"));
            return nullptr;
        }

        return find_or_create_reference_econ_item(this, id);
    }

    void clear_inventory_images() {
        static auto clear_inventory_images_addr = patterns::get_clear_inventory_images();
        static auto clear_inventory_images_fn = reinterpret_cast<void(__thiscall *)(void *)>(clear_inventory_images_addr);

        if (!clear_inventory_images_fn) {
            logging::info(xs("Failed to get econ_clear_inventory_images"));
            return;
        }

        clear_inventory_images_fn(this);
    }
};

class c_player_inventory {
  public:
    c_econ_item *add_item(const int index, const inventory_item_t &item, c_inventory_manager *manager) {
        if (item.definition <= 0 || item.paint_kit <= 0) {

            // ghetto check
            if (item.rarity > 0) 
                logging::info(xs("Failed to add item to inventory: {}"), index);

            return nullptr;
        }

        auto econ_item = create_econ_item();

        if (!econ_item) {
            logging::info(xs("Failed to create econ item"));
            return nullptr;
        }

        static int id = 20000;

        *econ_item->get_item_id() = 20000 + index;
        *econ_item->get_account_id() = this->get_steam_id();
        *econ_item->item_definition_index() = item.definition;
        *econ_item->get_inventory() = id++;
        *econ_item->get_flags() = 0;
        *econ_item->get_original_id() = 0;

        econ_item->set_origin(24);
        econ_item->set_level(1);
        econ_item->set_in_use(false);

        const bool is_item_knife = is_knife(item.definition);
        const bool is_item_glove = is_glove(item.definition);

        if (is_item_knife || is_item_glove) {
            econ_item->set_rarity(ITEM_RARITY_COVERT);
            econ_item->set_quality(ITEM_QUALITY_UNUSUAL);
        }
        else {
            econ_item->set_rarity(item.rarity);
            econ_item->set_quality(item.quality);

            int sticker_idx = 0;

            for (const auto &sticker : item.stickers) {
                if (sticker.kit > 0) {
                    econ_item->add_sticker(sticker_idx, sticker.kit, sticker.wear, sticker.scale, sticker.rotation);
                }

                sticker_idx++;
            }
        }

        if (!is_item_glove) {
            if (item.stat_trak > 0) {
                econ_item->set_stat_trak(item.stat_trak);
            }

            const std::string custom_name(item.custom_name);

            if (!custom_name.empty()) {
                econ_item->set_custom_name(custom_name.c_str());
            }
        }

        econ_item->set_paint_kit(float(item.paint_kit));
        econ_item->set_paint_seed(float(item.seed));
        econ_item->set_paint_wear(item.wear);

        return this->add_econ_item(econ_item, 1, 0, 1);
    }

    void remove_items(c_inventory_manager *manager, std::vector<c_econ_item *> &items) {
        const auto destroy_item = [&](c_econ_item *item) -> void {
            auto base_type_cache = this->base_type_cache();

            if (!base_type_cache) {
                logging::info(xs("Failed to get object cache"));
                return;
            }

            remove_item(*item->get_item_id());
            base_type_cache->remove_object(item);
        };

        for (auto &item : items) {
            if (!item) {
                continue;
            }

            auto item_view = this->get_inventory_item_by_item_id(*item->get_item_id());

            if (!item_view) {
                item_view = manager->find_or_create_reference_econ_item(*item->get_item_id());

                if (!item_view) {
                    continue;
                }
            }

            auto item_data = item_view->get_static_data();

            if (!item_data) {
                continue;
            }

            for (int i = 2; i <= 3; ++i) {
                manager->equip_item_in_loadout(i, item_data->get_loadout_slot(), 0);
            }

            destroy_item(item);
        }
    }

    void equip_item(c_inventory_manager *manager, c_econ_item *item, const int auto_equip) {
        if (!manager || !item || !auto_equip) {
            return;
        }

        const auto id = *item->get_item_id();

        if (id <= 0) {
            return;
        }

        auto item_view = this->get_inventory_item_by_item_id(id);

        if (!item_view) {
            item_view = manager->find_or_create_reference_econ_item(id);

            if (!item_view) {
                return;
            }
        }

        auto item_data = item_view->get_static_data();

        if (!item_data) {
            return;
        }

        switch (auto_equip) {
        case 1:
            manager->equip_item_in_loadout(2, item_data->get_loadout_slot(), id);
            break;
        case 2:
            manager->equip_item_in_loadout(3, item_data->get_loadout_slot(), id);
            break;
        case 3:
            manager->equip_item_in_loadout(2, item_data->get_loadout_slot(), id);
            manager->equip_item_in_loadout(3, item_data->get_loadout_slot(), id);
            break;
        default:
            break;
        }
    }

    c_econ_item_view *get_item_in_loadout(const int team, const int slot) {
        return util::get_method(this, 8).as<c_econ_item_view *(__thiscall *) (decltype(this), uint32_t, int)>()(this, team, slot);
    }

    CUtlVector<c_econ_item_view *> *get_inventory_items() {
        return reinterpret_cast<CUtlVector<c_econ_item_view *> *>(this + 0x2C);
    }

  private:
    void remove_item(const uint64_t id) {
        static auto remove_item_addr = patterns::get_remove_item();
        static auto remove_item_fn = reinterpret_cast<int(__thiscall *)(void *, int64_t)>(remove_item_addr);

        if (!remove_item_fn) {
            logging::info(xs("Failed to get RemoveItem"));
            return;
        }

        remove_item_fn(this, id);
    }

    CSharedObjectTypeCache *base_type_cache() {
        static auto find_shared_object_cache_addr = patterns::get_base_type_cache();
        static auto find_shared_object_cache_fn = reinterpret_cast<uintptr_t(__thiscall *)(uintptr_t, uint64_t, uint64_t, bool)>(find_shared_object_cache_addr);

        if (!find_shared_object_cache_fn) {
            logging::info(xs("Failed to get FindSharedObjectCache"));
            return nullptr;
        }

        static auto create_base_type_cache_addr = patterns::get_base_type_cache();
        static auto create_base_type_cache_fn = reinterpret_cast<CSharedObjectTypeCache *(__thiscall *) (uintptr_t, int)>(create_base_type_cache_addr);

        if (!create_base_type_cache_fn) {
            logging::info(xs("Failed to get CreateBaseTypeCache"));
            return nullptr;
        }

        static auto gc_client_system = **reinterpret_cast<uintptr_t **>(patterns::get_gc_client_system() + 0x2);

        if (!gc_client_system) {
            logging::info(xs("Failed to get GCClientSystem"));
            return nullptr;
        }

        const auto cache = find_shared_object_cache_fn(gc_client_system + 0x70, *reinterpret_cast<uint64_t *>(this + 0x8), *reinterpret_cast<uint64_t *>(this + 0x10), false);

        if (!cache) {
            logging::info(xs("Failed to create shared object cache"));
            return nullptr;
        }

        return create_base_type_cache_fn(cache, 1);
    }

    uint32_t get_steam_id() {
        return *reinterpret_cast<uint32_t *>(this + 0x8);
    }

    c_econ_item_view *get_inventory_item_by_item_id(const uint64_t id) {
        static auto get_inventory_item_by_item_id_addr = patterns::get_inventory_item_by_item_id();
        static auto get_inventory_item_by_item_id_fn = reinterpret_cast<c_econ_item_view *(__thiscall *) (void *, uint64_t)>(get_inventory_item_by_item_id_addr);

        if (!get_inventory_item_by_item_id_fn) {
            logging::info(xs("Failed to get GetInventoryItemByItemID"));
            return nullptr;
        }

        auto econ = get_inventory_item_by_item_id_fn(this, id);

        if (!econ || !*reinterpret_cast<uint8_t *>(uintptr_t(econ) + 0x204)) {
            return nullptr;
        }

        return econ;
    }

    c_econ_item *add_econ_item(c_econ_item *item, const int a3, const int a4, const char a5) {
        static auto add_econ_item_addr = patterns::get_add_econ_item();
        static auto add_econ_item_fn = reinterpret_cast<c_econ_item_view *(__thiscall *) (void *, c_econ_item *, int, int, char)>(add_econ_item_addr);

        if (!add_econ_item_fn) {
            logging::info(xs("Failed to get AddEconItem"));
            return nullptr;
        }

        auto base_type_cache = this->base_type_cache();

        if (!base_type_cache) {
            logging::info(xs("Failed to get object cache"));
            return nullptr;
        }

        base_type_cache->add_object(item);

        const auto ret = add_econ_item_fn(this, item, a3, a4, a5);

        if (ret) {
            c_econ_item_view *item_view = get_inventory_item_by_item_id(*item->get_item_id());

            if (item_view) {
                *reinterpret_cast<bool *>(uintptr_t(item_view) + 0xA1) = true;
            }
        }
        else {
            return nullptr;
        }

        return item;
    }
};