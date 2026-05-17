#pragma once

#include "nrender.hpp"
#include <stdint.h>

namespace nui {

enum ElementType { TYPE_LABEL, TYPE_BUTTON, TYPE_TEXTBOX };

class NElement {
public:
    virtual ~NElement() {}
    virtual void render() = 0;
    virtual void handle_touch(int tx, int ty, int action) = 0;
    virtual ElementType get_type() const = 0;
};

class NLabel : public NElement {
public:
    NLabel(int x, int y, const char* text);
    void render() override;
    void handle_touch(int tx, int ty, int action) override { (void)tx; (void)ty; (void)action; }
    ElementType get_type() const override { return TYPE_LABEL; }
private:
    int m_x, m_y;
    const char* m_text;
};

class NButton : public NElement {
public:
    NButton(int x1, int y1, int x2, int y2, const char* text, int id);
    void render() override;
    void handle_touch(int tx, int ty, int action) override;
    ElementType get_type() const override { return TYPE_BUTTON; }
    bool is_clicked() const { return m_clicked; }
    void reset_clicked() { m_clicked = false; }
private:
    int m_x1, m_y1, m_x2, m_y2;
    const char* m_text;
    int m_id;
    bool m_pressed;
    bool m_clicked;
};

class NTextBox : public NElement {
public:
    NTextBox(int x, int y, int w, int maxLen);
    void render() override;
    void handle_touch(int tx, int ty, int action) override;
    ElementType get_type() const override { return TYPE_TEXTBOX; }
    const char* GetText() const { return m_buffer; }
    void SetText(const char* text);
    void AppendChar(char c);
    void Backspace();
    bool is_focused() const { return m_focused; }
    void set_focused(bool f) { m_focused = f; }
private:
    int m_x, m_y, m_w;
    char m_buffer[256];
    int m_maxLen;
    bool m_focused;
};

class NDialog {
public:
    enum Height { Height25, Height35, Height55, Height75, Height95 };
    enum Result { DialogResultOK, DialogResultCancel };

    NDialog(Height h, const char* title);
    ~NDialog();

    void AddElement(NElement& el);
    Result ShowDialog();

    int GetLeftX() const { return m_x1; }
    int GetTopY() const { return m_y1; }

private:
    int m_x1, m_y1, m_x2, m_y2;
    const char* m_title;
    NElement* m_elements[10];
    int m_count;
};

} // namespace nui
