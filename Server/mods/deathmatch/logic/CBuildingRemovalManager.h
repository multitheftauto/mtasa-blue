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
    CBuildingRemovalManager(void);
    ~CBuildingRemovalManager(void);
    void CreateBuildingRemoval(unsigned short usModel, float fRadius, const CVector& vecPos, char cInterior);
    void ClearBuildingRemovals(void);
    void RestoreWorldModel(unsigned short usModel, float fRadius, const CVector& vecPos, char cInterior);
    multimap<unsigned short, CBuildingRemoval*>::const_iterator IterBegin(void) { return m_BuildingRemovals.begin(); };
    multimap<unsigned short, CBuildingRemoval*>::const_iterator IterEnd(void) { return m_BuildingRemovals.end(); };

private:
    std::multimap<unsigned short, CBuildingRemoval*> m_BuildingRemovals;
};
