#ifndef OUTLINE_HPP
#define OUTLINE_HPP

#include "ncinput.hpp"
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <atomic>

// =============================================================================
// OUTLINE VIEW COMPONENT (Expandable Tree View)
// =============================================================================

enum class OutlineItemType { Class, Method, Function, Variable, Section };

struct OutlineItem {
    std::string name;
    std::string type;     // e.g., "class", "def", "function"
    int line;             // Line number where this item is defined
    int endLine;          // End line (-1 if unknown)
    int indentLevel;      // Nesting level for tree display
    OutlineItemType itemType;
    
    bool expanded = true; // For collapsible tree nodes
    std::vector<std::shared_ptr<OutlineItem>> children;
};

// Abstract base class for outline provider plugins
class OutlinePlugin {
public:
    virtual ~OutlinePlugin() = default;
    
    // Parse the file content and build an outline tree
    // Should support interruption via shouldStop flag
    virtual std::vector<std::shared_ptr<OutlineItem>> 
        buildOutline(const std::string& filePath, 
                     const std::string& content,
                     std::atomic<bool>& shouldStop) = 0;
    
    // Human-readable name for the plugin
    virtual const char* getName() const = 0;
};

// Python class/method detection plugin
class PythonOutlinePlugin : public OutlinePlugin {
public:
    std::vector<std::shared_ptr<OutlineItem>> 
        buildOutline(const std::string& filePath, 
                     const std::string& content,
                     std::atomic<bool>& shouldStop) override;
    const char* getName() const override { return "Python"; }
};

class OutlineView {
public:
    using ItemSelectedCallback = std::function<void(const OutlineItem&)>;
    
    OutlineView(const std::string& themeName = "light");
    
    void draw();
    struct ListView::ActionResult update(const keyevent_t& ev);
    
    // Register an outline provider plugin
    void registerPlugin(std::unique_ptr<OutlinePlugin> plugin);
    
    // Build outline from file (runs in background thread)
    // Shows progress indicator while building
    void buildFromFile(const std::string& filePath);
    
    // Cancel ongoing build
    void cancelBuild();
    
    // Check if file has changed since last build
    void markFileChanged() { fileChanged = true; }
    bool isFileChanged() const { return fileChanged; }
    
    // Get outline items
    const std::vector<std::shared_ptr<OutlineItem>>& getItems() const { return items; }
    
    // Callback when user selects an item (jumps to that line)
    void setItemSelectedCallback(ItemSelectedCallback cb) { onItemSelected = cb; }
    
    // Check if currently building
    bool isBuilding() const { return building; }
    
private:
    std::string themeName;
    std::vector<std::shared_ptr<OutlineItem>> items;
    std::vector<std::unique_ptr<OutlinePlugin>> plugins;
    
    ListView listView;
    
    ItemSelectedCallback onItemSelected;
    
    std::atomic<bool> building{false};
    std::atomic<bool> shouldStop{false};
    bool fileChanged = false;
    
    // Flatten tree into list items for ListView
    void flattenTree(const std::vector<std::shared_ptr<OutlineItem>>& tree,
                     std::vector<ListItem>& outItems, int depth = 0);
    void rebuildListView();
    
    std::string getTypeIcon(OutlineItemType type);
    color_t getTypeColor(OutlineItemType type);
};

#endif // OUTLINE_HPP
