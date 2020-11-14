#include "../utils/hash.hpp"
#include "../utils/render.hpp"
#include "gui.hpp"

constexpr static auto GUI_DIMMED_TEXT_COLOR = color_t(130);
constexpr static auto GUI_REGULAR_TEXT_COLOR = color_t(220);

constexpr static auto WINDOW_FRAME_BORDER_COLOR = color_t(36);
constexpr static auto WINDOW_FRAME_BACKGROUND_COLOR = color_t(20);
constexpr static auto WINDOW_FRAME_CORNER_RADIUS = 4;
constexpr static auto WINDOW_FRAME_CONTENT_PADDING = 6;
constexpr static auto WINDOW_HEADER_BACKGROUND_COLOR = color_t(36);
constexpr static auto WINDOW_HEADER_SIZE = 40;
constexpr static auto WINDOW_SIDEBAR_BACKGROUND_COLOR = color_t(28);
constexpr static auto WINDOW_SIDEBAR_SIZE = 135;

static std::unordered_map<uint32_t, window_context_t> windows;
static std::vector<uint32_t> window_order;

point_t window_context_t::pop() {
	const auto top = window_cursor_pos.top();

	window_cursor_pos.pop();

	return top;
}

void window_context_t::push(const point_t& pos) {
	window_cursor_pos.push(pos);
}

void window_context_t::push(int x, int y) {
	push({ x, y });
}

uint32_t window_context_t::do_hash(const char* string) {
	return parent_hash + HASH_FNV(string);
}

uint32_t window_context_t::do_hash(const wchar_t* string) {
	return parent_hash + HASH_FNV(string);
}

void window_context_t::window(std::wstring_view title, const std::function<void()>& callback) {
	if (!open)
		return;

	tabs.clear();
	categories.clear();

	while (!window_cursor_pos.empty())
		window_cursor_pos.pop();

	render::fill_rect_rounded(position - 1, size + 1, WINDOW_FRAME_CORNER_RADIUS, CORNER_ALL, WINDOW_FRAME_BORDER_COLOR);
	render::fill_rect_rounded(position, size, WINDOW_FRAME_CORNER_RADIUS, CORNER_ALL, WINDOW_FRAME_BACKGROUND_COLOR);
	render::fill_rect_rounded(position.x, position.y, size.x, WINDOW_HEADER_SIZE, WINDOW_FRAME_CORNER_RADIUS, CORNER_TOP, WINDOW_HEADER_BACKGROUND_COLOR);
	render::fill_rect_rounded(position.x, position.y + WINDOW_HEADER_SIZE, WINDOW_SIDEBAR_SIZE, size.y - WINDOW_HEADER_SIZE, WINDOW_FRAME_CORNER_RADIUS, CORNER_BOTTOM_LEFT, WINDOW_SIDEBAR_BACKGROUND_COLOR);

	const auto title_size = render::measure_text(e_font::SEGOE_UI_16, title);

	render::text(position.x + 16, position.y + WINDOW_HEADER_SIZE / 2 - title_size.y / 2, e_font::SEGOE_UI_16, GUI_REGULAR_TEXT_COLOR, title);

	push(position.x + WINDOW_SIDEBAR_SIZE + WINDOW_FRAME_CONTENT_PADDING, position.y + WINDOW_HEADER_SIZE + WINDOW_FRAME_CONTENT_PADDING);

	working_area_size.x = size.x - WINDOW_SIDEBAR_SIZE - WINDOW_FRAME_CONTENT_PADDING * 2;
	working_area_size.y = size.y - WINDOW_HEADER_SIZE - WINDOW_FRAME_CONTENT_PADDING * 2;

	std::invoke(callback);

	for (auto i = 0u, offset = 0u; i < tabs.size(); i++) {
		const auto tab_text = tabs[tabs.size() - i - 1];
		const auto tab_size = render::measure_text(e_font::SEGOE_UI_16, tab_text);
		const auto selected = current_tab == tabs.size() - i - 1;

		render::text(position.x + size.x - 16 - offset - tab_size.x, position.y + WINDOW_HEADER_SIZE / 2 - tab_size.y / 2,
			e_font::SEGOE_UI_16, selected ? GUI_REGULAR_TEXT_COLOR : GUI_DIMMED_TEXT_COLOR, tab_text);

		offset += tab_size.x + 20;
	}

	for (auto i = 0u, offset = 0u; i < categories.size(); i++) {
		const auto category_text = categories[i];
		const auto category_size = render::measure_text(e_font::SEGOE_UI_16, category_text);

		render::text(position.x + 24, position.y + WINDOW_HEADER_SIZE + 24 - category_size.y / 2 + offset,
			e_font::SEGOE_UI_16, tabs_selected_category[current_tab] == i ? GUI_REGULAR_TEXT_COLOR : GUI_DIMMED_TEXT_COLOR, category_text);

		offset += category_size.y + 8;
	}
}

void window_context_t::tab(std::wstring_view title, const std::function<void()>& callback) {
	tabs.push_back(title.data());

	if (tabs.size() == current_tab + 1)
		std::invoke(callback);
}

void window_context_t::category(std::wstring_view title, const std::function<void()>& callback) {
	if (tabs_selected_category.find(current_tab) == tabs_selected_category.end())
		tabs_selected_category.insert_or_assign(current_tab, 0);

	categories.push_back(title.data());

	if (categories.size() == tabs_selected_category[current_tab] + 1)
		std::invoke(callback);
}

window_context_t& gui::get_window(uint32_t hash) {
	if (windows.find(hash) == windows.end()) {
		window_context_t ctx;

		ctx.open = true;
		ctx.dragging = false;

		ctx.parent_hash = 0;
		ctx.blocking_hash = 0;
		ctx.current_tab = 0;

		ctx.position = point_t(48);
		ctx.size = point_t(600, 380);

		windows.insert({ hash, ctx });
		window_order.push_back(hash);
	}

	return windows[hash];
}
