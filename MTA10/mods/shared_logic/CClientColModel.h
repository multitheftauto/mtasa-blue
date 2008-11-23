/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColModel.h
*  PURPOSE:     Model collision (.col file) entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CCLIENTCOLMODEL_H
#define __CCLIENTCOLMODEL_H

#include <list>
#include "CClientEntity.h"

class CClientColModel: public CClientEntity
{
public:
                                    CClientColModel         ( class CClientManager* pManager, ElementID ID );
                                    ~CClientColModel        ( void );

    eClientEntityType               GetType                 ( void ) const              { return CCLIENTCOL; }

    bool                            LoadCol                 ( const char* szFile );
    void                            UnloadCol               ( void );
    inline bool                     IsLoaded                ( void )                    { return m_pColModel != NULL; };

    bool                            Replace                 ( unsigned short usModel );
    void                            Restore                 ( unsigned short usModel );
    void                            RestoreAll              ( void );

    bool                            HasReplaced             ( unsigned short usModel );


    // Sorta a hack that these are required by CClientEntity...
    void                            Unlink                  ( void ) {};
    void                            GetPosition             ( CVector& vecPosition ) const {};
    void                            SetPosition             ( const CVector& vecPosition ) {};

private:
    void                            InternalRestore         ( unsigned short usModel );

    class CClientColModelManager*   m_pColModelManager;
    
    CColModel*                      m_pColModel;
    std::list < unsigned short >    m_Replaced;
};

#endif
