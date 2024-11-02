#ifndef FORM_H
#define FORM_H

#include "directory.h"
#include "keyGlobals.h"
#include "mainWindow.h"
#include "commandWindow.h"
#include "backgroundSetter.h"
#include "previewWindow.h"
#include "config.h"
#include <memory>

namespace fs = std::filesystem;

class Form {
private:
    const char* NO_IMAGE_IN_DIR_MSG = "No images of supported formats found in the directory->";
    const int BOTTOM_OFFSET = 4;
    const int IMG_DELAY = 200;
    std::unique_ptr<Window> focusedWindow;
    std::shared_ptr<MainWindow> mainWin;
    std::shared_ptr<CommandWindow> commandWin;
    std::shared_ptr<PreviewWindow> previewWin;
    std::shared_ptr<Directory> directory;
    std::shared_ptr<Config> config;
    BackgroundSetter backSetter;
    int maxRows;
    int maxCols;

    EventMap formEvents = {
        {KEY_MOVE_TOP, std::bind(&Form::jumpToTop, this)},
        {KEY_MOVE_BOTTOM, std::bind(&Form::jumpToBottom, this)},
        {KEY_FOCUS, std::bind(&Form::focus, this)},
        {KEY_RESIZE, std::bind(&Form::resize, this)},
        {KEY_UP, std::bind(&Form::scrollUp, this)},
        {KEY_UP_ALT, std::bind(&Form::scrollUp, this)},
        {KEY_DOWN, std::bind(&Form::scrollDown, this)},
        {KEY_DOWN_ALT, std::bind(&Form::scrollDown, this)},
        {KEY_LEFT, std::bind(&Form::goUpDir, this)},
        {KEY_LEFT_ALT, std::bind(&Form::goUpDir, this)},
        {KEY_RESIZE, std::bind(&Form::resize, this)},
        {KEY_RIGHT, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {KEY_RIGHT_ALT, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {KEY_ENTER, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {KEY_ENTER_ALT, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {CENTER_CHAR, std::bind(&Form::setBackgroundCenter, this)},
        {FILL_CHAR, std::bind(&Form::setBackgroundFill, this)},
        {HIDE_CHAR, std::bind(&Form::toggleDotfiles, this)},
        {PATH_CHAR, std::bind(&Form::toggleRelativePath, this)},
        {ASC_CHAR, std::bind(&Form::sortByNameAscending, this)},
        {DESC_CHAR, std::bind(&Form::sortByNameDescending, this)},
        {SEARCH_CHAR, std::bind(&Form::initiateSearch, this)},
        {HELP_CHAR, std::bind(&Form::printHelp, this)},
        {KEY_ESC, std::bind(&Form::clearSearchHighlights, this)},
        {LOOP_RESULTS_CHAR, std::bind(&Form::loopResultsForward, this)},
        {LOOP_RESULTS_CHAR_BACK, std::bind(&Form::loopResultsBackward, this)},
        {RANDOM_CHAR, std::bind(&Form::setRandomBackground, this)},
    };

    void jumpToTop() {
        int input = getch();
        if (input == KEY_MOVE_TOP && !directory->empty()) {
            mainWin->jumpToEntry(0);
        }
    }
    void jumpToBottom() {
        if (!directory->empty()) {
            mainWin->jumpToEntry(directory->size() - 1);
        }
    }
    void focus() {
        int input = getch();
        if (input == KEY_FOCUS && !directory->empty()) {
            mainWin->focusScrolling();
        }
    }
    void scrollUp() {
        mainWin->scrollUp();
    }
    void scrollDown() {
        mainWin->scrollDown();
    }
    void goUpDir() {
        if (directory->goUpDirectory()) {
            config->setPath(directory->getPath());
            mainWin->focusScrolling();
        }
    }
    void goIntoDirOrSetBackground() {
        if (directory->goIntoDirectory()) {
            config->setPath(directory->getPath());
            mainWin->focusScrolling();
        }
        else {
            setBackground(BackgroundSetter::Mode::FILL);
        }
    }
    void setBackgroundCenter() {
        setBackground(BackgroundSetter::Mode::CENTER);
    }
    void setBackgroundFill() {
        setBackground(BackgroundSetter::Mode::FILL);
    }
    void toggleDotfiles() {
        directory->toggleDots();
        mainWin->focusScrolling();
    }
    void toggleRelativePath() {
        directory->toggleRelativePath();
    }
    void sortByNameAscending() {
        directory->sortContentsByName(true);
    }
    void sortByNameDescending() {
        directory->sortContentsByName(false);
    }
    void initiateSearch() {
        std::string searchString = commandWin->getSearchInput();
        int matches = directory->findAllEntriesInDirectory(searchString);
        std::string searchResult;
        if (matches == 0) {
            searchResult = "Pattern not found: " + searchString;
        }
        else {
            searchResult = searchString + ": " + std::to_string(matches) + " match";
            if (matches > 1)
                searchResult += "es";
            mainWin->jumpToEntry(directory->foundEntries[0]);
        }

        commandWin->info = searchResult;
    }
    void printHelp() {
        if (previewWin->isUeberzugRunning) {
            previewWin->terminateImgPreview();
        }
        commandWin->move(maxRows - BOTTOM_OFFSET, maxCols - 3, BOTTOM_OFFSET, 2);
        commandWin->printHelp();
        getch();
        //reset the form entirely to prevent artifacts from appearing
        resize();
    }
    void clearSearchHighlights() {
        directory->clearSearchResults();
    }
    void loopResultsForward() {
        commandWin->info = mainWin->chooseNextFoundEntry(true);
    }
    void loopResultsBackward() {
        commandWin->info = mainWin->chooseNextFoundEntry(false);
    }
    void setRandomBackground() {
        std::vector<fs::path> images = directory->getAllImages();
        if(!images.empty()) {
            srand(time(0));
            int idx = rand() % images.size();
            setBackground(images[idx], BackgroundSetter::Mode::FILL);
        }
        else {
            commandWin->info = NO_IMAGE_IN_DIR_MSG;
        }
    }
    void initColors();
    void printWindows();
    void printInitialSetup();

public:
    Form(const std::shared_ptr<Config> &config);

    ~Form();

    void resize();

    void renderImgPreview();

    void setBackground(BackgroundSetter::Mode mode);

    void setBackground(fs::path imagePath, BackgroundSetter::Mode mode);

    void run();
};

#endif
