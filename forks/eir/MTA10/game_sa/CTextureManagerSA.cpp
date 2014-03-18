/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTextureManagerSA.cpp
*  PURPOSE:     Internal texture management
*               SubInterface of RenderWare
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

#define FUNC_InitTextureManager         0x00731F20
#define VAR_CPlayerTexDictionaries      0x00C88004

extern CBaseModelInfoSAInterface **ppModelInfo;
RwTexDictionary *g_textureEmitter = NULL;

// Imports to the textures of ppTxdPool. Used in the streaming model system.
dictImportList_t    g_dictImports[MAX_TXD];

/*=========================================================
    RwTexDictionaryDefaultScanGlobalEnv

    Arguments:
        name - primary texture look-up name
        secName - secondary texture look-up name
    Purpose:
        Default callback for the global environment texture look-up.
        By default, GTA:SA does not do anything here. If a texture is
        returned from here, it is expected to be stand-alone; it will
        be added to the current TexDictionary. This callback has
        lower priority than the local environment one.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731710
=========================================================*/
static RwTexture* __cdecl RwTexDictionaryDefaultScanGlobalEnv( const char *name, const char *secName )
{
    return NULL;
}

/*=========================================================
    RwTexDictionaryDefaultCurrentScan

    Arguments:
        name - primary texture look-up name
    Purpose:
        Default callback for the local environment scan. It scans
        inside the current TexDictionary and all its parents by
        name. It returns the Texture it found. If the texture was
        not found in the current TXD and a global emitter is set,
        this TXD is scanned, too.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731720
=========================================================*/
inline static RwTexture* RwTexDictionaryScanTree( RwTexDictionary *txd, const char *name )
{
    while ( txd )
    {
        if ( RwTexture *inst = txd->FindNamedTexture( name ) )
            return inst;

        txd = txd->m_parentTxd;
    }

    return NULL;
}

static RwTexture* __cdecl RwTexDictionaryDefaultCurrentScan( const char *name )
{
    // First check the current TexDictionary tree
    if ( RwTexture *tex = RwTexDictionaryScanTree( RwTexDictionaryGetCurrent(), name ) )
        return tex;

    // MTA extension: also scan a global texture emitter which is controlled by MTA (low priority)
    return RwTexDictionaryScanTree( g_textureEmitter, name );
}

/*=========================================================
    Hook_InitTextureManager

    Purpose:
        Initializes four default TexDictionaries and the default
        look-up callbacks.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731F20 (FUNC_InitTextureManager)
=========================================================*/
static void __cdecl Hook_InitTextureManager( void )
{
    // Reserve 4 txds
    for ( unsigned int n = 0; n < 4; n++ )
        *( (unsigned short*)VAR_CPlayerTexDictionaries + n ) = pGame->GetTextureManager()->CreateTxdEntry( "*" );

    // Register some callbacks
    RwInterface *rwInterface = RenderWare::GetInterface();

    rwInterface->m_textureManager.findInstance = RwTexDictionaryDefaultScanGlobalEnv;
    rwInterface->m_textureManager.findInstanceRef = RwTexDictionaryDefaultCurrentScan;
}

CTextureManagerSA::CTextureManagerSA( void )
{
    // We init it ourselves
    HookInstall( FUNC_InitTextureManager, (DWORD)Hook_InitTextureManager, 6 );

    // We can initialize the pool here
    // This is where GTA:SA stores its texture.
    TextureManager::GetTxdPool() = new CTxdPool();

    // Initialize the dictionary list
    LIST_CLEAR( m_txdList.root );

    // Make sure we are monitoring game activity.
    InitGameHooks();
}

CTextureManagerSA::~CTextureManagerSA( void )
{
    delete TextureManager::GetTxdPool();
}

/*=========================================================
    CTextureManagerSA::FindTxdEntry

    Arguments:
        name - case in-sensitive string of the TXD name
    Purpose:
        Returns the index of a TexDictionary instance pool
        entry if found, otherwise -1.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731850
=========================================================*/
int CTextureManagerSA::FindTxdEntry( const char *name ) const
{
    return TxdFind( name );
}

/*=========================================================
    CTextureManagerSA::CreateTxdEntry

    Arguments:
        name - name of the new TexDictionary instance
    Purpose:
        Attempts to create a new TexDictionary instance and
        returns its index if successful, otherwise -1.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731C80
=========================================================*/
int CTextureManagerSA::CreateTxdEntry( const char *name )
{
    return TxdCreate( name );
}

/*=========================================================
    CTextureManagerSA::CreateTxd (MTA extension)

    Arguments:
        file - source stream which contains the TexDictionary
    Purpose:
        Attempts to read a TexDictionary from the source MTA stream
        and returns its virtual manager instance if successful.
=========================================================*/
CTexDictionarySA* CTextureManagerSA::CreateTxd( CFile *file )
{
    RwStream *stream = RwStreamCreateTranslated( file );

    if ( !stream )
        return NULL;

    if ( !RwStreamFindChunk( stream, 0x16, NULL, NULL ) )
    {
        RwStreamClose( stream, NULL );
        return NULL;
    }

    RwTexDictionary *dict = RwTexDictionaryStreamReadEx( stream );
    CTexDictionarySA *txd;

    if ( dict )
    {
        txd = new CTexDictionarySA( dict );

        // Set it as global emitter here
        txd->SetGlobalEmitter();

        LIST_INSERT( m_txdList.root, txd->m_dicts );    // We keep track of all tex dictionaries
    }
    else
        txd = NULL;

    RwStreamClose( stream, NULL );
    return txd;
}

/*=========================================================
    CTextureManagerSA::LoadDictionary

    Arguments:
        filename - MTA filepath pointing to TexDictionary location
    Purpose:
        Attempts to create a new TexDictionary instance and
        returns its index if successful, otherwise -1.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007320B0
    Note:
        The original function locks until the TexDictionary
        is available (not locked by filesystem). We are not
        doing that.
        This function is a combination of TxdCreate and TxdLoad.
=========================================================*/
int CTextureManagerSA::LoadDictionary( const char *filename )
{
    return LoadDictionaryEx( FileMgr::GetFileNameItem( filename ).c_str(), filename );
}

/*=========================================================
    CTextureManagerSA::LoadDictionary

    Arguments:
        name - name of the new TexDictionary instance
        filename - MTA filepath pointing to TexDictionary location
    
    [ see CTextureManagerSA::LoadDictionary ]
=========================================================*/
int CTextureManagerSA::LoadDictionaryEx( const char *name, const char *filename )
{
    int id = TxdCreate( name );

    if ( id == -1 )
        return -1;

    int iRet = TxdLoadEx( id, name, filename );

    if ( iRet == -1 )
    {
        TxdDestroy( id );
        
        id = -1;
    }

    return id;
}

/*=========================================================
    CTextureManagerSA::SetCurrentTexture

    Arguments:
        id - TXD instance index
    Purpose:
        Attempts to set the specified TXD instance as current
        TXD container and returns whether the operation was
        successful.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007319C0
=========================================================*/
bool CTextureManagerSA::SetCurrentTexture( unsigned short id )
{
    return TxdSetCurrentSafe( id );
}

/*=========================================================
    CTextureManagerSA::DeallocateTxdEntry

    Arguments:
        id - TXD instance index
    Purpose:
        Deallocates resources associated with the specified
        TXD instance index.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731E90
=========================================================*/
void CTextureManagerSA::DeallocateTxdEntry( unsigned short id )
{
    TxdDeallocate( id );
}

/*=========================================================
    CTextureManagerSA::RemoveTxdEntry

    Arguments:
        id - TXD instance index
    Purpose:
        Destroys the given TXD instance index so that all indices
        pointing to it turn invalid.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00731CD0
=========================================================*/
void CTextureManagerSA::RemoveTxdEntry( unsigned short id )
{
    TxdDestroy( id );
}