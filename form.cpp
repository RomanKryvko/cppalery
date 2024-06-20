#include "cppalery.h"
#include "directory.h"
#include "mainWindow.cpp"
#include "commandWindow.cpp"
#include "backgroundSetter.cpp"
#include "previewWindow.cpp"
#include <chrono>

namespace ch = std::chrono;

class Form {
private:
    MainWindow mainWin;
    CommandWindow commandWin;
    PreviewWindow previewWin;
    int maxRows = 10;
    int maxCols = 150;
    fs::path workPath;

public:
    Form(const std::string &workPath) {
        initscr();
        start_color();
        init_pair(TermColors::DirColor, COLOR_WHITE, COLOR_BLUE);
        init_pair(TermColors::SelectedColor, COLOR_BLACK, COLOR_WHITE);
        use_default_colors();
        noecho();
        cbreak();
        intrflush(stdscr, FALSE);
        keypad(stdscr, TRUE);
        curs_set(0);
        getmaxyx(stdscr, maxRows, maxCols);
        this->workPath = workPath;
        this->mainWin.initialize(workPath, maxRows - BOTTOM_OFFSET, maxCols - 1);
        this->commandWin.initialize(1, maxCols - 1, maxRows - BOTTOM_OFFSET + 1, 1);
        this->previewWin.initialize(maxRows - BOTTOM_OFFSET - 2, maxCols / 2 - 1, 2, maxCols / 2);
        refresh();
    }

    ~Form() {
        endwin();
    }

    void resize() {
        erase();
        getmaxyx(stdscr, maxRows, maxCols);
        mainWin.resize(maxRows - BOTTOM_OFFSET, maxCols - 1); 
        commandWin.move(1, maxCols, maxRows - BOTTOM_OFFSET + 1, 1);
        previewWin.move(maxRows - BOTTOM_OFFSET - 2, maxCols / 2 - 1, 2, maxCols / 2);
        mainWin.focusScrolling();
        refresh();
    }

    void renderImgPreview() {
        if (mainWin.isSelectionAnImage()) {
            this->previewWin.resetSetup();
            previewWin.renderImg(mainWin.getCurrentFilePath(), maxRows - BOTTOM_OFFSET - 4, maxCols / 2 - 4, maxCols / 2 + 1, 3);
        }
        else if (previewWin.isUeberzugRunning) {
            previewWin.terminateImgPreview();
        }
    }

    void loopOptions() {
        mainWin.printDirectoryContents();
        renderImgPreview();

        while (true) {

            auto start = ch::duration_cast<ch::milliseconds>(ch::system_clock::now().time_since_epoch()).count();

            int ch = getch();

            auto end = ch::duration_cast<ch::milliseconds>(ch::system_clock::now().time_since_epoch()).count();

            if (ch == QUIT_CHAR)
                break;

            switch (ch) {
                case KEY_RESIZE: {
                    this->resize();
                    break;
                }

                case KEY_UP: {
                    mainWin.scrollUp();
                    break;
                }

                case KEY_DOWN: {
                    mainWin.scrollDown();
                    break;
                }

                case KEY_LEFT: {
                    if (mainWin.goUpDirectory()) {
                        this->workPath = mainWin.getDirectory().workPath;
                    }
                    break;
                }

                case KEY_ENTER_ALT:
                case KEY_ENTER:
                case KEY_RIGHT: {
                    if (mainWin.goIntoDirectory()) {
                        this->workPath = mainWin.getDirectory().workPath;
                    }
                    else if (mainWin.isSelectionAnImage()) {
                        BackgroundSetter back = BackgroundSetter();
                        back.setBackground(mainWin.getCurrentFilePath());
                    }
                    break;
                }

                case CENTER_CHAR: {
                    if (mainWin.isSelectionAnImage()) {
                        BackgroundSetter back = BackgroundSetter(BackgroundSetter::Mode::CENTER);
                        back.setBackground(mainWin.getCurrentFilePath());
                    }
                    break;
                }

                case FILL_CHAR: {
                    if (mainWin.isSelectionAnImage()) {
                        BackgroundSetter back = BackgroundSetter();
                        back.setBackground(mainWin.getCurrentFilePath());
                    }
                    break;
                }

                case HIDE_CHAR: {
                    mainWin.toggleDots();
                    break;
                }

                case ASC_CHAR: {
                    mainWin.sortContentsByName();
                    break;
                }

                case DESC_CHAR: {
                    mainWin.sortContentsByName(false);
                    break;
                }

                case SEARCH_CHAR: {
                    std::string searchString = commandWin.getSearchInput();
                    std::string searchResult = mainWin.findEntryInDirectory(searchString);
                    commandWin.printText(searchResult);
                    break;
                }

                case LOOP_RESULTS_CHAR: {
                    mainWin.chooseNextFoundEntry();
                    break;
                }

                default:
                    break;
            }

            mainWin.printDirectoryContents();
            if (end - start > 200) {
                renderImgPreview();
            }
            refresh();
        }
    }
};