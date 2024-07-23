#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
namespace fs = std::filesystem;

class Config {
    private:
        bool pathSet;
        bool wallpaperCommandsSet;
        fs::path workpath;

        inline static const std::string CONFIG_PATH = std::string(getenv("HOME")).append("/.config/cppalery/config");
        const std::string workPathConfLine = "workpath";
        const std::string relativePathConfLine = "relative_path";
        const std::string sortAscConfLine = "sort_name_ascending";
        const std::string previewConfLine = "preview";
        const std::string wallpaperFillConfLine = "wallpaper_set_fill";
        const std::string wallpaperCenterConfLine = "wallpaper_set_center";

        fs::path expandHomeInPath(const std::string &str);

    public:
        bool isPathRelative;
        bool showPreview;
        bool sortByNameAscending;
        std::string wallpaperFillCommand;
        std::string wallpaperCenterCommand;

        Config(bool isPathRelative = true, bool showPreview = true, bool sortByNameAscending = true);

        Config(Config& other);

        Config& operator=(Config& other);

        fs::path getPath();

        void setPath(std::string path);

        bool isPathSet();

        bool areWallpaperCommandsSet();

        bool parseConfig(std::string configPath = CONFIG_PATH);
};

#endif
