#pragma once

#include "directoryController.h"
#include "directoryPager.h"
#include "keybindings.h"
#include "mainWindow.h"
#include "commandWindow.h"
#include "backgroundSetter.h"
#include "previewWindow.h"
#include "config.h"
#include <map>
#include <memory>

namespace fs = std::filesystem;

class Form {
private:
    static constexpr int BOTTOM_OFFSET = 4;
    static constexpr int IMG_DELAY = 200;
    MainWindow mainWin;
    std::shared_ptr<CommandWindow> commandWin;
    PreviewWindow previewWin;
    std::shared_ptr<DirectoryController> directoryController;
    std::shared_ptr<DirectoryPager> pager;
    std::shared_ptr<Config> config;
    std::shared_ptr<BackgroundSetter> backSetter;
    std::map<fs::path, fs::path> directoryHistory;
    int maxRows;
    int maxCols;

    Keybindings keybindings = Keybindings({
        {{'g', 'g'}, [this]() { pager->jumpToTop(); }},
        {{'G'}, [this]() { pager->jumpToBottom(); }},
        {{'z', 'z'}, [this]() { pager->focusScrolling();}},
        {{KEY_UP}, [this]() { pager->scrollUp();}},
        {{'k'}, [this]() { pager->scrollUp();}},
        {{KEY_DOWN}, [this]() { pager->scrollDown();}},
        {{'j'}, [this]() { pager->scrollDown();}},
        {{KEY_RESIZE}, std::bind(&Form::resize, this)},
        {{KEY_LEFT}, [this]() { directoryController->goUpDirectory(); }},
        {{'h'}, [this]() { directoryController->goUpDirectory(); }},
        {{KEY_RIGHT}, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {{'l'}, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {{KEY_ENTER}, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {{KEY_ENTER_ALT}, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {{'c'}, [this]() { backSetter->setCurrentEntryAsBackground(BackgroundSetter::Mode::CENTER); }},
        {{'f'}, [this]() { backSetter->setCurrentEntryAsBackground(BackgroundSetter::Mode::FILL); }},
        {{'H'}, [this]() { directoryController->toggleDots(); }},
        {{'p'}, [this]() { directoryController->toggleRelativePath(); }},
        {{'a'}, [this]() { directoryController->sortByAscending(); }},
        {{'d'}, [this]() { directoryController->sortByDescending(); }},
        {{'/'}, std::bind(&Form::initiateSearch, this)},
        {{'?'}, std::bind(&Form::printHelp, this)},
        {{KEY_ESC}, [this]() { directoryController->clearSearchResults(); }},
        {{'n'}, [this]() { directoryController->chooseNextFoundEntry(); }},
        {{'N'}, [this]() { directoryController->choosePreviousFoundEntry(); }},
        {{'r'}, [this]() { backSetter->setRandomBackground(); }}
    });

    void goIntoDirOrSetBackground();
    void initiateSearch();
    void printHelp();

    void initColors();
    void printWindows();
    void printInitialSetup();

public:
    Form(const std::shared_ptr<Config> &config);

    ~Form();

    void resize();

    void renderImgPreview();

    void run();
};
