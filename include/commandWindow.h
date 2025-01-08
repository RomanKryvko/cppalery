#pragma once

#include <string>
#include "messagePrinter.h"
#include "window.h"

class CommandWindow : public Window, public MessagePrinter {
private:
    static constexpr int STATUS_RULER_OFFSET = 1;
    static constexpr int STATUS_RULER_GAP = 4;
    std::string message;

public:
    CommandWindow();
    CommandWindow(int height, int width, int startX, int startY);

    virtual void setMessage(const std::string& msg) override;

    void resetSetup() override;

    void move(int newHeigth, int newWidth, int startX, int startY);

    void resize(int newHeight, int newWidth) override;

    void printStatus(int position, int total);

    std::string getSearchInput();

    void printHelp();
};
