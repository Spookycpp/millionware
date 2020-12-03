#pragma once

#include <functional>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "../sdk/vector.hpp"
#include "../utils/render.hpp"

#define HELPER_METHOD(key_type, value_type, name, initial_value)  \
  inline value_type& name(key_type index) {                       \
    if (name##_map.find(index) == name##_map.end())               \
      name##_map[index] = initial_value;                          \
    return name##_map[index];                                     \
  }

struct window_context_t {
	// basic window info
	bool is_open;

	point_t position;
	point_t min_size;
	point_t size;

	// tabs and categories info
	std::vector<e_texture> tabs;
	std::vector<std::wstring> categories;

	int current_tab;

	// tab specific info
	std::unordered_map<int, int> current_category_map;
	std::unordered_map<int, float> tab_animation_map;
	std::unordered_map<int, float> category_animation_map;
	std::unordered_map<int, float> category_line_map;
	std::unordered_map<uint32_t, float> element_hover_animation_map;
	std::unordered_map<uint32_t, float> element_animation_map;

	// window interaction info
	bool is_being_dragged;
	bool is_being_resized;

	point_t interaction_start_point;
	point_t interaction_mouse_pos;

	// children layout info
	bool is_calculating_layout;

	point_t working_area;
	point_t cursor_pos;
	point_t inline_cursor_pos;

	uint32_t parent_hash;
	uint32_t blocking_hash;

	// group box positioning info
	bool group_box_flip;

	int group_box_left_offset;
	int group_box_right_offset;

	// helper methods
	HELPER_METHOD(int, int, current_category, 0);
	HELPER_METHOD(int, float, tab_animation, 0.0f);
	HELPER_METHOD(int, float, category_animation, 0.0f);
	HELPER_METHOD(int, float, category_line, 0.0f);
	HELPER_METHOD(uint32_t, float, element_hover_animation, 0.0f);
	HELPER_METHOD(uint32_t, float, element_animation, 0.0f);
};

namespace gui {

	void initialize();

	std::shared_ptr<window_context_t> get_context();

	void window(std::wstring_view title, const std::function<void()>& callback);
	void tab(e_texture icon, const std::function<void()>& callback);
	void category(std::wstring_view title, const std::function<void()>& callback);
	void group(std::wstring_view title, const std::function<void()>& callback);

	void checkbox(std::wstring_view title, uint32_t config_item);
	void slider(std::wstring_view title, uint32_t config_item, int min_value, int max_value);
	void slider(std::wstring_view title, uint32_t config_item, int min_value, int max_value, std::wstring_view format_string);
	void slider(std::wstring_view title, uint32_t config_item, int min_value, int max_value, const std::function<std::wstring_view(int)>& format_string_fn);
	void slider(std::wstring_view title, uint32_t config_item, float min_value, float max_value);
	void slider(std::wstring_view title, uint32_t config_item, float min_value, float max_value, std::wstring_view format_string);
	void slider(std::wstring_view title, uint32_t config_item, float min_value, float max_value, const std::function<std::wstring_view(float)>& format_string_fn);

}
