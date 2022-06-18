#pragma once

#include <memory>
#include <string>

#include "../base_element.h"
#include "../group/group.h"

class c_tab : public std::enable_shared_from_this<c_tab> {
    int icon_font_;

    std::string icon_;
    std::string name_;

    std::vector<std::shared_ptr<c_group>> children_;

    layout_item tab_item_;
    layout_item tab_inner_item_;
    layout_item icon_item_;
    layout_item name_item_;

    float text_animation_;
    float stripe_animation_;

public:
    c_tab(int icon_font, std::string_view icon, std::string_view name);

    std::shared_ptr<c_group> new_group(std::string_view title);

    void layout(int index, layout_item &overlay, layout_item &sidebar, layout_item &content);
    void render();
};
