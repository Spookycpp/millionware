#include "../../engine/input/input.h"
#include "../../engine/render/render.h"
#include "../../engine/security/xorstr.h"
#include "../../resources/font_awesome.h"
#include "../../ui/ui.h"
#include "select.h"

c_select::c_select(std::string_view title, int &value, const std::initializer_list<std::string> &options, bool is_multi_select) : value_(value)
{
	is_multi_select_ = is_multi_select;
	title_ = title;
	options_ = options;

	option_hover_animation_.resize(options_.size(), 0.0f);
	option_item_animation_.resize(options_.size(), 0.0f);
	option_items_.reserve(options_.size());

	title_wrap_width_ = 0.0f;
	value_wrap_width_ = 0.0f;

	for (auto i = 0u; i < options_.size(); i++)
	{
		all_selected_value_ |= (1 << i);
	}
}

void c_select::layout(layout_item &overlay, layout_item &parent)
{
	c_element::layout(overlay, parent);

	const auto title_size = render::measure_text(title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);

	root_ = get_row_item().new_item(LAY_HFILL, LAY_COLUMN | LAY_START);

	title_item_ = root_
		.new_item(LAY_LEFT)
		.size(title_size.x, title_size.y);

	container_ = root_
		.new_item(LAY_HFILL, LAY_ROW | LAY_START)
		.margins(0.0f, 8.0f, 0.0f, 0.0f);

	if (is_multi_select_)
	{
		option_items_.clear();

		if (value_.get() == 0)
		{
			const auto no_value_selected_size = render::measure_text(xs("Select an option"), FONT_CEREBRI_SANS_MEDIUM_18, value_wrap_width_);

			value_item_ = container_
				.new_item(LAY_LEFT | LAY_VCENTER)
				.margins(8.0f, 8.0f, 8.0f, 8.0f)
				.size(no_value_selected_size.x, no_value_selected_size.y);
		}
		else
		{
			auto items_container = container_
				.new_item(LAY_HFILL | LAY_LEFT, LAY_ROW | LAY_START | LAY_WRAP)
				.margins(8.0f, 8.0f, 4.0f, 4.0f);

			for (auto i = 0u; i < options_.size(); i++)
			{
				if (is_multi_select_ && (value_.get() & (1 << i)) == 0)
					continue;

				const auto item_text_size = render::measure_text(options_[i].data(), FONT_CEREBRI_SANS_MEDIUM_14);
				const auto remove_size = render::measure_text(ICON_FA_TIMES, FONT_FA_SOLID_32, 0.0f, 7.0f);

				auto option_item = items_container
					.new_item(LAY_LEFT | LAY_VCENTER, LAY_ROW | LAY_START)
					.margins(0.0f, 0.0f, 4.0f, 4.0f);

				auto option_text = option_item.new_item(LAY_LEFT | LAY_VCENTER)
					.margins(4.0f, 2.0f, 0.0f, 2.0f)
					.size(item_text_size.x, item_text_size.y);

				auto option_remove = option_item.new_item(LAY_LEFT | LAY_VCENTER)
					.margins(12.0f, 1.0f, 4.0f, 0.0f)
					.size(remove_size.x, remove_size.y);

				option_items_.push_back(active_item_t
					{
						option_item,
						option_text,
						option_remove,

						options_[i],
						1 << i,
					});
			}
		}
	}
	else if (!is_multi_select_)
	{
		const auto value_size = render::measure_text(options_[value_.get()].data(), FONT_CEREBRI_SANS_MEDIUM_18, value_wrap_width_);

		value_item_ = container_
			.new_item(LAY_LEFT | LAY_VCENTER)
			.margins(8.0f, 8.0f, 8.0f, 8.0f)
			.size(value_size.x, value_size.y);
	}

	const auto arrow_size = render::measure_text(ICON_FA_CHEVRON_DOWN, FONT_FA_SOLID_32, 0.0f, 12.0f);

	arrow_item_ = container_
		.new_item(LAY_RIGHT | LAY_VCENTER)
		.margins(8.0f, 0.0f, 8.0f, 0.0f)
		.size(arrow_size.x, arrow_size.y);

	if (ui::get_blocking() == shared_from_this())
	{
		overlay_container_ = overlay.new_item(LAY_HFILL, LAY_COLUMN | LAY_START);

		for (auto i = 0u; i < options_.size(); i++)
		{
			if (is_multi_select_ && (value_.get() & (1 << i)) != 0)
				continue;

			const auto option_size = render::measure_text(options_[i].data(), FONT_CEREBRI_SANS_MEDIUM_18, value_wrap_width_);

			auto option_container = overlay_container_.new_item(LAY_LEFT | LAY_HFILL, LAY_COLUMN | LAY_START);
			auto option_text = option_container
				.new_item(LAY_LEFT | LAY_VCENTER)
				.margins(8.0f, 6.0f, 8.0f, 6.0f)
				.size(option_size.x, option_size.y);
		}

		if (!overlay_container_.begin().is_valid())
		{
            const auto no_choices_size = render::measure_text(xs("No choices to choose from"), FONT_CEREBRI_SANS_MEDIUM_18, value_wrap_width_);

			no_choices_item_ = overlay_container_
				.new_item(LAY_LEFT | LAY_VCENTER)
				.margins(8.0f, 8.0f, 8.0f, 8.0f)
				.size(no_choices_size.x, no_choices_size.y);
		}
		else
		{
			no_choices_item_ = layout_item();
		}
	}
	else
	{
		overlay_container_ = layout_item();
	}

	c_element::layout_inline(overlay);
}

void c_select::render()
{
	const auto [root_pos, root_size] = rect_to_xywh(container_.get_rect());
	const auto [box_pos, box_size] = rect_to_xywh(container_.get_rect());
	const auto [title_pos, _1] = rect_to_xywh(title_item_.get_rect());
	const auto [arrow_pos, arrow_size] = rect_to_xywh(arrow_item_.get_rect());

	const auto shared_this = shared_from_this();
	const auto active = ui::get_blocking() == shared_this;

	auto hovered = (ui::get_blocking() == nullptr || ui::get_blocking() == shared_this) && input::is_in_bounds(box_pos, box_pos + box_size);

	if (is_multi_select_)
	{
		for (auto i = 0u; i < option_items_.size(); i++)
		{
			const auto &item = option_items_[i];

			const auto [item_box_pos, item_box_size] = rect_to_xywh(item.box.get_rect());
			const auto item_hovered = ui::get_blocking() == nullptr && input::is_in_bounds(item_box_pos, item_box_pos + item_box_size);

			if (item_hovered)
			{
				// nigga what the FUCK
				//printf("Hovered %s \n", item.name.data());

				hovered = false;
			}
		}
	}

	hover_animation_ = handle_animation(hover_animation_, active || hovered ? 1.0f : 0.0f);

	const auto color = color_t(
		std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255),
		std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255),
		std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255));

	if (hovered)
		input::set_cursor(CURSOR_HAND);

	auto has_been_closed = false;
	auto new_value = value_.get();

	if (ui::get_blocking() == nullptr && input::is_mouse_clicked(MOUSE_LEFT) && hovered)
	{
		ui::set_blocking(shared_this);
	}
	else if (active)
	{
		overlay_container_.set_size(box_size.x, 0.0f);
		overlay_container_.set_margins(box_pos.x, box_pos.y + box_size.y + 1.0f, 0.0f, 0.0f);
		overlay_container_.run();

		auto input_handled = false;
		auto option_element = overlay_container_.begin();

		if (!no_choices_item_.is_valid())
		{
			for (auto i = 0u; i < options_.size(); i++)
			{
				if (is_multi_select_ && (value_.get() & (1 << i)) != 0)
					continue;

				const auto [option_pos, option_size] = rect_to_xywh(option_element.get_rect());
				const auto option_hovered = input::is_in_bounds(option_pos, option_pos + option_size);

				if (option_hovered)
					input::set_cursor(CURSOR_HAND);

				if (!input_handled && option_hovered && input::is_mouse_clicked(MOUSE_LEFT))
				{
					if (is_multi_select_)
					{
						new_value |= (1 << i);
					}
					else
					{
						new_value = i;
						has_been_closed = true;

						ui::set_blocking(nullptr);
					}

					input_handled = true;
				}

				option_element = option_element.next();
			}
		}

		if (input::is_mouse_clicked(MOUSE_LEFT) && !input_handled)
		{
			ui::set_blocking(nullptr);

			has_been_closed = true;
		}
	}

	render::fill_rect(box_pos, box_size, color, 4.0f, active ? CORNER_TOP : CORNER_ALL);
	render::draw_rect(box_pos - 1.0f, box_size + 2.0f, { 51, 51, 51 }, 4.0f, active ? CORNER_TOP : CORNER_ALL);

	render::draw_text(title_pos, { 255, 255, 255 }, title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);
	render::draw_text({ box_pos.x + box_size.x - arrow_size.x - 8.0f, arrow_pos.y }, { 255, 255, 255 }, ICON_FA_CHEVRON_DOWN, FONT_FA_SOLID_32, 0.0f, 12.0f);

	if (is_multi_select_ && value_.get() == 0)
	{
		const auto [value_pos, _] = rect_to_xywh(value_item_.get_rect());
		const auto value_color = color_t(
			std::clamp((int) (150.0f + (20.0f * hover_animation_)), 150, 170),
			std::clamp((int) (150.0f + (20.0f * hover_animation_)), 150, 170),
			std::clamp((int) (150.0f + (20.0f * hover_animation_)), 150, 170));

		render::draw_text(value_pos, value_color, xs("Select an option"), FONT_CEREBRI_SANS_MEDIUM_18, value_wrap_width_);
	}
	else if (is_multi_select_)
	{
		for (auto i = 0u; i < option_items_.size(); i++)
		{
			const auto &item = option_items_[i];

			const auto [item_box_pos, item_box_size] = rect_to_xywh(item.box.get_rect());
			const auto [item_text_pos, _1] = rect_to_xywh(item.title.get_rect());
			const auto [item_remove_pos, _2] = rect_to_xywh(item.remove.get_rect());

			const auto hovered = ui::get_blocking() == nullptr && input::is_in_bounds(item_box_pos, item_box_pos + item_box_size);

			if (hovered)
			{
				input::set_cursor(CURSOR_HAND);

				if (input::is_mouse_clicked(MOUSE_LEFT))
					new_value &= ~item.mask;
			}

			option_item_animation_[i] = handle_animation(option_item_animation_[i], hovered ? 1.0f : 0.0f);

			const auto outline_alpha = std::clamp((int) (option_item_animation_[i] * 200.0f), 0, 200);
			const auto remove_color = color_t(
				std::clamp((int) (118.0f + (82.0f * option_item_animation_[i])), 0, 255),
				std::clamp((int) (118.0f + (82.0f * option_item_animation_[i])), 0, 255),
				std::clamp((int) (118.0f + (82.0f * option_item_animation_[i])), 0, 255));

			render::fill_rect(item_box_pos, item_box_size, { 40, 40, 40 }, 3.0f);
			render::draw_rect(item_box_pos - 1.0f, item_box_size + 2.0f, ui::get_accent_color().adjust_alpha(outline_alpha), 3.0f);

			render::draw_text(item_text_pos, { 255, 255, 255 }, item.name.data(), FONT_CEREBRI_SANS_MEDIUM_14);
			render::draw_text(item_remove_pos, remove_color, ICON_FA_TIMES, FONT_FA_SOLID_32, 0.0f, 7.0f);
		}
	}
	else if (!is_multi_select_)
	{
		const auto [value_pos, _] = rect_to_xywh(value_item_.get_rect());

		render::draw_text(value_pos, { 255, 255, 255 }, options_[value_.get()].data(), FONT_CEREBRI_SANS_MEDIUM_18, value_wrap_width_);
	}

	title_wrap_width_ = root_size.x;
	value_wrap_width_ = box_size.x - arrow_size.x - 24.0f;

	if (!has_been_closed && overlay_container_.is_valid())
	{
		render::set_layer(7);

		const auto [overlay_pos, overlay_size] = rect_to_xywh(overlay_container_.get_rect());

		render::fill_rect(overlay_pos, overlay_size, color, 4.0f, CORNER_BOTTOM);
		render::draw_rect(overlay_pos - 1.0f, overlay_size + 2.0f, { 51, 51, 51 }, 4.0f, CORNER_BOTTOM);

		if (!no_choices_item_.is_valid())
		{
			auto option_element = overlay_container_.begin();

			for (auto i = 0u; i < options_.size(); i++)
			{
				if (is_multi_select_ && (value_.get() & (1 << i)) != 0)
					continue;


				const auto [option_pos, option_size] = rect_to_xywh(option_element.get_rect());
				const auto [option_text_pos, _] = rect_to_xywh(option_element.begin().get_rect());

				const auto option_hovered = input::is_in_bounds(option_pos, option_pos + option_size);

				option_hover_animation_[i] = handle_animation(option_hover_animation_[i], option_hovered ? 1.0f : 0.0f);

				const auto hover_color = color_t(
					std::clamp((int) (230.0f + (25.0f * option_hover_animation_[i])), 0, 255),
					std::clamp((int) (230.0f + (25.0f * option_hover_animation_[i])), 0, 255),
					std::clamp((int) (230.0f + (25.0f * option_hover_animation_[i])), 0, 255));

				const auto option_color = is_multi_select_ || value_.get() != i ? hover_color : ui::get_accent_color();

				render::draw_text(option_text_pos, option_color, options_[i].data(), FONT_CEREBRI_SANS_MEDIUM_18, value_wrap_width_);

				option_element = option_element.next();
			}
		}
		else
		{
			const auto [no_choices_pos, _] = rect_to_xywh(no_choices_item_.get_rect());

			render::draw_text(no_choices_pos, { 255, 255, 255 }, xs("No choices to choose from"), FONT_CEREBRI_SANS_MEDIUM_18, value_wrap_width_);
		}

		render::set_layer(0);
	}

	value_.get() = new_value;

	c_element::render();
}
