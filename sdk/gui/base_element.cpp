#include "base_element.h"
#include "../engine/render/render.h"
#include "../ui/ui.h"
#include "color_picker/color_picker.h"
#include "key_bind/key_bind.h"

void c_element::layout(layout_item &overlay, layout_item &parent) {
    root_row_ = parent.new_item(LAY_HFILL, LAY_ROW).margins(0.0f, 0.0f, 0.0f, 8.0f);
}

void c_element::render() {
    for (const auto child : inline_children_) {
        if (ui::get_blocking() != nullptr && ui::get_blocking() == child)
            continue;

        child->render();
    }
}

layout_item &c_element::get_row_item() {
    return root_row_;
}

layout_item &c_element::get_item() {
    return root_;
}

void c_element::layout_inline(layout_item &overlay) {
    for (const auto child : inline_children_) {
        if (!child->meets_dependencies())
            continue;

        child->layout(overlay, root_row_);
    }
}

std::shared_ptr<c_element> c_element::add_color_picker(color_t &value, bool show_alpha_bar) {
    const auto color_picker = std::make_shared<c_color_picker>(value, show_alpha_bar);

    inline_children_.push_back(color_picker);

    return shared_from_this();
}

std::shared_ptr<c_element> c_element::add_key_bind(int &value, bool allow_keyboard, bool allow_mouse) {
    const auto key_bind = std::make_shared<c_key_bind>(value, allow_keyboard, allow_mouse);

    inline_children_.push_back(key_bind);

    return shared_from_this();
}

bool c_element::meets_dependencies() const {
    const auto shared_this = shared_from_this();

    for (const auto dependency : m_dependencies) {
        if (!dependency(shared_this))
            return false;
    }

    return true;
}

void c_element::add_dependency(const dependency_fn &dependency) {
    m_dependencies.push_back(dependency);
}

void c_element::add_dependency(std::reference_wrapper<bool> dependency) {
    add_dependency([dependency](const auto elem) { return dependency; });
}
