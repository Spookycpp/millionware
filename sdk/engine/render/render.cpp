#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <array>
#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <string>
#include <unordered_map>

#define NANOSVG_IMPLEMENTATION
#include "../../thirdparty/nanosvg/nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "../../thirdparty/nanosvg/nanosvgrast.h"

#include "../../engine/logging/logging.h"
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

#include "../../resources/pixelmix.h"
#include "render.h"

static std::array<ImFont *, FONT_MAX> fonts;
static std::array<IDirect3DTexture9 *, TEXTURE_MAX> textures;

static bool initialized = false;
static HWND target_window_handle;
static IDirect3DDevice9 *d3d9_device;
static IDirect3DStateBlock9 *state_block;
static ImDrawList *draw_list;

static bool get_system_font_path(std::string_view font_name, std::string &result) {
    char windows_directory[MAX_PATH];

    if (!GetWindowsDirectoryA(windows_directory, MAX_PATH))
        return false;

    const auto fonts_directory = std::string(windows_directory).append(xs("\\Fonts\\"));

    HKEY registry_key;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, xs("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"), 0, KEY_READ, &registry_key))
        return false;

    auto index = 0u;

    char value_name[MAX_PATH];
    char value_data[MAX_PATH];

    do {
        auto value_name_size = MAX_PATH;
        auto value_data_size = MAX_PATH;
        auto value_type = 0u;

        const auto error = RegEnumValueA(registry_key, index, value_name, (DWORD *) &value_name_size, nullptr, (DWORD *) &value_type,
                                         (BYTE *) value_data, (DWORD *) &value_data_size);

        index++;

        if (error == ERROR_NO_MORE_ITEMS) {
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

static ImFont *create_from_system(ImGuiIO &io, std::string_view font_name, float size, int rasterizer_flags = 0,
                                  std::initializer_list<ImWchar> glyph_ranges = {}) {
    std::string font_path;

    if (!get_system_font_path(font_name, font_path))
        return nullptr;

    ImFontConfig font_config;

    font_config.FontBuilderFlags = rasterizer_flags;

    return io.Fonts->AddFontFromFileTTF(font_path.data(), size, &font_config, glyph_ranges.size() == 0 ? nullptr : glyph_ranges.begin());
}

static ImFont *create_from_ttf(ImGuiIO &io, const uint8_t *ttf_data, int ttf_data_size, float size, int rasterizer_flags = 0,
                               std::initializer_list<ImWchar> glyph_ranges = {}) {
    ImFontConfig font_config;

    font_config.FontData = std::malloc(ttf_data_size);
    font_config.FontDataSize = ttf_data_size;
    font_config.SizePixels = size;
    font_config.GlyphRanges = glyph_ranges.size() == 0 ? nullptr : glyph_ranges.begin();
    font_config.FontBuilderFlags = rasterizer_flags;

    std::memcpy(font_config.FontData, ttf_data, ttf_data_size);

    return io.Fonts->AddFont(&font_config);
}

IDirect3DTexture9 *render::create_from_png(uint8_t *png_data, int png_data_size) {
    int image_width, image_height, channels;

    stbi_set_flip_vertically_on_load(false);
    stbi_set_flip_vertically_on_load_thread(false);

    const auto image_data = stbi_load_from_memory(png_data, png_data_size, &image_width, &image_height, &channels, 4);

    if (image_data == nullptr) {
        logging::error(stbi_failure_reason());
        return nullptr;
    }

    // stbi_write_png("color.png", image_width, image_height, 4, image_data, image_width * 4);

    // stbi__vertical_flip(image_data, image_width, image_height, 4);

    IDirect3DTexture9 *texture;
    if (d3d9_device->CreateTexture(image_width, image_height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL) < 0)
        return nullptr;

    D3DLOCKED_RECT tex_locked_rect;

    if (texture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
        return nullptr;

    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x) {
            const unsigned char *source_pixel = image_data + image_width * 4 * y + x * 4;
            unsigned char *destination_pixel = static_cast<unsigned char *>(tex_locked_rect.pBits) + tex_locked_rect.Pitch * y + x * 4;

            destination_pixel[0] = source_pixel[2];
            destination_pixel[1] = source_pixel[1];
            destination_pixel[2] = source_pixel[0];
            destination_pixel[3] = source_pixel[3];
        }
    }

    texture->UnlockRect(0);

    stbi_image_free(image_data);

    return texture;
}

IDirect3DTexture9 *render::rasterize_vector(char *data, const float scale) {
    NSVGimage *image = nsvgParse(data, "px", 96.0f);
    if (!image) {
        nsvgDelete(image);
    }

    NSVGrasterizer *rasterizer = nsvgCreateRasterizer();
    if (!rasterizer) {
        nsvgDeleteRasterizer(rasterizer);
        nsvgDelete(image);
    }

    const int w = (int) (image->width * scale);
    const int h = (int) (image->height * scale);

    const auto bytes = new unsigned char[w * h * 4];

    nsvgRasterize(rasterizer, image, 0, 0, scale, bytes, w, h, w * 4);

    int len;
    unsigned char *buf = stbi_write_png_to_mem(bytes, w * 4, w, h, 4, &len);
    if (!buf) {
        delete[] bytes;
        return nullptr;
    }

    delete[] bytes;
    return create_from_png(buf, len);
}

void render::init(HWND window, IDirect3DDevice9 *device) {
    if (initialized)
        return;

    // Store the device reference
    target_window_handle = window;
    d3d9_device = device;

    device->CreateStateBlock(D3DSBT_ALL, &state_block);

    // Create fonts and build the font atlas
    //
    // @todo: Load font data from memory
    auto &io = ImGui::GetIO();

    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    fonts[FONT_VERDANA_12] = create_from_system(io, xs("Verdana"), 13.0f);
    fonts[FONT_VERDANA_14] =
        create_from_system(io, xs("Verdana"), 15.0f, ImGuiFreeTypeBuilderFlags_Bold | ImGuiFreeTypeBuilderFlags_MonoHinting);
    fonts[FONT_VERDANA_24] =
        create_from_system(io, xs("Verdana"), 25.0f, ImGuiFreeTypeBuilderFlags_Bold | ImGuiFreeTypeBuilderFlags_MonoHinting);
    fonts[FONT_TAHOMA_11] =
        create_from_system(io, xs("Tahoma"), 12.f, ImGuiFreeTypeBuilderFlags_Monochrome | ImGuiFreeTypeBuilderFlags_MonoHinting);
    fonts[FONT_TAHOMA_12] =
        create_from_system(io, xs("Tahoma"), 13.f, ImGuiFreeTypeBuilderFlags_Monochrome | ImGuiFreeTypeBuilderFlags_MonoHinting);
    fonts[FONT_VERDANA_12_BOLD] =
        create_from_system(io, xs("Verdana Bold"), 12.0f, ImGuiFreeTypeBuilderFlags_Monochrome | ImGuiFreeTypeBuilderFlags_MonoHinting);
    fonts[FONT_SMALL_TEXT] = create_from_ttf(io, pixelmix_ttf, sizeof(pixelmix_ttf), 10.f,
                                             ImGuiFreeTypeBuilderFlags_MonoHinting | ImGuiFreeTypeBuilderFlags_Monochrome);
    // fonts[FONT_SMALL_TEXT] = create_from_system(io, xs("Verdana"), 10.f, ImGuiFreeTypeBuilderFlags_Monochrome);
    fonts[FONT_CEREBRI_SANS_BOLD_13] =
        create_from_ttf(io, cerebri_sans_medium_ttf, sizeof(cerebri_sans_medium_ttf), 13.0f, ImGuiFreeTypeBuilderFlags_Bold);
    fonts[FONT_CEREBRI_SANS_MEDIUM_14] = create_from_ttf(io, cerebri_sans_medium_ttf, sizeof(cerebri_sans_medium_ttf), 14.0f);
    fonts[FONT_CEREBRI_SANS_MEDIUM_18] = create_from_ttf(io, cerebri_sans_medium_ttf, sizeof(cerebri_sans_medium_ttf), 18.0f);
    fonts[FONT_CEREBRI_SANS_BOLD_32] =
        create_from_ttf(io, cerebri_sans_medium_ttf, sizeof(cerebri_sans_medium_ttf), 32.0f, ImGuiFreeTypeBuilderFlags_Bold);
    fonts[FONT_WEAPONS_16] = create_from_ttf(io, csgo_icons_ttf, sizeof(csgo_icons_ttf), 16.0f, 0, {ICON_MIN_WEAPON, ICON_MAX_WEAPON});
    fonts[FONT_WEAPONS_32] = create_from_ttf(io, csgo_icons_ttf, sizeof(csgo_icons_ttf), 32.0f, 0, {ICON_MIN_WEAPON, ICON_MAX_WEAPON});
    fonts[FONT_FA_BRANDS_32] = create_from_ttf(io, fa_brands_400_ttf, sizeof(fa_brands_400_ttf), 32.0f, 0, {ICON_MIN_FAB, ICON_MAX_FAB});
    fonts[FONT_FA_REGULAR_32] = create_from_ttf(io, fa_regular_400_ttf, sizeof(fa_regular_400_ttf), 32.0f, 0, {ICON_MIN_FA, ICON_MAX_FA});
    fonts[FONT_FA_SOLID_32] = create_from_ttf(io, fa_solid_900_ttf, sizeof(fa_solid_900_ttf), 32.0f, 0, {ICON_MIN_FA, ICON_MAX_FA});

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

void render::undo() {
    if (!initialized)
        return;

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();

    initialized = false;
}

HWND render::window() {
    return target_window_handle;
}

float render::get_frame_time() {
    return ImGui::GetIO().DeltaTime;
}

point_t render::get_screen_size() {
    const auto display_size = ImGui::GetIO().DisplaySize;

    return {display_size.x, display_size.y};
}

void render::begin() {
    auto &io = ImGui::GetIO();

    const auto active_window = GetForegroundWindow();
    const auto old_mouse_pos = io.MousePos;

    ImGui_ImplWin32_NewFrame();

    if (active_window != target_window_handle)
        io.MousePos = old_mouse_pos;

    state_block->Capture();

    ImGui_ImplDX9_NewFrame();

    ImGui::NewFrame();

    ImGui::GetForegroundDrawList()->ChannelsSplit(8);
    ImGui::GetForegroundDrawList()->ChannelsSetCurrent(0);

    draw_list = ImGui::GetForegroundDrawList();
}

void render::finish() {
    ImGui::GetForegroundDrawList()->ChannelsMerge();
    ImGui::GetIO().KeyMods = ImGui::GetMergedKeyModFlags();

    ImGui::EndFrame();
    ImGui::Render();

    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    state_block->Apply();
}

void render::set_layer(int index) {
    ImGui::GetForegroundDrawList()->ChannelsSetCurrent(index);
}

void render::push_clip(const point_t &position, const point_t &size, bool intersect) {
    draw_list->PushClipRect({position.x, position.y}, {position.x + size.x, position.y + size.y}, intersect);
}

void render::reset_clip() {
    draw_list->PushClipRectFullScreen();
}

void render::pop_clip() {
    draw_list->PopClipRect();
}

void render::draw_line(const point_t &start, const point_t &end, const color_t &color, float thickness) {
    draw_list->AddLine({start.x, start.y}, {end.x, end.y}, IM_COL32(color.r, color.g, color.b, color.a), thickness);
}

void render::draw_rect(const point_t &position, const point_t &size, const color_t &color, float rounding, int corners) {
    draw_list->AddRect({floor(position.x), floor(position.y)}, {floor(position.x + size.x), floor(position.y + size.y)},
                       IM_COL32(color.r, color.g, color.b, color.a), rounding, corners);
}

void render::fill_rect(const point_t &position, const point_t &size, const color_t &color, float rounding, int corners) {
    draw_list->AddRectFilled({floor(position.x), floor(position.y)}, {floor(position.x + size.x), floor(position.y + size.y)},
                             IM_COL32(color.r, color.g, color.b, color.a), rounding, corners);
}

void render::gradient_h(const point_t &position, const point_t &size, const color_t &color_start, const color_t &color_end) {
    ImGui::GetForegroundDrawList()->AddRectFilledMultiColor(
        {position.x, position.y}, {position.x + size.x, position.y + size.y},
        IM_COL32(color_start.r, color_start.g, color_start.b, color_start.a), IM_COL32(color_end.r, color_end.g, color_end.b, color_end.a),
        IM_COL32(color_end.r, color_end.g, color_end.b, color_end.a), IM_COL32(color_start.r, color_start.g, color_start.b, color_start.a));
}

void render::gradient_v(const point_t &position, const point_t &size, const color_t &color_start, const color_t &color_end) {
    ImGui::GetForegroundDrawList()->AddRectFilledMultiColor({position.x, position.y}, {position.x + size.x, position.y + size.y},
                                                            IM_COL32(color_start.r, color_start.g, color_start.b, color_start.a),
                                                            IM_COL32(color_start.r, color_start.g, color_start.b, color_start.a),
                                                            IM_COL32(color_end.r, color_end.g, color_end.b, color_end.a),
                                                            IM_COL32(color_end.r, color_end.g, color_end.b, color_end.a));
}

void render::draw_triangle(const point_t &point1, const point_t &point2, const point_t &point3, const color_t &color) {
    draw_list->AddTriangle({point1.x, point1.y}, {point2.x, point2.y}, {point3.x, point3.y}, IM_COL32(color.r, color.g, color.b, color.a));
}

void render::fill_triangle(const point_t &point1, const point_t &point2, const point_t &point3, const color_t &color) {
    draw_list->AddTriangleFilled({point1.x, point1.y}, {point2.x, point2.y}, {point3.x, point3.y},
                                 IM_COL32(color.r, color.g, color.b, color.a));
}

void render::draw_circle(const point_t &center, float radius, const color_t &color, int segments) {
    draw_list->AddCircle({center.x, center.y}, radius, IM_COL32(color.r, color.g, color.b, color.a), segments);
}

void render::fill_circle(const point_t &center, float radius, const color_t &color, int segments) {
    draw_list->AddCircleFilled({center.x, center.y}, radius, IM_COL32(color.r, color.g, color.b, color.a), segments);
}

void render::fill_convex_poly(const point_t *points, int num_points, const color_t &color) {
    draw_list->AddConvexPolyFilled((ImVec2 *) points, num_points, IM_COL32(color.r, color.g, color.b, color.a));
}

void render::draw_text(const point_t &position, const color_t &color, const char *text, int font, float wrap_width, float font_size) {
    if (font_size < 0.0f)
        font_size = fonts[font]->FontSize;

    draw_list->AddText(fonts[font], font_size, {position.x, position.y}, IM_COL32(color.r, color.g, color.b, color.a), text, nullptr,
                       wrap_width);
}

void render::draw_text_outlined(const point_t &position, const color_t &color, const color_t &outline_color, const char *text, int font,
                                float wrap_width, float font_size) {
    if (font_size < 0.0f)
        font_size = fonts[font]->FontSize;

    render::draw_text({position.x, position.y + 1.0f}, outline_color, text, font, wrap_width);
    render::draw_text({position.x, position.y - 1.0f}, outline_color, text, font, wrap_width);
    render::draw_text({position.x + 1.0f, position.y}, outline_color, text, font, wrap_width);
    render::draw_text({position.x - 1.0f, position.y}, outline_color, text, font, wrap_width);

    draw_list->AddText(fonts[font], font_size, {position.x, position.y}, IM_COL32(color.r, color.g, color.b, color.a), text, nullptr,
                       wrap_width);
}

void render::draw_image(const point_t &position, const point_t &size, const color_t &color, int texture, float rounding, int corners) {
    draw_list->AddImageRounded(textures[texture], {floor(position.x), floor(position.y)},
                               {floor(position.x + size.x), floor(position.y + size.y)}, {0.0f, 0.0f}, {1.0f, 1.0f},
                               IM_COL32(color.r, color.g, color.b, color.a), rounding, corners);
}

void render::draw_image(const point_t &position, const point_t &size, const color_t &color, IDirect3DTexture9 *texture, float rounding,
                        int corners) {
    draw_list->AddImageRounded(texture, {position.x, position.y}, {position.x + size.x, position.y + size.y}, {0.0f, 0.0f}, {1.0f, 1.0f},
                               IM_COL32(color.r, color.g, color.b, color.a), rounding, corners);
}

void render::draw_poly_line(const point_t *points, const int num_points, const color_t &color, const float thickness) {
    draw_list->AddPolyline((ImVec2 *) points, num_points, IM_COL32(color.r, color.g, color.b, color.a), 0, thickness);
}

void render::draw_bezier_cubic(const point_t &point1, const point_t &point2, const point_t &point3, const point_t &point4,
                               const color_t &color, const float thickness) {
    draw_list->AddBezierCubic({point1.x, point1.y}, {point2.x, point2.y}, {point3.x, point3.y}, {point4.x, point4.y},
                              IM_COL32(color.r, color.g, color.b, color.a), thickness);
}

void render::draw_bezier_quad(const point_t &point1, const point_t &point2, const point_t &point3, const color_t &color,
                              const float thickness) {
    draw_list->AddBezierQuadratic({point1.x, point1.y}, {point2.x, point2.y}, {point3.x, point3.y},
                                  IM_COL32(color.r, color.g, color.b, color.a), thickness);
}

point_t render::measure_text(const char *text, int font, float wrap_width, float font_size) {
    if (font_size < 0.0f)
        font_size = fonts[font]->FontSize;

    const auto size = fonts[font]->CalcTextSizeA(font_size, FLT_MAX, wrap_width, text);

    return {size.x, size.y};
}

render::animated_gif::animated_gif(const unsigned char *bytes, int size) {
    load_from_memory(bytes, size);
}

bool render::animated_gif::load_from_memory(const unsigned char *bytes, int size) {
    clear();

    int *int_delays = nullptr;
    int comp;

    stbi_set_flip_vertically_on_load(false);
    stbi_set_flip_vertically_on_load_thread(false);

    m_buffer = stbi_load_gif_from_memory(bytes, size, &int_delays, &width, &height, &m_frame_count, &comp, 4);
    if (!m_buffer || m_frame_count <= 0 || !int_delays) {
        clear();
        return false;
    }

    m_delays.resize(m_frame_count);
    for (int i = 0; i < m_frame_count; i++) {
        m_delays.at(i) = static_cast<float>(int_delays[i]) * 0.1f;
    }

    stbi_image_free(int_delays);

    return true;
}

void render::animated_gif::create_texture(const unsigned char *buffer) {
    if (d3d9_device->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_texture, nullptr) < 0)
        return;

    D3DLOCKED_RECT tex_locked_rect;

    if (m_texture->LockRect(0, &tex_locked_rect, nullptr, 0) != D3D_OK)
        return;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const unsigned char *source_pixel = buffer + width * 4 * y + x * 4;
            unsigned char *destination_pixel = static_cast<unsigned char *>(tex_locked_rect.pBits) + tex_locked_rect.Pitch * y + x * 4;

            destination_pixel[0] = source_pixel[2];
            destination_pixel[1] = source_pixel[1];
            destination_pixel[2] = source_pixel[0];
            destination_pixel[3] = source_pixel[3];
        }
    }

    m_texture->UnlockRect(0);
}

void render::animated_gif::draw(const point_t &position, const point_t &size, const color_t &color, float rounding, int corners) {
    update();
    draw_list->AddImageRounded(m_texture, {position.x, position.y}, {position.x + size.x, position.y + size.y}, {0.0f, 0.0f}, {1.0f, 1.0f},
                               IM_COL32(color.r, color.g, color.b, color.a), rounding, corners);
}

void render::animated_gif::update() {
    if (!m_buffer) {
        return;
    }

    if (m_frame_count <= 0) {
        return;
    }

    m_last_frame_update_num = ImGui::GetFrameCount();

    float last_delay = m_delay;
    if (m_timer > 0) {
        m_delay = ImGui::GetTime() * 100.0f - m_timer;
        if (m_delay < 0)
            m_timer = -1.0f;
    }
    if (m_timer < 0) {
        m_timer = ImGui::GetTime() * 100.0f;
        m_delay = 0.0f;
    }

    const int image_size = 4 * width * height;
    bool force_update = false;
    if (m_last_frame_num < 0) {
        force_update = true;
        m_last_frame_num = 0;
    }

    for (int i = m_last_frame_num; i < m_frame_count; i++) {
        const float frame_time = m_delays.at(i);
        if (m_delay <= last_delay + frame_time) {
            const bool changed_frame = i != m_last_frame_num;

            m_last_frame_num = i;
            if (changed_frame || force_update) {
                create_texture(&m_buffer[image_size * i]);
            }

            m_delay = last_delay;
            return;
        }
        last_delay += frame_time;
        if (i == m_frame_count - 1)
            i = -1;
    }
}

void render::animated_gif::clear() {
    if (m_buffer) {
        stbi_image_free(m_buffer);
        m_buffer = nullptr;
    }

    m_frame_count = 0;
    m_last_frame_num = 0;

    m_delay = 0.0f;
    m_timer = -1.0f;

    m_delays.clear();

    m_last_frame_update_num = -1;
}