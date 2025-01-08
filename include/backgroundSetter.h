#pragma once

#include "directoryController.h"
#include "directoryPager.h"
#include <memory>
#include <string>

namespace fs = std::filesystem;

class BackgroundSetter {
private:
    std::string wallpaperCenter;
    std::string wallpaperFill;
    std::shared_ptr<DirectoryPager> pager;
    std::shared_ptr<DirectoryController> directoryController;

public:
    enum class Mode {
        FILL,
        CENTER
    };

    BackgroundSetter();

    BackgroundSetter(const std::string& wallpaperCenter, const std::string& wallpaperFill, const std::shared_ptr<DirectoryPager>& pager, const std::shared_ptr<DirectoryController>& directoryController);

    void setRandomBackground() const;

    void setCurrentEntryAsBackground(BackgroundSetter::Mode mode = BackgroundSetter::Mode::FILL) const;

    void setBackground(const fs::path& fileName, BackgroundSetter::Mode mode = BackgroundSetter::Mode::FILL) const;

};
