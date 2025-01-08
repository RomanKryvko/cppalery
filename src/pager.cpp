#include "pager.h"
#include <algorithm>

Pager::Pager() = default;

Pager::Pager(int height, int numberOfElements) :
    height(height),
    numberOfElements(numberOfElements),
    selection(0),
    minDisplayedIdx(0)
{}

Pager::Pager(const Pager& other) {
    height = other.height;
    numberOfElements = other.numberOfElements;
    selection = other.selection;
    minDisplayedIdx = other.minDisplayedIdx;
}

Pager& Pager::operator=(const Pager& other) {
    if (this == &other)
        return *this;

    height = other.height;
    numberOfElements = other.numberOfElements;
    selection = other.selection;
    minDisplayedIdx = other.minDisplayedIdx;

    return  *this;
}

void Pager::setNumberOfElements(int number) {
    numberOfElements = number;
    selection = std::min(selection, numberOfElements - 1);
}

void Pager::setHeight(int height) {
    this->height = height;
    selection = std::min(selection, height);
}

void Pager::scrollUp() {
    if (selection > 0)
        selection--;

    if (minDisplayedIdx > 0 && selection < minDisplayedIdx + SCROLL_OFFSET) {
        minDisplayedIdx--;
    }
}

void Pager::scrollDown() {
    int maxDisplayedIdx = std::min(minDisplayedIdx + height, numberOfElements);

    if (selection < numberOfElements - 1) {
        selection++;
    }
    if (minDisplayedIdx < numberOfElements - 1 && selection >= maxDisplayedIdx - SCROLL_OFFSET && maxDisplayedIdx != numberOfElements) {
        minDisplayedIdx++;
    }
}

void Pager::jumpToIdx(int idx) {
    if (idx >= 0 && idx < numberOfElements) {
        selection = idx;
        focusScrolling();
        return;
    }
    selection = 0;
    minDisplayedIdx = 0;
}

void Pager::focusScrolling() {
    if (numberOfElements <= height) {
        minDisplayedIdx = 0;
        return;
    }

    // Prevent blank space from appearing when focusing on entries at the bottom of directory
    if (selection + height / 2 >= numberOfElements) {
        minDisplayedIdx = numberOfElements - height;
        return;
    }

    minDisplayedIdx = std::max(selection - height / 2, 0);
}

void Pager::jumpToTop() {
    jumpToIdx(0);
}

void Pager::jumpToBottom() {
    jumpToIdx(numberOfElements - 1);
}

int Pager::getSelection() const {
    return selection;
}

int Pager::getMinDisplayedIdx() const {
    return minDisplayedIdx;
}

int Pager::getNumberOfElements() const {
    return numberOfElements;
}
