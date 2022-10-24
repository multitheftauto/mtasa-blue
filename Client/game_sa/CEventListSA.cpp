/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEventListSA.cpp
 *  PURPOSE:     Event list
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEventListSA.h"
#include "CEventDamageSA.h"
#include "CEventGunShotSA.h"

CEventDamage* CEventListSA::GetEventDamage(CEventDamageSAInterface* pInterface)
{
    return new CEventDamageSA(pInterface);
}

CEventDamage* CEventListSA::CreateEventDamage(CEntity* pEntity, unsigned int i_1, eWeaponType weaponType, ePedPieceTypes hitZone, unsigned char uc_2, bool b_3,
                                              bool b_4)
{
    return new CEventDamageSA(pEntity, i_1, weaponType, hitZone, uc_2, b_3, b_4);
}

CEventGunShot* CEventListSA::CreateEventGunShot(CEntity* pEntity, CVector& vecOrigin, CVector& vecTarget, bool b_1)
{
    return new CEventGunShotSA(pEntity, vecOrigin, vecTarget, b_1);
}
