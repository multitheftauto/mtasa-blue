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
    m_hash = pGame->GetKeyGen()->GetUppercaseKey( name );
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
    return TextureManager::GetTxdPool()->Allocate();
}

void CTxdInstanceSA::operator delete( void *ptr )
{
    TextureManager::GetTxdPool()->Free( (CTxdInstanceSA*)ptr );
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
    int id = TextureManager::GetTxdPool()->GetIndex( this );

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
        CTxdInstanceSA *parentTxd = TextureManager::GetTxdPool()->Get( m_parentTxd );

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
        (1.0 US and 1.0 EU): 0x00731DD0 (actually void TxdLoadBinary( int, RwStream* ))
=========================================================*/
bool CTxdInstanceSA::LoadTXD( RwStream *stream )
{
    if ( !RwStreamFindChunk( stream, 0x16, NULL, NULL ) )
        return false;

    m_txd = RwTexDictionaryStreamReadEx( stream );

    return m_txd != NULL;
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
    CTxdPool *txdPool = TextureManager::GetTxdPool();

    // Assign texture imports
    int id = txdPool->GetIndex( this );

    // Notify the shader system
    OnStreamingAddedTxd( id );

    CTxdInstanceSA *parent = txdPool->Get( m_parentTxd );

    if ( parent )
    {
        m_txd->m_parentTxd = parent->m_txd;

        parent->Reference();
    }
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
        pGame->GetStreaming()->FreeModel( DATA_TEXTURE_BLOCK + TextureManager::GetTxdPool()->GetIndex( this ) );
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
    RwTexDictionarySetCurrent( m_txd );
}

/*=====================================================================
===
=== Global Texture Dictionary Game Lookup Interface
===
======================================================================*/

// Binary offsets: (1.0 US and 1.0 EU): 0x00731C80
int __cdecl TxdCreate( const char *name )
{
    CTxdInstanceSA *inst = new CTxdInstanceSA( name );

    // Check for crashes here
    if ( !inst )
        return -1;

    return TextureManager::GetTxdPool()->GetIndex( inst );
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00731CD0
void __cdecl TxdDestroy( int txdIndex )
{
    CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( txdIndex );

    // Crashfix
    if ( !txd )
        return;

    delete txd;
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00731850
static int _txdFindIndexCache = 0;      // Binary offsets: (1.0 US and 1.0 EU): 0x00C88014

int __cdecl TxdFind( const char *name )
{
    unsigned int hash = pGame->GetKeyGen()->GetUppercaseKey( name );

    CTxdPool *txdPool = TextureManager::GetTxdPool();

    bool shouldRepeat = true;

    int n = _txdFindIndexCache;
    int max = MAX_TXD;

    while ( true )
    {
        if ( n >= max )
        {
            if ( shouldRepeat )
            {
                shouldRepeat = false;

                // Reset scanning boundaries.
                max = n;
                n = 0;
            }
            else
                break;
        }

        CTxdInstanceSA *txd = txdPool->Get( n );

        if ( txd && txd->GetHash() == hash )
        {
            _txdFindIndexCache = n;
            return n;
        }

        n++;
    }

    return -1;
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00731DD0
bool __cdecl TxdLoadBinary( int id, RwStream *stream )
{
    CTxdInstanceSA *txdInst = TextureManager::GetTxdPool()->Get( id );

    // Checking them NULLs!
    if ( !txdInst )
        return false;

    return txdInst->LoadTXD( stream );
}

// MTA extension.
int __cdecl TxdLoadEx( int id, const char *name, const char *filename )
{
    CTxdInstanceSA *txdInst = TextureManager::GetTxdPool()->Get( id );

    CFile *file = OpenGlobalStream( filename, "rb" );

    bool success = txdInst->LoadTXD( file );

    if ( file )
        delete file;

    // Just to be sure :p
    if ( !success )
        return -1;

    txdInst->InitParent();
    return id;
}

// Binary offsets: (1.0 US and 1.0 EU): 0x007320B0
int __cdecl TxdLoad( int id, const char *filename )
{
    return TxdLoadEx( id, FileSystem::GetFileNameItem( filename ).c_str(), filename );
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00408340
RwTexDictionary* __cdecl TxdGetRwObject( int id )
{
    CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( id );

    // They may have used debug assertions to fix these NULL-ptr crashes.
    if ( !txd )
        return NULL;

    return txd->m_txd;
}

int _currentTxdItem = -1;

bool __cdecl TxdSetCurrentSafe( int id )
{
    CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( id );

    // Crashfix.
    if ( !txd )
        return false;

    _currentTxdItem = id;

    txd->SetCurrent();
    return true;
}

// Binary offsets: (1.0 US and 1.0 EU): 0x007319C0
void __cdecl TxdSetCurrent( int id )
{
    TxdSetCurrentSafe( id );
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00731E90
void __cdecl TxdDeallocate( int id )
{
    CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( id );

    // Just to be sure.
    if ( txd )
        txd->Deallocate();
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00731D50
void __cdecl TxdInitParent( int id )
{
    CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( id );

    // That we do not have game crashes.
    if ( txd )
        txd->InitParent();
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00731A00
void __cdecl TxdAddRef( int id )
{
    CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( id );

    // Imagine that R* did not make these.
    if ( txd )
        txd->Reference();
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00731A30
void __cdecl TxdRemoveRef( int id )
{
    CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( id );

    // They were either mad
    if ( txd )
        txd->Dereference();
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00731A70
void __cdecl TxdRemoveRefNoDestroy( int id )
{
    CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( id );

    // or extremely sure of their code.
    if ( txd )
        txd->DereferenceNoDestroy();
}

// Binary offsets: (1.0 US): 0x007F3AC0 (1.0 EU): 0x007F3B00
static RwTexture* __cdecl HOOK_RwFindTexture( const char *name, const char *secName )
{
    // We must scan texture replacement dictionaries here.
    if ( _currentTxdItem != -1 )
    {
        if ( RwTexture *tex = RwImportedScan::scanMethodEx( _currentTxdItem, name ) )
        {
            tex->refs++;
            return tex;
        }
    }

    return RwFindTexture( name, secName );
}

void CTextureManagerSA::InitGameHooks( void )
{
    // Hook important routines.
    HookInstall( 0x00731C80, (DWORD)TxdCreate, 5 );
    HookInstall( 0x00731CD0, (DWORD)TxdDestroy, 5 );
    HookInstall( 0x00731850, (DWORD)TxdFind, 5 );
    HookInstall( 0x00731DD0, (DWORD)TxdLoadBinary, 4 );
    HookInstall( 0x007320B0, (DWORD)TxdLoad, 5 );
    HookInstall( 0x00408340, (DWORD)TxdGetRwObject, 5 );
    HookInstall( 0x007319C0, (DWORD)TxdSetCurrent, 5 );
    HookInstall( 0x00731E90, (DWORD)TxdDeallocate, 5 );
    HookInstall( 0x00731D50, (DWORD)TxdInitParent, 5 );
    HookInstall( 0x00731A00, (DWORD)TxdAddRef, 5 );
    HookInstall( 0x00731A30, (DWORD)TxdRemoveRef, 5 );
    HookInstall( 0x00731A70, (DWORD)TxdRemoveRefNoDestroy, 5 );

    // Texture scanning fixes.
    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        HookInstall( 0x007F3B00, (DWORD)HOOK_RwFindTexture, 5 );
        break;
    case VERSION_US_10:
        HookInstall( 0x007F3AC0, (DWORD)HOOK_RwFindTexture, 5 );
        break;
    }
}