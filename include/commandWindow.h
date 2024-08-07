#ifndef COMMAND_WINDOW_H
#define COMMAND_WINDOW_H

#include <string>
#include <iomanip>
#include "window.h"
#include "keyGlobals.h"
#include <vector>
#include <array>

const int STATUS_RULER_OFFSET = 5;

class CommandWindow : public Window {
    public:
        CommandWindow();
        CommandWindow(int height, int width, int startX, int startY);

        std::string info;

        void resetSetup() override;

        void move(int newHeigth, int newWidth, int startX, int startY);

        void resize(int newHeight, int newWidth) override;

        void printStatus(int position, int total);

        std::string getSearchInput();

        void printHelp();
};

#endif
