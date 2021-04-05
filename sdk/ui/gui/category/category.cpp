// worst piece of code in this project!

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "../../../engine/input/input.h"
#include "../../../engine/render/render.h"
#include "../gui.h"
#include "category.h"

c_category::c_category(std::string_view title)
{
	std::string title_upper(title);

	for (auto &ch : title_upper)
	{
		ch = std::toupper(ch);
	}

	title_ = title_upper; // std::make_shared<c_text>(FONT_CEREBRI_SANS_BOLD_13, 0.0f, -1.0f, text, color_t(237, 242, 249));

	dragging_ = false;
	drag_scroll_delta_ = 0.0f;
	drag_start_scroll_ = 0.0f;
}

std::shared_ptr<c_tab> c_category::new_tab(int icon_font, std::string_view icon, std::string_view name)
{
	const auto tab = std::make_shared<c_tab>(icon_font, icon, name);

	tabs_.push_back(tab);

	content_heights_.resize(tabs_.size(), 0.0f);
	scroll_offsets_.resize(tabs_.size(), 0.0f);
	max_scroll_offsets_.resize(tabs_.size(), 0.0f);

	return tab;
}

void c_category::layout(const layout_item &root, layout_item &overlay, layout_item &sidebar, layout_item &content)
{
	const auto title_size = render::measure_text(title_.data(), FONT_CEREBRI_SANS_BOLD_13);

	has_scroll_bar_ = false;

	root_ = root;
	content_ = content;

	title_item_ = sidebar
		.new_item(LAY_LEFT)
		.size(title_size.x, title_size.y);

	for (auto i = 0u; i < tabs_.size(); i++)
	{
		if (gui::get_active_tab() == tabs_[i])
		{
			if (gui::get_blocking() == nullptr && std::fabs(input::get_mouse_wheel()) > 0.0f)
				scroll_offsets_[i] += input::get_mouse_wheel() * 60.0f;

			content_.set_margins(16.0f, 16.0f + (scroll_offsets_[i] = std::clamp(scroll_offsets_[i], -max_scroll_offsets_[i], 0.0f)), 16.0f, 16.0f);
		}

		tabs_[i]->layout(i, overlay, sidebar, content);

		if (!has_scroll_bar_ && gui::get_active_tab() == tabs_[i] && max_scroll_offsets_[i] > 0.0f)
		{
			const auto root_height = root_.get_size()[1] - 32.0f;
			const auto scroll_progress = std::fabs(std::clamp(scroll_offsets_[i] / max_scroll_offsets_[i], -1.0f, 0.0f));
			const auto scroll_height = root_height * (root_height / content_heights_[i]);

			scroll_root_ = root_
				.new_item(LAY_RIGHT | LAY_VFILL, LAY_ROW | LAY_START)
				.margins(0.0f, 4.0f, 4.0f, 4.0f)
				.size(5.0f, 0.0f);

			scroll_bar_ = scroll_root_
				.new_item(LAY_TOP | LAY_HFILL, LAY_ROW | LAY_START)
				.margins(0.0f, ((root_.get_size()[1] - 8.0f) - scroll_height) * scroll_progress, 0.0f, 0.0f)
				.size(0.0f, scroll_height);

			has_scroll_bar_ = true;
		}
	}

	category_divider_ = sidebar.new_item(LAY_HFILL);
	category_divider_.set_size(0.0f, 1.0f);
	category_divider_.set_margins(0.0f, 16.0f, 0.0f, 16.0f);
}

void c_category::render(bool show_divider)
{
	const auto [_1, root_size] = rect_to_xywh(root_.get_rect());
	const auto [title_pos, _2] = rect_to_xywh(title_item_.get_rect());
	const auto [divider_pos, divider_size] = rect_to_xywh(category_divider_.get_rect());

	render::draw_text(title_pos, { 237, 242, 249 }, title_.data(), FONT_CEREBRI_SANS_BOLD_13);

	for (auto i = 0u; i < tabs_.size(); i++)
	{
		auto content_height = 0.0f;

		for (auto child_row = content_.begin(); child_row.is_valid(); child_row = child_row.next())
		{
			const auto [_, row_size] = rect_to_xywh(child_row.get_rect());

			content_height = std::max(content_height, row_size.y);
		}

		content_heights_[i] = content_height;
		max_scroll_offsets_[i] = std::max((content_heights_[i] - 16.0f) - (root_size.y - 32.0f), 0.0f);

		tabs_[i]->render();

		if (gui::get_active_tab() == tabs_[i] && has_scroll_bar_)
		{
			const auto [scroll_root_pos, scroll_root_size] = rect_to_xywh(scroll_root_.get_rect());
			const auto [scroll_bar_pos, scroll_bar_size] = rect_to_xywh(scroll_bar_.get_rect());

			render::fill_rect(scroll_root_pos, scroll_root_size, { 19, 19, 19 }, 4.0f);
			render::fill_rect(scroll_bar_pos, scroll_bar_size, gui::get_accent_color(), 4.0f);

			const auto scroll_progress = std::fabs(std::clamp(scroll_offsets_[i] / max_scroll_offsets_[i], -1.0f, 0.0f));
			const auto click_pos_norm = (input::get_mouse_pos().y - scroll_root_pos.y) / scroll_root_size.y;
			const auto grab_pos_norm = scroll_progress * (scroll_root_size.y - scroll_bar_size.y) / scroll_root_size.y;
			const auto grab_height_norm = scroll_bar_size.y / scroll_root_size.y;

			if (!dragging_ && gui::get_blocking() == nullptr && input::is_in_bounds(scroll_bar_pos, scroll_bar_pos + scroll_bar_size))
			{
				input::set_cursor(CURSOR_HAND);

				if (input::is_mouse_clicked(MOUSE_LEFT))
				{
					dragging_ = true;
					drag_scroll_delta_ = click_pos_norm - grab_pos_norm - grab_height_norm * 0.5f;
				}
			}
			else if (!dragging_ && gui::get_blocking() == nullptr && input::is_in_bounds(scroll_root_pos, scroll_root_pos + scroll_root_size))
			{
				input::set_cursor(CURSOR_HAND);

				if (input::is_mouse_clicked(MOUSE_LEFT))
				{
					dragging_ = true;
					drag_scroll_delta_ = 0.0f;
				}
			}
			else if (dragging_)
			{
				input::set_cursor(CURSOR_HAND);

				if (input::is_mouse_down(MOUSE_LEFT))
				{
					const auto new_scroll_progress = (click_pos_norm - drag_scroll_delta_ - grab_height_norm * 0.5f) / (1.0f - grab_height_norm);

					scroll_offsets_[i] = -max_scroll_offsets_[i] * new_scroll_progress;
				}
				else
					dragging_ = false;
			}
		}
	}

	if (show_divider)
		render::fill_rect(divider_pos, divider_size, { 50, 50, 57 });
}
