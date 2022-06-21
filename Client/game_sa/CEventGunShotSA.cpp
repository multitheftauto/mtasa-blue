/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEvenGunShotSA.cpp
 *  PURPOSE:     Gunshot event
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CEventGunShotSA::CEventGunShotSA(CEntity* pEntity, CVector& vecOrigin, CVector& vecTarget, bool b_1)
{
    m_pInterface = new CEventGunShotSAInterface;

    // CEventGunShot::CEventGunShot
    ((CEventGunShotSAInterface*(__thiscall*)(CEventGunShotSAInterface*, CEntitySAInterface*, float, float, float, float, float, float, bool))
         FUNC_CEventGunShot_Constructor)(m_pInterface, pEntity ? pEntity->GetInterface() : nullptr, vecOrigin.fX, vecOrigin.fY, vecOrigin.fZ, vecTarget.fX, vecTarget.fY,
                                         vecTarget.fZ, b_1);
}

void CEventGunShotSA::Destroy(bool bDestroyInterface)
{
    if (bDestroyInterface)
    {
        // CEventGunShot::~CEventGunShot
        ((CEventGunShotSAInterface*(__thiscall*)(CEventGunShotSAInterface*))FUNC_CEventGunShot_Destructor)(m_pInterface);

        delete m_pInterface;
    }
    delete this;
}
