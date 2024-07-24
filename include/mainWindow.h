#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "directory.h"
#include "window.h"
#include "colors.h"
#include <algorithm>
#include <cstring>

class MainWindow : public Window {
    private:
        // Defines on which line from the egde scrolling starts
        constexpr static int SCROLL_OFFSET = 2;
        int ceiling;
        void printColoredString(const char* str, int y, int x, ColorPair pair);

    public:
        bool showPreview;

        Directory* directory;

        MainWindow();

        MainWindow(int height, int width, Directory* directory);

        MainWindow(const MainWindow& other);

        MainWindow& operator=(const MainWindow& other);

        void scrollUp();

        void scrollDown();

        void focusScrolling();

        void resetSetup() override;

        void jumpToEntry(int idx);

        void printDirectoryContents();

        std::string chooseNextFoundEntry(bool orderAsc);
};

#endif
