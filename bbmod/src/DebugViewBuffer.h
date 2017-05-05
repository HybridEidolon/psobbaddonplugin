#pragma once

#include <sstream>
#include <windows.h>

class DebugViewBuffer : public std::stringbuf
{
protected:
    int sync();
public:
    virtual ~DebugViewBuffer();

};
