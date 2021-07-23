#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../base_element.h"
#include "../tab/tab.h"

class c_category {
    std::string title_;

    bool has_scroll_bar_;
    bool dragging_;

    float drag_scroll_delta_;
    float drag_start_scroll_;

    layout_item root_;
    layout_item title_item_;
    layout_item content_;
    layout_item category_divider_;
    layout_item scroll_root_;
    layout_item scroll_bar_;

    std::vector<std::shared_ptr<c_tab>> tabs_;

    std::vector<float> content_heights_;
    std::vector<float> scroll_offsets_;
    std::vector<float> max_scroll_offsets_;

  public:
    c_category(std::string_view name);

    std::shared_ptr<c_tab> new_tab(int icon_font, std::string_view icon, std::string_view name);

    void layout(const layout_item &root, layout_item &overlay, layout_item &sidebar, layout_item &content);
    void render(bool show_divider);
};
