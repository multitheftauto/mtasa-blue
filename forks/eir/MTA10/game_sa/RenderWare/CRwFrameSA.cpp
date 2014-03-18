/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/CRwFrameSA.cpp
*  PURPOSE:     RenderWare RwFrame export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "../gamesa_renderware.h"

RwObject* CRwFrameSA::RwFrameObjectAssign( RwObject *obj, CRwFrameSA *parent )
{
    CRwObjectSA *inst;

    switch( obj->type )
    {
    case RW_ATOMIC:
        inst = new CRpAtomicSA( (RpAtomic*)obj );
        break;
    case RW_LIGHT:
        inst = new CRpLightSA( (RpLight*)obj );
        break;
    case RW_CAMERA:
        inst = new CRwCameraSA( (RwCamera*)obj );
        break;
    }
    parent->m_objects.push_back( inst );

    // Add a back-reference
    inst->m_frame = parent;
    return obj;
}

static RwFrame* RwFrameChildAssign( RwFrame *child, CRwFrameSA *parent )
{
    CRwFrameSA *frame = new CRwFrameSA( child );

    // Link it into our hierarchy
    frame->m_frame = parent;

    parent->m_children.push_back( frame );
    return child;
}

CRwFrameSA::CRwFrameSA( RwFrame *obj ) : CRwObjectSA( obj )
{
    // Load all frames into the hierarchy
    obj->ForAllChildren( RwFrameChildAssign, this );
    obj->ForAllObjects( RwFrameObjectAssign, this );
}

CRwFrameSA::~CRwFrameSA( void )
{
    // Destroy all children and objects
    while ( !m_objects.empty() )
        delete m_objects.front();

    while ( !m_children.empty() )
        delete m_children.front();

    // Let us not forget to unlink ourselves :3
    Unlink();

    RwFrameDestroy( GetObject() );
}

unsigned int CRwFrameSA::GetHash( void ) const
{
    return pGame->GetKeyGen()->GetKey( GetObject()->szName );
}

void CRwFrameSA::Link( CRwFrame *child )
{
    CRwFrameSA *frame = dynamic_cast <CRwFrameSA*> ( child );

    if ( frame->m_frame == this )
        return;

    // Unlink previous connection
    frame->Unlink();

    // Do the internal link
    GetObject()->Link( frame->GetObject() );

    // Specify the new root
    frame->m_frame = this;
    m_children.insert( m_children.begin(), frame );
}

void CRwFrameSA::Unlink( void )
{
    if ( !m_frame )
        return;

    GetObject()->Unlink();

    m_frame->m_children.remove( this );
    m_frame = NULL;
}