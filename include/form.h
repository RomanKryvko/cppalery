#ifndef FORM_H
#define FORM_H

#include "keyGlobals.h"
#include "mainWindow.h"
#include "commandWindow.h"
#include "backgroundSetter.h"
#include "previewWindow.h"

namespace fs = std::filesystem;

const int BOTTOM_OFFSET = 4;
const int IMG_DELAY = 200;

const std::string NO_IMAGE_IN_DIR_MSG = "No images of supported formats found in the directory.";

class Form {
    private:
        MainWindow mainWin;
        CommandWindow commandWin;
        PreviewWindow previewWin;
        BackgroundSetter backSetter;
        int maxRows;
        int maxCols;
        fs::path workPath;
        bool showPreview;
        bool sortNameAsc;
        bool relativePath;

    public:
        Form(const std::string &workPath, const std::string &wallpaperFill, const std::string &wallpaperCenter, bool showPreview, bool sortNameAsc, bool relativePath);

        ~Form();

        void resize();

        void renderImgPreview();

        void setBackground(BackgroundSetter::Mode mode);

        void setBackground(fs::path imagePath, BackgroundSetter::Mode mode);

        void loopOptions();
};

#endif
