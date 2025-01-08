#ifndef IDIRECTORY_OBSERVER_H
#define IDIRECTORY_OBSERVER_H

#include "IDirectoryController.h"

class IDirectoryObserver {
public:
    virtual void onDirectoryChanged(const IDirectoryController* directory) = 0;
    virtual void onDirectoryEntriesChanged(const IDirectoryController* directory) = 0;
};

#endif
