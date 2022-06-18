#pragma once

#include <functional>
#include <memory>
#include <string>

#include "../base_element.h"

class c_button : public c_element {
    int icon_font_;

    std::string icon_;
    std::string title_;
    std::function<void()> callback_;

    layout_item icon_item_;
    layout_item title_item_;

    float title_wrap_width_;
    float hover_animation_;
    float active_animation_;

public:
    c_button(std::string_view title, std::function<void()> callback, std::string_view icon, int icon_font);
    c_button(std::string_view title, std::function<void()> callback);

    void layout(layout_item &overlay, layout_item &parent) override;
    void render() override;
};
