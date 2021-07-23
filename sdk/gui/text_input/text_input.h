#pragma once

#define STB_TEXTEDIT_CHARTYPE char
#define STB_TEXTEDIT_POSITIONTYPE size_t
#define STB_TEXTEDIT_UNDOSTATECOUNT 100
#define STB_TEXTEDIT_UNDOCHARCOUNT 10000

#include <functional>
#include <memory>
#include <stb_textedit.h>
#include <string>

#include "../base_element.h"

class c_text_input : public c_element {
    bool password_;

    std::reference_wrapper<std::string> value_;
    std::function<bool(char)> filter_fn_;

    std::string title_;

    layout_item title_item_;
    layout_item container_;
    layout_item value_item_;

    float title_wrap_width_;
    float value_wrap_width_;
    float hover_animation_;

    STB_TexteditState text_edit_state_;

    float text_scroll_;
    float cursor_blink_;

  public:
    c_text_input(std::string_view title, std::string &value, bool password = false);
    c_text_input(std::string_view title, std::string &value, std::function<bool(char)> filter_fn, bool password = false);

    void layout(layout_item &overlay, layout_item &parent) override;
    void render() override;
};
