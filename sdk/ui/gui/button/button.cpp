#include "../../../engine/input/input.h"
#include "../../../engine/render/render.h"
#include "../gui.h"
#include "button.h"

c_button::c_button(std::string_view title, std::function<void()> callback, std::string_view icon, int icon_font)
{
	icon_font_ = icon_font;
	icon_ = icon;
	title_ = title;
	callback_ = callback;
	title_wrap_width_ = 0.0f;
	hover_animation_ = 0.0f;
	active_animation_ = 0.0f;
}

c_button::c_button(std::string_view title, std::function<void()> callback)
{
	icon_font_ = -1;
	title_ = title;
	callback_ = callback;
	title_wrap_width_ = 0.0f;
	hover_animation_ = 0.0f;
	active_animation_ = 0.0f;
}

void c_button::layout(layout_item &overlay, layout_item &parent)
{
	c_element::layout(overlay, parent);

	const auto title_size = render::measure_text(title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);

	root_ = get_row_item().new_item(LAY_HFILL, LAY_ROW);

	if (icon_font_ != -1)
	{
		const auto icon_size = render::measure_text(icon_.data(), icon_font_, 0.0f, 16.0f);

		icon_item_ = root_
			.new_item()
			.margins(12.0f, 8.0f, 8.0f, 8.0f)
			.size(icon_size.x, icon_size.y);

		title_item_ = root_
			.new_item()
			.margins(0.0f, 8.0f, 12.0f, 8.0f)
			.size(title_size.x, title_size.y);
	}
	else
	{
		title_item_ = root_
			.new_item()
			.margins(12.0f, 8.0f, 12.0f, 8.0f)
			.size(title_size.x, title_size.y);
	}

	c_element::layout_inline(overlay);
}

void c_button::render()
{
	const auto [root_pos, root_size] = rect_to_xywh(root_.get_rect());

	const auto shared_this = shared_from_this();
	const auto active = gui::get_blocking() == shared_this;
	const auto hovered = (gui::get_blocking() == nullptr || gui::get_blocking() == shared_this) && input::is_in_bounds(root_pos, root_pos + root_size);

	hover_animation_ = handle_animation(hover_animation_, active || hovered ? 1.0f : 0.0f);
	active_animation_ = handle_animation(active_animation_, active ? 1.0f : 0.0f);

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
			callback_();

		gui::set_blocking(nullptr);
	}

	render::fill_rect(root_pos, root_size, color, 4.0f);
	render::draw_rect(root_pos - 1.0f, root_size + 2.0f, { 51, 51, 51 }, 4.0f);

	const auto [title_pos, _] = rect_to_xywh(title_item_.get_rect());
	const auto title_color = blend_color({ 255, 255, 255 }, gui::get_accent_color(), active_animation_);

	if (icon_font_ != -1)
	{
		const auto [icon_pos, icon_size] = rect_to_xywh(icon_item_.get_rect());

		render::draw_text(icon_pos, title_color, icon_.data(), icon_font_, 0.0f, 16.0f);
		render::draw_text(title_pos, title_color, title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);

		title_wrap_width_ = root_size.x - icon_size.x - 28.0f;
	}
	else
	{
		render::draw_text(title_pos, title_color, title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);

		title_wrap_width_ = root_size.x - 24.0f;
	}

	c_element::render();
}
