/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpClump.cpp
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include <StdInc.h>
#include "../gamesa_renderware.h"

/*=========================================================
    RpClump::Render

    Purpose:
        Renders all atomics of this clump that are flagged
        visible.
    Binary offsets:
        (1.0 US): 0x00749B20
        (1.0 EU): 0x00749B70
=========================================================*/
bool RpClump::Render( void )
{
    bool successful = true;

    LIST_FOREACH_BEGIN( RpAtomic, atomics.root, atomics )
        if ( item->IsVisible() )
        {
            RpAtomic *atom = item->renderCallback( item );

            if ( !atom )
                successful = false;
        }
    LIST_FOREACH_END

    return successful;
}

RpClump* __cdecl RpClumpRender( RpClump *clump )        { return ( clump->Render() ) ? ( clump ) : ( NULL ); }
/*=========================================================
    RpClump::InitStaticSkeleton (GTA:SA extension)

    Purpose:
        Applies static skeletal information for this clump if it
        lacks animation information. It then caches all bone
        offsets.
    Note:
        This function is used in CClumpModelInfoSAInterface.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004D6720
=========================================================*/
static int RwAssignRenderLink( RwFrame *child, RwRenderLink **link )
{
    (*link)->context = child;
    (*link)++;

    child->ForAllChildren( RwAssignRenderLink, link );
    return true;
}

static void RwRenderLinkInit( RwRenderLink *link, void *data )
{
    RwFrame *frame = (RwFrame*)link->context;

    link->flags = 0;

    link->position = frame->GetPosition();

    link->id = -1;
}

static void RwAnimatedRenderLinkInit( RwRenderLink *link, int )
{
    link->flags = 0;
}

void RpClump::InitStaticSkeleton( void )
{
    RpAtomic *atomic = GetFirstAtomic();
    RwStaticGeometry *geom = CreateStaticGeometry();
    RwRenderLink *link;
    CVector boneOffsets[MAX_BONES];
    unsigned int boneCount;

    if ( !atomic || !atomic->geometry->skeleton )
    {
        // Initialize a non animated mesh
        link = geom->AllocateLink( parent->CountChildren() );

        // Assign all frames
        parent->ForAllChildren( RwAssignRenderLink, &link );

        // Init them
        geom->ForAllLinks( RwRenderLinkInit, (void*)NULL );

        geom->link->flags |= BONE_ROOT;
        return;
    }

    // Grab the number of bones
    boneCount = atomic->geometry->skeleton->boneCount;

    link = geom->AllocateLink( boneCount );

    if ( boneCount != 0 )
    {
        CVector *offset = boneOffsets;
        RpAnimHierarchy *anim = atomic->anim;  
        RwBoneInfo *bone = anim->boneInfo;
        RwAnimInfo *info = anim->anim->info;

        // Get the real bone positions
        GetBoneTransform( boneOffsets );

        // I guess its always one bone ahead...?
        link++;

        for ( unsigned int n = 0; n < boneCount; n++ )
        {
            link->context = info;
            link->id = bone->index;

            // Update the bone offset in the animation
            info->offset = *offset;
            
            info++;
            offset++;
            link++;
            bone++;
        }
    }

    geom->ForAllLinks( RwAnimatedRenderLinkInit, 0 );

    // Flag the first render link, root bone?
    geom->link->flags |= BONE_ROOT;
}

/*=========================================================
    RpClump::CreateStaticGeometry

    Purpose:
        Assigns a new static geometry instance to this clump
        and returns it.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004D5F50
=========================================================*/
RwStaticGeometry* RpClump::CreateStaticGeometry( void )
{
    return pStatic = new RwStaticGeometry();
}

/*=========================================================
    RpClump::GetNumAtomics

    Purpose:
        Returns the number of atomics that are registered
        in this clump.
    Binary offsets:
        (1.0 US): 0x007498E0
        (1.0 EU): 0x00749930
=========================================================*/
int RpClump::GetNumAtomics( void )
{
    int count = 0;

    LIST_FOREACH_BEGIN( RpAtomic, atomics.root, atomics )
        count++;
    LIST_FOREACH_END

    return count;
}

int __cdecl RpClumpGetNumAtomics( RpClump *clump )      { return clump->GetNumAtomics(); }
/*=========================================================
    RpClumpForAllAtomics

    Arguments:
        clump - the clump to loop through all atomics of
        callback - function to invoke for every found atomic
        data - userdata pointer to pass to callback
    Purpose:
        Loops through all registered atomics of this clump
        and invokes the given callback for each one.
    Binary offsets:
        (1.0 US): 0x00749B70
        (1.0 EU): 0x00749BC0
=========================================================*/
RpClump* __cdecl RpClumpForAllAtomics( RpClump *clump, clumpAtomicIterator_t callback, void *data )
{
    clump->ForAllAtomics( callback, data );
    return clump;
}

RpClump* __cdecl RpClumpForAllAtomics( RpClump *clump, void *callback, void *data )
{
    return RpClumpForAllAtomics( clump, (clumpAtomicIterator_t)callback, data );
}

/*=========================================================
    RpClump::GetAtomicAnimHierarchy

    Purpose:
        Returns the anim hierarchy of the first atomic in this
        clump.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734A40
=========================================================*/
RpAnimHierarchy* RpClump::GetAtomicAnimHierarchy( void )
{
    RpAtomic *atomic = GetFirstAtomic();

    if ( !atomic )
        return NULL;
    
    return atomic->anim;
}

/*=========================================================
    RpClump::GetAnimHierarchy

    Purpose:
        Returns the anim hierarchy of the container frame.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734B10
=========================================================*/
RpAnimHierarchy* RpClump::GetAnimHierarchy( void )
{
    return parent->GetAnimHierarchy();
}

/*=========================================================
    RpClump::ScanFrameHierarchy

    Arguments:
        atomic - array which shall hold all found frames
        max - number of frames te array may hold
    Purpose:
        Puts all construction hierarchy frames into an array.
        The hierarchyId was assigned when CClumpModelInfoSAInterface
        constructed a model using an atomic information structure.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C5440
=========================================================*/
struct _rwFrameScanHierarchy
{
    RwFrame **output;
    size_t max;
};

static int RwFrameGetAssignedHierarchy( RwFrame *child, _rwFrameScanHierarchy *info )
{
    if ( child->hierarchyId && child->hierarchyId < info->max )
        info->output[ child->hierarchyId ] = child;

    return child->ForAllChildren( RwFrameGetAssignedHierarchy, info );
}

void RpClump::ScanAtomicHierarchy( RwFrame **atomics, size_t max )
{
    _rwFrameScanHierarchy info;

    info.output = atomics;
    info.max = max;

    parent->ForAllChildren( RwFrameGetAssignedHierarchy, &info );
}

/*=========================================================
    RpClump::GetFirstAtomic

    Purpose:
        Returns the first registered atomic of this clump.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734820
=========================================================*/
RpAtomic* RpClump::GetFirstAtomic( void )
{
    if ( LIST_EMPTY( atomics.root ) )
        return NULL;

    return LIST_GETITEM( RpAtomic, atomics.root.next, atomics );
}

/*=========================================================
    RpClump::GetLastAtomic

    Purpose:
        Returns the last atomic registered at this clump.
=========================================================*/
RpAtomic* RpClump::GetLastAtomic( void )
{
    if ( LIST_EMPTY( atomics.root ) )
        return NULL;

    return LIST_GETITEM( RpAtomic, atomics.root.prev, atomics );
}

/*=========================================================
    RpClump::FindNamedAtomic

    Arguments:
        name - case-insensitive name to check the frame names against
    Purpose:
        Scans through the clump and returns the first atomic
        whose parent frame matches the name. If not found it returns
        NULL.
=========================================================*/
RpAtomic* RpClump::FindNamedAtomic( const char *name )
{
    LIST_FOREACH_BEGIN( RpAtomic, atomics.root, atomics )
        if ( strcmp( item->parent->szName, name ) == 0 )
            return item;
    LIST_FOREACH_END

    return NULL;
}

/*=========================================================
    RpClump::Find2dfx

    Purpose:
        Returns the first atomic whose geometry has valid 2dfx
        data. Valid == at least one effect type allocated to the
        2dfx container. If not found, it returns NULL.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734880
=========================================================*/
static int RwAtomicGet2dfx( RpAtomic *child, RpAtomic **atomic )
{
    // Crashfix, invalid geometry
    if ( !child->geometry )
        return true;

    if ( !child->geometry->_2dfx || child->geometry->_2dfx->count == 0 )
        return true;

    *atomic = child;
    return false;
}

RpAtomic* RpClump::Find2dfx( void )
{
    RpAtomic *atomic;

    return ( ForAllAtomics( RwAtomicGet2dfx, &atomic ) ) ? ( NULL ) : ( atomic );
}

/*=========================================================
    RpClump::SetupAtomicRender (GTA:SA extension)

    Purpose:
        Sets up the rendering logic for all atomics in this
        clump. This decides whether they are rendered with object
        or with vehicle techniques.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C4F30
=========================================================*/
static int RwAtomicSetupPipeline( RpAtomic *child, int )
{
    if ( child->IsNight() )
        RpAtomicSetupObjectPipeline( child );
    else if ( child->pipeline == RW_ATOMIC_RENDER_VEHICLE )
        RpAtomicSetupVehiclePipeline( child );

    return true;
}

void RpClump::SetupAtomicRender( void )
{
    ForAllAtomics( RwAtomicSetupPipeline, 0 );
}

/*=========================================================
    RpClump::RemoveAtomicComponentFlags

    Arguments:
        flags - the flags you wish to remove from all atomics
    Purpose:
        Loops through all atomics of this clump and removes the
        specified component flags.
=========================================================*/
static int RwAtomicRemoveComponentFlags( RpAtomic *child, unsigned short flags )
{
    child->componentFlags &= ~flags;
    return true;
}

void RpClump::RemoveAtomicComponentFlags( unsigned short flags )
{
    ForAllAtomics( RwAtomicRemoveComponentFlags, flags );
}

/*=========================================================
    RpClump::FetchMateria

    Arguments:
        mats - container to store all materials at
    Purpose:
        Lists the materials of all atomics registered in this clump.
        Be sure to allocate a big RpMaterials container for this.
=========================================================*/
static int RwAtomicFetchMateria( RpAtomic *child, RpMaterials *mats )
{
    child->FetchMateria( *mats );
    return true;
}

void RpClump::FetchMateria( RpMaterials& mats )
{
    ForAllAtomics( RwAtomicFetchMateria, &mats );
}

/*=========================================================
    RpClump::GetBoneTransform

    Arguments:
        offset - array of vectors allocated times the number of bones
    Purpose:
        Calculates the bone offsets for all bones in this model. This
        function is used in CPedModelInfoSAInterface::SetClump.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00735360
    Note:
        This function is untested (yet).
=========================================================*/
void RpClump::GetBoneTransform( CVector *offset )
{
    RpAtomic *atomic;
    RpSkeleton *skel;
    RpAnimHierarchy* anim;
    RwBoneInfo *bone;
    RwMatrix *skelMat;
    unsigned int n;
    unsigned int matId = 0;
    unsigned int boneIndexes[20];
    unsigned int *idxPtr = boneIndexes;

    if ( !offset )
        return;

    atomic = GetFirstAtomic();
    skel = atomic->geometry->skeleton;

    anim = atomic->anim;

    // Reset the matrix
    offset->fX = 0;
    offset->fY = 0;
    offset->fZ = 0;

    offset++;

    // Do nothing if the bone count is smaller than 2
    if ( skel->boneCount < 2 )
        return;

    skelMat = skel->boneMatrices + 1;
    bone = anim->boneInfo + 1;

    // We apparrently have the first one initialized already?
    for ( n = 1; n < skel->boneCount; n++ )
    {
        RwMatrix mat;

        RwMatrixInvert( &mat, skelMat );

        RenderWare::GetInterface()->m_matrixTransform3( offset, &mat.vUp, 1, skel->boneMatrices + matId );

        // Some sort of stacking mechanism, maximum 20
        if ( bone->flags & 0x02 )
            *(++idxPtr) = matId;

        if ( bone->flags & 0x01 )
            matId = *(idxPtr--);
        else
            matId = n;

        skelMat++;
        offset++;
        bone++;
    }
}

/*=========================================================
    RpClumpGetAlpha

    Arguments:
        clump - the clump to get the alpha value from
    Purpose:
        Returns the (plugin) alpha value that is assigned to
        the given clump.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732B20
=========================================================*/
int __cdecl RpClumpGetAlpha( const RpClump *clump )
{
    return clump->alpha;
}

/*=========================================================
    RpClumpSetAlpha

    Arguments:
        clump - the clump to set the alpha to
        alpha - alpha color to set to the clump
    Purpose:
        Updates the (plugin) alpha value of a specified
        clump object.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732B00
=========================================================*/
void __cdecl RpClumpSetAlpha( RpClump *clump, int alpha )
{
    clump->alpha = alpha;
}

/*=========================================================
    RpClumpCreate

    Purpose:
        Creates a new RpClump plugin instance and registers it
        into the system.
    Binary offsets:
        (1.0 US): 0x0074A290
        (1.0 EU): 0x0074A2E0
=========================================================*/
static RpClump* __cdecl _clumpCallback( RpClump *clump, void *data )
{
    return clump;
}

RpClump* __cdecl RpClumpCreate( void )
{
    RwInterface *rwInterface = RenderWare::GetInterface();

    RpClump *clump = (RpClump*)rwInterface->m_allocStruct( rwInterface->m_clumpInfo, 0x30010 );

    if ( !clump )
        return NULL;

    clump->type = RW_CLUMP;

    LIST_CLEAR( clump->atomics.root );
    LIST_CLEAR( clump->lights.root );
    LIST_CLEAR( clump->cameras.root );

    clump->subtype = 0;
    clump->flags = 0;
    clump->privateFlags = 0;
    clump->parent = NULL;

    LIST_INITNODE( clump->globalClumps );

    clump->callback = _clumpCallback;

    RwObjectRegister( (void*)0x008D6264, clump );
    return clump;
}