#include "../core/config.hpp"
#include "../core/interfaces.hpp"
#include "../gui/gui.hpp"
#include "../utils/hash.hpp"
#include "../utils/xorstr.hpp"
#include "menu.hpp"

void menu::frame() {
	const auto test_tab = []() {
		const auto test_sub_tab = []() {
			const auto test_group = []() {
				// gui::attach_color_picker(XOR(L"Color picker #1"), config::get<color_t>(FNV_CT("ui.accent")), true);
				// gui::attach_color_picker(XOR(L"Color picker #2"), config::get<color_t>(FNV_CT("ui.accent")), true);
				// gui::attach_color_picker(XOR(L"Color picker #3"), config::get<color_t>(FNV_CT("ui.accent")), true);
				gui::checkbox(XOR(L"Test checkbox"), config::get<bool>(FNV_CT("checkbox")));

				gui::slider(XOR(L"Trigger bot delay"), config::get<int>(FNV_CT("tbot.delay")), 0, 250, XOR(L"{}ms"));
				gui::slider(XOR(L"Field of view override"), config::get<float>(FNV_CT("visual.fov")), 45.0f, 120.0f, XOR(L"{:.0f}°"));
				gui::slider(XOR(L"Wall transparency"), config::get<int>(FNV_CT("visual.wt")), 0, 100, XOR(L"{}%"));

				gui::end_group();
			};

			if (gui::begin_group(XOR(L"Test group 1"))) test_group();
			if (gui::begin_group(XOR(L"Test group 2"))) test_group();
			if (gui::begin_group(XOR(L"Test group 3"))) test_group();
			if (gui::begin_group(XOR(L"Test group 4"))) test_group();

			gui::end_sub_tab();
		};

		if (gui::begin_sub_tab(XOR(L"Sub-tab 1"))) test_sub_tab();
		if (gui::begin_sub_tab(XOR(L"Sub-tab 2"))) test_sub_tab();
		if (gui::begin_sub_tab(XOR(L"Sub-tab 3"))) test_sub_tab();
		if (gui::begin_sub_tab(XOR(L"Sub-tab 4"))) test_sub_tab();
		if (gui::begin_sub_tab(XOR(L"Sub-tab 5"))) test_sub_tab();
	};

	if (gui::begin_window()) {
		if (gui::tab(XOR(L"Aimbot"))) test_tab();
		if (gui::tab(XOR(L"Visuals"))) test_tab();
		if (gui::tab(XOR(L"Miscellaneous"))) test_tab();
		if (gui::tab(XOR(L"Changers"))) test_tab();
		if (gui::tab(XOR(L"Presets"))) test_tab();

		gui::end_window();
	}
}
