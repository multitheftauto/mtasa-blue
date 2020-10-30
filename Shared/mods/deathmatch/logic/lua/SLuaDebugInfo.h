#pragma once

#include "SString.h"

// Lua debug info for logging
enum
{
    DEBUG_INFO_NONE,
    DEBUG_INFO_FILE_AND_LINE,
    DEBUG_INFO_SHORT_SRC,
};

#define INVALID_LINE_NUMBER (-1)

struct SLuaDebugInfo
{
    SLuaDebugInfo() : iLine(INVALID_LINE_NUMBER), infoType(DEBUG_INFO_NONE) {}
    SLuaDebugInfo(const SString& strFile, int iLine, const SString& strShortSrc = "")
        : strFile(strFile), strShortSrc(strShortSrc), iLine(iLine), infoType(DEBUG_INFO_FILE_AND_LINE)
    {
    }
    SString strFile;
    SString strShortSrc;
    int     iLine;
    int     infoType;
};
