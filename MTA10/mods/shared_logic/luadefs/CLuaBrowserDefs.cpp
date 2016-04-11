/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaBrowserDefs.cpp
*  PURPOSE:     Lua browser definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaBrowserDefs::LoadFunctions ( void )
{
    // Browser functions
    CLuaCFunctions::AddFunction ( "createBrowser", CreateBrowser );
    CLuaCFunctions::AddFunction ( "requestBrowserDomains", RequestBrowserDomains );
    CLuaCFunctions::AddFunction ( "loadBrowserURL", LoadBrowserURL );
    CLuaCFunctions::AddFunction ( "isBrowserLoading", IsBrowserLoading );
    CLuaCFunctions::AddFunction ( "injectBrowserMouseMove", InjectBrowserMouseMove );
    CLuaCFunctions::AddFunction ( "injectBrowserMouseDown", InjectBrowserMouseDown );
    CLuaCFunctions::AddFunction ( "injectBrowserMouseUp", InjectBrowserMouseUp );
    CLuaCFunctions::AddFunction ( "injectBrowserMouseWheel", InjectBrowserMouseWheel );
    CLuaCFunctions::AddFunction ( "getBrowserTitle", GetBrowserTitle );
    CLuaCFunctions::AddFunction ( "getBrowserURL", GetBrowserURL );
    CLuaCFunctions::AddFunction ( "setBrowserRenderingPaused", SetBrowserRenderingPaused );
    CLuaCFunctions::AddFunction ( "executeBrowserJavascript", ExecuteBrowserJavascript );
    CLuaCFunctions::AddFunction ( "getBrowserVolume", GetBrowserVolume );
    CLuaCFunctions::AddFunction ( "setBrowserVolume", SetBrowserVolume );
    CLuaCFunctions::AddFunction ( "isBrowserDomainBlocked", IsBrowserDomainBlocked );
    CLuaCFunctions::AddFunction ( "focusBrowser", FocusBrowser );
    CLuaCFunctions::AddFunction ( "isBrowserFocused", IsBrowserFocused );
    CLuaCFunctions::AddFunction ( "setBrowserProperty", SetBrowserProperty );
    CLuaCFunctions::AddFunction ( "getBrowserProperty", GetBrowserProperty );
    CLuaCFunctions::AddFunction ( "getBrowserSettings", GetBrowserSettings );
    CLuaCFunctions::AddFunction ( "getBrowserSource", GetBrowserSource );
    CLuaCFunctions::AddFunction ( "setBrowserAjaxHandler", SetBrowserAjaxHandler );
    CLuaCFunctions::AddFunction ( "canBrowserNavigateBack", CanBrowserNavigateBack );
    CLuaCFunctions::AddFunction ( "canBrowserNavigateForward", CanBrowserNavigateForward );
    CLuaCFunctions::AddFunction ( "navigateBrowserBack", NavigateBrowserBack );
    CLuaCFunctions::AddFunction ( "navigateBrowserForward", NavigateBrowserForward );
    CLuaCFunctions::AddFunction ( "reloadBrowserPage", ReloadBrowserPage );
    CLuaCFunctions::AddFunction ( "toggleBrowserDevTools", ToggleBrowserDevTools );
    CLuaCFunctions::AddFunction ( "guiCreateBrowser", GUICreateBrowser );
    CLuaCFunctions::AddFunction ( "guiGetBrowser", GUIGetBrowser );
}


void CLuaBrowserDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createBrowser" );
    lua_classfunction ( luaVM, "loadURL", "loadBrowserURL" );
    lua_classfunction ( luaVM, "isLoading", "isBrowserLoading" );
    lua_classfunction ( luaVM, "injectMouseMove", "injectBrowserMouseMove" );
    lua_classfunction ( luaVM, "injectMouseDown", "injectBrowserMouseDown" );
    lua_classfunction ( luaVM, "injectMouseUp", "injectBrowserMouseUp" );
    lua_classfunction ( luaVM, "injectMouseWheel", "injectBrowserMouseWheel" );
    lua_classfunction ( luaVM, "getTitle", "getBrowserTitle" );
    lua_classfunction ( luaVM, "getURL", "getBrowserURL" );
    lua_classfunction ( luaVM, "setRenderingPaused", "setBrowserRenderingPaused" );
    lua_classfunction ( luaVM, "executeJavascript", "executeBrowserJavascript" );
    lua_classfunction ( luaVM, "getVolume", "getBrowserVolume" );
    lua_classfunction ( luaVM, "setVolume", "setBrowserVolume" );
    lua_classfunction ( luaVM, "focus", "focusBrowser" );
    lua_classfunction ( luaVM, "isFocused", "isBrowserFocused" );
    lua_classfunction ( luaVM, "setProperty", "setBrowserProperty" );
    lua_classfunction ( luaVM, "getProperty", "getBrowserProperty" );
    lua_classfunction ( luaVM, "getSource", "getBrowserSource" );
    lua_classfunction ( luaVM, "setAjaxHandler", "setBrowserAjaxHandler" );
    lua_classfunction ( luaVM, "canNavigateBack", "canBrowserNavigateBack" );
    lua_classfunction ( luaVM, "canNavigateForward", "canBrowserNavigateForward" );
    lua_classfunction ( luaVM, "navigateBack", "navigateBrowserBack" );
    lua_classfunction ( luaVM, "navigateForward", "navigateBrowserForward" );
    lua_classfunction ( luaVM, "reloadPage", "reloadBrowserPage" );
    lua_classfunction ( luaVM, "toggleDevTools", "toggleBrowserDevTools" );

    lua_classfunction ( luaVM, "requestDomains", "requestBrowserDomains" );
    lua_classfunction ( luaVM, "isDomainBlocked", "isBrowserDomainBlocked" );

    lua_classvariable ( luaVM, "url", "loadBrowserURL", "getBrowserURL" );
    lua_classvariable ( luaVM, "loading", nullptr, "isBrowserLoading" );
    lua_classvariable ( luaVM, "title", nullptr, "getBrowserTitle" );
    lua_classvariable ( luaVM, "renderingPaused", "setBrowserRenderingPaused", nullptr );
    lua_classvariable ( luaVM, "volume", "setBrowserVolume", "getBrowserVolume" );
    lua_classvariable ( luaVM, "devTools", "toggleBrowserDevTools", nullptr );

    lua_registerclass ( luaVM, "Browser", "DxTexture" );

    // Add GUI browser class
    lua_newclass ( luaVM );
    lua_classfunction ( luaVM, "create", "guiCreateBrowser" );
    lua_classfunction ( luaVM, "getBrowser", "guiGetBrowser" );
    lua_classvariable ( luaVM, "browser", nullptr, "guiGetBrowser" );
    lua_registerclass ( luaVM, "GuiBrowser", "GuiElement" );
}


int CLuaBrowserDefs::CreateBrowser ( lua_State* luaVM )
{
    //  texture createBrowser ( int width, int height, bool isLocal [, bool transparent = false] )
    CVector2D vecSize; bool bIsLocal; bool bTransparent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector2D ( vecSize );
    argStream.ReadBool ( bIsLocal );
    argStream.ReadBool ( bTransparent, false );

    if ( !argStream.HasErrors () )
    {
        if ( !bIsLocal && !g_pCore->GetWebCore ()->GetRemotePagesEnabled () )
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pParentResource = pLuaMain->GetResource ();

            CClientWebBrowser* pBrowserTexture = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateWebBrowser ( (int) vecSize.fX, (int) vecSize.fY, bIsLocal, bTransparent );
            if ( pBrowserTexture )
            {
                // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                pBrowserTexture->SetParent ( pParentResource->GetResourceDynamicEntity () );

                // Set our owner resource
                pBrowserTexture->SetResource ( pParentResource );
            }
            lua_pushelement ( luaVM, pBrowserTexture );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::RequestBrowserDomains ( lua_State* luaVM )
{
    //  bool requestBrowserDomains ( table domains, bool isURL [, function callback ] )
    std::vector<SString> pages; bool bIsURL; CLuaFunctionRef callbackFunction;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadStringTable ( pages );
    argStream.ReadBool ( bIsURL, false );
    argStream.ReadFunction ( callbackFunction, LUA_REFNIL );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Convert to domains if we got a list of URLs
        if ( bIsURL )
        {
            for ( auto& strURL : pages )
            {
                strURL = g_pCore->GetWebCore ()->GetDomainFromURL ( strURL );
            }
        }

        WebRequestCallback callback = [=] ( bool bAllow, const std::vector<SString>& domains ) {
            // Test if luaVM is still available
            if ( m_pLuaManager->IsLuaVMValid ( luaVM ) && VERIFY_FUNCTION ( callbackFunction ) )
            {
                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
                if ( !pLuaMain )
                    return;

                CLuaArguments arguments;
                arguments.PushBoolean ( bAllow );

                CLuaArguments LuaTable;
                int i = 0;
                for ( const auto& domain : domains )
                {
                    LuaTable.PushNumber ( ++i );
                    LuaTable.PushString ( domain );
                }
                arguments.PushTable ( &LuaTable );
                arguments.Call ( pLuaMain, callbackFunction );
            }
        };

        g_pCore->GetWebCore ()->RequestPages ( pages, VERIFY_FUNCTION ( callbackFunction ) ? &callback : nullptr );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::LoadBrowserURL ( lua_State* luaVM )
{
    //  bool loadBrowserURL ( browser webBrowser, string url [, string postData = "", bool postURLEncoded = true ] )
    CClientWebBrowser* pWebBrowser; SString strURL; SString strPostData; bool bURLEncoded;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strURL );
    argStream.ReadString ( strPostData, "" );
    argStream.ReadBool ( bURLEncoded, true );

    if ( !argStream.HasErrors () )
    {
        // Are we dealing with a remote website?
        if ( strURL.substr ( 0, 7 ) == "http://" || strURL.substr ( 0, 8 ) == "https://" )
        {
            bool isLocalURL = strURL.substr ( 0, 11 ) == "http://mta/";
            if ( pWebBrowser->IsLocal () != isLocalURL )
            {
                lua_pushboolean ( luaVM, false );
                return 1;
            }

            lua_pushboolean ( luaVM, pWebBrowser->LoadURL ( strURL, !isLocalURL, strPostData, bURLEncoded ) );
            return 1;
        }

        // Are we dealing with a local website? If so, parse resource path. Otherwise, return false and load nothing
        // Todo: Add an ACL right which is necessary to load local websites or websites in general
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            SString strAbsPath;
            CResource* pResource = pLuaMain->GetResource ();
            if ( CResourceManager::ParseResourcePathInput ( strURL, pResource, strAbsPath ) && pWebBrowser->IsLocal () )
            {
                // Output deprecated warning, TODO: Remove this at a later point
                m_pScriptDebugging->LogWarning ( luaVM, "This URL scheme is deprecated and may not work in future versions. Please consider using http://mta/* instead. See https://wiki.mtasa.com/wiki/LoadBrowserURL for details" );

                lua_pushboolean ( luaVM, pWebBrowser->LoadURL ( "mtalocal://" + strURL, false, strPostData, bURLEncoded ) );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::IsBrowserLoading ( lua_State* luaVM )
{
    //  bool isBrowserLoading(browser webBrowser)
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pWebBrowser->IsLoading () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaBrowserDefs::InjectBrowserMouseMove ( lua_State* luaVM )
{
    //  bool injectBrowserMouseMove(browser webBrowser, int x, int y)
    CClientWebBrowser* pWebBrowser; CVector2D vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadVector2D ( vecPosition );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->InjectMouseMove ( (int) vecPosition.fX, (int) vecPosition.fY );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::InjectBrowserMouseDown ( lua_State* luaVM )
{
    //  bool injectBrowserMouseDown ( browser webBrowser, string mouseButton )
    CClientWebBrowser* pWebBrowser; eWebBrowserMouseButton mouseButton;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadEnumString ( mouseButton );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->InjectMouseDown ( mouseButton );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::InjectBrowserMouseUp ( lua_State* luaVM )
{
    //  bool injectBrowserMouseUp ( browser webBrowser, string mouseButton )
    CClientWebBrowser* pWebBrowser; eWebBrowserMouseButton mouseButton;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadEnumString ( mouseButton );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->InjectMouseUp ( mouseButton );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::InjectBrowserMouseWheel ( lua_State* luaVM )
{
    //  bool injectMouseWheel ( browser webBrowser, int scrollVertical, int scrollHorizontal )
    CClientWebBrowser* pWebBrowser; int iScrollVert; int iScrollHorz;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadNumber ( iScrollVert );
    argStream.ReadNumber ( iScrollHorz );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->InjectMouseWheel ( iScrollVert, iScrollHorz );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::GetBrowserTitle ( lua_State* luaVM )
{
    //  string getBrowserPageTitle ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        lua_pushstring ( luaVM, pWebBrowser->GetTitle () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::GetBrowserURL ( lua_State* luaVM )
{
    //  string getBrowserURL ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        lua_pushstring ( luaVM, pWebBrowser->GetURL () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::SetBrowserRenderingPaused ( lua_State* luaVM )
{
    //  bool setBrowserRenderingPaused ( browser webBrowser, bool paused )
    CClientWebBrowser* pWebBrowser; bool bPaused;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadBool ( bPaused );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->SetRenderingPaused ( bPaused );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::ExecuteBrowserJavascript ( lua_State* luaVM )
{
    //  bool executeBrowserJavascript ( browser webBrowser, string jsCode )
    CClientWebBrowser* pWebBrowser; SString strJavascriptCode;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strJavascriptCode );

    if ( !argStream.HasErrors () )
    {
        if ( pWebBrowser->ExecuteJavascript ( strJavascriptCode ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            argStream.SetCustomError ( "This function does not work with remote browsers" );
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::GetBrowserVolume ( lua_State* luaVM )
{
    //  float getBrowserVolume ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, pWebBrowser->GetAudioVolume () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::SetBrowserVolume ( lua_State* luaVM )
{
    //  bool setBrowserVolume ( float volume )
    //  bool setBrowserVolume ( browser webBrowser, float volume )
    CClientWebBrowser* pWebBrowser; float fVolume;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsNumber () )
    {
        argStream.ReadNumber ( fVolume );
        lua_pushboolean ( luaVM, g_pCore->GetWebCore ()->SetGlobalAudioVolume ( fVolume ) );
        return 1;
    }

    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadNumber ( fVolume );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pWebBrowser->SetAudioVolume ( fVolume ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::IsBrowserDomainBlocked ( lua_State* luaVM )
{
    //  bool isBrowserDomainBlocked ( string domain, bool isURL )
    SString strURL; bool bIsURL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strURL );
    argStream.ReadBool ( bIsURL, false );

    if ( !argStream.HasErrors () )
    {
        if ( bIsURL )
            strURL = g_pCore->GetWebCore ()->GetDomainFromURL ( strURL );

        if ( !strURL.empty () )
        {
            lua_pushboolean ( luaVM, g_pCore->GetWebCore ()->GetURLState ( strURL ) != eURLState::WEBPAGE_ALLOWED );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaBrowserDefs::FocusBrowser ( lua_State* luaVM )
{
    //  focusBrowser ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsNil () || argStream.NextIsNone () )
    {
        g_pCore->GetWebCore ()->SetFocusedWebView ( NULL );
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->Focus ();
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::IsBrowserFocused ( lua_State* luaVM )
{
    //  browser isBrowserFocused ()
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        CWebViewInterface* pWebView = g_pCore->GetWebCore ()->GetFocusedWebView ();
        lua_pushboolean ( luaVM, pWebBrowser->GetWebView () == pWebView );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaBrowserDefs::SetBrowserProperty ( lua_State* luaVM )
{
    //  bool setBrowserProperty ( browser webBrowser, string key, string value )
    CClientWebBrowser* pWebBrowser; SString strKey; SString strValue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strValue );

    if ( !argStream.HasErrors () )
    {
        if ( pWebBrowser->SetProperty ( strKey, strValue ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::GetBrowserProperty ( lua_State* luaVM )
{
    //  string getBrowserProperty ( browser webBrowser, string key )
    CClientWebBrowser* pWebBrowser; SString strKey;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        SString strValue;
        if ( pWebBrowser->GetProperty ( strKey, strValue ) )
        {
            lua_pushstring ( luaVM, strValue );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaBrowserDefs::GetBrowserSettings ( lua_State* luaVM )
{
    //  table getBrowserSettings ()
    lua_createtable ( luaVM, 0, 3 );

    lua_pushstring ( luaVM, "RemoteEnabled" );
    lua_pushboolean ( luaVM, g_pCore->GetWebCore ()->GetRemotePagesEnabled () );
    lua_settable ( luaVM, -3 );

    lua_pushstring ( luaVM, "RemoteJavascript" );
    lua_pushboolean ( luaVM, g_pCore->GetWebCore ()->GetRemoteJavascriptEnabled () );
    lua_settable ( luaVM, -3 );

    lua_pushstring ( luaVM, "PluginsEnabled" );
    lua_pushboolean ( luaVM, g_pCore->GetWebCore ()->GetPluginsEnabled () );
    lua_settable ( luaVM, -3 );

    return 1;
}

int CLuaBrowserDefs::GetBrowserSource ( lua_State* luaVM )
{
    //  bool getBrowserSource ( function callback )
    CClientWebBrowser* pWebBrowser; CLuaFunctionRef callbackFunction;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadFunction ( callbackFunction );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain && VERIFY_FUNCTION ( callbackFunction ) )
        {
            pWebBrowser->GetSourceCode ( [callbackFunction, pLuaMain] ( const std::string& code ) {
                /*
                This function should not be called when the resource is about to stop as
                stopping the resource destroys the browser element and thus cancels the
                CefStringVisitor callback class (see CWebView::GetSourceCode::MyStringVisitor)
                */
                if ( VERIFY_FUNCTION ( callbackFunction ) )
                {
                    CLuaArguments arguments;
                    // TODO: Use SCharStringRef/direct string access instead of copying strings around
                    arguments.PushString ( code );
                    arguments.Call ( pLuaMain, callbackFunction );
                }
            } );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::ToggleBrowserDevTools ( lua_State* luaVM )
{
    //  bool toggleBrowserDevTools ( browser webBrowser, bool visible )
    CClientWebBrowser* pWebBrowser; bool visible;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadBool ( visible );

    if ( !argStream.HasErrors () )
    {
        if ( g_pCore->GetWebCore ()->IsTestModeEnabled () )
        {
            lua_pushboolean ( luaVM, pWebBrowser->ToggleDevTools ( visible ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, "toggleBrowserDevtools can only be used in development mode" );

    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::CanBrowserNavigateBack ( lua_State* luaVM )
{
    //  bool canBrowserNavigateBack( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pWebBrowser->CanGoBack () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaBrowserDefs::CanBrowserNavigateForward ( lua_State* luaVM )
{
    //  bool canBrowserNavigateForward( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pWebBrowser->CanGoForward () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaBrowserDefs::NavigateBrowserBack ( lua_State* luaVM )
{
    //  bool navigateBrowserBack( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pWebBrowser->GoBack () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaBrowserDefs::NavigateBrowserForward ( lua_State* luaVM )
{
    //  bool navigateBrowserForward( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pWebBrowser->GoForward () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaBrowserDefs::ReloadBrowserPage( lua_State* luaVM )
{
    //  bool reloadBrowserPage( browser webBrowser )
    CClientWebBrowser* pWebBrowser;
    bool bIgnoreCache;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadIfNextIsBool ( bIgnoreCache, false );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->Refresh ( bIgnoreCache );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaBrowserDefs::GUICreateBrowser ( lua_State* luaVM )
{
    //  element guiCreateBrowser ( float x, float y, float width, float height, bool isLocal, bool isTransparent, bool relative, [element parent = nil] )
    float x; float y; float width; float height; bool bIsLocal; bool bIsTransparent; bool bIsRelative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadBool ( bIsLocal );
    argStream.ReadBool ( bIsTransparent );
    argStream.ReadBool ( bIsRelative );
    argStream.ReadUserData ( parent, nullptr );

    if ( !argStream.HasErrors () )
    {
        if ( !bIsLocal && !g_pCore->GetWebCore ()->GetRemotePagesEnabled () )
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateBrowser ( *pLuaMain, x, y, width, height, bIsLocal, bIsTransparent, bIsRelative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::GUIGetBrowser ( lua_State* luaVM ) // Or rather guiGetBrowserBrowser?
{
    //  webbrowser guiGetBrowser ( gui-webbrowser browser )
    CClientGUIElement* pGUIElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIWebBrowser > ( pGUIElement );

    if ( !argStream.HasErrors () )
    {
        if ( IS_GUI ( pGUIElement ) && pGUIElement->GetCGUIType () == CGUI_WEBBROWSER )
        {
            CClientGUIWebBrowser* pGUIBrowser = static_cast < CClientGUIWebBrowser* > ( pGUIElement );
            lua_pushelement ( luaVM, pGUIBrowser->GetBrowser () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaBrowserDefs::SetBrowserAjaxHandler ( lua_State* luaVM )
{
    //  bool setBrowserAjaxHandler ( browser browser, string URL[, function callback] )
    CClientWebBrowser* pWebBrowser; SString strURL; CLuaFunctionRef callbackFunction;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strURL );

    if ( argStream.NextIsNil () || argStream.NextIsNone () )
    {
        if ( !argStream.HasErrors () )
        {
            lua_pushboolean ( luaVM, pWebBrowser->RemoveAjaxHandler ( strURL ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }
    else
    {
        argStream.ReadFunction ( callbackFunction );
        argStream.ReadFunctionComplete ();
        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain && VERIFY_FUNCTION ( callbackFunction ) )
            {
                CResource* pResource = pLuaMain->GetResource ();
                CResourceManager * pResourceManager = m_pResourceManager;
                auto netId = pResource->GetNetID ();

                bool bResult = pWebBrowser->AddAjaxHandler ( strURL,
                    [=] ( std::vector<SString>& vecGet, std::vector<SString>& vecPost ) -> const SString
                {
                    // Make sure the resource is still running
                    if ( !pResourceManager->Exists ( pResource ) || pResource->GetNetID () != netId )
                    {
                        return "";
                    }

                // Make sure the function is valid
                if ( VERIFY_FUNCTION ( callbackFunction ) )
                {
                    CLuaArguments arguments;
                    CLuaArguments getArguments;
                    CLuaArguments postArguments;

                    for ( auto&& param : vecGet )
                        getArguments.PushString ( param );

                    for ( auto&& param : vecPost )
                        postArguments.PushString ( param );

                    arguments.PushTable ( &getArguments );
                    arguments.PushTable ( &postArguments );

                    CLuaArguments result;

                    arguments.Call ( pLuaMain, callbackFunction, &result );

                    if ( result.Count () == 0 )
                        return "";


                    CLuaArgument* returnedValue = *result.IterBegin ();
                    if ( returnedValue->GetType () == LUA_TSTRING )
                        return returnedValue->GetString ();
                    else
                        return "";
                }
                else
                    return "";

                } );

                lua_pushboolean ( luaVM, bResult );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}