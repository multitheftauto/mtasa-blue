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
#include "CClientEntity.h"

enum eClientModelType
{
    CCLIENTMODELPED,
    CCLIENTMODELOBJECT,
    CCLIENTMODELVEHICLE,
};

class CClientModel : public CClientEntity
{
    DECLARE_CLASS( CClientModel, CClientEntity )
    friend class CClientModelManager;

public:
                                    CClientModel              ( CClientManager* pManager, ElementID ID, ushort usModelID, eClientModelType eModelType );
                                   ~CClientModel              ( void );

    eClientEntityType               GetType                   ( void ) const              { return CCLIENTMODEL; };

    ushort                          GetModelID                ( void )                    { return m_usModelID; };
    eClientModelType                GetModelType              ( void )                    { return m_eModelType; };
    bool                            Allocate                  ( void );
    bool                            Deallocate                ( void );

    // Sorta a hack that these are required by CClientEntity...
    void                            Unlink                    ( void ) {};
    void                            GetPosition               ( CVector& vecPosition ) const {};
    void                            SetPosition               ( const CVector& vecPosition ) {};

protected:
    class CClientModelManager*      m_pModelManager;

    ushort                          m_usModelID;
    eClientModelType                m_eModelType;
    bool                            m_bAllocatedByUs;
};

#endif
