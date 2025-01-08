#ifndef FORM_H
#define FORM_H

#include "directoryController.h"
#include "keybindings.h"
#include "mainWindow.h"
#include "commandWindow.h"
#include "backgroundSetter.h"
#include "pager.h"
#include "previewWindow.h"
#include "config.h"
#include <map>
#include <memory>

namespace fs = std::filesystem;

class Form {
private:
    static constexpr inline char NO_IMAGE_IN_DIR_MSG[] = "No images of supported formats found in the directory.";
    static constexpr int BOTTOM_OFFSET = 4;
    static constexpr int IMG_DELAY = 200;
    MainWindow mainWin;
    CommandWindow commandWin;
    PreviewWindow previewWin;
    std::shared_ptr<DirectoryController> directoryController;
    Pager pager;
    std::shared_ptr<Config> config;
    BackgroundSetter backSetter;
    std::map<fs::path, fs::path> directoryHistory;
    int maxRows;
    int maxCols;

    Keybindings keybindings = Keybindings({
        {{'g', 'g'}, std::bind(&Pager::jumpToTop, &pager)},
        {{'G'}, std::bind(&Pager::jumpToBottom, &pager)},
        {{'z', 'z'}, std::bind(&Pager::focusScrolling, &pager)},
        {{KEY_UP}, std::bind(&Pager::scrollUp, &pager)},
        {{'k'}, std::bind(&Pager::scrollUp, &pager)},
        {{KEY_DOWN}, std::bind(&Pager::scrollDown, &pager)},
        {{'j'}, std::bind(&Pager::scrollDown, &pager)},
        {{KEY_RESIZE}, std::bind(&Form::resize, this)},
        {{KEY_LEFT}, std::bind(&Form::goUpDir, this)},
        {{'h'}, std::bind(&Form::goUpDir, this)},
        {{KEY_RESIZE}, std::bind(&Form::resize, this)},
        {{KEY_RIGHT}, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {{'l'}, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {{KEY_ENTER}, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {{KEY_ENTER_ALT}, std::bind(&Form::goIntoDirOrSetBackground, this)},
        {{'c'}, std::bind(&Form::setBackgroundCenter, this)},
        {{'f'}, std::bind(&Form::setBackgroundFill, this)},
        {{'H'}, std::bind(&Form::toggleDotfiles, this)},
        {{'p'}, std::bind(&Form::toggleRelativePath, this)},
        {{'a'}, std::bind(&Form::sortByNameAscending, this)},
        {{'d'}, std::bind(&Form::sortByNameDescending, this)},
        {{'/'}, std::bind(&Form::initiateSearch, this)},
        {{'?'}, std::bind(&Form::printHelp, this)},
        {{KEY_ESC}, std::bind(&Form::clearSearchHighlights, this)},
        {{'n'}, std::bind(&Form::loopResultsForward, this)},
        {{'N'}, std::bind(&Form::loopResultsBackward, this)},
        {{'r'}, std::bind(&Form::setRandomBackground, this)}
    });

    void goUpDir();
    void goIntoDirOrSetBackground();
    void setBackgroundCenter();
    void setBackgroundFill();
    void toggleDotfiles();
    void toggleRelativePath();
    void sortByNameAscending();
    void sortByNameDescending();
    void initiateSearch();
    void printHelp();
    void clearSearchHighlights();
    void loopResultsForward();
    void loopResultsBackward();
    void setRandomBackground();

    void initColors();
    void printWindows();
    void printInitialSetup();

public:
    Form(const std::shared_ptr<Config> &config);

    ~Form();

    void resize();

    void renderImgPreview();

    void setBackground(BackgroundSetter::Mode mode);

    void setBackground(const fs::path& imagePath, BackgroundSetter::Mode mode);

    void run();
};

#endif
