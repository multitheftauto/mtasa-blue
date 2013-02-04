/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.GUI.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Florian Busse <flobu@gmx.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::GUIGetInputEnabled ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, CStaticFunctionDefinitions::GUIGetInputEnabled () );
    return 1;
}


int CLuaFunctionDefs::GUISetInputEnabled ( lua_State* luaVM )
{
//  bool guiSetInputEnabled ( bool enabled )
    bool enabled;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( enabled );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUISetInputMode ( enabled ? INPUTMODE_NO_BINDS : INPUTMODE_ALLOW_BINDS );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetInputMode ( lua_State* luaVM )
{
//  bool guiSetInputMode ( string mode )
    eInputMode mode;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumString ( mode );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUISetInputMode ( mode );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetInputMode ( lua_State* luaVM )
{
    eInputMode mode = CStaticFunctionDefinitions::GUIGetInputMode ();
    lua_pushstring ( luaVM, EnumToString ( mode ) );
    return 1;
}


int CLuaFunctionDefs::GUIIsChatBoxInputActive ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pCore->IsChatInputEnabled () );
    return 1;
}


int CLuaFunctionDefs::GUIIsConsoleActive ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pCore->GetConsole ()->IsVisible () );
    return 1;
}


int CLuaFunctionDefs::GUIIsDebugViewActive ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pCore->IsDebugVisible () );
    return 1;
}


int CLuaFunctionDefs::GUIIsMainMenuActive ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pCore->IsMenuVisible () );
    return 1;
}


int CLuaFunctionDefs::GUIIsMTAWindowActive ( lua_State* luaVM )
{
    bool bActive = ( g_pCore->IsChatInputEnabled () ||
        g_pCore->IsMenuVisible () ||
        g_pCore->GetConsole ()->IsVisible () ||
        g_pClientGame->GetTransferBox ()->IsVisible () );

    lua_pushboolean ( luaVM, bActive );
    return 1;
}


int CLuaFunctionDefs::GUIIsTransferBoxActive ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pClientGame->GetTransferBox ()->IsVisible () );
    return 1;
}


int CLuaFunctionDefs::GUICreateWindow ( lua_State* luaVM )
{
//  element guiCreateWindow ( float x, float y, float width, float height, string titleBarText, bool relative )
    float x; float y; float width; float height; SString titleBarText; bool relative;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadString ( titleBarText );
    argStream.ReadBool ( relative );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateWindow ( *pLuaMain, x, y, width, height, titleBarText, relative );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateLabel ( lua_State* luaVM )
{
//  element guiCreateLabel ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
    float x; float y; float width; float height; SString text; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadString ( text );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateLabel ( *pLuaMain, x, y, width, height, text, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateStaticImage ( lua_State* luaVM )
{
//  element guiCreateStaticImage ( float x, float y, float width, float height, string path, bool relative, [element parent = nil] )
    float x; float y; float width; float height; SString path; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadString ( path );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            SString strPath;
            if ( CResourceManager::ParseResourcePathInput( path, pResource, strPath ) )
            {
                CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateStaticImage ( *pLuaMain, x, y, width, height, strPath, relative, parent );
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
            else
                argStream.SetCustomError( strPath, "Bad file path" );
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateButton ( lua_State* luaVM )
{
//  element guiCreateButton ( float x, float y, float width, float height, string text, bool relative, [ element parent = nil ] )
    float x; float y; float width; float height; SString text; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadString ( text );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateButton ( *pLuaMain, x, y, width, height, text, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateProgressBar ( lua_State* luaVM )
{
//  element guiCreateProgressBar ( float x, float y, float width, float height, bool relative, [element parent = nil] )
    float x; float y; float width; float height; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateProgressBar ( *pLuaMain, x, y, width, height, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateCheckBox ( lua_State* luaVM )
{
//  element guiCreateCheckBox ( float x, float y, float width, float height, string text, bool selected, bool relative, [element parent = nil] )
    float x; float y; float width; float height; SString text; bool selected; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadString ( text );
    argStream.ReadBool ( selected );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateCheckBox ( *pLuaMain, x, y, width, height, text, selected, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateRadioButton ( lua_State* luaVM )
{
//  element guiCreateRadioButton ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
    float x; float y; float width; float height; SString text; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadString ( text );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateRadioButton ( *pLuaMain, x, y, width, height, text, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateEdit ( lua_State* luaVM )
{
//  gui-edit guiCreateEdit ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
    float x; float y; float width; float height; SString text; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadString ( text );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateEdit ( *pLuaMain, x, y, width, height, text, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateMemo ( lua_State* luaVM )
{
//  gui-memo guiCreateMemo ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
    float x; float y; float width; float height; SString text; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadString ( text );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateMemo ( *pLuaMain, x, y, width, height, text, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateGridList ( lua_State* luaVM )
{
//  element guiCreateGridList ( float x, float y, float width, float height, bool relative, [ element parent = nil ] )
    float x; float y; float width; float height; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateGridList ( *pLuaMain, x, y, width, height, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateScrollPane ( lua_State* luaVM )
{
//  element guiCreateScrollPane( float x, float y, float width, float height, bool relative, [gui-element parent = nil])
    float x; float y; float width; float height; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateScrollPane ( *pLuaMain, x, y, width, height, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateScrollBar ( lua_State* luaVM )
{
//  gui-scrollbar guiCreateScrollBar ( float x, float y, float width, float height, bool horizontal, bool relative, [gui-element parent = nil])
    float x; float y; float width; float height; bool horizontal; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadBool ( horizontal );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateScrollBar ( *pLuaMain, x, y, width, height, horizontal, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateTabPanel ( lua_State* luaVM )
{
//  element guiCreateTabPanel ( float x, float y, float width, float height, bool relative, [element parent = nil ] )
    float x; float y; float width; float height; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateTabPanel ( *pLuaMain, x, y, width, height, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIStaticImageLoadImage ( lua_State* luaVM )
{
//  bool guiStaticImageLoadImage ( element theElement, string filename )
    CClientGUIElement* theElement; SString filename;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( theElement );
    argStream.ReadString ( filename );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( pLuaMain )
        {
            CResource* pResource =  pLuaMain->GetResource();
            SString strPath;
            if ( CResourceManager::ParseResourcePathInput( filename, pResource, strPath ) )
            {
                if ( CStaticFunctionDefinitions::GUIStaticImageLoadImage ( *theElement, strPath ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
                else
                    argStream.SetCustomError( SString( "Problem loading image '%s'", *strPath ) );
            }
            else
                argStream.SetCustomError( strPath, "Bad file path" );
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateTab ( lua_State* luaVM )
{
//  element guiCreateTab ( string text, element parent )
    SString text; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( text );
    argStream.ReadUserData ( parent );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateTab ( *pLuaMain, text, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetSelectedTab ( lua_State* luaVM )
{
//  element guiGetSelectedTab ( element tabPanel )
    CClientGUIElement* tabPanel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUITabPanel > ( tabPanel );

    if ( !argStream.HasErrors () )
    {
        CClientGUIElement* pTab = CStaticFunctionDefinitions::GUIGetSelectedTab ( *tabPanel );
        lua_pushelement ( luaVM, pTab );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetSelectedTab ( lua_State* luaVM )
{
//  bool guiSetSelectedTab ( element tabPanel, element theTab )
    CClientGUIElement* tabPanel; CClientGUIElement* theTab;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUITabPanel > ( tabPanel );
    argStream.ReadUserData < CGUITab > ( theTab );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GUISetSelectedTab ( *tabPanel, *theTab ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::GUIDeleteTab ( lua_State* luaVM )
{
//  bool guiDeleteTab ( element tabToDelete, element tabPanel )
    CClientGUIElement* tabToDelete; CClientGUIElement* tabPanel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUITab > ( tabToDelete );
    argStream.ReadUserData < CGUITabPanel > ( tabPanel );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            if ( CStaticFunctionDefinitions::GUIDeleteTab ( *pLuaMain, tabToDelete, tabPanel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetText ( lua_State* luaVM )
{
//  bool guiSetText ( element guiElement, string text )
    CClientGUIElement* guiElement; SString text;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );
    argStream.ReadString ( text );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUISetText ( *guiElement, text );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetFont ( lua_State* luaVM )
{
//  bool guiSetFont ( element guiElement, mixed font )
    CClientGUIElement* guiElement; SString strFontName; CClientGuiFont* pGuiFontElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );
    MixedReadGuiFontString ( argStream, strFontName, "default-normal", pGuiFontElement );

    if ( !argStream.HasErrors () )
    {
        if ( guiElement->SetFont ( strFontName, pGuiFontElement ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GUIBringToFront ( lua_State* luaVM )
{
//  bool guiBringToFront ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GUIBringToFront ( *guiElement ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIMoveToBack ( lua_State* luaVM )
{
//  bool guiMoveToBack( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIMoveToBack ( *guiElement );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIRadioButtonSetSelected ( lua_State* luaVM )
{
//  bool guiRadioButtonSetSelected ( element guiRadioButton, bool state )
    CClientGUIElement* guiRadioButton; bool state;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIRadioButton > ( guiRadioButton );
    argStream.ReadBool ( state );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIRadioButtonSetSelected ( *guiRadioButton, state );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICheckBoxSetSelected ( lua_State* luaVM )
{
//  bool guiCheckBoxSetSelected ( element theCheckbox, bool state )
    CClientGUIElement* theCheckbox; bool state;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUICheckBox > ( theCheckbox );
    argStream.ReadBool ( state );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUICheckBoxSetSelected ( *theCheckbox, state );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GUIRadioButtonGetSelected ( lua_State* luaVM )
{
//  bool guiRadioButtonGetSelected( element guiRadioButton )
    CClientGUIElement* guiRadioButton;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIRadioButton > ( guiRadioButton );

    if ( !argStream.HasErrors () )
    {
        bool bResult = static_cast < CGUIRadioButton* > ( guiRadioButton->GetCGUIElement () ) -> GetSelected ();
        lua_pushboolean ( luaVM, bResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICheckBoxGetSelected ( lua_State* luaVM )
{
//  bool guiCheckBoxGetSelected ( element theCheckbox )
    CClientGUIElement* theCheckbox;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUICheckBox > ( theCheckbox );

    if ( !argStream.HasErrors () )
    {
        bool bResult = static_cast < CGUICheckBox* > ( theCheckbox->GetCGUIElement () ) -> GetSelected ();
        lua_pushboolean ( luaVM, bResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIProgressBarSetProgress ( lua_State* luaVM )
{
//  bool guiProgressBarSetProgress ( progressBar theProgressbar, float progress )
    CClientGUIElement* theProgressbar; float progress;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIProgressBar > ( theProgressbar );
    argStream.ReadNumber ( progress );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIProgressBarSetProgress ( *theProgressbar, static_cast < int > ( progress ) );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollBarSetScrollPosition ( lua_State* luaVM )
{
//  bool guiScrollBarSetScrollPosition ( gui-scrollBar theScrollBar, float amount )
    CClientGUIElement* theScrollBar; float amount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIScrollBar > ( theScrollBar );
    argStream.ReadNumber ( amount );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIScrollBarSetScrollPosition ( *theScrollBar, static_cast < int > ( amount ) );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollPaneSetHorizontalScrollPosition ( lua_State* luaVM )
{
//  bool guiScrollPaneSetHorizontalScrollPosition ( gui-scrollPane theScrollPane, float amount )
    CClientGUIElement* theScrollPane; float amount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIScrollPane > ( theScrollPane );
    argStream.ReadNumber ( amount );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIScrollPaneSetHorizontalScrollPosition ( *theScrollPane, static_cast < int > ( amount ) );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollPaneSetVerticalScrollPosition ( lua_State* luaVM )
{
//  bool guiScrollPaneSetVerticalScrollPosition ( gui-scrollPane theScrollPane, float amount )
    CClientGUIElement* theScrollPane; float amount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIScrollPane > ( theScrollPane );
    argStream.ReadNumber ( amount );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIScrollPaneSetVerticalScrollPosition ( *theScrollPane, static_cast < int > ( amount ) );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIProgressBarGetProgress ( lua_State* luaVM )
{
//  float guiProgressBarGetProgress ( progressBar theProgressbar );
    CClientGUIElement* theProgressbar;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIProgressBar > ( theProgressbar );

    if ( !argStream.HasErrors () )
    {
        int iProgress = ( int ) ( static_cast < CGUIProgressBar* > ( theProgressbar->GetCGUIElement () ) -> GetProgress () * 100.0f + 0.5f );
        lua_pushnumber ( luaVM, iProgress );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollPaneGetHorizontalScrollPosition ( lua_State* luaVM )
{
//  float guiScrollPaneGetHorizontalScrollPosition ( gui-scrollPane theScrollPane  )
    CClientGUIElement* theScrollPane;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIScrollPane > ( theScrollPane );

    if ( !argStream.HasErrors () )
    {
        float fPos = static_cast < CGUIScrollPane* > ( theScrollPane->GetCGUIElement () ) -> GetHorizontalScrollPosition () * 100.0f;
        lua_pushnumber ( luaVM, fPos );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollPaneGetVerticalScrollPosition ( lua_State* luaVM )
{
//  float guiScrollPaneGetVerticalScrollPosition ( gui-scrollPane theScrollPane  )
    CClientGUIElement* theScrollPane;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIScrollPane > ( theScrollPane );

    if ( !argStream.HasErrors () )
    {
        float fPos = static_cast < CGUIScrollPane* > ( theScrollPane->GetCGUIElement () ) -> GetVerticalScrollPosition () * 100.0f;
        lua_pushnumber ( luaVM, fPos );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollBarGetScrollPosition ( lua_State* luaVM )
{
//  float guiScrollBarGetScrollPosition ( gui-scrollBar theScrollBar )
    CClientGUIElement* theScrollBar;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIScrollBar > ( theScrollBar );

    if ( !argStream.HasErrors () )
    {
        int iPos = ( int ) ( static_cast < CGUIScrollBar* > ( theScrollBar->GetCGUIElement () ) -> GetScrollPosition () * 100.0f );
        lua_pushnumber ( luaVM, iPos );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetText ( lua_State* luaVM )
{
//  string guiGetText ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );

    if ( !argStream.HasErrors () )
    {
        SString strText = guiElement->GetCGUIElement ()->GetText ();
        lua_pushstring ( luaVM, strText );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetFont ( lua_State* luaVM )
{
//  string,font guiGetFont ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );

    if ( !argStream.HasErrors () )
    {
        CClientGuiFont* pGuiFontElement;
        SString strFontName = guiElement->GetFont ( &pGuiFontElement );

        if ( strFontName != "" )
            lua_pushstring ( luaVM, strFontName );
        else
            lua_pushnil ( luaVM );
        lua_pushelement ( luaVM, pGuiFontElement );
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetSize ( lua_State* luaVM )
{
//  float float guiGetSize ( element theElement, bool relative )
    CClientGUIElement* theElement; bool relative;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( theElement );
    argStream.ReadBool ( relative );

    if ( !argStream.HasErrors () )
    {
        CVector2D Size;
        theElement->GetCGUIElement ()->GetSize ( Size, relative );

        lua_pushnumber ( luaVM, Size.fX );
        lua_pushnumber ( luaVM, Size.fY );
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetScreenSize ( lua_State* luaVM )
{
    const CVector2D Size = CStaticFunctionDefinitions::GUIGetScreenSize ();

    lua_pushnumber ( luaVM, Size.fX );
    lua_pushnumber ( luaVM, Size.fY );
    return 2;
}


int CLuaFunctionDefs::GUIGetPosition ( lua_State* luaVM )
{
//  float, float guiGetPosition ( element guiElement, bool relative )
    CClientGUIElement* guiElement; bool relative;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );
    argStream.ReadBool ( relative );

    if ( !argStream.HasErrors () )
    {
        CVector2D Pos;
        guiElement->GetCGUIElement ()->GetPosition ( Pos, relative );

        lua_pushnumber ( luaVM, Pos.fX );
        lua_pushnumber ( luaVM, Pos.fY );
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetAlpha ( lua_State* luaVM )
{
//  bool guiSetAlpha ( element guielement, float alpha )
    CClientGUIElement* guiElement; float alpha;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );
    argStream.ReadNumber ( alpha );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUISetAlpha ( *guiElement, Clamp ( 0.0f, alpha, 1.0f ) );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetAlpha ( lua_State* luaVM )
{
//  int guiGetAlpha ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );

    if ( !argStream.HasErrors () )
    {
        float fAlpha = guiElement->GetCGUIElement ()->GetAlpha ();
        lua_pushnumber ( luaVM, fAlpha );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetVisible ( lua_State* luaVM )
{
//  bool guiSetVisible ( element guiElement, bool state )
    CClientGUIElement* guiElement; bool state;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );
    argStream.ReadBool ( state );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUISetVisible ( *guiElement, state );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetEnabled ( lua_State* luaVM )
{
//  bool guiSetEnabled ( element guiElement, bool enabled )
    CClientGUIElement* guiElement; bool enabled;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );
    argStream.ReadBool ( enabled );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUISetEnabled ( *guiElement, enabled );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetProperty ( lua_State* luaVM )
{
//  bool guiSetProperty ( element guiElement, string property, string value )
    CClientGUIElement* guiElement; SString property; SString value;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );
    argStream.ReadString ( property );
    argStream.ReadString ( value );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUISetProperty ( *guiElement, property, value );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetVisible ( lua_State* luaVM )
{
//  bool guiGetVisible ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );

    if ( !argStream.HasErrors () )
    {
        bool bResult = guiElement->GetCGUIElement ()->IsVisible ();
        lua_pushboolean ( luaVM, bResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetEnabled ( lua_State* luaVM )
{
//  bool guiGetEnabled ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );

    if ( !argStream.HasErrors () )
    {
        bool bResult = guiElement->GetCGUIElement ()->IsEnabled ();
        lua_pushboolean ( luaVM, bResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetProperty ( lua_State* luaVM )
{
//  string guiGetProperty ( element guiElement, string property )
    CClientGUIElement* guiElement; SString property;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );
    argStream.ReadString ( property );

    if ( !argStream.HasErrors () )
    {
        SString strValue = guiElement->GetCGUIElement ()->GetProperty ( property );
        lua_pushstring ( luaVM, strValue );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetProperties ( lua_State* luaVM )
{
//  table guiGetProperties ( element guiElement )
    CClientGUIElement* guiElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );

    if ( !argStream.HasErrors () )
    {
        // Create a new table
        lua_newtable ( luaVM );

        // Add all our properties to the table on top of the given lua main's stack
        unsigned int uiIndex = 0;
        CGUIPropertyIter iter = guiElement->GetCGUIElement ()->GetPropertiesBegin ();
        CGUIPropertyIter iterEnd = guiElement->GetCGUIElement ()->GetPropertiesEnd ();
        for ( ; iter != iterEnd; iter++ )
        {
            const char * szKey = (*iter)->strKey;
            const char * szValue = (*iter)->strValue;

            // Add it to the table
            lua_pushstring ( luaVM, szKey );
            lua_pushstring ( luaVM, szValue );
            lua_settable ( luaVM, -3 );
        }

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetSize ( lua_State* luaVM )
{
//  bool guiSetSize ( element guiElement, float width, float height, bool relative )
    CClientGUIElement* guiElement; float width; float height; bool relative;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadBool ( relative );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUISetSize ( *guiElement, CVector2D ( width, height ), relative );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetPosition ( lua_State* luaVM )
{
//  bool guiSetPosition ( element guiElement, float x, float y, bool relative )
    CClientGUIElement* guiElement; float x; float y; bool relative;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( guiElement );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadBool ( relative );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUISetPosition ( *guiElement, CVector2D ( x, y ), relative );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetSortingEnabled ( lua_State* luaVM )
{
//  bool guiGridListSetSortingEnabled ( element guiGridlist, bool enabled )
    CClientGUIElement* guiGridlist; bool enabled;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadBool ( enabled );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIGridListSetSortingEnabled ( *guiGridlist, enabled );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListAddColumn ( lua_State* luaVM )
{
//  int guiGridListAddColumn ( element gridList, string title, float width )
    CClientGUIElement* guiGridlist; SString title; float width;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadString ( title );
    argStream.ReadNumber ( width );

    if ( !argStream.HasErrors () )
    {
        uint id = CStaticFunctionDefinitions::GUIGridListAddColumn ( *guiGridlist, title, width );
        lua_pushnumber ( luaVM, id );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListRemoveColumn ( lua_State* luaVM )
{
//  bool guiGridListRemoveColumn ( element guiGridlist, int columnIndex )
    CClientGUIElement* guiGridlist; int columnIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( columnIndex );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIGridListRemoveColumn ( *guiGridlist, columnIndex );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetColumnWidth ( lua_State* luaVM )
{
//  bool guiGridListSetColumnWidth ( element gridList, int columnIndex, number width, bool relative )
    CClientGUIElement* guiGridlist; int columnIndex; float width; bool relative;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( columnIndex );
    argStream.ReadNumber ( width );
    argStream.ReadBool ( relative );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIGridListSetColumnWidth ( *guiGridlist, columnIndex, width, relative );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetColumnTitle ( lua_State* luaVM )
{
//  bool guiGridListSetColumnTitle ( element guiGridlist, int columnIndex, string title )
    CClientGUIElement* guiGridlist; int iColumnIndex; SString sTitle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( iColumnIndex );
    argStream.ReadString ( sTitle );
       
    if ( !argStream.HasErrors () )
    {
        int iColumnCount = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) ->GetColumnCount ();
        if ( iColumnIndex > 0 && iColumnCount >= iColumnIndex )
        {
            CStaticFunctionDefinitions::GUIGridListSetColumnTitle ( *guiGridlist, iColumnIndex, sTitle );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetColumnTitle ( lua_State* luaVM )
{
//  string guiGridListGetColumnTitle ( element guiGridlist, int columnIndex )
    CClientGUIElement* guiGridlist; int iColumnIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( iColumnIndex );
       
    if ( !argStream.HasErrors () )
    {
        int iColumnCount = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) ->GetColumnCount ();
        if ( iColumnIndex > 0 && iColumnCount >= iColumnIndex )
        {
            const char* szTitle = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) ->GetColumnTitle ( iColumnIndex );
            lua_pushstring ( luaVM, szTitle );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListAddRow ( lua_State* luaVM )
{
//  int guiGridListAddRow ( element gridList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );

    if ( !argStream.HasErrors () )
    {
        int iRet = CStaticFunctionDefinitions::GUIGridListAddRow ( *guiGridlist, true );
        if ( iRet >= 0 )
        {
            m_pGUIManager->QueueGridListUpdate ( guiGridlist );
            lua_pushnumber ( luaVM, iRet );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListInsertRowAfter ( lua_State* luaVM )
{
//  int guiGridListInsertRowAfter ( element gridList, int rowIndex )
    CClientGUIElement* guiGridlist; int rowIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( rowIndex );

    if ( !argStream.HasErrors () )
    {
        int iRet = CStaticFunctionDefinitions::GUIGridListInsertRowAfter ( *guiGridlist, rowIndex );
        if ( iRet >= 0 ) 
        {
            lua_pushnumber ( luaVM, iRet );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListAutoSizeColumn ( lua_State* luaVM )
{
//  bool guiGridListAutoSizeColumn ( element gridList, int columnIndex )
    CClientGUIElement* guiGridlist; int columnIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( columnIndex );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIGridListAutoSizeColumn ( *guiGridlist, columnIndex );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListClear ( lua_State* luaVM )
{
//  bool guiGridListClear ( element gridList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIGridListClear ( *guiGridlist );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetSelectionMode ( lua_State* luaVM )
{
//  bool guiGridListSetSelectionMode ( guiElement gridlist, int mode )
    CClientGUIElement* guiGridlist; int mode;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( mode );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIGridListSetSelectionMode ( *guiGridlist, mode );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetSelectedItem ( lua_State* luaVM )
{
//  int, int guiGridListGetSelectedItem ( element gridList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );

    if ( !argStream.HasErrors () )
    {
        int iRow = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetSelectedItemRow ();
        int iColumn = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetSelectedItemColumn ();
        lua_pushnumber ( luaVM, iRow );
        lua_pushnumber ( luaVM, iColumn + 1 );  // columns start at 1
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetSelectedItems ( lua_State* luaVM )
{
//  table guiGridListGetSelectedItems ( element gridList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );

    if ( !argStream.HasErrors () )
    {
        CGUIGridList* pList = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () );
        CGUIListItem* pItem = NULL;

        lua_newtable ( luaVM );

        for ( int i = 1; i <= pList->GetSelectedCount(); i++ )
        {
            pItem = pList->GetNextSelectedItem ( pItem );
            if ( !pItem ) break;

            lua_pushnumber ( luaVM, i );
            lua_newtable ( luaVM );

            // column
            lua_pushstring ( luaVM, "column" );
            lua_pushnumber ( luaVM, pList->GetItemColumnIndex ( pItem ) );
            lua_settable ( luaVM, -3 );

            // row
            lua_pushstring ( luaVM, "row" );
            lua_pushnumber ( luaVM, pList->GetItemRowIndex ( pItem ) );
            lua_settable ( luaVM, -3 );

            // push to main table
            lua_settable ( luaVM, -3 );
        }

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetSelectedCount ( lua_State* luaVM )
{
//  int guiGridListGetSelectedCount ( element gridList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );

    if ( !argStream.HasErrors () )
    {
        int iCount = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetSelectedCount ();
        lua_pushnumber ( luaVM, iCount );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetSelectedItem ( lua_State* luaVM )
{
//  bool guiGridListSetSelectedItem ( element gridList, int rowIndex, int columnIndex )
    CClientGUIElement* guiGridlist; int rowIndex; int columnIndex; bool bReset;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( rowIndex );
    argStream.ReadNumber ( columnIndex );
    argStream.ReadBool ( bReset, true );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIGridListSetSelectedItem ( *guiGridlist, rowIndex, columnIndex, bReset );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListRemoveRow ( lua_State* luaVM )
{
//  bool guiGridListRemoveRow ( element gridList, int rowIndex )
    CClientGUIElement* guiGridlist; int rowIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( rowIndex );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIGridListRemoveRow ( *guiGridlist, rowIndex );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetItemText ( lua_State* luaVM )
{
//  string guiGridListGetItemText ( element gridList, int rowIndex, int columnIndex )
    CClientGUIElement* guiGridlist; int rowIndex; int columnIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( rowIndex );
    argStream.ReadNumber ( columnIndex );

    if ( !argStream.HasErrors () )
    {
        const char* szText = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetItemText ( rowIndex, columnIndex );
        lua_pushstring ( luaVM, szText );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetItemData ( lua_State* luaVM )
{
//  string guiGridListGetItemData ( element gridList, int rowIndex, int columnIndex )
    CClientGUIElement* guiGridlist; int rowIndex; int columnIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( rowIndex );
    argStream.ReadNumber ( columnIndex );

    if ( !argStream.HasErrors () )
    {
        void* pData = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetItemData ( rowIndex, columnIndex );
        CLuaArgument* pVariable = reinterpret_cast < CLuaArgument* > ( pData );
        if ( pVariable )
            pVariable->Push(luaVM);
        else
            lua_pushnil ( luaVM );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetItemColor ( lua_State* luaVM )
{
//  int int int int guiGridListGetItemColor ( element gridList, int rowIndex, int columnIndex )
    CClientGUIElement* guiGridlist; int rowIndex; int columnIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( rowIndex );
    argStream.ReadNumber ( columnIndex );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucRed = 255, ucGreen = 255, ucBlue = 255, ucAlpha = 255;
        if ( static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetItemColor ( rowIndex, columnIndex, ucRed, ucGreen, ucBlue, ucAlpha ) )
        {
            lua_pushnumber ( luaVM, ucRed );
            lua_pushnumber ( luaVM, ucGreen );
            lua_pushnumber ( luaVM, ucBlue );
            lua_pushnumber ( luaVM, ucAlpha );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetItemText ( lua_State* luaVM )
{
//  bool guiGridListSetItemText ( element gridList, int rowIndex, int columnIndex, string text, bool section, bool number )
    CClientGUIElement* guiGridlist; int rowIndex; int columnIndex; SString text; bool section; bool number;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( rowIndex );
    argStream.ReadNumber ( columnIndex );
    argStream.ReadString ( text );
    argStream.ReadBool ( section );
    argStream.ReadBool ( number );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIGridListSetItemText ( *guiGridlist, rowIndex, columnIndex, text, section, number, true );
        m_pGUIManager->QueueGridListUpdate ( guiGridlist );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetItemData ( lua_State* luaVM )
{
//  bool guiGridListSetItemData ( element gridList, int rowIndex, int columnIndex, string data )
    CClientGUIElement* guiGridlist; int rowIndex; int columnIndex; CLuaArgument data;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( rowIndex );
    argStream.ReadNumber ( columnIndex );
    argStream.ReadLuaArgument ( data );

    if ( !argStream.HasErrors () )
    {
        CLuaArgument* pData = new CLuaArgument ( data );
        CStaticFunctionDefinitions::GUIGridListSetItemData ( *guiGridlist, rowIndex, columnIndex, pData );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetItemColor ( lua_State* luaVM )
{
//  bool guiGridListSetItemColor ( element gridList, int rowIndex, int columnIndex, int red, int green, int blue[, int alpha = 255 ] )
    CClientGUIElement* guiGridlist; int rowIndex; int columnIndex; int red; int green; int blue; int alpha;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadNumber ( rowIndex );
    argStream.ReadNumber ( columnIndex );
    argStream.ReadNumber ( red );
    argStream.ReadNumber ( green );
    argStream.ReadNumber ( blue );
    argStream.ReadNumber ( alpha, 255 );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIGridListSetItemColor( *guiGridlist, rowIndex, columnIndex, red, green, blue, alpha );
        m_pGUIManager->QueueGridListUpdate ( guiGridlist );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetScrollBars ( lua_State* luaVM )
{
//  bool guiGridListSetScrollBars ( element guiGridlist, bool horizontalBar, bool verticalBar )
    CClientGUIElement* guiGridlist; bool horizontalBar; bool verticalBar;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );
    argStream.ReadBool ( horizontalBar );
    argStream.ReadBool ( verticalBar );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIGridListSetScrollBars ( *guiGridlist, horizontalBar, verticalBar );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollPaneSetScrollBars ( lua_State* luaVM )
{
//  bool guiScrollPaneSetScrollBars ( element scrollPane, bool horizontal, bool vertical )
    CClientGUIElement* scrollPane; bool horizontalBar; bool verticalBar;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIScrollPane > ( scrollPane );
    argStream.ReadBool ( horizontalBar );
    argStream.ReadBool ( verticalBar );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIScrollPaneSetScrollBars ( *scrollPane, horizontalBar, verticalBar );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetRowCount ( lua_State* luaVM )
{
//  int guiGridListGetRowCount ( element theList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );

    if ( !argStream.HasErrors () )
    {
        int iRowCount = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetRowCount ();
        lua_pushnumber ( luaVM, iRowCount );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetColumnCount ( lua_State* luaVM )
{
//  int guiGridListGetColumnCount ( element gridList )
    CClientGUIElement* guiGridlist;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIGridList > ( guiGridlist );

    if ( !argStream.HasErrors () )
    {
        int iColumnCount = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetColumnCount ();
        lua_pushnumber ( luaVM, iColumnCount );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIEditSetReadOnly ( lua_State* luaVM )
{
//  bool guiEditSetReadOnly ( element editField, bool status )
    CClientGUIElement* editField; bool status;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIEdit > ( editField );
    argStream.ReadBool ( status );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIEditSetReadOnly ( *editField, status );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIMemoSetReadOnly ( lua_State* luaVM )
{
//  bool guiMemoSetReadOnly ( gui-memo theMemo, bool status )
    CClientGUIElement* theMemo; bool status;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIMemo > ( theMemo );
    argStream.ReadBool ( status );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIMemoSetReadOnly ( *theMemo, status );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIEditSetMasked ( lua_State* luaVM )
{
//  bool guiEditSetMasked ( element theElement, bool status )
    CClientGUIElement* theElement; bool status;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIEdit > ( theElement );
    argStream.ReadBool ( status );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIEditSetMasked ( *theElement, status );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIEditSetMaxLength ( lua_State* luaVM )
{
//  bool guiEditSetMaxLength ( element theElement, int length )
    CClientGUIElement* theElement; int length;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIEdit > ( theElement );
    argStream.ReadNumber ( length );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIEditSetMaxLength ( *theElement, length );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIEditSetCaratIndex ( lua_State* luaVM )
{
//  bool guiEditSetCaratIndex ( element theElement, int index )
    CClientGUIElement* theElement; int index;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIEdit > ( theElement );
    argStream.ReadNumber ( index );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIEditSetCaratIndex ( *theElement, index );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIMemoSetCaratIndex ( lua_State* luaVM )
{
//  bool guiMemoSetCaratIndex ( gui-memo theMemo, int index )
    CClientGUIElement* theMemo; int index;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIMemo > ( theMemo );
    argStream.ReadNumber ( index );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIMemoSetCaratIndex ( *theMemo, index );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIWindowSetMovable ( lua_State* luaVM )
{
//  bool guiWindowSetMovable ( element theElement, bool status )
    CClientGUIElement* theElement; bool status;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIWindow > ( theElement );
    argStream.ReadBool ( status );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIWindowSetMovable ( *theElement, status );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIWindowSetSizable ( lua_State* luaVM )
{
//  bool guiWindowSetSizable ( element theElement, bool status )
    CClientGUIElement* theElement; bool status;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIWindow > ( theElement );
    argStream.ReadBool ( status );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUIWindowSetSizable ( *theElement, status );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUILabelGetTextExtent ( lua_State* luaVM )
{
//  float guiLabelGetTextExtent ( element theLabel )
    CClientGUIElement* theLabel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUILabel > ( theLabel );

    if ( !argStream.HasErrors () )
    {
        float fExtent = static_cast < CGUILabel* > ( theLabel->GetCGUIElement () ) -> GetTextExtent ();
        lua_pushnumber ( luaVM, fExtent );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUILabelGetFontHeight ( lua_State* luaVM )
{
//  float guiLabelGetFontHeight ( element theLabel )
    CClientGUIElement* theLabel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUILabel > ( theLabel );

    if ( !argStream.HasErrors () )
    {
        float fHeight = static_cast < CGUILabel* > ( theLabel->GetCGUIElement () ) -> GetFontHeight ();
        lua_pushnumber ( luaVM, fHeight );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUILabelSetColor ( lua_State* luaVM )
{
//  bool guiLabelSetColor ( element theElement, int red, int green, int blue )
    CClientGUIElement* theElement; int red; int green; int blue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUILabel > ( theElement );
    argStream.ReadNumber ( red );
    argStream.ReadNumber ( green );
    argStream.ReadNumber ( blue );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUILabelSetColor ( *theElement, red, green, blue );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUILabelGetColor ( lua_State* luaVM )
{
//  int r, int g, int b guiLabelGetColor ( element theElement )
    CClientGUIElement* theElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUILabel > ( theElement );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucRed = 255, ucGreen = 255, ucBlue = 255;
        static_cast < CGUILabel* > ( theElement->GetCGUIElement () ) ->GetTextColor ( ucRed, ucGreen, ucBlue );
        lua_pushnumber ( luaVM, ucRed );
        lua_pushnumber ( luaVM, ucGreen );
        lua_pushnumber ( luaVM, ucBlue );
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUILabelSetVerticalAlign ( lua_State* luaVM )
{
//  bool guiLabelSetVerticalAlign ( element theLabel, string align )
    CClientGUIElement* theLabel; CGUIVerticalAlign align;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUILabel > ( theLabel );
    argStream.ReadEnumString ( align );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::GUILabelSetVerticalAlign ( *theLabel, align );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::GUILabelSetHorizontalAlign ( lua_State* luaVM )
{
//  bool guiLabelSetHorizontalAlign ( element theLabel, string align, [ bool wordwrap = false ] )
    CClientGUIElement* theLabel; CGUIHorizontalAlign align; bool wordwrap;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUILabel > ( theLabel );
    argStream.ReadEnumString ( align );
    argStream.ReadBool ( wordwrap, false );

    if ( !argStream.HasErrors () )
    {
        if ( wordwrap )
            align = (CGUIHorizontalAlign)(align + 4);
        CStaticFunctionDefinitions::GUILabelSetHorizontalAlign ( *theLabel, align );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetChatboxLayout ( lua_State* luaVM )
{
    //* chat_font - Returns the chatbox font
    //* chat_lines - Returns how many lines the chatbox has
    //* chat_color - Returns the background color of the chatbox
    //* chat_text_color - Returns the chatbox text color
    //* chat_input_color - Returns the background color of the chatbox input
    //* chat_input_prefix_color - Returns the color of the input prefix text
    //* chat_input_text_color - Returns the color of the text in the chatbox input
    //* chat_scale - Returns the scale of the text in the chatbox
    //* chat_width - Returns the scale of the background width
    //* chat_css_style_text - Returns whether text fades out over time
    //* chat_css_style_background - Returns whether the background fades out over time
    //* chat_line_life - Returns how long it takes for text to start fading out
    //* chat_line_fade_out - Returns how long takes for text to fade out
    //* chat_use_cegui - Returns whether CEGUI is used to render the chatbox
    //* text_scale - Returns text scale

    CCVarsInterface* pCVars = g_pCore->GetCVars ();
    float fNumber;
    pCVars->Get("chat_font", fNumber);
    lua_newtable ( luaVM );
    lua_pushnumber ( luaVM, fNumber );
    lua_setfield ( luaVM, -2, "chat_font" );
    pCVars->Get("chat_lines", fNumber);
    lua_pushnumber ( luaVM, fNumber );
    lua_setfield ( luaVM, -2, "chat_lines" );
    pCVars->Get("chat_width", fNumber);
    lua_pushnumber ( luaVM, fNumber );
    lua_setfield ( luaVM, -2, "chat_width" );
    pCVars->Get("chat_css_style_text", fNumber);
    lua_pushnumber ( luaVM, fNumber );
    lua_setfield ( luaVM, -2, "chat_css_style_text" );
    pCVars->Get("chat_css_style_background", fNumber);
    lua_pushnumber ( luaVM, fNumber );
    lua_setfield ( luaVM, -2, "chat_css_style_background" );
    pCVars->Get("chat_line_life", fNumber);
    lua_pushnumber ( luaVM, fNumber );
    lua_setfield ( luaVM, -2, "chat_line_life" );
    pCVars->Get("chat_line_fade_out", fNumber);
    lua_pushnumber ( luaVM, fNumber );
    lua_setfield ( luaVM, -2, "chat_line_fade_out" );
    pCVars->Get("text_scale", fNumber);
    lua_pushnumber ( luaVM, fNumber );
    lua_setfield ( luaVM, -2, "text_scale" );
    pCVars->Get("chat_use_cegui", fNumber);
    lua_pushboolean ( luaVM, fNumber ? true : false );
    lua_setfield ( luaVM, -2, "chat_use_cegui" );
    std::string strCVar;
    std::stringstream ss;
    int iR, iG, iB, iA;
    pCVars->Get("chat_color", strCVar);
    if ( !strCVar.empty() )
    {
        ss.str ( strCVar );
        ss >> iR >> iG >> iB >> iA;
        lua_newtable ( luaVM );
        lua_pushnumber ( luaVM, 1 );
        lua_pushnumber ( luaVM, iR );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 2 );
        lua_pushnumber ( luaVM, iG );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 3 );
        lua_pushnumber ( luaVM, iB );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 4 );
        lua_pushnumber ( luaVM, iA );
        lua_settable( luaVM, -3 );
        lua_setfield ( luaVM, -2, "chat_color" );
    }
    pCVars->Get("chat_text_color", strCVar);
    if ( !strCVar.empty() )
    {
        ss.clear();
        ss.str ( strCVar );
        ss >> iR >> iG >> iB >> iA;
        lua_newtable ( luaVM );
        lua_pushnumber ( luaVM, 1 );
        lua_pushnumber ( luaVM, iR );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 2 );
        lua_pushnumber ( luaVM, iG );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 3 );
        lua_pushnumber ( luaVM, iB );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 4 );
        lua_pushnumber ( luaVM, iA );
        lua_settable( luaVM, -3 );
        lua_setfield ( luaVM, -2, "chat_text_color" );
    }
    pCVars->Get("chat_input_color", strCVar);
    if ( !strCVar.empty() )
    {
        ss.clear();
        ss.str ( strCVar );
        ss >> iR >> iG >> iB >> iA;
        lua_newtable ( luaVM );
        lua_pushnumber ( luaVM, 1 );
        lua_pushnumber ( luaVM, iR );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 2 );
        lua_pushnumber ( luaVM, iG );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 3 );
        lua_pushnumber ( luaVM, iB );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 4 );
        lua_pushnumber ( luaVM, iA );
        lua_settable( luaVM, -3 );
        lua_setfield ( luaVM, -2, "chat_input_color" );
    }
    pCVars->Get("chat_input_prefix_color", strCVar);
    if ( !strCVar.empty() )
    {
        ss.clear();
        ss.str ( strCVar );
        ss >> iR >> iG >> iB >> iA;
        lua_newtable ( luaVM );
        lua_pushnumber ( luaVM, 1 );
        lua_pushnumber ( luaVM, iR );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 2 );
        lua_pushnumber ( luaVM, iG );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 3 );
        lua_pushnumber ( luaVM, iB );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 4 );
        lua_pushnumber ( luaVM, iA );
        lua_settable( luaVM, -3 );
        lua_setfield ( luaVM, -2, "chat_input_prefix_color" );
    }
    pCVars->Get("chat_input_text_color", strCVar);
    if ( !strCVar.empty() )
    {
        ss.clear();
        ss.str ( strCVar );
        ss >> iR >> iG >> iB >> iA;
        lua_newtable ( luaVM );
        lua_pushnumber ( luaVM, 1 );
        lua_pushnumber ( luaVM, iR );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 2 );
        lua_pushnumber ( luaVM, iG );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 3 );
        lua_pushnumber ( luaVM, iB );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 4 );
        lua_pushnumber ( luaVM, iA );
        lua_settable( luaVM, -3 );
        lua_setfield ( luaVM, -2, "chat_input_text_color" );
    }
    pCVars->Get("chat_scale", strCVar);
    if ( !strCVar.empty() )
    {
        float fX, fY;
        ss.clear();
        ss.str ( strCVar );
        ss >> fX >> fY;
        lua_newtable ( luaVM );
        lua_pushnumber ( luaVM, 1 );
        lua_pushnumber ( luaVM, fX );
        lua_settable( luaVM, -3 );
        lua_pushnumber ( luaVM, 2 );
        lua_pushnumber ( luaVM, fY );
        lua_settable( luaVM, -3 );
        lua_setfield ( luaVM, -2, "chat_scale" );
    }
    return 1;
}

int CLuaFunctionDefs::GUICreateComboBox ( lua_State* luaVM )
{
//  element guiCreateComboBox ( float x, float y, float width, float height, string caption, bool relative, [ element parent = nil ] )
    float x; float y; float width; float height; SString caption; bool relative; CClientGUIElement* parent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( x );
    argStream.ReadNumber ( y );
    argStream.ReadNumber ( width );
    argStream.ReadNumber ( height );
    argStream.ReadString ( caption );
    argStream.ReadBool ( relative );
    argStream.ReadUserData ( parent, NULL );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateComboBox ( *pLuaMain, x, y, width, height, caption, relative, parent );
            lua_pushelement ( luaVM, pGUIElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GUIComboBoxAddItem ( lua_State* luaVM )
{
    // int guiComboBoxAddItem( element comboBox, string value )
    CClientGUIElement* comboBox; SString value;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIComboBox > ( comboBox );
    argStream.ReadString ( value );

    if ( !argStream.HasErrors () )
    {
        int newId = CStaticFunctionDefinitions::GUIComboBoxAddItem ( *comboBox, value );
        lua_pushnumber( luaVM, newId );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GUIComboBoxRemoveItem ( lua_State* luaVM )
{
//  bool guiComboBoxRemoveItem( element comboBox, int itemId )
    CClientGUIElement* comboBox; int itemId;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIComboBox > ( comboBox );
    argStream.ReadNumber ( itemId );

    if ( !argStream.HasErrors () )
    {
        bool ret = CStaticFunctionDefinitions::GUIComboBoxRemoveItem ( *comboBox, itemId );
        lua_pushboolean( luaVM, ret );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GUIComboBoxClear ( lua_State* luaVM )
{
//  bool guiComboBoxClear ( element comboBox )
    CClientGUIElement* comboBox;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIComboBox > ( comboBox );

    if ( !argStream.HasErrors () )
    {
        bool ret = CStaticFunctionDefinitions::GUIComboBoxClear ( *comboBox );
        lua_pushboolean( luaVM, ret );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GUIComboBoxGetSelected ( lua_State* luaVM )
{
//  int guiComboBoxGetSelected ( element comboBox )
    CClientGUIElement* comboBox;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIComboBox > ( comboBox );

    if ( !argStream.HasErrors () )
    {
        int selected = CStaticFunctionDefinitions::GUIComboBoxGetSelected( *comboBox );
        lua_pushnumber ( luaVM, selected );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFunctionDefs::GUIComboBoxSetSelected ( lua_State* luaVM )
{
//  bool guiComboBoxSetSelected ( element comboBox, int itemIndex )
    CClientGUIElement* comboBox; int itemIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIComboBox > ( comboBox );
    argStream.ReadNumber ( itemIndex );

    if ( !argStream.HasErrors () )
    {
        bool ret = CStaticFunctionDefinitions::GUIComboBoxSetSelected( *comboBox, itemIndex );
        lua_pushboolean ( luaVM, ret );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GUIComboBoxGetItemText ( lua_State* luaVM )
{
//  string guiComboBoxGetItemText ( element comboBox, int itemId )
    CClientGUIElement* comboBox; int itemId;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIComboBox > ( comboBox );
    argStream.ReadNumber ( itemId );

    if ( !argStream.HasErrors () )
    {
        SString ret = CStaticFunctionDefinitions::GUIComboBoxGetItemText( *comboBox, itemId );
        lua_pushstring ( luaVM, ret );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GUIComboBoxSetItemText ( lua_State* luaVM )
{
//  bool guiComboBoxSetItemText ( element comboBox, int itemId, string text )
    CClientGUIElement* comboBox; int itemId; SString text;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData < CGUIComboBox > ( comboBox );
    argStream.ReadNumber ( itemId );
    argStream.ReadString ( text );

    if ( !argStream.HasErrors () )
    {
        bool ret = CStaticFunctionDefinitions::GUIComboBoxSetItemText( *comboBox, itemId, text );
        lua_pushboolean ( luaVM, ret );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateFont ( lua_State* luaVM )
{
//  element guiCreateFont( string filepath [, int size=9 ] )
    SString strFilePath; int iSize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strFilePath );
    argStream.ReadNumber ( iSize, 9 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pParentResource = pLuaMain->GetResource ();
            CResource* pFileResource = pParentResource;
            SString strPath, strMetaPath;
            if ( CResourceManager::ParseResourcePathInput( strFilePath, pFileResource, strPath, strMetaPath ) )
            {
                if ( FileExists ( strPath ) )
                {
                    SString strUniqueName = SString ( "%s*%s*%s", pParentResource->GetName (), pFileResource->GetName (), strMetaPath.c_str () ).Replace ( "\\", "/" );
                    CClientGuiFont* pGuiFont = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateGuiFont ( strPath, strUniqueName, iSize );
                    if ( pGuiFont )
                    {
                        // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                        pGuiFont->SetParent ( pParentResource->GetResourceDynamicEntity () );
                    }
                    lua_pushelement ( luaVM, pGuiFont );
                    return 1;
                }
                else
                    argStream.SetCustomError( strFilePath, "Bad file path" );
            }
            else
                argStream.SetCustomError( strFilePath, "Bad file path" );
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}
