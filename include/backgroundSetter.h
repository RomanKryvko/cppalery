#ifndef BACKGROUND_SETTER_H
#define BACKGROUND_SETTER_H

#include <string>
#include <cstdlib>

class BackgroundSetter {
    private:
        const std::string fehCenter = "feh --bg-center ";
        const std::string fehFill = "feh --bg-fill ";

    public:
        enum Mode {
            FILL,
            CENTER
        };

        Mode mode;

        BackgroundSetter();

        BackgroundSetter(BackgroundSetter::Mode mode);

        void setBackground(const std::string &fileName);

};

#endif