/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBuildingRemovalManager.h
*  PURPOSE:     source file for the custom weapon stats manager.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/
#include "StdInc.h"

CBuildingRemovalManager::CBuildingRemovalManager ( void )
{

}

CBuildingRemovalManager::~CBuildingRemovalManager ( void )
{
    for ( std::multimap < unsigned short, CBuildingRemoval* >::const_iterator iter = m_BuildingRemovals.begin (); iter != m_BuildingRemovals.end ();iter++ )
    {
        if ( (*iter ).second )
            delete (*iter).second;
    }
}

CBuildingRemoval * CBuildingRemovalManager::CreateBuildingRemoval ( unsigned short usModel, float fRadius, CVector vecPos )
{
    CBuildingRemoval * pBuildingRemoval = new CBuildingRemoval ( usModel, fRadius, vecPos );
    m_BuildingRemovals.insert( std::pair < unsigned short, CBuildingRemoval * > ( usModel, pBuildingRemoval ) );
    return pBuildingRemoval;
}

void CBuildingRemovalManager::ClearBuildingRemovals ( void )
{
    for ( std::multimap < unsigned short, CBuildingRemoval* >::const_iterator iter = m_BuildingRemovals.begin (); iter != m_BuildingRemovals.end ();iter++ )
    {
        if ( (*iter ).second )
            delete (*iter).second;
    }
    m_BuildingRemovals.clear ( );
}

void CBuildingRemovalManager::RestoreWorldModel ( unsigned short usModel, float fRadius, CVector vecPos )
{

    CBuildingRemoval * pFind = NULL;
    std::pair < std::multimap < unsigned short, CBuildingRemoval* >::iterator, std::multimap < unsigned short, CBuildingRemoval* >::iterator> iterators = m_BuildingRemovals.equal_range ( usModel );
    std::multimap < unsigned short, CBuildingRemoval* > ::iterator iter = iterators.first;
    for ( ; iter != iterators.second; )
    {
        pFind = (*iter).second;
        if ( pFind ) 
        {
            CVector vecRemovalPos = pFind->GetPosition ( );
            // Grab distances across each axis
            float fDistanceX = vecPos.fX - vecRemovalPos.fX;
            float fDistanceY = vecPos.fY - vecRemovalPos.fY;
            float fDistanceZ = vecPos.fZ - vecRemovalPos.fZ;

            // Square root 'em
            float fDistance = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );

            if ( fDistance <= pFind->GetRadius ( ) )
            {
                m_BuildingRemovals.erase ( iter++ );
                delete pFind;
            }
            else
                iter++;
        }
        else
            iter++;
    }
}