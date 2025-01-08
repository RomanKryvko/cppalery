#pragma once

#include <string>

class MessagePrinter {
public:
    virtual void setMessage(const std::string& message) = 0;
};
