#pragma once

#include <functional>
#include <string>

namespace gui {

	void initialize();
	bool is_visible();

	bool begin_window();
	void end_window();

	bool tab(std::wstring_view title);

	bool begin_sub_tab(std::wstring_view title);
	void end_sub_tab();

	bool begin_group(std::wstring_view title);
	void end_group();

	void attach_color_picker(std::wstring_view title, color_t& value, bool alpha_bar);

	bool checkbox(std::wstring_view title, bool& value);

	int slider(std::wstring_view title, int& value, int min_value, int max_value);
	int slider(std::wstring_view title, int& value, int min_value, int max_value, std::wstring_view display_string);
	int slider(std::wstring_view title, int& value, int min_value, int max_value, const std::function<std::wstring_view(int)>& display_string_fn);

	float slider(std::wstring_view title, float& value, float min_value, float max_value);
	float slider(std::wstring_view title, float& value, float min_value, float max_value, std::wstring_view display_string);
	float slider(std::wstring_view title, float& value, float min_value, float max_value, const std::function<std::wstring_view(float)>& display_string_fn);

}
