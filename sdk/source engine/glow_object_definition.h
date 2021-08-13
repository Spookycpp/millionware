#pragma once

#include "vector.h"

class glow_object_definition_t {
  public:
    void set(const float r, const float g, const float b, const float a) {
        color = vector_t(r, g, b);
        alpha = a;
        render_when_occluded = true;
        render_when_unoccluded = false;
        bloom_amount = 1.0f;
    }

    int next_free_slot;
    void *entity;
    vector_t color;
    float alpha;
    char unknown0[8];
    float bloom_amount;
    char unknown1[4];
    bool render_when_occluded;
    bool render_when_unoccluded;
    bool full_bloom_render;
    int full_bloom_stencil_test_value;
    int glow_style;
    int split_screen_slot;
};