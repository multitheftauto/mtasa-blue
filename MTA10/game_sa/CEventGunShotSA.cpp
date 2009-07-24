/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CEvenGunShotSA.cpp
*  PURPOSE:		Gunshot event
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CEventGunShotSA::CEventGunShotSA ( CEntity * pEntity, CVector & vecOrigin, CVector & vecTarget, bool b_1 )
{
    m_pInterface = new CEventGunShotSAInterface;

    DWORD dwEntityInterface = 0;
    if ( pEntity ) dwEntityInterface = (DWORD)pEntity->GetInterface ();
    float originX = vecOrigin.fX, originY = vecOrigin.fY, originZ = vecOrigin.fZ;
    float targetX = vecTarget.fX, targetY = vecTarget.fY, targetZ = vecTarget.fZ;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CEventGunShot_Constructor;
    _asm
    {
        mov     ecx, dwThis
        push    b_1
        push    targetZ
        push    targetY
        push    targetX
        push    originZ
        push    originY
        push    originX
        push    dwEntityInterface
        call    dwFunc
    }
}

void CEventGunShotSA::Destroy ( bool bDestroyInterface )
{
    if ( bDestroyInterface )
    {
        DWORD dwThis = (DWORD)m_pInterface;
        DWORD dwFunc = FUNC_CEventGunShot_Destructor;
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
        delete m_pInterface;
    }
    delete this;
}