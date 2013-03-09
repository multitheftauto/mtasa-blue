/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rwapi.cpp
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"


/*****************************************************************************
*
*   RenderWare Functions
*
*****************************************************************************/

RwAtomicRenderChainInterface *const rwRenderChains = (RwAtomicRenderChainInterface*)0x00C88070;
RwScene *const *p_gtaScene = (RwScene**)0x00C17038;


void RwObjectFrame::AddToFrame( RwFrame *frame )
{
    RemoveFromFrame();

    parent = frame;

    if ( !frame )
        return;

    LIST_INSERT( frame->objects.root, lFrame );
}

void RwObjectFrame::RemoveFromFrame()
{
    if ( !parent )
        return;

    LIST_REMOVE( lFrame );

    parent = NULL;
}

void RwFrame::SetModelling( const RwMatrix& mat )
{
    modelling = mat;

    // Set the frame to dirty
    privateFlags |= RW_FRAME_DIRTY;
}

void RwFrame::SetPosition( const CVector& pos )
{
    modelling.pos = pos;

    // Set the frame to dirty
    privateFlags |= RW_FRAME_DIRTY;
}

const RwMatrix& RwFrame::GetLTM() const
{
    // This function will recalculate the LTM if frame is dirty
    return *RwFrameGetLTM( this );
}

void RwFrame::Link( RwFrame *frame )
{
    // Unlink previous relationship of new child
    frame->Unlink();    // interesting side effect: cached if usage of parent

    // Insert the new child at the beginning
    frame->next = child;
    child = frame;

    frame->parent = this;

    frame->SetRootForHierarchy( root );
    frame->UnregisterRoot();

    // Mark the main root as independent
    root->RegisterRoot();
}

void RwFrame::Unlink()
{
    if ( !parent )
        return;

    if ( parent->child == this )
        parent->child = next;
    else
    {
        RwFrame *prev = parent->child;

        while ( prev->next != this )
            prev = prev->next;

        prev->next = next;
    }

    parent = NULL;
    next = NULL;

    SetRootForHierarchy( this );

    // Mark as independent
    RegisterRoot();
}

void RwFrame::SetRootForHierarchy( RwFrame *_root )
{
    root = _root;

    RwFrame *_child = child;

    while ( _child )
    {
        _child->SetRootForHierarchy( root );

        _child = _child->next;
    }
}

static bool RwFrameGetChildCount( RwFrame *child, unsigned int *count )
{
    child->ForAllChildren( RwFrameGetChildCount, count );

    (*count)++;
    return true;
}

unsigned int RwFrame::CountChildren()
{
    unsigned int count = 0;

    ForAllChildren( RwFrameGetChildCount, &count );
    return count;
}

RwFrame* RwFrame::GetFirstChild()
{
    return child;
}

struct _rwFrameFindName
{
    const char *name;
    RwFrame *result;
};

static bool RwFrameGetFreeByName( RwFrame *child, _rwFrameFindName *info )
{
    if ( child->hierarchyId || strcmp(child->szName, info->name) != 0 )
        return child->ForAllChildren( RwFrameGetFreeByName, info );

    info->result = child;
    return false;
}

RwFrame* RwFrame::FindFreeChildByName( const char *name )
{
    _rwFrameFindName info;

    info.name = name;

    if ( ForAllChildren( RwFrameGetFreeByName, &info ) )
        return NULL;

    return info.result;
}

static bool RwFrameGetByName( RwFrame *child, _rwFrameFindName *info )
{
    if ( stricmp( child->szName, info->name ) != 0 )
        return child->ForAllChildren( RwFrameGetByName, info );

    info->result = child;
    return false;
}

RwFrame* RwFrame::FindChildByName( const char *name )
{
    _rwFrameFindName info;

    info.name = name;

    if ( ForAllChildren( RwFrameGetByName, &info ) )
        return NULL;

    return info.result;
}

struct _rwFrameFindHierarchy
{
    unsigned int    hierarchy;
    RwFrame*        result;
};

static bool RwFrameGetByHierarchy( RwFrame *child, _rwFrameFindHierarchy *info )
{
    if ( child->hierarchyId != info->hierarchy )
        return child->ForAllChildren( RwFrameGetByHierarchy, info );

    info->result = child;
    return false;
}

RwFrame* RwFrame::FindChildByHierarchy( unsigned int id )
{
    _rwFrameFindHierarchy info;

    info.hierarchy = id;

    if ( ForAllChildren( RwFrameGetByHierarchy, &info ) )
        return NULL;

    return info.result;
}

RwFrame* RwFrame::CloneRecursive() const
{
    RwFrame *cloned = RwFrameCloneRecursive( this, NULL );

    if ( !cloned )
        return NULL;

    cloned->privateFlags &= ~( RW_OBJ_REGISTERED | RW_FRAME_DIRTY );
    cloned->RegisterRoot();
    return cloned;
}

static bool RwFrameGetAnimHierarchy( RwFrame *frame, RpAnimHierarchy **rslt )
{
    if ( frame->anim )
    {
        *rslt = frame->anim;
        return false;
    }

    return frame->ForAllChildren( RwFrameGetAnimHierarchy, rslt );
}

static bool RwObjectGet( RwObject *child, RwObject **dst )
{
    *dst = child;
    return false;
}

RwObject* RwFrame::GetFirstObject()
{
    RwObject *obj;

    if ( ForAllObjects( RwObjectGet, &obj ) )
        return NULL;

    return obj;
}

struct _rwFindObjectType
{
    unsigned char type;
    RwObject *rslt;
};

static bool RwObjectGetByType( RwObject *child, _rwFindObjectType *info )
{
    if ( child->type != info->type )
        return true;

    info->rslt = child;
    return false;
}

RwObject* RwFrame::GetFirstObject( unsigned char type )
{
    _rwFindObjectType info;
    info.type = type;

    return ForAllObjects( RwObjectGetByType, &info ) ? NULL : info.rslt;
}

struct _rwObjectByIndex
{
    unsigned char type;
    unsigned int idx;
    unsigned int curIdx;
    RwObject *rslt;
};

static bool RwObjectGetByIndex( RwObject *obj, _rwObjectByIndex *info )
{
    if ( obj->type != info->type )
        return true;

    if ( info->idx != info->curIdx )
    {
        info->curIdx++;
        return true;
    }

    info->rslt = obj;
    return false;
}

RwObject* RwFrame::GetObjectByIndex( unsigned char type, unsigned int idx )
{
    _rwObjectByIndex info;
    info.type = type;
    info.idx = idx;
    info.curIdx = 0;
    
    if ( ForAllObjects( RwObjectGetByIndex, &info ) )
        return NULL;

    return info.rslt;
}

struct _rwObjCntByType
{
    unsigned char type;
    unsigned int cnt;
};

static bool RwFrameCountObjectsByType( RwObject *obj, _rwObjCntByType *info )
{
    if ( obj->type == info->type )
        info->cnt++;

    return true;
}

unsigned int RwFrame::CountObjectsByType( unsigned char type )
{
    _rwObjCntByType info;
    info.type = type;
    info.cnt = 0;

    ForAllObjects( RwFrameCountObjectsByType, &info );
    return info.cnt;
}

static bool RwFrameObjectGetLast( RwObject *obj, RwObject **dst )
{
    *dst = obj;
    return true;
}

RwObject* RwFrame::GetLastObject()
{
    RwObject *obj = NULL;

    ForAllObjects( RwFrameObjectGetLast, &obj );
    return obj;
}

static bool RwFrameObjectGetVisibleLast( RwObject *obj, RwObject **dst )
{
    if ( obj->flags & RW_OBJ_VISIBLE )
        *dst = obj;

    return true;
}

RwObject* RwFrame::GetLastVisibleObject()
{
    RwObject *obj = NULL;

    ForAllObjects( RwFrameObjectGetVisibleLast, &obj );
    return obj;
}

static bool RwObjectGetAtomic( RpAtomic *atomic, RpAtomic **dst )
{
    *dst = atomic;
    return false;
}

RpAtomic* RwFrame::GetFirstAtomic()
{
    RpAtomic *atomic;

    if ( ForAllAtomics( RwObjectGetAtomic, &atomic ) )
        return NULL;

    return atomic;
}

static bool RwObjectAtomicSetVisibilityFlags( RpAtomic *atomic, void *ud )
{
    atomic->ApplyVisibilityFlags( (unsigned short)ud );
    return true;
}

void RwFrame::SetAtomicVisibility( unsigned short flags )
{
    ForAllAtomics( RwObjectAtomicSetVisibilityFlags, (void*)flags );
}

static bool RwFrameAtomicBaseRoot( RpAtomic *atomic, RwFrame *root )
{
    RpAtomicSetFrame( atomic, root );
    return true;
}

struct _rwFrameVisibilityAtomics
{
    RpAtomic **primary;
    RpAtomic **secondary;
};

static bool RwFrameAtomicFindVisibility( RpAtomic *atomic, _rwFrameVisibilityAtomics *info )
{
    if ( atomic->GetVisibilityFlags() & 0x01 )
    {
        *info->primary = atomic;
        return true;
    }

    if ( atomic->GetVisibilityFlags() & 0x02 )
        *info->secondary = atomic;

    return true;
}

void RwFrame::FindVisibilityAtomics( RpAtomic **primary, RpAtomic **secondary )
{
    _rwFrameVisibilityAtomics info;

    info.primary = primary;
    info.secondary = secondary;

    ForAllAtomics( RwFrameAtomicFindVisibility, &info );
}

RpAnimHierarchy* RwFrame::GetAnimHierarchy()
{
    RpAnimHierarchy *_anim;

    if ( anim )
        return anim;

    // We want false, since it has to interrupt == found
    if ( ForAllChildren( RwFrameGetAnimHierarchy, &_anim ) )
        return NULL;

    return _anim;
}

void RwFrame::RegisterRoot()
{
    if ( !(root->privateFlags & ( RW_OBJ_REGISTERED | RW_FRAME_DIRTY ) ) )
    {
        // Add it to the internal list
        LIST_INSERT( pRwInterface->m_nodeRoot.root, nodeRoot );

        root->privateFlags |= RW_OBJ_REGISTERED | RW_FRAME_DIRTY;
    }

    privateFlags |= RW_OBJ_REGISTERED | RW_OBJ_HIERARCHY_CACHED;
}

void RwFrame::UnregisterRoot()
{
    if ( !(root->privateFlags & ( RW_OBJ_REGISTERED | 0x01 ) ) )
        return;

    LIST_REMOVE( nodeRoot );

    privateFlags &= ~(RW_OBJ_REGISTERED | 1);
}

static bool RwTexDictionaryGetFirstTexture( RwTexture *tex, RwTexture **rslt )
{
    *rslt = tex;
    return false;
}

RwTexture* RwTexDictionary::GetFirstTexture()
{
    RwTexture *tex;

    return ForAllTextures( RwTexDictionaryGetFirstTexture, &tex ) ? NULL : tex;
}

struct _rwTexDictFind
{
    const char *name;
    RwTexture *tex;
};

static bool RwTexDictionaryFindTexture( RwTexture *tex, _rwTexDictFind *find )
{
    if ( stricmp( tex->name, find->name ) != 0 )
        return true;

    find->tex = tex;
    return false;
}

RwTexture* RwTexDictionary::FindNamedTexture( const char *name )
{
    _rwTexDictFind find;
    find.name = name;

    if ( ForAllTextures( RwTexDictionaryFindTexture, &find ) )
        return NULL;

    return find.tex;
}

void RwTexture::AddToDictionary( RwTexDictionary *_txd )
{
    if ( txd )
        LIST_REMOVE( TXDList );

    LIST_INSERT( _txd->textures.root, TXDList );

    txd = _txd;
}

void RwTexture::RemoveFromDictionary()
{
    if ( !txd )
        return;

    LIST_REMOVE( TXDList );

    txd = NULL;
}

RwCamera* RwCameraCreate()
{
    RwCamera *cam = (RwCamera*)pRwInterface->m_allocStruct( pRwInterface->m_cameraInfo, 0x30005 );

    if ( !cam )
        return NULL;

    cam->type = RW_CAMERA;
    cam->subtype = 0;
    cam->flags = 0;
    cam->privateFlags = 0;
    cam->parent = NULL;

    cam->callback = (void*)0x007EE5A0;
    cam->preCallback = (RwCameraPreCallback)0x007EF370;
    cam->postCallback = (RwCameraPostCallback)0x007EF340;

    cam->screen.x = cam->screen.y = cam->screenInverse.x = cam->screenInverse.y = 1;
    cam->screenOffset.x = cam->screenOffset.y = 0;

    cam->nearplane = 0.05f;
    cam->farplane = 10;
    cam->fog = 5;

    cam->rendertarget = NULL;
    cam->bufferDepth = NULL;
    cam->camType = RW_CAMERA_PERSPECTIVE;

    // Do some plane shifting
    ((void (__cdecl*)( RwCamera* ))0x007EE200)( cam );

    cam->matrix.a = 0;

    RwObjectRegister( (void*)0x008E222C, cam );
    return cam;
}

void RwCamera::BeginUpdate()
{
    preCallback( this );
}

void RwCamera::EndUpdate()
{
    postCallback( this );
}

void RwCamera::AddToClump( RpClump *_clump )
{
    // Bugfix: remove from previous clump
    RemoveFromClump();

    LIST_INSERT( _clump->cameras.root, clumpCameras );

    clump = _clump;
}

void RwCamera::RemoveFromClump()
{
    if ( !clump )
        return;

    LIST_REMOVE( clumpCameras );

    clump = NULL;
}

RwStaticGeometry::RwStaticGeometry()
{
    count = 0;
    link = NULL;
}

RwRenderLink* RwStaticGeometry::AllocateLink( unsigned int _count )
{
    if ( link )
        RwFreeAligned( link );

    count = _count;
    return link = (RwRenderLink*)RwAllocAligned( (((_count * sizeof(RwRenderLink) - 1) >> 6 ) + 1) << 6, 0x40 );
}

bool RpAtomic::IsNight()
{
    if ( pipeline == RW_ATOMIC_RENDER_NIGHT )
        return true;

    if ( pipeline == RW_ATOMIC_RENDER_REFLECTIVE )
        return false;

    return geometry->nightColor && geometry->colors != NULL;
}

void RpAtomic::AddToClump( RpClump *_clump )
{
    RemoveFromClump();

    clump = _clump;

    LIST_INSERT( _clump->atomics.root, atomics );
}

void RpAtomic::RemoveFromClump()
{
    if ( !clump )
        return;

    LIST_REMOVE( atomics );

    clump = NULL;
}

void RpAtomic::SetRenderCallback( RpAtomicCallback callback )
{
    if ( !callback )
    {
        renderCallback = (RpAtomicCallback)RpAtomicRender;
        return;
    }

    renderCallback = callback;
}

void RpAtomic::ApplyVisibilityFlags( unsigned short flags )
{
    *(unsigned short*)&matrixFlags |= flags;
}

void RpAtomic::RemoveVisibilityFlags( unsigned short flags )
{
    *(unsigned short*)&matrixFlags &= ~flags;
}

unsigned short RpAtomic::GetVisibilityFlags()
{
    return *(unsigned short*)&matrixFlags;
}

void RpAtomic::SetExtendedRenderFlags( unsigned short flags )
{
    *(unsigned short*)&renderFlags = flags;
}

void RpAtomic::FetchMateria( RpMaterials& mats )
{
    unsigned int n;

    if ( !(GetVisibilityFlags() & 0x20) )
        return;

    for ( n = 0; n < geometry->linkedMateria->m_count; n++ )
        mats.Add( geometry->linkedMateria->Get(n)->m_material );
}

RpMaterials::RpMaterials( unsigned int max )
{
    m_data = (RpMaterial**)pRwInterface->m_malloc( sizeof(long) * max );

    m_max = max;
    m_entries = 0;
}

RpMaterials::~RpMaterials()
{
    unsigned int n;

    for ( n=0; n<m_entries; n++ )
        RpMaterialDestroy( m_data[n] );

    pRwInterface->m_free( m_data );

    m_entries = 0;
    m_max = 0;
}

bool RpMaterials::Add( RpMaterial *mat )
{
    if ( m_entries == m_max )
        return false;   // We do not need RW error handlers anymore?

    // Reference it
    mat->refs++;

    m_data[ m_entries++ ] = mat;
    return true;
}

RwLinkedMaterial* RwLinkedMateria::Get( unsigned int index )
{
    if ( index >= m_count )
        return NULL;

    return (RwLinkedMaterial*)(this + 1) + index;
}

void RpLight::SetLightIndex( unsigned int idx )
{
    lightIndex = min( idx, 8 );
}

void RpLight::AddToClump( RpClump *_clump )
{
    // Bugfix: remove from previous clump
    RemoveFromClump();

    LIST_INSERT( _clump->lights.root, clumpLights );

    clump = _clump;
}

void RpLight::RemoveFromClump()
{
    if ( !clump )
        return;

    LIST_REMOVE( clumpLights );

    clump = NULL;
}

void RpLight::AddToScene_Global( RwScene *_scene )
{
    RemoveFromScene();

    scene = _scene;

    LIST_INSERT( _scene->m_globalLights.root, sceneLights );
}

void RpLight::AddToScene_Local( RwScene *_scene )
{
    RemoveFromScene();

    scene = _scene;

    if ( _scene->parent )
        _scene->parent->RegisterRoot();

    LIST_INSERT( scene->m_localLights.root, sceneLights );
}

void RpLight::AddToScene( RwScene *scene )
{
    if ( subtype < 0x80 )
        AddToScene_Global( scene );
    else
        AddToScene_Local( scene );
}

void RpLight::RemoveFromScene()
{
    if ( !scene )
        return;

    RwSceneRemoveLight( scene, this );
}

void RpClump::Render()
{
    LIST_FOREACH_BEGIN( RpAtomic, atomics.root, atomics )
        if ( item->IsVisible() )
        {
            item->parent->GetLTM();     // Possibly update it's world position
            item->renderCallback( item );
        }
    LIST_FOREACH_END
}

static bool RwAssignRenderLink( RwFrame *child, RwRenderLink **link )
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

#define MAX_BONES   64

void RpClump::InitStaticSkeleton()
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
        unsigned int n;

        // Get the real bone positions
        GetBoneTransform( boneOffsets );

        // I guess its always one bone ahead...?
        link++;

        for (n=0; n<boneCount; n++)
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

RwStaticGeometry* RpClump::CreateStaticGeometry()
{
    return m_static = new RwStaticGeometry();
}

RpAnimHierarchy* RpClump::GetAtomicAnimHierarchy()
{
    RpAtomic *atomic = GetFirstAtomic();

    if (!atomic)
        return NULL;
    
    return atomic->anim;
}

RpAnimHierarchy* RpClump::GetAnimHierarchy()
{
    return parent->GetAnimHierarchy();
}

struct _rwFrameScanHierarchy
{
    RwFrame **output;
    size_t max;
};

static bool RwFrameGetAssignedHierarchy( RwFrame *child, _rwFrameScanHierarchy *info )
{
    if ( child->hierarchyId && child->hierarchyId < info->max )
        info->output[child->hierarchyId] = child;

    return child->ForAllChildren( RwFrameGetAssignedHierarchy, info );
}

void RpClump::ScanAtomicHierarchy( RwFrame **atomics, size_t max )
{
    _rwFrameScanHierarchy info;

    info.output = atomics;
    info.max = max;

    parent->ForAllChildren( RwFrameGetAssignedHierarchy, &info );
}

RpAtomic* RpClump::GetFirstAtomic()
{
    if ( LIST_EMPTY( atomics.root ) )
        return NULL;

    return LIST_GETITEM( RpAtomic, atomics.root.next, atomics );
}

RpAtomic* RpClump::GetLastAtomic()
{
    if ( LIST_EMPTY( atomics.root ) )
        return NULL;

    return LIST_GETITEM( RpAtomic, atomics.root.prev, atomics );
}

struct _rwFindAtomicNamed
{
    const char *name;
    RpAtomic *rslt;
};

static bool RpClumpFindNamedAtomic( RpAtomic *atom, _rwFindAtomicNamed *info )
{
    if ( strcmp( atom->parent->szName, info->name ) != 0 )
        return true;

    info->rslt = atom;
    return false;
}

RpAtomic* RpClump::FindNamedAtomic( const char *name )
{
    _rwFindAtomicNamed info;
    info.name = name;

    return ForAllAtomics( RpClumpFindNamedAtomic, &info ) ? NULL : info.rslt;
}

static bool RwAtomicGet2dfx( RpAtomic *child, RpAtomic **atomic )
{
    // Crashfix, invalid geometry
    if ( !child->geometry )
        return true;

    if ( !child->geometry->m_2dfx || child->geometry->m_2dfx->m_count == 0 )
        return true;

    *atomic = child;
    return false;
}

RpAtomic* RpClump::Find2dfx()
{
    RpAtomic *atomic;

    if ( ForAllAtomics( RwAtomicGet2dfx, &atomic ) )
        return NULL;

    return atomic;
}

static bool RwAtomicSetupPipeline( RpAtomic *child, int )
{
    if ( child->IsNight() )
        RpAtomicSetupObjectPipeline( child );
    else if ( child->pipeline == RW_ATOMIC_RENDER_VEHICLE )
        RpAtomicSetupVehiclePipeline( child );

    return true;
}

void RpClump::SetupAtomicRender()
{
    ForAllAtomics( RwAtomicSetupPipeline, 0 );
}

static bool RwAtomicRemoveVisibilityFlags( RpAtomic *child, unsigned short flags )
{
    child->RemoveVisibilityFlags( flags );
    return true;
}

void RpClump::RemoveAtomicVisibilityFlags( unsigned short flags )
{
    ForAllAtomics( RwAtomicRemoveVisibilityFlags, flags );
}

static bool RwAtomicFetchMateria( RpAtomic *child, RpMaterials *mats )
{
    child->FetchMateria( *mats );
    return true;
}

void RpClump::FetchMateria( RpMaterials& mats )
{
    ForAllAtomics( RwAtomicFetchMateria, &mats );
}

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
    for (n=1; n<skel->boneCount; n++)
    {
        RwMatrix mat;

        RwMatrixInvert( &mat, skelMat );

        pRwInterface->m_matrixTransform3( offset, &mat.up, 1, skel->boneMatrices + matId );

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

bool RwMaterialAlphaCheck( RpMaterial *mat, int )
{
    return mat->color.a != 0xFF;
}

bool RpGeometry::IsAlpha()
{
    return !ForAllMateria( RwMaterialAlphaCheck, 0 );
}

bool RpMaterialTextureUnlink( RpMaterial *mat, int )
{
    if ( RwTexture *tex = mat->texture )
        mat->texture = NULL;

    return true;
}

void RpGeometry::UnlinkFX()
{
    // Clean all texture links
    ForAllMateria( RpMaterialTextureUnlink, 0 );

    if ( m_2dfx )
    {
        // Clean the 2dfx structure
        pRwInterface->m_free( m_2dfx );
        m_2dfx = NULL;
    }
}

bool RwAtomicRenderChainInterface::PushRender( RwAtomicZBufferEntry *level )
{
    RwAtomicRenderChain *iter = &m_root;
    RwAtomicRenderChain *progr;

    while ( iter->list.prev != &m_rootLast.list )
    {
        iter = LIST_GETITEM(RwAtomicRenderChain, iter->list.prev, list);

        if ( iter->m_entry.m_distance >= level->m_distance )
            break;
    }

    if ( ( progr = LIST_GETITEM(RwAtomicRenderChain, m_renderStack.list.prev, list) ) == &m_renderLast )
        return false;

    // Update render details
    progr->m_entry = *level;

    LIST_REMOVE( progr->list );
    LIST_INSERT( iter->list, progr->list );
    return true;
}

RwTexture* RwFindTexture( const char *name, const char *secName )
{
    RwTexture *tex = pRwInterface->m_textureManager.m_findInstanceRef( name );

    // The global store will reference textures
    if ( tex )
    {
        tex->refs++;
        return tex;
    }

    if ( !( tex = pRwInterface->m_textureManager.m_findInstance( name, secName ) ) )
    {
        // If we have not found anything, we tell the system about an error
        RwError err;
        err.err1 = 0x01;
        err.err2 = 0x16;

        // Actually, there is a missing texture handler; it is void though
        RwSetError( &err );
        return NULL;
    }

    if ( RwTexDictionary *txd = pRwInterface->m_textureManager.m_current )
    {
        tex->RemoveFromDictionary();
        tex->AddToDictionary( txd );
    }

    return tex;
}

RwError* RwSetError( RwError *info )
{
    if ( pRwInterface->m_errorInfo.err1 )
        return info;

    if ( pRwInterface->m_errorInfo.err2 != 0x80000000 )
        return info;

    if ( info->err1 & 0x80000000 )
        pRwInterface->m_errorInfo.err1 = 0;
    else
        pRwInterface->m_errorInfo.err1 = info->err1;

    pRwInterface->m_errorInfo.err2 = info->err2;
    return info;
}

static void* _lightCallback( void *ptr )
{
    return ptr;
}

RpLight* RpLightCreate( unsigned char type )
{
    RpLight *light = (RpLight*)pRwInterface->m_allocStruct( pRwInterface->m_lightInfo, 0x30012 );

    if ( !light )
        return NULL;

    light->type = RW_LIGHT;
    light->subtype = type;
    light->color.a = 0;
    light->color.r = 0;
    light->color.g = 0;
    light->color.b = 0;

    light->callback = (void*)_lightCallback;
    light->flags = 0;
    light->parent = NULL;

    light->radius = 0;
    light->coneAngle = 0;
    light->privateFlags = 0;

    // Clear the list awareness
    LIST_CLEAR( light->sectors.root );
    LIST_INITNODE( light->clumpLights );

    light->flags = 3; // why write it again? R* hack?
    light->frame = pRwInterface->m_frame;

    RwObjectRegister( (void*)0x008D62F8, light );
    return light;
}

void RpLight::SetColor( const RwColorFloat& _color )
{
    color = _color;
    
    // Check whether we are brightness only
    privateFlags = ( color.r == color.g && color.r == color.b );
}

static RpClump* _clumpCallback( RpClump *clump, void *data )
{
    return clump;
}

RpClump* RpClumpCreate()
{
    RpClump *clump = (RpClump*)pRwInterface->m_allocStruct( pRwInterface->m_clumpInfo, 0x30010 );

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