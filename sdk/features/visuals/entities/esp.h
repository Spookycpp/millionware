#pragma once
#include "../../../source engine/vector.h"

namespace features::visuals::esp {
    struct bounding_box_t {
        float x;
        float y;
        float width;
        float height;
    };

    struct entity_esp_t {
        int predicted_money;
        float last_server_update;
        vector_t position;
        int last_health;
        int health_fade;
    };

    void frame();

    void draw_box(const bounding_box_t &entity_box, c_player *player);
    void draw_name(const bounding_box_t &entity_box, c_player *player);
    void draw_health(const bounding_box_t &entity_box, c_player *player);
    void draw_armor(const bounding_box_t &entity_box, c_player *player);
    void draw_ammo(const bounding_box_t &entity_box, c_player *player);
    void draw_weapon(const bounding_box_t &entity_box, c_player *player);
    void draw_flags(const bounding_box_t &entity_box, c_player *player);

    void draw_skeleton(c_player *player);
    void draw_headspot(c_player *player);
    void draw_barrel(c_player *player);

    void draw_planted_bomb(c_entity* entity);
    void draw_dropped_weapon(c_entity *entity);
    void draw_thrown_utility(c_entity *entity);
    void draw_defusal_kit(c_entity *entity, float dist_to_local);

    void update_dormant_pos(int index, const vector_t &position);
    void update_predicted_money(int index, int money);
} // namespace features::visuals::esp