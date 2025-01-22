#include "backgroundSetter.h"
#include <cstdlib>
#include <X11/Xlib.h>
#include <Imlib2.h>

BackgroundSetter::BackgroundSetter() = default;

BackgroundSetter::BackgroundSetter(const std::string& wallpaperCenter, const std::string& wallpaperFill, const std::shared_ptr<DirectoryPager>& pager, const std::shared_ptr<IDirectoryController>& directoryController, const std::shared_ptr<MessagePrinter>& messagePrinter) :
    wallpaperFill(wallpaperFill),
    wallpaperCenter(wallpaperCenter),
    pager(pager),
    directoryController(directoryController),
    messagePrinter(messagePrinter)
{}

void BackgroundSetter::setRandomBackground() const {
    std::vector<fs::path> images = directoryController->getAllImages();
    if(!images.empty()) {
        srand(time(0));
        int idx = rand() % images.size();
        setBackground(images[idx]);
        return;
    }
    messagePrinter->setMessage(NO_IMAGE_IN_DIR_MSG);
}

void BackgroundSetter::setCurrentEntryAsBackground(BackgroundSetter::Mode mode) const {
    if (directoryController->isAnImage(pager->getSelection())) {
        setBackground(directoryController->getPathAt(pager->getSelection()), mode);
    }
}

void BackgroundSetter::setBackground(const fs::path& fileName, BackgroundSetter::Mode mode) const {
    std::string command = '\"' + fileName.string() + "\" > /dev/null";
    // AUTO sets image setting in either FILL or CENTER mode depending on image dimensions
    if (mode == Mode::AUTO) {
        mode = determineOptimalModeForImage(fileName);
    }

    if (mode == Mode::FILL) {
        command = wallpaperFill + command;
    }
    else {
        command = wallpaperCenter + command;
    }

    system(command.c_str());
}

BackgroundSetter::Mode BackgroundSetter::determineOptimalModeForImage(const fs::path& fileName) const {
    int displayWidth;
    int displayHeight;
    getDisplayDimens(displayWidth, displayHeight);

    int imgWidth;
    int imgHeight;
    try {
        getImageDimens(fileName, imgWidth, imgHeight);
    }
    catch (const std::exception& ex) {
        messagePrinter->setMessage(ex.what());
        return BackgroundSetter::Mode::FILL;
    }

    if (imgWidth > displayWidth && imgHeight && displayHeight)
        return BackgroundSetter::Mode::CENTER;

    return BackgroundSetter::Mode::FILL;
}

void BackgroundSetter::getDisplayDimens(int& x, int& y) const {
    Display* dpy = XOpenDisplay(0);
    int screen = DefaultScreen(dpy);
    x = DisplayWidth(dpy, screen);
    y = DisplayHeight(dpy, screen);
    XCloseDisplay(dpy);
}

void BackgroundSetter::getImageDimens(const fs::path& imageName, int& x, int& y) const {
    Imlib_Image image;
    image = imlib_load_image(imageName.c_str());
    if (!image)
        throw std::runtime_error("Failed to load image " + imageName.string());

    imlib_context_set_image(image);
    x = imlib_image_get_width();
    y = imlib_image_get_height();
    imlib_free_image();
}
