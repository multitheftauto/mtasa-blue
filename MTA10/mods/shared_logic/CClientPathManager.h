/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPathManager.h
*  PURPOSE:     Path node entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

class CClientPathManager;

#ifndef __CCLIENTPATHMANAGER_H
#define __CCLIENTPATHMANAGER_H

#include "CClientPathNode.h"
#include "CClientManager.h"
#include <list>
using namespace std;

class CClientPathManager
{
    friend class CClientManager;
    friend class CClientPathNode;

public:
    void                                    DeleteAll               ( void );

    void                                    DoPulse                 ( void );
    void                                    LinkNodes               ( void );
    void                                    ReverseNodes            ( CClientPathNode* pPathNode );
    bool                                    DetachEntity            ( CClientEntity* pEntity );

    inline unsigned int                     Count                   ( void )                        { return static_cast < unsigned int > ( m_PathNodeList.size () ); }
    static CClientPathNode*                 Get                     ( ElementID ID );

    list < CClientPathNode* > ::iterator    IterBegin               ( void )                        { return m_PathNodeList.begin (); }
    list < CClientPathNode* > ::iterator    IterEnd                 ( void )                        { return m_PathNodeList.end (); }

private:
                                            CClientPathManager    ( CClientManager* pManager );
                                            ~CClientPathManager   ( void );

    inline void                             AddToList               ( CClientPathNode* pPathNode )  { m_PathNodeList.push_back ( pPathNode ); }
    void                                    RemoveFromList          ( CClientPathNode* pPathNode );

    CClientManager*                         m_pManager;
    list < CClientPathNode* >               m_PathNodeList;
    bool                                    m_bRemoveFromList;
};

#endif

