#ifndef COMMAND_WINDOW_H
#define COMMAND_WINDOW_H

#include "directory.h"
#include "window.h"
#include "keyGlobals.h"

class CommandWindow : public Window {
    public:
        CommandWindow();
        CommandWindow(int height, int width, int startX, int startY);

        std::string info;

        // A workaround for a constructor
        void initialize(int height, int width, int startX, int startY);

        void resetSetup() override;

        void move(int newHeigth, int newWidth, int startX, int startY);

        void printStatus(int position, int total);

        std::string getSearchInput();
};

#endif