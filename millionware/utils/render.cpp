#define STB_IMAGE_IMPLEMENTATION

#include <array>
#include <stack>
#include <stb_image.h>

#include "../core/cheat.hpp"
#include "../core/interfaces.hpp"
#include "../resources/textures.hpp"
#include "math.hpp"
#include "render.hpp"
#include "xorstr.hpp"

static std::stack<std::pair<point_t, point_t>> clip_rect_stack;
static std::array<vgui_font_t, static_cast<int>(e_font::MAX)> fonts;
static std::array<vgui_texture_t, static_cast<int>(e_texture::MAX)> textures;

constexpr vgui_font_t& get_font(e_font font) {
	return fonts[static_cast<int>(font)];
}

constexpr vgui_texture_t& get_texture(e_texture texture) {
	return textures[static_cast<int>(texture)];
}

inline void update_texture_from_png(e_texture texture, const uint8_t* data, int size, int width, int height) {
	int image_width, image_height, channels;

	stbi_set_flip_vertically_on_load(false);
	stbi_set_flip_vertically_on_load_thread(false);

	const auto image_data = stbi_load_from_memory(data, size, &image_width, &image_height, &channels, 4);

	interfaces::vgui_surface->update_texture(get_texture(texture), image_data, image_width, image_height);

	stbi_image_free(image_data);
}

std::vector<vertex_t> generate_rounded_rect_vertices(int x, int y, int width, int height, int radius, int corners) {
	std::vector<vertex_t> vertices;

	const auto radius_f = static_cast<float>(radius);
	const auto round_top_left = (corners & CORNER_TOP_LEFT) != 0;
	const auto round_top_right = (corners & CORNER_TOP_RIGHT) != 0;
	const auto round_bottom_left = (corners & CORNER_BOTTOM_LEFT) != 0;
	const auto round_bottom_right = (corners & CORNER_BOTTOM_RIGHT) != 0;

	vertices.reserve(
		(round_top_left ? 6 : 1) +
		(round_top_right ? 6 : 1) +
		(round_bottom_left ? 6 : 1) +
		(round_bottom_right ? 6 : 1)
	);

	for (auto i = 0; i < 4; i++) {
		const auto round_corner = i == 0 && round_top_right || i == 1 && round_bottom_right || i == 2 && round_bottom_left || i == 3 && round_top_left;

		if (!round_corner) {
			vertices.emplace_back(vertex_t(vector2_t(static_cast<float>(i < 2 ? x + width : x), static_cast<float>(i % 3 ? y + height : y)), 0.0f));
		}
		else {
			const auto vert_x = static_cast<float>(x + (i < 2 ? width - radius : radius));
			const auto vert_y = static_cast<float>(y + (i % 3 ? height - radius : radius));

			for (auto j = 0; j < 6; j++) {
				const auto angle = math::deg_to_rad(90.0f * i + 15.0f * j);

				vertices.emplace_back(vertex_t(vector2_t(vert_x + radius_f * std::sin(angle), vert_y - radius_f * std::cos(angle)), 0.0f));
			}
		}
	}

	return vertices;
}

void render::initialize() {
	constexpr uint8_t white_pixel[] = { 255, 255, 255, 255 };

	get_texture(e_texture::WHITE) = interfaces::vgui_surface->create_texture(true);
	get_texture(e_texture::MW_LOGO_32) = interfaces::vgui_surface->create_texture(true);
	get_texture(e_texture::QUESTION_MARK_22) = interfaces::vgui_surface->create_texture(true);
	get_texture(e_texture::LEGIT_22) = interfaces::vgui_surface->create_texture(true);
	get_texture(e_texture::VISUALS_22) = interfaces::vgui_surface->create_texture(true);
	get_texture(e_texture::MISC_22) = interfaces::vgui_surface->create_texture(true);
	get_texture(e_texture::CHANGERS_22) = interfaces::vgui_surface->create_texture(true);
	get_texture(e_texture::PROFILE_22) = interfaces::vgui_surface->create_texture(true);
	get_texture(e_texture::CHECKMARK_12) = interfaces::vgui_surface->create_texture(true);

	interfaces::vgui_surface->update_texture(get_texture(e_texture::WHITE), white_pixel, 1, 1);

	update_texture_from_png(e_texture::MW_LOGO_32, resources::textures::MW_LOGO_32, sizeof resources::textures::MW_LOGO_32, 32, 32);
	update_texture_from_png(e_texture::QUESTION_MARK_22, resources::textures::QUESTION_MARK_22, sizeof resources::textures::QUESTION_MARK_22, 22, 22);
	update_texture_from_png(e_texture::LEGIT_22, resources::textures::LEGIT_22, sizeof resources::textures::LEGIT_22, 22, 22);
	update_texture_from_png(e_texture::VISUALS_22, resources::textures::VISUALS_22, sizeof resources::textures::VISUALS_22, 22, 22);
	update_texture_from_png(e_texture::MISC_22, resources::textures::MISC_22, sizeof resources::textures::MISC_22, 22, 22);
	update_texture_from_png(e_texture::CHANGERS_22, resources::textures::CHANGERS_22, sizeof resources::textures::CHANGERS_22, 22, 22);
	update_texture_from_png(e_texture::PROFILE_22, resources::textures::PROFILE_22, sizeof resources::textures::PROFILE_22, 22, 22);
	update_texture_from_png(e_texture::CHECKMARK_12, resources::textures::CHECKMARK_12, sizeof resources::textures::CHECKMARK_12, 12, 12);

	refresh_fonts();
}

void render::refresh_fonts() {
	get_font(e_font::UI_REGULAR) = interfaces::vgui_surface->create_font();
	get_font(e_font::UI_GROUP) = interfaces::vgui_surface->create_font();

	interfaces::vgui_surface->set_font_glyph_set(get_font(e_font::UI_REGULAR), XOR("Segoe UI"), 16, 500, 0, 0, FONT_FLAG_ANTIALIAS);
	interfaces::vgui_surface->set_font_glyph_set(get_font(e_font::UI_GROUP), XOR("Segoe UI"), 14, 700, 0, 0, FONT_FLAG_ANTIALIAS);

	interfaces::engine_client->get_screen_size(cheat::screen_size.x, cheat::screen_size.y);

	reset_clip();
}

void render::reset_clip() {
	while (!clip_rect_stack.empty()) {
		clip_rect_stack.pop();
	}

	interfaces::vgui_surface->clipping_enabled = true;
	interfaces::vgui_surface->set_clip_rect(0, 0, cheat::screen_size.x, cheat::screen_size.y);
	interfaces::vgui_surface->clipping_enabled = false;
}

void render::pop_clip() {
	if (!clip_rect_stack.empty())
		clip_rect_stack.pop();

	if (!clip_rect_stack.empty()) {
		const auto& [min, max] = clip_rect_stack.top();

		interfaces::vgui_surface->clipping_enabled = true;
		interfaces::vgui_surface->set_clip_rect(min.x, min.y, max.x, max.y);
	}
	else {
		interfaces::vgui_surface->clipping_enabled = true;
		interfaces::vgui_surface->set_clip_rect(0, 0, cheat::screen_size.x, cheat::screen_size.y);
		interfaces::vgui_surface->clipping_enabled = false;
	}
}

void render::push_clip(int x1, int y1, int x2, int y2) {
	clip_rect_stack.push(std::make_pair(point_t(x1, y1), point_t(x2, y2)));

	interfaces::vgui_surface->clipping_enabled = true;
	interfaces::vgui_surface->set_clip_rect(x1, y1, x2, y2);
}

void render::push_clip(const point_t& position, const point_t& size) {
	push_clip(position.x, position.y, position.x + size.x, position.y + size.y);
}

void render::clipped(int x1, int y1, int x2, int y2, const std::function<void()>& callback) {
	push_clip(x1, y1, x2, y2);

	callback();

	pop_clip();
}

void render::clipped(const point_t& position, const point_t& size, const std::function<void()>& callback) {
	push_clip(position, size);

	callback();

	pop_clip();
}

void render::line(int x1, int y1, int x2, int y2, const color_t& color) {
	interfaces::vgui_surface->set_draw_color(color);
	interfaces::vgui_surface->draw_line(x1, y1, x2, y2);
}

void render::rect(int x, int y, int width, int height, const color_t& color) {
	interfaces::vgui_surface->set_draw_color(color);
	interfaces::vgui_surface->draw_rectangle(x, y, width, height);
}

void render::rect_rounded(int x, int y, int width, int height, int radius, int corners, const color_t& color) {
	const auto vertices = generate_rounded_rect_vertices(x, y, width, height, radius, corners);

	interfaces::vgui_surface->set_texture(get_texture(e_texture::WHITE));
	interfaces::vgui_surface->set_draw_color(color);
	interfaces::vgui_surface->draw_textured_polyline(vertices.data(), vertices.size());
}

void render::fill_rect(int x, int y, int width, int height, const color_t& color) {
	interfaces::vgui_surface->set_draw_color(color);
	interfaces::vgui_surface->fill_rectangle(x, y, width, height);
}

void render::fill_rect_rounded(int x, int y, int width, int height, int radius, int corners, const color_t& color) {
	const auto vertices = generate_rounded_rect_vertices(x, y, width, height, radius, corners);

	interfaces::vgui_surface->set_texture(get_texture(e_texture::WHITE));
	interfaces::vgui_surface->set_draw_color(color);
	interfaces::vgui_surface->draw_textured_polygon(vertices.data(), vertices.size());
}

void render::gradient_h(int x, int y, int width, int height, const color_t& start_color, const color_t& end_color) {
	interfaces::vgui_surface->set_draw_color(start_color);
	interfaces::vgui_surface->fill_faded_rectangle(x, y, width, height, 255, 255, true);

	interfaces::vgui_surface->set_draw_color(end_color);
	interfaces::vgui_surface->fill_faded_rectangle(x, y, width, height, 0, 255, true);
}

void render::gradient_v(int x, int y, int width, int height, const color_t& start_color, const color_t& end_color) {
	interfaces::vgui_surface->set_draw_color(start_color);
	interfaces::vgui_surface->fill_faded_rectangle(x, y, width, height, 255, 255, false);

	interfaces::vgui_surface->set_draw_color(end_color);
	interfaces::vgui_surface->fill_faded_rectangle(x, y, width, height, 0, 255, false);
}

void render::gradient_h_rounded(int x, int y, int width, int height, int radius, int corners, const color_t& start_color, const color_t& end_color) {
	const auto round_top_left = (corners & CORNER_TOP_LEFT) != 0;
	const auto round_top_right = (corners & CORNER_TOP_RIGHT) != 0;
	const auto round_bottom_left = (corners & CORNER_BOTTOM_LEFT) != 0;
	const auto round_bottom_right = (corners & CORNER_BOTTOM_RIGHT) != 0;

	round_top_left ? fill_rect_rounded(x, y, radius, radius, radius, CORNER_TOP_LEFT, start_color) : fill_rect(x, y, radius, radius, start_color);
	round_top_right ? fill_rect_rounded(x + width - radius, y, radius, radius, radius, CORNER_TOP_RIGHT, end_color) : fill_rect(x + width - radius, y, radius, radius, end_color);
	round_bottom_left ? fill_rect_rounded(x, y + height - radius, radius, radius, radius, CORNER_BOTTOM_LEFT, start_color) : fill_rect(x, y + height - radius, radius, radius, start_color);
	round_bottom_right ? fill_rect_rounded(x + width - radius, y + height - radius, radius, radius, radius, CORNER_BOTTOM_RIGHT, end_color) : fill_rect(x + width - radius, y + height - radius, radius, radius, end_color);

	fill_rect(x, y + radius, radius, height - radius * 2, start_color);
	fill_rect(x + width - radius, y + radius, radius, height - radius * 2, end_color);

	gradient_h(x + radius, y, width - radius * 2, height, start_color, end_color);
}

void render::gradient_v_rounded(int x, int y, int width, int height, int radius, int corners, const color_t& start_color, const color_t& end_color) {
	const auto round_top_left = (corners & CORNER_TOP_LEFT) != 0;
	const auto round_top_right = (corners & CORNER_TOP_RIGHT) != 0;
	const auto round_bottom_left = (corners & CORNER_BOTTOM_LEFT) != 0;
	const auto round_bottom_right = (corners & CORNER_BOTTOM_RIGHT) != 0;

	round_top_left ? fill_rect_rounded(x, y, radius, radius, radius, CORNER_TOP_LEFT, start_color) : fill_rect(x, y, radius, radius, start_color);
	round_top_right ? fill_rect_rounded(x + width - radius, y, radius, radius, radius, CORNER_TOP_RIGHT, start_color) : fill_rect(x + width - radius, y, radius, radius, start_color);
	round_bottom_left ? fill_rect_rounded(x, y + height - radius, radius, radius, radius, CORNER_BOTTOM_LEFT, end_color) : fill_rect(x, y + height - radius, radius, radius, end_color);
	round_bottom_right ? fill_rect_rounded(x + width - radius, y + height - radius, radius, radius, radius, CORNER_BOTTOM_RIGHT, end_color) : fill_rect(x + width - radius, y + height - radius, radius, radius, end_color);

	fill_rect(x + radius, y, width - radius * 2, radius, start_color);
	fill_rect(x + radius, y + height - radius, width - radius * 2, radius, end_color);

	gradient_v(x, y + radius, width, height - radius * 2, start_color, end_color);
}

void render::texture(int x, int y, int width, int height, e_texture texture, const color_t& color) {
	interfaces::vgui_surface->set_draw_color(color);
	interfaces::vgui_surface->set_texture(get_texture(texture));
	interfaces::vgui_surface->draw_textured_rect(x, y, width, height);
}

void render::triangle(int x1, int y1, int x2, int y2, int x3, int y3, const color_t& color) {
	const vertex_t vertices[] = {
		{ vector2_t(static_cast<float>(x1), static_cast<float>(y1)), vector2_t(0.0f) },
		{ vector2_t(static_cast<float>(x2), static_cast<float>(y2)), vector2_t(0.0f) },
		{ vector2_t(static_cast<float>(x3), static_cast<float>(y3)), vector2_t(0.0f) },
	};

	interfaces::vgui_surface->set_texture(get_texture(e_texture::WHITE));
	interfaces::vgui_surface->set_draw_color(color);
	interfaces::vgui_surface->draw_textured_polygon(vertices, 3);
}

void render::triangle_outline(int x1, int y1, int x2, int y2, int x3, int y3, const color_t& color) {
	const vertex_t vertices[] = {
		{ vector2_t(static_cast<float>(x1), static_cast<float>(y1)), vector2_t(0.0f) },
		{ vector2_t(static_cast<float>(x2), static_cast<float>(y2)), vector2_t(0.0f) },
		{ vector2_t(static_cast<float>(x3), static_cast<float>(y3)), vector2_t(0.0f) },
	};

	interfaces::vgui_surface->set_texture(get_texture(e_texture::WHITE));
	interfaces::vgui_surface->set_draw_color(color);
	interfaces::vgui_surface->draw_textured_polyline(vertices, 3);
}

void render::text(int x, int y, e_font font, const color_t& color, std::wstring_view string) {
	interfaces::vgui_surface->set_text_color(color);
	interfaces::vgui_surface->set_text_font(get_font(font));
	interfaces::vgui_surface->set_text_pos(x, y);
	interfaces::vgui_surface->render_text(string, string.size());
}

void render::text(int x, int y, e_font font, const color_t& color, std::string_view string) {
	text(x, y, font, color, std::wstring(string.begin(), string.end()));
}

void render::line(const point_t& start, const point_t& end, const color_t& color) {
	line(start.x, start.y, end.x, end.y, color);
}

void render::rect(const point_t& position, const point_t& size, const color_t& color) {
	rect(position.x, position.y, size.x, size.y, color);
}

void render::rect_rounded(const point_t& position, const point_t& size, int radius, int corners, const color_t& color) {
	rect_rounded(position.x, position.y, size.x, size.y, radius, corners, color);
}

void render::fill_rect(const point_t& position, const point_t& size, const color_t& color) {
	fill_rect(position.x, position.y, size.x, size.y, color);
}

void render::fill_rect_rounded(const point_t& position, const point_t& size, int radius, int corners, const color_t& color) {
	fill_rect_rounded(position.x, position.y, size.x, size.y, radius, corners, color);
}

void render::gradient_h(const point_t& position, const point_t& size, const color_t& start_color, const color_t& end_color) {
	gradient_h(position.x, position.y, size.x, size.y, start_color, end_color);
}

void render::gradient_v(const point_t& position, const point_t& size, const color_t& start_color, const color_t& end_color) {
	gradient_v(position.x, position.y, size.x, size.y, start_color, end_color);
}

void render::gradient_h_rounded(const point_t& position, const point_t& size, int radius, int corners, const color_t& start_color, const color_t& end_color) {
	gradient_h_rounded(position.x, position.y, size.x, size.y, radius, corners, start_color, end_color);
}

void render::gradient_v_rounded(const point_t& position, const point_t& size, int radius, int corners, const color_t& start_color, const color_t& end_color) {
	gradient_v_rounded(position.x, position.y, size.x, size.y, radius, corners, start_color, end_color);
}

void render::texture(const point_t& position, const point_t& size, e_texture texture_, const color_t& tint) {
	texture(position.x, position.y, size.x, size.y, texture_, tint);
}

void render::triangle(const point_t& point1, const point_t& point2, const point_t& point3, const color_t& color) {
	triangle(point1.x, point1.y, point2.x, point2.y, point3.x, point3.y, color);
}

void render::triangle_outline(const point_t& point1, const point_t& point2, const point_t& point3, const color_t& color) {
	triangle_outline(point1.x, point1.y, point2.x, point2.y, point3.x, point3.y, color);
}

void render::text(const point_t& position, e_font font, const color_t& color, std::wstring_view string) {
	text(position.x, position.y, font, color, string);
}

void render::text(const point_t& position, e_font font, const color_t& color, std::string_view string) {
	text(position.x, position.y, font, color, string);
}

point_t render::measure_text(e_font font, std::wstring_view string) {
	int width, height;

	interfaces::vgui_surface->calculate_text_size(get_font(font), string, width, height);

	return point_t(width, height);
}

point_t render::measure_text(e_font font, std::string_view string) {
	return measure_text(font, std::wstring(string.begin(), string.end()));
}
