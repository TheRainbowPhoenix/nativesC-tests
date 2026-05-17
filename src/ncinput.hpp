#pragma once

#include <stdint.h>
#include <stddef.h>
#include <os/input.h>

namespace ncinput {

struct Theme {
    uint16_t modal_bg;
    uint16_t kbd_bg;
    uint16_t key_bg;
    uint16_t key_spec;
    uint16_t key_out;
    uint16_t txt;
    uint16_t txt_dim;
    uint16_t accent;
    uint16_t txt_acc;
    uint16_t hl;
    uint16_t check;
};

extern const Theme light_theme;
extern const Theme dark_theme;
extern const Theme grey_theme;

const Theme& get_theme(const char* name);

enum class InputType {
    AlphaNumeric,
    NumericInt,
    NumericFloat,
    NumericNegative,
    Math
};

class Keyboard {
public:
    Keyboard(const Theme& theme, const char* layout = "qwerty");

    void draw();
    const char* handle_event(const struct Input_Event& ev);

    void set_visible(bool visible) { m_visible = visible; }
    bool is_visible() const { return m_visible; }

    int get_y() const { return m_y; }

private:
    const Theme& m_theme;
    bool m_visible;
    int m_y;
    int m_current_tab;
    bool m_shift;
    const char* m_last_key;

    void draw_key(int x, int y, int w, int h, const char* label, bool is_spec = false, bool is_pressed = false, bool is_accent = false);
};

// Public API
char* input(const char* prompt, InputType type = InputType::AlphaNumeric, const char* theme = "light", const char* layout = "qwerty");
int pick(const char** options, size_t count, const char* prompt = "Select:", const char* theme = "light");
bool ask(const char* title, const char* body, const char* ok_text = "OK", const char* cancel_text = "Cancel", const char* theme = "light");

} // namespace ncinput
