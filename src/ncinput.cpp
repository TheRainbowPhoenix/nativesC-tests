#include "ncinput.hpp"
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>
#include <algorithm>
#include <cmath>

// =============================================================================
// THEME MANAGER IMPLEMENTATION
// =============================================================================

const Theme& ThemeManager::getTheme(const std::string& name) {
    if (name == "dark") return darkTheme;
    if (name == "grey") return greyTheme;
    return lightTheme;
}

const Theme& ThemeManager::getTheme(const Theme* themePtr) {
    if (themePtr) return *themePtr;
    return lightTheme;
}

// =============================================================================
// LIST VIEW IMPLEMENTATION
// =============================================================================

ListView::ListView(int x, int y, int w, int h,
                   const std::vector<ListItem>& items,
                   int rowH,
                   const std::string& themeName,
                   int headersH)
    : x(x), y(y), w(w), h(h), items(items), base_row_h(rowH),
      theme(ThemeManager::getTheme(themeName))
{
    headers_h = (headersH >= 0) ? headersH : rowH;
    
    // Normalize items - set heights if not specified
    for (auto& it : this->items) {
        if (it.height == 0) {
            it.height = (it.type == ItemType::Section) ? headers_h : base_row_h;
        }
    }
    
    recalcLayout();
    
    // Select first selectable item
    selectNext(-1, 1);
}

void ListView::recalcLayout() {
    total_h = 0;
    for (auto& it : items) {
        it._h = it.height;
        it._y = total_h;
        total_h += it._h;
    }
    max_scroll = std::max(0, total_h - h);
}

void ListView::selectNext(int startIdx, int step) {
    int idx = startIdx;
    int count = static_cast<int>(items.size());
    
    if (count == 0) {
        selected_index = -1;
        return;
    }
    
    // Safety loop limit
    for (int i = 0; i < count; ++i) {
        if (idx >= 0 && idx < count) {
            if (items[idx].type != ItemType::Section) {
                selected_index = idx;
                ensureVisible();
                return;
            }
        }
        idx += step;
        if (idx < 0 || idx >= count) break;
    }
}

void ListView::ensureVisible() {
    if (selected_index < 0 || selected_index >= static_cast<int>(items.size())) return;
    
    const auto& it = items[selected_index];
    int item_top = it._y;
    int item_bot = item_top + it._h;
    
    int view_top = scroll_y;
    int view_bot = scroll_y + h;
    
    if (item_top < view_top) {
        scroll_y = item_top;
    } else if (item_bot > view_bot) {
        scroll_y = item_bot - h;
    }
    
    clampScroll();
}

void ListView::clampScroll() {
    max_scroll = std::max(0, total_h - h);
    scroll_y = std::max(0, std::min(max_scroll, scroll_y));
}

int ListView::getIndexAt(int y) {
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i]._y <= y && y < items[i]._y + items[i]._h) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

ListView::ActionResult ListView::update(const keyevent_t& ev) {
    uint64_t now = clock_ms() / 1000.0;  // Convert to seconds approximation
    
    ActionResult result;
    
    // Pre-process touch events
    const keyevent_t* touch_down = nullptr;
    const keyevent_t* touch_up = nullptr;
    const keyevent_t* current_touch = nullptr;
    
    if (ev.type == KEYEV_TOUCH_DOWN) {
        touch_down = &ev;
        current_touch = &ev;
    } else if (ev.type == KEYEV_TOUCH_UP) {
        touch_up = &ev;
    }
    
    // 1. Start Touch
    if (touch_down && !is_dragging && touch_start_time == 0) {
        if (x <= touch_down->x && touch_down->x < x + w &&
            y <= touch_down->y && touch_down->y < y + h) {
            
            touch_start_y = touch_down->y;
            touch_start_time = clock_ms();
            
            // Determine which item was touched initially
            int local_y = touch_down->y - y + scroll_y;
            touch_initial_item_idx = getIndexAt(local_y);
            
            // Anchor drag to the item under finger if valid, else selection
            if (touch_initial_item_idx != -1) {
                touch_start_idx = touch_initial_item_idx;
                // Immediate visual feedback
                if (touch_initial_item_idx >= 0 && 
                    touch_initial_item_idx < static_cast<int>(items.size())) {
                    if (items[touch_initial_item_idx].type != ItemType::Section) {
                        selected_index = touch_initial_item_idx;
                        ensureVisible();
                    }
                }
            } else {
                touch_start_idx = selected_index;
            }
            
            is_dragging = false;
            long_press_triggered = false;
            touch_acc_y = 0.0;
        }
    }
    
    // Long Press Detection (Time-based)
    if (touch_start_time != 0 && !is_dragging && !long_press_triggered) {
        if ((clock_ms() - touch_start_time) > 800) {  // 800ms threshold
            long_press_triggered = true;
            int idx = selected_index;
            if (idx >= 0 && idx < static_cast<int>(items.size())) {
                result.type = ActionResult::Type::LongPress;
                result.index = idx;
                result.item = &items[idx];
                
                if (onLongPress) onLongPress(idx, items[idx]);
                return result;
            }
        }
    }
    
    // 2. Touch Move / Drag
    if (touch_start_time != 0) {
        const keyevent_t* last_pos = current_touch ? current_touch : touch_down;
        
        if (last_pos) {
            int dy = last_pos->y - touch_start_y;
            
            // Check for drag threshold
            if (!is_dragging && std::abs(dy) > base_row_h) {
                is_dragging = true;
                long_press_triggered = true;
            }
            
            if (is_dragging) {
                // Scroll Logic (Snap)
                if (touch_start_idx >= 0 && touch_start_idx < static_cast<int>(items.size())) {
                    int start_item_y = items[touch_start_idx]._y;
                    int target_y = start_item_y - dy;
                    
                    // Find index at target_y
                    int found_idx = -1;
                    for (size_t i = 0; i < items.size(); ++i) {
                        if (items[i]._y <= target_y && target_y < items[i]._y + items[i]._h) {
                            found_idx = static_cast<int>(i);
                            break;
                        }
                    }
                    
                    if (found_idx == -1) {
                        if (target_y < 0) found_idx = 0;
                        else found_idx = static_cast<int>(items.size()) - 1;
                    }
                    
                    int final_idx = found_idx;
                    
                    // Apply Clamping Logic for sections
                    if (found_idx >= 0 && found_idx < static_cast<int>(items.size()) &&
                        items[found_idx].type == ItemType::Section) {
                        
                        if (selected_index < found_idx) {
                            final_idx = found_idx - 1;
                        } else {
                            final_idx = found_idx + 1;
                        }
                        final_idx = std::max(0, std::min(static_cast<int>(items.size()) - 1, final_idx));
                    }
                    
                    if (final_idx >= 0 && final_idx < static_cast<int>(items.size()) &&
                        items[final_idx].type != ItemType::Section) {
                        selected_index = final_idx;
                        ensureVisible();
                    }
                }
            }
        }
    }
    
    // 3. Touch Release
    if (touch_up) {
        if (touch_start_time != 0) {
            if (!is_dragging && !long_press_triggered) {
                // Click Candidate
                int local_y = touch_up->y - y + scroll_y;
                int release_idx = getIndexAt(local_y);
                
                // Logic: If release is on same item as start, it's a click.
                if (release_idx == touch_initial_item_idx && release_idx >= 0) {
                    if (items[release_idx].type != ItemType::Section) {
                        selected_index = release_idx;
                        ensureVisible();
                        
                        result.type = ActionResult::Type::Click;
                        result.index = release_idx;
                        result.item = &items[release_idx];
                        
                        if (onClick) onClick(release_idx, items[release_idx]);
                    }
                }
            }
            
            // Reset
            touch_start_time = 0;
            is_dragging = false;
            return result;
        }
    }
    
    // 4. Key handling
    if (ev.type == KEYEV_DOWN || (ev.type == KEYEV_HOLD && 
        (ev.key == KEY_UP || ev.key == KEY_DOWN))) {
        
        if (ev.key == KEY_UP) {
            selectNext(selected_index - 1, -1);
        } else if (ev.key == KEY_DOWN) {
            selectNext(selected_index + 1, 1);
        } else if (ev.key == KEY_EXE) {
            if (selected_index >= 0) {
                result.type = ActionResult::Type::Click;
                result.index = selected_index;
                result.item = &items[selected_index];
                
                if (onClick) onClick(selected_index, items[selected_index]);
            }
        }
    }
    
    return result;
}

void ListView::drawCheck(int x, int y, const Theme& theme) {
    drect(x, y, x + 20, y + 20, theme.accent);
    color_t c = theme.check;
    dline(x + 4, y + 10, x + 8, y + 14, c);
    dline(x + 8, y + 14, x + 15, y + 5, c);
}

void ListView::drawItem(int x, int y, const ListItem& item, bool isSelected) {
    const Theme& t = theme;
    int h = item._h;
    
    if (item.type == ItemType::Section) {
        drect(x, y, x + w, y + h, t.key_spec);
        drect_border(x, y, x + w, y + h, C_NONE, 1, t.key_spec);
        dtext_opt(x + 10, y + h/2, t.txt_dim, C_NONE, DTEXT_LEFT, DTEXT_MIDDLE, 
                  item.text.c_str(), -1);
    } else {
        color_t bg = isSelected ? t.hl : t.modal_bg;
        drect(x, y, x + w, y + h, bg);
        drect_border(x, y, x + w, y + h, C_NONE, 1, t.key_spec);
        
        int x_off = 20;
        if (item.checked) {
            drawCheck(x + 10, y + (h - 20)/2, t);
            x_off = 40;
        }
        
        dtext_opt(x + x_off, y + h/2, t.txt, C_NONE, DTEXT_LEFT, DTEXT_MIDDLE, 
                  item.text.c_str(), -1);
        
        // Draw Arrow if requested
        if (item.arrow) {
            int ar_x = x + w - 15;
            int ar_y = y + h/2;
            color_t c = t.txt_dim;
            dline(ar_x - 4, ar_y - 4, ar_x, ar_y, c);
            dline(ar_x - 4, ar_y + 4, ar_x, ar_y, c);
        }
    }
}

void ListView::draw() {
    const Theme& t = theme;
    drect(x, y, x + w, y + h, t.modal_bg);
    
    // Lazy Rendering: Find start index
    int start_y = scroll_y;
    int end_y = scroll_y + h;
    
    // Simple scan to find start
    int start_idx = 0;
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i]._y + items[i]._h > start_y) {
            start_idx = static_cast<int>(i);
            break;
        }
    }
    
    // Draw visible items
    for (int i = start_idx; i < static_cast<int>(items.size()); ++i) {
        const auto& it = items[i];
        if (it._y >= end_y) break;
        
        int item_y = y + it._y - scroll_y;
        drawItem(x, item_y, it, (i == selected_index));
    }
    
    // Scrollbar
    if (max_scroll > 0) {
        int sb_w = 4;
        int ratio_num = h;
        int ratio_den = (total_h > 0) ? total_h : 1;
        int thumb_h = std::max(20, (ratio_num * h) / ratio_den);
        
        int scroll_ratio_num = scroll_y;
        int scroll_ratio_den = max_scroll;
        int thumb_y = y + (scroll_ratio_num * (h - thumb_h)) / scroll_ratio_den;
        
        int sb_x = x + w - sb_w - 2;
        drect(sb_x, thumb_y, sb_x + sb_w, thumb_y + thumb_h, t.accent);
    }
}

// =============================================================================
// KEYBOARD IMPLEMENTATION
// =============================================================================

static const std::vector<std::string> LAYOUT_QWERTY[] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
    {"a", "s", "d", "f", "g", "h", "j", "k", "l", ":"},
    {"z", "x", "c", "v", "b", "n", "m", ",", ".", "_"}
};

static const std::vector<std::string> LAYOUT_AZERTY[] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"a", "z", "e", "r", "t", "y", "u", "i", "o", "p"},
    {"q", "s", "d", "f", "g", "h", "j", "k", "l", "m"},
    {"w", "x", "c", "v", "b", "n", ",", ".", "_", ":"}
};

static const std::vector<std::string> LAYOUT_SYM = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"@", "#", "$", "_", "&", "-", "+", "(", "/", ")"},
    {"=", "\\", "*", "<", "\"", "'", ":", ";", "!", "?"},
    {"{", "}", "[", "]", "^", "~", "`", "|", "<", ">"}
};

Keyboard::Keyboard(const std::string& themeName, KeyboardLayout layout, bool enableTabs)
    : theme(ThemeManager::getTheme(themeName)), keyboard_layout(layout), enable_tabs(enableTabs)
{
    y = SCREEN_H - KBD_H;
    if (layout == KeyboardLayout::AZERTY) {
        tabs[0] = "AZERTY";
    } else if (layout == KeyboardLayout::QWERTZ) {
        tabs[0] = "QWERTZ";
    }
}

void Keyboard::drawKey(int x, int y, int w, int h, const std::string& label,
                       bool isSpecial, bool isPressed, bool isAccent) {
    const Theme& t = theme;
    
    color_t bg;
    if (isPressed) bg = t.hl;
    else if (isAccent) bg = t.accent;
    else if (isSpecial) bg = t.key_spec;
    else bg = t.key_bg;
    
    color_t txt_col = isAccent ? t.txt_acc : t.txt;
    color_t border_col = t.key_spec;
    
    drect(x + 1, y + 1, x + w - 1, y + h - 1, bg);
    drect_border(x, y, x + w, y + h, C_NONE, 1, border_col);
    dtext_opt(x + w/2, y + h/2, txt_col, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE, 
              label.c_str(), -1);
}

void Keyboard::drawTabs() {
    const Theme& t = theme;
    int tab_w = SCREEN_W / 3;
    color_t border_col = t.key_spec;
    
    for (int i = 0; i < 3; ++i) {
        int tx = i * tab_w;
        bool is_active = (i == static_cast<int>(current_tab));
        color_t bg = is_active ? t.kbd_bg : t.key_spec;
        
        drect(tx, y, tx + tab_w, y + TAB_H, bg);
        drect_border(tx, y, tx + tab_w, y + TAB_H, C_NONE, 1, border_col);
        
        if (is_active) {
            drect(tx + 1, y + TAB_H - 1, tx + tab_w - 1, y + TAB_H + 1, t.kbd_bg);
        }
        
        dtext_opt(tx + tab_w/2, y + TAB_H/2, t.txt, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE,
                  tabs[i].c_str(), -1);
    }
}

void Keyboard::drawGrid() {
    const std::vector<std::string>* layout;
    if (current_tab == KeyboardTab::Symbol) {
        layout = &LAYOUT_SYM;
    } else {
        switch (keyboard_layout) {
            case KeyboardLayout::AZERTY: layout = &LAYOUT_AZERTY; break;
            default: layout = &LAYOUT_QWERTY; break;
        }
    }
    
    int grid_y = y + TAB_H;
    int row_h = 45;
    
    for (int r = 0; r < 4 && r < static_cast<int>(layout->size()); ++r) {
        const auto& row = (*layout)[r];
        int count = static_cast<int>(row.size());
        int kw = SCREEN_W / count;
        
        for (int c = 0; c < count; ++c) {
            int kx = c * kw;
            int ky = grid_y + r * row_h;
            std::string label = row[c];
            if (current_tab == KeyboardTab::Alpha && shift && label.size() == 1) {
                char ch = label[0];
                if (ch >= 'a' && ch <= 'z') label = std::string(1, static_cast<char>(ch - 32));
            }
            bool is_pressed = (last_key == label);
            drawKey(kx, ky, kw, row_h, label, false, is_pressed, false);
        }
    }
    
    // Bottom Control Row
    int bot_y = grid_y + 4 * row_h;
    int bot_h = row_h;
    drawKey(0, bot_y, 50, bot_h, "CAPS", true, shift, false);
    drawKey(50, bot_y, 50, bot_h, "<-", true, last_key == "BACKSPACE", false);
    drawKey(100, bot_y, 160, bot_h, "Space", false, last_key == " ", false);
    drawKey(260, bot_y, 60, bot_h, "EXE", false, last_key == "ENTER", true);
}

std::string Keyboard::updateGrid(int x, int y, int eventType) {
    int grid_y = y + TAB_H;
    int row_h = 45;
    int row_idx = (y - grid_y) / row_h;
    
    if (row_idx >= 0 && row_idx < 4) {
        const std::vector<std::string>* layout;
        if (current_tab == KeyboardTab::Symbol) {
            layout = &LAYOUT_SYM;
        } else {
            switch (keyboard_layout) {
                case KeyboardLayout::AZERTY: layout = &LAYOUT_AZERTY; break;
                default: layout = &LAYOUT_QWERTY; break;
            }
        }
        
        if (row_idx >= static_cast<int>(layout->size())) return "";
        
        const auto& row = (*layout)[row_idx];
        int kw = SCREEN_W / static_cast<int>(row.size());
        int col_idx = std::min(static_cast<int>(row.size()) - 1, std::max(0, x / kw));
        std::string ch = row[col_idx];
        
        if (current_tab == KeyboardTab::Alpha && shift && ch.size() == 1) {
            char c = ch[0];
            if (c >= 'a' && c <= 'z') ch = std::string(1, static_cast<char>(c - 32));
        }
        
        if (eventType == KEYEV_TOUCH_DOWN) last_key = ch;
        return ch;
    } else if (row_idx == 4) {
        std::string cmd;
        if (x < 50) {
            if (eventType == KEYEV_TOUCH_DOWN) shift = !shift;
        } else if (x < 100) {
            cmd = "BACKSPACE";
        } else if (x < 260) {
            cmd = " ";
        } else {
            cmd = "ENTER";
        }
        if (eventType == KEYEV_TOUCH_DOWN) last_key = cmd;
        return cmd;
    }
    return "";
}

std::vector<Keyboard::KeyRect> Keyboard::getMathRects() {
    std::vector<KeyRect> keys;
    int start_y = y + TAB_H;
    int total_h = KBD_H - TAB_H;
    int row_h = total_h / 4;
    int side_w = 50;
    int center_w = SCREEN_W - (side_w * 2);
    int numpad_w = center_w / 3;
    
    const char* ops[] = {"+", "-", "*", "/"};
    for (int i = 0; i < 4; ++i) {
        keys.push_back({0, start_y + i*row_h, side_w, row_h, ops[i], "", false, true});
    }
    
    struct RowChar { const char* disp; const char* val; bool spec; bool acc; };
    RowChar r_chars[] = {
        {"%", "", false, true}, {" ", "", false, true},
        {"<-", "BACKSPACE", true, false}, {"EXE", "ENTER", false, true}
    };
    for (int i = 0; i < 4; ++i) {
        keys.push_back({SCREEN_W - side_w, start_y + i*row_h, side_w, row_h,
                        r_chars[i].disp, r_chars[i].val, r_chars[i].spec, r_chars[i].acc});
    }
    
    const char* nums[][3] = {{"1","2","3"}, {"4","5","6"}, {"7","8","9"}};
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            keys.push_back({side_w + c*numpad_w, start_y + r*row_h, numpad_w, row_h,
                            nums[r][c], "", false, false});
        }
    }
    
    int y_bot = start_y + 3*row_h;
    int unit_w = center_w / 6;
    const char* bot_row[] = {",", "#", "0", "=", "."};
    int widths[] = {1, 1, 2, 1, 1};
    int cur_x = side_w;
    for (int i = 0; i < 5; ++i) {
        int w = widths[i] * unit_w;
        if (i == 4) w = (side_w + center_w) - cur_x;
        keys.push_back({cur_x, y_bot, w, row_h, bot_row[i], "", false, false});
        cur_x += w;
    }
    
    return keys;
}

std::vector<Keyboard::KeyRect> Keyboard::getNumpadRects(bool floatOpt, bool negOpt) {
    std::vector<KeyRect> keys;
    int start_y = y;
    int total_h = KBD_H;
    int row_h = total_h / 4;
    int action_w = 80;
    int digit_w = (SCREEN_W - action_w) / 3;
    
    keys.push_back({SCREEN_W - action_w, start_y, action_w, row_h, "<-", "BACKSPACE", true, false});
    keys.push_back({SCREEN_W - action_w, start_y + row_h, action_w, row_h*3, "EXE", "ENTER", false, true});
    
    const char* nums[][3] = {{"1","2","3"}, {"4","5","6"}, {"7","8","9"}};
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            keys.push_back({c*digit_w, start_y + r*row_h, digit_w, row_h, nums[r][c], "", false, false});
        }
    }
    
    int y_bot = start_y + 3*row_h;
    std::vector<const char*> bot_keys;
    if (negOpt) bot_keys.push_back("-");
    bot_keys.push_back("0");
    if (floatOpt) bot_keys.push_back(".");
    
    if (!bot_keys.empty()) {
        int bw = (SCREEN_W - action_w) / static_cast<int>(bot_keys.size());
        int cur_x = 0;
        for (size_t i = 0; i < bot_keys.size(); ++i) {
            int w = bw;
            if (i == bot_keys.size() - 1) w = (SCREEN_W - action_w) - cur_x;
            keys.push_back({cur_x, y_bot, w, row_h, bot_keys[i], "", false, false});
            cur_x += w;
        }
    }
    
    return keys;
}

void Keyboard::drawKeysFromRects(const std::vector<KeyRect>& rects) {
    for (const auto& rect : rects) {
        std::string check_val = !rect.value.empty() ? rect.value : rect.label;
        bool is_pressed = (last_key == check_val);
        drawKey(rect.x, rect.y, rect.w, rect.h, rect.label, rect.isSpecial, is_pressed, rect.isAccent);
    }
}

std::string Keyboard::updateKeysFromRects(const std::vector<KeyRect>& rects, int x, int y, int eventType) {
    for (const auto& rect : rects) {
        if (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h) {
            std::string ret = !rect.value.empty() ? rect.value : rect.label;
            if (eventType == KEYEV_TOUCH_DOWN) last_key = ret;
            return ret;
        }
    }
    return "";
}

void Keyboard::draw() {
    if (!visible) return;
    
    const Theme& t = theme;
    drect(0, y, SCREEN_W, SCREEN_H, t.kbd_bg);
    dhline(y, t.key_spec);
    
    if (enable_tabs) {
        drawTabs();
        if (current_tab == KeyboardTab::Math) {
            drawKeysFromRects(getMathRects());
        } else {
            drawGrid();
        }
    } else {
        drawKeysFromRects(getNumpadRects());
    }
}

std::string Keyboard::update(const keyevent_t& ev) {
    if (ev.type == KEYEV_TOUCH_DOWN) {
        last_key.clear();
    }
    
    if (!visible) return "";
    
    int x = ev.x, y_ev = ev.y;
    if (y_ev < y) return "";
    
    // Only process taps on tabs, ignore drags
    if (enable_tabs && y_ev < y + TAB_H) {
        if (ev.type == KEYEV_TOUCH_DOWN) {
            int tab_w = SCREEN_W / 3;
            current_tab = static_cast<KeyboardTab>(std::min(2, std::max(0, x / tab_w)));
        }
        return "";
    }
    
    // Determine active update method
    if (!enable_tabs) {
        return updateKeysFromRects(getNumpadRects(), x, y_ev, ev.type);
    } else if (current_tab == KeyboardTab::Math) {
        return updateKeysFromRects(getMathRects(), x, y_ev, ev.type);
    } else {
        return updateGrid(x, y_ev, ev.type);
    }
}

// =============================================================================
// UTILITY FUNCTIONS IMPLEMENTATION
// =============================================================================

std::string inputDialog(const std::string& prompt, const std::string& themeName) {
    // Simple implementation - would need full dialog UI
    (void)prompt;
    (void)themeName;
    return "";
}

std::optional<int> pickList(const std::vector<std::string>& options,
                            const std::string& prompt,
                            const std::string& themeName,
                            bool multi) {
    (void)options;
    (void)prompt;
    (void)themeName;
    (void)multi;
    return std::nullopt;
}

bool confirmDialog(const std::string& message, const std::string& themeName) {
    (void)message;
    (void)themeName;
    return false;
}

void showToast(const std::string& message, int duration) {
    (void)message;
    (void)duration;
}
