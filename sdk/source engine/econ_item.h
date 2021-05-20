#pragma once

#include <cstdint>
#include <vector>

#include "../core/patterns/patterns.h"

#pragma once

enum item_quality {
    ITEM_QUALITY_DEFAULT,
    ITEM_QUALITY_GENUINE,
    ITEM_QUALITY_VINTAGE,
    ITEM_QUALITY_UNUSUAL,
    ITEM_QUALITY_SKIN,
    ITEM_QUALITY_COMMUNITY,
    ITEM_QUALITY_DEVELOPER,
    ITEM_QUALITY_SELF_MADE,
    ITEM_QUALITY_CUSTOMIZED,
    ITEM_QUALITY_STRANGE,
    ITEM_QUALITY_COMPLETED,
    ITEM_QUALITY_UNK2,
    ITEM_QUALITY_TOURNAMENT
};

enum item_rarity { 
    ITEM_RARITY_DEFAULT,
    ITEM_RARITY_CONSUMER,
    ITEM_RARITY_INDUSTRIAL,
    ITEM_RARITY_MILSPEC,
    ITEM_RARITY_RESTRICTED,
    ITEM_RARITY_CLASSIFIED,
    ITEM_RARITY_COVERT,
    ITEM_RARITY_CONTRABAND
};

class c_econ_item;

inline c_econ_item *create_econ_item() {

	static auto address = patterns::get_create_econ_item();

    if (!address)
        return nullptr;

    static auto create_econ_item_fn = reinterpret_cast<c_econ_item *(__stdcall *) ()>(*reinterpret_cast<uintptr_t *>(address));

    if (!create_econ_item_fn)
        return nullptr;

    return create_econ_item_fn();
}

inline uintptr_t get_item_schema() {

    static auto address = patterns::get_item_schema();

    if (!address) 
        return 0;

    static auto get_item_schema_fn = reinterpret_cast<uintptr_t(__stdcall *)()>(address);

    if (!get_item_schema_fn)
        return 0;

    return get_item_schema_fn();
}

class c_econ_item {
    unsigned short *get_econ_item_data() {
        return reinterpret_cast<unsigned short *>(this + 0x26);
    }

  public:
    uint32_t *get_account_id() {
        return reinterpret_cast<uint32_t *>(this + 0x1C);
    }

    uint64_t *get_item_id() {
        return reinterpret_cast<uint64_t *>(this + 0x8);
    }

    uint64_t *get_original_id() {
        return reinterpret_cast<uint64_t *>(this + 0x10);
    }

    uint16_t *item_definition_index() {
        return reinterpret_cast<uint16_t *>(this + 0x24);
    }

    uint32_t *get_inventory() {
        return reinterpret_cast<uint32_t *>(this + 0x20);
    }

    unsigned char *get_flags() {
        return reinterpret_cast<unsigned char *>(this + 0x30);
    }

    void add_sticker(const int index, const int kit, const float wear, const float scale, const float rotation) {
        set_attribute_value<int>(113 + 4 * index, kit);
        set_attribute_value<float>(114 + 4 * index, wear);
        set_attribute_value<float>(115 + 4 * index, scale);
        set_attribute_value<float>(116 + 4 * index, rotation);
    }

    void set_stat_trak(const int stat_trak) {
        set_attribute_value<int>(80, stat_trak);
        set_attribute_value<int>(81, 0);
        set_quality(ITEM_QUALITY_STRANGE);
    }

    void set_paint_kit(const float kit) {
        set_attribute_value<float>(6, kit);
    }

    void set_paint_seed(const float seed) {
        set_attribute_value<float>(7, seed);
    }

    void set_paint_wear(const float wear) {
        set_attribute_value<float>(8, wear);
    }

    void set_quality(const int quality) {
        const auto data = *get_econ_item_data();
        *get_econ_item_data() = data ^ (data ^ 32 * quality) & 0x1E0;
    }

    void set_rarity(const int rarity) {
        const auto data = *get_econ_item_data();
        *get_econ_item_data() = (data ^ (rarity << 11)) & 0x7800 ^ data;
    }

    void set_origin(const int origin) {
        const auto data = *get_econ_item_data();
        *get_econ_item_data() = data ^ (uint8_t(data) ^ uint8_t(origin)) & 0x1F;
    }

    void set_level(const int level) {
        const auto data = *get_econ_item_data();
        *get_econ_item_data() = data ^ (data ^ (level << 9)) & 0x600;
    }

    void set_in_use(const bool in_use) {
        const auto data = *get_econ_item_data();
        *get_econ_item_data() = data & 0x7FFF | (in_use << 15);
    }

    void set_custom_name(const char *name) {

        static auto address = patterns::get_custom_name();

        if (!address)
            return;

        static auto set_custom_name_fn = reinterpret_cast<c_econ_item *(__thiscall *) (void *, const char *)>(*reinterpret_cast<uintptr_t *>(&address + 1) + &address + 5);

        if (!set_custom_name_fn)
            return;

        set_custom_name_fn(this, name);
    }

    void set_custom_description(const char *name) {

        static auto address = patterns::get_custom_description();

        if (!address)
            return;

        static auto set_custom_description_fn = reinterpret_cast<c_econ_item *(__thiscall *) (void *, const char *)>(*reinterpret_cast<uintptr_t *>(&address + 1) + &address + 5);

        if (!set_custom_description_fn) 
            return;

        set_custom_description_fn(this, name);
    }

    template <typename type> void set_attribute_value(const int index, type val) {

        const auto v15 = reinterpret_cast<uint32_t *>(get_item_schema());
        const auto v16 = *reinterpret_cast<uint32_t *>(v15[72] + 4 * index);

        static auto set_dynamic_attribute_value_addr = patterns::get_attribute_value();
        static auto set_dynamic_attribute_value_fn = reinterpret_cast<int(__thiscall *)(c_econ_item *, uint32_t, void *)>(set_dynamic_attribute_value_addr);

        if (!set_dynamic_attribute_value_fn)
            return;

        set_dynamic_attribute_value_fn(this, v16, &val);
    }

    void update_equipped_state(const uint32_t state) {

        static auto update_equipped_state_addr = patterns::get_update_equipped_state();
        static auto update_equipped_state_fn = reinterpret_cast<int(__thiscall *)(c_econ_item *, uint32_t)>(update_equipped_state_addr);

        if (!update_equipped_state_fn)
            return;

        update_equipped_state_fn(this, state);
    }
};

class c_econ_item_definition {
  public:

    /*std::vector<AttributeInfo> GetAttributes( )
    {
        std::vector<AttributeInfo> attributes;

        const int   size = *reinterpret_cast< int * >( uintptr_t( this ) + 0x3C );
        const uintptr_t data = *reinterpret_cast< uintptr_t * >( uintptr_t( this ) + 0x30 );

        if ( data )
        {
            for ( int i = 0; i < size; ++i )
            {
                int16_t id = *reinterpret_cast< int16_t * >( data + ( i * 0xC ) );
                int value = *reinterpret_cast< int * >( data + ( i * 0xC ) + 0x4 );

                attributes.push_back( AttributeInfo( id, value ) );
            }
        }

        return attributes;
    }*/

    std::vector<uint16_t> get_associated_items() {

        std::vector<uint16_t> items;

        const uintptr_t data = *reinterpret_cast<uintptr_t *>(uintptr_t(this) + 0xC);

        if (data) {
            const int size = *reinterpret_cast<int *>(uintptr_t(this) + 0x18);

            for (int i = 0; i < size; ++i) {
                items.push_back(*reinterpret_cast<uint16_t *>(data + (i * sizeof(uint16_t))));
            }
        }

        return items;
    }

    /*std::string get_localized_weapon_name( )
    {
        static const auto V_UCS2ToUTF8 = reinterpret_cast< int( * )( const wchar_t *ucs2, char *utf8, int len ) >( GetProcAddress( GetModuleHandle( "vstdlib.dll" ), "V_UCS2ToUTF8" ) );
        auto wname = *reinterpret_cast< const char ** >( uintptr_t( this ) + 0x4C );
        const auto wide_name = g_Valve.g_ILocalize->Find( wname );
        char name [ 256 ];
        V_UCS2ToUTF8( wide_name, name, sizeof( name ) );
        return std::string( name );
    }*/

    //@todo: add keyvalues later
    //KeyValues *get_raw_definition() {
    //    using original_fn = KeyValues *(__thiscall *) (void *);
    //    return util::get_method<original_fn>(this, 10)(this);
    //}

    const char *get_inventory_image() {
        return reinterpret_cast<const char *>((*reinterpret_cast<int(__thiscall **)(int)>(*reinterpret_cast<uint32_t *>(uintptr_t(this)) + 0x14))(uintptr_t(this)));
    }

    int get_item_definition_index() {
        return *reinterpret_cast<int *>(uintptr_t(this) + 0x8);
    }

    int get_rarity_value() {
        return *reinterpret_cast<char *>(uintptr_t(this) + 0x2A);
    }

    const char *get_hud_name() {
        return *reinterpret_cast<const char **>(uintptr_t(this) + 0x4C);
    }

    const char *get_item_type() {
        return *reinterpret_cast<const char **>(uintptr_t(this) + 0x54);
    }

    const char *get_item_description() {
        return *reinterpret_cast<const char **>(uintptr_t(this) + 0x5C);
    }

    const char *get_view_model_name() {
        return *reinterpret_cast<const char **>(uintptr_t(this) + 0x94);
    }

    const char *get_world_model_name() {
        return *reinterpret_cast<const char **>(uintptr_t(this) + 0x9C);
    }

    const char *get_world_model_dropped_name() {
        return *reinterpret_cast<const char **>(uintptr_t(this) + 0xA0);
    }

    int get_num_supported_sticker_slots() {
        return *reinterpret_cast<int *>(uintptr_t(this) + 0x108);
    }

    const char *get_weapon_name_1() {
        return *reinterpret_cast<const char **>(uintptr_t(this) + 0x1B0);
    }

    const char *get_weapon_name() {
        return *reinterpret_cast<const char **>(uintptr_t(this) + 0x1BC);
    }

    int get_loadout_slot() {
        return *reinterpret_cast<int *>(uintptr_t(this) + 0x248);
    }
};

class c_econ_item_view {
  public:
    char pad_0001[0x194];          // 0x0
    __int32 item_definition_index; // 0x194
    __int32 entity_quality;        // 0x198
    __int32 entity_level;          // 0x19C
    char pad0002[0x8];             // 0x1A0
    __int32 item_id_high;          // 0x1A8
    __int32 item_id_low;           // 0x1AC
    __int32 accout_id;             // 0x1B0
    char pad_0003[0x8];            // 0x1B4
    unsigned char initialized;     // 0x1BC
    char pad_0004[0x67];           // 0x1BD
    char custom_name[32];          // 0x224

    c_econ_item *get_soc_data() {

        static auto get_soc_data_addr = patterns::get_soc_data();
        static auto get_soc_data_fn = reinterpret_cast<c_econ_item *(__thiscall *) (c_econ_item_view *)>(get_soc_data_addr);

        if (!get_soc_data_fn)
            return nullptr;

        return get_soc_data_fn(this);
    }

    c_econ_item_definition *get_static_data() {

        static auto get_static_data_addr = patterns::get_static_data();
        static auto get_static_data_fn = reinterpret_cast<c_econ_item_definition *(__thiscall *) (void *)>(get_static_data_addr);

        if (!get_static_data_fn)
            return nullptr;

        return get_static_data_fn(this);
    }
};