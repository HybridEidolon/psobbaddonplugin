#include "log.h"

#include "DebugViewBuffer.h"

static DebugViewBuffer buf;

std::ostream g_log(&buf);

AppLog g_lualog;
