#include "debug_overlay.h"
#include "../render/render.h"
#include "../security/xorstr.h"

constexpr int overlay_height = 70;
constexpr int overlay_width = overlay_height * 4;

static std::vector<debug_overlay_t*> overlays;

debug_overlay_t::debug_overlay_t(std::string name_) {
	calls = 0;
	avg_per_sec = 0;
	name = name_;

	times.reserve((int)(overlay_width * 1.25f));
	times.clear();
}

void debug_overlay_t::add(uint64_t time) {
	std::lock_guard guard{ mutex };

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
	create_move = std::make_shared<debug_overlay_t>("create move");
	do_psfx = std::make_shared<debug_overlay_t>("do psfx");
	dme = std::make_shared<debug_overlay_t>("dme");

	fsn[0] = std::make_shared<debug_overlay_t>("fsn (start)");
	fsn[1] = std::make_shared<debug_overlay_t>("fsn (net update start)");
	fsn[2] = std::make_shared<debug_overlay_t>("fsn (net data start)");
	fsn[3] = std::make_shared<debug_overlay_t>("fsn (net data end)");
	fsn[4] = std::make_shared<debug_overlay_t>("fsn (net update end)");
	fsn[5] = std::make_shared<debug_overlay_t>("fsn (render start)");
	fsn[6] = std::make_shared<debug_overlay_t>("fsn (render end)");
	fsn[7] = std::make_shared<debug_overlay_t>("unknown");

	present[0] = std::make_shared<debug_overlay_t>("present (main)");
	present[1] = std::make_shared<debug_overlay_t>("present (lua back)");
	present[2] = std::make_shared<debug_overlay_t>("present (lua front)");

	push_notice = std::make_shared<debug_overlay_t>("push notice");
	send_datagram = std::make_shared<debug_overlay_t>("send datagram");
	write_user_cmd = std::make_shared<debug_overlay_t>("write user cmd");

	overlays.insert(overlays.end(),
		{
			create_move.get(),
			do_psfx.get(),
			fsn[0].get(),
			fsn[1].get(),
			fsn[2].get(),
			fsn[3].get(),
			fsn[4].get(),
			fsn[5].get(),
			fsn[6].get(),
			present[0].get(),
			present[1].get(),
			present[2].get(),
			push_notice.get(),
			send_datagram.get(),
			write_user_cmd.get(),
		});
}

void debug_overlay::draw() {
	auto screen_size = render::get_screen_size();

	auto next_goes_down = false;
	auto next_pos_x = screen_size.x - overlay_width * 2 - 32.0f;
	auto next_pos_y = 16.0f;

	for (auto i = 0; i < overlays.size(); i++) {
		auto overlay = overlays[i];

		std::lock_guard guard{ overlay->mutex };

		auto pos_x = next_pos_x;
		auto pos_y = next_pos_y;

		auto lowest_time = overlay->times.empty() ? 0 : *std::min_element(overlay->times.begin(), overlay->times.end());
		auto highest_time = overlay->times.empty() ? 0 : *std::max_element(overlay->times.begin(), overlay->times.end());

		auto text = std::format(xs("{} (min={:.2f}ms, max={:.2f}ms)"), overlay->name, (float)lowest_time / 1000.0f, (float)highest_time / 1000.0f);
		auto text_size = render::measure_text(text.c_str(), FONT_CEREBRI_SANS_MEDIUM_14);

		render::fill_rect({ pos_x - 4.0f, pos_y - 4.0f }, { overlay_width + 4.0f, overlay_height + 8.0f + text_size.y }, { 0, 0, 0, 100 }, 3.0f);
		render::draw_text({ pos_x, pos_y }, { 255, 255, 255, 255 }, text.c_str(), FONT_CEREBRI_SANS_MEDIUM_14);

		overlay->draw(pos_x, pos_y + 4.0f);

		if (next_goes_down) {
			next_pos_x = screen_size.x - overlay_width * 2 - 32.0f;
			next_pos_y += overlay_height + 8.0f + text_size.y + 8.0f;
		}
		else {
			next_pos_x += overlay_width + 16.0f;
		}

		next_goes_down ^= 1;
	}
}
