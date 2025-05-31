/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPedDamageResponseSA.h
 *  PURPOSE:     Header file for ped damage response
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPedDamageResponse.h>

class CPedDamageResponseSAInterface
{
public:
    float fDamageHealth;
    float fDamageArmor;
    bool  bUnk;
    bool  bForceDeath;
    bool  bDamageCalculated;
    bool  bUnk3;
};

class CPedDamageResponseSA : public CPedDamageResponse
{
public:
    CPedDamageResponseSA(CPedDamageResponseSAInterface* pInterface) { m_pInterface = pInterface; }

    CPedDamageResponseSAInterface* GetInterface() { return m_pInterface; }

    void Calculate(CEntity* pEntity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool b_1, bool bSpeak);

private:
    CPedDamageResponseSAInterface* m_pInterface;
};
