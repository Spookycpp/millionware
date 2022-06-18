#pragma once

#include <functional>
#include <memory>
#include <string>

#include "../base_element.h"

class c_key_bind : public c_element {
    bool allow_keyboard_;
    bool allow_mouse_;

    std::reference_wrapper<int> value_;

    layout_item value_item_;

    float hover_animation_;
    float active_animation_;

public:
    c_key_bind(int &value, bool allow_keyboard = true, bool allow_mouse = true);

    void layout(layout_item &overlay, layout_item &parent) override;
    void render() override;
};
