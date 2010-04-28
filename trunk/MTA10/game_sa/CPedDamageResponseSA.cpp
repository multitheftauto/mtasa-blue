/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedDamageResponseCalculatorSA.cpp
*  PURPOSE:     ped damage response calculator
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


void CPedDamageResponseSA::Calculate ( CEntity * pEntity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool b_1, bool bSpeak )
{
    CPedDamageResponseCalculatorSA calc ( pEntity, fDamage, weaponType, bodyPart, b_1 );
    calc.ComputeDamageResponse ( dynamic_cast < CPed * > ( pEntity ), this, bSpeak );
}