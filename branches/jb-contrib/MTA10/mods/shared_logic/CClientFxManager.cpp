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

#include "StdInc.h"
#include "../../game_sa/CFxManagerSA.h"
#include <CMatrix_Pad.h>

CClientFxManager::~CClientFxManager()
{
    // Clear FxSystemBP List
    {
        std::list<CClientFxSystemBP*>::iterator it = m_FxSystemBPList.begin();
        while(it != m_FxSystemBPList.end())
        {
            if(*it)
            {
                delete *it;
                it = m_FxSystemBPList.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    // Clear FxSystem List
    {
        std::list<CClientFxSystem*>::iterator it = m_FxSystemList.begin();
        while(it != m_FxSystemList.end())
        {
            if(*it)
            {
                delete *it;
                it = m_FxSystemList.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

CClientFxSystemBP* CClientFxManager::FindFxSystemBlueprint(const SString& strName)
{
    std::list<CClientFxSystemBP*>::iterator it = m_FxSystemBPList.begin();
    for(; it != m_FxSystemBPList.end(); ++it)
    {
        if((*it)->GetName() == strName)
        {
            return *it;
        }
    }
    return NULL;
}

CClientFxSystem* CClientFxManager::CreateFxSystem(const SString& strName, CVector& vecPoint, CMatrix& matMatrix, bool bUnknown)
{
    CClientFxSystemBP* pFxSystemBP = FindFxSystemBlueprint(strName);
    return InitialiseFxSystem(pFxSystemBP, vecPoint, matMatrix, bUnknown);
}

CClientFxSystem* CClientFxManager::CreateFxSystem(const SString& strName, CMatrix& matMatrix1, CMatrix& matMatrix2, bool bUnknown)
{
    CClientFxSystemBP* pFxSystemBP = FindFxSystemBlueprint(strName);
    return InitialiseFxSystem(pFxSystemBP, matMatrix1, matMatrix2, bUnknown);
}

CClientFxSystem* CClientFxManager::InitialiseFxSystem(CClientFxSystemBP* pFxSystemBP, CVector& vecPoint, CMatrix& matMatrix, bool bUnknown)
{
    CMatrix_Padded gtaMat;
    gtaMat.SetFromMatrix(matMatrix);
    CFxSystemSA* pFxSystem = m_pFxManager->InitialiseFxSystem(pFxSystemBP->m_pFxSystemBP, vecPoint, reinterpret_cast<RwMatrix*>(&gtaMat), bUnknown);
    if(pFxSystem)
    {
        CClientFxSystem
            *pClientFxSystem = new CClientFxSystem(pFxSystem);
        if(pClientFxSystem)
        {
            m_FxSystemList.push_back(pClientFxSystem);
        }
        return pClientFxSystem;
    }
    return NULL;
}

CClientFxSystem* CClientFxManager::InitialiseFxSystem(CClientFxSystemBP* pFxSystemBP, CMatrix& matMatrix1, CMatrix& matMatrix2, bool bUnknown)
{
    CMatrix_Padded gtaMat1;
    CMatrix_Padded gtaMat2;
    gtaMat1.SetFromMatrix(matMatrix1);
    gtaMat2.SetFromMatrix(matMatrix2);
    CFxSystemSA* pFxSystem = m_pFxManager->InitialiseFxSystem(pFxSystemBP->m_pFxSystemBP, reinterpret_cast<RwMatrix*>(&gtaMat1), reinterpret_cast<RwMatrix*>(&gtaMat2), bUnknown);
    if(pFxSystem)
    {
        CClientFxSystem
            *pClientFxSystem = new CClientFxSystem(pFxSystem);
        if(pClientFxSystem)
        {
            m_FxSystemList.push_back(pClientFxSystem);
        }
        return pClientFxSystem;
    }
    return NULL;
}

void CClientFxManager::DestroyFxSystem(CClientFxSystem* pFxSystem)
{
    if(pFxSystem)
    {
        if(m_FxSystemList.Contains(pFxSystem))
        {
            m_pFxManager->DestroyFxSystem(pFxSystem->m_pFxSystem);
            std::list<CClientFxSystem*>::iterator ptr = m_FxSystemList.begin();
            while(ptr != m_FxSystemList.end())
            {
                if(*ptr == pFxSystem)
                {
                    ptr = m_FxSystemList.erase(ptr);
                    delete pFxSystem;
                }
                else
                {
                    ptr++;
                }
            }
        }
    }
}

void CClientFxManager::SetWindData(const CVector& vecWindDir, float fWindStrength)
{
    m_pFxManager->SetWindData(vecWindDir, fWindStrength);
    m_vecWindDirection = vecWindDir;
    m_fWindStrength = fWindStrength;
}
