#pragma once

#include <memory>
#include <vector>

#include "../../layout/layout.h"
#include "../../../source engine/vector.h"
#include "../category/category.h"
#include "../base_element.h"

class c_window
{
	layout main_layout_;
	layout overlay_layout_;

	bool first_run_;
	bool dragging_;
	bool resizing_;

	point_t pos_;
	point_t size_;
	point_t min_size_;

	point_t interact_mouse_pos_;
	point_t interact_ref_;

	layout_item root_;
	layout_item sidebar_;
	layout_item content_;

	layout_item logo_item_;

	std::vector<std::shared_ptr<c_category>> categories_;

public:
	c_window(const point_t &pos, const point_t &size);

	std::shared_ptr<c_category> new_category(std::string_view name);

	void render();
};
