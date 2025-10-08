/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.MemAccess.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <SharedUtil.IntTypes.h>

#ifdef _WIN32
    #ifdef MTASA_EXPORT_SHARED_UTIL
        #define SHARED_UTIL_API __declspec(dllexport)
    #else
        #define SHARED_UTIL_API __declspec(dllimport)
    #endif
#else
    #define SHARED_UTIL_API
#endif

namespace SharedUtil
{
    struct SMemWrite
    {
        DWORD dwFirstPage;
        DWORD dwSize;
        DWORD oldProt;
    };

    SHARED_UTIL_API void      SetInitialVirtualProtect();
    SHARED_UTIL_API bool      IsSlowMem(const void* pAddr, uint uiAmount);
    SHARED_UTIL_API SMemWrite OpenMemWrite(const void* pAddr, uint uiAmount);
    SHARED_UTIL_API void      CloseMemWrite(SMemWrite& hMem);
    SHARED_UTIL_API bool      ismemset(const void* pAddr, int cValue, uint uiAmount);

    bool IsProtectedSlowMem(const void* pAddr);

#define DEBUG_CHECK_IS_FAST_MEM(addr, size) \
    { \
        dassert(!IsSlowMem((const void*)(addr), size)); \
    }
#define DEBUG_CHECK_IS_SLOW_MEM(addr, size) \
    { \
        dassert(IsSlowMem((const void*)(addr), size)); \
    }
}            // namespace SharedUtil
