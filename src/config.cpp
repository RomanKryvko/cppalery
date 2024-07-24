#include "config.h"
#include <stdexcept>

fs::path Config::expandHomeInPath(const std::string &str) {
    std::string homeDir = getenv("HOME");
    if (str.substr(0, 5) == "$HOME") {
        return fs::canonical(homeDir + str.substr(5));
    } else {
        return fs::canonical(str);
    }
}

Config::Config(bool isPathRelative, bool showPreview, bool sortByNameAscending) {
    this->isPathRelative = isPathRelative;
    this->showPreview = showPreview;
    this->sortByNameAscending = sortByNameAscending;
    pathSet = false;
    wallpaperCommandsSet = false;
}

fs::path Config::getPath() {
    return workpath;
}

void Config::setPath(std::string path) {
    workpath = fs::canonical(path);
    if (!fs::exists(path)) {
       throw std::invalid_argument("Provided path not valid.");
    }
    pathSet = true;
}

Config::Config(const Config& other) {
    pathSet = other.pathSet;
    wallpaperCommandsSet = other.wallpaperCommandsSet;
    workpath = other.workpath;
    isPathRelative = other.isPathRelative;
    showPreview = other.showPreview;
    sortByNameAscending = other.sortByNameAscending;
    wallpaperFillCommand = other.wallpaperFillCommand;
    wallpaperCenterCommand = other.wallpaperCenterCommand;
}

Config& Config::operator=(const Config& other) {
    pathSet = other.pathSet;
    wallpaperCommandsSet = other.wallpaperCommandsSet;
    workpath = other.workpath;
    isPathRelative = other.isPathRelative;
    showPreview = other.showPreview;
    sortByNameAscending = other.sortByNameAscending;
    wallpaperFillCommand = other.wallpaperFillCommand;
    wallpaperCenterCommand = other.wallpaperCenterCommand;

    return *this;
}

bool Config::isPathSet() {
    return pathSet;
}

bool Config::areWallpaperCommandsSet() {
    return wallpaperCommandsSet;
}

bool Config::parseConfig(std::string configPath) {
    bool wallpaperFillSet = false;
    bool wallpaperCenterSet = false;

    std::string line;
    std::ifstream file (configPath);
    if (!file.is_open()) {
        return false;
    }
    while (std::getline(file, line)) {
        if (line[0] == '#')
            continue;
        std::istringstream iss (line);
        std::string word;
        iss >> word;
        if (word == workPathConfLine) {
            std::getline(iss, word, '\"');
            std::getline(iss, word, '\"');
            try {
                workpath = expandHomeInPath(word);
            }
            catch (const std::exception &ex) {
                std::cerr << ex.what() << "\n";
                continue;
            }
            pathSet = true;
            continue;
        }
        else if (word == relativePathConfLine) {
            iss >> word >> word;
            isPathRelative = (word == "true");
            continue;
        }
        else if (word == sortAscConfLine) {
            iss >> word >> word;
            sortByNameAscending = (word == "true");
            continue;
        }
        else if (word == previewConfLine) {
            iss >> word >> word;
            showPreview = (word == "true");
            continue;
        }
        else if (word == wallpaperFillConfLine) {
            std::getline(iss, word, '\"');
            std::getline(iss, wallpaperFillCommand, '\"');
            wallpaperFillSet = true;
            continue;
        }
        else if (word == wallpaperCenterConfLine) {
            std::getline(iss, word, '\"');
            std::getline(iss, wallpaperCenterCommand, '\"');
            wallpaperCenterSet = true;
            continue;
        }
    }
    wallpaperCommandsSet = wallpaperCenterSet && wallpaperFillSet;
    return true;
}
