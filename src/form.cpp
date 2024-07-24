#include "form.h"
#include "directory.h"
#include "keyGlobals.h"
#include <ncurses.h>
#include <string>

Form::Form(Config config) {
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
    this->config = config;
    this->directory = Directory(config.getPath());
    this->mainWin = MainWindow(maxRows - BOTTOM_OFFSET, maxCols - 1, &directory);
    this->commandWin = CommandWindow(1, maxCols - 1, maxRows - BOTTOM_OFFSET + 1, 1);
    this->previewWin = PreviewWindow(maxRows - BOTTOM_OFFSET - 2, maxCols / 2 - 1, 2, maxCols / 2);
    this->backSetter = BackgroundSetter(config.wallpaperFillCommand, config.wallpaperCenterCommand);
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
    if (directory.isSelectionAnImage()) {
        previewWin.resetSetup();
        previewWin.renderImg(directory.getSelectedFilePathString(), maxRows - BOTTOM_OFFSET - 4, maxCols / 2 - 4, maxCols / 2 + 1, 3);
    }
    else if (previewWin.isUeberzugRunning) {
        previewWin.terminateImgPreview();
    }
}

void Form::setBackground(BackgroundSetter::Mode mode) {
    if (directory.isSelectionAnImage()) {
        backSetter.setBackground(directory.getSelectedFilePathString(), mode);
    }
}

void Form::setBackground(fs::path imagePath, BackgroundSetter::Mode mode) {
    if (directory.isSelectionAnImage()) {
        backSetter.setBackground(imagePath.string(), mode);
    }
}

void Form::loopOptions() {
    directory.sortContentsByName(config.sortByNameAscending);
    if (!config.isPathRelative) {
        directory.toggleRelativePath();
    }
    mainWin.printDirectoryContents();
    commandWin.printStatus(directory.getSelection() + 1, directory.size());
    if (config.showPreview)
        renderImgPreview();
    int ch;

    while (true) {
        commandWin.info = "";
        timeout(IMG_DELAY);
        ch = getch();
        // Render image only if user spends more than IMG_DELAY on an entry
        if (ch == ERR) {
            timeout(-1);
            if (config.showPreview)
                renderImgPreview();
            ch = getch();
        }

        if (ch == QUIT_CHAR)
            break;

        switch (ch) {
            case KEY_MOVE_TOP: {
                ch = getch();
                if (ch == KEY_MOVE_TOP && directory.size() > 0) {
                    mainWin.jumpToEntry(0);
                }
                break;
            }

            case KEY_MOVE_BOTTOM: {
                int dirSize = directory.size();
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
                if (directory.goUpDirectory()) {
                    config.setPath(directory.getPath());
                    mainWin.focusScrolling();
                }
                break;
            }

            case KEY_RIGHT_ALT:
            case KEY_ENTER_ALT:
            case KEY_ENTER:
            case KEY_RIGHT: {
                if (directory.goIntoDirectory()) {
                    config.setPath(directory.getPath());
                    mainWin.focusScrolling();
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
                directory.toggleDots();
                mainWin.focusScrolling();
                break;
            }

            case PATH_CHAR: {
                directory.toggleRelativePath();
                break;
            }

            case ASC_CHAR: {
                directory.sortContentsByName(true);
                break;
            }

            case DESC_CHAR: {
                directory.sortContentsByName(false);
                break;
            }

            case SEARCH_CHAR: {
                std::string searchString = commandWin.getSearchInput();
                int matches = directory.findAllEntriesInDirectory(searchString);
                std::string searchResult;
                if (matches == 0) {
                    searchResult = "Pattern not found: " + searchString;
                }
                else if (matches == 1) {
                    searchResult = searchString + ": 1 match";
                    mainWin.jumpToEntry(directory.foundEntries[0]);
                }
                else {
                    searchResult = searchString + ": " + std::to_string(matches) + " matches";
                    mainWin.jumpToEntry(directory.foundEntries[0]);
                }

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
                std::vector<fs::path> images = directory.getAllImages();
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
        commandWin.printStatus(directory.getSelection() + 1, directory.size());
        if (config.showPreview && directory.isSelectionAnImage()) {
            previewWin.resetSetup();
        }
        refresh();
    }
}
