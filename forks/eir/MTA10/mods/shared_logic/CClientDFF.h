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
    CModel*  pClump;
};


class CClientDFF : public CClientEntity
{
    DECLARE_CLASS( CClientDFF, CClientEntity )
    friend class CClientDFFManager;

public:
                                    CClientDFF              ( class CClientManager* pManager, ElementID ID, bool directCache = false );
                                    ~CClientDFF             ( void );

    eClientEntityType               GetType                 ( void ) const              { return CCLIENTDFF; }

    bool                            LoadDFF                 ( const char* szFile, unsigned short usModel );

    bool                            ReplaceModel            ( unsigned short usModel, bool bAlphaTransparency );

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

    bool                            ReplaceObjectModel      ( CModel* pClump, ushort usModel, bool bAlphaTransparency );
    bool                            ReplaceVehicleModel     ( CModel* pClump, ushort usModel, bool bAlphaTransparency );
    bool                            ReplaceWeaponModel      ( CModel* pClump, ushort usModel, bool bAlphaTransparency );
    bool                            ReplacePedModel         ( CModel* pClump, ushort usModel, bool bAlphaTransparency );

    CModel*                         GetLoadedClump          ( ushort usModelId );

    class CClientDFFManager*        m_pDFFManager;

    SString                         m_strDffFilename;
    std::map < ushort, SLoadedClumpInfo > m_LoadedClumpInfoMap;

    SLoadedClumpInfo                m_persistentClump;

    bool                            m_usePersistentClump;

    std::list < unsigned short >    m_Replaced;
};

#endif
