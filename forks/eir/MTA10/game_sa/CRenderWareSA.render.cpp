/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.render.cpp
*  PURPOSE:     RenderWare rendering API routines
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include <StdInc.h>

/*=========================================================
    RwFrameSyncObjects

    Arguments:
        frame - RenderWare frame object (matrix in 3d space)
    Purpose:
        Synchronizes the objects of given frame. This is made
        so they are properly positioned and rotated.
=========================================================*/
__forceinline void RwFrameSyncObjects( RwFrame *frame )
{
    // Call synchronization callbacks for all objects.
    LIST_FOREACH_BEGIN( RwObjectFrame, frame->objects.root, lFrame )
        item->callback( item );
    LIST_FOREACH_END
}

/*=========================================================
    RwFrameSyncChildren

    Arguments:
        child - child frame of another
    Purpose:
        Synchronizes child frames by resolving their local
        transformation matrix and updating their objects.
        This function is called recursively on the children of
        the child frames.
    Binary offsets:
        (1.0 US): 0x00809700
        (1.0 EU): 0x00809740
=========================================================*/
__forceinline void RwFrameSyncChildren( RwFrame *child, unsigned int parentFlags )
{
    for ( ; child != NULL; child = child->next )
    {
        unsigned int flags = child->privateFlags | parentFlags;

        if ( flags & RW_FRAME_UPDATEMATRIX )
            child->modelling.Multiply( child->parent->ltm, child->ltm );
            //((void (__cdecl*)( RwMatrix&, const RwMatrix&, const RwMatrix& ))0x007F18F0)( child->m_ltm, child->m_modelling, child->m_parent->m_ltm );

        RwFrameSyncObjects( child );

        child->privateFlags &= ~0x0C;

        RwFrameSyncChildren( child->child, flags );
    }
}

/*=========================================================
    RwFrameSyncChildrenOnlyObjects

    Arguments:
        child - child frame of another
    Purpose:
        Recursively updates the objects of all children frames.
        It is assumed that the local transformation matrix is
        up-to-date.
    Binary offsets:
        (1.0 US): 0x00809780
        (1.0 EU): 0x008097C0
=========================================================*/
__forceinline void RwFrameSyncChildrenOnlyObjects( RwFrame *child )
{
    for ( ; child != NULL; child = child->next )
    {
        RwFrameSyncObjects( child );

        child->privateFlags &= ~0x08;

        RwFrameSyncChildrenOnlyObjects( child->child );
    }
}

/*=========================================================
    RwFrameSyncDirtyList

    Arguments:
        frameRoot - queue of frames to update
    Purpose:
        Loops through frameRoot and updates all frames in
        it. Once done, frameRoot list is cleared.
=========================================================*/
__forceinline void RwFrameSyncDirtyList( RwList <RwFrame>& frameRoot )
{
    LIST_FOREACH_BEGIN( RwFrame, frameRoot.root, nodeRoot )
        unsigned int flags = item->privateFlags;

        if ( flags & RW_FRAME_DIRTY )
        {
            unsigned int dirtFlags = flags & RW_FRAME_UPDATEMATRIX;

            if ( dirtFlags )
                item->ltm = item->modelling;

            RwFrameSyncObjects( item );

            RwFrameSyncChildren( item->child, dirtFlags );
        }
        else
        {
            RwFrameSyncObjects( item );

            RwFrameSyncChildrenOnlyObjects( item->child );
        }

        item->privateFlags = flags & ~0x0F;
    LIST_FOREACH_END

    // We have no more dirty frames.
    LIST_CLEAR( frameRoot.root );
}

/*=========================================================
    RwFrameSyncDirty

    Arguments:
        frameRoot - queue of frames to update
    Purpose:
        Processes all known frame update queues and synchronizes
        the frames inside of them. This will properly position
        and rotate them in 3d space. There is a public GTA:SA
        update queue and a MTA one.
    Binary offsets:
        (1.0 US): 0x00809550
        (1.0 EU): 0x00809590
=========================================================*/
static RwList <RwFrame> mtaFrameDirtyList;

static unsigned int __cdecl RwFrameSyncDirty( void )
{
    RwFrameSyncDirtyList( RenderWare::GetInterface()->m_nodeRoot );
    RwFrameSyncDirtyList( mtaFrameDirtyList );  // we keep our own list for security reasons.
    return true;
}

/*=========================================================
    RwFrameGetDirtyList_MTA

    Purpose:
        Returns the MTA managed frame dirty list which is
        used to (safely) update RenderWare frames. It is
        created since it is uncertain whether GTA:SA screws
        with the RenderWare dirty list.
=========================================================*/
RwList <RwFrame>& RwFrameGetDirtyList_MTA( void )
{
    return mtaFrameDirtyList;
}

void RenderWareRender_Init( void )
{
    // Keep our own forced list (as GTA:SA might play around with RenderWare internal data)
    LIST_CLEAR( mtaFrameDirtyList.root );
    
    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        HookInstall( 0x00809590, (DWORD)RwFrameSyncDirty, 5 );
        break;
    case VERSION_US_10:
        HookInstall( 0x00809550, (DWORD)RwFrameSyncDirty, 5 );
        break;
    }
}

void RenderWareRender_Shutdown( void )
{
}