#pragma once

#include "../gui/category/category.h"
#include "../gui/tab/tab.h"
#include "../gui/window/window.h"
#include "../gui/base_element.h"
#include "../source engine/color.h"

namespace ui
{
	color_t &get_accent_color();

	std::shared_ptr<c_tab> get_active_tab();
	std::shared_ptr<c_element> get_blocking();

	void set_active_tab(std::shared_ptr<c_tab> active_tab);
	void set_blocking(std::shared_ptr<c_element> blocking);

	void init();
	void frame();

	bool is_active();
}
