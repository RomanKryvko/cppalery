#include "form.h"

void Form::initColors() {
    start_color();
    use_default_colors();
    for (auto color : config.colors) {
        init_pair(color.colorPair, color.foreground, color.background);
    }
}

Form::Form(Config config) {
    initscr();
    initColors();
    noecho();
    cbreak();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(0);
    getmaxyx(stdscr, maxRows, maxCols);
    this->config = config;
    directory = Directory(config.getPath(), config.isPathRelative);
    mainWin = MainWindow(maxRows - BOTTOM_OFFSET, maxCols - 1, &directory);
    commandWin = CommandWindow(1, maxCols - 1, maxRows - BOTTOM_OFFSET + 1, 1);
    previewWin = PreviewWindow(maxRows - BOTTOM_OFFSET - 2, maxCols / 2 - 1, 2, maxCols / 2);
    backSetter = BackgroundSetter(config.wallpaperFillCommand, config.wallpaperCenterCommand);
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

void Form::run() {
    printInitialSetup();

    InputProcessingResultCode returnCode = InputProcessingResultCode::CONTINUE;
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

        returnCode = processUserInput(ch);

        if (returnCode == InputProcessingResultCode::EXIT) {
            break;
        }
    }
}

void Form::printWindows() {
    mainWin.printDirectoryContents();
    commandWin.printStatus(directory.getSelection() + 1, directory.size());
    if (config.showPreview && directory.isSelectionAnImage()) {
        previewWin.resetSetup();
    }
}

void Form::printInitialSetup() {
    directory.sortContentsByName(config.sortByNameAscending);
    if (!config.isPathRelative) {
        directory.toggleRelativePath();
    }
    mainWin.printDirectoryContents();
    commandWin.printStatus(directory.getSelection() + 1, directory.size());
    if (config.showPreview)
        renderImgPreview();
}

Form::InputProcessingResultCode Form::processUserInput(int input) {
    if (input == QUIT_CHAR)
        return InputProcessingResultCode::EXIT;

    switch (input) {
        case KEY_MOVE_TOP: {
            input = getch();
            if (input == KEY_MOVE_TOP && !directory.empty()) {
                mainWin.jumpToEntry(0);
            }
            break;
        }

        case KEY_FOCUS: {
            input = getch();
            if (input == KEY_FOCUS && !directory.empty()) {
                mainWin.focusScrolling();
            }
            break;
        }

        case KEY_MOVE_BOTTOM: {
            if (!directory.empty()) {
                mainWin.jumpToEntry(directory.size() - 1);
            }
            break;
        }

        case KEY_RESIZE: {
            resize();
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

        case KEY_ESC: {
            directory.clearSearchResults();
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
    }

    printWindows();

    return InputProcessingResultCode::CONTINUE;
}
