/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientModel.h
*  PURPOSE:     Model handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

class CClientModel;

#ifndef __CCLIENTMODEL_H
#define __CCLIENTMODEL_H

#include <list>
#include "CClientModelManager.h"

enum eClientModelType
{
    CCLIENTMODELPED
};

class CClientModel
{
    friend class CClientModelManager;

public:
    CClientModel ( CClientManager* pManager, int iModelID, eClientModelType eModelType );
    ~CClientModel ( void );

    int                             GetModelID ( void ) { return m_iModelID; };
    eClientModelType                GetModelType ( void ) { return m_eModelType; };
    bool                            Allocate ( void );
    bool                            Deallocate ( void );

protected:
    CClientManager*                 m_pManager;
    class CClientModelManager*      m_pModelManager;

    int                             m_iModelID;
    eClientModelType                m_eModelType;
    bool                            m_bAllocatedByUs;
};

#endif