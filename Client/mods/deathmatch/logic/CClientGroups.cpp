/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGroups.cpp
*  PURPOSE:     Dummy grouping class
*  DEVELOPERS:  Chris McArthur <>
*               Jax <>
*
*****************************************************************************/

#include <StdInc.h>

void CClientGroups::DeleteAll ( void )
{
    // Delete all the elements
    m_bDontRemoveFromList = true;

    if ( !m_List.empty () )
    {
        for ( auto& pDummy : m_List )
        {
            if ( pDummy )
            {
                delete pDummy;
            }
        }
        m_List.clear ();
    }

    m_bDontRemoveFromList = false;
}


void CClientGroups::RemoveFromList ( CClientDummy* pDummy )
{
    if ( !m_bDontRemoveFromList )
    {
        if ( !m_List.empty () )
        {
            for ( auto iter = m_List.begin(); iter != m_List.end(); )
            {
                CClientDummy* pCurrentDummy = *iter;
                if ( pCurrentDummy == pDummy )
                {
                    iter = m_List.erase ( iter );
                }
                else
                {
                    ++iter;
                }
            }
        }
    }
}
