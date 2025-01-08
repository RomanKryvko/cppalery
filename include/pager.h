#pragma once

class Pager {
protected:
    static constexpr int SCROLL_OFFSET = 2;
    int minDisplayedIdx;
    int height;
    int numberOfElements;
    int selection;

public:
    Pager();
    Pager(const Pager& other);
    Pager& operator=(const Pager& other);
    Pager(int height, int numberOfElements);
    void setNumberOfElements(int number);
    void setHeight(int height);
    void focusScrolling();
    void scrollUp();
    void scrollDown();
    void jumpToIdx(int idx);
    void jumpToTop();
    void jumpToBottom();
    int getSelection() const;
    int getMinDisplayedIdx() const;
    int getNumberOfElements() const;
};
