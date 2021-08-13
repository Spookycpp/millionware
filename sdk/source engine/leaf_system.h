#pragma once

#include <cstdint>

#include "client_class.h"
#include "entity.h"

enum e_render_flag {
    RENDER_FLAG_TWO_PASS = 0x01,
    RENDER_FLAG_STATIC_PROP = 0x02,
    RENDER_FLAG_BRUSH_MODEL = 0x04,
    RENDER_FLAG_STUDIO_MODEL = 0x08,
    RENDER_FLAG_HAS_CHANGED = 0x10,
    RENDER_FLAG_ALTERNATE_SORTING = 0x20,
};