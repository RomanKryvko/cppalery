#include "form.h"
#include "commandWindow.h"
#include "directoryController.h"
#include "previewWindow.h"
#include <memory>

Form::Form(const std::shared_ptr<Config> &config) : config(config) {
    initscr();
    initColors();
    noecho();
    cbreak();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(0);
    getmaxyx(stdscr, maxRows, maxCols);
    directoryController = std::make_shared<DirectoryController>(config->getPath(), true, config->sortByNameAscending, config->isPathRelative);
    mainWin = MainWindow(maxRows - BOTTOM_OFFSET, maxCols - 1);
    pager = Pager(maxRows - BOTTOM_OFFSET - 2, directoryController->getNumberOfEntries());
    commandWin = CommandWindow(1, maxCols - 1, maxRows - BOTTOM_OFFSET + 1, 1);
    previewWin = PreviewWindow(maxRows - BOTTOM_OFFSET - 2, maxCols / 2 - 1, 2, maxCols / 2);
    backSetter = BackgroundSetter(config->wallpaperFillCommand, config->wallpaperCenterCommand);
    refresh();
}

void Form::initColors() {
    start_color();
    use_default_colors();
    for (const auto &color : config->colors) {
        init_pair(color.colorPair, color.foreground, color.background);
    }
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
    pager.setHeight(maxRows - BOTTOM_OFFSET - 2);
    pager.focusScrolling();
    refresh();
}

void Form::renderImgPreview() {
    if (directoryController->isAnImage(pager.getSelection())) {
        previewWin.resetSetup();
        previewWin.renderImg(directoryController->getPathAt(pager.getSelection()).string(),
                              maxRows - BOTTOM_OFFSET - 4, maxCols / 2 - 4, maxCols / 2 + 1, 3);
    }
    else if (previewWin.isUeberzugRunning) {
        previewWin.terminateImgPreview();
    }
}

void Form::setBackground(BackgroundSetter::Mode mode) {
    if (directoryController->isAnImage(pager.getSelection())) {
        backSetter.setBackground(directoryController->getPathAt(pager.getSelection()).string(), mode);
    }
}

void Form::setBackground(fs::path imagePath, BackgroundSetter::Mode mode) {
    if (directoryController->isAnImage(pager.getSelection())) {
        backSetter.setBackground(imagePath.string(), mode);
    }
}

void Form::run() {
    printInitialSetup();

    int ch;

    while (true) {
        commandWin.info = "";
        timeout(IMG_DELAY);
        ch = getch();
        // Render image only if user spends more than IMG_DELAY on an entry
        if (ch == ERR) {
            timeout(-1);
            if (config->showPreview)
                renderImgPreview();
            ch = getch();
        }

        if (ch == QUIT_CHAR) {
            break;
        }

        auto it = formEvents.find(ch);
        if (it != formEvents.end()) {
            it->second();
        }

        printWindows();
    }
}

void Form::printWindows() {
    mainWin.printDirectoryContents(pager, directoryController);
    commandWin.printStatus(pager.getSelection() + 1, directoryController->getNumberOfEntries());
    if (config->showPreview && directoryController->isAnImage(pager.getSelection())) {
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
    commandWin.printStatus(pager.getSelection() + 1, directoryController->getNumberOfEntries());
    if (config->showPreview)
        renderImgPreview();
}

void Form::jumpToTop() {
    int input = getch();
    if (input == KEY_MOVE_TOP) {
        pager.jumpToTop();
    }
}

void Form::jumpToBottom() {
    pager.jumpToBottom();
}

void Form::focus() {
    int input = getch();
    if (input == KEY_FOCUS && directoryController->getNumberOfEntries()) {
        pager.focusScrolling();
    }
}

void Form::scrollUp() {
    pager.scrollUp();
}

void Form::scrollDown() {
    pager.scrollDown();
}

void Form::goUpDir() {
    directoryController->goUpDirectory(pager.getSelection());
    pager.setNumberOfElements(directoryController->getNumberOfEntries());
    pager.focusScrolling();
}

void Form::goIntoDirOrSetBackground() {
    if (directoryController->goIntoDirectory(pager.getSelection())) {
        pager.setNumberOfElements(directoryController->getNumberOfEntries());
        pager.focusScrolling();
    }
    else {
        setBackground(BackgroundSetter::Mode::FILL);
    }
}

void Form::setBackgroundCenter() {
    setBackground(BackgroundSetter::Mode::CENTER);
}

void Form::setBackgroundFill() {
    setBackground(BackgroundSetter::Mode::FILL);
}

void Form::toggleDotfiles() {
    directoryController->toggleDots();
    pager.setNumberOfElements(directoryController->getNumberOfEntries());
    pager.focusScrolling();
}

void Form::toggleRelativePath() {
    directoryController->toggleRelativePath();
}

void Form::sortByNameAscending() {
    directoryController->sortByAscending();
}

void Form::sortByNameDescending() {
    directoryController->sortByDescending();
}

void Form::initiateSearch() {
    std::string searchString = commandWin.getSearchInput();
    int matches = directoryController->findAllEntriesInDirectory(searchString);
    std::string searchResult;
    if (matches == 0) {
        searchResult = "Pattern not found: " + searchString;
    }
    else {
        searchResult = searchString + ": " + std::to_string(matches) + " match";
        if (matches > 1)
            searchResult += "es";
        pager.jumpToIdx(directoryController->getFoundEntries()[0]);
    }

    commandWin.info = searchResult;
}

void Form::printHelp() {
    if (previewWin.isUeberzugRunning) {
        previewWin.terminateImgPreview();
    }
    commandWin.move(maxRows - BOTTOM_OFFSET, maxCols - 3, BOTTOM_OFFSET, 2);
    commandWin.printHelp();
    getch();
    //reset the form entirely to prevent artifacts from appearing
    resize();
}

void Form::clearSearchHighlights() {
    directoryController->clearSearchResults();
}

void Form::loopResultsForward() {
    int foundEntry = directoryController->getNextFoundEntry();
    if (foundEntry >= 0) {
        commandWin.info = directoryController->getPathAt(foundEntry).filename().string();
        pager.jumpToIdx(foundEntry);
    }
}

void Form::loopResultsBackward() {
    int foundEntry = directoryController->getPreviousFoundEntry();
    if (foundEntry >= 0) {
        commandWin.info = directoryController->getPathAt(foundEntry).filename().string();
        pager.jumpToIdx(foundEntry);
    }
}

void Form::setRandomBackground() {
    std::vector<fs::path> images = directoryController->getAllImages();
    if(!images.empty()) {
        srand(time(0));
        int idx = rand() % images.size();
        setBackground(images[idx], BackgroundSetter::Mode::FILL);
    }
    else {
        commandWin.info = NO_IMAGE_IN_DIR_MSG;
    }
}
