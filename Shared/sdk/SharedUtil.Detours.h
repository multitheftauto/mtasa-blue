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
PBYTE DetourFunction(T pbTargetFunction, PBYTE pbDetourFunction, SString* pstrOutError=nullptr)
{
    if ( pbTargetFunction == nullptr )
    {
        if ( pstrOutError )
            *pstrOutError += "[TargetFunction missing]";
        return nullptr;
    }

    LONG hr = DetourTransactionBegin();
    if ( hr != NO_ERROR )
    {
        if ( pstrOutError )
            *pstrOutError += SString("[DetourTransactionBegin %u]", hr);
        return nullptr;
    }

    hr = DetourUpdateThread(GetCurrentThread());
    if ( hr != NO_ERROR )
    {
        if ( pstrOutError )
            *pstrOutError += SString("[DetourUpdateThread %u]", hr);
    }

    PVOID pTemp = pbTargetFunction;
    hr = DetourAttach(&(PVOID&)pTemp,pbDetourFunction);
    if (hr != NO_ERROR)
    {
        if ( pstrOutError )
            *pstrOutError += SString("[DetourAttach %u]", hr);
        pTemp = nullptr;
    }

    hr = DetourTransactionCommit();
    if (hr != NO_ERROR)
    {
        if ( pstrOutError )
            *pstrOutError += SString("[DetourTransactionCommit %u]", hr);
        pTemp = nullptr;
    }

    return (PBYTE)pTemp;
}
