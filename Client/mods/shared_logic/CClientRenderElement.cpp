/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRenderElement.cpp
*  PURPOSE:     Client module's version of something renderable
*  DEVELOPERS:  idiot
*
*****************************************************************************/

#include <StdInc.h>


////////////////////////////////////////////////////////////////
//
// CClientRenderElement::CClientRenderElement
//
//
//
////////////////////////////////////////////////////////////////
CClientRenderElement::CClientRenderElement ( CClientManager* pManager, ElementID ID ) : ClassInit ( this ), CClientEntity ( ID )
{
    m_pRenderElementManager = pManager->GetRenderElementManager ();
    bDoneUnlink = false;
    m_pRenderItem = NULL;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElement::~CClientRenderElement
//
//
//
////////////////////////////////////////////////////////////////
CClientRenderElement::~CClientRenderElement ( void )
{
    Unlink ();
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElement::Unlink
//
// Remove from manager lists
//
////////////////////////////////////////////////////////////////
void CClientRenderElement::Unlink ( void )
{
    if ( !bDoneUnlink )
    {
        bDoneUnlink = true;
        m_pRenderElementManager->Remove ( this );
    }
}
