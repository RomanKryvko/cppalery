#pragma once

#include "IDirectoryController.h"
#include "directoryPager.h"
#include "messagePrinter.h"
#include <memory>
#include <string>

namespace fs = std::filesystem;

class BackgroundSetter {
public:
    enum class Mode {
        AUTO,
        FILL,
        CENTER
    };

    BackgroundSetter();

    BackgroundSetter(const std::string& wallpaperCenter, const std::string& wallpaperFill, const std::shared_ptr<DirectoryPager>& pager, const std::shared_ptr<IDirectoryController>& directoryController, const std::shared_ptr<MessagePrinter>& messagePrinter);

    void setRandomBackground() const;

    void setCurrentEntryAsBackground(BackgroundSetter::Mode mode) const;

    void setBackground(const fs::path& fileName, BackgroundSetter::Mode mode = BackgroundSetter::Mode::AUTO) const;

private:
    static constexpr inline char NO_IMAGE_IN_DIR_MSG[] = "No images of supported formats found in the directory.";
    std::string wallpaperCenter;
    std::string wallpaperFill;
    std::shared_ptr<DirectoryPager> pager;
    std::shared_ptr<IDirectoryController> directoryController;
    std::shared_ptr<MessagePrinter> messagePrinter;

    void getDisplayDimens(int& x, int& y) const;
    void getImageDimens(const fs::path& imageName, int& x, int& y) const;
    Mode determineOptimalModeForImage(const fs::path& fileName) const;

};
