#include <fmt/format.h>

#include "../../engine/input/input.h"
#include "../../engine/render/render.h"
#include "../../resources/font_awesome.h"
#include "../../ui/ui.h"
#include "slider.h"

constexpr static auto var_track_bar_height = 8.0f;
constexpr static auto var_track_head_size = 14.0f;

template <typename T> c_slider<T>::c_slider(std::string_view title, T &value, T value_min, T value_max, std::string_view format) : value_(value)
{
	title_ = title;
	title_wrap_width_ = 315.0f;
	hover_animation_ = 0.0f;
	value_min_ = value_min;
	value_max_ = value_max;
	format_ = format;
}

template <typename T> c_slider<T>::c_slider(std::string_view title, T &value, T value_min, T value_max, std::function<std::string(T)> format_fn) : value_(value)
{
	title_ = title;
	title_wrap_width_ = 315.0f;
	hover_animation_ = 0.0f;
	value_min_ = value_min;
	value_max_ = value_max;
	format_fn_ = format_fn;
}

template <typename T> void c_slider<T>::layout(layout_item &overlay, layout_item &parent)
{
	c_element::layout(overlay, parent);

	const auto track_head_pos = scale_value<float>((float) value_.get(), (float) value_min_, (float) value_max_, 0.0f, title_wrap_width_ - var_track_head_size);
	const auto value_formatted = format_fn_ != nullptr ? format_fn_(value_.get()) : fmt::format(format_, value_.get());

	const auto title_size = render::measure_text(title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);
	const auto value_size = render::measure_text(value_formatted.data(), FONT_CEREBRI_SANS_BOLD_32, 0.0f, 13.0f);

	root_ = get_row_item().new_item(LAY_HFILL, LAY_COLUMN | LAY_START);

	title_item_ = root_
		.new_item(LAY_LEFT)
		.size(title_size.x, title_size.y);

	track_root_ = root_
		.new_item(LAY_HFILL, LAY_COLUMN | LAY_START)
		.margins(0.0f, 8.0f, 0.0f, 0.0f);

	track_bar_ = track_root_
		.new_item(LAY_HFILL)
		.size(0.0f, var_track_bar_height);

	track_head_ = track_root_
		.new_item(LAY_LEFT | LAY_VCENTER)
		.margins(track_head_pos, -var_track_head_size + var_track_bar_height * 0.5f - 1.0f, 0.0f, 0.0f)
		.size(var_track_head_size, var_track_head_size);

	value_item_ = track_root_
		.new_item(LAY_LEFT | LAY_VCENTER)
		.size(value_size.x, value_size.y)
		.margins(track_head_pos + var_track_head_size * 0.5f - value_size.x * 0.5f + 0.5f, 0.0f, 0.0f, 0.0f);

	c_element::layout_inline(overlay);
}

template <typename T> void c_slider<T>::render()
{
	const auto value_formatted = format_fn_ != nullptr ? format_fn_(value_.get()) : fmt::format(format_, value_.get());

	const auto [root_pos, root_size] = rect_to_xywh(root_.get_rect());
	const auto [track_bar_pos, track_bar_size] = rect_to_xywh(track_bar_.get_rect());
	const auto [track_head_pos, track_head_size] = rect_to_xywh(track_head_.get_rect());
	const auto [title_pos, _1] = rect_to_xywh(title_item_.get_rect());
	const auto [value_pos, _2] = rect_to_xywh(value_item_.get_rect());

	const auto shared_this = shared_from_this();
	const auto active = ui::get_blocking() == shared_this;
	const auto hovered = (ui::get_blocking() == nullptr || ui::get_blocking() == shared_this) &&
		(input::is_in_bounds(track_bar_pos, track_bar_pos + track_bar_size) || input::is_in_bounds(track_head_pos, track_head_pos + track_head_size));

	hover_animation_ = handle_animation(hover_animation_, active || hovered ? 1.0f : 0.0f);

	if (active || hovered)
		input::set_cursor(CURSOR_HAND);

	if (ui::get_blocking() == nullptr && input::is_mouse_clicked(MOUSE_LEFT) && hovered)
	{
		ui::set_blocking(shared_this);
	}
	else if (active)
	{
		if (input::is_mouse_down(MOUSE_LEFT))
		{
			const auto mouse_rel = std::clamp(input::get_mouse_pos().x - track_bar_pos.x, 0.0f, track_bar_size.x);

			value_.get() = (T) scale_value<float>(mouse_rel, 0.0f, track_bar_size.x, (float) value_min_, (float) value_max_);
		}
		else if (input::is_mouse_released(MOUSE_LEFT))
		{
			ui::set_blocking(nullptr);
		}
	}

	const auto color = color_t(
		std::clamp((int) (25.0f + (6.0f * hover_animation_)), 0, 255),
		std::clamp((int) (25.0f + (6.0f * hover_animation_)), 0, 255),
		std::clamp((int) (25.0f + (6.0f * hover_animation_)), 0, 255));

	render::fill_rect(track_bar_pos, track_bar_size, color, 4.0f);

	if (value_min_ < 0 && value_max_ > 0)
	{
		const auto middle_pos = track_bar_pos.x + scale_value(0.0f, (float) value_min_, (float) value_max_, 0.0f, track_bar_size.x);
		const auto track_head_middle_x = track_head_pos.x + var_track_head_size * 0.5f;

		if (value_.get() >= 0)
			render::fill_rect({ middle_pos, track_bar_pos.y }, { track_head_middle_x - middle_pos, track_bar_size.y }, ui::get_accent_color());
		else
			render::fill_rect({ track_head_middle_x, track_bar_pos.y }, { middle_pos - track_head_middle_x, track_bar_size.y }, ui::get_accent_color());
	}
	else if ((value_min_ < 0 && value_max_ <= 0) || (value_min_ >= 0 && value_max_ > 0))
	{
		render::fill_rect(track_bar_pos, { track_head_pos.x - track_bar_pos.x + var_track_head_size * 0.5f, track_bar_size.y }, ui::get_accent_color(), 4.0f, CORNER_LEFT);
	}

	render::draw_rect(track_bar_pos - 1.0f, track_bar_size + 2.0f, { 51, 51, 51 }, 4.0f);

	render::fill_rect(track_head_pos, track_head_size, { 120, 120, 120 }, 4.0f);
	render::draw_rect(track_head_pos - 1.0f, track_head_size + 2.0f, { 51, 51, 51 }, 4.0f);

	render::draw_text(title_pos, { 255, 255, 255 }, title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);
	render::draw_text(value_pos, { 255, 255, 255 }, value_formatted.data(), FONT_CEREBRI_SANS_BOLD_32, 0.0f, 13.0f);

	title_wrap_width_ = root_size.x;

	c_element::render();
}

template class c_slider<int>;
template class c_slider<float>;
template class c_slider<double>;
