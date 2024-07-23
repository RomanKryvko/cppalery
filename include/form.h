#ifndef FORM_H
#define FORM_H

#include "directory.h"
#include "keyGlobals.h"
#include "mainWindow.h"
#include "commandWindow.h"
#include "backgroundSetter.h"
#include "previewWindow.h"
#include "config.h"

namespace fs = std::filesystem;

const int BOTTOM_OFFSET = 4;
const int IMG_DELAY = 200;

const std::string NO_IMAGE_IN_DIR_MSG = "No images of supported formats found in the directory.";

class Form {
    private:
        MainWindow mainWin;
        CommandWindow commandWin;
        PreviewWindow previewWin;
        Directory directory;
        BackgroundSetter backSetter;
        Config config;
        int maxRows;
        int maxCols;

    public:
        Form(Config config);

        ~Form();

        void resize();

        void renderImgPreview();

        void setBackground(BackgroundSetter::Mode mode);

        void setBackground(fs::path imagePath, BackgroundSetter::Mode mode);

        void loopOptions();
};

#endif
