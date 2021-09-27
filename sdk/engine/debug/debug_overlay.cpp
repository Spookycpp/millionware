#include "debug_overlay.h"
#include "../render/render.h"
#include "../security/xorstr.h"

constexpr int overlay_height = 60;
constexpr int overlay_width = overlay_height * 3;

static std::vector<debug_overlay_t> overlays;

debug_overlay_t::debug_overlay_t(std::string name_) {
	calls = 0;
	avg_per_sec = 0;
	name = name_;

	times.reserve((int)(overlay_width * 1.25f));
	times.clear();
}

void debug_overlay_t::add(uint64_t time) {
	has_pushed = true;

	if (times.size() > overlay_width)
		times.erase(times.begin());

	times.push_back(time);
}

void debug_overlay_t::draw(float pos_x, float pos_y) {
	auto times_ptr = times.data();
	auto max_x = (int)times.size();
	auto base_x = pos_x + overlay_width - max_x;
	auto max_y = 0.0f;

	for (auto x = 0; x < overlay_width; x++) {
		if (x > max_x)
			break;

		auto y = (float)times_ptr[x] / 1000.0f;

		if (y > max_y)
			max_y = y + 4.0f;
	}

	for (auto x = 0; x < overlay_width; x++) {
		if (x > max_x)
			break;

		auto y = (float)times_ptr[x] / 1000.0f;
		auto actual_y = overlay_height - y;
		auto scaled_y = y / max_y;

		render::gradient_v({ base_x + x, pos_y + actual_y }, { 1.0f, scaled_y }, { 200, 200, 200, 150 }, { 200, 200, 200, 0 });
		render::fill_rect({ base_x + x, pos_y + actual_y }, { 1.0f, 2.0f }, { 255, 255, 255, 255 });
	}
}

debug_timer_t::debug_timer_t(debug_overlay_t* overlay_) {
	overlay = overlay_;
	start = std::chrono::steady_clock::now();
}

debug_timer_t::~debug_timer_t() {
	auto end = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	overlay->add(diff.count());
}

void debug_overlay::init() {
	overlays.emplace_back(debug_overlay_t{ "create move" });
	overlays.emplace_back(debug_overlay_t{ "do psfx" });
	overlays.emplace_back(debug_overlay_t{ "dme" });
	overlays.emplace_back(debug_overlay_t{ "fsn" });
	overlays.emplace_back(debug_overlay_t{ "present" });
	overlays.emplace_back(debug_overlay_t{ "push notice" });
	overlays.emplace_back(debug_overlay_t{ "send datagram" });
	overlays.emplace_back(debug_overlay_t{ "wucdtb" });

	create_move = &overlays[0];
	do_psfx = &overlays[1];
	dme = &overlays[2];
	fsn = &overlays[3];
	present = &overlays[4];
	push_notice = &overlays[5];
	send_datagram = &overlays[6];
	wucdtb = &overlays[7];
}

void debug_overlay::draw() {
	auto screen_size = render::get_screen_size();

	for (auto i = 0; i < overlays.size(); i++) {
		auto overlay = &overlays[i];
		auto text_size = render::measure_text(overlay->name.c_str(), FONT_CEREBRI_SANS_MEDIUM_14);

		auto pos_x = screen_size.x - overlay_width - 30.0f;
		auto pos_y = 16.0f + i * (overlay_height + 16.0f);

		auto lowest_time = overlay->times.empty() ? 0 : *std::min_element(overlay->times.begin(), overlay->times.end());
		auto highest_time = overlay->times.empty() ? 0 : *std::max_element(overlay->times.begin(), overlay->times.end());

		auto lowest_time_txt = std::format(xs("min: {:.2f}ms"), (float)lowest_time / 1000.0f);
		auto highest_time_txt = std::format(xs("max: {:.2f}ms"), (float)highest_time / 1000.0f);

		auto lowest_time_size = render::measure_text(lowest_time_txt.c_str(), FONT_CEREBRI_SANS_MEDIUM_14);
		auto highest_time_size = render::measure_text(highest_time_txt.c_str(), FONT_CEREBRI_SANS_MEDIUM_14);

		auto widest_text = std::max({ text_size.x, lowest_time_size.x, highest_time_size.x });

		render::fill_rect({ pos_x - widest_text - 12.0f, pos_y - 4.0f }, { overlay_width + widest_text + 16.0f, overlay_height + 8.0f }, { 0, 0, 0, 140 });
		render::draw_rect({ pos_x, pos_y }, { overlay_width, overlay_height }, { 200, 200, 200, 80 });

		render::draw_text({ pos_x - text_size.x - 8.0f, pos_y }, { 255, 255, 255, 255 }, overlay->name.c_str(), FONT_CEREBRI_SANS_MEDIUM_14);
		render::draw_text({ pos_x - lowest_time_size.x - 8.0f, pos_y + text_size.y + 4.0f }, { 255, 255, 255, 255 }, lowest_time_txt.c_str(), FONT_CEREBRI_SANS_MEDIUM_14);
		render::draw_text({ pos_x - highest_time_size.x - 8.0f, pos_y + text_size.y + lowest_time_size.y + 8.0f }, { 255, 255, 255, 255 }, highest_time_txt.c_str(), FONT_CEREBRI_SANS_MEDIUM_14);

		/*if (!overlay->has_pushed)
			overlay->add(0);*/

		overlay->has_pushed = false;
		overlay->draw(pos_x, pos_y);
	}
}
