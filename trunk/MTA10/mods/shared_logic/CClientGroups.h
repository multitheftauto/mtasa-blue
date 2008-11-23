/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGroups.h
*  PURPOSE:     Dummy grouping class
*  DEVELOPERS:  Chris McArthur <>
*               Jax <>
*
*****************************************************************************/

#ifndef __CCLIENTGROUPS_H
#define __CCLIENTGROUPS_H

#include "CClientDummy.h"
#include "CClientEntity.h"
#include <list>

using namespace std;

class CClientGroups
{
    friend class CClientDummy;

public:
    inline                                  CClientGroups                   ( void )                { m_bDontRemoveFromList = false; };
    inline                                  ~CClientGroups                  ( void )                { DeleteAll (); };

    void                                    DeleteAll                       ( void );

private:
    inline void                             AddToList                       ( CClientDummy* pDummy )      { m_List.push_back ( pDummy ); };
    void                                    RemoveFromList                  ( CClientDummy* pDummy );

    list < CClientDummy* >                  m_List;
    bool                                    m_bDontRemoveFromList;
};

#endif
