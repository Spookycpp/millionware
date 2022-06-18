#pragma once

#include "vector.h"

struct view_setup_t {
public:
    int x;
    int x_old;
    int y;
    int y_old;
    int width;
    int width_old;
    int height;
    int height_old;

private:
    char padding0[0x90];

public:
    float fov;
    float viewmodel_fov;

    vector_t origin;
    vector_t angles;
};
