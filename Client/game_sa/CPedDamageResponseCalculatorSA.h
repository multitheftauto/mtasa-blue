/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedDamageResponseCalculatorSA.h
 *  PURPOSE:     Header file for ped damage response calculator
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPedDamageResponseCalculator.h>
#include <game/CPed.h>

class CEntity;
class CEntitySAInterface;
class CPed;
class CPedDamageResponse;

#define FUNC_CPedDamageResponseCalculator_Constructor               0x4ad3f0
#define FUNC_CPedDamageResponseCalculator_Destructor                0x4ad420
#define FUNC_CPedDamageResponseCalculator_ComputeDamageResponse     0x4b5ac0

class CPedDamageResponseCalculatorSAInterface
{
public:
    CEntitySAInterface* pEntity;
    float               fDamage;
    ePedPieceTypes      bodyPart;
    eWeaponType         weaponType;
    bool                bSpeak;            // refers to a CPed::Say call (the dying scream?)
};

class CPedDamageResponseCalculatorSA : public CPedDamageResponseCalculator
{
public:
    CPedDamageResponseCalculatorSA(CEntity* pEntity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool b_1);
    CPedDamageResponseCalculatorSA(CPedDamageResponseCalculatorSAInterface* pInterface);
    ~CPedDamageResponseCalculatorSA();

    void ComputeDamageResponse(CPed* pPed, CPedDamageResponse* pDamageResponse, bool bSpeak);

private:
    CPedDamageResponseCalculatorSAInterface* m_pInterface;
    bool                                     m_bDestroyInterface;
};
