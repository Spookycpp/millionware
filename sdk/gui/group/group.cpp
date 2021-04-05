#include "../../engine/render/render.h"
#include "../../ui/ui.h"
#include "group.h"

c_group::c_group(std::string_view title)
{
	title_ = title;
}

std::shared_ptr<c_element> c_group::new_button(std::string_view title, std::function<void()> callback, std::string_view icon, int icon_font)
{
	const auto button = std::make_shared<c_button>(title, callback, icon, icon_font);

	children_.push_back(button);

	return button;
}

std::shared_ptr<c_element> c_group::new_button(std::string_view title, std::function<void()> callback)
{
	const auto button = std::make_shared<c_button>(title, callback);

	children_.push_back(button);

	return button;
}

std::shared_ptr<c_element> c_group::new_checkbox(std::string_view title, bool &value)
{
	const auto checkbox = std::make_shared<c_checkbox>(title, value);

	children_.push_back(checkbox);

	return checkbox;
}

std::shared_ptr<c_element> c_group::new_select(std::string_view title, int &value, const std::initializer_list<std::string> &options, bool is_multi_select)
{
	const auto select = std::make_shared<c_select>(title, value, options, is_multi_select);

	children_.push_back(select);

	return select;
}

std::shared_ptr<c_element> c_group::new_text_input(std::string_view title, std::string &value, bool password)
{
	const auto text_input = std::make_shared<c_text_input>(title, value, password);

	children_.push_back(text_input);

	return text_input;
}

std::shared_ptr<c_element> c_group::new_text_input(std::string_view title, std::string &value, std::function<bool(char)> filter_fn, bool password)
{
	const auto text_input = std::make_shared<c_text_input>(title, value, filter_fn, password);

	children_.push_back(text_input);

	return text_input;
}

template <typename T> std::shared_ptr<c_slider<T>> c_group::new_slider(std::string_view title, T &value, T value_min, T value_max, std::string_view format)
{
	const auto slider = std::make_shared<c_slider<T>>(title, value, value_min, value_max, format);

	children_.push_back(slider);

	return slider;
}

template <typename T> std::shared_ptr<c_slider<T>> c_group::new_slider(std::string_view title, T &value, T value_min, T value_max, std::function<std::string(T)> format_fn)
{
	const auto slider = std::make_shared<c_slider<T>>(title, value, value_min, value_max, format_fn);

	children_.push_back(slider);

	return slider;
}

void c_group::layout(layout_item &overlay, layout_item &parent)
{
	const auto title_size = render::measure_text(title_.data(), FONT_CEREBRI_SANS_BOLD_32, 0.0f, 24.0f);

	root_ = parent.new_item(LAY_HFILL, LAY_COLUMN | LAY_START);
	header_ = root_.new_item(LAY_HFILL, LAY_COLUMN);
	content_ = root_.new_item(LAY_HFILL, LAY_COLUMN);

	title_item_ = header_
		.new_item(LAY_LEFT | LAY_VCENTER)
		.margins(16.0f, 16.0f, 0.0f, 16.0f)
		.size(title_size.x, title_size.y);

	auto content_inner = content_
		.new_item(LAY_HFILL, LAY_COLUMN | LAY_START)
		.margins(16.0f, 16.0f, 16.0f, 8.0f);

	for (const auto child : children_)
	{
		child->layout(overlay, content_inner);
	}
}

void c_group::render()
{
	const auto [header_pos, header_size] = rect_to_xywh(header_.get_rect());
	const auto [content_pos, content_size] = rect_to_xywh(content_.get_rect());
	const auto [title_pos, _] = rect_to_xywh(title_item_.get_rect());

	render::draw_rect(header_pos - 1.0f, { header_size.x + 2.0f, header_size.y + content_size.y + 2.0f }, { 51, 51, 51 }, 4.0f);
	render::draw_rect({ header_pos.x + 2.0f, header_pos.y - 1 }, { header_size.x - 4.0f, 2.0f }, ui::get_accent_color().adjust_alpha(200));

	render::fill_rect(header_pos, header_size, { 19, 19, 19 }, 4.0f, CORNER_TOP);
	render::fill_rect(content_pos, content_size, { 25, 25, 25 }, 4.0f, CORNER_BOTTOM);

	render::fill_rect({ header_pos.x, header_pos.y + header_size.y }, { header_size.x, 1.0f }, { 51, 51, 51 });
	render::draw_text(title_pos, { 255, 255, 255 }, title_.data(), FONT_CEREBRI_SANS_BOLD_32, 0.0f, 24.0f);

	render::push_clip(content_pos, content_size);

	for (const auto child : children_)
	{
		if (ui::get_blocking() != nullptr && ui::get_blocking() == child)
			continue;

		child->render();
	}

	render::pop_clip();
}

template std::shared_ptr<c_slider<int>> c_group::new_slider<int>(std::string_view, int &, int, int, std::string_view);
template std::shared_ptr<c_slider<int>> c_group::new_slider<int>(std::string_view, int &, int, int, std::function<std::string(int)>);

template std::shared_ptr<c_slider<float>> c_group::new_slider<float>(std::string_view, float &, float, float, std::string_view);
template std::shared_ptr<c_slider<float>> c_group::new_slider<float>(std::string_view, float &, float, float, std::function<std::string(float)>);

template std::shared_ptr<c_slider<double>> c_group::new_slider<double>(std::string_view, double &, double, double, std::string_view);
template std::shared_ptr<c_slider<double>> c_group::new_slider<double>(std::string_view, double &, double, double, std::function<std::string(double)>);
