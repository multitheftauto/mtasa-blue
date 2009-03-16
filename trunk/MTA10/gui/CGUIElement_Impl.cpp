/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIElement_Impl.cpp
*  PURPOSE:     Element (widget) base class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// Define no-drawing zones, a.k.a. the inside borders in the FrameWindow of BlueLook in pixels
// If something is drawn inside of these areas, the theme border is drawn on top of it
#define CGUI_NODRAW_LEFT 9.0f
#define CGUI_NODRAW_RIGHT 9.0f
#define CGUI_NODRAW_TOP 9.0f
#define CGUI_NODRAW_BOTTOM 9.0f

CGUIElement_Impl::CGUIElement_Impl ( void )
{
    m_pOnMoved = NULL;
    m_pOnSized = NULL;
	m_pOnClick = NULL;
	m_pOnMouseEnter = NULL;
	m_pOnMouseLeave = NULL;
}


void CGUIElement_Impl::SetVisible ( bool bVisible )
{
    m_pWindow->setVisible ( bVisible );
}


bool CGUIElement_Impl::IsVisible ( void )
{
    return m_pWindow->isVisible ();
}


void CGUIElement_Impl::SetEnabled ( bool bEnabled )
{
    m_pWindow->setEnabled ( bEnabled );
    //m_pWindow->setZOrderingEnabled ( bEnabled );
}


bool CGUIElement_Impl::IsEnabled ( void )
{
    return !m_pWindow->isDisabled ();
}


void CGUIElement_Impl::SetZOrderingEnabled ( bool bZOrderingEnabled )
{
    m_pWindow->setZOrderingEnabled ( bZOrderingEnabled );
}


bool CGUIElement_Impl::IsZOrderingEnabled  ( void )
{
    return m_pWindow->isZOrderingEnabled ();
}


void CGUIElement_Impl::BringToFront ( void )
{
    m_pWindow->moveToFront ();
}


void CGUIElement_Impl::MoveToBack ( void )
{
    m_pWindow->moveToBack ();
}


void CGUIElement_Impl::SetPosition ( const CVector2D& Position, bool bRelative )
{
    CEGUI::Point Temp = CEGUI::Point ( Position.fX, Position.fY );
	
	if ( bRelative )
		m_pWindow->setPosition ( CEGUI::Relative, Temp );
	else
		m_pWindow->setPosition ( CEGUI::Absolute, Temp );

	CorrectEdges ();
}


CVector2D CGUIElement_Impl::GetPosition ( bool bRelative )
{
    CEGUI::Point CEGUITemp;

	if ( bRelative )
		CEGUITemp = m_pWindow->getPosition ( CEGUI::Relative );
	else
		CEGUITemp = m_pWindow->getPosition ( CEGUI::Absolute );

    return CVector2D ( CEGUITemp.d_x, CEGUITemp.d_y );
}


void CGUIElement_Impl::GetPosition ( CVector2D& vecPosition, bool bRelative )
{
	CEGUI::MetricsMode type = CEGUI::Absolute;

	if ( bRelative )
		type = CEGUI::Relative;

	CEGUI::Point& Temp = m_pWindow->getPosition ( type );

    vecPosition.fX = Temp.d_x;
    vecPosition.fY = Temp.d_y;
}


void CGUIElement_Impl::SetWidth ( float fX, bool bRelative )
{
	if ( bRelative )
		m_pWindow->setWidth ( CEGUI::Relative, fX );
	else
		m_pWindow->setWidth ( CEGUI::Absolute, fX );
}


void CGUIElement_Impl::SetHeight ( float fY, bool bRelative )
{
	if ( bRelative )
		m_pWindow->setHeight ( CEGUI::Relative, fY );
	else
		m_pWindow->setHeight ( CEGUI::Absolute, fY );
}


void CGUIElement_Impl::SetSize ( const CVector2D& vecSize, bool bRelative )
{
	if ( bRelative )
		m_pWindow->setSize ( CEGUI::Relative, CEGUI::Size ( vecSize.fX, vecSize.fY ) );
	else
		m_pWindow->setSize ( CEGUI::Absolute, CEGUI::Size ( vecSize.fX, vecSize.fY ) );

	CorrectEdges ();
}


CVector2D CGUIElement_Impl::GetSize ( bool bRelative )
{
	CEGUI::Size TempSize;

	if ( bRelative )
		TempSize = m_pWindow->getRelativeSize ();
	else
		TempSize = m_pWindow->getAbsoluteSize ();

	return CVector2D ( TempSize.d_width, TempSize.d_height );
}


void CGUIElement_Impl::GetSize ( CVector2D& vecSize, bool bRelative )
{
	CEGUI::Size TempSize;

	if ( bRelative )
		TempSize = m_pWindow->getRelativeSize ();
	else
		TempSize = m_pWindow->getAbsoluteSize ();

    vecSize.fX = TempSize.d_width;
    vecSize.fY = TempSize.d_height;
}


void CGUIElement_Impl::SetMinimumSize ( const CVector2D& vecSize )
{
	m_pWindow->setMetricsMode ( CEGUI::Absolute );
	m_pWindow->setMinimumSize ( CEGUI::Size ( vecSize.fX, vecSize.fY ) );
}


CVector2D CGUIElement_Impl::GetMinimumSize ( void )
{
    const CEGUI::Size& TempSize = m_pWindow->getMinimumSize ();
    return CVector2D ( TempSize.d_width, TempSize.d_height );
}


void CGUIElement_Impl::GetMinimumSize ( CVector2D& vecSize )
{
    const CEGUI::Size& Temp = m_pWindow->getMinimumSize ();
    vecSize.fX = Temp.d_width;
    vecSize.fY = Temp.d_height;
}


void CGUIElement_Impl::SetMaximumSize ( const CVector2D& vecSize )
{
    m_pWindow->setMaximumSize ( CEGUI::Size ( vecSize.fX, vecSize.fY ) );
}


CVector2D CGUIElement_Impl::GetMaximumSize ( void )
{
    const CEGUI::Size& TempSize = m_pWindow->getMaximumSize ();
    return CVector2D ( TempSize.d_width, TempSize.d_height );
}


void CGUIElement_Impl::GetMaximumSize ( CVector2D& vecSize )
{
    const CEGUI::Size& Temp = m_pWindow->getSize ();
    vecSize.fX = Temp.d_width;
    vecSize.fY = Temp.d_height;
}


void CGUIElement_Impl::SetText ( const char *szText )
{
	CEGUI::String strText;

	if ( szText ) strText.assign ( szText );
    m_pWindow->setText ( strText );
}


std::string CGUIElement_Impl::GetText ( void )
{
    return m_pWindow->getText ().c_str ();
}


void CGUIElement_Impl::SetAlpha ( float fAlpha )
{
    m_pWindow->setAlpha ( fAlpha );
}


float CGUIElement_Impl::GetAlpha ( void )
{
    return m_pWindow->getAlpha ();
}


void CGUIElement_Impl::SetInheritsAlpha ( bool bInheritsAlpha )
{
    m_pWindow->setInheritsAlpha ( bInheritsAlpha );
}


bool CGUIElement_Impl::GetInheritsAlpha ( void )
{
    return m_pWindow->inheritsAlpha ();
}


void CGUIElement_Impl::Activate ( void )
{
    m_pWindow->activate ();
}


void CGUIElement_Impl::Deactivate ( void )
{
    m_pWindow->deactivate ();
}


bool CGUIElement_Impl::IsActive ( void )
{
    return m_pWindow->isActive ();
}


void CGUIElement_Impl::SetAlwaysOnTop ( bool bAlwaysOnTop )
{
    m_pWindow->setAlwaysOnTop ( bAlwaysOnTop );
}


bool CGUIElement_Impl::IsAlwaysOnTop ( void )
{
    return m_pWindow->isAlwaysOnTop ();
}


CRect2D CGUIElement_Impl::AbsoluteToRelative ( const CRect2D& Rect )
{
    CEGUI::Rect TempRect = CEGUI::Rect ( Rect.fX1, Rect.fY1, Rect.fX2, Rect.fY2 );
    TempRect = m_pWindow->absoluteToRelative ( TempRect );
    return CRect2D ( TempRect.d_left, TempRect.d_top, TempRect.d_right, TempRect.d_bottom );
}


CVector2D CGUIElement_Impl::AbsoluteToRelative ( const CVector2D& Vector )
{
    CEGUI::Size TempSize = CEGUI::Size ( Vector.fX, Vector.fY );
    TempSize = m_pWindow->absoluteToRelative ( TempSize );
    return CVector2D ( TempSize.d_width, TempSize.d_height );
}


CRect2D CGUIElement_Impl:: RelativeToAbsolute ( const CRect2D& Rect )
{
    CEGUI::Rect TempRect = CEGUI::Rect ( Rect.fX1, Rect.fY1, Rect.fX2, Rect.fY2 );
    TempRect = m_pWindow->relativeToAbsolute ( TempRect );
    return CRect2D ( TempRect.d_left, TempRect.d_top, TempRect.d_right, TempRect.d_bottom );
}


CVector2D CGUIElement_Impl::RelativeToAbsolute ( const CVector2D& Vector )
{
    CEGUI::Size TempSize = CEGUI::Size ( Vector.fX, Vector.fY );
    TempSize = m_pWindow->relativeToAbsolute ( TempSize );
    return CVector2D ( TempSize.d_width, TempSize.d_height );
}


void CGUIElement_Impl::SetParent ( CGUIElement* pParent )
{
	if ( pParent )
		((CGUIElement_Impl*)pParent)->m_pWindow->addChildWindow ( m_pWindow );
	m_pParent = pParent;
}


CGUIElement* CGUIElement_Impl::GetParent ( void )
{
	return m_pParent;
}


CEGUI::Window* CGUIElement_Impl::GetWindow ( void )
{
    return m_pWindow;
}


void CGUIElement_Impl::CorrectEdges ( void )
{
	CEGUI::Point currentPoint = m_pWindow->getPosition( CEGUI::Absolute );
	CEGUI::Size currentSize = m_pWindow->getSize( CEGUI::Absolute );
	// Label turns out to be buggy
	if(m_pWindow->getType( ) == "CGUI/StaticText") return;

	if(m_pWindow->getParent( )->getType( ) == "CGUI/FrameWindow") {
		CEGUI::Size parentSize = m_pWindow->getParent( )->getSize( CEGUI::Absolute );
		if ( currentPoint.d_x < CGUI_NODRAW_LEFT )
			currentPoint.d_x += CGUI_NODRAW_LEFT - currentPoint.d_x;
		if ( currentPoint.d_y < CGUI_NODRAW_TOP )
			currentPoint.d_y += CGUI_NODRAW_TOP - currentPoint.d_x;
		if ( ( currentSize.d_height + currentPoint.d_y ) > ( parentSize.d_height - CGUI_NODRAW_BOTTOM ) )
			currentSize.d_height -= ( currentSize.d_height + currentPoint.d_y ) - ( parentSize.d_height - CGUI_NODRAW_BOTTOM );
		if ( ( currentSize.d_width + currentPoint.d_x ) > ( parentSize.d_width - CGUI_NODRAW_RIGHT ) )
			currentSize.d_width -= ( currentSize.d_width + currentPoint.d_x ) - ( parentSize.d_width - CGUI_NODRAW_RIGHT );
		m_pWindow->setPosition( CEGUI::Absolute, currentPoint );
		m_pWindow->setSize( CEGUI::Absolute, currentSize );
	}
}


bool CGUIElement_Impl::SetFont ( const char * szFontName )
{
	try {
		m_pWindow->setFont ( CEGUI::String ( szFontName ) );
		return true;
	} catch ( CEGUI::Exception e ) {
		return false;
	}
}


std::string CGUIElement_Impl::GetFont ( void )
{
	try
	{
		const CEGUI::Font *pFont = m_pWindow->getFont ();
		if ( pFont )
		{
			// Return the contname. std::string will copy it.
			CEGUI::String strFontName = pFont->getName ();
			return strFontName.c_str ();
		}
	}
	catch ( CEGUI::Exception e ) {}

	return "";
}

void CGUIElement_Impl::SetProperty ( const char *szProperty, const char *szValue )
{
	try {
		m_pWindow->setProperty ( szProperty, szValue );
	} catch ( CEGUI::Exception e ) {}
}


std::string CGUIElement_Impl::GetProperty ( const char *szProperty )
{
    CEGUI::String strValue;
	try
    {
		// Return the string. std::string will copy it
		strValue = m_pWindow->getProperty ( CEGUI::String ( szProperty ) );
	}
    catch ( CEGUI::Exception e )
    {}

    return strValue.c_str ();
}


void CGUIElement_Impl::FillProperties ( void )
{
	CEGUI::Window::PropertyIterator itPropertySet = ((CEGUI::PropertySet*)m_pWindow)->getIterator ();
	while ( !itPropertySet.isAtEnd () ) {
		CEGUI::String strKey = itPropertySet.getCurrentKey ();
		CEGUI::String strValue = m_pWindow->getProperty ( strKey );

		const char *szKey = strKey.c_str ();
		const char *szValue = strValue.c_str ();

		CGUIProperty* pProperty = new CGUIProperty;
		pProperty->szKey = new char[strlen ( szKey ) + 1];
		pProperty->szValue = new char[strlen ( szValue ) + 1];

		strcpy ( pProperty->szKey, szKey );
		strcpy ( pProperty->szValue, szValue );

		m_Properties.push_back ( pProperty );
		itPropertySet++;
	}
}

void CGUIElement_Impl::EmptyProperties ( void )
{
	if ( !m_Properties.empty() ) {
		CGUIPropertyIter iter = m_Properties.begin ();
		CGUIPropertyIter iterEnd = m_Properties.end ();
		for ( ; iter != iterEnd; iter++ ) {
			if (*iter) {
				delete (*iter)->szKey;
				delete (*iter)->szValue;
				delete (*iter);
			}
		}
	}
}

CGUIPropertyIter CGUIElement_Impl::GetPropertiesBegin ( void )
{
	try {
		// Fill the properties list, if it's still empty (on first call)
		if ( m_Properties.empty() )
			FillProperties ();

		// Return the list begin iterator
		return m_Properties.begin ();

	} catch ( CEGUI::Exception e ) {
		return *(CGUIPropertyIter*)NULL;
	}
}


CGUIPropertyIter CGUIElement_Impl::GetPropertiesEnd ( void )
{
	try {
		// Fill the properties list, if it's still empty (on first call)
		if ( m_Properties.empty() )
			FillProperties ();

		// Return the list begin iterator
		return m_Properties.end ();

	} catch ( CEGUI::Exception e ) {
		return *(CGUIPropertyIter*)NULL;
	}
}


void CGUIElement_Impl::SetMovedHandler ( GUI_CALLBACK Callback )
{
    m_pOnMoved = new GUI_CALLBACK ( Callback );
}


void CGUIElement_Impl::SetSizedHandler ( GUI_CALLBACK Callback )
{
    m_pOnSized = new GUI_CALLBACK ( Callback );
}


void CGUIElement_Impl::SetClickHandler ( GUI_CALLBACK Callback )
{
	m_pOnClick = new GUI_CALLBACK ( Callback );
}


void CGUIElement_Impl::SetMouseEnterHandler ( GUI_CALLBACK Callback )
{
	m_pOnMouseEnter = new GUI_CALLBACK ( Callback );
}


void CGUIElement_Impl::SetMouseLeaveHandler ( GUI_CALLBACK Callback )
{
	m_pOnMouseLeave = new GUI_CALLBACK ( Callback );
}


void CGUIElement_Impl::AddEvents ( void )
{
    // Register our events
    m_pWindow->subscribeEvent ( CEGUI::Window::EventMoved, CEGUI::Event::Subscriber ( &CGUIElement_Impl::Event_OnMoved, this ) );
    m_pWindow->subscribeEvent ( CEGUI::Window::EventSized, CEGUI::Event::Subscriber ( &CGUIElement_Impl::Event_OnSized, this ) );
	m_pWindow->subscribeEvent ( CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber ( &CGUIElement_Impl::Event_OnClick, this ) );
	m_pWindow->subscribeEvent ( CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber ( &CGUIElement_Impl::Event_OnMouseEnter, this ) );
	m_pWindow->subscribeEvent ( CEGUI::Window::EventMouseLeaves, CEGUI::Event::Subscriber ( &CGUIElement_Impl::Event_OnMouseLeave, this ) );
}


bool CGUIElement_Impl::Event_OnMoved ( const CEGUI::EventArgs& e )
{
	if ( m_pOnMoved )
		(*m_pOnMoved) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}


bool CGUIElement_Impl::Event_OnSized ( const CEGUI::EventArgs& e )
{
	if ( m_pOnSized )
		(*m_pOnSized) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}


bool CGUIElement_Impl::Event_OnClick ( const CEGUI::EventArgs& e )
{
	if ( m_pOnClick )
		(*m_pOnClick) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}


bool CGUIElement_Impl::Event_OnMouseEnter ( const CEGUI::EventArgs& e )
{
	if ( m_pOnMouseEnter )
		(*m_pOnMouseEnter) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}


bool CGUIElement_Impl::Event_OnMouseLeave ( const CEGUI::EventArgs& e )
{
	if ( m_pOnMouseLeave )
		(*m_pOnMouseLeave) ( reinterpret_cast < CGUIElement* > ( this ) );
    return true;
}


inline void CGUIElement_Impl::ForceRedraw ( void )
{
    m_pWindow->forceRedraw();
}
