/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_ObjectStreamerOptimization.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

static unsigned int CEntryInfoNodePool_UsedSpaces = 0;
static unsigned int CPtrNodeDoubleLinkPool_UsedSpaces = 0;

#define HOOKPOS_CEntryInfoNodePool__New 0x536D6E
#define HOOKSIZE_CEntryInfoNodePool__New 0x5
static void _declspec(naked) HOOK_CEntryInfoNodePool__New()
{
    _asm {
        inc CEntryInfoNodePool_UsedSpaces

        lea eax, [ecx+eax*4]
        pop esi
        ret
    }
}

#define HOOKPOS_CEntryInfoNode__operator_delete 0x536DF1
#define HOOKSIZE_CEntryInfoNode__operator_delete 0x6
static const unsigned int RETURN_CEntryInfoNode__operator_delete = 0x536DF7;
static void _declspec(naked) HOOK_CEntryInfoNode__operator_delete()
{
    _asm {
        dec CEntryInfoNodePool_UsedSpaces

        lea eax, [ecx+edx]
        or byte ptr [eax], 0x80
        jmp RETURN_CEntryInfoNode__operator_delete
    }
}

#define HOOKPOS_CEntryInfoList__Flush 0x536E6C
#define HOOKSIZE_CEntryInfoList__Flush 0x5
static const unsigned int RETURN_CEntryInfoList__Flush = 0x536E71;
static void _declspec(naked) HOOK_CEntryInfoList__Flush()
{
    _asm {
        dec CEntryInfoNodePool_UsedSpaces

        or bl, 0x80
        mov [eax], bl
        jmp RETURN_CEntryInfoList__Flush
    }
}

#define HOOKPOS_CPtrNodeDoubleLinkPool__New 0x55233E
#define HOOKSIZE_CPtrNodeDoubleLinkPool__New 0x6
static void _declspec(naked) HOOK_CPtrNodeDoubleLinkPool__New()
{
    _asm {
        inc CPtrNodeDoubleLinkPool_UsedSpaces

        lea eax, [ecx+eax*4]
        pop esi
        ret
    }
}

#define HOOKPOS_CPtrNodeDoubleLink__operator_delete 0x5523F0
#define HOOKSIZE_CPtrNodeDoubleLink__operator_delete 0x6
static const unsigned int RETURN_CPtrNodeDoubleLink__operator_delete = 0x5523F6;
static void _declspec(naked) HOOK_CPtrNodeDoubleLink__operator_delete()
{
    _asm {
        dec CPtrNodeDoubleLinkPool_UsedSpaces

        lea eax, [ecx+edx]
        or byte ptr [eax], 0x80
        jmp RETURN_CPtrNodeDoubleLink__operator_delete
    }
}

#define HOOKPOS_CPtrListDoubleLink__Flush 0x5524CB
#define HOOKSIZE_CPtrListDoubleLink__Flush 0x5
static const unsigned int RETURN_CPtrListDoubleLink__Flush = 0x5524D0;
static void _declspec(naked) HOOK_CPtrListDoubleLink__Flush()
{
    _asm {
        dec CPtrNodeDoubleLinkPool_UsedSpaces

        or bl, 0x80
        mov [eax], bl
        jmp RETURN_CPtrListDoubleLink__Flush
    }
}

void CMultiplayerSA::InitHooks_ObjectStreamerOptimization()
{
    EZHookInstall(CEntryInfoNodePool__New);
    EZHookInstall(CEntryInfoNode__operator_delete);
    EZHookInstall(CEntryInfoList__Flush);

    EZHookInstall(CPtrNodeDoubleLinkPool__New);
    EZHookInstall(CPtrNodeDoubleLink__operator_delete);
    EZHookInstall(CPtrListDoubleLink__Flush);
}

unsigned int CMultiplayerSA::EntryInfoNodePool_NoOfUsedSpaces() const noexcept
{
    return CEntryInfoNodePool_UsedSpaces;
}

unsigned int CMultiplayerSA::PtrNodeDoubleLinkPool_NoOfUsedSpaces() const noexcept
{
    return CPtrNodeDoubleLinkPool_UsedSpaces;
}
