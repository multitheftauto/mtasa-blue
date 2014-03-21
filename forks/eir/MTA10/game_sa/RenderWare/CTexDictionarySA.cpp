/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/CTexDictionarySA.cpp
*  PURPOSE:     Internal texture dictionary handler
*               RenderWare extension
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "../gamesa_renderware.h"

extern CBaseModelInfoSAInterface** ppModelInfo;

static RwTexture* RwTexDictionaryAssign( RwTexture *tex, CTexDictionarySA *txd )
{
    new CTextureSA( txd, tex );
    return tex;
}

CTexDictionarySA::CTexDictionarySA( RwTexDictionary *txd ) : CRwObjectSA( txd )
{
    // Assign all textures to us
    txd->ForAllTexturesSafe( RwTexDictionaryAssign, this );

#ifdef _MTA_BLUE
    // Register into the shader system.
    pGame->GetRenderWare()->ScriptAddedTxd( txd );
#endif //_MTA_BLUE

    // The texture manager (or whatever management environment) sets up the list node
}

CTexDictionarySA::~CTexDictionarySA( void )
{
    // Make sure we unlink from the global emitter (if assigned)
    if ( g_textureEmitter == GetObject() )
        g_textureEmitter = NULL;

    Clear();

    LIST_REMOVE( m_dicts ); // unlink us from the texture manager

    // Destroy our txd
    RwTexDictionaryDestroy( GetObject() );
}

void CTexDictionarySA::Clear( void )
{
    // Destroy all textures, they automatically detach from txds
    while ( !m_textures.empty() )
        delete *m_textures.begin();

    m_imported.clear();
}

bool CTexDictionarySA::IsImported( unsigned short id ) const
{
    CBaseModelInfoSAInterface *info = ppModelInfo[id];

    if ( id > MAX_MODELS-1 )
        return false;

    return IsImportedTXD( info->usTextureDictionary );
}

bool CTexDictionarySA::IsImportedTXD( unsigned short id ) const
{
    importList_t::const_iterator iter = m_imported.begin();

    for ( ; iter != m_imported.end(); ++iter )
        if ( *iter == id )
            return true;

    return false;
}

bool CTexDictionarySA::IsUsed( void ) const
{
    return ( m_imported.size() != 0 );
}

void CTexDictionarySA::SetGlobalEmitter( void )
{
    // Hook ourselves into the loading schemantics
    g_textureEmitter = GetObject();
}

bool CTexDictionarySA::Import( unsigned short id )
{
    CBaseModelInfoSAInterface *info = ppModelInfo[id];

    if ( id > MAX_MODELS-1 )
        return false;

    if ( !info )
        return false;

    return ImportTXD( info->usTextureDictionary );
}

bool CTexDictionarySA::ImportTXD( unsigned short id )
{
    if ( !TextureManager::GetTxdPool()->Get( id ) )
        return false;

    textureList_t::iterator iter = m_textures.begin();

    for ( ; iter != m_textures.end(); ++iter )
        (*iter)->ImportTXD( id );

    m_imported.push_back( id );
    return true;
}

bool CTexDictionarySA::Remove( unsigned short id )
{
    CBaseModelInfoSAInterface *info = ppModelInfo[id];

    if ( id > MAX_MODELS-1 )
        return false;

    if ( !info )
        return false;

    return RemoveTXD( info->usTextureDictionary );
}

bool CTexDictionarySA::RemoveTXD( unsigned short id )
{
    importList_t::iterator iter = std::find( m_imported.begin(), m_imported.end(), id );

    if ( iter == m_imported.end() )
        return true;

    if ( !TextureManager::GetTxdPool()->Get( id ) )
        return false;

    textureList_t::iterator itera = m_textures.begin();

    for ( ; itera != m_textures.end(); itera++ )
        (*itera)->RemoveTXD( id );

    m_imported.erase( iter );
    return true;
}

void CTexDictionarySA::ClearImports( void )
{
    while ( !m_imported.empty() )
        RemoveTXD( *m_imported.begin() );
}
