#ifndef FORM_H
#define FORM_H

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

const std::string NO_IMAGE_IN_DIR_MSG = "No images of supported formats found in the directory.";

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

        void setBackground(BackgroundSetter::Mode mode);

        void setBackground(fs::path imagePath, BackgroundSetter::Mode mode);

        void loopOptions();
};

#endif