#include "window.h"

Window::Window() : window(nullptr) {}

Window::Window(int height, int width) : height(height), width(width), window(nullptr) {}

Window::~Window() {
    if (window) {
        delwin(window);
    }
}

Window::Window(const Window& other) {
    height = other.height;
    width = other.width;
    if (other.window) {
        int starty, startx;
        getbegyx(other.window, starty, startx);
        window = newwin(height, width, starty, startx);
    } else {
        window = nullptr;
    }
}

Window& Window::operator=(const Window& other) {
    if (this == &other) {
        return *this;
    }

    if (window) {
        delwin(window);
    }

    height = other.height;
    width = other.width;
    if (other.window) {
        int starty, startx;
        getbegyx(other.window, starty, startx);
        window = newwin(height, width, starty, startx);
    } else {
        window = nullptr;
    }

    return *this;
}

void Window::resize(int height, int width) {
    this->height = height;
    this->width = width;
    wresize(window, height, width);
    resetSetup();
}
