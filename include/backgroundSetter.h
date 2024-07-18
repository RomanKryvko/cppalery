#ifndef BACKGROUND_SETTER_H
#define BACKGROUND_SETTER_H

#include <string>
#include <cstdlib>

class BackgroundSetter {
    private:
        std::string wallpaperCenter;
        std::string wallpaperFill;

    public:
        enum Mode {
            FILL,
            CENTER
        };

        Mode mode;

        BackgroundSetter();

        BackgroundSetter(const std::string &wallpaperCenter, const std::string &wallpaperFill, BackgroundSetter::Mode mode = BackgroundSetter::Mode::FILL);

        void setBackground(const std::string &fileName, BackgroundSetter::Mode mode = BackgroundSetter::Mode::FILL);

};

#endif
