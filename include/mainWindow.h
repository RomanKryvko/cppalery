#pragma once

#include "directoryController.h"
#include "pager.h"
#include "window.h"
#include "colors.h"
#include <cstring>
#include <memory>

class MainWindow : public Window {
private:
    void printColoredString(const char* str, int y, int x, ColorPair pair);
    static std::string truncateString(const std::string& str, int maxLength) {
        return str.substr(0, maxLength).append("~");
    }

public:
    bool showPreview;

    MainWindow();

    MainWindow(int height, int width);

    MainWindow(const MainWindow& other);

    MainWindow& operator=(const MainWindow& other);

    void resetSetup() override;

    void printDirectoryContents(const std::shared_ptr<Pager>& pager, const std::shared_ptr<DirectoryController>& dirController);
};
