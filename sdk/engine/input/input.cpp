#include <imgui_impl_win32.h>
#include <imgui.h>
#include <windows.h>

#include "../../ui/ui.h"
#include "../render/render.h"
#include "input.h"

extern long ImGui_ImplWin32_WndProcHandler(HWND, unsigned, unsigned, long);

static bool initialized = false;
static long original_window_proc_callback;
static HWND target_window_handle;

static long __stdcall window_proc_callback(HWND window_handle, unsigned message, unsigned wparam, long lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(window_handle, message, wparam, lparam))
		return 1;

	if (ui::is_active())
	{
		const auto should_discard = message == WM_MOUSEWHEEL || message == WM_MOUSEMOVE
			|| message == WM_LBUTTONDOWN || message == WM_LBUTTONUP || message == WM_LBUTTONDBLCLK
			|| message == WM_MBUTTONDOWN || message == WM_MBUTTONUP || message == WM_MBUTTONDBLCLK
			|| message == WM_RBUTTONDOWN || message == WM_RBUTTONUP || message == WM_RBUTTONDBLCLK
			|| message == WM_XBUTTONDOWN || message == WM_XBUTTONUP || message == WM_XBUTTONDBLCLK;

		if (should_discard)
			return 1;
	}

	return CallWindowProcA((WNDPROC) original_window_proc_callback, window_handle, message, wparam, lparam);
}

void input::init(HWND window_handle)
{
	if (initialized)
		return;

	initialized = true;
	original_window_proc_callback = SetWindowLongA(window_handle, GWL_WNDPROC, (LONG) &window_proc_callback);
	target_window_handle = window_handle;
}

void input::undo()
{
	if (!initialized)
		return;

	SetWindowLongA(target_window_handle, GWL_WNDPROC, original_window_proc_callback);

	initialized = false;
}

void input::set_cursor(int cursor)
{
	ImGui::SetMouseCursor(cursor);
}

void input::set_can_change_cursor(bool state)
{
	auto &io = ImGui::GetIO();

	if (state)
	{
		io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
	}
	else
	{
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	}
}

float input::get_mouse_wheel()
{
	return ImGui::GetIO().MouseWheel;
}

point_t input::get_mouse_pos()
{
	const auto &io = ImGui::GetIO();

	return { io.MousePos.x, io.MousePos.y };
}

point_t input::get_mouse_delta()
{
	const auto &io = ImGui::GetIO();

	return { io.MouseDelta.x, io.MouseDelta.y };
}

bool input::is_in_bounds(const point_t &min, const point_t &max)
{
	const auto &io = ImGui::GetIO();

	return io.MousePos.x >= min.x && io.MousePos.y >= min.y && io.MousePos.x <= max.x && io.MousePos.y <= max.y;
}

bool input::is_mouse_clicked(int key, bool repeat)
{
	return ImGui::IsMouseClicked(key, repeat);
}

bool input::is_mouse_double_clicked(int key)
{
	return ImGui::IsMouseDoubleClicked(key);
}

bool input::is_mouse_down(int key)
{
	return ImGui::IsMouseDown(key);
}

bool input::is_mouse_released(int key)
{
	return ImGui::IsMouseReleased(key);
}

bool input::is_key_pressed(int key, bool repeat)
{
	return ImGui::IsKeyPressed(key, repeat);
}

bool input::is_key_down(int key)
{
	return ImGui::IsKeyDown(key);
}

bool input::is_key_released(int key)
{
	return ImGui::IsKeyReleased(key);
}
