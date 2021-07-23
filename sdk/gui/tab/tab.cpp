#include "tab.h"
#include "../../engine/input/input.h"
#include "../../engine/render/render.h"
#include "../../ui/ui.h"

c_tab::c_tab(int icon_font, std::string_view icon, std::string_view name) {
    icon_font_ = icon_font;
    icon_ = icon;
    name_ = name;

    text_animation_ = 0.0f;
    stripe_animation_ = 0.0f;
}

std::shared_ptr<c_group> c_tab::new_group(std::string_view title) {
    const auto group = std::make_shared<c_group>(title);

    children_.push_back(group);

    return group;
}

void c_tab::layout(int index, layout_item &overlay, layout_item &sidebar, layout_item &content) {
    const auto icon_size = render::measure_text(icon_.data(), icon_font_, 0.0f, 16.0f);
    const auto name_size = render::measure_text(name_.data(), FONT_CEREBRI_SANS_MEDIUM_18);

    tab_item_ = sidebar.new_item(LAY_LEFT | LAY_HFILL, LAY_COLUMN | LAY_START).margins(0.0f, index == 0 ? 16.0f : 0.0f, 0.0f, 0.0f);

    tab_inner_item_ = tab_item_.new_item(LAY_LEFT, LAY_ROW | LAY_START).margins(0.0f, 8.0f, 0.0f, 8.0f);

    icon_item_ = tab_inner_item_.new_item(LAY_LEFT, LAY_ROW).size(22.0f, 16.0f).new_item(LAY_LEFT).size(icon_size.x, icon_size.y);

    name_item_ = tab_inner_item_.new_item(LAY_LEFT | LAY_VCENTER).size(name_size.x, name_size.y).margins(8.0f, 0.0f, 0.0f, 0.0f);

    if (ui::get_active_tab() == shared_from_this()) {
        auto flip = false;

        auto first_row = content.new_item(LAY_TOP | LAY_HFILL, LAY_COLUMN | LAY_START).margins(0.0, 0.0, 16.0f, 0.0);

        auto second_row = content.new_item(LAY_TOP | LAY_HFILL, LAY_COLUMN | LAY_START);

        for (const auto child : children_) {
            if (!child->meets_dependencies())
                continue;

            child->layout(overlay, flip ? second_row : first_row);
            child->get_item().set_margins(0.0f, 0.0f, 0.0f, 16.0f);

            flip = !flip;
        }
    }
}

void c_tab::render() {
    const auto [tab_pos, tab_size] = rect_to_xywh(tab_item_.get_rect());
    const auto [inner_pos, inner_size] = rect_to_xywh(tab_inner_item_.get_rect());

    const auto shared_this = shared_from_this();
    const auto hovered = ui::get_blocking() == nullptr && input::is_in_bounds({tab_pos.x - 16.0f, tab_pos.y}, {tab_pos.x + tab_size.x + 16.0f, tab_pos.y + tab_size.y});
    const auto active = ui::get_active_tab() == shared_this;

    text_animation_ = handle_animation(text_animation_, ui::get_blocking() == nullptr && (active || hovered) ? 1.0f : 0.0f);
    stripe_animation_ = handle_animation(stripe_animation_, active ? 1.0f : 0.0f);

    render::fill_rect({inner_pos.x - 16.0f, inner_pos.y - 3.0f}, {2.0f, inner_size.y + 6.0f}, ui::get_accent_color().adjust_alpha((int) (stripe_animation_ * 255.0f)));

    if (active) {
        for (const auto child : children_) {
            if (!child->meets_dependencies())
                continue;

            child->render();
        }
    }

    const auto color = color_t(std::clamp((int) (237.0f + (18.0f * text_animation_)), 0, 255), std::clamp((int) (242.0f + (13.0f * text_animation_)), 0, 255),
                               std::clamp((int) (249.0f + (6.0f * text_animation_)), 0, 255));

    const auto [icon_pos, _1] = rect_to_xywh(icon_item_.get_rect());
    const auto [name_pos, _2] = rect_to_xywh(name_item_.get_rect());

    render::draw_text(icon_pos, color, icon_.data(), icon_font_, 0.0f, 16.0f);
    render::draw_text(name_pos, color, name_.data(), FONT_CEREBRI_SANS_MEDIUM_18);

    if (hovered) {
        input::set_cursor(CURSOR_HAND);

        if (input::is_mouse_clicked(MOUSE_LEFT))
            ui::set_active_tab(shared_this);
    }
}
