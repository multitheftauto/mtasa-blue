/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientModelManager.h
*  PURPOSE:     Model manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

class CClientModelManager;

#ifndef __CCLIENTMODELMANAGER_H
#define __CCLIENTMODELMANAGER_H

#include <list>
#include "CClientModel.h"

#define MAX_MODEL_ID        19999

class CClientModelManager
{
    friend class CClientModel;

public:
                                    CClientModelManager       ( class CClientManager* pManager );
                                    ~CClientModelManager      ( void );

    void                            RemoveAll                 ( void );

    void                            Add                       ( CClientModel * pModel );
    bool                            Remove                    ( CClientModel * pModel );

    CClientModel *                  FindModelByID             ( ushort usModelID );

private:
    CClientModel *                  m_Models [ MAX_MODEL_ID + 1 ];
};

#endif
