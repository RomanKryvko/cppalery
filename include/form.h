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

class Form {
    private:
        const std::string NO_IMAGE_IN_DIR_MSG = "No images of supported formats found in the directory.";
        const int BOTTOM_OFFSET = 4;
        const int IMG_DELAY = 200;
        MainWindow mainWin;
        CommandWindow commandWin;
        PreviewWindow previewWin;
        Directory directory;
        BackgroundSetter backSetter;
        Config config;
        int maxRows;
        int maxCols;

        enum InputProcessingResultCode {
            CONTINUE,
            EXIT
        };

        void initColors();
        void printWindows();
        void printInitialSetup();
        InputProcessingResultCode processUserInput(int input);

    public:
        Form(Config config);

        ~Form();

        void resize();

        void renderImgPreview();

        void setBackground(BackgroundSetter::Mode mode);

        void setBackground(fs::path imagePath, BackgroundSetter::Mode mode);

        void run();
};

#endif
