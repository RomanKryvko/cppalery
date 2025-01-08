#pragma once

#include "IDirectoryController.h"

class IDirectoryObserver {
public:
    virtual void onDirectoryChanged(const IDirectoryController* directory) = 0;
    virtual void onDirectoryEntriesChanged(const IDirectoryController* directory) = 0;
};
