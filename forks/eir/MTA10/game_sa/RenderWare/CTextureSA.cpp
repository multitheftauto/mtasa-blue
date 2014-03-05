/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/CStreamingSA.cpp
*  PURPOSE:     RenderWare texture extension
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

extern CBaseModelInfoSAInterface** ppModelInfo;

CTextureSA::CTextureSA( RwTexture *tex )
{
    m_texture = tex;
    m_dictionary = NULL;
}

CTextureSA::CTextureSA( CTexDictionarySA *dict, RwTexture *tex )
{
    m_texture = tex;
    m_dictionary = NULL;

    SetTXD( dict );
}

CTextureSA::~CTextureSA( void )
{
    ClearImports();

    // Remove us from the dictionary
    RemoveFromTXD();

    // Destroy ourselves
    RwTextureDestroy( m_texture );
}

const char* CTextureSA::GetName( void ) const
{
    return m_texture->name;
}

unsigned int CTextureSA::GetHash( void ) const
{
    return pGame->GetKeyGen()->GetUppercaseKey( m_texture->name );
}

void CTextureSA::SetTXD( CTexDictionary *_txd )
{
    CTexDictionarySA *txd = dynamic_cast <CTexDictionarySA*> ( _txd );

    if ( txd == m_dictionary )
        return;
    
    RemoveFromTXD();

    m_dictionary = txd;

    if ( txd )
    {
        m_dictionary->m_textures.push_front( this );
        m_texture->AddToDictionary( m_dictionary->GetObject() );
    }
}

void CTextureSA::RemoveFromTXD( void )
{
    if ( !m_dictionary )
        return;

    m_texture->RemoveFromDictionary();
    m_dictionary->m_textures.remove( this );

    m_dictionary = NULL;
}

CTexDictionary* CTextureSA::GetTXD( void )
{
    return m_dictionary;
}

bool CTextureSA::IsImported( unsigned short id ) const
{
    CBaseModelInfoSAInterface *info = ppModelInfo[id];

    if ( id > DATA_TEXTURE_BLOCK-1 )
        return false;

    if ( !info )
        return false;

    return IsImportedTXD( info->usTextureDictionary );
}

bool CTextureSA::IsImportedTXD( unsigned short id ) const
{
    return std::find( m_imported.begin(), m_imported.end(), id ) != m_imported.end();
}

bool CTextureSA::IsUsed( void ) const
{
    return ( m_imported.size() != 0 );
}

bool CTextureSA::Import( unsigned short id )
{
    CBaseModelInfoSAInterface *info = ppModelInfo[id];

    if ( id > DATA_TEXTURE_BLOCK-1 )
        return false;

    if ( !info )
        return false;

    return ImportTXD( info->usTextureDictionary );
}

bool CTextureSA::ImportTXD( unsigned short id )
{
    // Do not import twice
    if ( IsImportedTXD( id ) )
        return true;

    CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( id );

    if ( !txd )
        return false;

    g_dictImports[id].push_back( this );

    m_imported.push_front( id );
    return true;
}

bool CTextureSA::Remove( unsigned short id )
{
    CBaseModelInfoSAInterface *info = ppModelInfo[id];

    if ( id > MAX_MODELS-1 )
        return false;

    if ( !info )
        return false;

    return RemoveTXD( info->usTextureDictionary );
}

bool CTextureSA::RemoveTXD( unsigned short id )
{
    importList_t::iterator iter = std::find( m_imported.begin(), m_imported.end(), id );

    if ( iter == m_imported.end() )
        return false;

    g_dictImports[id].remove( this );

    m_imported.erase( iter );
    return true;
}

void CTextureSA::ClearImports( void )
{
    while ( !m_imported.empty() )
        RemoveTXD( (*m_imported.begin()) );
}