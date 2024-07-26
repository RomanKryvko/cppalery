#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include "colors.h"
#include <sstream>
namespace fs = std::filesystem;

class Config {
    private:
        const char* UEBERZUG_TEST_COMMAND = "ueberzug > /dev/null 2>&1";
        const int COMMAND_NOT_FOUND = 0x7F00;
        inline static const std::string CONFIG_PATH = std::string(getenv("HOME")).append("/.config/cppalery/config");

        const std::string workPathConfLine = "workpath";
        const std::string relativePathConfLine = "relative_path";
        const std::string sortAscConfLine = "sort_name_ascending";
        const std::string previewConfLine = "preview";
        const std::string wallpaperFillConfLine = "wallpaper_set_fill";
        const std::string wallpaperCenterConfLine = "wallpaper_set_center";

        bool pathSet;
        bool wallpaperCommandsSet;
        fs::path workpath;

        fs::path expandHomeInPath(const std::string &str);

    public:
        bool isPathRelative;
        bool showPreview;
        bool sortByNameAscending;
        std::string wallpaperFillCommand;
        std::string wallpaperCenterCommand;
        std::vector<TermColor> colors;

        Config(bool isPathRelative = true, bool showPreview = true, bool sortByNameAscending = true);

        Config(const Config& other);

        Config& operator=(const Config& other);

        fs::path getPath();

        void setPath(std::string path);

        bool isPathSet();

        bool areWallpaperCommandsSet();

        bool parseConfig(std::string configPath = CONFIG_PATH);
};

#endif
