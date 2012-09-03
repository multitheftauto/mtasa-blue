/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBuildingRemovalManager.h
*  PURPOSE:     Header file for building removal.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#ifndef __CCBUILDINGREMOVALMANAGER_H
#define __CCBUILDINGREMOVALMANAGER_H

#include <map>

class CBuildingRemovalManager
{
public:
                                CBuildingRemovalManager              ( void );
                                ~CBuildingRemovalManager             ( void );
    void                        CreateBuildingRemoval                ( unsigned short usModel, float fRadius, const CVector& vecPos );
    void                        ClearBuildingRemovals                ( void );
    void                        RestoreWorldModel                    ( unsigned short usModel, float fRadius, const CVector& vecPos );
    inline multimap < unsigned short, CBuildingRemoval* > ::const_iterator  IterBegin    ( void )                    { return m_BuildingRemovals.begin (); };
    inline multimap < unsigned short, CBuildingRemoval* > ::const_iterator  IterEnd      ( void )                    { return m_BuildingRemovals.end (); };

private:

    std::multimap< unsigned short, CBuildingRemoval* >                                     m_BuildingRemovals;

};

#endif