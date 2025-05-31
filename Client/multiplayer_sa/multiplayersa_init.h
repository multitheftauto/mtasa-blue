/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/multiplayersa_init.h
 *  PURPOSE:     Multiplayer module entry
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Common.h"

#include <game/CGame.h>
#include "../game_sa/CGameSA.h"

#include "multiplayer_keysync.h"
#include "multiplayer_shotsync.h"

#include "CMultiplayerSA.h"

extern CGame* pGameInterface;
// extern CMultiplayerSA* pMultiplayer;

/** Buffer overrun trace - attach debugger and watch out for EXCEPTION_GUARD_PAGE (0x80000001) **/
#ifdef IJSIFY
    #pragma message(__LOC__ "YOU HAVE ENABLED THE BOUNDS CHECKER. This may cause performance and/or stability issues!")

    #include <windows.h>
    #include <math.h>

inline void* __cdecl operator new(size_t size)
{
    DWORD dwOld;
    DWORD dwPageSpan = ceil(size / 4096.0f) * 4096;

    DWORD dwPage = 0;
    while (dwPage == NULL)
    {
        dwPage = (DWORD)VirtualAlloc(NULL, dwPageSpan + 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    }
    VirtualProtect((LPVOID)(dwPage + dwPageSpan), 1, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &dwOld);
    dwPage += (dwPageSpan - size);

    return (LPVOID)dwPage;
};

inline void* __cdecl operator new[](size_t size)
{
    DWORD dwOld;
    DWORD dwPageSpan = ceil(size / 4096.0f) * 4096;

    DWORD dwPage = 0;
    while (dwPage == NULL)
    {
        dwPage = (DWORD)VirtualAlloc(NULL, dwPageSpan + 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    }
    VirtualProtect((LPVOID)(dwPage + dwPageSpan), 1, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &dwOld);
    dwPage += (dwPageSpan - size);

    return (LPVOID)dwPage;
};

inline void __cdecl operator delete(LPVOID pPointer)
{
    VirtualFree(pPointer, NULL, MEM_RELEASE);
};

inline void __cdecl operator delete[](LPVOID pPointer)
{
    VirtualFree(pPointer, NULL, MEM_RELEASE);
};
#endif

//
// Use MemSet/Cpy/Put for non Mem*Fast memory regions
//

void MemSet(void* dwDest, int cValue, uint uiAmount);
void MemCpy(void* dwDest, const void* dwSrc, uint uiAmount);

template <class T, class U>
void MemPut(U ptr, const T value)
{
    if (*(T*)ptr != value)
        MemCpy((void*)ptr, &value, sizeof(T));
}

//
// Use Mem*Fast for the following memory regions:
//
// 0x4C0300 - 0x4C03FF
// 0x4EB900 - 0x4EB9FF
// 0x502200 - 0x5023FF
// 0x50AB00 - 0x50ABFF
// 0x50BF00 - 0x50BFFF
// 0x533200 - 0x5332FF
// 0x609C00 - 0x609CFF
// 0x60D800 - 0x60D8FF
// 0x60F200 - 0x60F2FF
// 0x642000 - 0x6420FF
// 0x648A00 - 0x648AFF
// 0x64CA00 - 0x64CAFF
// 0x687000 - 0x6870FF
// 0x6A0700 - 0x6A07FF
// 0x6AEA00 - 0x6AEAFF
// 0x729B00 - 0x729BFF
// 0x742B00 - 0x742BFF
//

inline void MemCpyFast(void* dwDest, const void* dwSrc, uint uiAmount)
{
    DEBUG_CHECK_IS_FAST_MEM(dwDest, uiAmount);
    memcpy(dwDest, dwSrc, uiAmount);
}

inline void MemSetFast(void* dwDest, int cValue, uint uiAmount)
{
    DEBUG_CHECK_IS_FAST_MEM(dwDest, uiAmount);
    memset(dwDest, cValue, uiAmount);
}

template <class T, class U>
void MemPutFast(U ptr, const T value)
{
    DEBUG_CHECK_IS_FAST_MEM(ptr, sizeof(T));
    *(T*)ptr = value;
}

template <class T, class U>
void MemSubFast(U ptr, const T value)
{
    DEBUG_CHECK_IS_FAST_MEM(ptr, sizeof(T));
    *(T*)ptr -= value;
}

bool                            GetDebugIdEnabled(uint uiDebugId);
void                            LogEvent(uint uiDebugId, const char* szType, const char* szContext, const char* szBody, uint uiAddReportLogId = 0);
void                            CallGameEntityRenderHandler(CEntitySAInterface* pEntity);
extern GameEntityRenderHandler* pGameEntityRenderHandler;
