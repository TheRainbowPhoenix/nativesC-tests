#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "ncinput.hpp"
#include <string>
#include <vector>
#include <functional>

// =============================================================================
// SEARCH COMPONENT (Search & Replace)
// =============================================================================

struct SearchResult {
    int line;       // Line number (0-indexed)
    int column;     // Column position
    int length;     // Length of match
    std::string text;  // Matched text context
};

class SearchScene {
public:
    using SearchCallback = std::function<void(const std::vector<SearchResult>&)>;
    using ReplaceCallback = std::function<void(int line, const std::string& replacement)>;
    
    SearchScene(const std::string& themeName = "light");
    
    void draw();
    struct ListView::ActionResult update(const keyevent_t& ev);
    
    // Set the file content to search (can be a file path or use chunked reading)
    void setFileSource(const std::string& filePath);
    
    // Search operations
    void search(const std::string& pattern, bool caseSensitive = false);
    void searchAndReplace(const std::string& pattern, const std::string& replacement, 
                          bool replaceAll = false);
    
    // Navigation
    void goToNextResult();
    void goToPreviousResult();
    void selectResult(int index);
    
    // Getters
    const std::vector<SearchResult>& getResults() const { return results; }
    int getCurrentResultIndex() const { return currentResultIndex; }
    
    // Callbacks
    void setSearchCompleteCallback(SearchCallback cb) { onSearchComplete = cb; }
    void setReplaceCallback(ReplaceCallback cb) { onReplace = cb; }
    
private:
    std::string themeName;
    std::string filePath;
    std::string searchPattern;
    std::string replaceText;
    
    std::vector<SearchResult> results;
    int currentResultIndex = -1;
    
    ListView listView;
    Keyboard keyboard;
    
    enum class Mode { SearchInput, ReplaceInput, Results };
    Mode currentMode = Mode::SearchInput;
    
    bool caseSensitive = false;
    bool showReplaceField = false;
    
    SearchCallback onSearchComplete;
    ReplaceCallback onReplace;
    
    // Efficient chunked file reading
    void searchInChunks(const std::string& pattern);
    std::vector<SearchResult> findInLine(int lineNum, const std::string& line, 
                                          const std::string& pattern);
};

#endif // SEARCH_HPP
