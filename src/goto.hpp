#ifndef GOTO_HPP
#define GOTO_HPP

#include "ncinput.hpp"
#include <string>
#include <functional>

// =============================================================================
// GO TO LINE COMPONENT
// =============================================================================

class GoToScene {
public:
    using GoToCallback = std::function<void(int line)>;
    
    GoToScene(int totalLines, const std::string& themeName = "light");
    
    void draw();
    struct ListView::ActionResult update(const keyevent_t& ev);
    
    // Set total lines for validation
    void setTotalLines(int lines) { totalLines = lines; }
    
    // Get current input
    int getLineNumber() const { return inputLine; }
    
    // Callback when user confirms goto
    void setGoToCallback(GoToCallback cb) { onGoTo = cb; }
    
private:
    std::string themeName;
    int totalLines;
    int inputLine = 1;
    
    Keyboard keyboard;
    ListView listView;
    
    GoToCallback onGoTo;
    
    bool validateAndConfirm();
};

#endif // GOTO_HPP
