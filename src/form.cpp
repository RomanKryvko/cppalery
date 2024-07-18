#include "form.h"
#include "keyGlobals.h"
#include <ncurses.h>

Form::Form(const std::string &workPath, const std::string &wallpaperFill, const std::string &wallpaperCenter, bool showPreview, bool sortNameAsc, bool relativePath) {
    initscr();
    start_color();
    use_default_colors();
    init_pair(TermColors::DirColor, COLOR_WHITE, COLOR_BLUE);
    init_pair(TermColors::SelectedColor, COLOR_BLACK, COLOR_WHITE);
    init_pair(TermColors::ImageColor, COLOR_YELLOW, -1);
    init_pair(TermColors::SelectedDirColor, COLOR_BLUE, COLOR_WHITE);
    init_pair(TermColors::EmptyColor, COLOR_WHITE, COLOR_RED);
    init_pair(TermColors::SearchHighlightColor, COLOR_WHITE, COLOR_YELLOW);
    noecho();
    cbreak();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(0);
    getmaxyx(stdscr, maxRows, maxCols);
    this->showPreview = showPreview;
    this->sortNameAsc = sortNameAsc;
    this->relativePath = relativePath;
    this->workPath = workPath;
    this->mainWin = MainWindow(workPath, maxRows - BOTTOM_OFFSET, maxCols - 1, showPreview);
    this->commandWin = CommandWindow(1, maxCols - 1, maxRows - BOTTOM_OFFSET + 1, 1);
    this->previewWin = PreviewWindow(maxRows - BOTTOM_OFFSET - 2, maxCols / 2 - 1, 2, maxCols / 2);
    this->backSetter = BackgroundSetter(wallpaperFill, wallpaperCenter);
    refresh();
}

Form::~Form() {
    endwin();
}

void Form::resize() {
    erase();
    getmaxyx(stdscr, maxRows, maxCols);
    mainWin.resize(maxRows - BOTTOM_OFFSET, maxCols - 1); 
    commandWin.move(1, maxCols, maxRows - BOTTOM_OFFSET + 1, 1);
    previewWin.move(maxRows - BOTTOM_OFFSET - 2, maxCols / 2 - 1, 2, maxCols / 2);
    mainWin.focusScrolling();
    refresh();
}

void Form::renderImgPreview() {
    if (mainWin.isSelectionAnImage()) {
        previewWin.resetSetup();
        previewWin.renderImg(mainWin.getCurrentFilePath(), maxRows - BOTTOM_OFFSET - 4, maxCols / 2 - 4, maxCols / 2 + 1, 3);
    }
    else if (previewWin.isUeberzugRunning) {
        previewWin.terminateImgPreview();
    }
}

void Form::setBackground(BackgroundSetter::Mode mode) {
    if (mainWin.isSelectionAnImage()) {
        backSetter.setBackground(mainWin.getCurrentFilePath(), mode);
    }
}

void Form::setBackground(fs::path imagePath, BackgroundSetter::Mode mode) {
    if (mainWin.isSelectionAnImage()) {
        backSetter.setBackground(imagePath.string(), mode);
    }
}

void Form::loopOptions() {
    mainWin.sortContentsByName(sortNameAsc);
    if (!relativePath) {
        mainWin.toggleRelativePath();
    }
    mainWin.printDirectoryContents();
    commandWin.printStatus(mainWin.getDirPosition() + 1, mainWin.getDirSize());
    if (showPreview)
        renderImgPreview();
    int ch;

    while (true) {
        commandWin.info = "";
        timeout(IMG_DELAY);
        ch = getch();
        // Render image only if user spends more than IMG_DELAY on an entry
        if (ch == ERR) {
            timeout(-1);
            if (showPreview)
                renderImgPreview();
            ch = getch();
        }

        if (ch == QUIT_CHAR)
            break;

        switch (ch) {
            case KEY_MOVE_TOP: {
                ch = getch();
                if (ch == KEY_MOVE_TOP && mainWin.getDirSize() > 0) {
                    mainWin.jumpToEntry(0);
                }
                break;
            }

            case KEY_MOVE_BOTTOM: {
                int dirSize = mainWin.getDirSize();
                if (dirSize > 0) {
                    mainWin.jumpToEntry(dirSize - 1);
                }
                break;
            }

            case KEY_RESIZE: {
                this->resize();
                break;
            }

            case KEY_UP_ALT:
            case KEY_UP: {
                mainWin.scrollUp();
                break;
            }

            case KEY_DOWN_ALT:
            case KEY_DOWN: {
                mainWin.scrollDown();
                break;
            }

            case KEY_LEFT_ALT:
            case KEY_LEFT: {
                if (mainWin.goUpDirectory()) {
                    this->workPath = mainWin.getDirectory().workPath;
                }
                break;
            }

            case KEY_RIGHT_ALT:
            case KEY_ENTER_ALT:
            case KEY_ENTER:
            case KEY_RIGHT: {
                if (mainWin.goIntoDirectory()) {
                    this->workPath = mainWin.getDirectory().workPath;
                }
                else {
                    setBackground(BackgroundSetter::Mode::FILL);
                }
                break;
            }

            case CENTER_CHAR: {
                setBackground(BackgroundSetter::Mode::CENTER);
                break;
            }

            case FILL_CHAR: {
                setBackground(BackgroundSetter::Mode::FILL);
                break;
            }

            case HIDE_CHAR: {
                mainWin.toggleDots();
                break;
            }

            case PATH_CHAR: {
                mainWin.toggleRelativePath();
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
                commandWin.info = searchResult;
                break;
            }

            case HELP_CHAR: {
                if (previewWin.isUeberzugRunning) {
                    previewWin.terminateImgPreview();
                }
                commandWin.move(maxRows - BOTTOM_OFFSET, maxCols - 3, BOTTOM_OFFSET, 2);
                commandWin.printHelp();
                getch();
                //reset the form entirely to prevent artifacts from appearing
                resize();
                break;
            }

            case LOOP_RESULTS_CHAR: {
                std::string searchResult = mainWin.chooseNextFoundEntry(true);
                commandWin.info = searchResult;
                break;
            }

            case LOOP_RESULTS_CHAR_BACK: {
                std::string searchResult = mainWin.chooseNextFoundEntry(false);
                commandWin.info = searchResult;
                break;
            }

            case RANDOM_CHAR: {
                std::vector<fs::path> images = mainWin.getAllImages();
                if(!images.empty()) {
                    srand(time(0));
                    int idx = rand() % images.size();
                    setBackground(images[idx], BackgroundSetter::Mode::FILL);
                }
                else {
                    commandWin.info = NO_IMAGE_IN_DIR_MSG;
                }
                break;
            }

            default:
                break;
        }

        mainWin.printDirectoryContents();
        commandWin.printStatus(mainWin.getDirPosition() + 1, mainWin.getDirSize());
        if (showPreview && mainWin.isSelectionAnImage()) {
            previewWin.resetSetup();
        }
        refresh();
    }
}
