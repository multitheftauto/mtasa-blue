/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedDamageResponseSA.cpp
 *  PURPOSE:     ped damage response
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CPedDamageResponseCalculatorSA::CPedDamageResponseCalculatorSA(CEntity* pEntity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool b_1)
{
    m_pInterface = new CPedDamageResponseCalculatorSAInterface;
    m_bDestroyInterface = true;

    // CPedDamageResponseCalculator::CPedDamageResponseCalculator
    ((CPedDamageResponseCalculatorSAInterface*
      (__thiscall*)(CPedDamageResponseCalculatorSAInterface*, CEntitySAInterface*, float, eWeaponType, ePedPieceTypes, bool))
         FUNC_CPedDamageResponseCalculator_Constructor)(m_pInterface, pEntity->GetInterface(), fDamage, weaponType, bodyPart, b_1);
}

CPedDamageResponseCalculatorSA::CPedDamageResponseCalculatorSA(CPedDamageResponseCalculatorSAInterface* pInterface)
{
    m_pInterface = pInterface;
    m_bDestroyInterface = false;
}

CPedDamageResponseCalculatorSA::~CPedDamageResponseCalculatorSA()
{
    if (m_bDestroyInterface)
    {
        // CPedDamageResponseCalculator::~CPedDamageResponseCalculator
        ((void(__thiscall*)(CPedDamageResponseCalculatorSAInterface*))FUNC_CPedDamageResponseCalculator_Destructor)(m_pInterface);

        delete m_pInterface;
    }
}

void CPedDamageResponseCalculatorSA::ComputeDamageResponse(CPed* pPed, CPedDamageResponse* pDamageResponse, bool bSpeak)
{
    // CPedDamageResponseCalculator::ComputeDamageResponse
    ((void(__thiscall*)(CPedDamageResponseCalculatorSAInterface*, CPedSAInterface*, CPedDamageResponseSAInterface*,
                        bool))FUNC_CPedDamageResponseCalculator_ComputeDamageResponse)(m_pInterface, pPed->GetPedInterface(), pDamageResponse->GetInterface(),
                                                                                       bSpeak);
}
