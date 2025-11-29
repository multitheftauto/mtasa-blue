/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/HookSystem.h
 *  PURPOSE:     Function hook installation system
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define MAX_JUMPCODE_SIZE           20

#include "gamesa_init.h"
#include <version.h>

#pragma warning(disable : 4102) // unreferenced label

// This macro adds an unreferenced label to your '__declspec(naked)' hook functions, to
// point to the value of __LOCAL_SIZE, which will be examined by an external tool after
// compilation, and it must be zero.
// 
// The Microsoft Visual C++ compiler (MSVC) expects you, the developer, to allocate space for
// local variables on the stack frame in custom prolog code. In the MSVC implementation of the
// C++17 language standard, the compiler started to use local space for certain statements,
// for which we will never support any sort of local space, in naked hook functions.
// https://learn.microsoft.com/en-us/cpp/cpp/considerations-for-writing-prolog-epilog-code
// https://developercommunity.visualstudio.com/t/stack-access-broken-in-naked-function/549628
// 
// IMPORTANT: We can't use static_assert because __LOCAL_SIZE is not a compile-time constant.
//            If you're going to change this macro, then copy your changes to the copy in multiplayer_sa.
#define MTA_VERIFY_HOOK_LOCAL_SIZE                     \
{                                                      \
    __asm {              push   eax                };  \
    __asm { _localSize:  mov    eax, __LOCAL_SIZE  };  \
    __asm {              pop    eax                };  \
}

template <typename T>
void* FunctionPointerToVoidP(T func)
{
    union
    {
        T     a;
        void* b;
    } c = {func};
    return c.b;
}

BYTE* CreateJump(DWORD dwFrom, DWORD dwTo, BYTE* ByteArray);

void HookInstallCall(DWORD dwInstallAddress, DWORD dwHookFunction);
void HookInstallVTBLCall(void* vtblMethodAddress, std::uintptr_t hookFunction);

template <typename T>
bool HookInstall(DWORD dwInstallAddress, T dwHookHandler, int iJmpCodeSize = 5)
{
    BYTE JumpBytes[MAX_JUMPCODE_SIZE];
    MemSetFast(JumpBytes, 0x90, MAX_JUMPCODE_SIZE);
    if (CreateJump(dwInstallAddress, (DWORD)FunctionPointerToVoidP(dwHookHandler), JumpBytes))
    {
        MemCpy((PVOID)dwInstallAddress, JumpBytes, iJmpCodeSize);
        return true;
    }
    else
    {
        return false;
    }
}

// Auto detect requirement of US/EU hook installation
#define EZHookInstall(type) \
    HookInstall(HOOKPOS_##type, (DWORD)HOOK_##type, HOOKSIZE_##type);

// Structure for holding hook info
struct SHookInfo
{
    template <class T>
    SHookInfo(DWORD dwAddress, T dwHook, uint uiSize) : dwAddress(dwAddress), dwHook((DWORD)dwHook), uiSize(uiSize)
    {
    }
    DWORD dwAddress;
    DWORD dwHook;
    uint  uiSize;
};

#define MAKE_HOOK_INFO(type)  SHookInfo ( HOOKPOS_##type, HOOK_##type, HOOKSIZE_##type )

// Structure for holding poke info
struct SPokeInfo
{
    DWORD dwAddress;
    BYTE  ucValue;
};
