/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CClientFxManager.cpp    
*  PURPOSE:
*  DEVELOPERS:  Nobody knows
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CClientCommon.h"
#include "CClientEntity.h"

class CClientFxSystem;
class CClientFxSystemBP;
class CFxManagerSA;
class CFxSA;

class CClientFxManager
{
public:
    ~CClientFxManager();

    CClientFxSystem* CreateFxSystem(const SString& strName, CVector& vecPoint, CMatrix& matMatrix, bool bUnknown);
    CClientFxSystem* CreateFxSystem(const SString& strName, CMatrix& matMatrix1, CMatrix& matMatrix2, bool bUnknown);
    CClientFxSystemBP* FindFxSystemBlueprint(const SString& strName);
    CClientFxSystem* InitialiseFxSystem(CClientFxSystemBP* pFxSystemBP, CVector& vecPoint, CMatrix& matMatrix, bool bUnknown);
    CClientFxSystem* InitialiseFxSystem(CClientFxSystemBP* pFxSystemBP, CMatrix& matMatrix1, CMatrix& matMatrix2, bool bUnknown);
    void DestroyFxSystem(CClientFxSystem* pFxSystem);
    void SetWindData(const CVector& vecWindDir, float fWindStrength);
protected:
private:
    CMappedList<CClientFxSystemBP*>     m_FxSystemBPList;
    CMappedList<CClientFxSystem*>       m_FxSystemList;
    uint32                              m_txdIndex;
    CVector                             m_vecWindDirection;
    float                               m_fWindStrength;
    /*
    CFxFrustumInfoSA*                   m_frustumInfo;
    uint32                              m_currentMatrix;
    RwMatrix*                           m_matrices[8];
    SimpleBuffer                        m_buffer;
    */

    CFxManagerSA*                       m_pFxManager;
    CFxSA*                              m_pFx;
};