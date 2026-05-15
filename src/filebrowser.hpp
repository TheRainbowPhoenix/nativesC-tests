#ifndef FILEBROWSER_HPP
#define FILEBROWSER_HPP

#include "ncinput.hpp"
#include <string>
#include <vector>
#include <functional>

// =============================================================================
// FILE BROWSER COMPONENT
// =============================================================================

struct FileEntry {
    std::string name;
    std::string path;
    bool isDirectory = false;
    int size = 0;  // in bytes, 0 for directories
};

class FileBrowser {
public:
    using FileSelectedCallback = std::function<void(const FileEntry&)>;
    using DirectoryChangedCallback = std::function<void(const std::string&)>;
    
    FileBrowser(const std::string& initialPath = "/", const std::string& themeName = "light");
    
    void draw();
    struct ActionResult update(const keyevent_t& ev);
    
    // Navigation
    void navigateTo(const std::string& path);
    void navigateUp();
    void refresh();
    
    // Getters
    const std::string& getCurrentPath() const { return currentPath; }
    const std::vector<FileEntry>& getEntries() const { return entries; }
    
    // Callbacks
    void setFileSelectedCallback(FileSelectedCallback cb) { onFileSelected = cb; }
    void setDirectoryChangedCallback(DirectoryChangedCallback cb) { onDirectoryChanged = cb; }
    
private:
    std::string currentPath;
    std::vector<FileEntry> entries;
    ListView listView;
    std::string themeName;
    
    FileSelectedCallback onFileSelected;
    DirectoryChangedCallback onDirectoryChanged;
    
    void loadDirectory(const std::string& path);
    void handleEntrySelection(int index, const ListItem& item);
};

// Utility functions
std::string getFileName(const std::string& path);
std::string getParentPath(const std::string& path);
std::string joinPath(const std::string& base, const std::string& name);

#endif // FILEBROWSER_HPP
