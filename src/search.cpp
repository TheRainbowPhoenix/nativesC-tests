#include "search.hpp"
#include <fstream>
#include <vector>

namespace ced {

SearchResult show_search(std::string const& filename, ncinput::ThemeName theme) {
    std::string query = ncinput::input("Search:", "alpha_numeric", theme);
    if (query.empty()) return {-1, -1};

    std::ifstream f(filename);
    std::string line;
    int line_idx = 0;
    while (std::getline(f, line)) {
        size_t pos = line.find(query);
        if (pos != std::string::npos) {
            return {line_idx, (int)pos};
        }
        line_idx++;
    }
    return {-1, -1};
}

void show_replace(std::string const& filename, ncinput::ThemeName theme) {
    std::string query = ncinput::input("Search for:", "alpha_numeric", theme);
    if (query.empty()) return;
    std::string replacement = ncinput::input("Replace with:", "alpha_numeric", theme);

    std::string temp_name = filename + ".tmp";
    std::ifstream f(filename);
    std::ofstream out(temp_name);
    std::string line;
    while (std::getline(f, line)) {
        size_t pos = 0;
        while ((pos = line.find(query, pos)) != std::string::npos) {
            line.replace(pos, query.length(), replacement);
            pos += replacement.length();
        }
        out << line << "\n";
    }
    f.close();
    out.close();
    std::remove(filename.c_str());
    std::rename(temp_name.c_str(), filename.c_str());
}

}
