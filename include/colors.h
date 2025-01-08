#pragma once

#include <ncurses.h>

enum class ColorPair {
    // in ncurses 0 is reserved for default terminal colors
    Default = 0,
    Directory,
    Selected,
    Image,
    SelectedDirectory,
    Empty,
    SearchHighlight
};

enum class NCursesColor {
        TRANSPARENT = -1,
        WHITE = COLOR_WHITE,
        BLACK = COLOR_BLACK,
        YELLOW = COLOR_YELLOW,
        GREEN = COLOR_GREEN,
        BLUE = COLOR_BLUE,
        RED = COLOR_RED,
        MAGENTA = COLOR_MAGENTA,
        CYAN = COLOR_CYAN
};

class TermColor {
    public:
        ColorPair colorPair;
        NCursesColor  background;
        NCursesColor foreground;

        TermColor(ColorPair colorPair, NCursesColor background, NCursesColor foreground) {
            this->colorPair = colorPair;
            this->background = background;
            this->foreground = foreground;
        }
};
