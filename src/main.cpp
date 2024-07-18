#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "form.h"
#include "keyGlobals.h"

const std::string CONFIG_PATH = std::string(getenv("HOME")).append("/.config/cppalery/config");
const std::string workPathConfLine = "workpath";
const std::string relativePathConfLine = "relative_path";
const std::string sortAscConfLine = "sort_name_ascending";
const std::string previewConfLine = "preview";
const std::string wallpaperFillConfLine = "wallpaper_set_fill";
const std::string wallpaperCenterConfLine = "wallpaper_set_center";

const char CONFIG_FLAG = 'c';
const char HELP_FLAG = 'h';
const std::string HELP_MSG {
    "Cppalery: file browsing and wallpaper setting application\n\n"
    "Usage: cppalery [option]\n"
    "Options:\n"
    " -" + std::string{HELP_FLAG} + "\t\tPrint this help message\n"
    " -" + std::string{CONFIG_FLAG} + "\t\tSet path to load config file from\n"
    " <path>\t\tSet starting workpath\n\n"
    " Press " + std::string{HELP_CHAR} + " in the app to display keybindings\n"
};

fs::path expandHomeInPath(const std::string &str) {
    std::string homeDir = getenv("HOME");
    if (str.substr(0, 5) == "$HOME") {
        return fs::canonical(homeDir + str.substr(5));
    } else {
        return fs::canonical(str);
    }
}

bool parseConfig(fs::path &path, bool &relativePath, bool &sortNameAsc, bool &showPreview, std::string &wallpaperFill, std::string &wallpaperCenter, bool &pathSet, bool &wallpaperSet, std::string configPath = CONFIG_PATH) {
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
                path = expandHomeInPath(word);
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
            relativePath = (word == "true");
            continue;
        }
        else if (word == sortAscConfLine) {
            iss >> word >> word;
            sortNameAsc = (word == "true");
            continue;
        }
        else if (word == previewConfLine) {
            iss >> word >> word;
            showPreview = (word == "true");
            continue;
        }
        else if (word == wallpaperFillConfLine) {
            std::getline(iss, word, '\"');
            std::getline(iss, wallpaperFill, '\"');
            wallpaperFillSet = true;
            continue;
        }
        else if (word == wallpaperCenterConfLine) {
            std::getline(iss, word, '\"');
            std::getline(iss, wallpaperCenter, '\"');
            wallpaperCenterSet = true;
            continue;
        }
    }
    wallpaperSet = wallpaperCenterSet * wallpaperFillSet;
    return true;
}

int main (int argc, char** argv) {
    setlocale(LC_ALL, "");

    fs::path path;
    bool relativePath = true;
    bool sortNameAsc = true;
    bool showPreview = true;
    std::string wallpaperFill;
    std::string wallpaperCenter;

    bool pathSet = false;
    bool wallpaperSet = false;

    if (!parseConfig(path, relativePath, sortNameAsc, showPreview, wallpaperFill, wallpaperCenter, pathSet, wallpaperSet))
        std::cerr << "Error in conf file\n";

    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-' && strlen(argv[i]) > 1) {
                for (int j = 1; j < strlen(argv[i]); j++) {
                    if (argv[i][j] == CONFIG_FLAG) {
                        if (argc <= i+1) {
                            std::cerr << "No config file provided\n";
                            return 3;
                        }
                        if (!parseConfig(path, relativePath, sortNameAsc, showPreview, wallpaperFill, wallpaperCenter, pathSet, wallpaperSet, argv[++i]))
                            std::cerr << "Error in conf file\n";
                        break;
                    }
                    else if (argv[i][j] == HELP_FLAG) {
                        std::cout << HELP_MSG;
                    }
                }
            }
            else {
                try {
                    path = fs::canonical(argv[i]);
                }
                catch (const std::exception &ex) {
                    std::cerr << "Provided path not valid: " << ex.what() << "\n";
                    return 1;
                }

                if (!fs::exists(path)) {
                    std::cerr << "Provided path not valid.\n";
                    return 2;
                }
                pathSet = true;
            }
        }
    }

    if (!pathSet)
        path = fs::current_path();

    if (!wallpaperSet) {
        std::cerr << "No wallpaper setting backend provided.\n";
        return 4;
    }

    Form form = Form(path.string(), wallpaperFill, wallpaperCenter, showPreview, sortNameAsc, relativePath);
    form.loopOptions();

    return 0;
}
