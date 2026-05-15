#include "filebrowser.hpp"
#include <gint/display.h>
#include <algorithm>

// =============================================================================
// PATH UTILITIES
// =============================================================================

std::string getFileName(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) return path;
    return path.substr(pos + 1);
}

std::string getParentPath(const std::string& path) {
    if (path.empty() || path == "/") return "/";
    
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos || pos == 0) return "/";
    
    return path.substr(0, pos);
}

std::string joinPath(const std::string& base, const std::string& name) {
    if (base.empty()) return name;
    if (name.empty()) return base;
    
    if (base.back() == '/') return base + name;
    return base + "/" + name;
}

// =============================================================================
// FILE BROWSER IMPLEMENTATION
// =============================================================================

FileBrowser::FileBrowser(const std::string& initialPath, const std::string& themeName)
    : currentPath(initialPath), themeName(themeName),
      listView(0, 0, SCREEN_W, SCREEN_H - 40, {}, 50, themeName)
{
    loadDirectory(initialPath);
    
    listView.setClickCallback([this](int index, const ListItem& item) {
        handleEntrySelection(index, item);
    });
}

void FileBrowser::loadDirectory(const std::string& path) {
    entries.clear();
    
    // Add parent directory entry if not at root
    if (path != "/" && !path.empty()) {
        FileEntry parent;
        parent.name = "..";
        parent.path = getParentPath(path);
        parent.isDirectory = true;
        entries.push_back(parent);
    }
    
    // NOTE: In a real implementation, this would use gint file APIs
    // to enumerate the directory contents. For now, we create dummy entries.
    // This is a placeholder that should be replaced with actual file system access.
    
    // Example dummy entries for demonstration
    /*
    FileEntry dir1;
    dir1.name = "Documents";
    dir1.path = joinPath(path, "Documents");
    dir1.isDirectory = true;
    entries.push_back(dir1);
    
    FileEntry file1;
    file1.name = "example.py";
    file1.path = joinPath(path, "example.py");
    file1.isDirectory = false;
    file1.size = 1024;
    entries.push_back(file1);
    */
    
    // Build ListView items from entries
    std::vector<ListItem> listItems;
    for (const auto& entry : entries) {
        ListItem item;
        item.text = entry.name;
        item.type = ItemType::Item;
        item.arrow = entry.isDirectory;
        
        if (!entry.isDirectory) {
            // Show file size
            char sizeStr[32];
            if (entry.size >= 1024) {
                snprintf(sizeStr, sizeof(sizeStr), " (%d KB)", entry.size / 1024);
            } else {
                snprintf(sizeStr, sizeof(sizeStr), " (%d B)", entry.size);
            }
            item.text += sizeStr;
        }
        
        listItems.push_back(item);
    }
    
    listView.getItems() = listItems;
    listView.recalcLayout();
}

void FileBrowser::navigateTo(const std::string& path) {
    currentPath = path;
    loadDirectory(path);
    
    if (onDirectoryChanged) {
        onDirectoryChanged(path);
    }
}

void FileBrowser::navigateUp() {
    if (currentPath != "/" && !currentPath.empty()) {
        navigateTo(getParentPath(currentPath));
    }
}

void FileBrowser::refresh() {
    loadDirectory(currentPath);
}

void FileBrowser::handleEntrySelection(int index, const ListItem& item) {
    (void)item;
    
    if (index < 0 || index >= static_cast<int>(entries.size())) return;
    
    const FileEntry& entry = entries[index];
    
    if (entry.isDirectory) {
        if (entry.name == "..") {
            navigateUp();
        } else {
            navigateTo(entry.path);
        }
    } else {
        // File selected
        if (onFileSelected) {
            onFileSelected(entry);
        }
    }
}

struct ListView::ActionResult FileBrowser::update(const keyevent_t& ev) {
    return listView.update(ev);
}

void FileBrowser::draw() {
    const Theme& theme = ThemeManager::getTheme(themeName);
    
    // Draw header
    drect(0, 0, SCREEN_W, 40, theme.accent);
    dtext_opt(SCREEN_W/2, 20, theme.txt_acc, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE, 
              currentPath.c_str(), -1);
    
    // Draw file list
    listView.y = 40;
    listView.h = SCREEN_H - 40;
    listView.draw();
}
