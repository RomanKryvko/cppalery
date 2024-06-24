#include "commandWindow.h"

CommandWindow::CommandWindow() {}
CommandWindow::CommandWindow(int height, int width, int startX, int startY) {
    this->height = height;
    this->width = width;
    this->window = newwin(this->height, this->width, startX, startY);
}

// A workaround for a constructor
void CommandWindow::initialize(int height, int width, int startX, int startY) {
    this->height = height;
    this->width = width;
    this->window = newwin(this->height, this->width, startX, startY);
}

void CommandWindow::resetSetup() {
    werase(this->window);
}

void CommandWindow::move(int newHeigth, int newWidth, int startX, int startY) {
    // for some reason I coudn't get the mvwin to work
    delwin(this->window);
    this->window = newwin(newHeigth, newWidth, startX, startY);
}

void CommandWindow::printText(std::string str) {
    resetSetup();
    mvwprintw(this->window, 0, 1, str.c_str());
    wrefresh(this->window);
}

std::string CommandWindow::getSearchInput() {
    resetSetup();
    mvwprintw(this->window, 0, 1, "/");
    wrefresh(this->window);
    std::string str = "";
    int ch = getch();
    mvwprintw(this->window, 0, 2, str.c_str());
    wrefresh(this->window);
    while (ch != KEY_ENTER_ALT && ch != KEY_ENTER) {
        if (ch == KEY_DC || ch == '\b' || ch == KEY_BACKSPACE || ch == KEY_BACKSPACE_ALT) {
            if (str.empty()) {
                break;
            }
            else {
                str.pop_back();
            }
        }
        else if (ch == KEY_ESC) {
            break;
        }
        else {
            str.push_back(ch);
        }
        resetSetup();
        mvwprintw(this->window, 0, 1, "/");
        mvwprintw(this->window, 0, 2, str.c_str());
        wrefresh(this->window);
        ch = getch();
    }
    return str;
}