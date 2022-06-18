#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../base_element.h"

class c_select : public c_element {
    struct active_item_t {
        layout_item box;
        layout_item title;
        layout_item remove;

        std::string_view name;

        int mask;
    };

    bool is_multi_select_;

    std::string title_;
    std::vector<std::string> options_;
    std::reference_wrapper<int> value_;

    std::vector<active_item_t> option_items_;
    std::vector<float> option_hover_animation_;
    std::vector<float> option_item_animation_;

    layout_item container_;
    layout_item title_item_;
    layout_item value_item_;
    layout_item arrow_item_;
    layout_item no_choices_item_;
    layout_item overlay_container_;

    float title_wrap_width_;
    float value_wrap_width_;
    float hover_animation_;

    int all_selected_value_;

public:
    c_select(std::string_view title, int &value, const std::initializer_list<std::string> &options, bool is_multi_select = false);

    void layout(layout_item &overlay, layout_item &parent) override;
    void render() override;
};
