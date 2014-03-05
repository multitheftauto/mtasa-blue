/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.clump.cpp
*  PURPOSE:     Atomic and clump file streaming.
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "gamesa_renderware.h"

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
    _initAtomicNormals (MTA extension)

    Arguments:
        atom - atomic which was newly created and lacks geometry normals
    Purpose:
        Calculates geometry normals for the referenced geometry
        of said atomic if not already present. Normals are required
        for dynamic lighting.
=========================================================*/
void _initAtomicNormals( RpAtomic *atom )
{
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

    _initAtomicNormals( atom );
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

    // TODO: reven this function.
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
bool __cdecl LoadClumpFile( RwStream *stream, modelId_t model )
{
    CAtomicModelInfoSA *atomInfo = ppModelInfo[model]->GetAtomicModelInfo();
    bool appliedRemapCheck, result;

    // MTA extension: Apply our global imports
    RwImportedScan::Apply( atomInfo->usTextureDictionary );

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

    // MTA extension: remove the global imports handler from the scan stack.
    RwImportedScan::Unapply();

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
bool __cdecl LoadClumpFilePersistent( RwStream *stream, modelId_t id )
{
    CClumpModelInfoSAInterface *info = (CClumpModelInfoSAInterface*)ppModelInfo[id];

    // Not sure about this flag anymore. Apparently it stands for multi-clump here.
    if ( info->renderFlags & RENDER_NOSKELETON )
    {
        RpClump *clump = RpClumpCreate();
        RwFrame *frame = clump->parent = RwFrameCreate();

        RwImportedScan::Apply( info->usTextureDictionary );

        while ( RwStreamFindChunk( stream, 0x10, NULL, NULL ) )
        {
            RpClump *item = RpClumpStreamRead( stream );

            if ( !item )
            {
                // Small memory leak fix
                RwFrameDestroy( frame );
                clump->parent = NULL;

                RwImportedScan::Unapply();

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

        RwImportedScan::Unapply();

        info->SetClump( clump );
        return true;
    }

    bool isVehicle = info->GetModelType() == MODEL_VEHICLE;
    
    if ( !RwStreamFindChunk( stream, 0x10, NULL, NULL ) )
        return false;

    // MTA extension: include our imported textures
    RwImportedScan::Apply( info->usTextureDictionary );

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

    RwImportedScan::Unapply();

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
bool __cdecl LoadClumpFileBigContinue( RwStream *stream, modelId_t id )
{
    CClumpModelInfoSAInterface *info = (CClumpModelInfoSAInterface*)ppModelInfo[id];

    assert( info->GetRwModelType() == RW_CLUMP );

    bool isVehicle = info->GetModelType() == MODEL_VEHICLE;

    if ( isVehicle )
        RwRemapScan::Apply();

    // Do load our imported textures
    RwImportedScan::Apply( id );

    RpClump *clump = ReadClumpBigContinue( stream );

    // Pop the texture scanners
    RwImportedScan::Unapply();

    if ( isVehicle )
        RwRemapScan::Unapply();

    if ( clump )
    {
        info->SetClump( clump );
        return true;
    }

    return false;
}