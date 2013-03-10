/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rwapi.cpp
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
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
*   WARNING: Do not modify these classes if you do not know what you are doing!
*   They are C++ representations of internal GTA:SA logic. I suggest you analyze
*   the internals first before you touch the RenderWare interfaces. Any change
*   might break the compatibility with the engine, so be careful.
*
*****************************************************************************/

RwAtomicRenderChainInterface *const rwRenderChains = (RwAtomicRenderChainInterface*)0x00C88070;
RwScene *const *p_gtaScene = (RwScene**)0x00C17038;


/*=========================================================
    RwObjectFrame::AddToFrame

    Arguments:
        frame - new parent frame
    Purpose:
        Set the parent for a frame extension object. The
        previous parent is unlinked.
    Binary offsets:
        (1.0 US): 0x0074BF20
        (1.0 EU): 0x0074BF70
=========================================================*/
void RwObjectFrame::AddToFrame( RwFrame *frame )
{
    RemoveFromFrame();

    parent = frame;
    privateFlags |= RW_FRAME_DIRTY;

    if ( !frame )
        return;

    LIST_INSERT( frame->objects.root, lFrame );
}

/*=========================================================
    RwObjectFrame::RemoveFromFrame

    Purpose:
        Unlink the current parent frame if there is one.
=========================================================*/
void RwObjectFrame::RemoveFromFrame( void )
{
    if ( !parent )
        return;

    LIST_REMOVE( lFrame );

    parent = NULL;
}

/*=========================================================
    RwFrame::SetModelling

    Arguments:
        mat - matrix to apply to the frame
    Purpose:
        Update the modelling matrix of the frame. It will flag the
        frame for updating. The flag will be checked once the
        local transformation matrix (LTM) is retrieved. The LTM
        will then be recalculated.
        The modelling matrix is the local offset of the frame based
        on the absolute position of the parent frame.
=========================================================*/
void RwFrame::SetModelling( const RwMatrix& mat )
{
    modelling = mat;

    // Set the frame to dirty
    privateFlags |= RW_FRAME_DIRTY;
}

/*=========================================================
    RwFrame::SetPosition

    Arguments:
        pos - vector for new frame position
    Purpose:
        Set the modelling position of the frame without changing
        the frame's rotation. It flags the frame to update the LTM
        once requested.
=========================================================*/
void RwFrame::SetPosition( const CVector& pos )
{
    modelling.pos = pos;

    // Set the frame to dirty
    privateFlags |= RW_FRAME_DIRTY;
}

/*=========================================================
    RwFrame::GetLTM

    Purpose:
        Returns the current local transformation matrix (LTM).
        This is the absolute position of the frame in the scene;
        all objects are rendered at the LTM matrix position and
        rotation. If the frame changes it's modelling matrix, the
        LTM is updated in this function. Updating the LTM does
        not make sense outside this function. That is why RwMatrix
        is returned constant.
    Binary offsets:
        (1.0 US): 0x007F0990
        (1.0 EU): 0x007F09D0
=========================================================*/
const RwMatrix& RwFrame::GetLTM( void )
{
    // This function will recalculate the LTM if frame is dirty
    return *RwFrameGetLTM( this );
}

/*=========================================================
    RwFrame::Link

    Arguments:
        frame - child to add to the frame
    Purpose:
        Adds a child to the frame. The child will have this frame
        as parent, while the previous parent is unlinked. Reparenting
        the frame this way will update it's frame root, too.
        Children frames cannot be root themselves.
    Binary offsets:
        (1.0 US): 0x007F0B00
        (1.0 EU): 0x007F0A00
=========================================================*/
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

/*=========================================================
    RwFrame::Unlink

    Purpose:
        Unparents this frame from any hierarchy. This frame will
        then be a root frame.
    Binary offsets:
        (1.0 US): 0x007F0CD0
        (1.0 EU): 0x007F0D10
=========================================================*/
void RwFrame::Unlink( void )
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

/*=========================================================
    RwFrame::SetRootForHierarchy

    Arguments:
        _root - root frame to be set for this frame and all children
    Purpose:
        Sets the root frame for this frame hierarchy. This function
        should not be used outside the children system.
    Binary offsets:
        (1.0 US): 0x007F0210
        (1.0 EU): 0x007F0250
=========================================================*/
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

/*=========================================================
    RwFrame::CountChildren

    Purpose:
        Returns the number of children frames present in this frame
        hierarchy. This means that it scans recursively.
    Binary offsets:
        (1.0 US): 0x007F0E00
        (1.0 EU): 0x007F0E40
    Note:
        The function at the binary offsets is RwFrameCountHierarchyFrames.
        It returns 1 at least, since it stands for parent frame itself.
=========================================================*/
static bool RwFrameGetChildCount( RwFrame *child, unsigned int *count )
{
    child->ForAllChildren( RwFrameGetChildCount, count );

    (*count)++;
    return true;
}

unsigned int RwFrame::CountChildren( void )
{
    unsigned int count = 0;

    ForAllChildren( RwFrameGetChildCount, &count );
    return count;
}

/*=========================================================
    RwFrame::GetFirstChild

    Purpose:
        Returns the first child frame of this frame.
=========================================================*/
RwFrame* RwFrame::GetFirstChild( void )
{
    return child;
}

/*=========================================================
    RwFrame::FindFreeChildByName (GTA:SA extension)

    Arguments:
        name - name of the frame to find
    Purpose:
        Returns the first child with the matching name that has
        not been assigned to a hierarchy yet. Assigned to a hierarchy
        means that hierarchyId is != 0.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C52F0
    Note:
        At the binary offset is the handler which is passed to
        ForAllChildren.
=========================================================*/
struct _rwFrameFindName
{
    const char *name;
    RwFrame *result;
};

static bool RwFrameGetFreeByName( RwFrame *child, _rwFrameFindName *info )
{
    if ( child->hierarchyId || stricmp(child->szName, info->name) != 0 )
        return child->ForAllChildren( RwFrameGetFreeByName, info );

    info->result = child;
    return false;
}

RwFrame* RwFrame::FindFreeChildByName( const char *name )
{
    _rwFrameFindName info;
    info.name = name;

    return ForAllChildren( RwFrameGetFreeByName, &info ) ? NULL : info.result;
}

/*=========================================================
    RwFrame::FindChildByName

    Arguments:
        name - name of the frame you want to find
    Purpose:
        Returns the first frame with the matching name.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C5400
=========================================================*/
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

/*=========================================================
    RwFrame::FindChildByHierarchy

    Arguments:
        id - hierarchyId of the frame you want to find
    Purpose:
        Returns the first frame with the matching hierarchyId.
        hierarchyId represents the index of the atomic model
        construction information. A clump model is constructed
        by this information at CClumpModelInfoSAInterface::SetClump.
        hierarchyId can then be used at runtime to retrieve that
        unique frame which the constructor routine picked.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C53C0
=========================================================*/
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

    return ForAllChildren( RwFrameGetByHierarchy, &info ) ? NULL : info.result;
}

/*=========================================================
    RwFrame::CloneRecursive

    Purpose:
        Returns a newly allocated frame which is the exact copy
        of this frame. That means that all it's objects and children
        frames have been cloned.
    Binary offsets:
        (1.0 US): 0x007F0050
        (1.0 EU): 0x007F0090
=========================================================*/
RwFrame* RwFrame::CloneRecursive( void ) const
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

/*=========================================================
    RwFrame::GetFirstObject

    Purpose:
        Returns the first object of this frame.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007348C0
=========================================================*/
RwObject* RwFrame::GetFirstObject( void )
{
    if ( LIST_EMPTY( objects.root ) )
        return NULL;

    return LIST_GETITEM( RwObjectFrame, objects.root.next, lFrame );
}

/*=========================================================
    RwFrame::GetFirstObject

    Arguments:
        type - type of the object you want to find
    Purpose:
        Returns the first frame with the matching type.
=========================================================*/
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

/*=========================================================
    RwFrame::GetObjectByIndex

    Arguments:
        type - eRwType of the object you want to find
        idx - index of the object
    Purpose:
        Returns the object it finds which is indexed by idx.
        The type of the object has to match type. Only the objects
        of this frame are scanned.
=========================================================*/
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

/*=========================================================
    RwFrame::CountObjectsByType

    Arguments:
        type - eRwType of object instances you want to count
    Purpose:
        Returns the number of objects of specified type in this frame.
=========================================================*/
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

/*=========================================================
    RwFrame::GetLastObject

    Purpose:
        Returns the last object in this frame.
=========================================================*/
RwObject* RwFrame::GetLastObject( void )
{
    if ( LIST_EMPTY( objects.root ) )
        return NULL;
    
    return LIST_GETITEM( RwObjectFrame, objects.root.prev, lFrame );
}

/*=========================================================
    RwFrame::GetLastVisibleObject

    Purpose:
        Returns the last object which was found visible.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x006A0750
    Note:
        At the binary offset location is the handler for
        ForAllObjects.
=========================================================*/
static bool RwFrameObjectGetVisibleLast( RwObject *obj, RwObject **dst )
{
    if ( obj->IsVisible() )
        *dst = obj;

    return true;
}

RwObject* RwFrame::GetLastVisibleObject( void )
{
    RwObject *obj = NULL;

    ForAllObjects( RwFrameObjectGetVisibleLast, &obj );
    return obj;
}

/*=========================================================
    RwFrame::GetFirstAtomic

    Purpose:
        Returns the first atomic type object in this frame.
    Note:
        This function was created as a ossible bugfix for GTA:SA.
        When the engine handles frames, it automatically assumes
        that their objects are all atomics which they may not be
        (light, camera).
=========================================================*/
static bool RwObjectGetAtomic( RpAtomic *atomic, RpAtomic **dst )
{
    *dst = atomic;
    return false;
}

RpAtomic* RwFrame::GetFirstAtomic( void )
{
    RpAtomic *atomic;

    return ForAllAtomics( RwObjectGetAtomic, &atomic ) ? NULL : atomic;
}

/*=========================================================
    RwFrame::SetAtomicVisibility

    Arguments:
        flags - visibility flags for all atomics
    Purpose:
        Applies the specified visibility flags for all atomics
        in this frame.
=========================================================*/
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
    atomic->AddToFrame( root );
    return true;
}

/*=========================================================
    RwFrame::FindVisibilityAtomics

    Arguments:
        primary - atomic of primary visibility type
        secondary - atomic of secondary visibility type
    Purpose:
        Returns the last atomics which are either primary or
        secondary visibility. Otherwise, the values are left
        untouched [if you are unsure that the frame has the
        atomics, initialize the values to NULL yourself].
=========================================================*/
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

/*=========================================================
    RwFrame::GetAnimHierarchy

    Purpose:
        Returns either the frame anim hierarchy or the first
        one found at atomics.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734AB0
    Note:
        The concept of RpAnimHierarchy has not been researched
        entirely yet.
=========================================================*/
RpAnimHierarchy* RwFrame::GetAnimHierarchy( void )
{
    RpAnimHierarchy *_anim;

    if ( anim )
        return anim;

    // We want false, since it has to interrupt == found
    return ForAllChildren( RwFrameGetAnimHierarchy, &_anim ) ? NULL : _anim;
}

/*=========================================================
    RwFrame::RegisterRoot

    Purpose:
        Marks this frame as root frame. All root frames
        are registered in the main RenderWare interface.
        Root frames are not children; they may not have a
        parent frame.
    Binary offsets:
        (1.0 US): 0x007F0910
        (1.0 EU): 0x007F0950
    Note:
        This function has been inlined into other RenderWare
        functions. Look closely at the pattern to find out
        where!
=========================================================*/
void RwFrame::RegisterRoot( void )
{
    if ( !(root->privateFlags & ( RW_OBJ_REGISTERED | RW_FRAME_DIRTY ) ) )
    {
        // Add it to the internal list
        LIST_INSERT( pRwInterface->m_nodeRoot.root, nodeRoot );

        root->privateFlags |= RW_OBJ_REGISTERED | RW_FRAME_DIRTY;
    }

    privateFlags |= RW_OBJ_REGISTERED | RW_OBJ_HIERARCHY_CACHED;
}

/*=========================================================
    RwFrame::UnregisterRoot

    Purpose:
        Unregisters this frame, so that it is not a root frame
        anymore. This is done if the frame turns a child; it
        receives a parent frame.
    Note:
        This function has been heavily inlined and does only
        occur in RwFrame::Unlink.
=========================================================*/
void RwFrame::UnregisterRoot( void )
{
    if ( !(root->privateFlags & ( RW_OBJ_REGISTERED | 0x01 ) ) )
        return;

    LIST_REMOVE( nodeRoot );

    privateFlags &= ~(RW_OBJ_REGISTERED | 1);
}

/*=========================================================
    RwTexDictionary::GetFirstTexture

    Purpose:
        Returns the first texture of this TXD.
=========================================================*/
RwTexture* RwTexDictionary::GetFirstTexture( void )
{
    if ( LIST_EMPTY( textures.root ) )
        return NULL;

    return LIST_GETITEM( RwTexture, textures.root.next, TXDList );
}

/*=========================================================
    RwTexDictionary::FindNamedTexture

    Arguments:
        name - case-insensitive name to check the textures against
    Purpose:
        Scans the textures of this TXD and returns the first one
        which is found with the name. If not, it returns NULL.
    Binary offsets:
        (1.0 US): 0x007F39F0
        (1.0 EU): 0x007F3A30
=========================================================*/
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

    return ForAllTextures( RwTexDictionaryFindTexture, &find ) ? NULL : find.tex;
}

/*=========================================================
    RwTexture::AddToDictionary

    Arguments:
        _txd - the new TXD to apply this texture to
    Purpose:
        Assigns this texture to another TXD container.
        It unlinks this texture from the previous TXD.
    Binary offsets:
        (1.0 US): 0x007F3980
        (1.0 EU): 0x007F39C0
    Note:
        At the binary offset location actually is
        RwTexDictionaryAddTexture.
=========================================================*/
void RwTexture::AddToDictionary( RwTexDictionary *_txd )
{
    if ( txd )
        LIST_REMOVE( TXDList );

    LIST_INSERT( _txd->textures.root, TXDList );

    txd = _txd;
}

/*=========================================================
    RwTexture::RemoveFromDictionary

    Purpose:
        Unlinks this texture from the TXD container it is
        assigned to.
    Binary offsets:
        (1.0 US): 0x007F39C0
        (1.0 EU): 0x007F3A00
=========================================================*/
void RwTexture::RemoveFromDictionary( void )
{
    if ( !txd )
        return;

    LIST_REMOVE( TXDList );

    txd = NULL;
}

/*=========================================================
    RwCameraCreate

    Purpose:
        Returns a new RenderWare camera plugin instance.
    Binary offsets:
        (1.0 US): 0x007EE4F0
        (1.0 EU): 0x007EE530
=========================================================*/
RwCamera* RwCameraCreate( void )
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

    cam->screen.fX = cam->screen.fY = cam->screenInverse.fX = cam->screenInverse.fY = 1;
    cam->screenOffset.fX = cam->screenOffset.fY = 0;

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

/*=========================================================
    RwCamera::BeginUpdate

    Purpose:
        Enters the RenderWare rendering stage by notifying this
        camera. The direct3d 9 rendertarget is set to the
        camera's buffer. All atomic rendering calls will render
        on this camera's render target. Only one camera can
        be rendering at a time. The active camera is set at
        RwInterface::m_renderCam.
    Binary offsets:
        (1.0 US): 0x007EE190
        (1.0 EU): 0x007EE1D0
=========================================================*/
void RwCamera::BeginUpdate( void )
{
    preCallback( this );
}

/*=========================================================
    RwCamera::EndUpdate

    Purpose:
        Leaves the RenderWare rendering stage. It applies all
        rendering to the buffer. The camera is unset from
        RwInterface::m_renderCam.
    Binary offsets:
        (1.0 US): 0x007EE180
        (1.0 EU): 0x007EE1C0
=========================================================*/
void RwCamera::EndUpdate( void )
{
    postCallback( this );
}

/*=========================================================
    RwCamera::AddToClump

    Arguments:
        _clump - model to which the camera shall be added to
    Purpose:
        Adds this camera into the clump's list. It is unlinked
        from any previous clump.
    Note:
        The GTA:SA RenderWare function did fail to unlink
        the camera; that would result in crashes. This function
        fixed that issue.
    Binary offsets:
        (1.0 US): 0x0074A550
        (1.0 EU): 0x0074A5A0
    Note:
        At the binary offset location actually is
        RpClumpAddCamera.
=========================================================*/
void RwCamera::AddToClump( RpClump *_clump )
{
    // Bugfix: remove from previous clump
    RemoveFromClump();

    LIST_INSERT( _clump->cameras.root, clumpCameras );

    clump = _clump;
}

/*=========================================================
    RwCamera::RemoveFromClump

    Purpose:
        Unlists this camera from the clump's camera registry.
    Binary offsets:
        (1.0 US): 0x0074A580
        (1.0 EU): 0x0074A5D0
    Note:
        At the binary offset location actually is
        RpClumpRemoveCamera.
=========================================================*/
void RwCamera::RemoveFromClump( void )
{
    if ( !clump )
        return;

    LIST_REMOVE( clumpCameras );

    clump = NULL;
}

RwStaticGeometry::RwStaticGeometry( void )
{
    count = 0;
    link = NULL;
}

/*=========================================================
    RwStaticGeometry::AllocateLink (GTA:SA extension)

    Arguments:
        _count - number of links to allocate
    Purpose:
        Allocates a number of links to this static geometry.
        Previous links are discarded.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004CF140
=========================================================*/
RwRenderLink* RwStaticGeometry::AllocateLink( unsigned int _count )
{
    if ( link )
        RwFreeAligned( link );

    count = _count;
    return link = (RwRenderLink*)RwAllocAligned( (((_count * sizeof(RwRenderLink) - 1) >> 6 ) + 1) << 6, 0x40 );
}

/*=========================================================
    RpAtomic::IsNight (GTA:SA extension)

    Purpose:
        Returns whether this atomic is rendered using the night
        vertex colors extension.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005D7F40
=========================================================*/
bool RpAtomic::IsNight( void )
{
    if ( pipeline == RW_ATOMIC_RENDER_NIGHT )
        return true;

    if ( pipeline == RW_ATOMIC_RENDER_REFLECTIVE )
        return false;

    return geometry->nightColor && geometry->colors != NULL;
}

/*=========================================================
    RpAtomic::AddToClump

    Arguments:
        _clump - model to register this atomic at
    Purpose:
        Adds this atomic to a clump. The atomic is unlinked
        from any previous clump.
    Binary offsets:
        (1.0 US): 0x0074A490
        (1.0 EU): 0x0074A4E0
    Note:
        At the binary offset location actually is
        RpClumpAddAtomic.
=========================================================*/
void RpAtomic::AddToClump( RpClump *_clump )
{
    RemoveFromClump();

    clump = _clump;

    LIST_INSERT( _clump->atomics.root, atomics );
}

/*=========================================================
    RpAtomic::RemoveFromClump

    Purpose:
        Removes this atomic from any clump it may be registered at.
    Binary offsets:
        (1.0 US): 0x0074A4C0
        (1.0 EU): 0x0074A510
    Note:
        At the binary offset location actually is
        RpClumpRemoveAtomic.
=========================================================*/
void RpAtomic::RemoveFromClump( void )
{
    if ( !clump )
        return;

    LIST_REMOVE( atomics );

    clump = NULL;
}

/*=========================================================
    RpAtomic::SetRenderCallback

    Arguments:
        callback - an anonymous function which is called at rendering
    Purpose:
        Sets a new rendering callback to this atomic. If NULL is
        given, then the rendering callback of this atomic is set
        to the default one.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007328A0
=========================================================*/
void RpAtomic::SetRenderCallback( RpAtomicCallback callback )
{
    if ( !callback )
    {
        renderCallback = (RpAtomicCallback)RpAtomicRender;
        return;
    }

    renderCallback = callback;
}

/*=========================================================
    RpAtomic::ApplyVisibilityFlags
    (currently under investigation)

    Arguments:
        flags - visibility flags for this atomic
    Purpose:
        Applies the specified visibility flags for this atomic.
        This means that they are OR'd to the existing flags.
=========================================================*/
void RpAtomic::ApplyVisibilityFlags( unsigned short flags )
{
    *(unsigned short*)&matrixFlags |= flags;
}

/*=========================================================
    RpAtomic::RemoveVisibilityFlags
    (currently under investigation)

    Arguments:
        flags - visibility flags for this atomic
    Purpose:
        Removes visibility flags from this atomic.
=========================================================*/
void RpAtomic::RemoveVisibilityFlags( unsigned short flags )
{
    *(unsigned short*)&matrixFlags &= ~flags;
}

/*=========================================================
    RpAtomic::GetVisibilityFlags
    (currently under investigation)

    Purpose:
        Returns the current atomic visibility flags.
=========================================================*/
unsigned short RpAtomic::GetVisibilityFlags( void )
{
    return *(unsigned short*)&matrixFlags;
}

/*=========================================================
    RpAtomic::SetExtendedRenderFlags
    (currently under investigation)

    Arguments:
        flags - extended render flags to set
    Purpose:
        Sets the extended render flags of this atomic
=========================================================*/
void RpAtomic::SetExtendedRenderFlags( unsigned short flags )
{
    *(unsigned short*)&renderFlags = flags;
}

/*=========================================================
    RpAtomic::FetchMateria

    Arguments:
        mats - container for material storage
    Purpose:
        Adds all materia of this atomic to a specified container.
        It fails if a special visibility flag is set.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8B60
=========================================================*/
void RpAtomic::FetchMateria( RpMaterials& mats )
{
    if ( !(GetVisibilityFlags() & 0x20) )
        return;

    for ( unsigned int n = 0; n < geometry->linkedMaterials->m_count; n++ )
        mats.Add( geometry->linkedMaterials->Get(n)->m_material );
}

RpMaterials::RpMaterials( unsigned int max )
{
    m_data = (RpMaterial**)pRwInterface->m_malloc( sizeof(long) * max );

    m_max = max;
    m_entries = 0;
}

RpMaterials::~RpMaterials( void )
{
    for ( unsigned int n = 0; n < m_entries; n++ )
        RpMaterialDestroy( m_data[n] );

    pRwInterface->m_free( m_data );

    m_entries = 0;
    m_max = 0;
}

/*=========================================================
    RpMaterials::Add

    Arguments:
        mat - new material to add to the storage
    Purpose:
        Registers another material to this storage. It fails if
        this container is full.
    Binary offsets:
        (1.0 US): 0x0074E350
        (1.0 EU): 0x0074E3A0
=========================================================*/
bool RpMaterials::Add( RpMaterial *mat )
{
    if ( m_entries == m_max )
        return false;   // We do not need RW error handlers anymore?

    // Reference it
    mat->refs++;

    m_data[ m_entries++ ] = mat;
    return true;
}

/*=========================================================
    RwLinkedMaterials::Get

    Arguments:
        index - index of the material you wish to retrieve
    Purpose:
        Returns an indexed linked material from this storage.
=========================================================*/
RwLinkedMaterial* RwLinkedMaterials::Get( unsigned int index )
{
    if ( index >= m_count )
        return NULL;

    return (RwLinkedMaterial*)( this + 1 ) + index;
}

/*=========================================================
    RpLight::SetLightIndex

    Arguments:
        idx - number from 0 to 8 representing the hardware light index
    Purpose:
        Sets the hardware light index. It is used in the rendering
        stage. Only one light with the same index can be active during
        rendering. Light indices are not dynamically managed by
        RenderWare (at the moment). The first time a light is assigned
        to an atomic, it gains a light index. Settings this light index
        to 0 will force an update to a freely available index at next
        atomic render.
=========================================================*/
void RpLight::SetLightIndex( unsigned int idx )
{
    lightIndex = min( idx, 8 );
}

/*=========================================================
    RpLight::AddToClump

    Arguments:
        _clump - model registry to put this light into
    Purpose:
        Registers this light at another clump. It unregisters
        it from the previous clump.
    Binary offsets:
        (1.0 US): 0x0074A4F0
        (1.0 EU): 0x0074A540
    Note:
        At the binary offset location actually is
        RpClumpAddLight.
=========================================================*/
void RpLight::AddToClump( RpClump *_clump )
{
    // Bugfix: remove from previous clump
    RemoveFromClump();

    LIST_INSERT( _clump->lights.root, clumpLights );

    clump = _clump;
}

/*=========================================================
    RpLight::RemoveFromClump

    Purpose:
        Removes this light from any clump it might be registered at.
    Binary offsets:
        (1.0 US): 0x0074A520
        (1.0 EU): 0x0074A570
    Note:
        At the binary offset location actually is
        RpClumpRemoveLight.
=========================================================*/
void RpLight::RemoveFromClump( void )
{
    if ( !clump )
        return;

    LIST_REMOVE( clumpLights );

    clump = NULL;
}

/*=========================================================
    RpLight::AddToScene

    Arguments:
        _scene - scene to register this light at
    Purpose:
        Puts this light into a scene. It will interact with
        all atomics inside of the scene. Global lights are applied
        to all atomics without position preference. Local lights
        are tailored along sectors, applying to in-range atomics.
    Binary offsets:
        (1.0 US): 0x00751910
        (1.0 EU): 0x00751960
    Note:
        At the binary offset location actually is
        RwSceneAddLight.
=========================================================*/
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

/*=========================================================
    RpLight::RemoveFromScene

    Purpose:
        Unregisters this light from a scene it might be inside.
    Binary offsets:
        (1.0 US): 0x00751960
        (1.0 EU): 0x007519B0
    Note:
        At the binary offset location actually is
        RwSceneRemoveLight.
=========================================================*/
void RpLight::RemoveFromScene( void )
{
    if ( !scene )
        return;

    RwSceneRemoveLight( scene, this );
}

/*=========================================================
    RpClump::Render

    Purpose:
        Renders all atomics of this clump if they are flagged
        visible.
    Binary offsets:
        (1.0 US): 0x00749B20
        (1.0 EU): 0x00749B70
=========================================================*/
void RpClump::Render( void )
{
    LIST_FOREACH_BEGIN( RpAtomic, atomics.root, atomics )
        if ( item->IsVisible() )
        {
            item->parent->GetLTM();     // Possibly update it's world position
            item->renderCallback( item );
        }
    LIST_FOREACH_END
}

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
    return m_static = new RwStaticGeometry();
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

static bool RwFrameGetAssignedHierarchy( RwFrame *child, _rwFrameScanHierarchy *info )
{
    if ( child->hierarchyId && child->hierarchyId < info->max )
        info->output[child->hierarchyId] = child;

    return child->ForAllChildren( RwFrameGetAssignedHierarchy, info );
}

void RpClump::ScanFrameHierarchy( RwFrame **frames, size_t max )
{
    _rwFrameScanHierarchy info;

    info.output = frames;
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
struct _rwFindAtomicNamed
{
    const char *name;
    RpAtomic *rslt;
};

static bool RpClumpFindNamedAtomic( RpAtomic *atom, _rwFindAtomicNamed *info )
{
    if ( stricmp( atom->parent->szName, info->name ) != 0 )
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

/*=========================================================
    RpClump::Find2dfx

    Purpose:
        Returns the first atomic whose geometry has valid 2dfx
        data. Valid == at least one effect type allocated to the
        2dfx container. If not found, it returns NULL.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734880
=========================================================*/
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

RpAtomic* RpClump::Find2dfx( void )
{
    RpAtomic *atomic;

    return ForAllAtomics( RwAtomicGet2dfx, &atomic ) ? NULL : atomic;
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
static bool RwAtomicSetupPipeline( RpAtomic *child, int )
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
    RpClump::RemoveAtomicVisibilityFlags

    Arguments:
        flags - the flags you wish to remove from all atomics
    Purpose:
        Loops through all atomics of this clump and removes the
        specified visibility flags.
=========================================================*/
static bool RwAtomicRemoveVisibilityFlags( RpAtomic *child, unsigned short flags )
{
    child->RemoveVisibilityFlags( flags );
    return true;
}

void RpClump::RemoveAtomicVisibilityFlags( unsigned short flags )
{
    ForAllAtomics( RwAtomicRemoveVisibilityFlags, flags );
}

/*=========================================================
    RpClump::FetchMateria

    Arguments:
        mats - container to store all materials at
    Purpose:
        Lists the materials of all atomics registered in this clump.
        Be sure to allocate a big RpMaterials container for this.
=========================================================*/
static bool RwAtomicFetchMateria( RpAtomic *child, RpMaterials *mats )
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

    // Reset the offset
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

/*=========================================================
    RpGeometry::IsAlpha

    Purpose:
        Returns whether the geometry requires alpha blending.
=========================================================*/
bool RwMaterialAlphaCheck( RpMaterial *mat, int )
{
    return mat->color.a != 0xFF;
}

bool RpGeometry::IsAlpha( void )
{
    return !ForAllMateria( RwMaterialAlphaCheck, 0 );
}

/*=========================================================
    RpGeometry::UnlinkFX

    Purpose:
        Scans through all assgined textures and effects of this 
        geometry and un-gracefully removes the references to them.
    Note:
        This function is outdated and hacky. Do not use it.
        It was used to debug the RenderWare texture system.
        We have since then developed much better ways.
=========================================================*/
bool RpMaterialTextureUnlink( RpMaterial *mat, int )
{
    if ( RwTexture *tex = mat->texture )
        mat->texture = NULL;

    return true;
}

void RpGeometry::UnlinkFX( void )
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

/*=========================================================
    RwAtomicRenderChainInterface::PushRender

    Arguments:
        level - the description of the new entry
    Purpose:
        Queries to render this atomic internally using the specified
        callback. The new render request will occupy a slot.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00733910
    Note:
        Apparrently the render chains have a pre-allocated amount of
        possible render instances. We should investigate, how this limit
        affects the performance and quality of gameplay.
=========================================================*/
bool RwAtomicRenderChainInterface::PushRender( RwAtomicZBufferEntry *level )
{
    RwAtomicRenderChain *iter = &m_root;
    RwAtomicRenderChain *progr;

    // Check at which position we may insert the z-buffer entry
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

/*=========================================================
    RwFindTexture

    Arguments:
        name - primary name of the texture you want to find
        secName - name of a fallback texture if primary one is not found
    Purpose:
        Scans the global and the current TXD containers for a
        matching texture. If the TXD is found in the current TXD
        (m_findInstanceRef), it is referenced and returned to the
        caller. If not, the global texture container is researched.
        Textures in the global environment are not referenced upon
        return but are put into the current texture container if it
        is set. The global texture environment is either like a
        temporary storage for textures or a routine to dynamically
        create them and give to models.
    Note:
        GTA:SA does not use the global texture routine (m_findInstance).
    Binary offsets:
        (1.0 US): 0x007F3AC0
        (1.0 EU): 0x007F3B00
=========================================================*/
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

        // Actually, there is a missing texture handler; it is void/nullsub though
        RwSetError( &err );
        return NULL;
    }

    // Put the global environment texture into the current TXD container.
    if ( RwTexDictionary *txd = pRwInterface->m_textureManager.m_current )
    {
        tex->RemoveFromDictionary();
        tex->AddToDictionary( txd );
    }

    return tex;
}

/*=========================================================
    RwSetError

    Arguments:
        info - error information
    Purpose:
        Notifies the RenderWare system about a runtime error.
    Binary offsets:
        (1.0 US): 0x00808820
        (1.0 EU): 0x00808860
=========================================================*/
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

/*=========================================================
    RpLightCreate

    Arguments:
        type - type identifier of the new light
               see RpLightType enum (LIGHT_TYPE_* )
    Purpose:
        Creates a new RpLight plugin instance and registers
        it into the system. It assigns a light-type to it.
        This light-type may not be changed during the light's
        lifetime.
    Binary offsets:
        (1.0 US): 0x00752110
        (1.0 EU): 0x00752160
=========================================================*/
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

    // Register the RpLight into the RpLight plugin registry
    RwObjectRegister( (void*)0x008D62F8, light );
    return light;
}

/*=========================================================
    RpLight::SetColor

    Arguments:
        _color - new color to set the light to
    Purpose:
        Changes the light's color to another. If the color is
        brightness only, it sets privateFlags to 1; otherwise 0.
    Binary offsets:
        (1.0 US): 0x00751A90
        (1.0 EU): 0x00751AE0
=========================================================*/
void RpLight::SetColor( const RwColorFloat& _color )
{
    color = _color;
    
    // Check whether we are brightness only
    privateFlags = ( color.r == color.g && color.r == color.b );
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
static RpClump* _clumpCallback( RpClump *clump, void *data )
{
    return clump;
}

RpClump* RpClumpCreate( void )
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

    // Register the clump into the RpClump plugin registry
    RwObjectRegister( (void*)0x008D6264, clump );
    return clump;
}