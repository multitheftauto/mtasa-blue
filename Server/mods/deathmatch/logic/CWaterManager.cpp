/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CWaterManager.cpp
 *  PURPOSE:     Water entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWaterManager.h"
#include "CWater.h"
#include "Utils.h"

CWaterManager::CWaterManager()
{
    m_WorldWaterLevelInfo.bNonSeaLevelSet = false;
    m_WorldWaterLevelInfo.bOutsideLevelSet = false;
    m_WorldWaterLevelInfo.fSeaLevel = 0;
    m_WorldWaterLevelInfo.fNonSeaLevel = 0;
    m_WorldWaterLevelInfo.fOutsideLevel = 0;
    m_fGlobalWaveHeight = 0.0f;
}

CWaterManager::~CWaterManager()
{
    DeleteAll();
}

CWater* CWaterManager::Create(CWater::EWaterType waterType, CElement* pParent, bool bShallow)
{
    CWater* const pWater = new CWater(this, pParent, waterType, bShallow);

    if (pWater->GetID() == INVALID_ELEMENT_ID)
    {
        delete pWater;
        return nullptr;
    }

    return pWater;
}

CWater* CWaterManager::CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents)
{
    CWater* const pWater = new CWater(this, pParent);

    if (pWater->GetID() == INVALID_ELEMENT_ID || !pWater->LoadFromCustomData(pEvents, Node))
    {
        delete pWater;
        return nullptr;
    }

    return pWater;
}

void CWaterManager::SetElementWaterLevel(CWater* pWater, float fLevel)
{
    pWater->SetLevel(fLevel);
}

void CWaterManager::SetAllElementWaterLevel(float fLevel)
{
    std::list<CWater*>::const_iterator iter = m_List.begin();
    for (; iter != m_List.end(); ++iter)
    {
        SetElementWaterLevel(*iter, fLevel);
    }
}

void CWaterManager::SetWorldWaterLevel(float fLevel, bool bIncludeWorldNonSeaLevel, bool bIncludeWorldSeaLevel, bool bIncludeOutsideWorldLevel)
{
    if (bIncludeWorldSeaLevel)
    {
        m_WorldWaterLevelInfo.fSeaLevel = fLevel;
    }
    if (bIncludeWorldNonSeaLevel)
    {
        m_WorldWaterLevelInfo.bNonSeaLevelSet = true;
        m_WorldWaterLevelInfo.fNonSeaLevel = fLevel;
    }
    if (bIncludeOutsideWorldLevel)
    {
        m_WorldWaterLevelInfo.bOutsideLevelSet = true;
        m_WorldWaterLevelInfo.fOutsideLevel = fLevel;
    }
}

void CWaterManager::ResetWorldWaterLevel()
{
    m_WorldWaterLevelInfo.bNonSeaLevelSet = false;
    m_WorldWaterLevelInfo.bOutsideLevelSet = false;
    m_WorldWaterLevelInfo.fSeaLevel = 0;
    m_WorldWaterLevelInfo.fNonSeaLevel = 0;
    m_WorldWaterLevelInfo.fOutsideLevel = 0;
}

void CWaterManager::DeleteAll()
{
    // Delete all items
    DeletePointersAndClearList(m_List);
}

void CWaterManager::RemoveFromList(CWater* pWater)
{
    m_List.remove(pWater);
}

bool CWaterManager::Exists(CWater* pWater)
{
    return ListContains(m_List, pWater);
}
