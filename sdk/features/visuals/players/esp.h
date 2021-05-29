#pragma once
#include "../../../source engine/vector.h"

namespace features::visuals::esp {
    struct bounding_box_t {
        float x;
        float y;
        float width;
        float height;
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

    void update_dormant_pos(int index, const vector_t &position);
    void update_predicted_money(int index, int money);
} // namespace features::visuals::esp