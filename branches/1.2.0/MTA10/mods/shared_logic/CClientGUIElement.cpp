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

#include "StdInc.h"

using std::list;

extern CClientGame* g_pClientGame;

CClientGUIElement::CClientGUIElement ( CClientManager * pManager, CLuaMain* pLuaMain, CGUIElement* pCGUIElement, ElementID ID ) : ClassInit ( this ), CClientEntity ( ID )
{
    m_pManager = pManager;
    m_pGUIManager = pManager->GetGUIManager ();
    m_pCGUIElement = pCGUIElement;
    m_pLuaMain = pLuaMain;
    m_pFontElement = NULL;

    memset ( &_szCallbackFunc1[0], NULL, sizeof ( _szCallbackFunc1 ) );
    memset ( &_szCallbackFunc2[0], NULL, sizeof ( _szCallbackFunc2 ) );

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
        case CGUI_COMBOBOX:
            m_szCGUITypeName = "combobox";
            break;
        default:
            m_szCGUITypeName = "unknown";
            break;
    }
    SetTypeName ( SString ( "gui-%s", m_szCGUITypeName ) );

    // Add us to the list in the manager
    m_pGUIManager->Add ( this );
}


CClientGUIElement::~CClientGUIElement ( void )
{
    // Remove us from the list in the manager
    Unlink ();

    if ( m_pCGUIElement )
        delete m_pCGUIElement;
}

void CClientGUIElement::Unlink ( void )
{
    // Detach from any custom font
    if ( m_pFontElement )
        SetFont( "", NULL );

    m_pGUIManager->Remove ( this );
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
        {
            Arg.PushElement ( pElement );
            pElement->CallEvent ( _szCallbackFunc1, Arg, true );
            return true;
        }
    }
    return false;
}


bool CClientGUIElement::_CallbackEvent2 ( CGUIElement* pCGUIElement )
{
    CLuaArguments Arg;
    if ( pCGUIElement )
    {
        CClientGUIElement* pElement = m_pGUIManager->Get ( pCGUIElement );
        if ( pElement )
        {
            Arg.PushElement ( pElement );
            pElement->CallEvent ( _szCallbackFunc2, Arg, true );
            return true;
        }
    }
    return false;
}


//
// Get which font name and font element we are using now
//
SString CClientGUIElement::GetFont ( CClientGuiFont** ppFontElement )
{
    *ppFontElement = m_pFontElement;
    return GetCGUIElement ()->GetFont ();
}

//
// Change font
//
bool CClientGUIElement::SetFont ( const SString& strInFontName, CClientGuiFont* pFontElement )
{
    SString strFontName = strInFontName;

    if ( pFontElement )
        strFontName = pFontElement->GetCEGUIFontName ();
    else
    if ( strFontName.empty () )
        strFontName = "default-normal";

    if ( GetCGUIElement ()->SetFont ( strFontName ) )
    {
        if ( m_pFontElement )   m_pFontElement->NotifyGUIElementDetach ( this );
        m_pFontElement = pFontElement;
        if ( m_pFontElement )   m_pFontElement->NotifyGUIElementAttach ( this );
        return true;
    }
    return false;
}

