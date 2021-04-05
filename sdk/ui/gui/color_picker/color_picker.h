#pragma once

#include <functional>
#include <memory>
#include <string>

#include "../base_element.h"

class c_color_picker : public c_element
{
	bool show_alpha_bar_;

	std::reference_wrapper<color_t> value_;

	layout_item overlay_container_;
	layout_item overlay_picker_row_;
	layout_item overlay_color_box_;
	layout_item overlay_hue_bar_;
	layout_item overlay_alpha_bar_;

	point_t cursor_pos_;

	bool picking_color_;
	bool picking_hue_;
	bool picking_alpha_;

	float picked_hue_;
	float picked_saturation_;
	float picked_value_;

public:
	c_color_picker(color_t &value, bool show_alpha_bar = true);

	void layout(layout_item &overlay, layout_item &parent) override;
	void render() override;
};
