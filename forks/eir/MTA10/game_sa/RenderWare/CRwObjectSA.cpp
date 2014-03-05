/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/CRwObjectSA.cpp
*  PURPOSE:     RenderWare RwObject export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CRwObjectSA::CRwObjectSA( RwObject *obj )
{
    m_object = obj;

    // Parent frames have to be assigned during runtime
    m_frame = NULL;
}

CRwObjectSA::~CRwObjectSA( void )
{
    if ( !IsFrameExtension() )
        SetFrame( NULL );
}

void CRwObjectSA::SetFrame( CRwFrame *frame )
{
    CRwFrameSA *parent = dynamic_cast <CRwFrameSA*> ( frame );

    if ( m_frame == parent )
        return;

    // If we are a frame extension and were at a previous frame, unregister us
    if ( m_frame && IsFrameExtension() )
    {
        m_frame->m_objects.remove( this );

        ((RwObjectFrame*)GetObject())->RemoveFromFrame();
    }

    m_frame = parent;

    if ( !m_frame )
        return;

    // If we are a frame extension, register us at the new frame
    if ( IsFrameExtension() )
    {
        ((RwObjectFrame*)GetObject())->AddToFrame( m_frame->GetObject() );

        m_frame->m_objects.push_back( this );
    }
}

CRwFrame* CRwObjectSA::GetFrame( void )
{
    return m_frame;
}

const CRwFrame* CRwObjectSA::GetFrame( void ) const
{
    return m_frame;
}