#ifndef PROBLEMS_HPP
#define PROBLEMS_HPP

#include "ncinput.hpp"
#include <string>
#include <vector>
#include <functional>
#include <memory>

// =============================================================================
// PROBLEMS/LINTER COMPONENT (Pluggable Framework)
// =============================================================================

enum class ProblemSeverity { Info, Warning, Error };

struct Problem {
    std::string message;
    int line;           // Line number (0-indexed, -1 for file-wide)
    int column;         // Column position (-1 if not applicable)
    int length;         // Length of problematic region (-1 if not applicable)
    ProblemSeverity severity;
    std::string source; // Which linter/plugin reported this
};

// Abstract base class for linter plugins
class LinterPlugin {
public:
    virtual ~LinterPlugin() = default;
    
    // Run the linter on the given file content
    // Returns a list of problems found
    virtual std::vector<Problem> lint(const std::string& filePath, 
                                       const std::string& content) = 0;
    
    // Human-readable name for the plugin
    virtual const char* getName() const = 0;
};

// Python-style class/method detection plugin (stub)
class PythonOutlinePlugin : public LinterPlugin {
public:
    std::vector<Problem> lint(const std::string& filePath, 
                              const std::string& content) override;
    const char* getName() const override { return "Python Outline"; }
};

class ProblemsView {
public:
    using ProblemSelectedCallback = std::function<void(const Problem&)>;
    
    ProblemsView(const std::string& themeName = "light");
    
    void draw();
    struct ListView::ActionResult update(const keyevent_t& ev);
    
    // Register a linter plugin
    void registerPlugin(std::unique_ptr<LinterPlugin> plugin);
    
    // Run all registered linters on the given content
    void runLinters(const std::string& filePath, const std::string& content);
    
    // Clear all problems
    void clear();
    
    // Get problems
    const std::vector<Problem>& getProblems() const { return problems; }
    
    // Callback when user selects a problem
    void setProblemSelectedCallback(ProblemSelectedCallback cb) { onProblemSelected = cb; }
    
private:
    std::string themeName;
    std::vector<Problem> problems;
    std::vector<std::unique_ptr<LinterPlugin>> plugins;
    
    ListView listView;
    
    ProblemSelectedCallback onProblemSelected;
    
    void rebuildListView();
    std::string getSeverityIcon(ProblemSeverity sev);
    color_t getSeverityColor(ProblemSeverity sev);
};

#endif // PROBLEMS_HPP
