#define NOMINMAX

#include <algorithm>
#include <fmt/format.h>

#include "../core/cheat.hpp"
#include "../core/config.hpp"
#include "../core/interfaces.hpp"
#include "../utils/hash.hpp"
#include "../utils/input.hpp"
#include "../utils/math.hpp"
#include "../utils/render.hpp"
#include "../utils/xorstr.hpp"
#include "gui.hpp"

constexpr auto WINDOW_SIDEBAR_SIZE = 44;
constexpr auto WINDOW_CONTENT_PADDING = 7;
constexpr auto WINDOW_TABS_HEIGHT = 28;

constexpr auto TOGGLE_SIZE = 12;
constexpr auto SLIDER_SIZE = 5;

static std::shared_ptr<window_context_t> ctx = nullptr;

inline float handle_animation(float& state, float target, float rate = 0.035f) {
	const auto interval = (1.0f / rate) * interfaces::global_vars->frame_time;
	const auto delta = target - state;

	return state = std::clamp(state + delta * interval, 0.0f, 1.0f);
}

void gui::initialize() {
	ctx = std::make_shared<window_context_t>();
	{
		ctx->is_open = true;
		ctx->is_being_dragged = false;
		ctx->is_being_resized = false;
		ctx->current_tab = 0;

		ctx->position = point_t(96, 64);
		ctx->min_size = point_t(600, 380);
		ctx->size = ctx->min_size;

		ctx->parent_hash = 0;
		ctx->blocking_hash = 0;

		ctx->group_box_flip = false;
		ctx->group_box_left_offset = 0;
		ctx->group_box_right_offset = 0;
	}
}

std::shared_ptr<window_context_t> gui::get_context() {
	return ctx;
}

void gui::window(std::wstring_view title, const std::function<void()>& callback) {
	if (input::is_key_pressed(VK_INSERT)) {
		if (ctx->is_open) {
			ctx->is_being_dragged = false;
			ctx->is_being_resized = false;
		}

		ctx->is_open = !ctx->is_open;
	}

	if (!ctx->is_open)
		return;

	// calculate widest category
	auto categories_width = 0;

	for (const auto& category_title : ctx->categories) {
		const auto category_size = render::measure_text(e_font::UI_REGULAR, category_title);

		categories_width += category_size.x + WINDOW_CONTENT_PADDING * 3;
	}

	// handle dragging
	const auto hovered_dragging_region = ctx->blocking_hash == 0 && (input::is_in_bounds(ctx->position, ctx->position + WINDOW_SIDEBAR_SIZE) || input::is_in_bounds(
		ctx->position.x + WINDOW_SIDEBAR_SIZE + WINDOW_CONTENT_PADDING + categories_width, ctx->position.y,
		ctx->position.x + ctx->size.x, ctx->position.y + WINDOW_TABS_HEIGHT + WINDOW_CONTENT_PADDING));

	if (ctx->blocking_hash == 0 && !ctx->is_being_dragged && hovered_dragging_region && input::is_key_pressed(VK_LBUTTON)) {
		ctx->is_being_dragged = true;
		ctx->interaction_start_point = ctx->position;
		ctx->interaction_mouse_pos = input::get_mouse_pos();
	}
	else if (ctx->is_being_dragged && input::is_key_down(VK_LBUTTON)) {
		const auto new_position = ctx->interaction_start_point + input::get_mouse_pos() - ctx->interaction_mouse_pos;

		ctx->position.x = std::max(WINDOW_CONTENT_PADDING, std::min(cheat::screen_size.x - ctx->size.x - WINDOW_CONTENT_PADDING, new_position.x));
		ctx->position.y = std::max(WINDOW_CONTENT_PADDING, std::min(cheat::screen_size.y - ctx->size.y - WINDOW_CONTENT_PADDING, new_position.y));
	}
	else if (ctx->is_being_dragged && input::is_key_released(VK_LBUTTON)) {
		ctx->is_being_dragged = false;
	}

	// handle resizing
	if (ctx->blocking_hash == 0 && !ctx->is_being_resized && input::is_in_bounds(ctx->position + ctx->size - WINDOW_CONTENT_PADDING, ctx->position + ctx->size) && input::is_key_pressed(VK_LBUTTON)) {
		ctx->is_being_resized = true;
		ctx->interaction_start_point = ctx->size;
		ctx->interaction_mouse_pos = input::get_mouse_pos();
	}
	else if (ctx->is_being_resized && input::is_key_down(VK_LBUTTON)) {
		const auto new_size = ctx->interaction_start_point + input::get_mouse_pos() - ctx->interaction_mouse_pos;

		ctx->size.x = std::min(std::max(new_size.x, ctx->min_size.x), cheat::screen_size.x - ctx->position.x - WINDOW_CONTENT_PADDING);
		ctx->size.y = std::min(std::max(new_size.y, ctx->min_size.y), cheat::screen_size.y - ctx->position.y - WINDOW_CONTENT_PADDING);
	}
	else if (ctx->is_being_resized && input::is_key_released(VK_LBUTTON)) {
		ctx->is_being_resized = false;
	}

	// clear up previous state
	ctx->tabs.clear();
	ctx->categories.clear();
	ctx->is_calculating_layout = false;

	// useful stuff ig
	const auto title_size = render::measure_text(e_font::UI_REGULAR, title);

	// draw window frame
	render::fill_rect_rounded(ctx->position - 1, ctx->size + 2, 3, CORNER_ALL, color_t(64));
	render::fill_rect_rounded(ctx->position, ctx->size, 3, CORNER_ALL, color_t(32));
	render::fill_rect_rounded(ctx->position.x, ctx->position.y, WINDOW_SIDEBAR_SIZE, ctx->size.y, 3, CORNER_LEFT, color_t(46));
	render::fill_rect_rounded(ctx->position.x, ctx->position.y, WINDOW_SIDEBAR_SIZE, WINDOW_SIDEBAR_SIZE, 3, CORNER_TOP_LEFT, color_t(64));

	render::texture(ctx->position + WINDOW_SIDEBAR_SIZE / 2 - 16, 32, e_texture::MW_LOGO_32, color_t(240));

	// set up state for layout calculation
	ctx->parent_hash = 0;
	ctx->is_calculating_layout = false;
	ctx->working_area = point_t(ctx->size.x - WINDOW_SIDEBAR_SIZE - WINDOW_CONTENT_PADDING * 2, ctx->size.y - WINDOW_TABS_HEIGHT - WINDOW_CONTENT_PADDING * 3);
	ctx->cursor_pos = point_t(ctx->position.x + WINDOW_SIDEBAR_SIZE + WINDOW_CONTENT_PADDING, ctx->position.y + WINDOW_TABS_HEIGHT + WINDOW_CONTENT_PADDING * 2);
	ctx->inline_cursor_pos = -1;
	ctx->group_box_flip = false;
	ctx->group_box_left_offset = 0;
	ctx->group_box_right_offset = 0;

	callback();

	// draw tabs and categories
	render::line(ctx->position.x + WINDOW_SIDEBAR_SIZE + WINDOW_CONTENT_PADDING, ctx->position.y + WINDOW_CONTENT_PADDING + WINDOW_TABS_HEIGHT,
		ctx->position.x + ctx->size.x - WINDOW_CONTENT_PADDING, ctx->position.y + WINDOW_CONTENT_PADDING + WINDOW_TABS_HEIGHT, color_t(46));

	for (auto i = 0u; i < ctx->tabs.size(); i++) {
		const auto tab_hash = FNV(FNV_CT("tab"), utils::FNV_BASIS + i);
		const auto tab_position = point_t(ctx->position.x, ctx->position.y + WINDOW_SIDEBAR_SIZE * (i + 1));
		const auto hovered = input::is_in_bounds(tab_position, tab_position + WINDOW_SIDEBAR_SIZE);

		if (hovered && input::is_key_pressed(VK_LBUTTON)) {
			ctx->blocking_hash = tab_hash;
		}
		else if (ctx->blocking_hash == tab_hash && input::is_key_released(VK_LBUTTON)) {
			if (hovered)
				ctx->current_tab = i;

			ctx->blocking_hash = 0;
		}

		auto target_animation = 0.0f;

		if (ctx->current_tab == i)
			target_animation = 1.0f;
		else if ((ctx->blocking_hash == 0 && !input::is_key_down(VK_LBUTTON) && hovered) || ctx->blocking_hash == tab_hash)
			target_animation = 0.5f;

		const auto tab_animation = handle_animation(ctx->tab_animation(i), target_animation, 0.06f);
		const auto tab_color = color_t(120 + static_cast<int>(120.0f * tab_animation));

		render::texture(tab_position + WINDOW_SIDEBAR_SIZE / 2 - 11, 22, ctx->tabs[i], tab_color);
	}

	auto category_draw_offset = 0;

	for (auto i = 0u; i < ctx->categories.size(); i++) {
		auto& category_index = ctx->current_category(ctx->current_tab);

		const auto category_hash = FNV(ctx->categories[i].data());
		const auto category_size = render::measure_text(e_font::UI_REGULAR, ctx->categories[i]);
		const auto category_width = category_size.x + WINDOW_CONTENT_PADDING * 3;
		const auto category_position = point_t(ctx->position.x + WINDOW_SIDEBAR_SIZE + WINDOW_CONTENT_PADDING + category_draw_offset, ctx->position.y + WINDOW_CONTENT_PADDING);
		const auto category_box_size = point_t(category_width, WINDOW_TABS_HEIGHT);
		const auto hovered = input::is_in_bounds(category_position, category_position + category_box_size);

		if (hovered && input::is_key_pressed(VK_LBUTTON)) {
			ctx->blocking_hash = category_hash;
		}
		else if (ctx->blocking_hash == category_hash && input::is_key_released(VK_LBUTTON)) {
			if (hovered)
				category_index = i;

			ctx->blocking_hash = 0;
		}

		auto target_animation = 0.0f;

		if (category_index == i)
			target_animation = 1.0f;
		else if ((ctx->blocking_hash == 0 && !input::is_key_down(VK_LBUTTON) && hovered) || ctx->blocking_hash == category_hash)
			target_animation = 0.5f;

		const auto category_animation = handle_animation(ctx->category_animation(i), target_animation, 0.06f);
		const auto line_animation = handle_animation(ctx->category_line(i), target_animation, 0.1f);

		const auto category_color = 120 + static_cast<int>(120.0f * category_animation);
		const auto line_color = 46 + static_cast<int>(14.0f * line_animation);

		render::line(category_position.x, category_position.y + category_box_size.y, category_position.x + category_box_size.x, category_position.y + category_box_size.y, color_t(line_color));
		render::text(category_position.x + category_box_size.x / 2 - category_size.x / 2, ctx->position.y + (WINDOW_TABS_HEIGHT + WINDOW_CONTENT_PADDING) / 2 - category_size.y / 2, e_font::UI_REGULAR, color_t(category_color), ctx->categories[i]);

		category_draw_offset += category_width;
	}
}

void gui::tab(e_texture icon, const std::function<void()>& callback) {
	ctx->tabs.push_back(icon);

	if (ctx->current_tab == ctx->tabs.size() - 1)
		callback();
}

void gui::category(std::wstring_view title, const std::function<void()>& callback) {
	ctx->categories.push_back(title.data());

	if (ctx->current_category(ctx->current_tab) == ctx->categories.size() - 1)
		callback();
}

void gui::group(std::wstring_view title, const std::function<void()>& callback) {
	const auto group_box_width = (ctx->working_area.x - 8) / 2;
	const auto base_position = point_t(ctx->position.x + WINDOW_SIDEBAR_SIZE + WINDOW_CONTENT_PADDING, ctx->position.y + WINDOW_TABS_HEIGHT + WINDOW_CONTENT_PADDING * 2);
	const auto box_position = point_t(base_position.x + (ctx->group_box_flip ? group_box_width + WINDOW_CONTENT_PADDING : 0), base_position.y + (ctx->group_box_flip ? ctx->group_box_right_offset : ctx->group_box_left_offset));

	const auto title_size = render::measure_text(e_font::UI_GROUP, title);
	const auto background_pos = point_t(box_position.x, box_position.y + title_size.y + 6);

	ctx->parent_hash += FNV(title.data());
	ctx->is_calculating_layout = true;

	ctx->cursor_pos = background_pos + WINDOW_CONTENT_PADDING;
	ctx->working_area.x = group_box_width - WINDOW_CONTENT_PADDING * 2;

	callback();

	ctx->is_calculating_layout = false;

	const auto background_size = point_t(group_box_width, std::max(ctx->cursor_pos.y - background_pos.y, 8));

	if (ctx->group_box_flip)
		ctx->group_box_right_offset += background_size.y + 30;
	else
		ctx->group_box_left_offset += background_size.y + 30;

	render::text(box_position.x + 4, box_position.y, e_font::UI_GROUP, color_t(180), title);

	render::fill_rect_rounded(background_pos - 1, background_size + 2, 3, CORNER_ALL, color_t(58));
	render::fill_rect_rounded(background_pos, background_size, 3, CORNER_ALL, color_t(46));

	ctx->cursor_pos = background_pos + WINDOW_CONTENT_PADDING;

	callback();

	ctx->working_area = point_t(ctx->size.x - WINDOW_SIDEBAR_SIZE - WINDOW_CONTENT_PADDING * 2, ctx->size.y - WINDOW_TABS_HEIGHT - WINDOW_CONTENT_PADDING * 3);
	ctx->cursor_pos = point_t(ctx->position.x + WINDOW_SIDEBAR_SIZE + WINDOW_CONTENT_PADDING, ctx->position.y + WINDOW_TABS_HEIGHT + WINDOW_CONTENT_PADDING * 2);
	ctx->group_box_flip = !ctx->group_box_flip;
}

void gui::checkbox(std::wstring_view title, uint32_t config_item) {
	const auto cursor_pos = ctx->cursor_pos;

	if (!ctx->is_calculating_layout) {
		auto& config_value = config::get<bool>(config_item);

		const auto this_hash = FNV(title.data(), ctx->parent_hash);
		const auto title_size = render::measure_text(e_font::UI_REGULAR, title);
		const auto title_position = point_t(cursor_pos.x + TOGGLE_SIZE + 10, cursor_pos.y + TOGGLE_SIZE / 2 - title_size.y / 2);
		const auto hovered_mouse = input::is_in_bounds(cursor_pos.x, cursor_pos.y, cursor_pos.x + TOGGLE_SIZE + 10, cursor_pos.y + TOGGLE_SIZE) || input::is_in_bounds(title_position, title_position + title_size);
		const auto hovered = !ctx->is_being_dragged && !ctx->is_being_resized && (ctx->blocking_hash == 0 || ctx->blocking_hash == this_hash) && hovered_mouse;

		if (hovered && input::is_key_pressed(VK_LBUTTON)) {
			ctx->blocking_hash = this_hash;
		}
		else if (ctx->blocking_hash == this_hash && input::is_key_released(VK_LBUTTON)) {
			if (hovered_mouse)
				config_value = !config_value;

			ctx->blocking_hash = 0;
		}

		auto target_hover_animation = 0.3f;
		auto target_toggle_animation = 0.0f;

		if (config_value) {
			target_hover_animation = 0.0f;
			target_toggle_animation = 1.0f;
		}

		if ((ctx->blocking_hash == 0 && !input::is_key_down(VK_LBUTTON) && hovered) || ctx->blocking_hash == this_hash) {
			target_hover_animation = 1.0f;
		}

		const auto hover_animation = handle_animation(ctx->element_hover_animation(this_hash), target_hover_animation, 0.06f);
		const auto toggle_animation = handle_animation(ctx->element_animation(this_hash), target_toggle_animation, 0.06f);

		const auto& accent_color = config::get<color_t>(FNV_CT("ui.accent"));
		const auto hover_color = color_t(12 + static_cast<int>(24.0f * hover_animation));
		const auto toggle_color = accent_color.adjust_alpha(static_cast<int>(255.0f * toggle_animation));

		render::fill_rect_rounded(cursor_pos - 1, TOGGLE_SIZE + 2, 3, CORNER_ALL, color_t(38));
		render::fill_rect_rounded(cursor_pos, TOGGLE_SIZE, 3, CORNER_ALL, hover_color);
		render::fill_rect_rounded(cursor_pos, TOGGLE_SIZE, 3, CORNER_ALL, toggle_color);

		if (config_value)
			render::texture(cursor_pos + TOGGLE_SIZE / 2 - 6, 12, e_texture::CHECKMARK_12, color_t(255, toggle_color.a));

		render::text(title_position, e_font::UI_REGULAR, color_t(180), title);
	}

	ctx->cursor_pos.y += TOGGLE_SIZE + WINDOW_CONTENT_PADDING;

	ctx->inline_cursor_pos = ctx->cursor_pos;

	ctx->inline_cursor_pos.x += ctx->working_area.x;
}

template <typename T = float>
void slider_impl(std::wstring_view title, uint32_t config_item, T min_value, T max_value, std::wstring_view format_string) {
	const auto this_hash = FNV(title.data(), ctx->parent_hash);
	const auto title_size = render::measure_text(e_font::UI_REGULAR, title);
	const auto group_box_width = ctx->working_area.x;
	const auto slider_position = point_t(ctx->cursor_pos.x, ctx->cursor_pos.y + title_size.y + 4);
	const auto slider_size = point_t(group_box_width, SLIDER_SIZE);

	if (!ctx->is_calculating_layout) {
		auto& config_value = config::get<T>(config_item);

		const auto slider_progress = math::map_range<float>(static_cast<float>(config_value), static_cast<float>(min_value), static_cast<float>(max_value), 0.0f, 1.0f);
		const auto slider_fill_width = std::clamp(static_cast<int>(static_cast<float>(slider_size.x) * slider_progress), 0, slider_size.x);
		const auto hovered_mouse = input::is_in_bounds(slider_position, slider_position + slider_size);
		const auto hovered = !ctx->is_being_dragged && !ctx->is_being_resized && (ctx->blocking_hash == 0 || ctx->blocking_hash == this_hash) && hovered_mouse;

		if (hovered && input::is_key_pressed(VK_LBUTTON)) {
			ctx->blocking_hash = this_hash;
		}
		else if (ctx->blocking_hash == this_hash && input::is_key_down(VK_LBUTTON)) {
			const auto mouse_relative_pos = input::get_mouse_pos() - slider_position;
			const auto new_value = math::map_range<float>(static_cast<float>(mouse_relative_pos.x), 0.0f, static_cast<float>(slider_size.x), static_cast<float>(min_value), static_cast<float>(max_value));

			config_value = static_cast<T>(std::clamp<float>(new_value, static_cast<float>(min_value), static_cast<float>(max_value)));
		}
		else if (ctx->blocking_hash == this_hash && input::is_key_released(VK_LBUTTON)) {
			ctx->blocking_hash = 0;
		}

		const auto value_label = fmt::format(format_string, config_value);
		const auto value_label_size = render::measure_text(e_font::UI_REGULAR, value_label);

		const auto target_hover_animation = ((ctx->blocking_hash == 0 && !input::is_key_down(VK_LBUTTON) && hovered) || ctx->blocking_hash == this_hash) ? 1.0f : 0.0f;
		const auto hover_animation = handle_animation(ctx->element_hover_animation(this_hash), target_hover_animation, 0.06f);
		const auto hover_color = color_t(200, static_cast<int>(50.0f * hover_animation));

		render::fill_rect_rounded(slider_position - 1, slider_size + 2, 3, CORNER_ALL, color_t(38));
		render::fill_rect_rounded(slider_position, slider_size, 3, CORNER_ALL, color_t(20));

		render::clipped(slider_position, point_t(slider_fill_width, slider_size.y), [&]() {
			render::fill_rect_rounded(slider_position, slider_size, 3, CORNER_ALL, config::get<color_t>(FNV_CT("ui.accent")).adjust_alpha(255));
			render::fill_rect_rounded(slider_position, slider_size, 3, CORNER_ALL, hover_color);
		});

		render::text(ctx->cursor_pos, e_font::UI_REGULAR, color_t(180), title);
		render::text(ctx->cursor_pos.x + slider_size.x - value_label_size.x, ctx->cursor_pos.y, e_font::UI_REGULAR, color_t(180), value_label);
	}

	ctx->cursor_pos.y = slider_position.y + SLIDER_SIZE + WINDOW_CONTENT_PADDING;

	ctx->inline_cursor_pos = ctx->cursor_pos;

	ctx->inline_cursor_pos.x += ctx->working_area.x;
}

void gui::slider(std::wstring_view title, uint32_t config_item, int min_value, int max_value) {
	slider_impl<int>(title, config_item, min_value, max_value, XOR(L"{}"));
}

void gui::slider(std::wstring_view title, uint32_t config_item, int min_value, int max_value, std::wstring_view format_string) {
	slider_impl<int>(title, config_item, min_value, max_value, format_string);
}

void gui::slider(std::wstring_view title, uint32_t config_item, int min_value, int max_value, const std::function<std::wstring_view(int)>& format_string_fn) {
	slider_impl<int>(title, config_item, min_value, max_value, format_string_fn(config::get<int>(config_item)));
}

void gui::slider(std::wstring_view title, uint32_t config_item, float min_value, float max_value) {
	slider_impl<float>(title, config_item, min_value, max_value, XOR(L"{:.1f}"));
}

void gui::slider(std::wstring_view title, uint32_t config_item, float min_value, float max_value, std::wstring_view format_string) {
	slider_impl<float>(title, config_item, min_value, max_value, format_string);
}

void gui::slider(std::wstring_view title, uint32_t config_item, float min_value, float max_value, const std::function<std::wstring_view(float)>& format_string_fn) {
	slider_impl<float>(title, config_item, min_value, max_value, format_string_fn(config::get<float>(config_item)));
}
