#include "text_input.h"
#include "../../engine/render/render.h"

struct stb_textedit_string_wrapper {
    std::string *str;
};

#define STB_TEXTEDIT_IMPLEMENTATION
#define STB_TEXTEDIT_STRING stb_textedit_string_wrapper

static int STB_TEXTEDIT_STRINGLEN(STB_TEXTEDIT_STRING *obj) {
    return (int) obj->str->size();
}

static void STB_TEXTEDIT_LAYOUTROW(StbTexteditRow *row, STB_TEXTEDIT_STRING *obj, int line_start_idx) {
    const auto text_size = render::measure_text(obj->str->data(), FONT_CEREBRI_SANS_MEDIUM_18);

    row->x0 = 0.0f;
    row->x1 = row->x0 + text_size.x;
    row->baseline_y_delta = text_size.y;
    row->ymin = 0.0f;
    row->ymax = text_size.y;
    row->num_chars = STB_TEXTEDIT_STRINGLEN(obj);
}

static float STB_TEXTEDIT_GETWIDTH(STB_TEXTEDIT_STRING *obj, int line_start_idx, int char_idx) {
    char text[2];

    text[0] = obj->str->at(line_start_idx + char_idx);
    text[1] = '\x00';

    if (text[0] == '\n')
        return -1.0f;

    return render::measure_text(text, FONT_CEREBRI_SANS_MEDIUM_18).x;
}

static int STB_TEXTEDIT_KEYTOTEXT(int key) {
    return key >= 0x200000 ? 0 : key;
}

static char STB_TEXTEDIT_GETCHAR(const STB_TEXTEDIT_STRING *obj, int idx) {
    return obj->str->at(idx);
}

static void STB_TEXTEDIT_DELETECHARS(STB_TEXTEDIT_STRING *obj, int pos, int n) {
    obj->str->erase(pos, n);
}

static bool STB_TEXTEDIT_INSERTCHARS(STB_TEXTEDIT_STRING *obj, int pos, const char *new_text, int new_text_len) {
    obj->str->insert(pos, new_text, new_text_len);

    return true;
}

#define STB_TEXTEDIT_NEWLINE '\n'
#define STB_TEXTEDIT_K_SHIFT 0x400000
#define STB_TEXTEDIT_K_LEFT 0x200000
#define STB_TEXTEDIT_K_RIGHT 0x200001
#define STB_TEXTEDIT_K_UP 0x200002
#define STB_TEXTEDIT_K_DOWN 0x200003
#define STB_TEXTEDIT_K_LINESTART 0x200004
#define STB_TEXTEDIT_K_LINEEND 0x200005
#define STB_TEXTEDIT_K_TEXTSTART 0x200006
#define STB_TEXTEDIT_K_TEXTEND 0x200007
#define STB_TEXTEDIT_K_DELETE 0x200008
#define STB_TEXTEDIT_K_BACKSPACE VK_BACK
#define STB_TEXTEDIT_K_UNDO 0x200009
#define STB_TEXTEDIT_K_REDO 0x25

#include <imgui.h>
#include <stb_textedit.h>

#include "../../engine/input/input.h"
#include "../../ui/ui.h"
#include "text_input.h"

c_text_input::c_text_input(std::string_view title, std::string &value, bool password) : value_(value) {
    password_ = password;
    filter_fn_ = nullptr;
    title_ = title;
    title_wrap_width_ = 0.0f;
    value_wrap_width_ = 0.0f;
    hover_animation_ = 0.0f;
    text_scroll_ = 0.0f;
    cursor_blink_ = 0.0f;
}

c_text_input::c_text_input(std::string_view title, std::string &value, std::function<bool(char)> filter_fn, bool password) : value_(value) {
    password_ = password;
    filter_fn_ = filter_fn;
    title_ = title;
    title_wrap_width_ = 0.0f;
    value_wrap_width_ = 0.0f;
    hover_animation_ = 0.0f;
    text_scroll_ = 0.0f;
    cursor_blink_ = 0.0f;
}

void c_text_input::layout(layout_item &overlay, layout_item &parent) {
    c_element::layout(overlay, parent);

    const auto title_size = render::measure_text(title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);
    const auto value_size = render::measure_text(value_.get().data(), FONT_CEREBRI_SANS_MEDIUM_18, value_wrap_width_);

    root_ = get_row_item().new_item(LAY_HFILL, LAY_COLUMN | LAY_START);

    title_item_ = root_.new_item(LAY_LEFT).size(title_size.x, title_size.y);

    container_ = root_.new_item(LAY_HFILL, LAY_ROW | LAY_START).margins(0.0f, 8.0f, 0.0f, 0.0f);

    value_item_ = container_.new_item(LAY_LEFT | LAY_VCENTER).margins(8.0f, 8.0f, 8.0f, 8.0f).size(value_size.x, value_size.y);

    c_element::layout_inline(overlay);
}

void c_text_input::render() {
    const auto [root_pos, root_size] = rect_to_xywh(container_.get_rect());
    const auto [box_pos, box_size] = rect_to_xywh(container_.get_rect());
    const auto [title_pos, _1] = rect_to_xywh(title_item_.get_rect());
    const auto [value_pos, value_size] = rect_to_xywh(value_item_.get_rect());

    const auto shared_this = shared_from_this();
    const auto active = ui::get_blocking() == shared_this;
    const auto hovered =
        (ui::get_blocking() == nullptr || ui::get_blocking() == shared_this) && input::is_in_bounds(box_pos, box_pos + box_size);

    hover_animation_ = handle_animation(hover_animation_, active || hovered ? 1.0f : 0.0f);

    const auto color = color_t(std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255),
                               std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255),
                               std::clamp((int) (19.0f + (6.0f * hover_animation_)), 0, 255));

    if (hovered)
        input::set_cursor(CURSOR_TEXT_INPUT);

    if (ui::get_blocking() == nullptr && input::is_mouse_clicked(MOUSE_LEFT) && hovered) {
        ui::set_blocking(shared_this);

        memset(&text_edit_state_, 0, sizeof(STB_TexteditState));

        text_scroll_ = 0.0f;
        cursor_blink_ = 0.0f;

        stb_textedit_string_wrapper str{&value_.get()};

        stb_textedit_initialize_state(&text_edit_state_, true);
        
        const auto mouse_offset = input::get_mouse_pos() - value_pos;

        stb_textedit_click(&str, &text_edit_state_, mouse_offset.x, mouse_offset.y);
    } else if (active) {
        stb_textedit_string_wrapper str{&value_.get()};

        if (input::is_mouse_clicked(MOUSE_LEFT)) {
            if (input::is_in_bounds(box_pos, box_pos + box_size)) {
                const auto mouse_offset = input::get_mouse_pos() - value_pos;

                stb_textedit_click(&str, &text_edit_state_, mouse_offset.x, mouse_offset.y);

                cursor_blink_ = -0.3f;
            } else {
                ui::set_blocking(nullptr);
            }
        }

        auto &io = ImGui::GetIO();

        if (io.InputQueueCharacters.Size > 0) {
            for (auto i = 0; i < io.InputQueueCharacters.Size; i++) {
                stb_textedit_key(&str, &text_edit_state_, io.InputQueueCharacters[i]);

                cursor_blink_ = -0.3f;
            }

            io.InputQueueCharacters.resize(0);
        }
    }

    render::fill_rect(box_pos, box_size, color, 4.0f);
    render::draw_rect(box_pos - 1.0f, box_size + 2.0f, {51, 51, 51}, 4.0f);

    render::draw_text(title_pos, {255, 255, 255}, title_.data(), FONT_CEREBRI_SANS_MEDIUM_18, title_wrap_width_);
    render::draw_text(value_pos, {255, 255, 255}, value_.get().data(), FONT_CEREBRI_SANS_MEDIUM_18, value_wrap_width_);

    cursor_blink_ += render::get_frame_time();

    if (active && (cursor_blink_ <= 0.0f || std::fmod(cursor_blink_, 1.2f) <= 0.6f)) {
        auto cursor_pos_chars = text_edit_state_.cursor;
        auto string_n_chars =
            render::measure_text(value_.get().substr(0, cursor_pos_chars).data(), FONT_CEREBRI_SANS_MEDIUM_18, value_wrap_width_);

        render::draw_line(value_pos + point_t(string_n_chars.x, 2.0f), value_pos + point_t(string_n_chars.x, value_size.y - 1.0f),
                          {255, 255, 255});
    }

    title_wrap_width_ = root_size.x;
    value_wrap_width_ = box_size.x - 16.0f;

    c_element::render();
}
