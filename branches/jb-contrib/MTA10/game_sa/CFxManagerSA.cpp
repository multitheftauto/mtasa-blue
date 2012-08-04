/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleSystemManagerSA.h
*  PURPOSE:     Header file for particle system manager class
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CFxManagerSA::CFxManagerSA(CFxManagerSAInterface * pInterface)
{
    m_pInterface = pInterface;
}

CFxSystem* CFxManagerSA::CreateFxSystem(const SString & strName, CVector & vecPoint, RwMatrix * pMatrix, bool bUnk)
{
    uint32 dwFunc = (uint32)FUNC_CFxManager__CreateFxSystem;
    CVector * pVecPoint = &vecPoint;
    const char * pSzName = strName.data();
    __asm
    {
        push bUnk
        push pMatrix
        push pVecPoint
        push pSzName
        call dwFunc
    }
}
