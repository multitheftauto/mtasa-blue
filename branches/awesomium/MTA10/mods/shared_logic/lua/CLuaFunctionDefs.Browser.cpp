/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Browser.cpp
*  PURPOSE:     Lua function definitions class
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::CreateBrowser ( lua_State* luaVM )
{
//  texture createBrowser ( int width, int height )
    int width; int height;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    
    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pParentResource = pLuaMain->GetResource ();

            CClientWebBrowser* pBrowserTexture = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateWebBrowser ( width, height );
            if ( pBrowserTexture )
            {
                // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                pBrowserTexture->SetParent ( pParentResource->GetResourceDynamicEntity () );
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

int CLuaFunctionDefs::UpdateBrowser ( lua_State* luaVM )
{
//  bool updateBrowser ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        g_pCore->GetGraphics ()->GetRenderItemManager ()->UpdateWebBrowser ( pWebBrowser->GetWebBrowserItem () );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::RequestBrowserPages ( lua_State* luaVM )
{
//  bool requestBrowserPages ( table pages )
    std::vector<SString> pages;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadStringTable ( pages );

    if ( !argStream.HasErrors () )
    {
        g_pCore->GetWebCore ()->RequestPages ( pages );
        // Todo: Add a callback or event to check if the pagerequest dialog was successfully done
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::LoadBrowserURL ( lua_State* luaVM )
{
//  bool loadBrowserURL(browser webBrowser, string url)
    CClientWebBrowser* pWebBrowser; SString strURL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strURL );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pWebBrowser->LoadURL ( strURL ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsBrowserLoading ( lua_State* luaVM )
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

int CLuaFunctionDefs::InjectBrowserMouseMove ( lua_State* luaVM )
{
//  bool injectBrowserMove(browser webBrowser, int x, int y)
    CClientWebBrowser* pWebBrowser; int posX; int posY;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadNumber ( posX );
    argStream.ReadNumber ( posY );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->InjectMouseMove ( posX, posY );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::InjectBrowserMouseDown ( lua_State* luaVM )
{
//  bool injectBrowserMouseDown ( browser webBrowser, int mouseButton )
    CClientWebBrowser* pWebBrowser; int mouseButton;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadNumber ( mouseButton );

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

int CLuaFunctionDefs::InjectBrowserMouseUp ( lua_State* luaVM )
{
//  bool injectBrowserMouseUp ( browser webBrowser, int mouseButton )
    CClientWebBrowser* pWebBrowser; int mouseButton;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadNumber ( mouseButton );

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

int CLuaFunctionDefs::InjectBrowserMouseWheel ( lua_State* luaVM )
{
//  bool injectMouseWheel ( browser webBrowser, int scrollVertical, int scrollHorizontal )
    CClientWebBrowser* pWebBrowser; int iScrollVert; int iScrollHorz;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadNumber ( iScrollVert );
    argStream.ReadNumber ( iScrollHorz );

    if ( !argStream.HasErrors() )
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

int CLuaFunctionDefs::InjectBrowserKeyDown ( lua_State* luaVM )
{
//  bool injectBrowserKeyDown ( browser webBrowser, string key )
    CClientWebBrowser* pWebBrowser; SString strKey;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->InjectKeyboardEvent ( strKey, true );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::InjectBrowserKeyUp ( lua_State* luaVM )
{
//  bool injectBrowserKeyDown ( browser webBrowser, string key )
    CClientWebBrowser* pWebBrowser; SString strKey;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->InjectKeyboardEvent ( strKey, false );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::InjectBrowserCharacter ( lua_State* luaVM )
{
//  bool injectBrowserCharacter ( browser webBrowser, string character )
    CClientWebBrowser* pWebBrowser; SString strCharacter;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );
    argStream.ReadString ( strCharacter );

    if ( !argStream.HasErrors () )
    {
        pWebBrowser->InjectKeyboardEvent ( strCharacter, true, true );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetBrowserTitle ( lua_State* luaVM )
{
//  string getBrowserPageTitle ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        SString strPageTitle;
        pWebBrowser->GetTitle ( strPageTitle );

        lua_pushstring ( luaVM, strPageTitle );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetBrowserURL ( lua_State* luaVM )
{
//  string getBrowserURL ( browser webBrowser )
    CClientWebBrowser* pWebBrowser;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWebBrowser );

    if ( !argStream.HasErrors () )
    {
        SString strURL;
        pWebBrowser->GetURL ( strURL );

        lua_pushstring ( luaVM, strURL );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetBrowserRenderingPaused ( lua_State* luaVM )
{
//  bool setBrowserRenderingPaused ( browser webBrowser, bool paused )
    CClientWebBrowser* pWebBrowser; bool bPaused;

    CScriptArgReader argStream(luaVM);
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
