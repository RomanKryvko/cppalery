#include "window.h"

Window::Window() {}

Window::~Window() {
    if (window != nullptr) {
        delwin(window);
    }
}

void Window::resize(int height, int width) {
    this->height = height;
    this->width = width;
    resetSetup();
}