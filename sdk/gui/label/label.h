#pragma once

#include <functional>
#include <memory>
#include <string>

#include "../base_element.h"

class c_label : public c_element {
    std::string title_;

    layout_item title_item_;

    float title_wrap_width_;

public:
    c_label(std::string_view title);

    void layout(layout_item &overlay, layout_item &parent) override;
    void render() override;
};
