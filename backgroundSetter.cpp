#include "cppalery.h"

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

        BackgroundSetter() {
            this->mode = BackgroundSetter::Mode::FILL;
        }

        BackgroundSetter(BackgroundSetter::Mode mode){
            this->mode = mode;
        }

        void setBackground(const std::string &fileName) {
            std::string command;
            if (mode == Mode::FILL) {
                command = fehFill + fileName;
            }
            else {
                command = fehCenter + fileName;
            }

            system(command.c_str());
        }
};