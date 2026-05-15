#ifndef CED_HPP
#define CED_HPP

#include "ncinput.hpp"
#include "filebrowser.hpp"
#include "search.hpp"
#include "goto.hpp"
#include "problems.hpp"
#include "outline.hpp"
#include <string>
#include <vector>
#include <memory>

// =============================================================================
// CED - Code EDitor Application
// =============================================================================

// Scene states for the state machine
enum class CedScene {
    Editor,
    FileBrowser,
    Search,
    GoTo,
    Problems,
    Outline,
    Menu,
    Dialog
};

struct CedConfig {
    std::string theme = "light";
    bool wordWrap = false;
    int tabSize = 4;
    bool lineNumbers = true;
    bool autoSave = false;
    int autoSaveInterval = 60;  // seconds
    std::vector<std::string> recentFiles;
};

class CED {
public:
    CED();
    
    // Main entry point - runs the editor
    void run();
    
    // Load config from .ced file
    void loadConfig(const std::string& configPath = "/.ced");
    void saveConfig(const std::string& configPath = "/.ced");
    
private:
    // State
    CedScene currentScene = CedScene::Editor;
    CedScene previousScene = CedScene::Editor;
    CedConfig config;
    
    // Editor state
    std::vector<std::string> lines;
    std::string filename = "untitled.py";
    int cursorX = 0;
    int cursorY = 0;
    int viewY = 0;
    bool fileModified = false;
    bool outlineNeedsRebuild = true;
    
    // UI Components
    Keyboard keyboard;
    std::unique_ptr<FileBrowser> fileBrowser;
    std::unique_ptr<SearchScene> searchScene;
    std::unique_ptr<GoToScene> goToScene;
    std::unique_ptr<ProblemsView> problemsView;
    std::unique_ptr<OutlineView> outlineView;
    
    // Message system
    std::string message;
    int messageTimer = 0;
    
    // Syntax highlighting colors (loaded from theme)
    color_t colBg, colTxt, colKw, colStr, colCom, colNum, colOp;
    
    // Constants
    static constexpr int HEADER_H = 40;
    static constexpr int TEXT_LINE_H = 20;
    static constexpr int TEXT_MARGIN_X = 5;
    static constexpr int TEXT_Y_OFFSET = 4;
    
    // Methods
    void updateColors();
    void switchTheme();
    
    // Cursor management
    void clampCursor();
    void scrollToCursor();
    
    // Text editing
    void insertChar(char c);
    void deleteChar();
    void newLine();
    
    // File operations
    void newFile();
    void openFile(const std::string& path);
    void saveFile();
    void saveAs();
    bool canClose();  // Returns true if safe to close (no unsaved changes or user confirms)
    
    // Drawing
    void draw();
    void drawHeader();
    void drawTextContent(int viewH);
    void drawMessage();
    
    // Scene management
    void pushScene(CedScene scene);
    void popScene();
    
    // Event handling
    void handleEditorEvent(const keyevent_t& ev);
    void handleFileBrowserEvent(const keyevent_t& ev);
    void handleSearchEvent(const keyevent_t& ev);
    void handleGoToEvent(const keyevent_t& ev);
    void handleProblemsEvent(const keyevent_t& ev);
    void handleOutlineEvent(const keyevent_t& ev);
    
    // Menu
    void showMenu();
    
    // Utility
    void showMessage(const std::string& msg);
    std::vector<std::string> tokenizeLine(const std::string& line);
};

#endif // CED_HPP
