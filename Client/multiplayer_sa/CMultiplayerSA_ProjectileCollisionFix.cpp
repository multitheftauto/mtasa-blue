/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_ProjectileCollisionFix.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "../game_sa/CColModelSA.h"

static CColModelSAInterface colModelGrenade;

static void InitializeGrenadeColModel()
{
    // CColModel::CColModel
    ((CColModelSAInterface*(__thiscall*)(CColModelSAInterface*))(0x40FB60))(&colModelGrenade);

    colModelGrenade.m_bounds.m_vecMin = CVector(-0.25f, -0.25f, -0.25f);
    colModelGrenade.m_bounds.m_vecMax = CVector(0.25f, 0.25f, 0.25f);

    colModelGrenade.m_sphere.m_center = CVector(0, 0, 0);
    colModelGrenade.m_sphere.m_radius = 0.25f;

    // CColModel::AllocateData
    ((void(__thiscall*)(CColModelSAInterface*, int, int, int, int, int, int))(0x40F870))(&colModelGrenade, 1, 0, 0, 0, 0, 0);

    // CColSphere::Set
    ((void(__thiscall*)(CColSphereSA*, float, CVector&, unsigned char, char, unsigned char))(0x40FD10))(
        &colModelGrenade.m_data->m_spheres[0],
        colModelGrenade.m_sphere.m_radius * 0.75f,
        colModelGrenade.m_sphere.m_center,
        56, 0, 255);
}

#define HOOKPOS_CTempColModels__Initialise 0x5BB87D
#define HOOKSIZE_CTempColModels__Initialise 0x6
static const unsigned int RETURN_CTempColModels__Initialise = 0x5BB883;
static void _declspec(naked) HOOK_CTempColModels__Initialise()
{
    _asm {
        mov ds:[0x968FE4], edx

        call InitializeGrenadeColModel

        jmp RETURN_CTempColModels__Initialise
    }
}

#define HOOKPOS_CFileLoader__LoadWeaponObject 0x5B401E
#define HOOKSIZE_CFileLoader__LoadWeaponObject 0x5
static const unsigned int RETURN_CFileLoader__LoadWeaponObject = 0x5B4023;
static void _declspec(naked) HOOK_CFileLoader__LoadWeaponObject()
{
    _asm {
        mov eax, [esp+0x8]
        cmp eax, 342
        je grenade
        cmp eax, 343
        je grenade
        cmp eax, 344
        je grenade
        cmp eax, 363
        je grenade

        push 0x968FD0
        jmp RETURN_CFileLoader__LoadWeaponObject

    grenade:
        push offset colModelGrenade
        jmp RETURN_CFileLoader__LoadWeaponObject
    }
}

void CMultiplayerSA::InitHooks_ProjectileCollisionFix()
{
    EZHookInstall(CTempColModels__Initialise);
    EZHookInstall(CFileLoader__LoadWeaponObject);

    MemCpy((void*)0x7383FF, "\xE9\x5E\x01\x00\x00", 5);
    MemSet((void*)0x738404, 0x90, 0x9C);
}
