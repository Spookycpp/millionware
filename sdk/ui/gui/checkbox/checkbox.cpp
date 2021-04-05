#include "../../resources/font_awesome.h"
#include "../../engine/input/input.h"
#include "../../engine/render/render.h"
#include "../gui.h"
#include "checkbox.h"

c_checkbox::c_checkbox(std::string_view title, bool &value) : value_(value)
{
	title_ = title;
	title_wrap_width_ = 0.0f;
	hover_animation_ = 0.0f;
	toggle_animation_ = value ? 1.0f : 0.0f;
}

void c_checkbox::layout(layout_item &overlay, layout_item &parent)
{
	c_element::layout(overlay, parent);

	const auto title_size = render::measure_text(title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);
	const auto check_size = render::measure_text(ICON_FA_CHECK, FONT_FA_SOLID_32, 0.0f, 12.0f);

	root_ = get_row_item().new_item(LAY_HFILL, LAY_ROW | LAY_START);

	box_ = root_
		.new_item(LAY_LEFT | LAY_VCENTER, LAY_ROW | LAY_MIDDLE)
		.size(16.0f, 16.0f);

	title_item_ = root_
		.new_item(LAY_LEFT)
		.margins(12.0f, 0.0f, 0.0f, 0.0f)
		.size(title_size.x, title_size.y);

	check_item_ = box_
		.new_item()
		.size(check_size.x, check_size.y);

	c_element::layout_inline(overlay);
}

void c_checkbox::render()
{
	const auto [root_pos, root_size] = rect_to_xywh(root_.get_rect());
	const auto [box_pos, box_size] = rect_to_xywh(box_.get_rect());
	const auto [title_pos, title_size] = rect_to_xywh(title_item_.get_rect());
	const auto [check_pos, _] = rect_to_xywh(check_item_.get_rect());

	const auto shared_this = shared_from_this();
	const auto active = gui::get_blocking() == shared_this;
	const auto hovered = (gui::get_blocking() == nullptr || gui::get_blocking() == shared_this) &&
		(input::is_in_bounds(box_pos, box_pos + box_size) || input::is_in_bounds(title_pos, title_pos + title_size));

	hover_animation_ = handle_animation(hover_animation_, active || hovered ? 1.0f : 0.0f);
	toggle_animation_ = handle_animation(toggle_animation_, value_ ? 1.0f : 0.0f);

	const auto color = color_t(
		std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255),
		std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255),
		std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255));

	if (active || hovered)
		input::set_cursor(CURSOR_HAND);

	if (gui::get_blocking() == nullptr && input::is_mouse_clicked(MOUSE_LEFT) && hovered)
	{
		gui::set_blocking(shared_this);
	}
	else if (active && input::is_mouse_released(MOUSE_LEFT))
	{
		if (hovered)
			value_.get() = !value_;

		gui::set_blocking(nullptr);
	}

	render::fill_rect(box_pos, box_size, color, 4.0f);
	render::fill_rect(box_pos, box_size, gui::get_accent_color().adjust_alpha((int) (toggle_animation_ * 255.0f)), 4.0f);
	render::draw_rect(box_pos - 1.0f, box_size + 2.0f, { 51, 51, 51 }, 4.0f);

	checkmark_animation_ = handle_animation(checkmark_animation_, gui::get_accent_color().luminance() >= 0.5f ? 0.0f : 1.0f);

	const auto checkmark_color = color_t((int) (checkmark_animation_ * 255.0f), (int) (toggle_animation_ * 255.0f));

	render::draw_text(title_pos, { 255, 255, 255 }, title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);
	render::draw_text(check_pos, checkmark_color, ICON_FA_CHECK, FONT_FA_SOLID_32, 0.0f, 12.0f);

	title_wrap_width_ = root_size.x - box_size.x - 12.0f;

	c_element::render();
}
