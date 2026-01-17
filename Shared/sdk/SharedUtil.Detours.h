/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Detours.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#if defined(WIN32)
#include <detours.h>

namespace SharedUtil
{
    template <typename TargetType>
    inline bool DetourFunction(TargetType& target, void* replacement)
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&reinterpret_cast<PVOID&>(target), replacement);
        return DetourTransactionCommit() == NO_ERROR;
    }

    template <typename TargetType>
    inline bool DetourLibraryFunction(const char* libraryName, const char* functionName, TargetType& target, void* replacement)
    {
        target = reinterpret_cast<TargetType>(DetourFindFunction(libraryName, functionName));
        return (target != nullptr) ? DetourFunction(target, replacement) : false;
    }

    inline bool DetourLibraryFunction(const char* libraryName, const char* functionName, void* replacement)
    {
        PVOID target = DetourFindFunction(libraryName, functionName);
        return DetourFunction(target, replacement);
    }

    template <typename TargetType>
    inline bool UndoFunctionDetour(TargetType& target, void* replacement)
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&reinterpret_cast<PVOID&>(target), replacement);
        return DetourTransactionCommit() == NO_ERROR;
    }
}            // namespace SharedUtil

#endif
