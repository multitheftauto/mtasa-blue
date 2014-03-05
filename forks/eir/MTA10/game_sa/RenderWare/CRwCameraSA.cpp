/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/CRwCameraSA.cpp
*  PURPOSE:     RenderWare RwCamera export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include <gamesa_renderware.h>

CRwCameraSA::CRwCameraSA( RwCamera *cam ) : CRwObjectSA( cam )
{
    m_isRendering = false;
    m_model = NULL;
}

CRwCameraSA::~CRwCameraSA( void )
{
    // If we are still rendering, unset
    if ( IsRendering() )
        EndUpdate();

    // Remove from clump
    RemoveFromModel();

    // Also from frame
    SetFrame( NULL );

    // Kill the previously created targets
    DestroyBuffers();

    RwCameraDestroy( GetObject() );
}

void CRwCameraSA::AddToModel( CModel *model )
{
    RemoveFromModel();

    m_model = dynamic_cast <CModelSA*> ( model );

    if ( !m_model )
        return;

    m_model->m_cameras.push_back( this );
    GetObject()->AddToClump( m_model->GetObject() );
}

CModel* CRwCameraSA::GetModel( void )
{
    return m_model;
}

void CRwCameraSA::RemoveFromModel( void )
{
    if ( !m_model )
        return;

    GetObject()->RemoveFromClump();

    m_model->m_cameras.remove( this );
    m_model = NULL;
}

void CRwCameraSA::DestroyBuffers( void )
{
    RwCamera *cam = GetObject();

    if ( cam->rendertarget )
        RwRasterDestroy( cam->rendertarget );

    if ( cam->bufferDepth )
        RwRasterDestroy( cam->bufferDepth );
}

void CRwCameraSA::SetRenderSize( int width, int height )
{
    DestroyBuffers();

    GetObject()->rendertarget = RwRasterCreate( width, height, 0, RASTER_RENDERTARGET );
    GetObject()->bufferDepth = RwRasterCreate( width, height, 0, RASTER_DEPTHBUFFER );
}

void CRwCameraSA::GetRenderSize( int& width, int& height ) const
{
    const RwCamera *cam = GetObject();

    width = cam->rendertarget->width;
    height = cam->rendertarget->height;
}

bool CRwCameraSA::BeginUpdate( void )
{
    if ( IsRendering() )
        return true;

    if ( RenderWare::GetInterface()->m_renderCam || !m_frame )
        return false;

    GetObject()->BeginUpdate();

    m_isRendering = true;
    return true;
}

void CRwCameraSA::EndUpdate( void )
{
    if ( !IsRendering() )
        return;

    GetObject()->EndUpdate();

    m_isRendering = false;
}