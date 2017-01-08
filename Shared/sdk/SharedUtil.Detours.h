/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Detours.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

#include "detours/src/detours.h"

// Map 1.5 functions to work with 3.0
inline
void DetourRemove(PBYTE pbTrampoline, PBYTE pbDetour)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    PVOID pTemp = pbTrampoline;
    DetourDetach(&(PVOID&)pTemp, pbDetour);
    DetourTransactionCommit();
}

template < class T >
PBYTE DetourFunction(T pbTargetFunction, PBYTE pbDetourFunction)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    PVOID pTemp = pbTargetFunction;
    if (DetourAttach(&(PVOID&)pTemp,pbDetourFunction) != NO_ERROR)
    {
        pTemp = nullptr;
    }
    if (DetourTransactionCommit() != NO_ERROR)
    {
        pTemp = nullptr;
    }
    return (PBYTE)pTemp;
}
