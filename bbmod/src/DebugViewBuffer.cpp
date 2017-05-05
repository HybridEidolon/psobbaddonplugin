#include "DebugViewBuffer.h"

#include "log.h"

DebugViewBuffer::~DebugViewBuffer()
{
    sync(); // can be avoided
}

int DebugViewBuffer::sync()
{
    OutputDebugString(str().c_str());
    g_lualog.AddLog("%s", str().c_str());
    str("");

    return 0;
}
