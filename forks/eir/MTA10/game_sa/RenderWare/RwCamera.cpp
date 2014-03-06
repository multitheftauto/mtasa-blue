/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwCamera.cpp
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
    RwCameraCreate

    Purpose:
        Returns a new RenderWare camera plugin instance.
    Binary offsets:
        (1.0 US): 0x007EE4F0
        (1.0 EU): 0x007EE530
=========================================================*/
RwCamera* RwCameraCreate( void )
{
    RwInterface *rwInterface = RenderWare::GetInterface();

    RwCamera *cam = (RwCamera*)rwInterface->m_allocStruct( rwInterface->m_cameraInfo, 0x30005 );

    if ( !cam )
        return NULL;

    cam->type = RW_CAMERA;
    cam->subtype = 0;
    cam->flags = 0;
    cam->privateFlags = 0;
    cam->parent = NULL;

    cam->callback = (RwObjectFrame::syncCallback_t)0x007EE5A0;
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
void RwCamera::AddToClump( RpClump *clump )
{
    // Bugfix: remove from previous clump
    RemoveFromClump();

    LIST_INSERT( clump->cameras.root, clumpCameras );

    this->clump = clump;
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