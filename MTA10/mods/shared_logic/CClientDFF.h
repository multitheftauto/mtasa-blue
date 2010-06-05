/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDFF.h
*  PURPOSE:     .dff model handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

class CClientDFF;

#ifndef __CCLIENTDFF_H
#define __CCLIENTDFF_H

#include <list>
#include "CClientEntity.h"
//#include "CClientDFFManager.h"

class CClientDFF: public CClientEntity
{
    friend class CClientDFFManager;

public:
                                    CClientDFF              ( class CClientManager* pManager, ElementID ID );
                                    ~CClientDFF             ( void );

    eClientEntityType               GetType                 ( void ) const              { return CCLIENTDFF; }

    bool                            LoadDFF                 ( const char* szFile, unsigned short usCollisionModel = 0 );
    void                            UnloadDFF               ( void );
    inline bool                     IsLoaded                ( void )                    { return m_pLoadedClump != NULL; };

    bool                            ReplaceModel            ( unsigned short usModel );

    bool                            HasReplaced             ( unsigned short usModel );

    void                            RestoreModel            ( unsigned short usModel );
    void                            RestoreModels           ( void );

    // Sorta a hack that these are required by CClientEntity...
    void                            Unlink                  ( void ) {};
    void                            GetPosition             ( CVector& vecPosition ) const {};
    void                            SetPosition             ( const CVector& vecPosition ) {};

protected:
    void                            InternalRestoreModel    ( unsigned short usModel );

    bool                            ReplaceObjectModel      ( unsigned short usModel );
    bool                            ReplaceVehicleModel     ( unsigned short usModel );

    class CClientDFFManager*        m_pDFFManager;

    RpClump*                        m_pLoadedClump;

    std::list < unsigned short >    m_Replaced;
};

#endif
