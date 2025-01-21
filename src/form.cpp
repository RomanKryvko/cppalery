#include "form.h"
#include "keybindings.h"

Form::Form(const std::shared_ptr<Config> &config) : config(config) {
    initscr();
    initColors();
    noecho();
    cbreak();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(0);
    getmaxyx(stdscr, maxRows, maxCols);
    pager = std::make_shared<DirectoryPager>(maxRows - BOTTOM_OFFSET - 2, 0);
    mainWin = MainWindow(maxRows - BOTTOM_OFFSET, maxCols - 1);
    commandWin = std::make_shared<CommandWindow>(1, maxCols - 1, maxRows - BOTTOM_OFFSET + 1, 1);
    previewWin = PreviewWindow(maxRows - BOTTOM_OFFSET - 2, maxCols / 2 - 1, 2, maxCols / 2);
    directoryController = std::make_shared<DirectoryController>(config->getPath(), true, config->sortByNameAscending, config->isPathRelative, pager, commandWin);
    backSetter = std::make_shared<BackgroundSetter>(config->wallpaperFillCommand, config->wallpaperCenterCommand, pager, directoryController, commandWin);
    directoryController->setBackgroundSetter(backSetter);
    refresh();
}

void Form::initColors() {
    start_color();
    use_default_colors();
    for (const auto &color : config->colors) {
        init_pair((short) color.colorPair, (short) color.foreground, (short) color.background);
    }
}

Form::~Form() {
    endwin();
}

void Form::resize() {
    erase();
    getmaxyx(stdscr, maxRows, maxCols);
    mainWin.resize(maxRows - BOTTOM_OFFSET, maxCols - 1);
    commandWin->move(1, maxCols, maxRows - BOTTOM_OFFSET + 1, 1);
    previewWin.move(maxRows - BOTTOM_OFFSET - 2, maxCols / 2 - 1, 2, maxCols / 2);
    pager->setHeight(maxRows - BOTTOM_OFFSET - 2);
    pager->focusScrolling();
    refresh();
}

void Form::renderImgPreview() {
    if (directoryController->isAnImage(pager->getSelection())) {
        previewWin.resetSetup();
        previewWin.renderImg(directoryController->getPathAt(pager->getSelection()).string(),
                              maxRows - BOTTOM_OFFSET - 4, maxCols / 2 - 4, maxCols / 2 + 1, 3);
    }
    else if (previewWin.isUeberzugRunning) {
        previewWin.terminateImgPreview();
    }
}

void Form::run() {
    printInitialSetup();

    short ch = -1;

    while (ch != QUIT_CHAR) {
        timeout(IMG_DELAY);
        ch = getch();
        // Render image only if user spends more than IMG_DELAY on an entry
        if (ch == ERR) {
            timeout(-1);
            if (config->showPreview)
                renderImgPreview();
            ch = getch();
        }

        keybindings.handleKeyPress(ch);

        printWindows();
    }
}

void Form::printWindows() {
    mainWin.printDirectoryContents(pager, directoryController);
    commandWin->printStatus(pager->getSelection() + 1, directoryController->getNumberOfEntries());
    if (config->showPreview && directoryController->isAnImage(pager->getSelection())) {
        previewWin.resetSetup();
    }
}

void Form::printInitialSetup() {
    //TODO: rewrite this
    if (config->sortByNameAscending)
        directoryController->sortByAscending();
    else
        directoryController->sortByDescending();

    if (!config->isPathRelative) {
        directoryController->toggleRelativePath();
    }
    mainWin.printDirectoryContents(pager, directoryController);
    commandWin->printStatus(pager->getSelection() + 1, directoryController->getNumberOfEntries());
}

void Form::goIntoDirOrSetBackground() {
    //TODO: rewrite this
    directoryController->goIntoDirectory(pager->getSelection());
}

void Form::initiateSearch() {
    std::string searchString = commandWin->getSearchInput();
    int matches = directoryController->findAllEntriesInDirectory(searchString);
    std::string searchResult;
    if (matches == 0) {
        searchResult = "Pattern not found: " + searchString;
    }
    else {
        searchResult = searchString + ": " + std::to_string(matches) + " match";
        if (matches > 1)
            searchResult += "es";
        directoryController->chooseNextFoundEntry();
    }

    commandWin->setMessage(searchResult);
}

void Form::printHelp() {
    if (previewWin.isUeberzugRunning) {
        previewWin.terminateImgPreview();
    }
    commandWin->move(maxRows - BOTTOM_OFFSET, maxCols - 3, BOTTOM_OFFSET, 2);
    commandWin->printHelp();
    getch();
    //reset the form entirely to prevent artifacts from appearing
    resize();
}
