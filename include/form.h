#ifndef FORM_H
#define FORM_H

#include <ncurses.h>
#include <filesystem>
#include <chrono>
#include "keyGlobals.h"
#include "directory.h"
#include "mainWindow.h"
#include "commandWindow.h"
#include "backgroundSetter.h"
#include "previewWindow.h"

namespace fs = std::filesystem;
namespace ch = std::chrono;

const int BOTTOM_OFFSET = 4;
const int IMG_DELAY = 200;

class Form {
    private:
        MainWindow mainWin;
        CommandWindow commandWin;
        PreviewWindow previewWin;
        int maxRows;
        int maxCols;
        fs::path workPath;

    public:
        Form(const std::string &workPath);

        ~Form();

        void resize();

        void renderImgPreview();

        void loopOptions();
};

#endif