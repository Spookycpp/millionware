#pragma once

#include <mutex>

namespace features::visuals::esp {
    struct bounding_box_t {
        float x;
        float y;
        float width;
        float height;

        bool valid() const {
            return x != -100.0f && y != 100.0f;
        }
    };

    struct entity_esp_t {
        vector_t position;
        float fade;
        bool spotted;
        float bottom_offset;
    };

    struct footstep_t {
        vector_t position;
        float time;
        float alpha;
    };

    inline std::vector<footstep_t> footsteps;
    inline std::mutex footsteps_mutex;

    void frame();

    void draw_footsteps();
    void draw_player(int idx, c_entity *entity);

    void draw_box(const bounding_box_t &entity_box, c_player *player);
    void draw_name(const bounding_box_t &entity_box, c_player *player);
    void draw_health(const bounding_box_t &entity_box, c_player *player);
    void draw_armor(const bounding_box_t &entity_box, c_player *player);
    void draw_ammo(const bounding_box_t &entity_box, c_player *player);
    void draw_weapon(const bounding_box_t &entity_box, c_player *player);
    void draw_flags(const bounding_box_t &entity_box, c_player *player);

    void draw_skeleton(c_player *player);
    void draw_headspot(c_player *player);

    void draw_planted_bomb(c_entity *entity);
    void draw_dropped_weapon(c_entity *entity, const float dist_to_local);
    void draw_thrown_utility(c_entity *entity);
    void draw_defusal_kit(c_entity *entity, float dist_to_local);

    void draw_recoil_crosshair();

    bounding_box_t get_bounding_box(c_entity *entity);
    bool get_bounding_box(c_entity *entity, bounding_box_t &out_box);
    IDirect3DTexture9 *get_weapon_texture(std::string weapon_name, float scale = 4.0f);
    color_t get_color(c_entity *entity, color_t col);
    bool update_dormancy(int idx, c_entity *entity, float dist_to_local);
    void update_positions();
    void update_position(int idx, const vector_t &pos, bool force_update = false);
    void reset_position();
} // namespace features::visuals::esp