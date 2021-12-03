#pragma once

#include <cstdint>
#include <string_view>

#define PATTERN_GET(p, a)                                                                                                                  \
    inline auto get_##p() {                                                                                                                \
        return p == 0u ? 0u : (a);                                                                                                         \
    }

class c_player;

namespace patterns {
    inline uintptr_t engine_vgui_start_drawing;
    inline uintptr_t engine_vgui_finish_drawing;
    inline uintptr_t input;
    inline uintptr_t local_player;
    inline uintptr_t view_matrix;
    inline uintptr_t d3d9_device;
    inline uintptr_t set_clantag;
    inline uintptr_t client_mode;
    inline uintptr_t client_state;
    inline uintptr_t global_vars;
    inline uintptr_t is_demo_or_hltv;
    inline uintptr_t money_reveal;
    inline uintptr_t insert_into_tree_list_leaves_in_box_call;
    inline uintptr_t weapon_system;
    inline uintptr_t flash_time;
    inline uintptr_t line_goes_through_smoke;
    inline uintptr_t move_helper;
    inline uintptr_t accept_match;
    inline uintptr_t send_datagram;
    inline uintptr_t load_named_sky;
    inline uintptr_t set_abs_angles;
    inline uintptr_t sequence_activity;
    inline uintptr_t has_bomb;
    inline uintptr_t set_absolute_origin;
    inline uintptr_t report_player;
    inline uintptr_t write_user_command;
    inline uintptr_t create_econ_item;
    inline uintptr_t item_schema;
    inline uintptr_t custom_name;
    inline uintptr_t custom_description;
    inline uintptr_t attribute_value;
    inline uintptr_t update_equipped_state;
    inline uintptr_t soc_data;
    inline uintptr_t static_data;
    inline uintptr_t econ_item_view;
    inline uintptr_t kit_parser_data_1;
    inline uintptr_t kit_parser_data_2;
    inline uintptr_t equip_item_in_loadout;
    inline uintptr_t find_or_create_reference_econ_item;
    inline uintptr_t clear_inventory_images;
    inline uintptr_t remove_item;
    inline uintptr_t base_type_cache;
    inline uintptr_t create_base_type_cache;
    inline uintptr_t gc_client_system;
    inline uintptr_t inventory_item_by_item_id;
    inline uintptr_t add_econ_item;
    inline uintptr_t render_beams;
    inline uintptr_t client_precipitation;
    inline uintptr_t clear_death_notices;
    inline uintptr_t find_hud_element_addr_1;
    inline uintptr_t find_hud_element_addr_2;
    inline uintptr_t enable_world_fog;
    inline uintptr_t game_rules_proxy;
    // inline uintptr_t relay_cluster;
    inline uintptr_t inventory_unlocker;
    inline uintptr_t demo_file_end_reached;
    inline uintptr_t key_values_system;
    inline uintptr_t load_from_buffer;
    inline uintptr_t find_key;
    inline uintptr_t set_string;
    inline uintptr_t glow_manager;
    inline uintptr_t push_notice;
    inline uintptr_t is_breakable;
    inline uintptr_t play_step_sound;
    inline uintptr_t calc_view;
    inline uintptr_t get_color_modulation;
    inline uintptr_t is_using_static_prop_debug_modes;
    inline uintptr_t smoke_count;

    PATTERN_GET(engine_vgui_start_drawing, engine_vgui_start_drawing);
    PATTERN_GET(engine_vgui_finish_drawing, engine_vgui_finish_drawing);
    PATTERN_GET(input, *(uintptr_t *) (input + 1u));
    PATTERN_GET(local_player, **(c_player ***) (local_player + 2u));
    PATTERN_GET(view_matrix, view_matrix);
    PATTERN_GET(d3d9_device, **(uintptr_t **) (d3d9_device + 1u));
    PATTERN_GET(set_clantag, set_clantag);
    PATTERN_GET(client_mode, **(uintptr_t **) (client_mode + 5u));
    PATTERN_GET(client_state, **(uintptr_t **) (client_state + 1u));
    PATTERN_GET(global_vars, **(uintptr_t **) (global_vars + 1u));
    PATTERN_GET(is_demo_or_hltv, is_demo_or_hltv);
    PATTERN_GET(money_reveal, money_reveal);
    PATTERN_GET(insert_into_tree_list_leaves_in_box_call, insert_into_tree_list_leaves_in_box_call);
    PATTERN_GET(weapon_system, *(uintptr_t **) (weapon_system + 2u));
    PATTERN_GET(flash_time, *(uint32_t *) (flash_time + 4u));
    PATTERN_GET(line_goes_through_smoke, *(uintptr_t *) (line_goes_through_smoke));
    PATTERN_GET(move_helper, **(uint32_t ***) (move_helper + 2u));
    PATTERN_GET(accept_match, accept_match);
    PATTERN_GET(send_datagram, send_datagram);
    PATTERN_GET(load_named_sky, load_named_sky);
    PATTERN_GET(set_abs_angles, set_abs_angles);
    PATTERN_GET(sequence_activity, sequence_activity);
    PATTERN_GET(has_bomb, has_bomb);
    PATTERN_GET(set_absolute_origin, set_absolute_origin);
    PATTERN_GET(report_player, report_player);
    PATTERN_GET(write_user_command, write_user_command);
    PATTERN_GET(create_econ_item, *(uintptr_t *) (create_econ_item + 3u));
    PATTERN_GET(item_schema, item_schema);
    PATTERN_GET(custom_name, custom_name);
    PATTERN_GET(custom_description, custom_description);
    PATTERN_GET(attribute_value, attribute_value);
    PATTERN_GET(update_equipped_state, update_equipped_state);
    PATTERN_GET(soc_data, soc_data);
    PATTERN_GET(static_data, static_data);
    PATTERN_GET(econ_item_view, econ_item_view);
    PATTERN_GET(kit_parser_data_1, kit_parser_data_1);
    PATTERN_GET(kit_parser_data_2, kit_parser_data_2);
    PATTERN_GET(equip_item_in_loadout, equip_item_in_loadout);
    PATTERN_GET(find_or_create_reference_econ_item, find_or_create_reference_econ_item);
    PATTERN_GET(clear_inventory_images, clear_inventory_images);
    PATTERN_GET(remove_item, remove_item);
    PATTERN_GET(base_type_cache, base_type_cache);
    PATTERN_GET(create_base_type_cache, create_base_type_cache);
    PATTERN_GET(gc_client_system, gc_client_system);
    PATTERN_GET(inventory_item_by_item_id, inventory_item_by_item_id);
    PATTERN_GET(add_econ_item, add_econ_item);
    PATTERN_GET(render_beams, *(uintptr_t *) (render_beams + 1u));
    PATTERN_GET(client_precipitation, client_precipitation);
    PATTERN_GET(clear_death_notices, clear_death_notices);
    PATTERN_GET(find_hud_element_addr_1, find_hud_element_addr_1);
    PATTERN_GET(find_hud_element_addr_2, find_hud_element_addr_2);
    PATTERN_GET(enable_world_fog, enable_world_fog);
    PATTERN_GET(game_rules_proxy, game_rules_proxy);
    // PATTERN_GET(relay_cluster, relay_cluster);
    PATTERN_GET(inventory_unlocker, inventory_unlocker);
    PATTERN_GET(demo_file_end_reached, demo_file_end_reached);
    PATTERN_GET(key_values_system, key_values_system);
    PATTERN_GET(load_from_buffer, load_from_buffer);
    PATTERN_GET(find_key, find_key);
    PATTERN_GET(set_string, set_string);
    PATTERN_GET(glow_manager, glow_manager);
    PATTERN_GET(push_notice, push_notice);
    PATTERN_GET(is_breakable, is_breakable);
    PATTERN_GET(play_step_sound, play_step_sound);
    PATTERN_GET(calc_view, calc_view);
    PATTERN_GET(get_color_modulation, get_color_modulation);
    PATTERN_GET(is_using_static_prop_debug_modes, is_using_static_prop_debug_modes);
    PATTERN_GET(smoke_count, smoke_count);

    uint32_t get_pattern(std::string_view module_name, std::string_view pattern);
    bool init();
} // namespace patterns