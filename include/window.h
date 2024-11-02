#ifndef WINDOW_H
#define WINDOW_H
#include <ncurses.h>

class Window {
    protected:
        WINDOW *window;
        int height;
        int width;

    public:
        Window();

        Window(int height, int width);

        Window(const Window& other);

        virtual Window& operator=(const Window& other);

        ~Window();

        virtual void resetSetup() = 0;

        virtual void resize(int newHeight, int newWidth);
};

#endif
