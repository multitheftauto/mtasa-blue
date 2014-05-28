/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CWebView.cpp
*  PURPOSE:     Web view class
*
*****************************************************************************/

#include "StdInc.h"
#include "CWebView.h"

CWebView::CWebView ( unsigned int uiWidth, unsigned int uiHeight, IDirect3DSurface9* pD3DSurface )
{
    m_pD3DSurface = pD3DSurface;

    // Create Awesomium webview as offscreen webview
    m_pWebView = Awesomium::WebCore::instance()->CreateWebView ( uiWidth, uiHeight, NULL, Awesomium::kWebViewType_Offscreen );

    // Set handlers
    m_pWebView->set_load_listener ( this );
    m_pWebView->set_js_method_handler ( &m_JSMethodHandler );

}

CWebView::~CWebView()
{
    // Release WebView
    m_pWebView->Destroy ();
}

bool CWebView::LoadURL ( const SString& strURL, bool bFilterEnabled )
{
    Awesomium::WebURL webURL ( CWebCore::ToWebString ( strURL ) );
    if ( bFilterEnabled && g_pCore->GetWebCore ()->GetURLState ( CWebCore::ToSString ( webURL.host() ) ) != eURLState::WEBPAGE_ALLOWED )
        return false;

    m_pWebView->LoadURL ( webURL );
    return true;
}

bool CWebView::IsLoading ()
{
    return m_pWebView->IsLoading ();
}

void CWebView::GetURL ( SString& outURL )
{
    if ( !m_bIsLocal )
        outURL = static_cast<SString> ( CWebCore::ToSString ( m_pWebView->url ().spec () ) );
    else
        outURL = m_strTempURL;
}

void CWebView::GetTitle(SString& outTitle)
{
    outTitle = static_cast<SString> ( CWebCore::ToSString ( m_pWebView->title() ) );
}

void CWebView::SetRenderingPaused ( bool bPaused )
{
    if ( bPaused )
        m_pWebView->PauseRendering ();
    else
        m_pWebView->ResumeRendering ();
}

void CWebView::ExecuteJavascript(const SString& strJavascriptCode)
{
    m_pWebView->ExecuteJavascript ( CWebCore::ToWebString ( strJavascriptCode ), CWebCore::ToWebString("") );
}

void CWebView::InjectMouseMove ( int iPosX, int iPosY )
{
    m_pWebView->InjectMouseMove ( iPosX, iPosY );
}

void CWebView::InjectMouseDown ( int mouseButton )
{
    m_pWebView->InjectMouseDown ( (Awesomium::MouseButton) mouseButton );
}

void CWebView::InjectMouseUp ( int mouseButton )
{
    m_pWebView->InjectMouseUp ( (Awesomium::MouseButton) mouseButton );
}

void CWebView::InjectMouseWheel ( int iScrollVert, int iScrollHorz )
{
    m_pWebView->InjectMouseWheel ( iScrollVert, iScrollHorz );
}

void CWebView::InjectKeyboardEvent ( const SString& strKey, bool bKeyDown, bool bCharacter )
{
    // Hack fix
    SString key = strKey;
    if (key == " ")
        key = "space";

    Awesomium::WebKeyboardEvent keyboardEvent;
    keyboardEvent.is_system_key = false;
    keyboardEvent.modifiers = Awesomium::WebKeyboardEvent::kModIsAutorepeat;

    if ( !bCharacter )
    {
        // Get BindableKey structure (to be able to process MTA's key names correctly)
        const SBindableKey* pBindableKey = g_pCore->GetKeyBinds ()->GetBindableFromKey ( key.c_str () );
        if ( !pBindableKey )
            return;

        keyboardEvent.virtual_key_code = pBindableKey->ulCode;
        keyboardEvent.type = bKeyDown ? Awesomium::WebKeyboardEvent::kTypeKeyDown : Awesomium::WebKeyboardEvent::kTypeKeyUp;
    }
    else
    {
        std::wstring strCharacter (MbUTF8ToUTF16(strKey));
        if ( strCharacter.length() > 4 )
            return;
        memcpy ( keyboardEvent.text, strCharacter.c_str (), strCharacter.length () * 2 );
        keyboardEvent.type = Awesomium::WebKeyboardEvent::kTypeChar;
    }
    
    m_pWebView->InjectKeyboardEvent ( keyboardEvent );
}


////////////////////////////////////////////////////////////////////
//                                                                //
//      Implementation: Awesomium::WebViewListener::Load          //
// http://www.awesomium.com/docs/1_7_2/cpp_api/class_awesomium_1_1_web_view_listener_1_1_load.html#a3cb1ee5563db02f90cd5562c6d8342b1 //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::OnFinishLoadingFrame ( Awesomium::WebView* pCaller, int64 iFrameId, bool bMainFrame, const Awesomium::WebURL& url )
{
    // Clear the old list
    m_JSMethodHandler.Clear ();

    // Bind javascript global object
    Awesomium::JSValue jsMTA = pCaller->CreateGlobalJavascriptObject ( Awesomium::WSLit ("mta") );
    if ( jsMTA.IsObject () )
    {
        Awesomium::JSObject& mtaObject = jsMTA.ToObject ();
        m_JSMethodHandler.Bind ( mtaObject, Awesomium::WSLit ("triggerEvent"), Javascript_triggerEvent );
    }

    // Todo: Trigger an event

}



////////////////////////////////////////////////////////////////////
//                                                                //
//        Static Javascript methods: triggerEvent                 //
//                                                                //
////////////////////////////////////////////////////////////////////
void CWebView::Javascript_triggerEvent ( Awesomium::WebView* pWebView, const Awesomium::JSArray& args )
{
    if ( args.size() == 0 )
        return;

    // Convert JSArray to string array
    std::vector<SString> stringArray;

    for ( unsigned int i = 1; i < args.size (); ++i ) // Ignore first arg as it is the event name
    {
        stringArray.push_back ( CWebCore::ToSString ( args[i].ToString() ) );
    }

    // Pass string array to real triggerEvent
    SString strEventName = CWebCore::ToSString ( args[0].ToString() );
    CModManager::GetSingleton().GetCurrentMod ()->WebsiteTriggerEventHandler ( strEventName, stringArray ); // Does anybody know a better way to trigger an event in the deathmatch module?
}

////////////////////////////////////////////////////////////////////
//                                                                //
//        Static Javascript methods: alert                        //
//        Exports the javascript alert to Lua                     //
//                                                                //
////////////////////////////////////////////////////////////////////
/*void CWebView::Javascript_alert ( Awesomium::WebView* pWebView, const Awesomium::JSArray& args )
{
    SString message = "";
    if ( args.size() > 0 )
        message = CWebCore::ToSString ( args[0].ToString () );

    CModManager::GetSingleton().GetCurrentMod()->WebsiteAlertHandler ( message );
}
*/