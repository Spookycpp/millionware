#include "window.h"
#include "../../engine/input/input.h"
#include "../../engine/render/render.h"
#include "../../ui/ui.h"

c_window::c_window(const point_t &pos, const point_t &size) {
    main_layout_.reserve(2048);
    overlay_layout_.reserve(2048);

    first_run_ = true;
    dragging_ = false;
    resizing_ = false;

    pos_ = pos;
    size_ = size;
    min_size_ = size;
}

std::shared_ptr<c_category> c_window::new_category(std::string_view name) {
    const auto category = std::make_shared<c_category>(name);

    categories_.push_back(category);

    return category;
}

void c_window::render() {
    // draw everything
    if (!first_run_) {
        const auto [root_pos, root_size] = rect_to_xywh(root_.get_rect());
        const auto [sidebar_pos, sidebar_size] = rect_to_xywh(sidebar_.get_rect());
        const auto [content_pos, content_size] = rect_to_xywh(content_.get_rect());
        const auto [logo_pos, logo_size] = rect_to_xywh(logo_item_.get_rect());

        const auto logo_hovered = ui::get_blocking() == nullptr && input::is_in_bounds(logo_pos, logo_pos + logo_size);
        const auto resize_hovered =
            ui::get_blocking() == nullptr && input::is_in_bounds(root_pos + root_size - 6.0f, root_pos + root_size + 6.0f);

        if (!dragging_ && logo_hovered && input::is_mouse_clicked(MOUSE_LEFT)) {
            dragging_ = true;
            interact_mouse_pos_ = input::get_mouse_pos();
            interact_ref_ = pos_;
        } else if (dragging_) {
            if (input::is_mouse_down(MOUSE_LEFT)) {
                const auto mouse_delta = input::get_mouse_pos() - interact_mouse_pos_;
                const auto screen_size = render::get_screen_size();

                pos_.x = std::clamp(interact_ref_.x + mouse_delta.x, 8.0f, screen_size.x - size_.x - 8.0f);
                pos_.y = std::clamp(interact_ref_.y + mouse_delta.y, 8.0f, screen_size.y - size_.y - 8.0f);
            } else {
                dragging_ = false;
            }
        }

        if (!resizing_ && resize_hovered) {
            input::set_cursor(CURSOR_RESIZE_NWSE);

            if (input::is_mouse_clicked(MOUSE_LEFT)) {
                resizing_ = true;
                interact_mouse_pos_ = input::get_mouse_pos();
                interact_ref_ = size_;
            }
        } else if (resizing_) {
            input::set_cursor(CURSOR_RESIZE_NWSE);

            if (input::is_mouse_down(MOUSE_LEFT)) {
                const auto mouse_delta = input::get_mouse_pos() - interact_mouse_pos_;
                const auto screen_size = render::get_screen_size();

                size_.x = std::clamp(interact_ref_.x + mouse_delta.x, min_size_.x, screen_size.x - pos_.x - 8.0f);
                size_.y = std::clamp(interact_ref_.y + mouse_delta.y, min_size_.y, screen_size.y - pos_.y - 8.0f);
            } else {
                resizing_ = false;
            }
        }
    }

    first_run_ = false;

    // recalculate the layout
    main_layout_.reset();
    overlay_layout_.reset();

    auto overlay_root = overlay_layout_.item();

    root_ = main_layout_.item(0, LAY_ROW | LAY_START).margins(pos_.x, pos_.y, 0.0f, 0.0f).size(size_.x, size_.y);

    sidebar_ = root_.new_item(LAY_VFILL, LAY_COLUMN | LAY_START).size(250.0f, 0.0f);

    content_ = root_.new_item(LAY_TOP | LAY_HFILL, LAY_ROW | LAY_WRAP | LAY_START).margins(16.0f, 16.0f, 16.0f, 16.0f);

    logo_item_ = sidebar_.new_item(LAY_HCENTER).size(113.0f, 48.0f).margins(0.0f, 32.0f, 0.0f, 12.0f);

    auto sidebar_inner = sidebar_.new_item(LAY_HFILL, LAY_COLUMN | LAY_START).margins(16.0f, 16.0f, 16.0f, 16.0f);

    for (const auto category : categories_) {
        category->layout(root_, overlay_root, sidebar_inner, content_);
    }

    main_layout_.run();
    overlay_layout_.run();

    // draw everything
    const auto [root_pos, root_size] = rect_to_xywh(root_.get_rect());
    const auto [sidebar_pos, sidebar_size] = rect_to_xywh(sidebar_.get_rect());
    const auto [logo_pos, logo_size] = rect_to_xywh(logo_item_.get_rect());

    render::fill_rect(root_pos, root_size, {40, 40, 40}, 4.0f);
    render::fill_rect(sidebar_pos, sidebar_size, {19, 19, 19}, 4.0f, CORNER_LEFT);

    render::draw_image(logo_pos, logo_size, {255, 255, 255}, TEXTURE_MW_LOGO_BASE);
    render::draw_image(logo_pos, logo_size, ui::get_accent_color(), TEXTURE_MW_LOGO_DOLLAR);

    render::push_clip(root_pos, root_size);

    for (auto i = 0u; i < categories_.size(); i++) {
        categories_[i]->render(i < categories_.size() - 1);
    }

    render::pop_clip();

    render::draw_rect(root_pos - 1.0f, root_size + 2.0f, {51, 51, 51}, 4.0f);
}
