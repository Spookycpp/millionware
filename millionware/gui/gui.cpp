#define NOMINMAX

#include <algorithm>

#include "../core/cheat.hpp"
#include "../core/interfaces.hpp"
#include "../utils/hash.hpp"
#include "../utils/input.hpp"
#include "../utils/render.hpp"
#include "gui.hpp"

constexpr auto WINDOW_SIDEBAR_SIZE = 50;
constexpr auto WINDOW_CONTENT_PADDING = 8;
constexpr auto WINDOW_TABS_HEIGHT = 28;

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

		ctx->position = point_t(48);
		ctx->min_size = point_t(600, 380);
		ctx->size = ctx->min_size;
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

	// calculate widest tab
	auto widest_tab_size = 0;

	for (const auto& tab_title : ctx->tabs) {
		const auto tab_size = render::measure_text(e_font::UI_REGULAR, tab_title);

		if (tab_size.x > widest_tab_size)
			widest_tab_size = tab_size.x;
	}

	// handle dragging
	const auto hovered_dragging_region = input::is_in_bounds(ctx->position, ctx->position + WINDOW_SIDEBAR_SIZE) ||
		input::is_in_bounds(
			ctx->position.x + WINDOW_SIDEBAR_SIZE + WINDOW_CONTENT_PADDING + (widest_tab_size + WINDOW_CONTENT_PADDING * 2) * ctx->tabs.size(), ctx->position.y,
			ctx->position.x + ctx->size.x, ctx->position.y + WINDOW_TABS_HEIGHT + WINDOW_CONTENT_PADDING);

	if (!ctx->is_being_dragged && hovered_dragging_region && input::is_key_pressed(VK_LBUTTON)) {
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
	if (!ctx->is_being_resized && input::is_in_bounds(ctx->position + ctx->size - WINDOW_CONTENT_PADDING, ctx->position + ctx->size) && input::is_key_pressed(VK_LBUTTON)) {
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
	render::fill_rect_rounded(ctx->position - 1, ctx->size + 2, 3, CORNER_ALL, color_t(64, 64, 70));
	render::fill_rect_rounded(ctx->position, ctx->size, 3, CORNER_ALL, color_t(32, 32, 35));
	render::fill_rect_rounded(ctx->position.x, ctx->position.y, WINDOW_SIDEBAR_SIZE, ctx->size.y, 3, CORNER_LEFT, color_t(46, 46, 50));
	render::fill_rect_rounded(ctx->position.x, ctx->position.y, WINDOW_SIDEBAR_SIZE, WINDOW_SIDEBAR_SIZE, 3, CORNER_TOP_LEFT, color_t(64, 64, 70));

	render::texture(ctx->position + WINDOW_SIDEBAR_SIZE / 2 - 16, 32, e_texture::MW_LOGO_32, color_t(240));

	// set up state for layout calculation
	ctx->is_calculating_layout = false;
	ctx->working_area = point_t(ctx->size.x - WINDOW_SIDEBAR_SIZE - WINDOW_CONTENT_PADDING * 2, ctx->size.y - WINDOW_TABS_HEIGHT - WINDOW_CONTENT_PADDING * 3);
	ctx->cursor_pos = point_t(ctx->position.x + WINDOW_SIDEBAR_SIZE + WINDOW_CONTENT_PADDING, ctx->position.y + WINDOW_TABS_HEIGHT + WINDOW_CONTENT_PADDING * 2);
	ctx->inline_cursor_pos = -1;

	callback();

	// draw tabs and categories
	render::line(ctx->position.x + WINDOW_SIDEBAR_SIZE + WINDOW_CONTENT_PADDING, ctx->position.y + WINDOW_CONTENT_PADDING + WINDOW_TABS_HEIGHT,
		ctx->position.x + ctx->size.x - WINDOW_CONTENT_PADDING, ctx->position.y + WINDOW_CONTENT_PADDING + WINDOW_TABS_HEIGHT, color_t(46, 46, 50));

	for (auto i = 0u; i < ctx->tabs.size(); i++) {
		const auto tab_size = render::measure_text(e_font::UI_REGULAR, ctx->tabs[i]);
		const auto tab_width = widest_tab_size + WINDOW_CONTENT_PADDING * 2;
		const auto tab_position = point_t(ctx->position.x + WINDOW_SIDEBAR_SIZE + WINDOW_CONTENT_PADDING + tab_width * i, ctx->position.y + WINDOW_CONTENT_PADDING);
		const auto tab_box_size = point_t(tab_width, WINDOW_TABS_HEIGHT);
		const auto hovered = input::is_in_bounds(tab_position, tab_position + tab_box_size);

		if (hovered && input::is_key_released(VK_LBUTTON))
			ctx->current_tab = i;

		const auto tab_animation = handle_animation(ctx->tab_animation(i), ctx->current_tab == i ? 1.0f : hovered ? 0.5f : 0.0f, 0.06f);
		const auto line_animation = handle_animation(ctx->tab_line(i), ctx->current_tab == i ? 1.0f : 0.0f, 0.1f);

		const auto tab_color = 120 + static_cast<int>(120.0f * tab_animation);
		const auto line_color = 46 + static_cast<int>(14.0f * line_animation);

		render::line(tab_position.x, tab_position.y + tab_box_size.y, tab_position.x + tab_box_size.x, tab_position.y + tab_box_size.y, color_t(line_color));
		render::text(tab_position.x + tab_box_size.x / 2 - tab_size.x / 2, ctx->position.y + (WINDOW_TABS_HEIGHT + WINDOW_CONTENT_PADDING) / 2 - tab_size.y / 2, e_font::UI_REGULAR, color_t(tab_color), ctx->tabs[i]);
	}

	for (auto i = 0u; i < ctx->categories.size(); i++) {
		auto& category_index = ctx->current_category(ctx->current_tab);

		const auto category_position = point_t(ctx->position.x, ctx->position.y + WINDOW_SIDEBAR_SIZE * (i + 1));
		const auto hovered = input::is_in_bounds(category_position, category_position + WINDOW_SIDEBAR_SIZE);

		if (hovered && input::is_key_released(VK_LBUTTON))
			category_index = i;

		const auto category_animation = handle_animation(ctx->category_animation(i), category_index == i ? 1.0f : hovered ? 0.5f : 0.0f, 0.06f);
		const auto category_color = 120 + static_cast<int>(120.0f * category_animation);

		render::texture(category_position + WINDOW_SIDEBAR_SIZE / 2 - 11, 22, ctx->categories[i], category_color);
	}
}

void gui::tab(std::wstring_view title, const std::function<void()>& callback) {
	ctx->tabs.push_back(title.data());

	if (ctx->current_tab == ctx->tabs.size() - 1)
		callback();
}

void gui::category(e_texture icon, const std::function<void()>& callback) {
	ctx->categories.push_back(icon);

	if (ctx->current_category(ctx->current_tab) == ctx->categories.size() - 1)
		callback();
}
