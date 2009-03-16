/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGUIElement.cpp
*  PURPOSE:     GUI wrapper entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*****************************************************************************/

#include <StdInc.h>

extern CClientGame* g_pClientGame;

CClientGUIElement::CClientGUIElement ( CClientManager * pManager, CLuaMain* pLuaMain, CGUIElement* pCGUIElement, ElementID ID ) : CClientEntity ( ID )
{
    m_pManager = pManager;
	m_pGUIManager = pManager->GetGUIManager ();
	m_pCGUIElement = pCGUIElement;
	m_pLuaMain = pLuaMain;
	m_bParentDestroyed = false;

	memset ( &_szCallbackFunc1[0], NULL, sizeof ( _szCallbackFunc1 ) );
	memset ( &_szCallbackFunc2[0], NULL, sizeof ( _szCallbackFunc2 ) );

	// Store the parent flag now since m_pCGUIElement may be deleted already when we call the destructor
	m_bHasParent = (m_pCGUIElement->GetParent() != NULL);

	// Store the this-pointer in the userdata variable
	CGUI_SET_CCLIENTGUIELEMENT ( pCGUIElement, this );

	// Generate the CGUI type name variable
	switch ( m_pCGUIElement->GetType () ) {
		case CGUI_BUTTON:
			m_szCGUITypeName = "button";
			break;
		case CGUI_CHECKBOX:
			m_szCGUITypeName = "checkbox";
			break;
		case CGUI_EDIT:
			m_szCGUITypeName = "edit";
			break;
		case CGUI_GRIDLIST:
			m_szCGUITypeName = "gridlist";
			break;
		case CGUI_LABEL:
			m_szCGUITypeName = "label";
			break;
		case CGUI_MEMO:
			m_szCGUITypeName = "memo";
			break;
		case CGUI_PROGRESSBAR:
			m_szCGUITypeName = "progressbar";
			break;
		case CGUI_RADIOBUTTON:
			m_szCGUITypeName = "radiobutton";
			break;
		case CGUI_STATICIMAGE:
			m_szCGUITypeName = "staticimage";
			break;
		case CGUI_TAB:
			m_szCGUITypeName = "tab";
			break;
		case CGUI_TABPANEL:
			m_szCGUITypeName = "tabpanel";
			break;
		case CGUI_WINDOW:
			m_szCGUITypeName = "window";
			break;
		case CGUI_SCROLLPANE:
			m_szCGUITypeName = "scrollpane";
			break;
		case CGUI_SCROLLBAR:
			m_szCGUITypeName = "scrollbar";
			break;
		default:
			m_szCGUITypeName = "unknown";
			break;
	}
	strcpy ( m_szTypeName, "gui-" );
	strcat ( m_szTypeName, m_szCGUITypeName );
    m_uiTypeHash = HashString ( m_szTypeName );

    // Add us to the list in the manager
    m_pGUIManager->Add ( this );
}


CClientGUIElement::~CClientGUIElement ( void )
{
    // Remove us from the list in the manager
    m_pGUIManager->Remove ( this );

	// Only delete if it doesn't have a parent (otherwise it'll be automatically deleted by CEGUI)
	if ( m_pCGUIElement && !m_bParentDestroyed )
		delete m_pCGUIElement;

	SetParentDestroyed(); // we tell all our children that their parent has been destroyed, so the 
                          // internal representation in cgui will be destroyed too.
}

void CClientGUIElement::SetParentDestroyed ( void )
{
	m_bParentDestroyed = true;
	list < CClientEntity* > ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
		if ( (*iter)->GetType() == CCLIENTGUI ) {
			CClientGUIElement * childElement = static_cast < CClientGUIElement *> ( (*iter) );
			childElement->SetParentDestroyed ();
		}
	}
}

void CClientGUIElement::Unlink ( void )
{
}


void CClientGUIElement::SetEvents ( const char* szFunc1, const char* szFunc2 )
{
	if ( szFunc1 && strlen ( szFunc1 ) < MAX_EVENT_NAME )
		strncpy ( &_szCallbackFunc1[0], szFunc1, strlen ( szFunc1 ) );

	if ( szFunc2 && strlen ( szFunc2 ) < MAX_EVENT_NAME )
		strncpy ( &_szCallbackFunc2[0], szFunc2, strlen ( szFunc2 ) );    
}


bool CClientGUIElement::_CallbackEvent1 ( CGUIElement* pCGUIElement )
{
	CLuaArguments Arg;
    if ( pCGUIElement )
    {
        CClientGUIElement* pElement = m_pGUIManager->Get ( pCGUIElement );
        if ( pElement )
	        Arg.PushElement ( pElement );
    }
	CallEvent ( _szCallbackFunc1, Arg, true );
	return true;
}


bool CClientGUIElement::_CallbackEvent2 ( CGUIElement* pCGUIElement )
{
	CLuaArguments Arg;
    if ( pCGUIElement )
    {
        CClientGUIElement* pElement = m_pGUIManager->Get ( pCGUIElement );
        if ( pElement )
	        Arg.PushElement ( pElement );
    }
	CallEvent ( _szCallbackFunc2, Arg, true );
	return true;
}
