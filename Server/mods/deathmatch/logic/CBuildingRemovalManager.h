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

class CBuildingRemoval;

class CBuildingRemovalManager
{
public:
    CBuildingRemovalManager();
    ~CBuildingRemovalManager();
    void CreateBuildingRemoval(std::uint16_t usModel, float fRadius, const CVector& vecPos, char cInterior);
    void ClearBuildingRemovals();
    void RestoreWorldModel(std::uint16_t usModel, float fRadius, const CVector& vecPos, char cInterior);

    std::multimap<std::uint16_t, CBuildingRemoval*>::const_iterator IterBegin() { return m_BuildingRemovals.begin(); };
    std::multimap<std::uint16_t, CBuildingRemoval*>::const_iterator IterEnd() { return m_BuildingRemovals.end(); };

private:
    std::multimap<std::uint16_t, CBuildingRemoval*> m_BuildingRemovals;
};
