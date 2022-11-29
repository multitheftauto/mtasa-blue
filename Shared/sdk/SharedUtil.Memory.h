/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Memory.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <stddef.h>

namespace SharedUtil
{
    struct ProcessMemoryStats
    {
        size_t virtualMemorySize;
        size_t residentMemorySize;
        size_t sharedMemorySize;
        size_t privateMemorySize;
    };

    bool TryGetProcessMemoryStats(ProcessMemoryStats& out);
}
