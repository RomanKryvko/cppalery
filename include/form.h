#ifndef FORM_H
#define FORM_H

#include "directoryController.h"
#include "keyGlobals.h"
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
    const int BOTTOM_OFFSET = 4;
    const int IMG_DELAY = 200;
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

    void jumpToTop();
    void jumpToBottom();
    void focus();
    void scrollUp();
    void scrollDown();
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

    void setBackground(fs::path imagePath, BackgroundSetter::Mode mode);

    void run();
};

#endif
