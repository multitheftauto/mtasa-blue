/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CEventList.h
 *  PURPOSE:     Event list interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CEntity;
class CEventDamage;
class CEventDamageSAInterface;
class CEventGunShot;
class CVector;

class CEventList
{
public:
    virtual CEventDamage* GetEventDamage(CEventDamageSAInterface* pInterface) = 0;

    virtual CEventDamage*  CreateEventDamage(CEntity* pEntity, unsigned int i_1, eWeaponType weaponType, ePedPieceTypes hitZone, unsigned char uc_2, bool b_3,
                                             bool b_4) = 0;
    virtual CEventGunShot* CreateEventGunShot(CEntity* pEntity, CVector& vecOrigin, CVector& vecTarget, bool b_1) = 0;
};
