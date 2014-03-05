/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.textures.cpp
*  PURPOSE:     Texture streaming routines
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "gamesa_renderware.h"

/*
    Texture Scanner Namespaces

    Those namesspaces are texture scanners which - once applied - are called
    during RwFindTexture. They are meant to be stack-based, so that they have
    to be unattached the same order they were applied. If the scanner does not
    find the texture in it's environment, it calls the previously attached
    scanner. Most of the time they use the local reference storage.
*/

/*=========================================================
    RwRemapScan

    This logic scans the general VEHICLE.TXD that is
    loaded in _VehicleModels_Init. If the texture was not found,
    the previously applied handler is called.

    It is used during the loading of vehicle models, currently
    GTA:SA internal only. The original GTA:SA function did not
    call the previous texture scanner.
=========================================================*/
namespace RwRemapScan
{
    static RwScanTexDictionaryStackRef_t    prevStackScan;

    // Method which scans VEHICLE.TXD
    // Binary offsets: (1.0 US and 1.0 EU): 0x004C7510
    RwTexture* scanMethod( const char *name )
    {
        RwTexture *tex = g_vehicleTxd->FindNamedTexture( name );

        if ( tex )
            return tex;

        // The_GTA: usually the engine flagged used remap textures with a '#'
        // * It was done by replacing the first character in their name.
        // * The engine would always perform two scans: one for the provided name
        // * and another for the '#' flagged version.
        // We do not want this feature. If we do, discuss with midnightStar/Martin.
        return prevStackScan( name );
    }

    // Stores the previous texture scanner and applies ours
    // Binary offsets: (1.0 US and 1.0 EU): 0x004C75A0
    void Apply( void )
    {
        RwInterface *rwInterface = RenderWare::GetInterface();

        prevStackScan = rwInterface->m_textureManager.findInstanceRef;
        rwInterface->m_textureManager.findInstanceRef = scanMethod;
    }

    // Restores the previous texture scanner
    // Binary offsets: (1.0 US and 1.0 EU): 0x004C75C0
    void Unapply( void )
    {
        RwInterface *rwInterface = RenderWare::GetInterface();

        rwInterface->m_textureManager.findInstanceRef = prevStackScan;
        prevStackScan = NULL;
    }
};

/*=========================================================
    RwImportedScan (MTA extension)

    This logic scans the textures which virtually included themselves
    into a specific TXD id slot. It is meant to be a cleaner solution than 
    modifying the GTA:SA internal TexDictionaries.
    1) GTA:SA can unload it's TXDs without corrupting MTA data (vid memory saved)
    2) Individual textures can be applied instead of whole TXDs (flexibility)
    3) Texture instances come straight - without copying - from the
       provider (i.e. deathmatch Lua). Modifications to the provider
       texture instance result in direct changes of the ingame representation
       (i.e. anisotropy change or filtering flags) (integrity).

    We are applying this whenever a model is loaded, either by the
    GTA:SA streaming requester or the MTA model loader. That is why
    this namespace is globally exported.

    The problem with the current approach is that if a user chooses to replace
    textures, only textures from a model are replaced (i.e. no HUD ones).
    A proper fix would be a hook on the GTA:SA function TXDSetCurrent and
    RwFindTexture. Another interesting fix may be to rewrite all GTA:SA functions
    which use TXDSetCurrent.
    We may split replacing of global and replacing of model textures. If the user
    decides to import textures into models above DATA_TEXTURE_BLOCK, then the global
    replacer is issued, which will be the hook in RwFindTexture. This way we could
    save some performance, since RwImportedScan is cleaner.
=========================================================*/
namespace RwImportedScan
{
    static RwScanTexDictionaryStackRef_t    prevStackScan;
    static unsigned short   txdId;
    static bool             applied;

    RwTexture* scanMethodEx( int id, const char *name )
    {
        dictImportList_t& imported = g_dictImports[id];

        for ( dictImportList_t::const_iterator iter = imported.begin(); iter != imported.end(); ++iter )
        {
            if ( stricmp( (*iter)->GetName(), name ) == 0 )
                return (*iter)->GetTexture();
        }

        return NULL;
    }

    RwTexture* scanMethod( const char *name )
    {
        if ( RwTexture *tex = scanMethodEx( txdId, name ) )
            return tex;

        return prevStackScan( name );
    }

    void Apply( unsigned short id )
    {
        // Performance improvement: only apply this handler if we actually have imported textures.
        if ( !g_dictImports[id].empty() )
        {
            RwInterface *rwInterface = RenderWare::GetInterface();

            prevStackScan = rwInterface->m_textureManager.findInstanceRef;
            rwInterface->m_textureManager.findInstanceRef = scanMethod;

            txdId = id;
            applied = true;
        }
        else
            applied = false;
    }

    void Unapply( void )
    {
        if ( applied )
        {
            RwInterface *rwInterface = RenderWare::GetInterface();

            rwInterface->m_textureManager.findInstanceRef = prevStackScan;
            prevStackScan = NULL;
        }
    }
};

/*=========================================================
    RwTexDictionaryLoadFirstHalf

    Arguments:
        stream - binary stream which contains the TXD
    Purpose:
        Loads half of the textures from a TexDictionary found in
        the provided stream. The rest of it is loaded in another
        function. This way execution time is (somewhat) flattened out.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731070
    Note:
        Other than the big clump loading, this code does appear
        to be used!
=========================================================*/
static unsigned int big_numTXDBlocks = 0;
static unsigned int big_txdStreamOffset = 0;

static inline void RwTexDictionaryClear( RwTexDictionary *txd )
{
    // Destroy all attached textures
    LIST_FOREACH_BEGIN( RwTexture, txd->textures.root, TXDList )
        RwTextureDestroy( item );
    LIST_FOREACH_END

    RwTexDictionaryDestroy( txd );
}

static RwTexDictionary* RwTexDictionaryLoadFirstHalf( RwStream *stream )
{
    big_numTXDBlocks = 0;

    unsigned int version;
    unsigned int length;

    if ( !RwStreamFindChunk( stream, 1, &length, &version ) )
        return NULL;

    // The_GTA: fixed possible exploit based on block info being bigger than 4 (buffer overflow + code injection, at worst)
    if ( length < sizeof(RwBlocksInfo) )
        return NULL;

    RwBlocksInfo info;

    if ( RwStreamReadBlocks( stream, &info, sizeof(RwBlocksInfo) ) != sizeof(RwBlocksInfo) )
        return NULL;

    RwTexDictionary *txd = RwTexDictionaryCreate();

    if ( !txd )
        return NULL;

    unsigned short numBlocksHalf = info.count / 2;

    big_numTXDBlocks = numBlocksHalf;

    while ( info.count > numBlocksHalf )
    {
        RwTexture *tex = RwTextureStreamReadEx( stream );

        if ( !tex )
        {
            RwTexDictionaryClear( txd );
            return NULL;
        }

        tex->AddToDictionary( txd );

        info.count--;
    }

    big_txdStreamOffset = stream->data.buffered.position;
    return txd;
}

/*=========================================================
    RwTexDictionaryContinueLoading

    Arguments:
        stream - binary stream which contains the TXD
    Purpose:
        Loads the other half of the requested big TexDictionary
        by using the cached stream offset and block count. Returns
        the TexDictionary if successful.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731150
    Note:
        Other than the big clump loading, this code does appear
        to be used!
=========================================================*/
static RwTexDictionary* __cdecl RwTexDictionaryContinueLoading( RwStream *stream, RwTexDictionary *txd )
{
    // Jump to the previous stream offset
    RwStreamSkip( stream, big_txdStreamOffset - stream->data.buffered.position );

    for ( unsigned int n = big_numTXDBlocks; n != 0; n-- )
    {
        RwTexture *tex = RwTextureStreamReadEx( stream );

        if ( !tex )
        {
            RwTexDictionaryClear( txd );
            return NULL;
        }

        tex->AddToDictionary( txd );
    }

    big_numTXDBlocks = 0;
    return txd;
}

/*=========================================================
    LoadTXDFirstHalf

    Arguments:
        id - index of the TXD instance
        stream - binary stream which contains the TXD
    Purpose:
        Checks whether the stream contains a TXD. If not, it
        returns false. Then it returns whether the loading of
        half the TexDictionary into the TXD instance was
        successful.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731930
=========================================================*/
bool __cdecl LoadTXDFirstHalf( unsigned int id, RwStream *stream )
{
    CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( id );

    if ( !RwStreamFindChunk( stream, 0x16, NULL, NULL ) )
        return false;

    return ( txd->m_txd = RwTexDictionaryLoadFirstHalf( stream ) ) != NULL;
}

/*=========================================================
    LoadTXDContinue

    Arguments:
        id - index of the TXD instance
        stream - binary stream which contains the TXD
    Purpose:
        Continues the TXD loading procedure which was initiated
        by LoadTXDFirstHalf. The remaining textures are loaded
        from the stream and added to the TXD pointed at by id.
        Returns false if there was an error during loading.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731E40
=========================================================*/
bool __cdecl LoadTXDContinue( unsigned int id, RwStream *stream )
{
    CTxdInstanceSA *txdInst = TextureManager::GetTxdPool()->Get( id );

    RwTexDictionary *txd = txdInst->m_txd = RwTexDictionaryContinueLoading( stream, txdInst->m_txd );

    if ( txd )
        txdInst->InitParent();

    return txd != NULL;
}