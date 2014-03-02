/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.utils.cpp
*  PURPOSE:     Data streamer utilities
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

#include "CStreamingSA.utils.hxx"

// This file includes examples of clever goto usage.

extern CBaseModelInfoSAInterface **ppModelInfo;
static RtDictSchema *const animDict =   (RtDictSchema*)0x008DED50;
static CModelLoadInfoSA *const VAR_ModelLoadInfo = (CModelLoadInfoSA*)0x008E4CC0;

namespace Streaming
{
    streamingLoadCallback_t  streamingLoadCallback = NULL;
};

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
        prevStackScan = pRwInterface->m_textureManager.findInstanceRef;
        pRwInterface->m_textureManager.findInstanceRef = scanMethod;
    }

    // Restores the previous texture scanner
    // Binary offsets: (1.0 US and 1.0 EU): 0x004C75C0
    void Unapply( void )
    {
        pRwInterface->m_textureManager.findInstanceRef = prevStackScan;
        prevStackScan = NULL;
    }
};

#ifdef RENDERWARE_VIRTUAL_INTERFACES
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

    static RwTexture* scanMethod( const char *name )
    {
        dictImportList_t& imported = g_dictImports[txdId];

        for ( dictImportList_t::const_iterator iter = imported.begin(); iter != imported.end(); iter++ )
        {
            if ( stricmp( (*iter)->GetName(), name ) == 0 )
                return (*iter)->GetTexture();
        }

        return prevStackScan( name );
    }

    void Apply( unsigned short id )
    {
        // Performance improvement: only apply this handler if we actually have imported textures.
        if ( !g_dictImports[id].empty() )
        {
            prevStackScan = pRwInterface->m_textureManager.m_findInstanceRef;
            pRwInterface->m_textureManager.m_findInstanceRef = scanMethod;

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
            pRwInterface->m_textureManager.m_findInstanceRef = prevStackScan;
            prevStackScan = NULL;
        }
    }
};
#endif //RENDERWARE_VIRTUAL_INTERFACES

/*=========================================================
    _RpGeometryAllocateNormals (MTA extension)

    Arguments:
        geom - geometry to which you want to attach normals
        mesh - 3d vertice data which requires normals and belongs to geom
    Purpose:
        Calculates "vertex normals" for a mesh from a given geometry.
        They are required to apply position dependent (local) lighting
        to an atomic. The normals array is returned.
=========================================================*/
static inline CVector* _RpGeometryAllocateNormals( RpGeometry *geom, RpGeomMesh *mesh )
{
    CVector *normals = (CVector*)RwAllocAligned( sizeof(CVector) * geom->verticeSize, 0x10 );

    for ( unsigned int n = 0; n < geom->verticeSize; n++ )
    {
        CVector& norm = normals[n];
        norm.fX = 0; norm.fY = 0; norm.fZ = 0;

        for ( unsigned int i = 0; i < geom->triangleSize; i++ )
        {
            const RpTriangle& tri = geom->triangles[i];

            // If the vertex connects to any point of the triangle...
            if ( tri.v1 == n || tri.v2 == n || tri.v3 == n )
            {
                // ... we should adjust the triangle normal.
                const CVector& origin = mesh->positions[tri.v1];
                CVector v1 = mesh->positions[tri.v2] - origin;
                const CVector v2 = mesh->positions[tri.v3] - origin;
                v1.CrossProduct( v2 );
                v1.Normalize();

                norm += v1;
            }
        }

        norm.Normalize();
    }

    return normals;
}

/*=========================================================
    _initAtomScene (MTA extension)

    Arguments:
        atom - atomic which should be included into a scene
    Purpose:
        Includes the given atomic into the default GTA:SA scene.
        Then it recalculates the normals for all its meshes.
        By that dynamic lighting will be enabled.
=========================================================*/
static void _initAtomScene( RpAtomic *atom )
{
    // Apply the default GTA:SA scene
    atom->scene = *p_gtaScene;

    // TODO: reenable this using multi-threading (streamline extension!)
    //return;

    RpGeometry& geom = *atom->geometry;

    if ( !( geom.flags & RW_GEOMETRY_NORMALS ) )
    {
        // Allocate normals for every mesh
        for ( unsigned int n = 0; n < geom.numMeshes; n++ )
        {
            RpGeomMesh& mesh = geom.meshes[n];

            if ( !mesh.normals )
                mesh.normals = _RpGeometryAllocateNormals( &geom, &mesh );
        }

        if ( !geom.skeleton )
            geom.flags |= RW_GEOMETRY_NO_SKIN;

        geom.flags |= RW_GEOMETRY_NORMALS;
    }
}

/*=========================================================
    RpClumpAtomicActivator

    Arguments:
        atom - atomic to set as model for an atomic model info
        replacerId - id of the atomic model info
    Purpose:
        Loads an atomic model info with the given atomic. The atomic
        is registered as the official representative of that model info.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00537150 (FUNC_AtomicsReplacer)
=========================================================*/
void RpClumpAtomicActivator( RpAtomic *atom, modelId_t replacerId )
{
    CAtomicModelInfoSA *atomInfo = ppModelInfo[replacerId]->GetAtomicModelInfo();
    bool unk;
    char unk2[24];

    // This possibly adds the reference to the texture, we should reven this
    ((void (__cdecl*)(const char*, char*, bool&))0x005370A0)( atom->parent->szName, unk2, unk );

    atom->SetRenderCallback( NULL );

    _initAtomScene( atom );

    if ( unk )
        atomInfo->GetDamageAtomicModelInfo()->SetupPipeline( atom );
    else
        atomInfo->SetAtomic( atom );

    atom->RemoveFromClump();

    atom->AddToFrame( RwFrameCreate() );
    
    atom->modelId = replacerId;
}

/*=========================================================
    _initClumpScene (MTA extension)

    Arguments:
        clump - RpClump whose atomics should be included into the scene
    Purpose:
        Prepares a clump for usage with the dynamic lighting system.
        This means that its atomics are added to the default GTA:SA
        scene. Atomics have to belong to a scene so that they traverse
        along sectors. Sectors also contain the lights, so lights are
        properly adjusted.
=========================================================*/
inline static void _initClumpScene( RpClump *clump )
{
    LIST_FOREACH_BEGIN( RpAtomic, clump->atomics.root, atomics )
        _initAtomScene( item );
    LIST_FOREACH_END
}

/*=========================================================
    LoadClumpFile

    Arguments:
        stream - binary stream which contains the clump file
        model - id of a atomic model info
    Purpose:
        Loads a clump file from the stream. The RpClump is expected
        to have one atomic. For every atomic it calls
        RpClumpAtomicActivator which removes it from the clump and
        adds it to the atomic model info. More than one atomic may
        be applied to the model info if one parent frame name designates
        a damage atomic model info and the other does not. For this
        function to succeed, there has to be a non-damage atomic
        inside the clump.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005371F0
=========================================================*/
static bool __cdecl LoadClumpFile( RwStream *stream, modelId_t model )
{
    CAtomicModelInfoSA *atomInfo = ppModelInfo[model]->GetAtomicModelInfo();
    bool appliedRemapCheck, result;

#ifdef RENDERWARE_VIRTUAL_INTERFACES
    // MTA extension: Apply our global imports
    RwImportedScan::Apply( atomInfo->usTextureDictionary );
#endif //RENDERWARE_VIRTUAL_INTERFACES

    if ( atomInfo && ( atomInfo->collFlags & COLL_WETROADREFLECT ) )
    {
        RwRemapScan::Apply();
        appliedRemapCheck = true;
    }
    else
        appliedRemapCheck = false;

    result = false;

    if ( RwStreamFindChunk( stream, 0x10, NULL, NULL ) )
    {
        if ( RpClump *clump = RpClumpStreamRead( stream ) )
        {
            while ( !LIST_EMPTY( clump->atomics.root ) )
                RpClumpAtomicActivator( LIST_GETITEM( RpAtomic, clump->atomics.root.next, atomics ), model );

            RpClumpDestroy( clump );

            result = atomInfo->GetRwObject() != NULL;
        }
    }

    if ( appliedRemapCheck )
        RwRemapScan::Unapply();

#ifdef RENDERWARE_VIRTUAL_INTERFACES
    RwImportedScan::Unapply();
#endif //RENDERWARE_VIRTUAL_INTERFACES

    return result;
}

/*=========================================================
    LoadClumpFilePersistent

    Arguments:
        stream - binary stream which contains the clump file
        id - clump model info id
    Purpose:
        Takes the stream and loads either a single or a multi clump
        into the given clump model info. Returns true if there
        was no error during loading.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005372D0
=========================================================*/
static bool __cdecl LoadClumpFilePersistent( RwStream *stream, modelId_t id )
{
    CClumpModelInfoSAInterface *info = (CClumpModelInfoSAInterface*)ppModelInfo[id];

    // Not sure about this flag anymore. Apparently it stands for multi-clump here.
    if ( info->renderFlags & RENDER_NOSKELETON )
    {
        RpClump *clump = RpClumpCreate();
        RwFrame *frame = clump->parent = RwFrameCreate();

#ifdef RENDERWARE_VIRTUAL_INTERFACES
        RwImportedScan::Apply( info->usTextureDictionary );
#endif //RENDERWARE_VIRTUAL_INTERFACES

        while ( RwStreamFindChunk( stream, 0x10, NULL, NULL ) )
        {
            RpClump *item = RpClumpStreamRead( stream );

            if ( !item )
            {
                // Small memory leak fix
                RwFrameDestroy( frame );
                clump->parent = NULL;

#ifdef RENDERWARE_VIRTUAL_INTERFACES
                RwImportedScan::Unapply();
#endif //RENDERWARE_VIRTUAL_INTERFACES

                RpClumpDestroy( clump );
                return false;
            }
    
            RwFrame *clonedParent = item->parent->CloneRecursive();

            frame->Link( clonedParent );
            
            while ( !LIST_EMPTY( item->atomics.root ) )
            {
                RpAtomic *atom = LIST_GETITEM( RpAtomic, item->atomics.root.next, atomics );

                atom->AddToFrame( atom->parent->root );
                atom->AddToClump( clump );

                _initAtomScene( atom );
            }

            RpClumpDestroy( item );
        }

#ifdef RENDERWARE_VIRTUAL_INTERFACES
        RwImportedScan::Unapply();
#endif //RENDERWARE_VIRTUAL_INTERFACES

        info->SetClump( clump );
        return true;
    }

    bool isVehicle = info->GetModelType() == MODEL_VEHICLE;
    
    if ( !RwStreamFindChunk( stream, 0x10, NULL, NULL ) )
        return false;

#ifdef RENDERWARE_VIRTUAL_INTERFACES
    // MTA extension: include our imported textures
    RwImportedScan::Apply( info->usTextureDictionary );
#endif //RENDERWARE_VIRTUAL_INTERFACES

    CColLoaderModelAcquisition *colAcq;

    if ( isVehicle )
    {
        colAcq = new CColLoaderModelAcquisition;
        RwRemapScan::Apply();
    }

    RpClump *clump = RpClumpStreamRead( stream );

    if ( isVehicle )
    {
        if ( clump )
        {
            // See if we loaded a collision
            CColModelSAInterface *col = colAcq->GetCollision();

            // If we did not load one, we are likely to crash;
            // At least give the vehicle an empty collision interface.
            if ( !col )
            {
                col = new CColModelSAInterface;

                col->AllocateData();
            }

            // If we replaced the collision, we want to put it into the original storage
            // Loading it into the model does not make sense ;)
            if ( CColModelSA *colModel = g_colReplacement[id] )
            {
                colModel->SetOriginal( col, true );
            }
            else
            {
                // Set the collision to the model.
                info->SetColModel( col, true );

                // Set a special model flag
                info->flags |= 0x0800;
            }
        }

        RwRemapScan::Unapply();
        delete colAcq;
    }

#ifdef RENDERWARE_VIRTUAL_INTERFACES
    RwImportedScan::Unapply();
#endif //RENDERWARE_VIRTUAL_INTERFACES

    if ( !clump )
        return false;

    _initClumpScene( clump );

    info->SetClump( clump );

    // Game fix??? R*
    if ( id == VT_JOURNEY )
        ((CVehicleModelInfoSAInterface*)info)->numberOfDoors &= 0x02;

    return true;
}

/*=========================================================
    ReadClumpBigContinue

    Arguments:
        stream - binary stream which contains the clump file
    Purpose:
        Reads a RenderWare clump from the given stream and returns
        it. Returns NULL if the clump is invalid.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0072E620
    Note:
        This function is never used in GTA:SA.
=========================================================*/
static RpClump* __cdecl ReadClumpBigContinue( RwStream *stream )
{
    RpClump *clump = RpClumpCreate();

    if ( !clump )
        return NULL;

    // Get to the current stream offset
    RwStreamSkip( stream, *(unsigned int*)0x00C87AFC - stream->data.buffered.position );

    // The_GTA: since this function is never used and there are much better
    // ways to load RenderWare resources, I decide to leave this old code out.
    // The way of the future is fibered loading! We shall utilize it in
    // MTA:BLUE 2.0 to increase overall performance of MTA. It is a way
    // to load resources using multiple threads.

    return clump;
}

/*=========================================================
    LoadClumpFileBigContinue

    Arguments:
        stream - binary stream which contains the clump file
        id - clump model info id
    Purpose:
        Takes the stream and loads a big clump into the requested
        model info. Returns whether the clump was successfully
        loaded. This function continues a previous clump read
        request.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00537450
    Note:
        This function is never used in GTA:SA.
=========================================================*/
static bool __cdecl LoadClumpFileBigContinue( RwStream *stream, modelId_t id )
{
    CClumpModelInfoSAInterface *info = (CClumpModelInfoSAInterface*)ppModelInfo[id];

    assert( info->GetRwModelType() == RW_CLUMP );

    bool isVehicle = info->GetModelType() == MODEL_VEHICLE;

    if ( isVehicle )
        RwRemapScan::Apply();

#ifdef RENDERWARE_VIRTUAL_INTERFACES
    // Do load our imported textures
    RwImportedScan::Apply( id );
#endif //RENDERWARE_VIRTUAL_INTERFACES

    RpClump *clump = ReadClumpBigContinue( stream );

#ifdef RENDERWARE_VIRTUAL_INTERFACES
    // Pop the texture scanners
    RwImportedScan::Unapply();
#endif //RENDERWARE_VIRTUAL_INTERFACES

    if ( isVehicle )
        RwRemapScan::Unapply();

    if ( clump )
    {
        info->SetClump( clump );
        return true;
    }

    return false;
}

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

    RwBlocksInfo info;

    if ( RwStreamReadBlocks( stream, &info, length ) != length )
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
static RwTexDictionary* RwTexDictionaryContinueLoading( RwStream *stream, RwTexDictionary *txd )
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
static bool __cdecl LoadTXDFirstHalf( unsigned int id, RwStream *stream )
{
    CTxdInstanceSA *txd = (*ppTxdPool)->Get( id );

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
static bool __cdecl LoadTXDContinue( unsigned int id, RwStream *stream )
{
    CTxdInstanceSA *txdInst = (*ppTxdPool)->Get( id );

    RwTexDictionary *txd = txdInst->m_txd = RwTexDictionaryContinueLoading( stream, txdInst->m_txd );

    if ( txd )
        txdInst->InitParent();

    return txd != NULL;
}

/*=========================================================
    RegisterCOLLibraryModel

    Arguments:
        collId - index of the COL library
        modelId - model info id to which a collision was applied
    Purpose:
        Extends the range of applicability for the given COL lib.
        This loading of collisions for said COL library can be
        limited to a range so that future loading attempts are
        boosted.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00410820
=========================================================*/
void __cdecl RegisterCOLLibraryModel( unsigned short collId, unsigned short modelId )
{
    CColFileSA *col = (*ppColFilePool)->Get( collId );

    if ( (short)modelId < col->m_rangeStart )
        col->m_rangeStart = (short)modelId;

    if ( (short)modelId > col->m_rangeEnd )
        col->m_rangeEnd = (short)modelId;
}

/*=========================================================
    ReadCOLLibraryGeneral

    Arguments:
        buf - binary string of the COL library
        size - size of the memory pointed at by buf
        collId - COL library index
    Purpose:
        Reads a COL library from memory pointed at by buf and assigns all
        collision entries to the COL library designated with collId. During
        the scan for models which need collisions the range of applicance
        is expanded. The next loading of this COL library will be boosted.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B5000
    Update:
        Added support for version 4 collision.
=========================================================*/
static bool __cdecl ReadCOLLibraryGeneral( const char *buf, size_t size, unsigned char collId )
{
    CBaseModelInfoSAInterface *info = NULL;

    while ( size > 8 )
    {
        const ColModelFileHeader& header = *(const ColModelFileHeader*)buf;

        buf += sizeof(header);

        if ( header.checksum != '4LOC' && header.checksum != '3LOC' && header.checksum != '2LOC' && header.checksum != 'LLOC' )
            return true;

        modelId_t modelId = header.modelId;

        // Collisions may come with a cached model id.
        // Valid ids skip the need for name-checking.
        if ( modelId < DATA_TEXTURE_BLOCK )
            info = ppModelInfo[modelId];

        unsigned int hash = pGame->GetKeyGen()->GetUppercaseKey( header.name );

        if ( !info || hash != info->GetHashKey() )
            info = Streaming::GetModelByHash( hash, &modelId );

        // I am not a fan of uselessly big scopes.
        // The closer the code is to the left border, the easier it is to read for everybody.
        // Compilers do optimize goto.
        if ( !info )
            goto skip;

        // Update collision boundaries
        RegisterCOLLibraryModel( collId, modelId );

        // I do not expect collision replacements to be loaded this early.
        // The engine does preload the world collisions once. Further
        // loadings will be faster due to limitation of model scans to
        // id regions.
        assert( g_colReplacement[modelId] == NULL );

        if ( !info->IsDynamicCol() )
            goto skip;

        CColModelSAInterface *col = new CColModelSAInterface();

        switch( header.checksum )
        {
        case '4LOC':
            LoadCollisionModelVer4( buf, header.size - 0x18, col, header.name );
            break;
        case '3LOC':
            LoadCollisionModelVer3( buf, header.size - 0x18, col, header.name );
            break;
        case '2LOC':
            LoadCollisionModelVer2( buf, header.size - 0x18, col, header.name );
            break;
        default:
            LoadCollisionModel( buf, col, header.name );
            break;
        }

        // MTA extension: Put it into our global storage
        g_originalCollision[modelId] = col;

        col->m_colPoolIndex = collId;

        info->SetColModel( col, true );
        SetCachedCollision( modelId, col );

skip:
        size -= header.size;
        buf += header.size - (sizeof(ColModelFileHeader) - 8);
    }

    return true;
}

/*=========================================================
    ReadCOLLibraryBounds

    Arguments:
        buf - binary string of the COL library
        size - size of the memory pointed at by buf
        collId - COL library index
    Purpose:
        Reads a COL library from memory pointed at by buf and assigns all
        collision entries to the COL library designated with collId. It uses
        the cached range of applicancy to boost name-based collision loading.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00538440
=========================================================*/
static bool __cdecl ReadCOLLibraryBounds( const char *buf, size_t size, unsigned char collId )
{
    CBaseModelInfoSAInterface *info = NULL;

    while ( size > 8 )
    {
        const ColModelFileHeader& header = *(const ColModelFileHeader*)buf;

        buf += sizeof(header);

        // Note: this function has version 4 support by default!
        // It never worked properly since ReadCOLLibraryGeneral rejected version 4 by default.
        // We have updated the engine by allowing version 4 in ReadCOLLibraryGeneral!
        if ( header.checksum != '4LOC' && header.checksum != '3LOC' && header.checksum != '2LOC' && header.checksum != 'LLOC' )
            return true;

        modelId_t modelId = header.modelId;

        if ( modelId < DATA_TEXTURE_BLOCK )
            info = ppModelInfo[modelId];

        unsigned int hash = pGame->GetKeyGen()->GetUppercaseKey( header.name );

        if ( !info || hash != info->GetHashKey() )
        {
            CColFileSA *colFile = (*ppColFilePool)->Get( collId );

            info = Streaming::GetModelInfoByName( header.name, (unsigned short)colFile->m_rangeStart, (unsigned short)colFile->m_rangeEnd, &modelId );
        }

        if ( info )
        {
            // Wire in a MTA team fix.
            if ( OnMY_CFileLoader_LoadCollisionFile_Mid( modelId ) )
            {
                CColModelSAInterface *col;

                if ( CColModelSA *colInfo = g_colReplacement[modelId] )
                {
                    // MTA extension: We store it in our data, so we can restore to it later
                    col = colInfo->GetOriginal();

                    if ( !col )
                        colInfo->SetOriginal( col = new CColModelSAInterface(), true );
                }
                else
                {
                    // Clever usage of goto here; in order to save memory, we let the original collision interfaces unload their data.
                    // We have to put the data back into the col-replacement original info, which would not work if the model info is not set
                    // to have a dynamic collision. Dynamic collisions always reside in COL libraries, which COL replacements are not.
                    // I had to restructure the loading here, and the best way is to use a goto.
                    // If you disagree, troll me at IRC.
                    if ( !info->IsDynamicCol() )
                        goto skip;

                    // The original route
                    col = info->pColModel;

                    if ( !col )
                    {
                        col = new CColModelSAInterface();

                        info->SetColModel( col, true );
                    }
                }

                switch( header.checksum )
                {
                case '4LOC':
                    LoadCollisionModelVer4( buf, header.size - 0x18, col, header.name );
                    break;
                case '3LOC':
                    LoadCollisionModelVer3( buf, header.size - 0x18, col, header.name );
                    break;
                case '2LOC':
                    LoadCollisionModelVer2( buf, header.size - 0x18, col, header.name );
                    break;
                default:
                    LoadCollisionModel( buf, col, header.name );
                    break;
                }

                // MTA extension: Put it into our global storage
                g_originalCollision[modelId] = col;

                col->m_colPoolIndex = collId;

                // Do some procedural object logic
                if ( info->GetModelType() == MODEL_ATOMIC )
                    ((bool (__cdecl*)( CBaseModelInfoSAInterface *info ))0x005DB650)( info );
            }
        }
        
skip:
        size -= header.size;
        buf += header.size - (sizeof(ColModelFileHeader) - 8);
    }

    return true;
}

/*=========================================================
    LoadCOLLibrary

    Arguments:
        collId - COL library index
        buf - binary string of the COL library
        size - size of the memory pointed at by buf
    Purpose:
        Loads a COL library into the container designated by collId.
        If no range has been cached to the COL library, it executes
        ReadCOLLibraryGeneral. Otherwise the loading is accelerated
        by knowing the model id bounds.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004106D0
=========================================================*/
static bool __cdecl LoadCOLLibrary( unsigned char collId, const char *buf, size_t size )
{
    CColFileSA *col = (*ppColFilePool)->Get( collId );
    bool success;

    // If the library was previously loaded and knows its regions, call ReadCOLLibaryBounds.
    // Otherwise we perform a global replacement (ReadCOLLibraryGeneral), to cache the id region.
    if ( col->m_rangeStart > col->m_rangeEnd )
        success = ReadCOLLibraryGeneral( buf, size, collId );
    else
        success = ReadCOLLibraryBounds( buf, size, collId );

    // Mark that the library is ready for interaction!
    if ( success )
        col->m_loaded = true;

    return success;
}

/*=========================================================
    FreeCOLLibrary

    Arguments:
        collId - COL library index
    Purpose:
        Unloads a specific COL library and deletes the collision
        data from affected models.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00410730
    Note:
        This function has been updated so it is compatible with
        the MTA model loading system. Models whose collisions were
        replaced are not affected by this function anymore.
=========================================================*/
void __cdecl FreeCOLLibrary( unsigned char collId )
{
    CColFileSA *col = (*ppColFilePool)->Get( collId );

    // Mark this COL library as unloaded.
    col->m_loaded = false;

    for ( short n = col->m_rangeStart; n <= col->m_rangeEnd; n++ )
    {
        CBaseModelInfoSAInterface *info = ppModelInfo[n];

        if ( !info )
            continue;

        CColModelSAInterface *colModel;
        bool isDynamic;

        // MTA extension: Handle collision replacements by MTA.
        // We should free the original collision, as we will obtain it again later
        // during COL library loading. This saves quite some memory in total
        // conversions.
        if ( CColModelSA *c_col = g_colReplacement[n] )
        {
            colModel = c_col->GetOriginal();
            isDynamic = c_col->IsOriginalDynamic();
        }
        else
        {
            colModel = info->pColModel;
            isDynamic = info->IsDynamicCol();
        }

        if ( colModel && isDynamic && colModel->m_colPoolIndex == collId )
            colModel->ReleaseData();
    }
}

/*=========================================================
    CheckTXDDependency

    Arguments:
        id - index of the TXD instance to check dependency on resources of
    Purpose:
        Returns whether the given TXD instance is required by any
        model or not. If this function returns false, the resource
        loader should not bother about loading that TXD instance to
        save streaming memory. Thus, this function is a TXD instance
        Garbage Collector check.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00409A90
=========================================================*/
// Generic dependency routine
template <class checkType, class dependType>
inline bool CheckDependency( dependType id )
{
    using namespace Streaming;
    checkType checker;
    
    // Check the queued requests
    {
        CModelLoadInfoSA *item = GetQueuedLoadInfo();
        CModelLoadInfoSA *endItem = *(CModelLoadInfoSA**)0x008E4C54;    // toBeLoadedQueue
        modelId_t itemId = item->GetIndex();

        for ( ; item != endItem; item = &GetModelLoadInfo( itemId = item->m_primaryModel ) )
        {
            // The_GTA: I optimized the id calculation, so it only calculates it once (for the first requested item)
            // An example of how compilers cannot be perfect. Developers should always try to optimize their code.
            // Do not listen to people who want to prevent you from optimizing.
            if ( checker( itemId, id ) )
                return true;
        }
    }

    // Check the streaming requests.
    // The_GTA: I fixed a bug in here which made invalid memory requests if anything else
    // than models or textures were requested in here. I do not know how R* could mess this up.
    // Hard to explain, since their code does not make sense :P
    for ( unsigned char n = 0; n < 2; n++ )
    {
        for ( unsigned char i = 0; i < MAX_STREAMING_REQUESTS; i++ )
        {
            modelId_t itemId = GetStreamingRequest( n ).ids[i];

            if ( itemId != -1 && checker( itemId, id ) )
                return true;
        }
    }

    // There is no dependency on that instance.
    // Hence we do not require that instance loaded.
    // If we want instances loaded in MTA anyway, it could be interesting to add our
    // own hook for dependency logic here!
    return false;
}

struct ModelTXDDependency
{
    inline bool operator() ( modelId_t model, unsigned short txdId )
    {
        return model < DATA_TEXTURE_BLOCK && (unsigned short)ppModelInfo[model]->usTextureDictionary == txdId ||
              idOffset( model, DATA_TEXTURE_BLOCK ) < MAX_TXD && (*ppTxdPool)->Get( idOffset( model, DATA_TEXTURE_BLOCK ) )->m_parentTxd == txdId;
    }
};

bool __cdecl CheckTXDDependency( modelId_t id )
{
    return CheckDependency <ModelTXDDependency> ( id );
}

/*=========================================================
    CheckAnimDependency

    Arguments:
        id - index of the TXD instance to check dependency on resources of
    Purpose:
        Returns whether the given animation is required by any
        model or not. If this function returns false, the resource
        loader should not bother about loading it (saves streaming memory).
        Thus, this function is an animation Garbage Collector check.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00407AD0
=========================================================*/
struct ModelAnimDependency
{
    inline bool operator() ( modelId_t model, int animId )
    {
        return model < DATA_TEXTURE_BLOCK && ppModelInfo[model]->GetAnimFileIndex() == animId;
    }
};

bool __cdecl CheckAnimDependency( modelId_t id )
{
    // It is pretty crazy how you can optimize it, right? :P
    // Instead of doubling the code, you can use templates!
    // And, it is very smart to use generic callbacks, too.
    // Read about functors (like ModelAnimDependency), Cazomino05.
    return CheckDependency <ModelAnimDependency> ( id );
}

/*=========================================================
    LoadModel

    Arguments:
        buf - memory buffer which contains the object
        id - index of the model/resource info
        threadId - index of the thread which requested the load (unused)
    Purpose:
        Attempts to load a requested resource into the model info
        at id. It gets executed after a thread loaded the resource
        contents from the IMG file. Returns whether the loading
        was successful.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040C6B0
    Note:
        This function has been updated so it is compatible with
        the MTA model loading system. Collisions are now properly assigned
        to vehicles whose collisions were replaced by MTA.

        SCM script loading has been temporarily disabled, since MTA
        does not use it.
=========================================================*/
bool __cdecl LoadModel( void *buf, modelId_t id, unsigned int threadId )
{
    CModelLoadInfoSA& loadInfo = VAR_ModelLoadInfo[id];

    RwBuffer streamBuffer;
    streamBuffer.ptr = buf;
    streamBuffer.size = loadInfo.GetSize();     // we are reading from IMG chunks

    // Create a stream
    RwStream *stream = RwStreamInitialize( (void*)0x008E48AC, 0, STREAM_TYPE_BUFFER, STREAM_MODE_READ, &streamBuffer );

    if ( id < DATA_TEXTURE_BLOCK )
    {
        CBaseModelInfoSAInterface *info = ppModelInfo[id];
        int animIndex = info->GetAnimFileIndex();

        CTxdInstanceSA *txdInst = (*ppTxdPool)->Get( info->usTextureDictionary );
        CAnimBlockSAInterface *animBlock;

        if ( txdInst->m_txd == NULL )
            goto failure;

        if ( animIndex != 0xFFFFFFFF )
        {
            animBlock = pGame->GetAnimManager()->GetAnimBlock( animIndex );

            if ( !animBlock->IsLoaded() )
                goto failure;
        }
        else
            animBlock = NULL;

        // Reference the resources
        txdInst->Reference();
        
        // ... and anim block
        if ( animBlock )
            animBlock->Reference();

        txdInst->SetCurrent();

        eRwType type = info->GetRwModelType();
        bool success;

        if ( type == RW_ATOMIC )
        {
            RtDict *dict;

            RwChunkHeader header;
            RwStreamReadChunkHeaderInfo( stream, header );

            // Check if there is animation attached
            if ( header.id == 0x2B )
                animDict->current = dict = RtDictSchemaStreamReadDict( animDict, stream );
            else
                dict = NULL;

            // At this point, GTA_SA utilizes a weird stream logic
            // I have fixed it here (bad clean-up of stream pointers)
            RwStreamClose( stream, &streamBuffer );
            stream = RwStreamInitialize( (void*)0x008E48AC, 0, STREAM_TYPE_BUFFER, STREAM_MODE_READ, &streamBuffer );

            success = LoadClumpFile( stream, id );

            if ( dict )
                RtDictDestroy( dict );
        }
        else
            success = LoadClumpFilePersistent( stream, id );

        if ( loadInfo.m_eLoading != MODEL_RELOAD )
        {
            txdInst->DereferenceNoDestroy();

            if ( animBlock )
                animBlock->Dereference();

            if ( !success )
                goto failure;
          
            if ( info->GetModelType() == MODEL_VEHICLE )
                success = ((bool (__cdecl*)( unsigned int id ))0x00408000)( id );
        }

        if ( !success )
            goto failure;
    }
    else if ( id < DATA_TEXTURE_BLOCK + MAX_TXD )
    {
        modelId_t txdId = idOffset( id, DATA_TEXTURE_BLOCK );
        CTxdInstanceSA *txdInst = (*ppTxdPool)->Get( txdId );

        if ( txdInst->m_parentTxd != 0xFFFF )
        {
            if ( !(*ppTxdPool)->Get( txdInst->m_parentTxd )->m_txd )
                goto failure;
        }

        // Check whether we depend on this TXD instance (unless we specifically want it without dependency)
        if ( !( loadInfo.m_flags & FLAG_NODEPENDENCY ) && !CheckTXDDependency( txdId ) )
            goto failureDamned;

        bool successLoad;

        if ( *(bool*)0x008E4A58 )
        {
            successLoad = LoadTXDFirstHalf( txdId, stream );

            if ( !successLoad )
                goto failure;

            loadInfo.m_eLoading = MODEL_RELOAD;
        }
        else
        {
            successLoad = txdInst->LoadTXD( stream );

            if ( successLoad )
                txdInst->InitParent();
        }

        if ( !successLoad )
            goto failure;
    }
    else if ( id < 25255 )  // collision
    {
        if ( !LoadCOLLibrary( (unsigned char)( id - 25000 ), (const char*)buf, streamBuffer.size ) )
            goto failure;
    }
    else if ( id < 25511 )
    {
        if ( !((bool (__cdecl*)( modelId_t iplId, const void *data, size_t size ))0x00406080)( id - 25255, buf, streamBuffer.size ) )
            goto failure;
    }
    else if ( id < 25575 )
    {
        __asm
        {
            mov eax,id
            sub eax,25511
            push eax
            push stream
            mov ecx,CLASS_CPathFind
            mov eax,0x004529F0
            call eax
        }
    }
    else if ( id < 25755 )
    {
        if ( loadInfo.m_flags & FLAG_NODEPENDENCY || CheckAnimDependency( id - 25575 ) )
        {
            if ( loadInfo.m_blockCount == 0 )
                goto failureDamned;

            pGame->GetAnimManager()->LoadAnimFile( stream, true, NULL );
            pGame->GetAnimManager()->CreateAnimAssocGroups();
        }
        else
            goto failureDamned;
    }
    else if ( id < 26230 )
    {
        ((void (__cdecl*)( RwStream *stream, modelId_t id, size_t size ))0x0045A8F0)( stream, id - 25755, streamBuffer.size );
    }
    else
    {
        // Skip loading scripts (are not utilized in MTA anyway)
        // If we need them in future, no problem to add.
        goto failure;
    }
    
    RwStreamClose( stream, &streamBuffer );

    if ( id < DATA_TEXTURE_BLOCK )
    {
        CBaseModelInfoSAInterface *info = ppModelInfo[id];
        eModelType type = info->GetModelType();

        if ( type != MODEL_VEHICLE && type != MODEL_PED )   // Well, there also is weapon model info?
        {
            if ( CAtomicModelInfoSA *atomInfo = info->GetAtomicModelInfo() )
                atomInfo->ucAlpha = ( loadInfo.m_flags & 0x24 ) ? 0xFF : 0;

            if ( !( loadInfo.m_flags & 0x06 ) )
                loadInfo.PushIntoLoader( *(CModelLoadInfoSA**)0x008E4C60 );
        }
    }
    else if ( id < 25000 || id >= 25575 && id < 25755 || id > 26230 )
    {
        if ( !( loadInfo.m_flags & 0x06 ) )
            loadInfo.PushIntoLoader( *(CModelLoadInfoSA**)0x008E4C60 );
    }

    // If we do not require a second loader run...
    if ( loadInfo.m_eLoading != MODEL_RELOAD )
    {
        // ... we are done loading!
        loadInfo.m_eLoading = MODEL_LOADED;
        (*(unsigned int*)0x008E4CB4) += streamBuffer.size;  // increase the streaming memory statistics

        if ( Streaming::streamingLoadCallback )
            Streaming::streamingLoadCallback( id );
    }

    return true;

    // failure should be a commonly used routine in this function.
failure:
    Streaming::FreeModel( id );
    Streaming::RequestModel( id, loadInfo.m_flags );

    RwStreamClose( stream, &streamBuffer );
    return false;

    // failureDamned is rarely used but important for reference/comparison here.
failureDamned:
    Streaming::FreeModel( id );

    RwStreamClose( stream, &streamBuffer );
    return false;
}

/*=========================================================
    CompleteStreamingRequest

    Arguments:
        id - index of the streamingRequest
    Purpose:
        Finishes all data requesting of the specified streamingRequest.
        After that the streamingRequest may be ready for loading.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004076C0
=========================================================*/
static modelId_t streamingWaitModel = -1;

void __cdecl CompleteStreamingRequest( unsigned int idx )
{
    using namespace Streaming;

    streamingRequest& requester = GetStreamingRequest( idx );

    while ( streamingWaitModel != -1 )
    {
        unsigned int status = requester.status;

        if ( status )
        {
            if ( status == 1 )
            {
                // Have we successfully processed the request?
                if ( ProcessStreamingRequest( idx ) )
                {
                    // Now that we are finished loading the resources, load it
                    if ( requester.status == streamingRequest::STREAMING_LOADING )
                        ProcessStreamingRequest( idx );

                    // We no longer wait for a resource to load
                    streamingWaitModel = -1;
                    break;
                }
                
                continue;
            }
            else if ( status == 2 )
                continue;
            else
            {
                requester.count++;

                unsigned int syncStatus = GetSyncSemaphoreStatus( idx );

                if ( syncStatus == 0xFF || syncStatus == 0xFA )
                    continue;
            }

            // Perform the read request
            ReadStream( idx, ((char**)0x008E4CAC)[idx], requester.offset, requester.blockCount );

            // Change our status
            requester.status = streamingRequest::STREAMING_BUFFERING;
            requester.statusCode = -0xFF;
        }
    }

    // Call some random updater function
    // This function was prolly used by the loading bar in the past.
    ((void (__cdecl*)( void ))0x00590320)();
}

/*=========================================================
    LoadBigModel

    Arguments:
        buf - memory buffer which contains the resource
        id - model info index
    Purpose:
        Loads a big model resource and inserts it into the model
        info described by id. Returns whether the loading process
        was successful.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00408CB0
=========================================================*/
bool __cdecl LoadBigModel( char *buf, modelId_t id )
{
    using namespace Streaming;

    CModelLoadInfoSA *loadInfo = &GetModelLoadInfo( id );

    // Check whether we are ready to load in the first place.
    if ( loadInfo->m_eLoading != MODEL_RELOAD )
    {
        if ( id < DATA_TEXTURE_BLOCK )
            ppModelInfo[id]->Dereference();

        return false;
    }

    // Set up the RenderWare stream
    RwBuffer rwbuf;
    rwbuf.ptr = buf;
    rwbuf.size = loadInfo->m_blockCount * 2048;

    RwStream *stream = RwStreamInitialize( (void*)0x008E48AC, 0, STREAM_TYPE_BUFFER, STREAM_MODE_READ, &rwbuf );

    bool success;

    // Dispatch by request type.
    if ( id < DATA_TEXTURE_BLOCK )
    {
        // Actually, this is unused; so screw it :P
        success = false;
    }
    else if ( id < DATA_TEXTURE_BLOCK + MAX_TXD )
    {
        CTxdInstanceSA *txdInst = (*ppTxdPool)->Get( idOffset( id, DATA_TEXTURE_BLOCK ) );

        // Reference it during the loading
        txdInst->Reference();

        success = LoadTXDContinue( idOffset( id, DATA_TEXTURE_BLOCK ), stream );

        // Remove the reference again. Let the garbage collector unload it if necessary.
        txdInst->DereferenceNoDestroy();
    }
    else
        success = true; // succeeds by default

    RwStreamClose( stream, &rwbuf );

    // Increase the actual streaming memory usage
    *(unsigned int*)0x008E4CB4 += rwbuf.size;

    // Big model has been successfully loaded?
    loadInfo->m_eLoading = MODEL_LOADED;

    // Check whether the loading was actually successful.
    if ( !success )
    {
        // We failed. Try again!
        FreeModel( id );
        RequestModel( id, loadInfo->m_flags );
        return false;
    }

    // Notify our environment that we loaded another model.
    if ( streamingLoadCallback )
        streamingLoadCallback( id );

    return true;
}

/*=========================================================
    UnclogMemoryUsage

    Arguments:
        mem_size - amount of memory required to free
    Purpose:
        Attempts to free the given amount of streaming memory
        so that the allocation of future resources which require
        that amount will not fail due to memory shortage.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040E120
=========================================================*/
void __cdecl UnclogMemoryUsage( size_t mem_size )
{
    // Will investigate this later.
    ((void (__cdecl*)( size_t mem_size ))0x0040E120)( mem_size );
}

/*=========================================================
    ProcessStreamingRequest

    Arguments:
        id - index of the streamingRequest
    Purpose:
        Handles a streaming request table and attempts to load
        associated models while freeing others to fit the
        maximum streaming memory. This function is the core
        utility of the GTA:SA model streaming management.
        Returns whether processing was successful. It fails if
        the loading of the syncSemaphore is still pending.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040E170
    Note:
        The original GTA:SA function is a mess and an example of
        how GTA:SA is not a great engine. The GTA community figured
        about this anyway, so that should be proof.
=========================================================*/
bool __cdecl ProcessStreamingRequest( unsigned int id )
{
    using namespace Streaming;

    streamingRequest& requester = Streaming::GetStreamingRequest( id );

    // Are we waiting for resources?
    if ( unsigned int status = GetSyncSemaphoreStatus( id ) )
    {
        if ( status == 0xFF || status == 0xFA )
            return false;

        // Make our initial status
        requester.returnCode = status;
        requester.status = streamingRequest::STREAMING_WAITING;

        // We cannot wait for a model if we already wait for one!
        // Let this request fail then.
        if ( streamingWaitModel != -1 )
            return false;

        CompleteStreamingRequest( id );
        return true;
    }

    bool isLoading = requester.status == streamingRequest::STREAMING_LOADING;
    requester.status = streamingRequest::STREAMING_NONE;

    if ( isLoading )
    {
        // Continue the loading procedure.
        LoadBigModel( ((char**)0x008E4CAC)[id] + requester.bufOffsets[0] * 2048, requester.ids[0] );

        // Mark that we are done.
        requester.ids[0] = -1;
    }
    else
    {
        // Attempt to load the queue
        for ( unsigned int n = 0; n < MAX_STREAMING_REQUESTS; n++ )
        {
            modelId_t mid = requester.ids[n];

            if ( mid != -1 )
            {
                CModelLoadInfoSA& loadInfo = GetModelLoadInfo( mid );
                
                // I removed the broken vehicle stream limit from this function.
                // That stream.ini "vehicles" feature was never properly implemented into GTA:SA 1.0.
                // 1) It was used for big vehicle models only, which were never loaded using the appropriate
                // API in the first place.
                // 2) Keeping that Rockstar code hinders MTA logic, as it expects no limits.
                //  * location: 0x0040E1F1
                if ( mid < 25255 || mid >= 25511 )
                {
                    // Free some memory usage for the request
                    UnclogMemoryUsage( loadInfo.GetSize() );
                }

                // Perform the loading
                LoadModel( ((char**)0x008E4CAC)[id] + requester.bufOffsets[n] * 2048, mid, id );

                // Do we have to continue loading this model?
                if ( loadInfo.m_eLoading == MODEL_RELOAD )
                {
                    // The_GTA: this appears to simulate coroutine behaviour.
                    // To improve the performance of the whole system, I propose a CExecutiveManagerSA
                    // class which will host fibers using frame pulses. It will be adapted from
                    // Lua coroutines, but will have multiple ways of yielding (time, count, etc).
                    // It will ultimatively remove lag-spikes due to resource loading.
                    requester.status = streamingRequest::STREAMING_LOADING;
                    requester.bufOffsets[0] = requester.bufOffsets[n];
                    requester.ids[0] = mid;

                    // Only clear if not zero (because 0 is slot of big model request)
                    if ( n != 0 )
                        requester.ids[n] = 0xFFFF;
                }
                else
                    requester.ids[n] = 0xFFFF;  // clear the request slot
            }
        }
    }

    // Is a big model loading? But is the requester in an inappropriate status?
    if ( *(bool*)0x008E4A58 && requester.status != streamingRequest::STREAMING_LOADING )
    {
        // Reset big model loading
        *(bool*)0x008E4A58 = false;

        // Clear the secondary queue
        memset( GetStreamingRequest( 1 ).ids, 0xFF, sizeof(int) * MAX_STREAMING_REQUESTS );
    }

    // Processing succeeded!
    return true;
}

/*=========================================================
    PulseStreamingRequests

    Purpose:
        Updates the general status of the primary and secondary
        streaming requests. Called before and after
        LoadAllRequestedModels.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040E460
=========================================================*/
void __cdecl PulseStreamingRequests( void )
{
    using namespace Streaming;

    streamingRequest& primary = GetStreamingRequest( 0 );
    streamingRequest& secondary = GetStreamingRequest( 1 );

    // Finish big model loading?
    if ( secondary.status == streamingRequest::STREAMING_LOADING )
        ProcessStreamingRequest( 1 );

    if ( primary.status == streamingRequest::STREAMING_BUFFERING )
    {
        CancelSyncSemaphore( 0 );
        ProcessStreamingRequest( 0 );
    }

    // Possibly finish big model loading
    if ( primary.status == streamingRequest::STREAMING_LOADING )
        ProcessStreamingRequest( 0 );

    if ( secondary.status == streamingRequest::STREAMING_BUFFERING )
    {
        CancelSyncSemaphore( 1 );
        ProcessStreamingRequest( 1 );
    }

    // I do not understand why it is checked twice.
    // Does the secondary streaming request have higher priority?
    if ( secondary.status == streamingRequest::STREAMING_LOADING )
        ProcessStreamingRequest( 1 );
}

/*=========================================================
    ProcessLoadQueue

    Arguments:
        offset - IMG archive offset to match against the result
        favorPriority - true if priority models should be handled first
    Purpose:
        Returns the model id of the model id which matches the
        given offset best.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00408E20
=========================================================*/
inline bool IsModelLoaded( const unsigned short offset, modelId_t id )
{
    eLoadingState status = Streaming::GetModelLoadInfo( offset, id ).m_eLoading;

    return status == MODEL_LOADED || status == MODEL_QUEUE;
}

inline bool EnsureResourceAvailability( const unsigned short offset, modelId_t id, unsigned char flags )
{
    if ( !IsModelLoaded( offset, id ) )
    {
        // Load any yet not finished dependencies
        Streaming::RequestModel( offset + id, flags );
        return true;    // we have to wait
    }

    return false; // ready for processing
}

inline bool AreAnimationDependenciesLoaded( modelId_t id )
{
    // Are we loading animations at all?
    return !*(bool*)0x00B5F852;// && Streaming::GetModelLoadInfo( 7 ).m_eLoading == MODEL_LOADED;

    // The_GTA: I removed the check which prevented animation loading if skin 7 was
    // not previously loaded. This should allow ped loading without the previous limitation.
}

struct ModelLoadQueueDispatch : ModelCheckDispatch <true, false>
{
    CModelLoadInfoSA*& m_item;

    ModelLoadQueueDispatch( CModelLoadInfoSA*& item ) : m_item( item )
    {
    }

    bool __forceinline DoBaseModel( modelId_t id )
    {
        // Make sure all dependencies of this model are loaded
        CBaseModelInfoSAInterface *model = ppModelInfo[id];

        // Check out texture loading status
        if ( model->usTextureDictionary == -1 || !EnsureResourceAvailability( DATA_TEXTURE_BLOCK, model->usTextureDictionary, m_item->m_flags ) )
        {
            int animId = model->GetAnimFileIndex();

            return animId == -1 || !EnsureResourceAvailability( DATA_ANIM_BLOCK, animId, 0x08 );
        }

        // We need to wait, do not call success handler
        return false;
    }

    bool __forceinline DoTexDictionary( modelId_t id )
    {
        CTxdInstanceSA *txdInst = (*ppTxdPool)->Get( id );
        unsigned short parentId = txdInst->m_parentTxd;

        return parentId == 0xFFFF || !EnsureResourceAvailability( DATA_TEXTURE_BLOCK, parentId, 0x08 );
    }

    bool __forceinline DoAnimation( modelId_t id )
    {
        // Are we loading animations at all?
        return AreAnimationDependenciesLoaded( id );
    }
};

modelId_t __cdecl ProcessLoadQueue( unsigned int offset, bool favorPriority )
{
    using namespace Streaming;

    CModelLoadInfoSA *item = GetQueuedLoadInfo();

    unsigned int lastOffset = 0xFFFFFFFF;
    modelId_t lastId = -1;

    if ( item != *(CModelLoadInfoSA**)0x008E4C54 )
    {
        unsigned int upperOffset = 0xFFFFFFFF;
        modelId_t rangeId = -1;

        modelId_t itemId = item->GetIndex();

        do
        {
            if ( !favorPriority || *(unsigned int*)0x008E4BA0 == 0 || item->m_flags & 0x10 )
            {
                // Are we allowed to return this value?
                if ( DefaultDispatchExecute( itemId, ModelLoadQueueDispatch( item ) ) )
                {
                    // Try to find a return value.
                    unsigned int itemOffset = item->GetStreamOffset();

                    if ( itemOffset < lastOffset )
                    {
                        lastId = itemId;
                        lastOffset = itemOffset;
                    }

                    if ( itemOffset < upperOffset && itemOffset >= offset )
                    {
                        upperOffset = itemOffset;
                        rangeId = itemId;
                    }
                }
            }

            item = &GetModelLoadInfo( itemId = item->m_primaryModel );
        }
        while ( item != *(CModelLoadInfoSA**)0x008E4C54 );

        if ( rangeId != -1 )
            return rangeId;
    }

    // Did we not result in anything and are there priority requests?
    if ( lastId == -1 && *(unsigned int*)0x008E4BA0 != 0 )
    {
        // Reset the number of priority requests
        *(unsigned int*)0x008E4BA0 = 0;

        // Try again with all requests included
        return ProcessLoadQueue( offset, false );
    }

    return lastId;
}

/*=========================================================
    PulseStreamingRequest

    Arguments:
        id - index of the streaming request
    Purpose:
        Main handler function of streaming request logic. Goes through
        the requested queue and puts them into the streaming request
        id and size array. If the resource fails the dependency tests,
        it is not loaded. If the streaming request waits for syncSemaphore
        activity, the syncSemaphore is notified.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040CBA0
=========================================================*/
inline bool CheckModelDependency( modelId_t id )
{
    if ( idOffset( id, DATA_TEXTURE_BLOCK ) < MAX_TXD )
        return CheckTXDDependency( idOffset( id, DATA_TEXTURE_BLOCK ) );
    else if ( idOffset( id, DATA_ANIM_BLOCK ) < 180 )
        return CheckAnimDependency( idOffset( id, DATA_ANIM_BLOCK ) );

    return true;
}

inline bool HaveModelDependenciesLoaded( CBaseModelInfoSAInterface *model )
{
    unsigned short txdId = (unsigned short)model->usTextureDictionary;

    if ( txdId != 0xFFFF && !IsModelLoaded( DATA_TEXTURE_BLOCK, txdId ) )
        return false;

    int animId = model->GetAnimFileIndex();

    return animId == -1 || IsModelLoaded( DATA_ANIM_BLOCK, animId );
}

struct ModelStreamingPulseDispatch : ModelCheckDispatch <false>
{
    CBaseModelInfoSAInterface*& m_model;
    unsigned int& m_blockCount;
    bool m_isVehicleLoading;
    bool m_isPedLoading;

    ModelStreamingPulseDispatch( CBaseModelInfoSAInterface*& model, unsigned int& blockCount ) :
        m_model( model ),
        m_blockCount( blockCount )
    {
        m_isVehicleLoading = false;
        m_isPedLoading = false;
    }

    bool __forceinline DoBaseModel( modelId_t id )
    {
        m_model = ppModelInfo[id];

        // We may only load one ped model at once
        return ( !m_isPedLoading || m_model->GetModelType() != MODEL_PED ) &&
        // We may only load one vehicle model at once
               ( !m_isVehicleLoading || m_model->GetModelType() != MODEL_VEHICLE ) &&
        // By now we require the dependencies loaded!
               HaveModelDependenciesLoaded( m_model );
    }

    bool __forceinline DoAnimation( modelId_t id )
    {
        // Make sure the animation dependencies are present.
        return AreAnimationDependenciesLoaded( id );
    }

    bool __forceinline CheckValidity( void )
    {
        return !m_isVehicleLoading || m_blockCount <= 200;
    }

    bool __forceinline DoCollision( modelId_t id )
    {
        return CheckValidity();
    }

    bool __forceinline DoIPL( modelId_t id )
    {
        return CheckValidity();
    }

    bool __forceinline DoRecording( modelId_t id )
    {
        return CheckValidity();
    }

    bool __forceinline DoTexDictionary( modelId_t id )
    {
        return CheckValidity();
    }

    bool __forceinline DoPathFind( modelId_t id )
    {
        return CheckValidity();
    }

    void __forceinline AfterPerform( modelId_t id )
    {
        if ( id < MAX_MODELS )
        {
            if ( m_model->GetModelType() == MODEL_PED )
                m_isPedLoading = true;
            else if ( m_model->GetModelType() == MODEL_VEHICLE )
                m_isVehicleLoading = true;
        }
        else if ( m_blockCount > 200 )
        {
            // This confuses me. Is it really describing a vehicle loading?
            // Maybe R* thought that only vehicle models could have such high poly counts.
            m_isVehicleLoading = true;
        }
    }
};

void __cdecl PulseStreamingRequest( unsigned int reqId )
{
    using namespace Streaming;

    // The thread allocation buffer supports a chain of resource buffers.
    // The streaming requests keep track of buffer offsets (that is where
    // resource data is written to in the thread allocation buffer).
    // This also means that if this memory block count is overshot, we cause a
    // buffer overflow. R* indeed employed a dangerous loading mechanism!
    unsigned int threadBufferOffset = 0;

    // The_GTA: My speculation about the GetNextReadOffset is that it is
    // a performance improvement based on investigation. After all, reading
    // the exactly next offset in a file should be faster than jumping all
    // over the place!
    // This optimization could be disabled on SSD.
    unsigned int offset = GetStreamNextReadOffset();
    unsigned int blockCount = 0;

    modelId_t modelId = ProcessLoadQueue( offset, true );

    if ( modelId == -1 )
        return;

    CBaseModelInfoSAInterface *model;
    ModelStreamingPulseDispatch dispatch( model, blockCount );

    {
        CModelLoadInfoSA *loadInfo = &GetModelLoadInfo( modelId );

        // Check for a model which matches dependency
        for ( ; !( loadInfo->m_flags & FLAG_NODEPENDENCY ) && !CheckModelDependency( modelId ); loadInfo = &GetModelLoadInfo( modelId ) )
        {
            // Since this model failed the dependency check, burn it!
            FreeModel( modelId );

            // Request the offset of this info (and blockCount)
            loadInfo->GetOffset( offset, blockCount );

            // Try to grab another modelId
            modelId = ProcessLoadQueue( offset, true );

            if ( modelId == -1 )
                return;
        }

        if ( modelId == -1 )
            return;

        // Get our offset information
        loadInfo->GetOffset( offset, blockCount );

        // Check whether it really is a big block
        if ( blockCount > *(unsigned int*)0x008E4CA8 )
        {
            // We cannot request big models on the second requester
            // If the second requester is doing anything, screw it
            if ( reqId == 1 || GetStreamingRequest( 1 ).status != streamingRequest::STREAMING_NONE )
                return;

            // We are loading a big model, eh
            *(bool*)0x008E4A58 = true;
        }
    }

    streamingRequest& requester = GetStreamingRequest( reqId );

    unsigned char n = 0;

    for ( ; n < MAX_STREAMING_REQUESTS; n++ )
    {
        if ( modelId == -1 )
            goto abortedLoading;

        CModelLoadInfoSA *loadInfo = &GetModelLoadInfo( modelId );

        if ( loadInfo->m_eLoading != MODEL_LOADING )
            goto abortedLoading;

        loadInfo->GetBlockCount( blockCount );

        // If there are priority requests waiting to be loaded and this is
        // not a priority request, we cannot afford continuing
        if ( *(unsigned int*)0x008E4BA0 && !( loadInfo->m_flags & FLAG_PRIORITY ) )
            goto abortedLoading;

        // Only valid for modelId < DATA_TEXTURE_BLOCK
        if ( !DefaultDispatchExecute( modelId, dispatch ) )
            goto abortedLoading;

        // Write our request into the streaming requester
        requester.bufOffsets[n] = threadBufferOffset;
        requester.ids[n] = modelId;

        // Set the new offset
        threadBufferOffset += blockCount;

        // If the request overshoots the thread allocation buffer at its offset...
        if ( threadBufferOffset + blockCount > *(unsigned int*)0x008E4CA8 )
        {
            if ( n != 0 )
            {
                threadBufferOffset -= blockCount;
                goto abortedLoading;
            }
        }

        // The_GTA: Here was some sort of debug table which stored all model ids that reached this
        // code location. I have left it out since this information was not further used during runtime.
        // Must have been a left-over from quick debugging. (1.0 US and 1.0 EU: 0x0040CE59)

        dispatch.AfterPerform( modelId );

        // Put it into the direct loading queue
        loadInfo->m_eLoading = MODEL_QUEUE;

        // Remove it from the long queue
        loadInfo->PopFromLoader();

        // Decrease the number of models that are in the long queue
        (*(unsigned int*)0x008E4CB8)--;

        // Unset priority status since we loaded
        if ( loadInfo->m_flags & FLAG_PRIORITY )
        {
            // Decrease number of priority requests
            (*(unsigned int*)0x008E4BA0)--;

            loadInfo->m_flags &= ~FLAG_PRIORITY;
        }

        // Try to load the next model id in order.
        modelId = loadInfo->m_lastID;
    }

pulseSemaphore:
    // Notify the synchronous semaphore.
    // Is this a normal request or just a big model one?
    ReadStream( reqId, ((char**)0x008E4CAC)[reqId], offset, threadBufferOffset );

    // Update the requester
    requester.status = streamingRequest::STREAMING_BUFFERING;
    requester.statusCode = 0;
    requester.blockCount = threadBufferOffset;
    requester.offset = offset;
    requester.count = 0;

    // I do not know what this is.
    *(bool*)0x009654C4 = false;
    return;

abortedLoading:
    // GTA:SA code checked for n < MAX_STREAMING_REQUESTS; that is established here,
    // so I removed the check
    for ( ; n < MAX_STREAMING_REQUESTS; n++ )
        requester.ids[n] = -1;
    
    goto pulseSemaphore;
}

/*=========================================================
    CStreamingSA::SetLoadCallback

    Arguments:
        callback - function to be called once a model successfully loaded
    Purpose:
        Specify an internal callback which should be issued once a
        model was successfully loaded.
=========================================================*/
void CStreamingSA::SetLoadCallback( streamingLoadCallback_t callback )
{
    Streaming::streamingLoadCallback = callback;
}