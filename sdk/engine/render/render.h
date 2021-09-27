#pragma once

#include <d3d9.h>

#include "../../source engine/color.h"
#include "../../source engine/vector.h"

#include <vector>

enum e_font {
    FONT_VERDANA_12,
    FONT_VERDANA_14,
    FONT_VERDANA_24,
    FONT_VERDANA_12_BOLD,
    FONT_TAHOMA_11,
    FONT_TAHOMA_12,
    FONT_SMALL_TEXT,
    FONT_CEREBRI_SANS_BOLD_13,
    FONT_CEREBRI_SANS_MEDIUM_14,
    FONT_CEREBRI_SANS_MEDIUM_18,
    FONT_CEREBRI_SANS_BOLD_32,
    FONT_WEAPONS_16,
    FONT_WEAPONS_32,
    FONT_FA_BRANDS_32,
    FONT_FA_REGULAR_32,
    FONT_FA_SOLID_32,

    // marks the last font available and is to tell how many fonts there are
    // if you add any new fonts please put it BEFORE this one or else u might cause heap corruption
    FONT_MAX,
};

enum e_texture {
    TEXTURE_MW_LOGO_BASE,
    TEXTURE_MW_LOGO_DOLLAR,
    TEXTURE_TRANSPARENCY,

    // same here as with fonts
    TEXTURE_MAX,
};

enum e_corner {
    CORNER_NONE = 0,
    CORNER_TOP_LEFT = 1 << 0,
    CORNER_TOP_RIGHT = 1 << 1,
    CORNER_BOTTOM_LEFT = 1 << 2,
    CORNER_BOTTOM_RIGHT = 1 << 3,
    CORNER_TOP = CORNER_TOP_LEFT | CORNER_TOP_RIGHT,
    CORNER_BOTTOM = CORNER_BOTTOM_LEFT | CORNER_BOTTOM_RIGHT,
    CORNER_LEFT = CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT,
    CORNER_RIGHT = CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
    CORNER_ALL = CORNER_TOP | CORNER_BOTTOM,
};

namespace render {
    void init(HWND window, IDirect3DDevice9 *device);
    void undo();

    HWND window();

    float get_frame_time();

    point_t get_screen_size();

    void begin();
    void finish();
    void set_layer(int index);

    void push_clip(const point_t &position, const point_t &size, bool intersect = true);
    void reset_clip();
    void pop_clip();

    IDirect3DTexture9 *create_from_png(uint8_t *png_data, int png_data_size);
    IDirect3DTexture9 *rasterize_vector(char *data, float scale);

    void draw_line(const point_t &start, const point_t &end, const color_t &color, float thickness = 1.0f);
    void draw_rect(const point_t &position, const point_t &size, const color_t &color, float rounding = 0.0f, int corners = CORNER_ALL);
    void fill_rect(const point_t &position, const point_t &size, const color_t &color, float rounding = 0.0f, int corners = CORNER_ALL);
    void gradient_h(const point_t &position, const point_t &size, const color_t &color_start, const color_t &color_end);
    void gradient_v(const point_t &position, const point_t &size, const color_t &color_start, const color_t &color_end);
    void draw_triangle(const point_t &point1, const point_t &point2, const point_t &point3, const color_t &color);
    void fill_triangle(const point_t &point1, const point_t &point2, const point_t &point3, const color_t &color);
    void draw_circle(const point_t &center, float radius, const color_t &color, int segments = 0);
    void fill_circle(const point_t &center, float radius, const color_t &color, int segments = 0);
    void fill_convex_poly(const point_t *points, int num_points, const color_t &color);
    void draw_text(const point_t &position, const color_t &color, const char *text, int font, float wrap_width = 0.0f,
                   float font_size = -1.0f);
    void draw_text_outlined(const point_t &position, const color_t &color, const color_t &outline_color, const char *text, int font,
                            float wrap_width = 0.0f, float font_size = -1.0f);
    void draw_image(const point_t &position, const point_t &size, const color_t &color, int texture, float rounding = 0.0f,
                    int corners = CORNER_ALL);
    void draw_image(const point_t &position, const point_t &size, const color_t &color, IDirect3DTexture9 *texture, float rounding = 0.0f,
                    int corners = CORNER_ALL);
    void draw_poly_line(const point_t *points, int num_points, const color_t &color, float thickness);
    void draw_bezier_cubic(const point_t &point1, const point_t &point2, const point_t &point3, const point_t &point4, const color_t &color,
                           const float thickness);
    void draw_bezier_quad(const point_t &point1, const point_t &point2, const point_t &point3, const color_t &color, const float thickness);

    point_t measure_text(const char *text, int font, float wrap_width = 0.0f, float font_size = -1.0f);

    class animated_gif {
    private:
        int width = 0, height = 0;

        int m_frame_count = 0;
        int m_last_frame_num = 0;
        int m_last_frame_update_num = -1;

        float m_delay = 0;
        float m_timer = -1.0f;

        IDirect3DTexture9 *m_texture = nullptr;
        unsigned char *m_buffer = nullptr;

        std::vector<float> m_delays;

    public:
        animated_gif(const unsigned char *bytes, int size);

        bool load_from_memory(const unsigned char *gif_buffer, int gif_buffer_size);

        void create_texture(const unsigned char *buffer);

        void draw(const point_t &position, const point_t &size, const color_t &color, float rounding, int corners);
        void update();

        void clear();
    };
} // namespace render
