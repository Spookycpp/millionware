#include <algorithm>
#include <fmt/format.h>
#include <stack>
#include <vector>

#include "../core/cheat.hpp"
#include "../core/config.hpp"
#include "../core/interfaces.hpp"
#include "../sdk/vector.hpp"
#include "../utils/input/input.hpp"
#include "../utils/math/math.hpp"
#include "../utils/render/render.hpp"
#include "../utils/xorstr/xorstr.hpp"
#include "gui.hpp"

constexpr auto TABS_BAR_HEIGHT = 50;
constexpr auto SUB_TABS_BAR_HEIGHT = 40;
constexpr auto TABS_INITIAL_OFFSET = 74;
constexpr auto TAB_BUTTON_PADDING = 18;
constexpr auto SUB_TABS_INITIAL_OFFSET = 7;
constexpr auto SUB_TAB_BUTTON_PADDING = 8;
constexpr auto CONTENT_PADDING = 10;
constexpr auto GROUP_HEADER_HEIGHT = 40;
constexpr auto GROUP_HEADER_PADDING = 10;
constexpr auto CHECKBOX_BOX_SIZE = 13;
constexpr auto SLIDER_BAR_HEIGHT = 6;
constexpr auto SLIDER_HANDLE_WIDTH = 6;
constexpr auto SLIDER_HANDLE_HEIGHT = 10;

#define MAP_HELPER(key_type, value_type, name, initial_value)													\
	static std::unordered_map<key_type, value_type> name##_map;													\
	inline value_type& get_##name(key_type index, value_type initial = initial_value) {	\
		if (name##_map.find(index) == name##_map.end())																		\
			name##_map[index] = initial;																										\
		return name##_map[index];																													\
	}

static bool is_open;
static bool is_dragging;
static bool group_box_side_flip;

static int current_tab;
static int group_box_left_offset;
static int group_box_right_offset;

static uint32_t blocking_hash;
static uint32_t parent_hash;

static point_t position;
static point_t size;
static point_t working_area;
static point_t interaction_mouse_pos;
static point_t interaction_window_pos;

static std::stack<point_t> cursor_stack;
static std::vector<std::wstring> tabs;
static std::vector<std::wstring> sub_tabs;
static std::vector<std::shared_ptr<class c_attachable>> attachables;

static float profile_tab_animation;

MAP_HELPER(int, float, tab_hover_animation, 0.0f);
MAP_HELPER(int, float, tab_text_hover_animation, 0.0f);
MAP_HELPER(int, float, tab_glow_animation, 0.0f);
MAP_HELPER(int, float, sub_tab_animation, 0.0f);
MAP_HELPER(int, int, sub_tab_index, 0);

MAP_HELPER(uint32_t, bool, group_collapsed, false);
MAP_HELPER(uint32_t, float, group_toggle_animation, 0.0f);
MAP_HELPER(uint32_t, int, group_height, 0);
MAP_HELPER(uint32_t, float, group_height_animation, 0.0f);
MAP_HELPER(uint32_t, float, element_animation, 0.0f);
MAP_HELPER(uint32_t, float, element_hover_animation, 0.0f);

class c_attachable {
public:
	virtual void handle() = 0;

	uint32_t this_hash;

	point_t position;
	point_t size;
};

class c_color_picker : public c_attachable {
	color_t* value_ptr;
	bool alpha_bar;

public:
	c_color_picker(std::wstring_view title, color_t& value, bool alpha_bar);

	void handle() override;
};

inline float handle_animation(float& state, float target, float rate = 0.035f) {
	const auto interval = (1.0f / rate) * interfaces::global_vars->frame_time;
	const auto delta = target - state;

	return state = std::clamp(state + delta * interval, 0.0f, 1.0f);
}

inline point_t pop_cursor_pos() {
	const auto top = cursor_stack.top();

	cursor_stack.pop();

	return top;
}

inline void push_cursor_pos(point_t cursor_pos) {
	cursor_stack.push(cursor_pos);
}

inline void push_cursor_pos(int x, int y) {
	cursor_stack.push({ x, y });
}

c_color_picker::c_color_picker(std::wstring_view title, color_t& value, bool alpha_bar) {
	const auto cursor_pos = pop_cursor_pos();

	this->value_ptr = &value;
	this->alpha_bar = alpha_bar;

	this->this_hash = FNV(title.data(), parent_hash);
	this->size = point_t(22, 13);

	this->position.x = cursor_pos.x + working_area.x - size.x;
	this->position.y = cursor_pos.y;

	for (const auto& attachable : attachables)
		this->position.x -= attachable->size.x + CONTENT_PADDING / 2;

	push_cursor_pos(cursor_pos);
}

void c_color_picker::handle() {
	render::fill_rect_rounded(position - 1, size + 2, 3, CORNER_ALL, color_t(22, 22, 26));
	render::fill_rect_rounded(position, size, 3, CORNER_ALL, *value_ptr);
}

void gui::initialize() {
	is_open = true;
	current_tab = 0;
	blocking_hash = 0;
	position = point_t(80);
	size = point_t(700, 480);
}

bool gui::is_visible() {
	return is_open;
}

bool gui::begin_window() {
	if (input::is_key_pressed(VK_INSERT))
		is_open = !is_open;

	if (!is_open)
		return false;

	auto tabs_offset = TABS_INITIAL_OFFSET;

	for (auto i = 0u; !tabs.empty() && i < tabs.size() - 1; i++) {
		const auto text_size = render::measure_text(e_font::UI_14, tabs.at(i));

		tabs_offset += text_size.x + TAB_BUTTON_PADDING * 2;
	}

	const auto username_text_size = render::measure_text(e_font::UI_13, cheat::get_username());
	const auto username_position = point_t(position.x + size.x - TABS_INITIAL_OFFSET / 2 - username_text_size.x + 20, position.y + TABS_BAR_HEIGHT / 2 - username_text_size.y / 2);

	const auto logo_area_hovered = blocking_hash == 0 && input::is_in_bounds(position.x, position.y, position.x + TABS_INITIAL_OFFSET, position.y + TABS_BAR_HEIGHT);
	const auto past_tabs_area_hovered = blocking_hash == 0 && input::is_in_bounds(position.x + tabs_offset, position.y, username_position.x - CONTENT_PADDING * 2 - 32, position.y + TABS_BAR_HEIGHT);

	if (!is_dragging && input::is_key_pressed(VK_LBUTTON) && (logo_area_hovered || past_tabs_area_hovered)) {
		is_dragging = true;
		interaction_window_pos = position;
		interaction_mouse_pos = input::get_mouse_pos();
		blocking_hash = FNV_CT("draggin' da window");
	}
	else if (input::is_key_down(VK_LBUTTON) && is_dragging) {
		const auto mouse_delta = input::get_mouse_pos() - interaction_mouse_pos;

		position = interaction_window_pos + mouse_delta;
	}
	else if (input::is_key_released(VK_LBUTTON) && is_dragging) {
		is_dragging = false;
		blocking_hash = 0;
	}

	cursor_stack = {};
	tabs = {};
	sub_tabs = {};

	render::fill_rect_rounded(position - 1, size + 2, 3, CORNER_ALL, color_t(43, 43, 46));
	render::fill_rect_rounded(position, size, 3, CORNER_ALL, color_t(35, 35, 39));

	render::fill_rect_rounded(position, point_t(size.x, TABS_BAR_HEIGHT + SUB_TABS_BAR_HEIGHT + 1), 3, CORNER_TOP, color_t(24, 24, 28));
	render::line(position.x, position.y + TABS_BAR_HEIGHT + 1, position.x + size.x, position.y + TABS_BAR_HEIGHT + 1, color_t(59, 59, 71));

	const auto icon_pos = point_t(position.x + TABS_INITIAL_OFFSET / 2 - 20, position.y + TABS_BAR_HEIGHT / 2 - 20);

	render::texture(icon_pos, 40, e_texture::MW_LOGO_40, color_t(255));
	render::texture(icon_pos, 40, e_texture::MW_DOLLAR_40, config::get<color_t>(FNV_CT("ui.accent")).adjust_alpha(255));

	group_box_side_flip = false;
	group_box_left_offset = TABS_BAR_HEIGHT + SUB_TABS_BAR_HEIGHT + CONTENT_PADDING;
	group_box_right_offset = TABS_BAR_HEIGHT + SUB_TABS_BAR_HEIGHT + CONTENT_PADDING;

	parent_hash = 0;

	working_area.x = size.x - CONTENT_PADDING * 2;
	working_area.y = size.y - TABS_BAR_HEIGHT - SUB_TABS_BAR_HEIGHT - CONTENT_PADDING * 2;

	push_cursor_pos(position.x + CONTENT_PADDING, position.y + group_box_left_offset);

	return true;
}

void gui::end_window() {
	render::reset_clip();

	auto tabs_offset = TABS_INITIAL_OFFSET;
	auto tab_index = 0;

	for (auto i = 0u; i < tabs.size() - 1; i++) {
		const auto& tab_text = tabs.at(i);
		const auto text_size = render::measure_text(e_font::UI_14, tab_text);

		const auto button_position = point_t(position.x + tabs_offset, position.y);
		const auto button_size = point_t(text_size.x + TAB_BUTTON_PADDING * 2, TABS_BAR_HEIGHT);
		const auto hovered = blocking_hash == 0 && input::is_in_bounds(button_position, button_position + button_size - 1);

		auto target_hover_animation = 0.0f;
		auto target_text_hover_animation = 0.0f;
		auto target_glow_animation = 0.0f;

		if (current_tab == tab_index)
			target_glow_animation = 1.0f;
		else if (hovered)
			target_glow_animation = 0.3f;

		if (current_tab == tab_index)
			target_text_hover_animation = 1.0f;
		else if (hovered)
			target_text_hover_animation = 0.75f;

		// if (current_tab != tab_index && hovered)
		if (current_tab == tab_index || hovered)
			target_hover_animation = 1.0f;

		const auto hover_animation = handle_animation(get_tab_hover_animation(tab_index), target_hover_animation, 0.06f);
		const auto text_hover_animation = handle_animation(get_tab_text_hover_animation(tab_index), target_text_hover_animation, 0.06f);
		const auto glow_animation = handle_animation(get_tab_glow_animation(tab_index), target_glow_animation, 0.06f);

		const auto& accent_color = config::get<color_t>(FNV_CT("ui.accent"));

		if (hovered && input::is_key_pressed(VK_LBUTTON))
			current_tab = tab_index;

		render::fill_rect(button_position.x, button_position.y, button_size.x, button_size.y + 1, color_t(100, static_cast<int>(hover_animation * 10.0f)));
		render::fill_rect(button_position.x, button_position.y, button_size.x, 2, accent_color.adjust_alpha(static_cast<int>(glow_animation * 255.0f)));

		// auto accent_color_copy = accent_color.adjust_alpha(255);

		// the perfect way to make the color lighter lmfao
		// accent_color_copy.r = std::min(255, std::max(0, accent_color_copy.r + 60));
		// accent_color_copy.g = std::min(255, std::max(0, accent_color_copy.g + 60));
		// accent_color_copy.b = std::min(255, std::max(0, accent_color_copy.b + 60));

		// this is a joke
		// interfaces::vgui_surface->set_draw_color(accent_color_copy);
		// interfaces::vgui_surface->fill_faded_rectangle(button_position.x, button_position.y + 2, button_size.x, static_cast<int>(button_size.y * 0.7f), static_cast<int>(glow_animation * 5.0f), 0, false);

		render::text(button_position + button_size / 2 - text_size / 2, e_font::UI_14, color_t(150 + static_cast<int>(105.0f * text_hover_animation)), tab_text);

		tab_index++;
		tabs_offset += button_size.x;
	}

	auto& current_sub_tab = get_sub_tab_index(current_tab);

	auto sub_tabs_offset = SUB_TABS_INITIAL_OFFSET;
	auto sub_tab_index = 0;

	for (const auto& sub_tab_text : sub_tabs) {
		const auto text_size = render::measure_text(e_font::UI_13, sub_tab_text);

		const auto button_position = point_t(position.x + sub_tabs_offset, position.y + TABS_BAR_HEIGHT);
		const auto button_size = point_t(text_size.x + SUB_TAB_BUTTON_PADDING * 2, SUB_TABS_BAR_HEIGHT);
		const auto hovered = blocking_hash == 0 && input::is_in_bounds(button_position, button_position + button_size - 1);

		if (hovered && input::is_key_pressed(VK_LBUTTON))
			current_sub_tab = sub_tab_index;

		auto target_animation = 0.0f;

		if (current_sub_tab == sub_tab_index)
			target_animation = 1.0f;
		else if (hovered)
			target_animation = 0.75f;

		const auto sub_tab_animation = handle_animation(get_sub_tab_animation(sub_tab_index), target_animation, 0.06f);
		const auto sub_tab_color = color_t(150 + static_cast<int>(105.0f * sub_tab_animation));

		render::text(button_position + button_size / 2 - text_size / 2, e_font::UI_13, sub_tab_color, sub_tab_text);

		sub_tab_index++;
		sub_tabs_offset += button_size.x;
	}

	const auto username_text_size = render::measure_text(e_font::UI_13, cheat::get_username());
	const auto username_position = point_t(position.x + size.x - TABS_INITIAL_OFFSET / 2 - username_text_size.x + 20, position.y + TABS_BAR_HEIGHT / 2 - username_text_size.y / 2);
	const auto profile_hovered = blocking_hash == 0 && input::is_in_bounds(username_position.x - CONTENT_PADDING * 2 - 28, position.y, position.x + size.x, position.y + TABS_BAR_HEIGHT);

	auto target_animation = 0.0f;

	if (current_tab == tabs.size() - 1)
		target_animation = 1.0f;
	else if (profile_hovered)
		target_animation = 0.75f;

	const auto profile_animation = handle_animation(profile_tab_animation, target_animation, 0.06f);
	const auto profile_text_color = color_t(150 + static_cast<int>(105.0f * profile_animation));

	if (profile_hovered && input::is_key_pressed(VK_LBUTTON))
		current_tab = tabs.size() - 1;

	render::texture(username_position.x - CONTENT_PADDING - 28, position.y + TABS_BAR_HEIGHT / 2 - 14, 28, 28, e_texture::AVATAR_32, color_t(255));
	render::text(username_position, e_font::UI_13, profile_text_color, cheat::get_username());
}

bool gui::tab(std::wstring_view title) {
	tabs.push_back(title.data());

	const auto is_current_tab = current_tab + 1 == tabs.size();

	if (is_current_tab)
		parent_hash = FNV('0' + current_tab);

	return is_current_tab;
}

bool gui::begin_sub_tab(std::wstring_view title) {
	sub_tabs.push_back(title.data());

	const auto is_current_sub_tab = get_sub_tab_index(current_tab) + 1 == sub_tabs.size();

	if (is_current_sub_tab)
		parent_hash = FNV('0' + get_sub_tab_index(current_tab), FNV('0' + current_tab));

	return is_current_sub_tab;
}

void gui::end_sub_tab() {
	parent_hash = FNV('0' + current_tab);
}

bool gui::begin_group(std::wstring_view title) {
	parent_hash = FNV(title.data(), parent_hash);

	const auto group_width = working_area.x / 2 - CONTENT_PADDING / 2;
	const auto cursor_pos = pop_cursor_pos();

	auto& group_collapsed = get_group_collapsed(parent_hash);
	
	const auto group_height_animation = handle_animation(get_group_height_animation(parent_hash), group_collapsed ? 0.0f : 1.0f);
	const auto group_height = static_cast<int>(static_cast<float>(get_group_height(parent_hash)) * group_height_animation);

	const auto collapse_button_position = point_t(cursor_pos.x + group_width - GROUP_HEADER_PADDING - 20, cursor_pos.y + GROUP_HEADER_HEIGHT / 2 - 10);
	const auto collapse_button_hovered = blocking_hash == 0 && input::is_in_bounds(collapse_button_position - CONTENT_PADDING / 2, collapse_button_position + 20 + CONTENT_PADDING / 2);

	auto target_animation = 0.0f;

	if (collapse_button_hovered) {
		target_animation = 1.0f;

		if (input::is_key_pressed(VK_LBUTTON))
			group_collapsed = !group_collapsed;
	}

	render::push_clip(position.x, position.y + TABS_BAR_HEIGHT + SUB_TABS_BAR_HEIGHT + 1, position.x + size.x, position.y + size.y);

	render::fill_rect_rounded(cursor_pos.x, cursor_pos.y - 1, group_width, 6, 3, CORNER_TOP, config::get<color_t>(FNV_CT("ui.accent")).adjust_alpha(255));
	render::fill_rect_rounded(cursor_pos.x, cursor_pos.y, group_width, GROUP_HEADER_HEIGHT + group_height, 3, CORNER_ALL, color_t(30, 30, 34));
	render::fill_rect_rounded(cursor_pos.x, cursor_pos.y, group_width, GROUP_HEADER_HEIGHT, 3, CORNER_TOP, color_t(24, 24, 28));

	const auto title_text_size = render::measure_text(e_font::UI_14, title);
	const auto hover_animation = handle_animation(get_group_toggle_animation(parent_hash), target_animation, 0.06f);
	const auto hover_color = color_t(100, static_cast<int>(14.0f * hover_animation));

	render::text(cursor_pos.x + GROUP_HEADER_PADDING, cursor_pos.y + GROUP_HEADER_HEIGHT / 2 - title_text_size.y / 2, e_font::UI_14, color_t(255), title);

	render::fill_rect_rounded(collapse_button_position - CONTENT_PADDING / 2, 20 + CONTENT_PADDING, 3, CORNER_ALL, hover_color);
	render::texture(collapse_button_position, 20, group_collapsed ? e_texture::CHEVRON_DOWN_20 : e_texture::CHEVRON_UP_20, color_t(255));

	if (group_collapsed && group_height <= CONTENT_PADDING) {
		if (group_box_side_flip) {
			group_box_right_offset += group_height + GROUP_HEADER_HEIGHT + CONTENT_PADDING + 1;
		}
		else {
			group_box_left_offset += group_height + GROUP_HEADER_HEIGHT + CONTENT_PADDING + 1;
		}

		parent_hash = FNV('0' + get_sub_tab_index(current_tab), FNV('0' + current_tab));
		group_box_side_flip = !group_box_side_flip;

		push_cursor_pos(position.x + CONTENT_PADDING + (group_box_side_flip ? group_width + CONTENT_PADDING : 0), position.y + (group_box_side_flip ? group_box_right_offset : group_box_left_offset));

		return false;
	}
	else {
		push_cursor_pos(cursor_pos.x + CONTENT_PADDING, cursor_pos.y + GROUP_HEADER_HEIGHT + CONTENT_PADDING); // first element's position (used as reference for calculating group height)
		push_cursor_pos(cursor_pos.x + CONTENT_PADDING, cursor_pos.y + GROUP_HEADER_HEIGHT + CONTENT_PADDING); // copy of first element's position (so it can actually be used)

		render::push_clip(
			cursor_pos.x, std::min(cursor_pos.y + GROUP_HEADER_HEIGHT, position.y + size.y),
			cursor_pos.x + group_width, std::min(cursor_pos.y + GROUP_HEADER_HEIGHT + group_height, position.y + size.y)
		);

		working_area.x = group_width - CONTENT_PADDING * 2;
		working_area.y = 0;

		return true;
	}
}

void gui::end_group() {
	working_area.x = size.x - CONTENT_PADDING * 2;
	working_area.y = size.y - TABS_BAR_HEIGHT - SUB_TABS_BAR_HEIGHT - CONTENT_PADDING * 2;

	const auto new_cursor_pos = pop_cursor_pos();
	const auto original_cursor_pos = pop_cursor_pos();
	const auto difference = new_cursor_pos.y - original_cursor_pos.y;
	const auto group_width = (working_area.x + CONTENT_PADDING * 2) / 2 - CONTENT_PADDING / 2;

	get_group_height(parent_hash) = std::max(difference + CONTENT_PADDING, CONTENT_PADDING * 2);

	const auto group_height_visible = static_cast<int>(static_cast<float>(get_group_height(parent_hash)) * get_group_height_animation(parent_hash));

	if (group_box_side_flip) {
		group_box_right_offset += group_height_visible + GROUP_HEADER_HEIGHT + CONTENT_PADDING + 1;
	}
	else {
		group_box_left_offset += group_height_visible + GROUP_HEADER_HEIGHT + CONTENT_PADDING + 1;
	}

	parent_hash = FNV('0' + get_sub_tab_index(current_tab), FNV('0' + current_tab));
	group_box_side_flip = !group_box_side_flip;

	push_cursor_pos(position.x + CONTENT_PADDING + (group_box_side_flip ? group_width : 0), position.y + (group_box_side_flip ? group_box_right_offset : group_box_left_offset));

	render::reset_clip();

	attachables = {};
}

void gui::attach_color_picker(std::wstring_view title, color_t& value, bool alpha_bar) {
	attachables.push_back(std::make_shared<c_color_picker>(title, value, alpha_bar));
}

bool gui::checkbox(std::wstring_view title, bool& value) {
	const auto this_hash = FNV(title.data(), parent_hash);
	const auto cursor_pos = pop_cursor_pos();

	const auto title_text_size = render::measure_text(e_font::UI_13, title);
	const auto hovered_mouse = input::is_in_bounds(position, position + size) && input::is_in_bounds(cursor_pos, cursor_pos + CHECKBOX_BOX_SIZE) ||
		input::is_in_bounds(cursor_pos.x + CHECKBOX_BOX_SIZE + CONTENT_PADDING, cursor_pos.y, cursor_pos.x + CHECKBOX_BOX_SIZE + CONTENT_PADDING + title_text_size.x, cursor_pos.y + CHECKBOX_BOX_SIZE);

	if (blocking_hash == 0 && hovered_mouse && input::is_key_pressed(VK_LBUTTON)) {
		blocking_hash = this_hash;
	}
	else if (blocking_hash == this_hash && input::is_key_released(VK_LBUTTON)) {
		if (hovered_mouse)
			value = !value;

		blocking_hash = 0;
	}

	auto target_hover_animation = 0.0f;

	if (hovered_mouse || blocking_hash == this_hash)
		target_hover_animation = 1.0f;

	const auto hover_animation = handle_animation(get_element_hover_animation(this_hash), target_hover_animation, 0.06f);
	const auto toggle_animation = handle_animation(get_element_animation(this_hash), value ? 1.0f : 0.0f, 0.06f);
	const auto lightness_value = static_cast<int>(12.0f * hover_animation);
	const auto text_lightness_value = static_cast<int>(24.0f * hover_animation);
	const auto background_color = color_t(70 + lightness_value, 70 + lightness_value, 80 + lightness_value);

	render::fill_rect_rounded(cursor_pos - 1, CHECKBOX_BOX_SIZE + 2, 3, CORNER_ALL, color_t(22, 22, 26));
	render::fill_rect_rounded(cursor_pos, CHECKBOX_BOX_SIZE, 3, CORNER_ALL, background_color);
	render::fill_rect_rounded(cursor_pos, CHECKBOX_BOX_SIZE, 3, CORNER_ALL, config::get<color_t>(FNV_CT("ui.accent")).adjust_alpha(static_cast<int>(255.0f * toggle_animation)));

	render::texture(cursor_pos + CHECKBOX_BOX_SIZE / 2 - 6, 12, e_texture::CHECKMARK_12, color_t(255, static_cast<int>(255.0f * toggle_animation)));
	render::text(cursor_pos.x + CHECKBOX_BOX_SIZE + CONTENT_PADDING, cursor_pos.y + CHECKBOX_BOX_SIZE / 2 - title_text_size.y / 2, e_font::UI_13, color_t(210 + text_lightness_value), title);

	push_cursor_pos(cursor_pos.x, cursor_pos.y + CHECKBOX_BOX_SIZE + CONTENT_PADDING);

	for (auto& attachable : attachables) {
		attachable->handle();
	}

	attachables.clear();

	return value;
}

template <typename T>
T slider_impl(std::wstring_view title, T& value, T min_value, T max_value, std::wstring_view display_string);

int gui::slider(std::wstring_view title, int& value, int min_value, int max_value) {
	return slider_impl<int>(title, value, min_value, max_value, STR_ENC(L"{}"));
}

int gui::slider(std::wstring_view title, int& value, int min_value, int max_value, std::wstring_view display_string) {
	return slider_impl<int>(title, value, min_value, max_value, display_string);
}

int gui::slider(std::wstring_view title, int& value, int min_value, int max_value, const std::function<std::wstring_view(int)>& display_string_fn) {
	return slider_impl<int>(title, value, min_value, max_value, display_string_fn(value));
}

float gui::slider(std::wstring_view title, float& value, float min_value, float max_value) {
	return slider_impl<float>(title, value, min_value, max_value, STR_ENC(L"{:.1f}"));
}

float gui::slider(std::wstring_view title, float& value, float min_value, float max_value, std::wstring_view display_string) {
	return slider_impl<float>(title, value, min_value, max_value, display_string);
}

float gui::slider(std::wstring_view title, float& value, float min_value, float max_value, const std::function<std::wstring_view(float)>& display_string_fn) {
	return slider_impl<float>(title, value, min_value, max_value, display_string_fn(value));
}

template <typename T>
T slider_impl(std::wstring_view title, T& value, T min_value, T max_value, std::wstring_view display_string) {
	const auto this_hash = FNV(title.data(), parent_hash);
	const auto cursor_pos = pop_cursor_pos();

	const auto title_text_size = render::measure_text(e_font::UI_13, title);
	const auto fill_percent = std::clamp(math::map_range<float>(static_cast<float>(value), static_cast<float>(min_value), static_cast<float>(max_value), 0.0f, 1.0f), 0.0f, 1.0f);
	const auto slider_fill_width = static_cast<int>(fill_percent * working_area.x);
	const auto slider_handle_pos = point_t(cursor_pos.x + slider_fill_width - SLIDER_HANDLE_WIDTH / 2, cursor_pos.y + title_text_size.y + 4);

	const auto elevation_top = (SLIDER_HANDLE_HEIGHT - SLIDER_BAR_HEIGHT) / 2 + title_text_size.y + 4;
	const auto hovered = input::is_in_bounds(position, position + size) && (
		input::is_in_bounds(cursor_pos.x, cursor_pos.y + elevation_top, cursor_pos.x + working_area.x, cursor_pos.y + elevation_top + SLIDER_BAR_HEIGHT) ||
		input::is_in_bounds(slider_handle_pos, slider_handle_pos + point_t(SLIDER_HANDLE_WIDTH, SLIDER_HANDLE_HEIGHT)));

	if (blocking_hash == 0 && hovered && input::is_key_pressed(VK_LBUTTON)) {
		blocking_hash = this_hash;
	}
	else if (blocking_hash == this_hash && input::is_key_down(VK_LBUTTON)) {
		const auto mouse_relative_pos = input::get_mouse_pos() - cursor_pos;
		const auto new_value = math::map_range<float>(static_cast<float>(mouse_relative_pos.x), 0.0f, static_cast<float>(working_area.x), static_cast<float>(min_value), static_cast<float>(max_value));

		value = static_cast<T>(std::clamp<float>(new_value, static_cast<float>(min_value), static_cast<float>(max_value)));
	}
	else if (blocking_hash == this_hash && input::is_key_released(VK_LBUTTON)) {
		blocking_hash = 0;
	}

	auto target_hover_animation = 0.0f;

	if (hovered || blocking_hash == this_hash)
		target_hover_animation = 1.0f;

	const auto hover_animation = handle_animation(get_element_hover_animation(this_hash), target_hover_animation, 0.06f);
	const auto lightness_value = static_cast<int>(12.0f * hover_animation);
	const auto text_lightness_value = static_cast<int>(24.0f * hover_animation);
	const auto accent_lightness_value = static_cast<int>(30.0f * hover_animation);
	const auto background_color = color_t(70 + lightness_value, 70 + lightness_value, 80 + lightness_value);

	auto accent_color_copy = config::get<color_t>(FNV_CT("ui.accent")).adjust_alpha(255);

	// the perfect way to make the color lighter lmfao
	accent_color_copy.r = std::min(255, std::max(0, accent_color_copy.r + accent_lightness_value));
	accent_color_copy.g = std::min(255, std::max(0, accent_color_copy.g + accent_lightness_value));
	accent_color_copy.b = std::min(255, std::max(0, accent_color_copy.b + accent_lightness_value));

	const auto display_text = fmt::format(display_string, value);
	const auto display_text_size = render::measure_text(e_font::UI_11, display_text);

	render::fill_rect_rounded(cursor_pos.x - 1, cursor_pos.y + elevation_top - 1, working_area.x + 2, SLIDER_BAR_HEIGHT + 2, 2, CORNER_ALL, color_t(22, 22, 26));
	render::fill_rect_rounded(cursor_pos.x, cursor_pos.y + elevation_top, working_area.x, SLIDER_BAR_HEIGHT, 2, CORNER_ALL, background_color);
	render::fill_rect_rounded(cursor_pos.x, cursor_pos.y + elevation_top, slider_fill_width, SLIDER_BAR_HEIGHT, 2, CORNER_ALL, accent_color_copy);
	
	render::fill_rect_rounded(slider_handle_pos - 1, point_t(SLIDER_HANDLE_WIDTH, SLIDER_HANDLE_HEIGHT) + 2, 2, CORNER_ALL, color_t(60, 60, 70));
	render::fill_rect_rounded(slider_handle_pos, point_t(SLIDER_HANDLE_WIDTH, SLIDER_HANDLE_HEIGHT), 2, CORNER_ALL, color_t(110));

	render::text(cursor_pos.x, cursor_pos.y, e_font::UI_13, color_t(210 + text_lightness_value), title);
	render::text(cursor_pos.x + working_area.x - display_text_size.x, cursor_pos.y, e_font::UI_11, color_t(210 + text_lightness_value), display_text);

	push_cursor_pos(cursor_pos.x, cursor_pos.y + title_text_size.y + std::max(SLIDER_BAR_HEIGHT, SLIDER_HANDLE_HEIGHT) + 2 + CONTENT_PADDING);

	return value;
}
