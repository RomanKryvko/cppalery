#ifndef WINDOW_H
#define WINDOW_H
#include "cppalery.h"

class Window {
    public:
        WINDOW *window;
        int height;
        int width;

        Window();
        ~Window();

        virtual void resetSetup() = 0;

        virtual void resize(int newHeight, int newWidth);
};

#endif