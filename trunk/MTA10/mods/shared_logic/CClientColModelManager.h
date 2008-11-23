/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColModelManager.cpp
*  PURPOSE:     Model collision entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CCLIENTCOLMODELMANAGER_H
#define __CCLIENTCOLMODELMANAGER_H

#include <list>
#include "CClientColModel.h"

class CClientColModelManager
{
    friend class CClientColModel;

public:
                                    CClientColModelManager      ( class CClientManager* pManager );
                                    ~CClientColModelManager     ( void );

    void                            RemoveAll                   ( void );
    bool                            Exists                      ( CClientColModel* pCol );

    CClientColModel*                GetElementThatReplaced      ( unsigned short usModel, CClientColModel* pDontSearch = NULL );

    static bool                     IsReplacableModel           ( unsigned short usModel );
    bool                            RestoreModel                ( unsigned short usModel );

    list < CClientColModel* > ::const_iterator IterBegin        ( void )                    { return m_List.begin (); }
    list < CClientColModel* > ::const_iterator IterEnd          ( void )                    { return m_List.end (); }

private:
    void                            AddToList                   ( CClientColModel* pCol )   { m_List.push_back ( pCol ); }
    void                            RemoveFromList              ( CClientColModel* pCol );

    list < CClientColModel* >       m_List;
    bool                            m_bRemoveFromList;
};

#endif
