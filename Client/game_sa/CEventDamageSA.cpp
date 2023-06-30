/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CEventDamageSA.cpp
 *  PURPOSE:     Damage event
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEntitySA.h"
#include "CEventDamageSA.h"
#include "CGameSA.h"
#include "CPoolsSA.h"

extern CGameSA* pGame;

CEventDamageSA::CEventDamageSA(CEntity* pEntity, unsigned int i_1, eWeaponType weaponType, ePedPieceTypes hitZone, unsigned char uc_2, bool b_3, bool b_4)
{
    m_pInterface = new CEventDamageSAInterface;
    m_pDamageResponse = new CPedDamageResponseSA(&m_pInterface->damageResponseData);
    m_bDestroyInterface = true;
    m_DamageReason = EDamageReason::OTHER;

    DWORD dwEntityInterface = (DWORD)pEntity->GetInterface();
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x4AD830;
    _asm
    {
        mov     ecx, dwThis
        push    b_4
        push    b_3
        push    uc_2
        push    hitZone
        push    weaponType
        push    i_1
        push    dwEntityInterface
        call    dwFunc
    }
}

CEventDamageSA::CEventDamageSA(CEventDamageSAInterface* pInterface)
{
    m_pInterface = pInterface;
    m_pDamageResponse = new CPedDamageResponseSA(&m_pInterface->damageResponseData);
    m_bDestroyInterface = false;
    m_DamageReason = EDamageReason::OTHER;
}

CEventDamageSA::~CEventDamageSA()
{
    delete m_pDamageResponse;

    if (m_bDestroyInterface)
    {
        DWORD dwThis = (DWORD)m_pInterface;
        DWORD dwFunc = 0x4AD960;
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
        delete m_pInterface;
    }
}

CEntity* CEventDamageSA::GetInflictingEntity()
{
    CEntity*            pReturn = nullptr;
    CEntitySAInterface* pInterface = m_pInterface->pInflictor;
    if (pInterface)
    {
        CPools* pPools = pGame->GetPools();
        return pPools->GetEntity((DWORD*)pInterface);
    }
    return pReturn;
}

bool CEventDamageSA::HasKilledPed()
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x4ABCA0;
    bool  bReturn = false;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

float CEventDamageSA::GetDamageApplied()
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x4ABCE0;
    float fReturn = 0.0f;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fReturn
    }
    return fReturn;
}

AssocGroupId CEventDamageSA::GetAnimGroup()
{
    DWORD        dwThis = (DWORD)m_pInterface;
    DWORD        dwFunc = 0x4B8060;
    AssocGroupId animGroup = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     animGroup, eax
    }
    return animGroup;
}

AnimationId CEventDamageSA::GetAnimId()
{
    DWORD       dwThis = (DWORD)m_pInterface;
    DWORD       dwFunc = 0x4B8070;
    AnimationId animID = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     animID, eax
    }
    return animID;
}

bool CEventDamageSA::GetAnimAdded()
{
    bool  bReturn = false;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x4B80A0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CEventDamageSA::ComputeDeathAnim(CPed* pPed, bool bUnk)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwPed = (DWORD)pPed->GetInterface();
    DWORD dwFunc = 0x4B3A60;
    _asm
    {
        mov     ecx, dwThis
        push    bUnk
        push    dwPed
        call    dwFunc
    }
}

void CEventDamageSA::ComputeDamageAnim(CPed* pPed, bool bUnk)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwPed = (DWORD)pPed->GetInterface();
    DWORD dwFunc = 0x4B3FC0;
    _asm
    {
        mov     ecx, dwThis
        push    bUnk
        push    dwPed
        call    dwFunc
    }
}

bool CEventDamageSA::AffectsPed(CPed* pPed)
{
    bool  bReturn = false;
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = 0x4D35A0;
    _asm
    {
        mov     ecx, dwThis
        push    dwPedInterface
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}
