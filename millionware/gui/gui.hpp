#pragma once

#include <functional>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "../sdk/vector.hpp"

struct window_context_t
{
	std::stack<point_t> window_cursor_pos;

	std::vector<std::wstring> tabs;
	std::vector<std::wstring> categories;

	std::unordered_map<int, int> tabs_selected_category_;
	std::unordered_map<int, float> tabs_hover_animation_;
	std::unordered_map<int, float> category_hover_animation_;
	std::unordered_map<uint32_t, float> element_hover_animation_;

	bool open;
	bool dragging;

	uint32_t parent_hash;
	uint32_t blocking_hash;

	point_t drag_start_mouse;
	point_t drag_start_position;

	point_t working_area_size;
	point_t position;
	point_t size;

	int current_tab;

	int& tabs_selected_category(int key);
	float& tabs_hover_animation(int key);
	float& category_hover_animation(int key);
	float& element_hover_animation(uint32_t key);

	point_t pop();

	void push(const point_t& pos);
	void push(int x, int y);

	uint32_t do_hash(const char* string);
	uint32_t do_hash(const wchar_t* string);

	void window(std::wstring_view title, const std::function<void()>& callback);
	void tab(std::wstring_view title, const std::function<void()>& callback);
	void category(std::wstring_view title, const std::function<void()>& callback);
};

namespace gui
{
	window_context_t& get_window(uint32_t hash);
}
