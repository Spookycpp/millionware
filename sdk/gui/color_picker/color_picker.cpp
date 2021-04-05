#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <imgui.h>

#include "../../engine/input/input.h"
#include "../../engine/render/render.h"
#include "../../ui/ui.h"
#include "color_picker.h"

static color_t hues_colors[7] =
{
	{ 255, 0, 0 },
	{ 255, 255, 0 },
	{ 0, 255, 0 },
	{ 0, 255, 255 },
	{ 0, 0, 255 },
	{ 255, 0, 255 },
	{ 255, 0, 0 },
};

c_color_picker::c_color_picker(color_t &value, bool show_alpha_bar) : value_(value)
{
	show_alpha_bar_ = show_alpha_bar;
	picking_color_ = false;
	picking_hue_ = false;
	picking_alpha_ = false;
	cursor_pos_ = -1.0f;
}

void c_color_picker::layout(layout_item &overlay, layout_item &parent)
{
	root_ = parent
		.new_item(LAY_TOP | LAY_RIGHT)
		.margins(4.0f, 0.0f, 0.0f, 0.0f)
		.size(26.0f, 18.0f);

	if (ui::get_blocking() == shared_from_this())
	{
		overlay_container_ = overlay.new_item(LAY_HFILL, LAY_COLUMN | LAY_START);

		overlay_picker_row_ = overlay_container_
			.new_item(LAY_HFILL, LAY_ROW | LAY_START)
			.margins(6.0f, 6.0f, 6.0f, 6.0f);

		overlay_color_box_ = overlay_picker_row_
			.new_item(LAY_HFILL)
			.margins(0.0f, 0.0f, 6.0f, 0.0f)
			.size(200.0f, 200.0f);

		overlay_hue_bar_ = overlay_picker_row_
			.new_item(LAY_VFILL)
			.size(16.0f, 0.0f);

		if (show_alpha_bar_)
		{
			overlay_alpha_bar_ = overlay_container_
				.new_item(LAY_HFILL)
				.margins(6.0f, 0.0f, 6.0f, 6.0f)
				.size(0.0f, 16.0f);
		}
	}
	else
	{
		overlay_container_ = layout_item();
	}

	c_element::layout_inline(overlay);
}

void c_color_picker::render()
{
	const auto [root_pos, root_size] = rect_to_xywh(root_.get_rect());

	const auto shared_this = shared_from_this();
	const auto active = ui::get_blocking() == shared_this;
	const auto hovered = (ui::get_blocking() == nullptr || ui::get_blocking() == shared_this) && input::is_in_bounds(root_pos, root_pos + root_size);

	if (hovered)
		input::set_cursor(CURSOR_HAND);

	auto has_been_closed = false;

	if (ui::get_blocking() == nullptr && input::is_mouse_clicked(MOUSE_LEFT) && hovered)
	{
		ui::set_blocking(shared_this);
	}
	else if (active)
	{
		const auto screen_size = render::get_screen_size();
		const auto overlay_pos_x = std::min(screen_size.x - 248.0f, root_pos.x);
		const auto overlay_hovered = input::is_in_bounds({ overlay_pos_x, root_pos.y + root_size.y + 8.0f }, { overlay_pos_x + 240.0f, root_pos.y + root_size.y + 248.0f });

		overlay_container_.set_margins(overlay_pos_x, root_pos.y + root_size.y + 8.0f, 0.0f, 0.0f);
		overlay_container_.run();

		const auto [color_box_pos, color_box_size] = rect_to_xywh(overlay_color_box_.get_rect());
		const auto [hue_bar_pos, hue_bar_size] = rect_to_xywh(overlay_hue_bar_.get_rect());

		const auto color_box_hovered = input::is_in_bounds(color_box_pos, color_box_pos + color_box_size);
		const auto hue_bar_hovered = input::is_in_bounds(hue_bar_pos, hue_bar_pos + hue_bar_size);

		if ((!picking_color_ && !picking_hue_ && !picking_color_) && (color_box_hovered || hue_bar_hovered))
		{
			input::set_cursor(CURSOR_HAND);

			if (input::is_mouse_clicked(MOUSE_LEFT))
			{
				picking_color_ = color_box_hovered;
				picking_hue_ = hue_bar_hovered;
			}
		}
		else if (picking_color_)
		{
			input::set_cursor(CURSOR_HAND);

			if (input::is_mouse_released(MOUSE_LEFT))
			{
				picking_color_ = false;
			}
			else
			{
				const auto mouse_delta = input::get_mouse_pos() - color_box_pos;

				cursor_pos_.x = std::clamp(mouse_delta.x, 0.0f, color_box_size.x);
				cursor_pos_.y = std::clamp(mouse_delta.y, 0.0f, color_box_size.y);

				auto new_r = 0.0f, new_g = 0.0f, new_b = 0.0f;

				picked_saturation_ = std::clamp(cursor_pos_.x / color_box_size.x, 0.0f, 1.0f);
				picked_value_ = 1.0f - std::clamp(cursor_pos_.y / color_box_size.y, 0.0f, 1.0f);

				ImGui::ColorConvertHSVtoRGB(picked_hue_, picked_saturation_, picked_value_, new_r, new_g, new_b);

				value_.get().r = (int) (new_r * 255.0f);
				value_.get().g = (int) (new_g * 255.0f);
				value_.get().b = (int) (new_b * 255.0f);
			}
		}
		else if (picking_hue_)
		{
			input::set_cursor(CURSOR_HAND);

			if (input::is_mouse_released(MOUSE_LEFT))
			{
				picking_hue_ = false;
			}
			else
			{
				auto new_r = 0.0f, new_g = 0.0f, new_b = 0.0f;

				picked_hue_ = std::clamp((input::get_mouse_pos().y - color_box_pos.y) / hue_bar_size.y, 0.0f, 1.0f);

				ImGui::ColorConvertHSVtoRGB(picked_hue_, picked_saturation_, picked_value_, new_r, new_g, new_b);

				value_.get().r = (int) (new_r * 255.0f);
				value_.get().g = (int) (new_g * 255.0f);
				value_.get().b = (int) (new_b * 255.0f);
			}
		}
		else if (picking_alpha_)
		{
			input::set_cursor(CURSOR_HAND);

			if (input::is_mouse_released(MOUSE_LEFT))
			{
				picking_alpha_ = false;
			}
			else
			{
				const auto [alpha_bar_pos, alpha_bar_size] = rect_to_xywh(overlay_alpha_bar_.get_rect());
				const auto new_alpha = std::clamp((input::get_mouse_pos().x - alpha_bar_pos.x) / alpha_bar_size.x, 0.0f, 1.0f);

				value_.get().a = (int) (new_alpha * 255.0f);
			}
		}

		if (show_alpha_bar_)
		{
			const auto [alpha_bar_pos, alpha_bar_size] = rect_to_xywh(overlay_alpha_bar_.get_rect());

			const auto alpha_bar_hovered = input::is_in_bounds(alpha_bar_pos, alpha_bar_pos + alpha_bar_size);

			if (alpha_bar_hovered)
			{
				input::set_cursor(CURSOR_HAND);

				if (input::is_mouse_clicked(MOUSE_LEFT))
					picking_alpha_ = true;
			}
		}

		if (input::is_mouse_clicked(MOUSE_LEFT) && !overlay_hovered)
		{
			ui::set_blocking(nullptr);

			has_been_closed = true;
		}
	}


	if (value_.get().a < 255)
	{
		render::push_clip(root_pos, root_size);

		render::draw_image(root_pos, { 18.0f, 18.0f }, { 255, 255, 255 }, TEXTURE_TRANSPARENCY, 4.0f);
		render::draw_image({ root_pos.x + 18.0f, root_pos.y }, { 18.0f, 18.0f }, { 255, 255, 255 }, TEXTURE_TRANSPARENCY, 4.0f);

		render::pop_clip();
	}

	render::fill_rect(root_pos, root_size, value_.get(), 4.0f);

	render::draw_rect(root_pos - 1.0f, root_size + 2.0f, { 51, 51, 51 }, 3.0f);
	render::draw_rect(root_pos - 1.0f, root_size + 2.0f, { 51, 51, 51 }, 4.0f);
	render::draw_rect(root_pos - 1.0f, root_size + 2.0f, { 51, 51, 51 }, 5.0f);

	if (!has_been_closed && overlay_container_.is_valid())
	{
		render::set_layer(7);

		const auto [overlay_pos, overlay_size] = rect_to_xywh(overlay_container_.get_rect());
		const auto [color_box_pos, color_box_size] = rect_to_xywh(overlay_color_box_.get_rect());
		const auto [hue_bar_pos, hue_bar_size] = rect_to_xywh(overlay_hue_bar_.get_rect());

		render::fill_rect(overlay_pos, overlay_size, { 19, 19, 19 }, 4.0f);
		render::draw_rect(overlay_pos - 1.0f, overlay_size + 2.0f, { 51, 51, 51 }, 4.0f);

		// convert color to individual components
		if (cursor_pos_.x == -1.0f)
		{
			auto hue = 0.0f, saturation = 0.0f, value = 0.0f;

			ImGui::ColorConvertRGBtoHSV(value_.get().r / 255.0f, value_.get().g / 255.0f, value_.get().b / 255.0f, hue, saturation, value);

			cursor_pos_.x = std::clamp(saturation * color_box_size.x, 0.0f, color_box_size.x);
			cursor_pos_.y = std::clamp((1.0f - value) * color_box_size.y, 0.0f, color_box_size.y);

			picked_hue_ = std::clamp((input::get_mouse_pos().y - color_box_pos.y) / hue_bar_size.y, 0.0f, 1.0f);
			picked_saturation_ = std::clamp(cursor_pos_.x / color_box_size.x, 0.0f, 1.0f);
			picked_value_ = 1.0f - std::clamp(cursor_pos_.y / color_box_size.y, 0.0f, 1.0f);
		}

		// draw color box
		auto washed_out_r = 0.0f, washed_out_g = 0.0f, washed_out_b = 0.0f;
		auto fully_saturated_r = 0.0f, fully_saturated_g = 0.0f, fully_saturated_b = 0.0f;

		ImGui::ColorConvertHSVtoRGB(picked_hue_, 0.0f, 1.0f, washed_out_r, washed_out_g, washed_out_b);
		ImGui::ColorConvertHSVtoRGB(picked_hue_, 1.0f, 1.0f, fully_saturated_r, fully_saturated_g, fully_saturated_b);

		const auto washed_out_color = color_t((int) (washed_out_r * 255.0f), (int) (washed_out_g * 255.0f), (int) (washed_out_b * 255.0f));
		const auto fully_saturated_color = color_t((int) (fully_saturated_r * 255.0f), (int) (fully_saturated_g * 255.0f), (int) (fully_saturated_b * 255.0f));

		render::gradient_h(color_box_pos, color_box_size, washed_out_color, fully_saturated_color);
		render::gradient_v(color_box_pos, color_box_size, { 0, 0, 0, 0 }, { 0, 0, 0 });

		render::draw_rect(color_box_pos - 1.0f, color_box_size + 2.0f, { 19, 19, 19 }, 2.0f);
		render::draw_rect(color_box_pos - 1.0f, color_box_size + 2.0f, { 51, 51, 51 }, 4.0f);

		// draw cursor
		render::fill_circle(color_box_pos + cursor_pos_, 4.0f, { 120, 120, 120 });
		render::draw_circle(color_box_pos + cursor_pos_, 4.0f, { 51, 51, 51 });

		// draw hue bar
		for (auto i = 0; i < 6; i++)
		{
			render::gradient_v({ hue_bar_pos.x, hue_bar_pos.y + (hue_bar_size.y / 6.0f) * i }, { hue_bar_size.x, hue_bar_size.y / 6.0f }, hues_colors[i], hues_colors[i + 1]);
		}

		const auto hue_indicator_offset = (hue_bar_size.y - 4.0f) * picked_hue_;
		const auto hue_indicator_pos = point_t(hue_bar_pos.x + 1.0f, hue_bar_pos.y + hue_indicator_offset);
		const auto hue_indicator_size = point_t(hue_bar_size.x - 2.0f, 3.0f);

		render::draw_rect(hue_bar_pos - 1.0f, hue_bar_size + 2.0f, { 19, 19, 19 }, 2.0f);
		render::draw_rect(hue_bar_pos - 1.0f, hue_bar_size + 2.0f, { 51, 51, 51 }, 4.0f);

		render::fill_rect(hue_indicator_pos, hue_indicator_size, { 120, 120, 120 }, 4.0f);
		render::draw_rect(hue_indicator_pos - 1.0f, hue_indicator_size + 2.0f, { 51, 51, 51 }, 4.0f);

		// draw alpha bar
		if (show_alpha_bar_)
		{
			const auto [alpha_bar_pos, alpha_bar_size] = rect_to_xywh(overlay_alpha_bar_.get_rect());

			render::push_clip(alpha_bar_pos, alpha_bar_size);

			for (auto i = 0;; i++)
			{
				const auto offset = i * 18.0f;

				render::draw_image({ alpha_bar_pos.x + offset, alpha_bar_pos.y }, { 18.0f, 18.0f }, { 255, 255, 255 }, TEXTURE_TRANSPARENCY);

				if (offset > alpha_bar_size.x)
					break;
			}

			render::pop_clip();

			render::gradient_h(alpha_bar_pos, alpha_bar_size, { 0, 0, 0, 0 }, value_.get().adjust_alpha(255));

			render::draw_rect(alpha_bar_pos - 1.0f, alpha_bar_size + 2.0f, { 51, 51, 51 }, 3.0f);
			render::draw_rect(alpha_bar_pos - 1.0f, alpha_bar_size + 2.0f, { 51, 51, 51 }, 4.0f);
			render::draw_rect(alpha_bar_pos - 1.0f, alpha_bar_size + 2.0f, { 51, 51, 51 }, 5.0f);

			const auto alpha_indicator_offset = (alpha_bar_size.x - 4.0f) * std::clamp(value_.get().a / 255.0f, 0.0f, 1.0f);
			const auto alpha_indicator_pos = point_t(alpha_bar_pos.x + alpha_indicator_offset, alpha_bar_pos.y + 1.0f);
			const auto alpha_indicator_size = point_t(3.0f, alpha_bar_size.y - 2.0f);

			render::draw_rect(alpha_bar_pos - 1.0f, alpha_bar_size + 2.0f, { 19, 19, 19 }, 2.0f);
			render::draw_rect(alpha_bar_pos - 1.0f, alpha_bar_size + 2.0f, { 51, 51, 51 }, 4.0f);

			render::fill_rect(alpha_indicator_pos, alpha_indicator_size, { 120, 120, 120 }, 4.0f);
			render::draw_rect(alpha_indicator_pos - 1.0f, alpha_indicator_size + 2.0f, { 51, 51, 51 }, 4.0f);
		}

		render::set_layer(0);
	}

	c_element::render();
}
