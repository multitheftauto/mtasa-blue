/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CFx.h
*  PURPOSE:     Game effects interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

class CFxSystemSA;
class CFxSystemBPSA;

class CFxManager
{
public:
    virtual void SetWindData(const CVector& vecWindDirection, float fWindStrength) = 0;
    virtual void DestroyFxSystem(CFxSystemSA* pFxSystem) = 0;
    virtual CFxSystemSA* InitialiseFxSystem(CFxSystemBPSA* pFxSystemBP, CVector& vecPos, RwMatrix* pMatrix, bool bUnknown) = 0;
    virtual CFxSystemSA* InitialiseFxSystem(CFxSystemBPSA* pFxSystemBP, RwMatrix* pMatrix1, RwMatrix* pMatrix2, bool bUnknown) = 0;
};