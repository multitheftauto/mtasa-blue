/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareExtensionSA.cpp
*  PURPOSE:     RenderWare extension management
*       In here you should put functions which do not fit into the RenderWare
*       framework by ~default and are utility based. The RenderWare framework
*       shall consist of the root code and all plugins.
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

static RwExtensionInterface **ppExtInterface = (RwExtensionInterface**)0x00C9B920;
#define pExtInterface   (*ppExtInterface)

static unsigned int *m_pNumRwExtensions = (unsigned int*)0x00C97900;
#define m_numRwExtensions   (*m_pNumRwExtensions)

/*=========================================================
    RpAtomicRenderAlpha

    Arguments:
        atom - atomic to render with special alpha
        alpha - alpha value to adjust materials to
    Purpose:
        Renders an atomic with a specified alpha by temporarily
        adjusting all its materials to the alpha value. Only
        materials with a higher alpha value are set to alpha.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732480
    Update:
        The GTA:SA function saved the material alpha values on the
        stack with a capacity of 152. If there were more than 152
        materials, the engine would crash. This function fixed this
        limitation by using heap memory, allocated by count.
=========================================================*/
void RpAtomicRenderAlpha( RpAtomic *atom, unsigned int alpha )
{
    // Fix to overcome material limit of 152 (yes, we actually reached that in GTA:United)
    RpGeometry *geom = atom->geometry;
    unsigned int _flags = geom->flags;
    unsigned int n;

    // Apply rendering flags. My guess is alpha blending?
    geom->flags |= 0x40;

    RpMaterials& mats = geom->materials;
    char *alphaVals = new char [mats.entries];

    // Store the atomic alpha values
    for ( n = 0; n < mats.entries; n++ )
    {
        RpMaterial& mat = *mats.data[n];
        unsigned char a = mat.color.a;

        alphaVals[n] = a;

        if ( a > alpha )
            mat.color.a = alpha;
    }

    // Render it
    RpAtomicRender( atom );

    // Restore values
    while ( n )
        mats.data[n]->color.a = alphaVals[--n];

    delete [] alphaVals;

    // Restore flags as they were previous to calling this function.
    geom->flags = _flags;
}

/*=========================================================
    _worldAtomicSceneCopyConstructor

    Arguments:
        atom - new plugin atomic instance
        src - source atomic, the one to clone
    Purpose:
        RpWorld plugin copy constructor. In MTA atomics should trade
        their scene details over to clones, so that every atomic will
        have a scene and dynamic lighting will work. In a strict sense,
        this is a hack.
    Binary offsets:
        (1.0 US): 0x007509A0
        (1.0 EU): 0x007509F0
=========================================================*/
static RpAtomic* __cdecl _worldAtomicSceneCopyConstructor( RpAtomic *atom, RpAtomic *src )
{
    atom->scene = src->scene;
    return atom;
}

/*=========================================================
    ReadCollisionFromClump

    Arguments:
        stream - RenderWare stream containing the collision data
    Purpose:
        When a GTA:SA collision extension is found in a clump stream,
        this handler is called. It allocates the collision data
        and assigns it to the model which requested it.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0041B1D0
=========================================================*/
CColLoaderModelAcquisition *g_clumpLoaderCOLAcquisition = NULL;

static RwStream* ReadCollisionFromClump( RwStream *stream, size_t size )
{
    // We do not wanna load if we are not requesting a collision.
    if ( !g_clumpLoaderCOLAcquisition )
        return stream;

    // We should not load invalid sized collisions
    if ( size < sizeof(ColModelFileHeader) )
        return stream;

    // Allocate the required buffer for the COL data
    char *buf = new char[size];

    // Read the collision from the RenderWare stream block
    if ( RwStreamReadBlocks( stream, buf, size ) != size )
    {
        delete buf;

        return NULL;
    }

    ColModelFileHeader& header = *(ColModelFileHeader*)buf;

    CColModelSAInterface *colModel = new CColModelSAInterface;

    // Load according to version
    switch( header.checksum )
    {
    case 'LLOC':
        LoadCollisionModel( buf + sizeof(ColModelFileHeader), colModel, NULL );
        break;
    case '2LOC':
        LoadCollisionModelVer2( buf + sizeof(ColModelFileHeader), header.size - 0x18, colModel, NULL );
        break;
    case '3LOC':
        LoadCollisionModelVer3( buf + sizeof(ColModelFileHeader), header.size - 0x18, colModel, NULL );
        break;
    case '4LOC':
        LoadCollisionModelVer4( buf + sizeof(ColModelFileHeader), header.size - 0x18, colModel, NULL );
        break;
    default:
        // Invalid checksum; maybe invalid version or bad chunk
        // Happens for RC CAM vehicle; by returning NULL we disabled this model.
        delete [] buf;
        delete colModel;

        return NULL;
    }

    // Allocate remaining/missing data
    colModel->AllocateData();

    delete [] buf;

    // Update the request.
    // The_GTA: Made the collision assignment logic request based; original loader at
    // CStreamingSA.utils.cpp - LoadClumpFilePersistent.
    g_clumpLoaderCOLAcquisition->SetCollision( colModel );

    return stream;
}

/*=========================================================
    CRwExtensionManagerSA::constructor

    Purpose:
        Initializes various RenderWare extensions. This interface
        has access to special RenderWare extensions of GTA:SA (Allocate).
=========================================================*/
CRwExtensionManagerSA::CRwExtensionManagerSA( void )
{
    // Patch some fixes
    HookInstall( 0x00732480, (DWORD)RpAtomicRenderAlpha, 5 );
    HookInstall( 0x0041B1D0, (DWORD)ReadCollisionFromClump, 5 );

    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        HookInstall( 0x007509F0, (DWORD)_worldAtomicSceneCopyConstructor, 5 );
        break;
    case VERSION_US_10:
        HookInstall( 0x007509A0, (DWORD)_worldAtomicSceneCopyConstructor, 5 );
        break;
    }

#if 0
    // Initialize global extensions
    RpGeometryStreamlineInit();
#endif
}

/*=========================================================
    CRwExtensionManagerSA::destructor

    Purpose:
        Shutdown the RenderWare extension management.
=========================================================*/
CRwExtensionManagerSA::~CRwExtensionManagerSA( void )
{
#if 0
    // Shutdown global extensions
    RpGeometryStreamlineShutdown();
#endif
}

/*=========================================================
    CRwExtensionManagerSA::Allocate
    (not investigated yet)

    Arguments:
        rwId - ID of the extension to allocate
        count - number of special data chunks to allocate
        size - ???
        unk - ???
    Purpose:
        Allocates a user-defined RenderWare extension interface.
        GTA:SA animation is using this function to allocate itself.
    Binary offsets:
        (1.0 US): 0x007CCE40
        (1.0 EU): 0x007CCE80
=========================================================*/
RwExtension* CRwExtensionManagerSA::Allocate( unsigned int rwId, unsigned int count, size_t size, unsigned int unk )
{
    unsigned int n;
    RwExtensionInterface *ext;
    RwExtension *inst;

    for ( n = 0; n < m_numRwExtensions; n++ )
    {
        if ((ext = &pExtInterface[n])->id == rwId)
            break;
    }

    if (n == m_numRwExtensions)
    {
        // Probably some error handler here
        return NULL;
    }

    inst = (RwExtension*)RenderWare::GetInterface()->m_memory.m_malloc( sizeof(RwExtension) + ext->structSize * count + ext->internalSize, 0 );

    inst->size = size;
    inst->count = count;
    inst->unknown = unk;

    inst->extension = ext;

    inst->data = (void*)(inst + 1);

    if ( ext->internalSize == 0 )
    {
        inst->pInternal = NULL;
        return inst;
    }

    inst->pInternal = (void*)((unsigned int)inst->data + ext->structSize * count);
    return inst;
}

/*=========================================================
    CRwExtensionManagerSA::Free

    Arguments:
        ext - pointer to extension interface
    Purpose:
        Frees the previously allocated extension interface.
    Binary offsets:
        (1.0 US): 0x007CCF10
        (1.0 EU): 0x007CCF50
=========================================================*/
void CRwExtensionManagerSA::Free( RwExtension *ext )
{
    // No idea if that is correct, i.e. cleanup?
    RenderWare::GetInterface()->m_memory.m_free( ext );
}

/*
    MTA RwStream Filesystem Wrapper Extension

    These RenderWare extension functions allow seemless intergration of
    the MTA:Eir FileSystem to RenderWare stream functions. This way we can theoretically
    load RenderWare chunks from special containers (i.e. a scripter has created his own
    multi-clump container format). 

    An isolated stream will terminate the MTA stream when it is closed. It could be
    used to inject a stream somewhere, or when the MTA stream handle is not required.

    A non-isolated stream will preserve the MTA stream when it is closed. The MTA stream
    handle can be reused this way.
*/

static int RwTranslatedIsoStreamClose( void *file )
{
    delete (CFile*)file;

    return 0;
}

static int RwTranslatedStreamClose( void *file )
{
    return 0;
}

static size_t RwTranslatedStreamRead( void *file, void *buffer, size_t length )
{
    return ((CFile*)file)->Read( buffer, 1, length );
}

static size_t RwTranslatedStreamWrite( void *file, const void *buffer, size_t length )
{
    return ((CFile*)file)->Write( buffer, 1, length );
}

static void* RwTranslatedStreamSeek( void *file, unsigned int offset )
{
    size_t endOff = ((CFile*)file)->GetSize() - (size_t)((CFile*)file)->Tell();
    unsigned int roff = std::min( offset, endOff );

    if ( roff == 0 )
        return NULL;

    return ( ((CFile*)file)->Seek( roff, SEEK_CUR ) == 0 ) ? file : NULL;
}

/*=========================================================
    RwStreamCreateTranslated (MTA extension)

    Arguments:
        file - MTA stream handle
    Purpose:
        Creates a RenderWare stream wrapper for a MTA stream.
        Returns NULL if the operation failed or the MTA stream
        handle is NULL.
=========================================================*/
RwStream* RwStreamCreateTranslated( CFile *file )
{
    if ( !file )
        return NULL;

    RwStreamTypeData data;
    data.callbackClose = RwTranslatedStreamClose;
    data.callbackRead = RwTranslatedStreamRead;
    data.callbackWrite = RwTranslatedStreamWrite;
    data.callbackSeek = RwTranslatedStreamSeek;
    data.ptr_callback = file;

    return RwStreamOpen( STREAM_TYPE_CALLBACK, STREAM_MODE_NULL, &data );
}

/*=========================================================
    RwStreamCreateIsoTranslated (MTA extension)

    Arguments:
        file - MTA stream handle
    Purpose:
        Transforms the MTA stream handle into a RenderWare stream.
        The MTA stream handle should not be reused after passing
        to this function. Returns NULL if the operation failed or
        the MTA stream handle is NULL.
=========================================================*/
RwStream* RwStreamCreateIsoTranslated( CFile *file )
{
    if ( !file )
        return NULL;

    RwStreamTypeData data;
    data.callbackClose = RwTranslatedIsoStreamClose;
    data.callbackRead = RwTranslatedStreamRead;
    data.callbackWrite = RwTranslatedStreamWrite;
    data.callbackSeek = RwTranslatedStreamSeek;
    data.ptr_callback = file;

    return RwStreamOpen( STREAM_TYPE_CALLBACK, STREAM_MODE_NULL, &data );
}

/*=========================================================
    RwStreamOpenTranslated (MTA extension)

    Arguments:
        path - MTA filepath descriptor
        mode - valid RenderWare stream mode (read, write or append)
    Purpose:
        Opens a RenderWare stream using a MTA filepath.
        OpenGlobalStream checks multiple fileroots for the given
        path. Invalid filepaths (not MTA or GTA related) are discarded
        in the process (returns NULL). Returns the isolated RenderWare
        stream if successful.
=========================================================*/
RwStream* RwStreamOpenTranslated( const char *path, RwStreamMode mode )
{
    CFile *file;

    switch( mode )
    {
    case STREAM_MODE_READ:
        file = OpenGlobalStream( path, "rb" );
        break;
    case STREAM_MODE_WRITE:
        file = OpenGlobalStream( path, "wb" );
        break;
    case STREAM_MODE_APPEND:
        file = OpenGlobalStream( path, "ab" );
        break;
    default:
        return NULL;
    }

    return RwStreamCreateIsoTranslated( file );
}

/*=========================================================
    RwTextureStreamReadEx (GTA:SA extension)

    Arguments:
        stream - RenderWare stream which contains the texture
    Purpose:
        Reads a RenderWare texture from the given stream and
        returns it. Returns NULL if...
            the version is invalid,
            there was no texture in the stream,
            the reading process failed or
            the data chunks were unable to be read.
        This function is used by GTA:SA to plugin-load a high
        quality texture.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00730E60
=========================================================*/
RwTexture* RwTextureStreamReadEx( RwStream *stream )
{
    unsigned int version;
    unsigned int size;

    if ( !RwStreamFindChunk( stream, 0x15, &size, &version ) )
        return NULL;

    if ( version < 0x34000 && version > 0x36003 )
    {
        RwError error;
        error.err1 = 1;
        error.err2 = -4;

        RwSetError( &error );
        return NULL;
    }

    // Here actually is performance measurement logic of GTA:SA
    // We do not require it, so I leave it out
    RwTexture *tex;

    if ( RenderWare::GetInterface()->m_readTexture( stream, &tex, size ) == 0 || !tex )
        return NULL;

    if ( !RwPluginRegistryReadDataChunks( (void*)0x008E23CC, stream, tex ) )
    {
        RwTextureDestroy( tex );
        return NULL;
    }

    // Apply special flags
    unsigned int flags = tex->flags_a;

    if ( flags == 1 )
        tex->flags_a = 2;
    else if ( flags == 3 )
        tex->flags_b = 4;

    // Note: MTA usually has a fxQuality hook here, but it did nothing for the texture loading.
    if ( *(bool*)0x00C87FFC && tex->anisotropy > 0 && g_effectManager->GetEffectQuality() > 1 )
        tex->anisotropy = pRwDeviceInfo->maxAnisotropy;

    return tex;
}

/*=========================================================
    RwTexDictionaryStreamReadEx (MTA extension)

    Arguments:
        stream - RenderWare stream which contains the TXD
    Purpose:
        Reads a RenderWare TexDictionary from a given stream and
        returns it. Returns NULL if...
            there is no TXD in the stream,
            the version is invalid,
            the texture blocks could not be read/were invalid,
            reading of individual HQ textures has failed or
            the TXD plugin data chunks could not be read.
        This function loads a high quality, GTA:SA style TexDictionary
        while performing the full logic like RenderWare wants us to.
    Binary offsets (derived only, RwTexDictionaryStreamRead):
        (1.0 US): 0x00804C30
        (1.0 EU): 0x00804C70
    Note:
        The GTA:SA streaming loader neglected version checks, system
        communication (RwDeviceSystemRequest) and plugin data chunk
        reading (it used quick&dirty texture functions). This function
        should be used to ensure texture quality consistency.
=========================================================*/
RwTexDictionary* RwTexDictionaryStreamReadEx( RwStream *stream )
{
    unsigned int size;
    unsigned int version;

    // Check whether we are a TexDictionary.
    if ( !RwStreamFindChunk( stream, 1, &size, &version ))
        return NULL;

    // We cannot load too old and not too new TXDs
    if ( version < 0x34000 && version > 0x36003 )
    {
        RwError error;
        error.err1 = 1;
        error.err2 = -4;

        RwSetError( &error );
        return NULL;
    }

    // The_GTA: fixed possible exploit based on block info being bigger than 4 (buffer overflow + code injection, at worst)
    if ( size < sizeof(RwBlocksInfo) )
        return NULL;

    RwBlocksInfo texBlocksInfo;

    if ( RwStreamReadBlocks( stream, &texBlocksInfo, sizeof(RwBlocksInfo) ) != sizeof(RwBlocksInfo) )
        return NULL;

    unsigned int rendStatus;

    // Request whether the system is ready (?).
    RwDeviceSystemRequest( RenderWare::GetInterface()->m_renderSystem, 0x16, rendStatus, 0, 0 );

    rendStatus &= 0xFF;

    if ( rendStatus != 0 && texBlocksInfo.unk != 0 && rendStatus != texBlocksInfo.unk )
        return NULL;

    RwTexDictionary *txd = RwTexDictionaryCreate();

    if ( !txd )
        return NULL;

    // Read all individual HQ textures.
    while ( texBlocksInfo.count-- )
    {
        RwTexture *texture = RwTextureStreamReadEx( stream );

        if ( !texture )
            goto txdParseError;

        texture->AddToDictionary( txd );
    }

    if ( !RwPluginRegistryReadDataChunks( (void*)0x008E23E4, stream, txd ) )
        goto txdParseError;

    return txd;

    // Common error handler for this function after creating the TXD
txdParseError:
    LIST_FOREACH_BEGIN( RwTexture, txd->textures.root, TXDList )
        RwTextureDestroy( item );
    LIST_FOREACH_END

    RwTexDictionaryDestroy( txd );
    return NULL;
}