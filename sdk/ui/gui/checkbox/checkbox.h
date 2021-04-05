#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../base_element.h"

class c_checkbox : public c_element
{
	std::string title_;

	std::reference_wrapper<bool> value_;

	layout_item box_;
	layout_item title_item_;
	layout_item check_item_;

	float title_wrap_width_;
	float hover_animation_;
	float toggle_animation_;
	float checkmark_animation_;

public:
	c_checkbox(std::string_view title, bool &value);

	void layout(layout_item &overlay, layout_item &parent) override;
	void render() override;
};
