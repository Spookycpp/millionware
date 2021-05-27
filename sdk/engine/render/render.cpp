#define STB_IMAGE_IMPLEMENTATION

#include <array>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_freetype.h>
#include <stb_image.h>
#include <string>
#include <unordered_map>

#include "../../resources/cerebri_sans_medium_ttf.h"
#include "../../resources/cerebri_sans_regular_ttf.h"
#include "../../resources/csgo_icons.h"
#include "../../resources/csgo_icons_ttf.h"
#include "../../resources/fa_brands_400_ttf.h"
#include "../../resources/fa_regular_400_ttf.h"
#include "../../resources/fa_solid_900_ttf.h"
#include "../../resources/font_awesome.h"
#include "../../resources/mw_logo_png.h"
#include "../../resources/transparency_checkerboard_png.h"
#include "../hash/hash.h"
#include "../security/xorstr.h"
#include "render.h"

static std::array<ImFont *, FONT_MAX> fonts;
static std::array<IDirect3DTexture9 *, TEXTURE_MAX> textures;

static bool initialized = false;
static HWND target_window_handle;
static IDirect3DDevice9 *d3d9_device;
static IDirect3DStateBlock9* state_block;
static ImDrawList *draw_list;

static bool get_system_font_path(std::string_view font_name, std::string &result)
{
	char windows_directory[MAX_PATH];

	if (!GetWindowsDirectoryA(windows_directory, MAX_PATH))
		return false;

	const auto fonts_directory = std::string(windows_directory).append(XORSTR("\\Fonts\\"));

	HKEY registry_key;

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, XORSTR("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"), 0, KEY_READ, &registry_key))
		return false;

	auto index = 0u;

	char value_name[MAX_PATH];
	char value_data[MAX_PATH];

	do
	{
		auto value_name_size = MAX_PATH;
		auto value_data_size = MAX_PATH;
		auto value_type = 0u;

		const auto error = RegEnumValueA(registry_key, index, value_name, (DWORD *) &value_name_size, nullptr, (DWORD *) &value_type, (BYTE *) value_data, (DWORD *) &value_data_size);

		index++;

		if (error == ERROR_NO_MORE_ITEMS)
		{
			RegCloseKey(registry_key);

			return false;
		}

		if (error || value_type != REG_SZ)
			continue;

		if (_strnicmp(font_name.data(), value_name, font_name.size()) != 0)
			continue;

		result = fonts_directory + std::string(value_data, value_data_size);

		RegCloseKey(registry_key);

		return true;
	} while (true);
}

static ImFont *create_from_system(ImGuiIO &io, std::string_view font_name, float size, int rasterizer_flags = 0, std::initializer_list<ImWchar> glyph_ranges = { })
{
	std::string font_path;

	if (!get_system_font_path(font_name, font_path))
		return nullptr;

	ImFontConfig font_config;

	font_config.FontBuilderFlags = rasterizer_flags;

	return io.Fonts->AddFontFromFileTTF(font_path.data(), size, &font_config, glyph_ranges.size() == 0 ? nullptr : glyph_ranges.begin());
}

static ImFont *create_from_ttf(ImGuiIO &io, const uint8_t *ttf_data, int ttf_data_size, float size, int rasterizer_flags = 0, std::initializer_list<ImWchar> glyph_ranges = { })
{
	ImFontConfig font_config;

	font_config.FontData = std::malloc(ttf_data_size);
	font_config.FontDataSize = ttf_data_size;
	font_config.SizePixels = size;
	font_config.GlyphRanges = glyph_ranges.size() == 0 ? nullptr : glyph_ranges.begin();
	font_config.FontBuilderFlags = rasterizer_flags;

	std::memcpy(font_config.FontData, ttf_data, ttf_data_size);

	return io.Fonts->AddFont(&font_config);
}

static IDirect3DTexture9 *create_from_png(const uint8_t *png_data, int png_data_size)
{
	int image_width, image_height, channels;

	const auto image_data = stbi_load_from_memory(png_data, png_data_size, &image_width, &image_height, &channels, 4);

	if (image_data == nullptr)
		return nullptr;

	IDirect3DTexture9 *texture;

	if (d3d9_device->CreateTexture(image_width, image_height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL) < 0)
		return nullptr;

	D3DLOCKED_RECT tex_locked_rect;

	if (texture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
		return nullptr;

	for (int y = 0; y < image_height; y++)
	{
		memcpy((unsigned char *) tex_locked_rect.pBits + tex_locked_rect.Pitch * y, image_data + image_width * 4 * y, image_width * 4);
	}

	texture->UnlockRect(0);

	stbi_image_free(image_data);

	return texture;
}

void render::init(HWND window, IDirect3DDevice9 *device)
{
	if (initialized)
		return;

	// Store the device reference
	target_window_handle = window;
	d3d9_device = device;

	device->CreateStateBlock(D3DSBT_ALL, &state_block);

	// Set up imgui context and colors style
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// Create fonts and build the font atlas
	//
	// @todo: Load font data from memory
	auto &io = ImGui::GetIO();

	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	fonts[FONT_VERDANA_12] = create_from_system(io, XORSTR("Verdana"), 13.0f);
	fonts[FONT_VERDANA_14] = create_from_system(io, XORSTR("Verdana"), 15.0f, ImGuiFreeTypeBuilderFlags_Bold | ImGuiFreeTypeBuilderFlags_MonoHinting);
	fonts[FONT_VERDANA_24] = create_from_system(io, XORSTR("Verdana"), 25.0f, ImGuiFreeTypeBuilderFlags_Bold | ImGuiFreeTypeBuilderFlags_MonoHinting);
    fonts[FONT_TAHOMA_11] = create_from_system(io, XORSTR("Tahoma"), 12.f, ImGuiFreeTypeBuilderFlags_Monochrome | ImGuiFreeTypeBuilderFlags_MonoHinting);
	fonts[FONT_TAHOMA_12] = create_from_system(io, XORSTR("Tahoma"), 13.f, ImGuiFreeTypeBuilderFlags_Bold);
	//fonts[FONT_SMALL_TEXT] = create_from_system(io, XORSTR("Small Fonts"), 10.f, ImGuiFreeTypeBuilderFlags_MonoHinting | ImGuiFreeTypeBuilderFlags_Monochrome);
	fonts[FONT_SMALL_TEXT] = create_from_system(io, XORSTR("Verdana"), 10.f, ImGuiFreeTypeBuilderFlags_Monochrome);
	fonts[FONT_CEREBRI_SANS_BOLD_13] = create_from_ttf(io, cerebri_sans_medium_ttf, sizeof(cerebri_sans_medium_ttf), 13.0f, ImGuiFreeTypeBuilderFlags_Bold);
	fonts[FONT_CEREBRI_SANS_MEDIUM_14] = create_from_ttf(io, cerebri_sans_medium_ttf, sizeof(cerebri_sans_medium_ttf), 14.0f);
	fonts[FONT_CEREBRI_SANS_MEDIUM_18] = create_from_ttf(io, cerebri_sans_medium_ttf, sizeof(cerebri_sans_medium_ttf), 18.0f);
	fonts[FONT_CEREBRI_SANS_BOLD_32] = create_from_ttf(io, cerebri_sans_medium_ttf, sizeof(cerebri_sans_medium_ttf), 32.0f, ImGuiFreeTypeBuilderFlags_Bold);
	fonts[FONT_WEAPONS_16] = create_from_ttf(io,csgo_icons_ttf, sizeof(csgo_icons_ttf), 16.0f, 0, { ICON_MIN_WEAPON, ICON_MAX_WEAPON });
	fonts[FONT_WEAPONS_32] = create_from_ttf(io,csgo_icons_ttf, sizeof(csgo_icons_ttf), 32.0f, 0, { ICON_MIN_WEAPON, ICON_MAX_WEAPON });
	fonts[FONT_FA_BRANDS_32] = create_from_ttf(io, fa_brands_400_ttf, sizeof(fa_brands_400_ttf), 32.0f, 0, { ICON_MIN_FAB, ICON_MAX_FAB });
	fonts[FONT_FA_REGULAR_32] = create_from_ttf(io, fa_regular_400_ttf, sizeof(fa_regular_400_ttf), 32.0f, 0, { ICON_MIN_FA, ICON_MAX_FA });
	fonts[FONT_FA_SOLID_32] = create_from_ttf(io, fa_solid_900_ttf, sizeof(fa_solid_900_ttf), 32.0f, 0, { ICON_MIN_FA, ICON_MAX_FA });

	textures[TEXTURE_MW_LOGO_BASE] = create_from_png(mw_logo_base_png, sizeof(mw_logo_base_png));
	textures[TEXTURE_MW_LOGO_DOLLAR] = create_from_png(mw_logo_dollar_png, sizeof(mw_logo_dollar_png));
	textures[TEXTURE_TRANSPARENCY] = create_from_png(transparency_checkerboard_png, sizeof(transparency_checkerboard_png));

	ImGuiFreeType::BuildFontAtlas(io.Fonts);

	// Initialize the binding
	//
	// todo: Handle possible errors
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	initialized = true;
}

void render::undo()
{
	if (!initialized)
		return;

	ImGui_ImplDX9_Shutdown();

	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	initialized = false;
}

float render::get_frame_time()
{
	return ImGui::GetIO().DeltaTime;
}

point_t render::get_screen_size()
{
	const auto display_size = ImGui::GetIO().DisplaySize;

	return { display_size.x, display_size.y };
}

void render::begin()
{
	auto &io = ImGui::GetIO();

	const auto active_window = GetForegroundWindow();
	const auto old_mouse_pos = io.MousePos;

	ImGui_ImplWin32_NewFrame();

	if (active_window != target_window_handle)
		io.MousePos = old_mouse_pos;

	state_block->Capture();

	ImGui_ImplDX9_NewFrame();

	ImGui::NewFrame();

	ImGui::GetOverlayDrawList()->ChannelsSplit(8);
	ImGui::GetOverlayDrawList()->ChannelsSetCurrent(0);

	draw_list = ImGui::GetOverlayDrawList();
}

void render::finish()
{
	ImGui::GetOverlayDrawList()->ChannelsMerge();

	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	state_block->Apply();
}

void render::set_layer(int index)
{
	ImGui::GetOverlayDrawList()->ChannelsSetCurrent(index);
}

void render::push_clip(const point_t &position, const point_t &size, bool intersect)
{
	draw_list->PushClipRect({ position.x, position.y }, { position.x + size.x, position.y + size.y }, intersect);
}

void render::reset_clip()
{
	draw_list->PushClipRectFullScreen();
}

void render::pop_clip()
{
	draw_list->PopClipRect();
}

void render::draw_line(const point_t &start, const point_t &end, const color_t &color)
{
	draw_list->AddLine({ start.x, start.y }, { end.x, end.y }, IM_COL32(color.r, color.g, color.b, color.a));
}

void render::draw_rect(const point_t &position, const point_t &size, const color_t &color, float rounding, int corners)
{
	draw_list->AddRect({ position.x, position.y }, { position.x + size.x, position.y + size.y }, IM_COL32(color.r, color.g, color.b, color.a), rounding, corners);
}

void render::fill_rect(const point_t &position, const point_t &size, const color_t &color, float rounding, int corners)
{
	draw_list->AddRectFilled({ position.x, position.y }, { position.x + size.x, position.y + size.y }, IM_COL32(color.r, color.g, color.b, color.a), rounding, corners);
}

void render::gradient_h(const point_t &position, const point_t &size, const color_t &color_start, const color_t &color_end)
{
	ImGui::GetOverlayDrawList()->AddRectFilledMultiColor({ position.x, position.y }, { position.x + size.x, position.y + size.y },
		IM_COL32(color_start.r, color_start.g, color_start.b, color_start.a), IM_COL32(color_end.r, color_end.g, color_end.b, color_end.a),
		IM_COL32(color_end.r, color_end.g, color_end.b, color_end.a), IM_COL32(color_start.r, color_start.g, color_start.b, color_start.a));
}

void render::gradient_v(const point_t &position, const point_t &size, const color_t &color_start, const color_t &color_end)
{
	ImGui::GetOverlayDrawList()->AddRectFilledMultiColor({ position.x, position.y }, { position.x + size.x, position.y + size.y },
		IM_COL32(color_start.r, color_start.g, color_start.b, color_start.a), IM_COL32(color_start.r, color_start.g, color_start.b, color_start.a),
		IM_COL32(color_end.r, color_end.g, color_end.b, color_end.a), IM_COL32(color_end.r, color_end.g, color_end.b, color_end.a));
}

void render::draw_triangle(const point_t &point1, const point_t &point2, const point_t &point3, const color_t &color)
{
	draw_list->AddTriangle({ point1.x, point1.y }, { point2.x, point2.y }, { point3.x, point3.y }, IM_COL32(color.r, color.g, color.b, color.a));
}

void render::fill_triangle(const point_t &point1, const point_t &point2, const point_t &point3, const color_t &color)
{
	draw_list->AddTriangleFilled({ point1.x, point1.y }, { point2.x, point2.y }, { point3.x, point3.y }, IM_COL32(color.r, color.g, color.b, color.a));
}

void render::draw_circle(const point_t &center, float radius, const color_t &color, int segments)
{
	draw_list->AddCircle({ center.x, center.y }, radius, IM_COL32(color.r, color.g, color.b, color.a), segments);
}

void render::fill_circle(const point_t &center, float radius, const color_t &color, int segments)
{
	draw_list->AddCircleFilled({ center.x, center.y }, radius, IM_COL32(color.r, color.g, color.b, color.a), segments);
}

void render::draw_text(const point_t &position, const color_t &color, const char *text, int font, float wrap_width, float font_size)
{
	if (font_size < 0.0f)
		font_size = fonts[font]->FontSize;

	draw_list->AddText(fonts[font], font_size, { position.x, position.y }, IM_COL32(color.r, color.g, color.b, color.a), text, nullptr, wrap_width);
}

void render::draw_image(const point_t &position, const point_t &size, const color_t &color, int texture, float rounding, int corners)
{
	draw_list->AddImageRounded(textures[texture], { position.x, position.y }, { position.x + size.x, position.y + size.y },
		{ 0.0f, 0.0f }, { 1.0f, 1.0f }, IM_COL32(color.r, color.g, color.b, color.a), rounding, corners);
}

point_t render::measure_text(const char *text, int font, float wrap_width, float font_size)
{
	if (font_size < 0.0f)
		font_size = fonts[font]->FontSize;

	const auto size = fonts[font]->CalcTextSizeA(font_size, FLT_MAX, wrap_width, text);

	return { size.x, size.y };
}
