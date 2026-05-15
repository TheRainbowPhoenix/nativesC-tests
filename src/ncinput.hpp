#ifndef NCINPUT_HPP
#define NCINPUT_HPP

#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <cstdint>

// =============================================================================
// CONSTANTS & CONFIG
// =============================================================================

constexpr int SCREEN_W = 320;
constexpr int SCREEN_H = 528;

// Layout Dimensions
constexpr int KBD_H = 260;
constexpr int TAB_H = 30;
constexpr int PICK_HEADER_H = 40;
constexpr int PICK_FOOTER_H = 45;
constexpr int PICK_ITEM_H = 50;

// =============================================================================
// THEMES
// =============================================================================

struct Theme {
    color_t modal_bg;
    color_t kbd_bg;
    color_t key_bg;
    color_t key_spec;
    color_t key_out;
    color_t txt;
    color_t txt_dim;
    color_t accent;
    color_t txt_acc;
    color_t hl;
    color_t check;
};

inline color_t safe_rgb(int r, int g, int b) {
    return C_RGB(r, g, b);
}

class ThemeManager {
public:
    static const Theme& getTheme(const std::string& name);
    static const Theme& getTheme(const Theme* themePtr);
    
private:
    static inline const Theme lightTheme = {
        .modal_bg = C_WHITE,
        .kbd_bg = C_WHITE,
        .key_bg = C_WHITE,
        .key_spec = safe_rgb(28, 29, 28),
        .key_out = C_DARK,
        .txt = safe_rgb(4, 4, 4),
        .txt_dim = safe_rgb(8, 8, 8),
        .accent = safe_rgb(1, 11, 26),
        .txt_acc = C_WHITE,
        .hl = safe_rgb(28, 29, 28),
        .check = C_WHITE
    };
    
    static inline const Theme darkTheme = {
        .modal_bg = safe_rgb(7, 7, 8),
        .kbd_bg = safe_rgb(7, 7, 8),
        .key_bg = safe_rgb(7, 7, 8),
        .key_spec = safe_rgb(11, 11, 12),
        .key_out = safe_rgb(12, 19, 31),
        .txt = C_WHITE,
        .txt_dim = safe_rgb(8, 8, 8),
        .accent = safe_rgb(12, 19, 31),
        .txt_acc = C_WHITE,
        .hl = safe_rgb(11, 11, 12),
        .check = C_WHITE
    };
    
    static inline const Theme greyTheme = {
        .modal_bg = C_LIGHT,
        .kbd_bg = C_LIGHT,
        .key_bg = C_WHITE,
        .key_spec = 0xCE59,
        .key_out = C_BLACK,
        .txt = C_BLACK,
        .txt_dim = safe_rgb(8, 8, 8),
        .accent = C_BLACK,
        .txt_acc = C_WHITE,
        .hl = 0xCE59,
        .check = C_WHITE
    };
};

// =============================================================================
// LIST VIEW ITEM
// =============================================================================

enum class ItemType { Item, Section };

struct ListItem {
    std::string text;
    ItemType type = ItemType::Item;
    int height = 0;
    bool checked = false;
    bool arrow = false;
    int idx = -1;  // For multi-select tracking
    
    // Layout computed values
    int _h = 0;
    int _y = 0;
};

// =============================================================================
// LIST VIEW WIDGET
// =============================================================================

class ListView {
public:
    using ClickCallback = std::function<void(int, const ListItem&)>;
    using LongPressCallback = std::function<void(int, const ListItem&)>;
    
    ListView(int x, int y, int w, int h, 
             const std::vector<ListItem>& items,
             int rowH = 40,
             const std::string& themeName = "light",
             int headersH = -1);
    
    void recalcLayout();
    void selectNext(int startIdx, int step);
    void ensureVisible();
    void clampScroll();
    
    // Returns: std::optional<Action> where Action could be click/long/etc
    struct ActionResult {
        enum Type { None, Click, LongPress };
        Type type = Type::None;
        int index = -1;
        const ListItem* item = nullptr;
    };
    
    ActionResult update(const keyevent_t& ev);
    int getIndexAt(int y);
    
    void draw();
    void drawItem(int x, int y, const ListItem& item, bool isSelected);
    void drawCheck(int x, int y, const Theme& theme);
    
    // Getters/Setters
    int getSelectedIndex() const { return selected_index; }
    void setSelectedIndex(int idx) { selected_index = idx; ensureVisible(); }
    const std::vector<ListItem>& getItems() const { return items; }
    std::vector<ListItem>& getItems() { return items; }
    
    void setClickCallback(ClickCallback cb) { onClick = cb; }
    void setLongPressCallback(LongPressCallback cb) { onLongPress = cb; }
    
private:
    int x, y, w, h;
    std::vector<ListItem> items;
    int base_row_h;
    int headers_h;
    Theme theme;
    
    // Layout State
    int total_h = 0;
    int max_scroll = 0;
    
    // Selection & Scroll
    int selected_index = -1;
    int scroll_y = 0;
    
    // Touch State
    bool is_dragging = false;
    int touch_start_y = 0;
    int touch_start_idx = 0;
    uint64_t touch_start_time = 0;
    double touch_acc_y = 0.0;
    int touch_initial_item_idx = -1;
    bool long_press_triggered = false;
    
    // Configuration
    int drag_threshold = 10;
    double long_press_delay = 0.5;  // seconds
    double snap_sensitivity = 1.0;
    
    // Callbacks
    ClickCallback onClick;
    LongPressCallback onLongPress;
};

// =============================================================================
// KEYBOARD WIDGET
// =============================================================================

enum class KeyboardLayout { QWERTY, AZERTY, QWERTZ, ABC };
enum class KeyboardTab { Alpha, Symbol, Math };

class Keyboard {
public:
    Keyboard(const std::string& themeName = "light", 
             KeyboardLayout layout = KeyboardLayout::QWERTY,
             bool enableTabs = true);
    
    void draw();
    std::string update(const keyevent_t& ev);
    
    // Configuration
    bool visible = false;
    bool enable_tabs = true;
    bool shift = false;
    
    int y = SCREEN_H - KBD_H;
    std::string last_key;
    
private:
    Theme theme;
    KeyboardLayout keyboard_layout;
    KeyboardTab current_tab = KeyboardTab::Alpha;
    
    void drawKey(int x, int y, int w, int h, const std::string& label, 
                 bool isSpecial = false, bool isPressed = false, bool isAccent = false);
    void drawTabs();
    void drawGrid();
    std::string updateGrid(int x, int y, int eventType);
    
    struct KeyRect {
        int x, y, w, h;
        std::string label;
        std::string value;
        bool isSpecial;
        bool isAccent;
    };
    
    std::vector<KeyRect> getMathRects();
    std::vector<KeyRect> getNumpadRects(bool floatOpt = true, bool negOpt = true);
    void drawKeysFromRects(const std::vector<KeyRect>& rects);
    std::string updateKeysFromRects(const std::vector<KeyRect>& rects, int x, int y, int eventType);
    
    std::vector<std::string> tabs = {"ABC", "Sym", "Math"};
};

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

// Input dialog - returns user input string
std::string inputDialog(const std::string& prompt, const std::string& themeName = "light");

// List picker - returns selected option(s)
std::optional<int> pickList(const std::vector<std::string>& options, 
                            const std::string& prompt = "Select:",
                            const std::string& themeName = "light",
                            bool multi = false);

// Confirmation dialog
bool confirmDialog(const std::string& message, const std::string& themeName = "light");

// Message toast
void showToast(const std::string& message, int duration = 60);

#endif // NCINPUT_HPP
