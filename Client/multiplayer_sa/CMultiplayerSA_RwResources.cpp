/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_RwResources.cpp
 *  PORPOISE:    Record usage of certain Renderware resources
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <cstddef>

namespace
{
    SRwResourceStats ms_Stats;

    constexpr std::size_t kTextureRefsReadableSize = offsetof(RwTexture, refs) + sizeof(int);
    constexpr std::size_t kGeometryRefsReadableSize = offsetof(RwGeometry, refs) + sizeof(short);

}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwTextureCreate
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwTextureCreate_Pre(DWORD calledFrom)
{
}

void OnMY_RwTextureCreate_Post(RwTexture* pTexture, DWORD calledFrom)
{
    if (!pTexture)
        return;

    ms_Stats.uiTextures++;
}

// Hook info
#define HOOKPOS_RwTextureCreate                            0x7F37C0
#define HOOKSIZE_RwTextureCreate                           5
DWORD RETURN_RwTextureCreate = 0x7F37C5;
static void __declspec(naked) HOOK_RwTextureCreate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off

    __asm
    {
        pushad
        push    [esp+32+4*0]
        call    OnMY_RwTextureCreate_Pre
        add     esp, 4*1
        popad

        push    [esp+4*1]
        call    inner
        add     esp, 4*1

        pushad
        push    [esp+32+4*0]
        push    eax
        call    OnMY_RwTextureCreate_Post
        add     esp, 4*1+4
        popad

        retn

inner:
        mov     eax,dword ptr ds:[00C97B24h]
        jmp     RETURN_RwTextureCreate
    }

    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwTextureDestroy
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwTextureDestroy(RwTexture* pTexture, DWORD calledFrom)
{
    if (!pTexture)
        return;

    if (SharedUtil::IsReadablePointer(pTexture, kTextureRefsReadableSize))
    {
        if (pTexture->refs == 1 && ms_Stats.uiTextures > 0)
            ms_Stats.uiTextures--;
        return;
    }

    if (ms_Stats.uiTextures > 0)
        ms_Stats.uiTextures--;
}

// Hook info
#define HOOKPOS_RwTextureDestroy                            0x7F3820
#define HOOKSIZE_RwTextureDestroy                           5
DWORD RETURN_RwTextureDestroy = 0x7F3825;
static void __declspec(naked) HOOK_RwTextureDestroy()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off

    __asm
    {
        push    esi
        mov         esi,dword ptr [esp+8]
        pushad
        push    [esp+32+4*1]
        push    esi
        call    OnMY_RwTextureDestroy
        add     esp, 4*1+4
        popad

        jmp     RETURN_RwTextureDestroy
    }

    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwRasterCreate
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwRasterCreate(DWORD calledFrom)
{
    ms_Stats.uiRasters++;
}

// Hook info
#define HOOKPOS_RwRasterCreate                            0x7FB230
#define HOOKSIZE_RwRasterCreate                           5
DWORD RETURN_RwRasterCreate = 0x7FB235;
static void __declspec(naked) HOOK_RwRasterCreate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off

    __asm
    {
        mov         eax,dword ptr ds:[00C97B24h]

        pushad
        push    [esp+32+4*0]
        call    OnMY_RwRasterCreate
        add     esp, 4*1
        popad

        jmp     RETURN_RwRasterCreate
    }

    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwRasterDestroy
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwRasterDestroy(DWORD calledFrom)
{
    ms_Stats.uiRasters--;
}

// Hook info
#define HOOKPOS_RwRasterDestroy                            0x7FB020
#define HOOKSIZE_RwRasterDestroy                           5
DWORD RETURN_RwRasterDestroy = 0x7FB025;
static void __declspec(naked) HOOK_RwRasterDestroy()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off

    __asm
    {
        push    esi
        mov         esi,dword ptr [esp+8]
        pushad
        push    [esp+32+4*1]
        call    OnMY_RwRasterDestroy
        add     esp, 4*1
        popad

        jmp     RETURN_RwRasterDestroy
    }

    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwGeometryCreate
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwGeometryCreate_Pre(DWORD calledFrom)
{
}

void OnMY_RwGeometryCreate_Post(RwGeometry* pGeometry, DWORD calledFrom)
{
    if (!pGeometry)
        return;

    ms_Stats.uiGeometries++;
}

// Hook info
#define HOOKPOS_RwGeometryCreate                            0x74CA90
#define HOOKSIZE_RwGeometryCreate                           7
DWORD RETURN_RwGeometryCreate = 0x74CA97;
static void __declspec(naked) HOOK_RwGeometryCreate()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off

    __asm
    {
        pushad
        push    [esp+32+4*0]
        call    OnMY_RwGeometryCreate_Pre
        add     esp, 4*1
        popad

        push    [esp+4*3]
        push    [esp+4*3]
        push    [esp+4*3]
        call    inner
        add     esp, 4*3

        pushad
        push    [esp+32+4*0]
        push    eax
        call    OnMY_RwGeometryCreate_Post
        add     esp, 4*1+4
        popad

        retn

inner:
        mov     eax, [esp+04h]
        sub     esp, 8
        jmp     RETURN_RwGeometryCreate
    }

    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwGeometryDestroy
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwGeometryDestroy(DWORD calledFrom, RwGeometry* pGeometry)
{
    if (!pGeometry)
        return;

    if (SharedUtil::IsReadablePointer(pGeometry, kGeometryRefsReadableSize))
    {
        if (pGeometry->refs == 1 && ms_Stats.uiGeometries > 0)
            ms_Stats.uiGeometries--;
        return;
    }

    if (ms_Stats.uiGeometries > 0)
        ms_Stats.uiGeometries--;
}

// Hook info
#define HOOKPOS_RwGeometryDestroy                            0x74CCC0
#define HOOKSIZE_RwGeometryDestroy                           5
DWORD RETURN_RwGeometryDestroy = 0x74CCC5;
static void __declspec(naked) HOOK_RwGeometryDestroy()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off

    __asm
    {
        pushad
        push    [esp+32+4*1]
        push    [esp+32+4*1]
        call    OnMY_RwGeometryDestroy
        add     esp, 4*2
        popad

        push    esi
        mov         esi,dword ptr [esp+8]
        jmp     RETURN_RwGeometryDestroy
    }

    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::GetRwResourceStats
//
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::GetRwResourceStats(SRwResourceStats& outStats)
{
    outStats = ms_Stats;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_RwResources
//
// Setup hooks for RwResources
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_RwResources()
{
    memset(&ms_Stats, 0, sizeof(ms_Stats));

    EZHookInstall(RwTextureCreate);
    EZHookInstall(RwTextureDestroy);
    EZHookInstall(RwRasterCreate);
    EZHookInstall(RwRasterDestroy);
    EZHookInstall(RwGeometryCreate);
    EZHookInstall(RwGeometryDestroy);
}
