#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "directory.h"
#include "window.h"
#include <cstring>

enum TermColors
{
    DefaultColor = 0,
    DirColor = 1,
    SelectedColor = 2,
    ImageColor = 3,
    SelectedDirColor = 4,
    EmptyColor = 5
};

// Defines on which line from the egde scrolling starts
const int SCROLL_OFFSET = 2;

class MainWindow : public Window {
    private:
        Directory directory;
        int ceiling;

    public:
        MainWindow();

        MainWindow(const std::string &workPath, int height, int width);

        // A workaround for a constructor
        void initialize(const std::string &workPath, int height, int width);

        Directory getDirectory();

        int getDirSize();

        void scrollUp();

        void scrollDown();

        void focusScrolling();

        void resetSetup() override;

        bool goUpDirectory();

        bool goIntoDirectory();

        void jumpToEntry(int idx);

        bool isSelectionAnImage();

        std::string getCurrentFilePath();

        void printDirectoryContents();

        void toggleDots();

        void sortContentsByName(bool ascending = true);

        void chooseNextFoundEntry();

        std::string findEntryInDirectory(const std::string &str);

};

#endif