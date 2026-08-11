/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEventDamageSA.cpp
 *  PURPOSE:     Damage event
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
    DWORD dwFunc = FUNC_CEventDamage_Constructor;
    // clang-format off
    __asm
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
    // clang-format on
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
        DWORD dwFunc = FUNC_CEventDamage_Destructor;
        // clang-format off
        __asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
        // clang-format on
        delete m_pInterface;
    }
}

CEntity* CEventDamageSA::GetInflictingEntity()
{
    CEntity*            pReturn = NULL;
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
    DWORD dwFunc = FUNC_CEventDamage_HasKilledPed;
    bool  bReturn = false;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    // clang-format on
    return bReturn;
}

float CEventDamageSA::GetDamageApplied()
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CEventDamage_GetDamageApplied;
    float fReturn = 0.0f;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fReturn
    }
    // clang-format on
    return fReturn;
}

AssocGroupId CEventDamageSA::GetAnimGroup()
{
    DWORD        dwThis = (DWORD)m_pInterface;
    DWORD        dwFunc = FUNC_CEventDamage_GetAnimGroup;
    AssocGroupId animGroup = 0;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     animGroup, eax
    }
    // clang-format on
    return animGroup;
}

AnimationId CEventDamageSA::GetAnimId()
{
    DWORD       dwThis = (DWORD)m_pInterface;
    DWORD       dwFunc = FUNC_CEventDamage_GetAnimId;
    AnimationId animID = 0;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     animID, eax
    }
    // clang-format on
    return animID;
}

bool CEventDamageSA::GetAnimAdded()
{
    bool  bReturn;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CEventDamage_GetAnimAdded;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    // clang-format on
    return bReturn;
}

void CEventDamageSA::ComputeDeathAnim(CPed* pPed, bool bUnk)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwPed = (DWORD)pPed->GetInterface();
    DWORD dwFunc = FUNC_CEventDamage_ComputeDeathAnim;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    bUnk
        push    dwPed
        call    dwFunc
    }
    // clang-format on
}

void CEventDamageSA::ComputeDamageAnim(CPed* pPed, bool bUnk)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwPed = (DWORD)pPed->GetInterface();
    DWORD dwFunc = FUNC_CEventDamage_ComputeDamageAnim;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    bUnk
        push    dwPed
        call    dwFunc
    }
    // clang-format on
}

bool CEventDamageSA::AffectsPed(CPed* pPed)
{
    bool  bReturn;
    DWORD dwPedInterface = (DWORD)pPed->GetInterface();
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CEventDamage_AffectsPed;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    dwPedInterface
        call    dwFunc
        mov     bReturn, al
    }
    // clang-format on
    return bReturn;
}
