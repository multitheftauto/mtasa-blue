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

struct SLoadedClumpInfo
{
    SLoadedClumpInfo ( void ) : bTriedLoad ( false ), pClump ( NULL ) {} 
    bool     bTriedLoad;
    RpClump* pClump;
};


class CClientDFF : public CClientEntity
{
    DECLARE_CLASS( CClientDFF, CClientEntity )
    friend class CClientDFFManager;

public:
                                    CClientDFF              ( class CClientManager* pManager, ElementID ID );
                                    ~CClientDFF             ( void );

    eClientEntityType               GetType                 ( void ) const              { return CCLIENTDFF; }

    bool                            LoadDFF                 ( const char* szFile, unsigned short usModelId );

    bool                            ReplaceModel            ( unsigned short usModel );

    bool                            HasReplaced             ( unsigned short usModel );

    void                            RestoreModel            ( unsigned short usModel );
    void                            RestoreModels           ( void );

    // Sorta a hack that these are required by CClientEntity...
    void                            Unlink                  ( void ) {};
    void                            GetPosition             ( CVector& vecPosition ) const {};
    void                            SetPosition             ( const CVector& vecPosition ) {};

protected:
    void                            UnloadDFF               ( void );
    void                            InternalRestoreModel    ( unsigned short usModel );

    bool                            ReplaceObjectModel      ( RpClump* pClump, ushort usModel );
    bool                            ReplaceVehicleModel     ( RpClump* pClump, ushort usModel );
    bool                            ReplaceWeaponModel      ( RpClump* pClump, ushort usModel );
    bool                            ReplacePedModel         ( RpClump* pClump, ushort usModel );

    RpClump*                        GetLoadedClump          ( ushort usModelId );

    class CClientDFFManager*        m_pDFFManager;

    SString                         m_strDffFilename;
    std::map < ushort, SLoadedClumpInfo > m_LoadedClumpInfoMap;

    std::list < unsigned short >    m_Replaced;
};

#endif
