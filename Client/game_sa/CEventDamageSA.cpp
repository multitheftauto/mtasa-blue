/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEventDamageSA.cpp
 *  PURPOSE:     Damage event
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CEventDamageSA::CEventDamageSA(CEntity* pEntity, unsigned int i_1, eWeaponType weaponType, ePedPieceTypes hitZone, unsigned char uc_2, bool b_3, bool b_4)
{
    m_pInterface = new CEventDamageSAInterface;
    m_pDamageResponse = new CPedDamageResponseSA(&m_pInterface->damageResponseData);
    m_bDestroyInterface = true;
    m_DamageReason = EDamageReason::OTHER;

    // CEventDamage::CEventDamage
    ((CEventDamageSAInterface*(__thiscall*)(CEventDamageSAInterface*, CEntitySAInterface*, unsigned int, eWeaponType, ePedPieceTypes, unsigned char, bool, bool))
         FUNC_CEventDamage_Constructor)(m_pInterface, pEntity->GetInterface(), i_1, weaponType, hitZone, uc_2, b_3, b_4);
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
        // CEventDamage::~CEventDamage
        ((void(__thiscall*)(CEventDamageSAInterface*))FUNC_CEventDamage_Destructor)(m_pInterface);

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
    // CEventDamage::HasKilledPed
    return ((bool(__thiscall*)(CEventDamageSAInterface*))FUNC_CEventDamage_HasKilledPed)(m_pInterface);
}

float CEventDamageSA::GetDamageApplied()
{
    // CEventDamage::GetDamageApplied
    return ((float(__thiscall*)(CEventDamageSAInterface*))FUNC_CEventDamage_GetDamageApplied)(m_pInterface);
}

AssocGroupId CEventDamageSA::GetAnimGroup()
{
    // CEventDamage::GetAnimGroup
    return ((AssocGroupId(__thiscall*)(CEventDamageSAInterface*))FUNC_CEventDamage_GetAnimGroup)(m_pInterface);
}

AnimationId CEventDamageSA::GetAnimId()
{
    // CEventDamage::GetAnimGroup
    return ((AnimationId(__thiscall*)(CEventDamageSAInterface*))FUNC_CEventDamage_GetAnimId)(m_pInterface);
}

bool CEventDamageSA::GetAnimAdded()
{
    // CEventDamage::GetAnimAdded
    return ((bool(__thiscall*)(CEventDamageSAInterface*))FUNC_CEventDamage_GetAnimAdded)(m_pInterface);
}

void CEventDamageSA::ComputeDeathAnim(CPed* pPed, bool bUnk)
{
    // CEventDamage::ComputeDeathAnim
    ((void(__thiscall*)(CEventDamageSAInterface*, CEntitySAInterface*, bool))FUNC_CEventDamage_ComputeDeathAnim)(m_pInterface, pPed->GetInterface(), bUnk);
}

void CEventDamageSA::ComputeDamageAnim(CPed* pPed, bool bUnk)
{
    // CEventDamage::ComputeDamageAnim
    ((void(__thiscall*)(CEventDamageSAInterface*, CEntitySAInterface*, bool))FUNC_CEventDamage_ComputeDamageAnim)(m_pInterface, pPed->GetInterface(), bUnk);
}

bool CEventDamageSA::AffectsPed(CPed* pPed)
{
    // CEventDamage::AffectsPed
    return ((bool(__thiscall*)(CEventDamageSAInterface*, CEntitySAInterface*))FUNC_CEventDamage_AffectsPed)(m_pInterface, pPed->GetInterface());
}
