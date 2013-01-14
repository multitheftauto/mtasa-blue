/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIScrollPane_Impl.cpp
*  PURPOSE:     Scroll pane widget class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define CGUISCROLLPANE_NAME "CGUI/ScrollablePane"

CGUIScrollPane_Impl::CGUIScrollPane_Impl ( CGUI_Impl* pGUI, CGUIElement* pParent )
{
    m_pManager = pGUI;

    // Initialize
    m_pGUI = pGUI;

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUISCROLLPANE_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );
    m_pWindow->setRect ( CEGUI::Relative, CEGUI::Rect (0.9f, 0.9f, 0.9f, 0.9f) );

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );
    AddEvents ();

    // If a parent is specified, add it to it's children list, if not, add it as a child to the m_pManager
    if ( pParent )
    {
        SetParent ( pParent );
    }
    else
    {
        pGUI->AddChild ( this );
        SetParent ( NULL );
    }
    SetHorizontalScrollStepSize ( 100.0f );
    SetVerticalScrollStepSize ( 100.0f );
}

CGUIScrollPane_Impl::~CGUIScrollPane_Impl ( void )
{
    DestroyElement ();
}


void CGUIScrollPane_Impl::SetHorizontalScrollBar ( bool bEnabled )
{
    reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> setShowHorzScrollbar ( bEnabled );
}


void CGUIScrollPane_Impl::SetVerticalScrollBar ( bool bEnabled )
{
    reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> setShowVertScrollbar ( bEnabled );
}


void CGUIScrollPane_Impl::SetHorizontalScrollPosition ( float fPosition )
{
    try {
        CEGUI::ScrollablePane* pScrollPane = reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow );
        float fFullWidth = pScrollPane -> getContentPaneArea ().getWidth ();
        float fViewWidth = pScrollPane -> getAbsoluteWidth ();

        pScrollPane -> setHorizontalScrollPosition ( fPosition * ( ( fFullWidth - fViewWidth ) / fFullWidth ) );
    } catch ( CEGUI::Exception ) {}
}


void CGUIScrollPane_Impl::SetVerticalScrollPosition ( float fPosition )
{
    try {
        CEGUI::ScrollablePane* pScrollPane = reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow );
        float fFullHeight = pScrollPane -> getContentPaneArea ().getHeight ();
        float fViewHeight = pScrollPane -> getAbsoluteHeight ();

        pScrollPane -> setVerticalScrollPosition ( fPosition * ( ( fFullHeight - fViewHeight ) / fFullHeight ) );
    } catch ( CEGUI::Exception ) {}
}


float CGUIScrollPane_Impl::GetHorizontalScrollPosition ( void )
{
    try {
        CEGUI::ScrollablePane* pScrollPane = reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow );

        float fFullWidth = pScrollPane -> getContentPaneArea ().getWidth ();
        float fViewWidth = pScrollPane -> getAbsoluteWidth ();

        // Prevent division by zero
        if ( fFullWidth == fViewWidth )
            return 0.0f;
        
        return ( pScrollPane -> getHorizontalScrollPosition () / ( ( fFullWidth - fViewWidth ) / fFullWidth ) );

    } catch ( CEGUI::Exception ) { return 0; }
}


float CGUIScrollPane_Impl::GetVerticalScrollPosition ( void )
{
    try {
        CEGUI::ScrollablePane* pScrollPane = reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow );

        float fFullHeight = pScrollPane -> getContentPaneArea ().getHeight ();
        float fViewHeight = pScrollPane -> getAbsoluteHeight ();

        // Prevent division by zero
        if ( fFullHeight == fViewHeight )
            return 0.0f;
        
        return ( pScrollPane -> getVerticalScrollPosition () / ( ( fFullHeight - fViewHeight ) / fFullHeight ) );
    } catch ( CEGUI::Exception ) { return 0; }
}


void CGUIScrollPane_Impl::SetHorizontalScrollStepSize ( float fPosition )
{
    try {
        reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> setHorizontalStepSize ( fPosition );
    } catch ( CEGUI::Exception ) {}
}


void CGUIScrollPane_Impl::SetVerticalScrollStepSize ( float fPosition )
{
    try {
        reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> setVerticalStepSize ( fPosition );
    } catch ( CEGUI::Exception ) {}
}


float CGUIScrollPane_Impl::GetHorizontalScrollStepSize ( void )
{
    try {
        return reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> getHorizontalStepSize ();
    } catch ( CEGUI::Exception ) { return 0; }
}


float CGUIScrollPane_Impl::GetVerticalScrollStepSize ( void )
{
    try {
        return reinterpret_cast < CEGUI::ScrollablePane* > ( m_pWindow ) -> getVerticalStepSize ();
    } catch ( CEGUI::Exception ) { return 0; }
}
