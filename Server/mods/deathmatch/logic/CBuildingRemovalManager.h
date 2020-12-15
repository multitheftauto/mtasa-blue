/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBuildingRemovalManager.h
 *  PURPOSE:     Header file for building removal.
 *
 *****************************************************************************/

#pragma once

#include <map>

class CBuildingRemovalManager
{
public:
    CBuildingRemovalManager();
    ~CBuildingRemovalManager();
    void CreateBuildingRemoval(uint32 usModel, float fRadius, const CVector& vecPos, char cInterior);
    void ClearBuildingRemovals();
    void RestoreWorldModel(uint32 usModel, float fRadius, const CVector& vecPos, char cInterior);
    multimap<uint32, CBuildingRemoval*>::const_iterator         IterBegin() { return m_BuildingRemovals.begin(); };
    multimap<uint32, CBuildingRemoval*>::const_iterator IterEnd() { return m_BuildingRemovals.end(); };

private:
    std::multimap<uint32, CBuildingRemoval*> m_BuildingRemovals;
};
