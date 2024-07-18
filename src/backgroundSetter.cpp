#include "backgroundSetter.h"

BackgroundSetter::BackgroundSetter() {
    this->mode = BackgroundSetter::Mode::FILL;
}

BackgroundSetter::BackgroundSetter(const std::string &wallpaperCenter, const std::string &wallpaperFill, BackgroundSetter::Mode mode) {
    this->wallpaperFill = wallpaperFill;
    this->wallpaperCenter = wallpaperCenter;
    this->mode = mode;
}

void BackgroundSetter::setBackground(const std::string &fileName, BackgroundSetter::Mode mode) {
    this->mode = mode;
    std::string command;
    if (mode == Mode::FILL) {
        command = wallpaperFill + '\"' + fileName + "\" > /dev/null";
    }
    else {
        command = wallpaperCenter + '\"' + fileName + "\" > /dev/null";
    }

    system(command.c_str());
}
