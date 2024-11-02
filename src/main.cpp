#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include "form.h"
#include "version.h"
#include "keyGlobals.h"
#include "config.h"
#include <memory>

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

int main (int argc, char** argv) {
    setlocale(LC_ALL, "");

    std::shared_ptr<Config> config = std::make_shared<Config>();

    if (!config->parseConfig())
        std::cerr << "Error in config file\n";

    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-' && strlen(argv[i]) > 1) {
                for (int j = 1; j < strlen(argv[i]); j++) {
                    if (argv[i][j] == CONFIG_FLAG) {
                        if (argc <= i+1) {
                            std::cerr << "No config file provided\n";
                            return 3;
                        }
                        if (!config->parseConfig(argv[++i]))
                            std::cerr << "Error in config file\n";
                        break;
                    }
                    else if (argv[i][j] == HELP_FLAG) {
                        std::cout << HELP_MSG;
                        return 1;
                    }
                }
            }
            else {
                try {
                    config->setPath(argv[i]);
                }
                catch (const std::exception &ex) {
                    std::cerr << ex.what() << "\n";
                    return 2;
                }
            }
        }
    }

    if (!config->isPathSet())
        config->setPath(fs::current_path());

    if (!config->areWallpaperCommandsSet()) {
        std::cerr << "No wallpaper setting backend provided.\n";
        return 4;
    }

    Form form = Form(config);
    form.run();

    return 0;
}
