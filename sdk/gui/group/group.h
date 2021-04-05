#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../button/button.h"
#include "../checkbox/checkbox.h"
#include "../select/select.h"
#include "../slider/slider.h"
#include "../text_input/text_input.h"
#include "../base_element.h"

class c_group : public c_element
{
	std::string title_;
	std::vector<std::shared_ptr<c_element>> children_;

	layout_item header_;
	layout_item content_;
	layout_item title_item_;

public:
	c_group(std::string_view title);

	std::shared_ptr<c_element> new_button(std::string_view title, std::function<void()> callback, std::string_view icon, int icon_font);
	std::shared_ptr<c_element> new_button(std::string_view title, std::function<void()> callback);
	std::shared_ptr<c_element> new_checkbox(std::string_view title, bool &value);
	std::shared_ptr<c_element> new_select(std::string_view title, int &value, const std::initializer_list<std::string> &options, bool is_multi_select = false);
	std::shared_ptr<c_element> new_text_input(std::string_view title, std::string &value, bool password = false);
	std::shared_ptr<c_element> new_text_input(std::string_view title, std::string &value, std::function<bool(char)> filter_fn, bool password = false);

	template <typename T> std::shared_ptr<c_slider<T>> new_slider(std::string_view title, T &value, T value_min, T value_max, std::string_view format);
	template <typename T> std::shared_ptr<c_slider<T>> new_slider(std::string_view title, T &value, T value_min, T value_max, std::function<std::string(T)> format_fn);

	void layout(layout_item &overlay, layout_item &parent) override;
	void render() override;
};
