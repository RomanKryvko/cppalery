#include "backgroundSetter.h"
#include <cstdlib>

BackgroundSetter::BackgroundSetter() = default;

BackgroundSetter::BackgroundSetter(const std::string& wallpaperCenter, const std::string& wallpaperFill, const std::shared_ptr<DirectoryPager>& pager, const std::shared_ptr<DirectoryController>& directoryController, const std::shared_ptr<MessagePrinter>& messagePrinter) :
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
    if (mode == Mode::FILL) {
        command = wallpaperFill + command;
    }
    else {
        command = wallpaperCenter + command;
    }

    system(command.c_str());
}
