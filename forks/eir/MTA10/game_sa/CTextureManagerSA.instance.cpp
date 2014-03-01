/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTextureManagerSA.instance.cpp
*  PURPOSE:     Internal texture management definitions
*               SubInterface of RenderWare
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "gamesa_renderware.h"

// Events (for shader system)
void _cdecl OnStreamingAddedTxd ( DWORD dwTxdId );
void _cdecl OnStreamingRemoveTxd ( DWORD dwTxdId );

/*=========================================================
    CTxdInstanceSA::constructor

    Arguments:
        name - name of the txd entry
    Purpose:
        Creates a new internal GTA:SA TXD entry. TXD instances are
        GTA:SA internal-only structures. We do not need them to import
        textures into models. Rockstar made them so they could have
        ids point at loaded TXDs.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731C80 (actually int TxdCreate( int ))
=========================================================*/
CTxdInstanceSA::CTxdInstanceSA( const char *name )
{
    m_txd = NULL;
    m_references = 0;
    m_parentTxd = 0xFFFF;
    m_hash = pGame->GetKeyGen()->GetUppercaseKey(name);
}

/*=========================================================
    CTxdInstanceSA::destructor

    Purpose:
        Destroys the given TXD entry. All ids pointing to this
        TXD will turn invalid.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731CD0 (actually void TxdDestroy( int ))
=========================================================*/
CTxdInstanceSA::~CTxdInstanceSA( void )
{
    Deallocate();
}

// Allocators for TXD instances. They have to be placed inside the pool.
void* CTxdInstanceSA::operator new( size_t )
{
    return (*ppTxdPool)->Allocate();
}

void CTxdInstanceSA::operator delete( void *ptr )
{
    (*ppTxdPool)->Free( (CTxdInstanceSA*)ptr );
}

/*=========================================================
    CTxdInstanceSA::Allocate (MTA extension)

    Purpose:
        Makes sure that this instance has an active TXD.
=========================================================*/
void CTxdInstanceSA::Allocate( void )
{
    if ( m_txd )
        return;

    m_txd = RwTexDictionaryCreate();
}

/*=========================================================
    CTxdInstanceSA::Deallocate

    Purpose:
        Destroys the TXD associated with this instance, if any.
        We notify the shader system of this event. Parent TXD
        instances are dereferenced.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731E90 (actually void TxdDeallocate( int ))
=========================================================*/
// If textures from this TXD are used somewhere (referenced), detach and dereference them
static bool Txd_DeleteAll( RwTexture *tex, int )
{
    if ( tex->refs > 1 )
    {
        RwTextureDestroy( tex );
        tex->RemoveFromDictionary();
    }

    return true;
}

void CTxdInstanceSA::Deallocate( void )
{
    unsigned short id = (*ppTxdPool)->GetIndex( this );

    // Notify the shader system
    OnStreamingRemoveTxd( id + 20000 );

    if ( m_txd )
    {
        // HACK: detach the global texture emitter if present
        if ( g_textureEmitter == m_txd )
            g_textureEmitter = NULL;

        m_txd->ForAllTexturesSafe( Txd_DeleteAll, 0 );

        RwTexDictionaryDestroy( m_txd );

        m_txd = NULL;
    }

    if ( m_parentTxd != 0xFFFF )
    {
        // Bugfix for resource termination (we check if the id is still valid)
        CTxdInstanceSA *parentTxd = (*ppTxdPool)->Get( m_parentTxd );

        if ( parentTxd )
            parentTxd->Dereference();
    }
}

/*=========================================================
    CTxdInstanceSA::LoadTXD

    Arguments:
        stream - binary stream which contains a TexDictionary
    Purpose:
        Attempts to load a TXD from a RenderWare stream. The
        operation succeeds if there is a TXD in the stream and
        there was no error during the loading of individual
        textures.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007320B0 (actually void TxdLoad( int, const char* ))
=========================================================*/
bool CTxdInstanceSA::LoadTXD( RwStream *stream )
{
    if ( !RwStreamFindChunk( stream, 0x16, NULL, NULL ) )
        return false;

    m_txd = RwTexDictionaryStreamReadEx( stream );

    if ( !m_txd )
        return false;

    // HACK: set the global texture emitter so loading the txd is the only requirement for model textures
    // (BACKWARDS COMPATIBILITY WITH MTA:BLUE)
    //g_textureEmitter = m_txd;
    return true;
}

/*=========================================================
    CTxdInstanceSA::LoadTXD (MTA extension)

    Arguments:
        file - FileSystem binary stream which contains a TexDictionary
    Purpose:
        Transforms the FileSystem stream into a RenderWare stream
        and attempts to load a TexDictionary from it using LoadTXD( RwStream* ).
        Returns false if there already is a TexDictionary attached to
        this instance or the system failed to transform the stream.
=========================================================*/
bool CTxdInstanceSA::LoadTXD( CFile *file )
{
    if ( m_txd )
        return false;

    RwStream *stream = RwStreamCreateTranslated( file );

    if ( !stream ) 
        return false;

    bool success = LoadTXD( stream );

    RwStreamClose( stream, NULL );
    return success;
}

/*=========================================================
    CTxdInstanceSA::InitParent

    Purpose:
        Initializes a TXD look-up tree. If a texture was not found inside
        this instance's TXD, then its parent is scanned. This way TXD
        instances can reuse textures from common containers. Usually all
        loaded TXD instances have their parent initialized.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731D50 (actually void TxdInitParent( int ))
=========================================================*/
void CTxdInstanceSA::InitParent( void )
{
    // Assign texture imports
    unsigned short id = (*ppTxdPool)->GetIndex( this );

    // Notify the shader system
    OnStreamingAddedTxd( id );

    CTxdInstanceSA *parent = (*ppTxdPool)->Get( m_parentTxd );

    if ( !parent )
        return;

    m_txd->m_parentTxd = parent->m_txd;

    parent->Reference();
}

/*=========================================================
    CTxdInstanceSA::Reference

    Purpose:
        Increases the usage/reference count for this TXD instance.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731A00 (actually void TxdAddRef( int ))
=========================================================*/
void CTxdInstanceSA::Reference( void )
{
    m_references++;
}

/*=========================================================
    CTxdInstanceSA::Dereference

    Purpose:
        Decreases the reference count of this TXD instance. If
        it reaches 0, the resources of this TXD instance are
        deallocated.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731A30 (actually void TxdRemoveRef( int ))
=========================================================*/
void CTxdInstanceSA::Dereference( void )
{
    m_references--;

    // We unload ourselves if not used anymore
    if ( m_references == 0 )
        pGame->GetStreaming()->FreeModel( DATA_TEXTURE_BLOCK + (*ppTxdPool)->GetIndex( this ) );
}

/*=========================================================
    CTxdInstanceSA::DereferenceNoDestroy

    Purpose:
        Decreases the reference count of this TXD instance without
        deallocating its resources. This is an internal function
        used by the streaming environment.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731A70 (actually void TxdDereferenceNoDestroy( int ))
=========================================================*/
void CTxdInstanceSA::DereferenceNoDestroy( void )
{
    // Used by streaming
    m_references--;
}

/*=========================================================
    CTxdInstanceSA::SetCurrent

    Purpose:
        Sets the TexDictionary of this instance as the current
        TexDictionary for named texture look-up.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007319C0 (actually void TxdSetCurrent( int ))
=========================================================*/
void CTxdInstanceSA::SetCurrent( void )
{
    pRwInterface->m_textureManager.current = m_txd;
}