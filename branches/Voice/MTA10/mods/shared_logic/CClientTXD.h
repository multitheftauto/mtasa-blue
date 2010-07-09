/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTXD.h
*  PURPOSE:     TXD manager class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#ifndef __CCLIENTTXD_H
#define __CCLIENTTXD_H

#include "CClientEntity.h"

class CClientTXD: public CClientEntity
{
public:
                                    CClientTXD              ( class CClientManager* pManager, ElementID ID );
                                    ~CClientTXD             ( void );

    eClientEntityType               GetType                 ( void ) const              { return CCLIENTTXD; }

    bool                            LoadTXD                 ( const char* szFile, bool bFilteringEnabled = true );
    void                            UnloadTXD               ( void );
    inline bool                     IsLoaded                ( void )                    { return !m_Textures.empty (); }

    bool                            Import                  ( unsigned short usModelID );
    bool                            IsImported              ( unsigned short usModelID );

    void                            Remove                  ( unsigned short usModelID );
    void                            RemoveAll               ( void );

    static bool                     IsImportableModel       ( unsigned short usModelID );

    // Sorta a hack that these are required by CClientEntity...
    void                            Unlink                  ( void ) {};
    void                            GetPosition             ( CVector& vecPosition ) const {};
    void                            SetPosition             ( const CVector& vecPosition ) {};

private:
    void                            InternalRemove          ( unsigned short usModel );
    void                            Restream                ( unsigned short usModel );
    static RwTexture*               FindNamedTextureInList ( std::list < RwTexture* >& list, const char* szTexName );

    std::list < RwTexture* >        m_Textures;

    // This model contains our primary textures rather than copies
    unsigned short                  m_usMainModel;

    // Any SA textures that were replaced by custom ones (txd => texture list)
    static std::map < unsigned short, std::list < RwTexture* > > ms_ReplacedTXDTextures;
    // Any custom textures that were added (txd => texture list)
    static std::map < unsigned short, std::list < RwTexture* > > ms_AddedTXDTextures;

    // The models we have imported ourselves into
    std::list < unsigned short >    m_ImportedModels;
};

#endif
