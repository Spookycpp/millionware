#include "label.h"
#include "../../engine/render/render.h"
#include "../../ui/ui.h"

c_label::c_label(std::string_view title) {
    title_ = title;
    title_wrap_width_ = 0.0f;
}

void c_label::layout(layout_item &overlay, layout_item &parent) {
    c_element::layout(overlay, parent);

    const auto title_size = render::measure_text(title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);

    root_ = get_row_item().new_item(LAY_HFILL, LAY_COLUMN | LAY_START);
    title_item_ = root_.new_item(LAY_LEFT).size(title_size.x, title_size.y);

    c_element::layout_inline(overlay);
}

void c_label::render() {
    const auto [_1, root_size] = rect_to_xywh(root_.get_rect());
    const auto [title_pos, _2] = rect_to_xywh(title_item_.get_rect());

    render::draw_text(title_pos, {255, 255, 255}, title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);

    title_wrap_width_ = root_size.x - 24.0f;

    c_element::render();
}
