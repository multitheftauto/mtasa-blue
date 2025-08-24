/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CWaterManager.h
 *  PURPOSE:     Water entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CWater.h"
#include "packets/CMapInfoPacket.h"

class CWaterManager
{
    friend class CWater;

public:
    CWaterManager();
    ~CWaterManager();

    CWater* Create(CWater::EWaterType waterType, CElement* pParent, bool bShallow = false);
    CWater* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void    DeleteAll();

    unsigned int Count() { return static_cast<unsigned int>(m_List.size()); };
    bool         Exists(CWater* pWater);

    float GetGlobalWaveHeight() const { return m_fGlobalWaveHeight; }
    void  SetGlobalWaveHeight(float fHeight) { m_fGlobalWaveHeight = fHeight; }

    const SWorldWaterLevelInfo& GetWorldWaterLevelInfo() const { return m_WorldWaterLevelInfo; }
    void                        SetWorldWaterLevel(float fLevel, bool bIncludeWorldNonSeaLevel, bool bIncludeWorldSeaLevel, bool bIncludeOutsideWorldLevel);
    void                        ResetWorldWaterLevel();
    void                        SetElementWaterLevel(CWater* pWater, float fLevel);
    void                        SetAllElementWaterLevel(float fLevel);

    std::list<CWater*>::const_iterator IterBegin() { return m_List.begin(); }
    std::list<CWater*>::const_iterator IterEnd() { return m_List.end(); }

protected:
    void AddToList(CWater* pWater) { m_List.push_back(pWater); }
    void RemoveFromList(CWater* pWater);

    std::list<CWater*> m_List;

    SWorldWaterLevelInfo m_WorldWaterLevelInfo;
    float                m_fGlobalWaveHeight;
};
