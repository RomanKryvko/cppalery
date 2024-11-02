#include "form.h"
#include "commandWindow.h"
#include "directory.h"
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
    directory = std::make_shared<Directory>(config->getPath(), config->isPathRelative);
    mainWin = std::make_shared<MainWindow>(maxRows - BOTTOM_OFFSET, maxCols - 1, directory);
    commandWin = std::make_shared<CommandWindow>(1, maxCols - 1, maxRows - BOTTOM_OFFSET + 1, 1);
    previewWin = std::make_shared<PreviewWindow>(maxRows - BOTTOM_OFFSET - 2, maxCols / 2 - 1, 2, maxCols / 2);
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
    mainWin->resize(maxRows - BOTTOM_OFFSET, maxCols - 1); 
    commandWin->move(1, maxCols, maxRows - BOTTOM_OFFSET + 1, 1);
    previewWin->move(maxRows - BOTTOM_OFFSET - 2, maxCols / 2 - 1, 2, maxCols / 2);
    mainWin->focusScrolling();
    refresh();
}

void Form::renderImgPreview() {
    if (directory->isSelectionAnImage()) {
        previewWin->resetSetup();
        previewWin->renderImg(directory->getSelectedFilePathString(), maxRows - BOTTOM_OFFSET - 4, maxCols / 2 - 4, maxCols / 2 + 1, 3);
    }
    else if (previewWin->isUeberzugRunning) {
        previewWin->terminateImgPreview();
    }
}

void Form::setBackground(BackgroundSetter::Mode mode) {
    if (directory->isSelectionAnImage()) {
        backSetter.setBackground(directory->getSelectedFilePathString(), mode);
    }
}

void Form::setBackground(fs::path imagePath, BackgroundSetter::Mode mode) {
    if (directory->isSelectionAnImage()) {
        backSetter.setBackground(imagePath.string(), mode);
    }
}

void Form::run() {
    printInitialSetup();

    int ch;

    while (true) {
        commandWin->info = "";
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
    mainWin->printDirectoryContents();
    commandWin->printStatus(directory->getSelection() + 1, directory->size());
    if (config->showPreview && directory->isSelectionAnImage()) {
        previewWin->resetSetup();
    }
}

void Form::printInitialSetup() {
    directory->sortContentsByName(config->sortByNameAscending);
    if (!config->isPathRelative) {
        directory->toggleRelativePath();
    }
    mainWin->printDirectoryContents();
    commandWin->printStatus(directory->getSelection() + 1, directory->size());
    if (config->showPreview)
        renderImgPreview();
}
