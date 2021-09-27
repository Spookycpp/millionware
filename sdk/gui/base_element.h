#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../engine/render/render.h"
#include "../source engine/color.h"
#include "../thirdparty/layout/layout.h"

using dependency_fn = std::function<bool(const std::shared_ptr<const class c_element> &)>;

template <typename T>
inline T scale_value(T value, T in_min, T in_max, T out_min, T out_max) {
    return (T) ((value - in_min) * (out_max - out_min) / (float) (in_max - in_min) + out_min);
}

inline color_t blend_color(const color_t &start, const color_t &end, float rate) {
    const auto diff_r = end.r - start.r;
    const auto diff_g = end.g - start.g;
    const auto diff_b = end.b - start.b;
    const auto diff_a = end.a - start.a;

    return color_t((int) std::clamp(start.r + diff_r * rate, 0.0f, 255.0f), (int) std::clamp(start.g + diff_g * rate, 0.0f, 255.0f),
                   (int) std::clamp(start.b + diff_b * rate, 0.0f, 255.0f), (int) std::clamp(start.a + diff_a * rate, 0.0f, 255.0f));
}

inline float handle_animation(float value, float target, float rate = 0.065f, float rewind_rate = 0.09f) {
    const auto interval = (1.0f / (target < value ? rewind_rate : rate)) * render::get_frame_time();
    const auto delta = target - value;

    return std::clamp(value + delta * interval, 0.0f, 1.0f);
}

class c_color_picker;
class c_key_bind;

class c_element : public std::enable_shared_from_this<c_element> {
    std::vector<std::shared_ptr<c_element>> inline_children_;
    std::vector<dependency_fn> m_dependencies;

    layout_item root_row_;

protected:
    layout_item root_;

public:
    virtual void layout(layout_item &overlay, layout_item &parent);
    virtual void render();

    layout_item &get_row_item();
    layout_item &get_item();

    void layout_inline(layout_item &overlay);

    std::shared_ptr<c_element> add_color_picker(color_t &value, bool show_alpha_bar = true);
    std::shared_ptr<c_element> add_key_bind(int &value, bool allow_keyboard = true, bool allow_mouse = true);

    bool meets_dependencies() const;

    void add_dependency(const dependency_fn &dependency);
    void add_dependency(std::reference_wrapper<bool> dependency);

    template <typename... T>
    void add_dependencies(const T &...dependencies) {
        (add_dependency(dependencies), ...);
    }
};
