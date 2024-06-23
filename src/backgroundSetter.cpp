#include "backgroundSetter.h"

BackgroundSetter::BackgroundSetter() {
    this->mode = BackgroundSetter::Mode::FILL;
}

BackgroundSetter::BackgroundSetter(BackgroundSetter::Mode mode){
    this->mode = mode;
}

void BackgroundSetter::setBackground(const std::string &fileName) {
    std::string command;
    if (mode == Mode::FILL) {
        command = fehFill + fileName;
    }
    else {
        command = fehCenter + fileName;
    }

    system(command.c_str());
}