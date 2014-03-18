/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwFrame.cpp
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
    RwFrame::SetModelling (MTA extension)

    Arguments:
        mat - matrix to apply to the frame
    Purpose:
        Update the modelling matrix of the frame. It will flag the
        frame for updating. The flag will be checked once the
        local transformation matrix (LTM) is retrieved. The LTM
        will then be recalculated.
        The modelling matrix is the local offset of the frame based
        on the absolute position of the parent frame.
    Note:
        This function is MTA centered because it uses UpdateMTA.
=========================================================*/
void RwFrame::SetModelling( const RwMatrix& mat )
{
    modelling = mat;

    // Update this frame
    UpdateMTA();
}

/*=========================================================
    RwFrame::SetPosition (MTA extension)

    Arguments:
        pos - vector for new frame position
    Purpose:
        Set the modelling position of the frame without changing
        the frame's rotation. It flags the frame to update the LTM
        once requested.
=========================================================*/
void RwFrame::SetPosition( const CVector& pos )
{
    modelling.vPos = pos;

    // Update this frame
    UpdateMTA();
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
        Children frames are not root frames. The child frame is
        then updated.
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
    frame->root->ThrowUpdate();     // make sure it is not inside the update queue anymore

    // We need to update the child
    frame->Update();
}

RwFrame* __cdecl RwFrameLink( RwFrame *frame, RwFrame *child )          { frame->Link( child ); return frame; }
RwFrame* __cdecl RwFrameAddChild( RwFrame *frame, RwFrame *child )      { RwFrameLink( frame, child ); return frame; }
/*=========================================================
    RwFrame::Unlink

    Purpose:
        Unparents this frame from any hierarchy. This frame will
        then be a root frame. It is then updated to set it at the
        correct position.
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

    // Update this frame, as it became independent
    Update();
}

RwFrame* __cdecl RwFrameRemoveChild( RwFrame *child )           { child->Unlink(); return child; }
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
void RwFrame::SetRootForHierarchy( RwFrame *root )
{
    this->root = root;

    RwFrame *child = this->child;

    while ( child )
    {
        child->SetRootForHierarchy( root );

        child = child->next;
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
        MTA does not need to count the main frame?
=========================================================*/
static RwFrame* RwFrameGetChildCount( RwFrame *child, unsigned int *count )
{
    child->ForAllChildren( RwFrameGetChildCount, count );

    (*count)++;
    return child;
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
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734900
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

static int RwFrameGetFreeByName( RwFrame *child, _rwFrameFindName *info )
{
    if ( child->hierarchyId || strcmp( child->szName, info->name ) != 0 )
        return child->ForAllChildren( RwFrameGetFreeByName, info );

    info->result = child;
    return false;
}

RwFrame* RwFrame::FindFreeChildByName( const char *name )
{
    _rwFrameFindName info;
    info.name = name;

    return ( ForAllChildren( RwFrameGetFreeByName, &info ) ) ? ( NULL ) : ( info.result );
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
static int RwFrameGetByName( RwFrame *child, _rwFrameFindName *info )
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

    return ( ForAllChildren( RwFrameGetByName, &info ) ) ? ( NULL ) : info.result;
}

// MTA extension that looks up case sensitively.
static int RwFrameGetByNameCaseSensitive( RwFrame *child, _rwFrameFindName *info )
{
    if ( strcmp( child->szName, info->name ) != 0 )
        return child->ForAllChildren( RwFrameGetByNameCaseSensitive, info );

    info->result = child;
    return false;
}

RwFrame* __cdecl RwFrameFindFrame( RwFrame *parent, const char *name )
{
    _rwFrameFindName info;
    info.name = name;

    return ( parent->ForAllChildren( RwFrameGetByNameCaseSensitive, &info ) ) ? ( NULL ) : ( info.result );
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

static int RwFrameGetByHierarchy( RwFrame *child, _rwFrameFindHierarchy *info )
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

/*=========================================================
    RwFrame::CloneRecursive

    Purpose:
        Returns a newly allocated frame which is the exact copy
        of this frame. That means that all it's objects and children
        frames have been cloned (needs confirmation). Thew new frame
        is forced into the update synchronization queue.
    Binary offsets:
        (1.0 US): 0x007F0050
        (1.0 EU): 0x007F0090
=========================================================*/
RwFrame* RwFrame::CloneRecursive( void ) const
{
    RwFrame *cloned = RwFrameCloneRecursive( this, NULL );

    if ( !cloned )
        return NULL;

    cloned->SetUpdating( false );
    cloned->Update();
    return cloned;
}

static int RwFrameGetAnimHierarchy( RwFrame *frame, RpAnimHierarchy **rslt )
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
    RwFrame::GetFirstObject (MTA extension)

    Arguments:
        type - type of the object you want to find
    Purpose:
        Returns the first object with the matching type.
=========================================================*/
RwObject* RwFrame::GetFirstObject( unsigned char type )
{
    LIST_FOREACH_BEGIN( RwObjectFrame, objects.root, lFrame )
        if ( item->type == type )
            return item;
    LIST_FOREACH_END

    return NULL;
}

/*=========================================================
    RwFrame::GetObjectByIndex (MTA extension)

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

static int RwObjectGetByIndex( RwObject *obj, _rwObjectByIndex *info )
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
    
    return ForAllObjects( RwObjectGetByIndex, &info ) ? NULL : info.rslt;
}

/*=========================================================
    RwFrame::CountObjectsByType (MTA extension)

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

static int RwFrameCountObjectsByType( RwObject *obj, _rwObjCntByType *info )
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
    RwFrameForAllObjects

    Arguments:
        frame - the frame to loop all objects of
        callback - function to call for every object found
        data - userdata pointer to pass to callback
    Purpose:
        Loops through all objects of the given frame and
        invokes the callback for each.
    Binary offsets:
        (1.0 US): 0x007F1200
        (1.0 EU): 0x007F1240
=========================================================*/
RwFrame* __cdecl RwFrameForAllObjects( RwFrame *frame, frameObjectIterator_t callback, void *data )
{
    frame->ForAllObjects <void*> ( callback, data );
    return frame;
}

RwFrame* __cdecl RwFrameForAllObjects( RwFrame *frame, void *callback, void *data )
{
    return RwFrameForAllObjects( frame, (frameObjectIterator_t)callback, data );
}

/*=========================================================
    RwFrame::GetLastObject (MTA extension)

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
static int RwFrameObjectGetVisibleLast( RwObject *obj, RwObject **dst )
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
    RwFrame::GetFirstAtomic (MTA extension)

    Purpose:
        Returns the first atomic type object in this frame.
    Note:
        This function was created as a possible bugfix for GTA:SA.
        When the engine handles frames, it automatically assumes
        that their objects are all atomics which they may not be
        (light, camera). This function fixes that assumption.
=========================================================*/
RpAtomic* RwFrame::GetFirstAtomic( void )
{
    return (RpAtomic*)GetFirstObject( RW_ATOMIC );
}

/*=========================================================
    RwFrame::SetAtomicComponentFlags

    Arguments:
        flags - visibility flags for all atomics
    Purpose:
        Applies the specified visibility flags for all atomics
        in this frame.
=========================================================*/
static int RwObjectAtomicSetComponentFlags( RpAtomic *atomic, unsigned short flags )
{
    atomic->componentFlags |= flags;
    return true;
}

void RwFrame::SetAtomicComponentFlags( unsigned short flags )
{
    ForAllAtomics( RwObjectAtomicSetComponentFlags, flags );
}

static bool RwFrameAtomicBaseRoot( RpAtomic *atomic, RwFrame *root )
{
    RpAtomicSetFrame( atomic, root );
    return true;
}

/*=========================================================
    RwFrame::FindComponentAtomics

    Arguments:
        primary - atomic of primary visibility type
        secondary - atomic of secondary visibility type
    Purpose:
        Returns the last atomics which are either marked as 'ok'
        or 'dam' versions. Otherwise, the values are left
        untouched [if you are unsure that the frame has the
        atomics, initialize the values to NULL yourself].
=========================================================*/
struct _rwFrameComponentAtomics
{
    RpAtomic **primary;
    RpAtomic **secondary;
};

static int RwFrameAtomicFindComponents( RpAtomic *atomic, _rwFrameComponentAtomics *info )
{
    if ( atomic->componentFlags & 0x01 )
    {
        if ( info->primary )
            *info->primary = atomic;
    }
    else if ( atomic->componentFlags & 0x02 )
    {
        if ( info->secondary )
            *info->secondary = atomic;
    }

    return true;
}

void RwFrame::FindComponentAtomics( RpAtomic **okay, RpAtomic **damaged )
{
    _rwFrameComponentAtomics info;

    info.primary = okay;
    info.secondary = damaged;

    ForAllAtomics( RwFrameAtomicFindComponents, &info );
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
    RpAnimHierarchy *anim;

    if ( this->anim )
        return this->anim;

    // We want false, since it has to interrupt == found
    if ( ForAllChildren( RwFrameGetAnimHierarchy, &anim ) )
        return NULL;

    return anim;
}

/*=========================================================
    RwFrame::Update

    Purpose:
        Marks this frame to be updated in the next camera
        focus call (RwCameraBeginUpdate). Once it is updated,
        the flags are unset and it is removed from the list.
        There is a MTA and a GTA:SA dirty list.
    Binary offsets:
        (1.0 US): 0x007F0910
        (1.0 EU): 0x007F0950
    Note:
        This function has been inlined into other RenderWare
        functions. Look closely at the pattern to find out
        where!
        _Update may only be used on root frames, as their
        whole hierarchy is being updated during RwFrameSyncDirty.
        RW_FRAME_UPDATEMATRIX tells the updater that the LTM
        matrix should be updated. Only the modelling matrix
        should be changed by code, LTM is read-only.
=========================================================*/
static void RwFrameCheckUpdateNode( void )
{
    LIST_FOREACH_BEGIN( RwFrame, RenderWare::GetInterface()->m_nodeRoot.root, nodeRoot )
        if ( !LIST_ISVALID( *iter ) )
            __asm int 3
    LIST_FOREACH_END
}

void RwFrame::_Update( RwList <RwFrame>& list )
{
    unsigned char flagIntegrity = privateFlags;

    if ( !( flagIntegrity & RW_FRAME_UPDATEFLAG ) )
    {
        // Add it to the internal list
        LIST_INSERT( list.root, nodeRoot );
    }

    privateFlags = ( flagIntegrity | RW_FRAME_UPDATEFLAG );
}

void RwFrame::Update( void )
{
    root->_Update( RenderWare::GetInterface()->m_nodeRoot );
    privateFlags |= RW_FRAME_UPDATEMATRIX | 8;
}

// MTA extension: update this node on a seperate queue.
void RwFrame::UpdateMTA( void )
{
    root->_Update( RwFrameGetDirtyList_MTA() );
    privateFlags |= RW_FRAME_UPDATEMATRIX | 8;
}

/*=========================================================
    RwFrame::ThrowUpdate

    Purpose:
        Unregisters this frame from the update queue.
        Synchronization attempts will be halted no matter on
        what queue the frame resides in.
    Note:
        This function has been heavily inlined and does only
        occur in RwFrame::Link.
=========================================================*/
void RwFrame::ThrowUpdate( void )
{
    if ( !IsWaitingForUpdate() )
        return;

    LIST_REMOVE( nodeRoot );

    SetUpdating( false );
}