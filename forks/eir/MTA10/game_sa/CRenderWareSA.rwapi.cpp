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
*   The definitions for each RenderWare struct are in their own files now.
*   You can locate them inside the "RenderWare/" folder.
*
*****************************************************************************/

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

    if ( frame )
    {
        LIST_INSERT( frame->objects.root, lFrame );

        frame->Update();
    }
}

RpAtomic* __cdecl RpAtomicSetFrame( RpAtomic *atomic, RwFrame *frame )
{
    atomic->AddToFrame( frame );

    atomic->privateFlags |= 1;
    return atomic;
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

RwStaticGeometry::RwStaticGeometry( void )
{
    count = 0;
    link = NULL;

    unknown2 = NULL;
    unknown = 0;
    unknown3 = 0;
}

/*=========================================================
    RwStaticGeometry::AllocateLink (GTA:SA extension)

    Arguments:
        count - number of links to allocate
    Purpose:
        Allocates a number of links to this static geometry.
        Previous links are discarded.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004CF140
=========================================================*/
RwRenderLink* RwStaticGeometry::AllocateLink( unsigned int count )
{
    if ( link )
        RwFreeAligned( link );

    this->count = count;

    return link = (RwRenderLink*)RwAllocAligned( (((count * sizeof(RwRenderLink) - 1) >> 6 ) + 1) << 6, 0x40 );
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
    RwInterface *rwInterface = RenderWare::GetInterface();

    RwTexture *tex = rwInterface->m_textureManager.findInstanceRef( name );

    // The global store will reference textures
    if ( tex )
    {
        tex->refs++;
        return tex;
    }

    if ( !( tex = rwInterface->m_textureManager.findInstance( name, secName ) ) )
    {
        // If we have not found anything, we tell the system about an error
        RwError err;
        err.err1 = 0x01;
        err.err2 = 0x16;

        // Actually, there is a missing texture handler; it is void though
        RwSetError( &err );
        return NULL;
    }

    if ( RwTexDictionary *txd = RwTexDictionaryGetCurrent() )
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
    RwInterface *rwInterface = RenderWare::GetInterface();

    if ( rwInterface->m_errorInfo.err1 )
        return info;

    if ( rwInterface->m_errorInfo.err2 != 0x80000000 )
        return info;

    if ( info->err1 & 0x80000000 )
        rwInterface->m_errorInfo.err1 = 0;
    else
        rwInterface->m_errorInfo.err1 = info->err1;

    rwInterface->m_errorInfo.err2 = info->err2;
    return info;
}