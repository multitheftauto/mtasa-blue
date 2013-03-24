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
#include "CFxSystemSA.h"

CFxManagerSA::CFxManagerSA(CFxManagerSAInterface * pInterface)
{
    m_pInterface = pInterface;
}

void CFxManagerSA::SetWindData(const CVector& vecWindDirection, float fWindStrength)
{
    uint32 dwFunc = (uint32)FUNC_CFxManager__SetWindData;
    const CVector * pVecWindDir = &vecWindDirection;
    __asm
    {
        push fWindStrength
        push pVecWindDir
        mov ecx, m_pInterface
        call dwFunc
    }
}

void CFxManagerSA::DestroyFxSystem(CFxSystemSA* pFxSystem)
{
    uint32 dwFunc = (uint32)FUNC_CFxManager__DestroyFxSystem;
    __asm
    {
        push pFxSystem
        mov ecx, m_pInterface
        call dwFunc
    }
}

CFxSystemSA* CFxManagerSA::InitialiseFxSystem(CFxSystemBPSA* pFxSystemBP, CVector& vecPos, RwMatrix* pMatrix, bool bUnknown)
{
    const CVector * pVecPos = &vecPos;
    uint32 dwFunc = (uint32)FUNC_CFxManager__InitialiseFxSystem;
    class CFxSystemSAInterface* pRet = NULL;
    __asm
    {
        push bUnknown
        push pMatrix
        push pVecPos
        push pFxSystemBP
        mov ecx, m_pInterface
        call dwFunc
        mov pRet, eax
    }
    if(pRet)
    {
        return new CFxSystemSA(pRet);
    }
    return NULL;
}

CFxSystemSA* CFxManagerSA::InitialiseFxSystem(CFxSystemBPSA* pFxSystemBP, RwMatrix* pMatrix1, RwMatrix* pMatrix2, bool bUnknown)
{
    uint32 dwFunc = (uint32)FUNC_CFxManager__InitialiseFxSystem2;
    CFxSystemSAInterface* pRet = NULL;
    __asm
    {
        push bUnknown
        push pMatrix2
        push pMatrix1
        push pFxSystemBP
        mov ecx, m_pInterface
        call dwFunc
        mov pRet, eax
    }
    if(pRet)
    {
        return new CFxSystemSA(pRet);
    }
    return NULL;
}