/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.cpp
*  PURPOSE:
*  DEVELOPERS:  xidiot
*
*****************************************************************************/

#include "StdInc.h"


////////////////////////////////////////////////////////////////
//
// CRenderItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItem::PostConstruct ( CRenderItemManager* pManager )
{
    m_pManager = pManager;
    m_pDevice = pManager->m_pDevice;
    m_iRefCount = 1;
    m_pManager->NotifyContructRenderItem ( this );
}


////////////////////////////////////////////////////////////////
//
// CRenderItem::~CRenderItem
//
//
//
////////////////////////////////////////////////////////////////
CRenderItem::~CRenderItem ( void )
{
    assert ( m_iRefCount == 0 );
}


////////////////////////////////////////////////////////////////
//
// CRenderItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItem::PreDestruct ( void )
{
    assert ( m_iRefCount == 0 );
    m_pManager->NotifyDestructRenderItem ( this );
}


////////////////////////////////////////////////////////////////
//
// CRenderItem::Release
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItem::Release ( void )
{
    assert ( m_iRefCount > 0 );
    if ( --m_iRefCount > 0 )
        return;

    PreDestruct ();
    delete this;
}


////////////////////////////////////////////////////////////////
//
// CRenderItem::AddRef
//
//
//
////////////////////////////////////////////////////////////////
void CRenderItem::AddRef ( void )
{
    assert ( m_iRefCount > 0 );
    ++m_iRefCount;
}
