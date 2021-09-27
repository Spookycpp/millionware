#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <imgui.h>

#include "../../engine/input/input.h"
#include "../../engine/render/render.h"
#include "../../engine/security/xorstr.h"
#include "../../ui/ui.h"
#include "key_bind.h"

static std::string get_key_display_name(int key) {
    auto scan_code = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);

    switch (key) {
    case VK_LEFT:
    case VK_UP:
    case VK_RIGHT:
    case VK_DOWN:
    case VK_RCONTROL:
    case VK_RMENU:
    case VK_LWIN:
    case VK_RWIN:
    case VK_APPS:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_END:
    case VK_HOME:
    case VK_INSERT:
    case VK_DELETE:
    case VK_DIVIDE:
    case VK_NUMLOCK: scan_code |= KF_EXTENDED;
    }

    char key_name_buffer[32];

    if (GetKeyNameTextA(scan_code << 16, key_name_buffer, sizeof(key_name_buffer)) == 0) {
        switch (key) {
        case VK_LBUTTON: return xs("Left Mouse");

        case VK_MBUTTON: return xs("Middle Mouse");

        case VK_RBUTTON: return xs("Right Mouse");

        case VK_XBUTTON1: return xs("Mouse 4");

        case VK_XBUTTON2: return xs("Mouse 5");
        }
    }

    return key_name_buffer;
}

c_key_bind::c_key_bind(int &value, bool allow_keyboard, bool allow_mouse) : value_(value) {
    allow_keyboard_ = allow_keyboard;
    allow_mouse_ = allow_mouse;
    hover_animation_ = 0.0f;
    active_animation_ = 0.0f;
}

void c_key_bind::layout(layout_item &overlay, layout_item &parent) {
    const auto key_name = get_key_display_name(value_.get());
    const auto value_size = render::measure_text(key_name.data(), FONT_CEREBRI_SANS_MEDIUM_14);

    root_ = parent.new_item(LAY_TOP | LAY_RIGHT).margins(4.0f, 0.0f, 0.0f, 0.0f).size(0.0f, 18.0f);

    value_item_ = root_.new_item(LAY_CENTER).margins(8.0f, 0.0f, 8.0f, 0.0f).size(value_size.x, value_size.y);

    c_element::layout_inline(overlay);
}

void c_key_bind::render() {
    const auto [root_pos, root_size] = rect_to_xywh(root_.get_rect());
    const auto [value_pos, _] = rect_to_xywh(value_item_.get_rect());

    const auto shared_this = shared_from_this();
    const auto active = ui::get_blocking() == shared_this;
    const auto hovered =
        (ui::get_blocking() == nullptr || ui::get_blocking() == shared_this) && input::is_in_bounds(root_pos, root_pos + root_size);
    const auto key_name = get_key_display_name(value_.get());

    hover_animation_ = handle_animation(hover_animation_, active || hovered ? 1.0f : 0.0f);
    active_animation_ = handle_animation(active_animation_, active ? 1.0f : 0.0f);

    const auto color = color_t(std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255),
                               std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255),
                               std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255));

    if (hovered)
        input::set_cursor(CURSOR_HAND);

    if (ui::get_blocking() == nullptr && input::is_mouse_clicked(MOUSE_LEFT) && hovered) {
        ui::set_blocking(shared_this);
    } else if (active) {
        const auto set_new_value = [this](int new_value) {
            value_.get() = new_value;

            ui::set_blocking(nullptr);
        };
        if (input::is_key_pressed(VK_ESCAPE)) {
            set_new_value(0);
        } else if (!input::is_in_bounds(root_pos, root_pos + root_size) && input::is_mouse_clicked(MOUSE_LEFT)) {
            ui::set_blocking(nullptr);
        } else {
            if (allow_mouse_) {
                if (input::is_mouse_clicked(MOUSE_LEFT))
                    set_new_value(VK_LBUTTON);
                else if (input::is_mouse_clicked(MOUSE_RIGHT))
                    set_new_value(VK_RBUTTON);
                else if (input::is_mouse_clicked(MOUSE_MIDDLE))
                    set_new_value(VK_MBUTTON);
                else if (input::is_mouse_clicked(MOUSE_SIDE1))
                    set_new_value(VK_XBUTTON1);
                else if (input::is_mouse_clicked(MOUSE_SIDE2))
                    set_new_value(VK_XBUTTON2);
            }

            if (allow_keyboard_) {
                for (int i = 0; i < 250; i++) {
                    if (!input::is_key_pressed(i) || input::get_key_press_length(i) > 0.5f)
                        continue;

                    set_new_value(i);
                }
            }
        }
    }

    render::fill_rect(root_pos, root_size, color, 4.0f);
    render::draw_rect(root_pos - 1.0f, root_size + 2.0f, {51, 51, 51}, 4.0f);

    const auto value_color = blend_color({255, 255, 255}, ui::get_accent_color(), active_animation_);

    render::draw_text(value_pos, value_color, key_name.data(), FONT_CEREBRI_SANS_MEDIUM_14);

    c_element::render();
}
