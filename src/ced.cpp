#include "ced.hpp"
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>
#include <algorithm>

// =============================================================================
// CONSTANTS & DEFAULTS
// =============================================================================

constexpr int SCREEN_W = 320;
constexpr int SCREEN_H = 528;

// Default syntax colors
static color_t COL_BG = C_WHITE;
static color_t COL_TXT = C_BLACK;
static color_t COL_KW = C_BLUE;
static color_t COL_STR = 0x0480;  // Dark Green
static color_t COL_COM = 0x7BEF;  // Gray
static color_t COL_NUM = C_RED;
static color_t COL_OP = 0xF81F;   // Magenta

// Keywords for syntax highlighting
static const char* KEYWORDS[] = {
    "def", "class", "if", "else", "elif", "while", "for", "import", "from",
    "return", "True", "False", "None", "break", "continue", "pass", "try",
    "except", "with", "as", "global", "print", "len", "range", "in", "is",
    "not", "and", "or", nullptr
};

// =============================================================================
// CED IMPLEMENTATION
// =============================================================================

CED::CED() 
    : keyboard("light", KeyboardLayout::QWERTY, true),
      fileBrowser(std::make_unique<FileBrowser>("/", "light")),
      searchScene(std::make_unique<SearchScene>("light")),
      goToScene(std::make_unique<GoToScene>(100, "light")),
      problemsView(std::make_unique<ProblemsView>("light")),
      outlineView(std::make_unique<OutlineView>("light"))
{
    lines.push_back("");
    updateColors();
    
    // Setup callbacks
    fileBrowser->setFileSelectedCallback([this](const FileEntry& entry) {
        openFile(entry.path);
        pushScene(CedScene::Editor);
    });
    
    outlineView->setItemSelectedCallback([this](const OutlineItem& item) {
        cursorY = item.line;
        cursorX = 0;
        scrollToCursor();
        pushScene(CedScene::Editor);
    });
}

void CED::updateColors() {
    const Theme& theme = ThemeManager::getTheme(config.theme);
    
    colBg = theme.modal_bg;
    colTxt = theme.txt;
    
    if (config.theme == "dark") {
        COL_KW = 0x4C7F;  // Light Blue
        COL_STR = 0x8666; // Light Green
        COL_OP = 0xF81F;  // Magenta
    } else {
        COL_KW = C_BLUE;
        COL_STR = 0x0480;
        COL_OP = 0xF81F;
    }
    
    colKw = COL_KW;
    colStr = COL_STR;
    colCom = COL_COM;
    colNum = COL_NUM;
    colOp = COL_OP;
}

void CED::switchTheme() {
    const char* themes[] = {"light", "dark", "grey"};
    int idx = 0;
    
    for (int i = 0; themes[i]; ++i) {
        if (config.theme == themes[i]) {
            idx = i;
            break;
        }
    }
    
    config.theme = themes[(idx + 1) % 3];
    updateColors();
    
    // Re-init keyboard with new theme
    bool wasVisible = keyboard.visible;
    keyboard = Keyboard(config.theme, KeyboardLayout::QWERTY, true);
    keyboard.visible = wasVisible;
}

void CED::clampCursor() {
    if (cursorY < 0) cursorY = 0;
    if (cursorY >= static_cast<int>(lines.size())) cursorY = static_cast<int>(lines.size()) - 1;
    
    int lineLen = static_cast<int>(lines[cursorY].length());
    if (cursorX < 0) cursorX = 0;
    if (cursorX > lineLen) cursorX = lineLen;
}

void CED::scrollToCursor() {
    int kbH = keyboard.visible ? KBD_H : 0;
    int viewH = SCREEN_H - HEADER_H - kbH;
    
    // Simple scroll logic - ensure cursor is visible
    if (cursorY < viewY) {
        viewY = cursorY;
        return;
    }
    
    // Safety break for huge files
    if (cursorY > viewY + 100) {
        viewY = cursorY - 5;
        return;
    }
    
    // Calculate visual height to cursor
    int visualH = 0;
    for (int i = viewY; i <= cursorY && i < static_cast<int>(lines.size()); ++i) {
        visualH += TEXT_LINE_H;
    }
    
    while (visualH > viewH && viewY < cursorY) {
        viewY++;
        visualH -= TEXT_LINE_H;
    }
}

void CED::insertChar(char c) {
    clampCursor();
    std::string& line = lines[cursorY];
    line.insert(cursorX, 1, c);
    cursorX++;
    clampCursor();
    fileModified = true;
    outlineNeedsRebuild = true;
}

void CED::deleteChar() {
    clampCursor();
    
    if (cursorX > 0) {
        std::string& line = lines[cursorY];
        line.erase(cursorX - 1, 1);
        cursorX--;
    } else if (cursorY > 0) {
        std::string curr = lines[cursorY];
        lines.erase(lines.begin() + cursorY);
        cursorY--;
        cursorX = static_cast<int>(lines[cursorY].length());
        lines[cursorY] += curr;
    }
    
    clampCursor();
    fileModified = true;
    outlineNeedsRebuild = true;
}

void CED::newLine() {
    clampCursor();
    std::string& line = lines[cursorY];
    std::string rem = line.substr(cursorX);
    line = line.substr(0, cursorX);
    cursorY++;
    lines.insert(lines.begin() + cursorY, rem);
    cursorX = 0;
    clampCursor();
    fileModified = true;
    outlineNeedsRebuild = true;
}

void CED::newFile() {
    if (!canClose()) return;
    
    lines.clear();
    lines.push_back("");
    filename = "untitled.py";
    cursorX = 0;
    cursorY = 0;
    viewY = 0;
    fileModified = false;
    outlineNeedsRebuild = true;
}

void CED::openFile(const std::string& path) {
    if (!canClose()) return;
    
    // NOTE: In real implementation, use gint file APIs
    // For now, just set the filename
    filename = path;
    lines.clear();
    lines.push_back("// File would be loaded here");
    cursorX = 0;
    cursorY = 0;
    viewY = 0;
    fileModified = false;
    outlineNeedsRebuild = true;
    
    showMessage("Opened " + path);
}

void CED::saveFile() {
    if (filename == "untitled.py") {
        saveAs();
        return;
    }
    
    // NOTE: In real implementation, use gint file APIs
    // For now, just mark as saved
    fileModified = false;
    showMessage("Saved " + filename);
}

void CED::saveAs() {
    // Would show input dialog in real implementation
    showMessage("Save As... (not implemented)");
}

bool CED::canClose() {
    if (!fileModified) return true;
    // Would show confirmation dialog
    return true;  // For now, always allow
}

void CED::showMessage(const std::string& msg) {
    message = msg;
    messageTimer = 60;  // ~1 second at 60fps
}

void CED::loadConfig(const std::string& configPath) {
    // NOTE: Would load from actual file
    (void)configPath;
    // Default config already set in struct
}

void CED::saveConfig(const std::string& configPath) {
    // NOTE: Would save to actual file
    (void)configPath;
}

void CED::showMenu() {
    // Would show menu using ListPicker
    std::vector<std::string> options = {
        "New",
        "Open...",
        "Save",
        "Save As...",
        "Search...",
        "Go To Line...",
        "Word Wrap: " + std::string(config.wordWrap ? "On" : "Off"),
        "Theme: " + config.theme,
        "Quit"
    };
    
    // Simplified - would use pickList in real implementation
    (void)options;
}

void CED::pushScene(CedScene scene) {
    previousScene = currentScene;
    currentScene = scene;
    
    // Scene-specific initialization
    switch (scene) {
        case CedScene::Outline:
            if (outlineNeedsRebuild || outlineView->isFileChanged()) {
                outlineView->buildFromFile(filename);
            }
            break;
        default:
            break;
    }
}

void CED::popScene() {
    currentScene = previousScene;
}

void CED::handleEditorEvent(const keyevent_t& ev) {
    if (ev.type == KEYEV_DOWN) {
        if (ev.key == KEY_MENU || ev.key == KEY_F1) {
            showMenu();
            clearevents();
            return;
        } else if (ev.key == KEY_EXIT || ev.key == KEY_KBD) {
            keyboard.visible = !keyboard.visible;
        } else if (ev.key == KEY_UP) {
            cursorY--;
            clampCursor();
            scrollToCursor();
        } else if (ev.key == KEY_DOWN) {
            cursorY++;
            clampCursor();
            scrollToCursor();
        } else if (ev.key == KEY_LEFT) {
            cursorX--;
            clampCursor();
        } else if (ev.key == KEY_RIGHT) {
            cursorX++;
            clampCursor();
        } else if (ev.key == KEY_EXE) {
            newLine();
        } else if (ev.key == KEY_DEL) {
            deleteChar();
        }
    }
    
    // Keyboard handling
    if (keyboard.visible) {
        std::string result = keyboard.update(ev);
        if (!result.empty()) {
            if (result == "ENTER") {
                newLine();
            } else if (result == "BACKSPACE") {
                deleteChar();
            } else if (result.length() == 1) {
                insertChar(result[0]);
            }
        }
    }
}

void CED::handleFileBrowserEvent(const keyevent_t& ev) {
    fileBrowser->update(ev);
}

void CED::handleSearchEvent(const keyevent_t& ev) {
    searchScene->update(ev);
}

void CED::handleGoToEvent(const keyevent_t& ev) {
    goToScene->update(ev);
}

void CED::handleProblemsEvent(const keyevent_t& ev) {
    problemsView->update(ev);
}

void CED::handleOutlineEvent(const keyevent_t& ev) {
    outlineView->update(ev);
}

void CED::drawHeader() {
    const Theme& theme = ThemeManager::getTheme(config.theme);
    color_t headerCol = theme.accent;
    color_t headerTxt = theme.txt_acc;
    
    drect(0, 0, SCREEN_W, HEADER_H, headerCol);
    
    // Menu icon (left)
    for (int i = 0; i < 3; ++i) {
        drect(10, 10 + i*5, 28, 11 + i*5, headerTxt);
    }
    
    // Keyboard toggle icon (right)
    int kbdX = SCREEN_W - 35;
    drect_border(kbdX, 12, kbdX + 22, 26, C_NONE, 1, headerTxt);
    if (!keyboard.visible) {
        for (int r = 0; r < 2; ++r) {
            for (int c = 0; c < 3; ++c) {
                drect(kbdX + 3 + c*6, 15 + r*5, kbdX + 6 + c*6, 17 + r*5, headerTxt);
            }
        }
    }
    
    // Title (centered)
    std::string title = filename + (fileModified ? "*" : "");
    dtext_opt(SCREEN_W/2, HEADER_H/2, headerTxt, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE,
              title.c_str(), -1);
}

void CED::drawTextContent(int viewH) {
    dwindow_set(0, HEADER_H, SCREEN_W, SCREEN_H);
    
    int currentScreenY = HEADER_H + 6;
    int maxY = HEADER_H + viewH;
    
    for (int i = viewY; i < static_cast<int>(lines.size()); ++i) {
        if (currentScreenY >= maxY) break;
        
        const std::string& line = lines[i];
        
        // Draw cursor if on this line
        if (i == cursorY) {
            // Calculate cursor X position (simplified)
            int cursorScreenY = currentScreenY;
            if (cursorScreenY < maxY) {
                // Find cursor pixel position
                int cx = TEXT_MARGIN_X;
                // Simplified - would calculate based on text width
                drect(cx, cursorScreenY, cx + 2, cursorScreenY + TEXT_LINE_H - 2, colTxt);
            }
        }
        
        // Draw line content (simplified - no syntax highlighting shown)
        dtext(TEXT_MARGIN_X, currentScreenY + TEXT_Y_OFFSET, colTxt, line.c_str());
        
        currentScreenY += TEXT_LINE_H;
    }
    
    dwindow_set(0, 0, SCREEN_W, SCREEN_H);
}

void CED::drawMessage() {
    if (messageTimer > 0) {
        messageTimer--;
        int kbH = keyboard.visible ? KBD_H : 0;
        dtext(10, SCREEN_H - kbH - 20, C_RED, message.c_str());
    }
}

void CED::draw() {
    dclear(colBg);
    
    int kbH = keyboard.visible ? KBD_H : 0;
    int viewH = SCREEN_H - HEADER_H - kbH;
    
    switch (currentScene) {
        case CedScene::Editor:
            drawHeader();
            drawTextContent(viewH);
            drawMessage();
            keyboard.draw();
            break;
            
        case CedScene::FileBrowser:
            fileBrowser->draw();
            break;
            
        case CedScene::Search:
            searchScene->draw();
            break;
            
        case CedScene::GoTo:
            goToScene->draw();
            break;
            
        case CedScene::Problems:
            problemsView->draw();
            break;
            
        case CedScene::Outline:
            outlineView->draw();
            break;
            
        default:
            drawHeader();
            drawTextContent(viewH);
            keyboard.draw();
            break;
    }
    
    dupdate();
}

void CED::run() {
    clearevents();
    bool running = true;
    bool touchLatched = false;
    
    while (running) {
        draw();
        cleareventflips();
        
        keyevent_t ev;
        while ((ev = pollevent()).type != KEYEV_NONE) {
            // Touch latch handling
            if (ev.type == KEYEV_TOUCH_UP) {
                touchLatched = false;
                keyboard.last_key.clear();
            } else if (ev.type == KEYEV_TOUCH_DOWN && !touchLatched) {
                touchLatched = true;
                
                // Header interaction in editor mode
                if (currentScene == CedScene::Editor && ev.y < HEADER_H) {
                    if (ev.x < 40) {
                        showMenu();
                    } else if (ev.x > SCREEN_W - 40) {
                        keyboard.visible = !keyboard.visible;
                    }
                }
            }
            
            // Route event to current scene handler
            switch (currentScene) {
                case CedScene::Editor:
                    handleEditorEvent(ev);
                    break;
                case CedScene::FileBrowser:
                    handleFileBrowserEvent(ev);
                    break;
                case CedScene::Search:
                    handleSearchEvent(ev);
                    break;
                case CedScene::GoTo:
                    handleGoToEvent(ev);
                    break;
                case CedScene::Problems:
                    handleProblemsEvent(ev);
                    break;
                case CedScene::Outline:
                    handleOutlineEvent(ev);
                    break;
                default:
                    handleEditorEvent(ev);
                    break;
            }
        }
    }
}

// Entry point function
extern "C" void ced_app() {
    CED editor;
    editor.loadConfig();
    editor.run();
    editor.saveConfig();
}
