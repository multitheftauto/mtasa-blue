/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CPedDamageResponseCalculatorSA.cpp
 *  PURPOSE:     ped damage response calculator
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPedDamageResponseSA.h"
#include "CPedDamageResponseCalculatorSA.h"
#include "CPedSA.h"

void CPedDamageResponseSA::Calculate(CEntity* pEntity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool b_1, bool bSpeak)
{
    CPedDamageResponseCalculatorSA calc(pEntity, fDamage, weaponType, bodyPart, b_1);
    calc.ComputeDamageResponse(dynamic_cast<CPed*>(pEntity), this, bSpeak);
}
