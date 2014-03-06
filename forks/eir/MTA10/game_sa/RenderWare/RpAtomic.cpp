/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpAtomic.cpp
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
    RpAtomic::GetWorldBoundingSphere

    Purpose:
        Calculates and returns a bounding sphere in world space
        which entirely contains the geometry.
    Binary offsets:
        (1.0 US): 0x00749330
        (1.0 EU): 0x00749380
=========================================================*/
const RwSphere& RpAtomic::GetWorldBoundingSphere( void )
{
    return RpAtomicGetWorldBoundingSphere( this );
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
        clump - model to register this atomic at
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
void RpAtomic::AddToClump( RpClump *clump )
{
    RemoveFromClump();

    this->clump = clump;

    LIST_INSERT( clump->atomics.root, atomics );
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
    }
    else
    {
        renderCallback = callback;
    }
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
    if ( componentFlags & 0x2000 )
        return;

    for ( unsigned int n = 0; n < geometry->linkedMateria->count; n++ )
    {
        mats.Add( geometry->linkedMateria->Get(n)->material );
    }
}