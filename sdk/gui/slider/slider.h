#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../base_element.h"

template <typename T> class c_slider : public c_element {
    static_assert(std::is_arithmetic_v<T>, "type isn't arithmetic");

    T value_min_;
    T value_max_;

    std::string title_;
    std::string format_;
    std::function<std::string(T)> format_fn_;

    std::reference_wrapper<T> value_;

    layout_item track_root_;
    layout_item track_bar_;
    layout_item track_head_;

    layout_item title_item_;
    layout_item value_item_;

    float title_wrap_width_;
    float hover_animation_;

  public:
    c_slider(std::string_view title, T &value, T value_min, T value_max, std::string_view format);
    c_slider(std::string_view title, T &value, T value_min, T value_max, std::function<std::string(T)> format_fn);

    void layout(layout_item &overlay, layout_item &parent) override;
    void render() override;
};
