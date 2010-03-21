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
    bool bRet = false;

    if ( lua_istype ( luaVM, 1, LUA_TBOOLEAN ) ) {
        CStaticFunctionDefinitions::GUISetInputEnabled ( lua_toboolean ( luaVM, 1 ) ? true : false );
        bRet = true;
    }	// else: error, bad arguments

    lua_pushboolean ( luaVM, bRet );
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
    lua_pushboolean ( luaVM, g_pCore->IsSettingsVisible () || g_pCore->IsMenuVisible () );
    return 1;
}


int CLuaFunctionDefs::GUIIsMTAWindowActive ( lua_State* luaVM )
{
    bool bActive = g_pCore->IsChatInputEnabled () ||
        g_pCore->IsSettingsVisible () ||
        g_pCore->IsMenuVisible () ||
        g_pCore->GetConsole ()->IsVisible () ||
        g_pClientGame->GetTransferBox ()->IsVisible ();

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
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TSTRING ) && lua_istype ( luaVM, 6, LUA_TBOOLEAN ) )
        {
            const char *szCaption = lua_tostring ( luaVM, 5 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateWindow (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                szCaption,
                lua_toboolean ( luaVM, 6 ) ? true : false
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateLabel ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TSTRING ) && lua_istype ( luaVM, 6, LUA_TBOOLEAN ) )
        {
            const char *szCaption = lua_tostring ( luaVM, 5 );
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 7 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateLabel (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                szCaption,
                lua_toboolean ( luaVM, 6 ) ? true : false,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateStaticImage ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TSTRING ) && lua_istype ( luaVM, 6, LUA_TBOOLEAN ) )
        {
            SString strFile = lua_tostring ( luaVM, 5 );
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 7 );

            CResource* pResource = pLuaMain->GetResource();
            SString strPath;
            if ( CResourceManager::ParseResourcePathInput( strFile, pResource, strPath ) )
            {
                CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateStaticImage (
                    *pLuaMain,
                    static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                    static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                    static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                    static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                    strPath,
                    lua_toboolean ( luaVM, 6 ) ? true : false,
                    pParent
                    );

                // Only proceed if we have a valid element (since GUICreateStaticImage can return a NULL pointer)
                if ( pGUIElement ) {
                    lua_pushelement ( luaVM, pGUIElement );
                    return 1;
                }
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateButton ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TSTRING ) && lua_istype ( luaVM, 6, LUA_TBOOLEAN ) )	// ACHTUNG: EVENTS!
        {
            const char *szCaption = lua_tostring ( luaVM, 5 );
            //const char *szOnClick = lua_istype ( luaVM, 8, LUA_TSTRING ) ? lua_tostring ( luaVM, 8 ) : NULL;
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 7 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateButton (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                szCaption,
                lua_toboolean ( luaVM, 6 ) ? true : false,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateProgressBar ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TBOOLEAN ) )
        {
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 6 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateProgressBar (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                lua_toboolean ( luaVM, 5 ) ? true : false,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateCheckBox ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TSTRING ) && lua_istype ( luaVM, 6, LUA_TBOOLEAN ) && 
            lua_istype ( luaVM, 7, LUA_TBOOLEAN ) )
        {
            const char *szCaption = lua_tostring ( luaVM, 5 );
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 8 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateCheckBox (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                szCaption,
                lua_toboolean ( luaVM, 6 ) ? true : false,
                lua_toboolean ( luaVM, 7 ) ? true : false,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateRadioButton ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TSTRING ) && lua_istype ( luaVM, 6, LUA_TBOOLEAN ) )
        {
            const char *szCaption = lua_tostring ( luaVM, 5 );
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 7 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateRadioButton (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                szCaption,
                lua_toboolean ( luaVM, 6 ) ? true : false,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateEdit ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TSTRING ) && lua_istype ( luaVM, 6, LUA_TBOOLEAN ) )
        {
            const char *szCaption = lua_tostring ( luaVM, 5 );
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 7 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateEdit (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                szCaption,
                lua_toboolean ( luaVM, 6 ) ? true : false,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateMemo ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TSTRING ) && lua_istype ( luaVM, 6, LUA_TBOOLEAN ) )
        {
            const char *szCaption = lua_tostring ( luaVM, 5 );
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 7 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateMemo (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                szCaption,
                lua_toboolean ( luaVM, 6 ) ? true : false,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateGridList ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TBOOLEAN ) )
        {
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 6 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateGridList (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                lua_toboolean ( luaVM, 5 ) ? true : false,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateScrollPane ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TBOOLEAN ) )
        {
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 6 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateScrollPane (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                lua_toboolean ( luaVM, 5 ) ? true : false,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateScrollBar ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TBOOLEAN ) && lua_istype ( luaVM, 6, LUA_TBOOLEAN ) )
        {
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 7 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateScrollBar (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                lua_toboolean ( luaVM, 5 ) ? true : false,
                lua_toboolean ( luaVM, 6 ) ? true : false,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICreateTabPanel ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
            lua_istype ( luaVM, 5, LUA_TBOOLEAN ) )
        {
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 6 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateTabPanel (
                *pLuaMain,
                static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                lua_toboolean ( luaVM, 5 ) ? true : false,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIStaticImageLoadImage ( lua_State* luaVM )
{
    bool bRet = false;
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
            lua_istype ( luaVM, 2, LUA_TSTRING ) )
        {
            CResource* pResource =  pLuaMain->GetResource();

            // sanitize the input
            SString strFile = lua_tostring ( luaVM, 2 );
            SString strPath;
            if ( CResourceManager::ParseResourcePathInput( strFile, pResource, strPath ) )
            {
                // and attempt to load the image
                CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
                if ( pEntity )
                {
                    bRet = CStaticFunctionDefinitions::GUIStaticImageLoadImage ( *pEntity, strPath );
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "guiStaticImageLoadImage", "gui-element", 1 );
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, bRet );
    return 1;
}


int CLuaFunctionDefs::GUICreateTab ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
            lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) )
        {
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 2 );

            const char *szCaption = lua_tostring ( luaVM, 1 );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateTab (
                *pLuaMain,
                szCaption,
                pParent
                );

            if ( pGUIElement ) {
                lua_pushelement ( luaVM, pGUIElement );
                return 1;
            }
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetSelectedTab ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientEntity* pPanel = lua_toelement ( luaVM, 1 );
        if ( pPanel )
        {
            CClientGUIElement* pTab = NULL;
            if ( pTab = CStaticFunctionDefinitions::GUIGetSelectedTab ( *pPanel ) )
            {
                lua_pushelement ( luaVM, pTab );
                return 1;
            }
            else
            {
                lua_pushnil ( luaVM );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGetSelectedTab", "gui-element", 1 );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetSelectedTab ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) )
    {
        CClientEntity* pPanel = lua_toelement ( luaVM, 1 );
        CClientEntity* pTab = lua_toelement ( luaVM, 2 );
        if ( pPanel )
        {
            if ( pTab )
            {
                if ( CStaticFunctionDefinitions::GUISetSelectedTab ( *pPanel, *pTab ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "guiSetSelectedTab", "gui-element", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiSetSelectedTab", "gui-element", 1 );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefs::GUIDeleteTab ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
            lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) )
        {
            CClientGUIElement* pTab = lua_toguielement ( luaVM, 1 );
            CClientGUIElement* pParent = lua_toguielement ( luaVM, 2 );
            if ( pTab && IS_CGUIELEMENT_TAB ( pTab ) )
            {
                if ( pParent && IS_CGUIELEMENT_TABPANEL ( pParent ) )
                {
                    CStaticFunctionDefinitions::GUIDeleteTab (
                        *pLuaMain,
                        pTab,
                        pParent
                        );

                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "guiDeleteTab", "gui-element", 2 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "guiDeleteTab", "gui-element", 1 );
        }
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetText ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        ( lua_istype ( luaVM, 2, LUA_TSTRING ) || lua_istype ( luaVM, 2, LUA_TNUMBER ) ) )
    {
        const char *szText = lua_tostring ( luaVM, 2 );
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUISetText ( *pEntity, szText );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiSetText", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetFont ( lua_State* luaVM )
{
    bool bResult = false;

    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TSTRING ) )
    {
        const char *szFont = lua_tostring ( luaVM, 2 );
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            bResult = CStaticFunctionDefinitions::GUISetFont ( *pEntity, szFont );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiSetFont", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, bResult );
    return 1;
}


int CLuaFunctionDefs::GUIBringToFront ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::GUIBringToFront ( *pEntity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiBringToFront", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIMoveToBack ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIMoveToBack ( *pEntity );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiMoveToBack", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIRadioButtonSetSelected ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIRadioButtonSetSelected ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiRadiaButtonSetSelected", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUICheckBoxSetSelected ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUICheckBoxSetSelected ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiCheckBoxSetSelected", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIRadioButtonGetSelected ( lua_State* luaVM )
{
    bool bRet = false;

    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_RADIOBUTTON ( pGUIElement ) )
        {
            bRet = static_cast < CGUIRadioButton* > ( pGUIElement->GetCGUIElement () ) -> GetSelected ();
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiRadiaButtonGetSelected", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, bRet );
    return 1;
}


int CLuaFunctionDefs::GUICheckBoxGetSelected ( lua_State* luaVM )
{
    bool bRet = false;

    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_CHECKBOX ( pGUIElement ) )
        {
            bRet = static_cast < CGUICheckBox* > ( pGUIElement->GetCGUIElement () ) -> GetSelected ();
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiCheckBoxGetSelected", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, bRet );
    return 1;
}


int CLuaFunctionDefs::GUIProgressBarSetProgress ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIProgressBarSetProgress ( *pEntity, static_cast < int > ( lua_tonumber ( luaVM, 2 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiProgressBarSetProgress", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollBarSetScrollPosition ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIScrollBarSetScrollPosition ( *pEntity, static_cast < int > ( lua_tonumber ( luaVM, 2 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiScrollBarSetScrollPosition", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollPaneSetHorizontalScrollPosition ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientEntity* pEntity = lua_toelement( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIScrollPaneSetHorizontalScrollPosition ( *pEntity, static_cast < int > ( lua_tonumber ( luaVM, 2 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiScrollPaneSetHorizontalScrollPosition", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollPaneSetVerticalScrollPosition ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIScrollPaneSetVerticalScrollPosition ( *pEntity, static_cast < int > ( lua_tonumber ( luaVM, 2 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiScrollPaneSetVerticalScrollPosition", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIProgressBarGetProgress ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_PROGRESSBAR ( pGUIElement ) )
        {
            int iProgress = ( int ) ( static_cast < CGUIProgressBar* > ( pGUIElement->GetCGUIElement () ) -> GetProgress () * 100.0f + 0.5f );
            lua_pushnumber ( luaVM, iProgress );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiProgressBarGetProgress", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollPaneGetHorizontalScrollPosition ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_SCROLLPANE ( pGUIElement ) )
        {
            float fPos = static_cast < CGUIScrollPane* > ( pGUIElement->GetCGUIElement () ) -> GetHorizontalScrollPosition () * 100.0f;
            lua_pushnumber ( luaVM, fPos );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiScrollPaneGetHorizontalScrollPosition", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollPaneGetVerticalScrollPosition ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_SCROLLPANE ( pGUIElement ) )
        {
            float fPos = static_cast < CGUIScrollPane* > ( pGUIElement->GetCGUIElement () ) -> GetVerticalScrollPosition () * 100.0f;
            lua_pushnumber ( luaVM, fPos );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiScrollPaneGetVerticalScrollPosition", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollBarGetScrollPosition ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_SCROLLBAR ( pGUIElement ) )
        {
            int iPos = ( int ) ( static_cast < CGUIScrollBar* > ( pGUIElement->GetCGUIElement () ) -> GetScrollPosition () * 100.0f );
            lua_pushnumber ( luaVM, iPos );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiScrollBarGetScrollPosition", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetText ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement )
        {
            std::string strText = pGUIElement->GetCGUIElement ()->GetText ();
            lua_pushstring ( luaVM, strText.c_str () );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGetText", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetFont ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement )
        {
            std::string strFont = pGUIElement->GetCGUIElement ()->GetFont ();

            if ( strFont != "" )
                lua_pushstring ( luaVM, strFont.c_str () );
            else
                lua_pushnil ( luaVM );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGetFont", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetSize ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement )
        {
            CVector2D Size;
            pGUIElement->GetCGUIElement ()->GetSize ( Size, ( lua_toboolean ( luaVM, 2 ) ) ? true:false );

            lua_pushnumber ( luaVM, Size.fX );
            lua_pushnumber ( luaVM, Size.fY );
            return 2;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGetSize", "gui-element", 1 );
    }

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
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement )
        {
            CVector2D Pos;
            pGUIElement->GetCGUIElement ()->GetPosition ( Pos, ( lua_toboolean ( luaVM, 2 ) ) ? true:false );

            lua_pushnumber ( luaVM, Pos.fX );
            lua_pushnumber ( luaVM, Pos.fY );
            return 2;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGetPosition", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetAlpha ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            float fAlpha = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );

            if ( fAlpha > 1.0f )
            {
                fAlpha = 1.0f;
            }
            else if ( fAlpha < 0.0f )
            {
                fAlpha = 0.0f;
            }

            CStaticFunctionDefinitions::GUISetAlpha ( *pEntity, fAlpha );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiSetAlpha", "gui-element", 1 );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetAlpha ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement )
        {
            float fAlpha = pGUIElement->GetCGUIElement ()->GetAlpha ();
            lua_pushnumber ( luaVM, fAlpha );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGetAlpha", "gui-element", 1 );
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetVisible ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && 
        lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUISetVisible ( *pEntity, ( lua_toboolean ( luaVM, 2 ) ? true : false ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiSetVisible", "gui-element", 1 );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetEnabled ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUISetEnabled ( *pEntity, ( lua_toboolean ( luaVM, 2 ) ? true : false ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiSetEnabled", "gui-element", 1 );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetProperty ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TSTRING ) &&
        lua_istype ( luaVM, 3, LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUISetProperty ( *pEntity, lua_tostring ( luaVM, 2 ), lua_tostring ( luaVM, 3 ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiSetProperty", "gui-element", 1 );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetVisible ( lua_State* luaVM )
{
    bool bRet = false;

    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement )
        {
            bRet = pGUIElement->GetCGUIElement ()->IsVisible ();
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGetVisible", "gui-element", 1 );
    }
    lua_pushboolean ( luaVM, bRet );
    return 1;
}


int CLuaFunctionDefs::GUIGetEnabled ( lua_State* luaVM )
{
    bool bRet = false;

    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement )
        {
            bRet = pGUIElement->GetCGUIElement ()-> IsEnabled ();
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGetEnabled", "gui-element", 1 );
    }

    lua_pushboolean ( luaVM, bRet );
    return 1;
}


int CLuaFunctionDefs::GUIGetProperty ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TSTRING ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement )
        {
            std::string strValue = pGUIElement->GetCGUIElement ()->GetProperty ( lua_tostring ( luaVM, 2 ) );

            lua_pushstring ( luaVM, strValue.c_str () );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGetProperty", "gui-element", 1 );
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetProperties ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement )
        {
            // Create a new table
            lua_newtable ( luaVM );

            // Add all our properties to the table on top of the given lua main's stack
            unsigned int uiIndex = 0;
            CGUIPropertyIter iter = pGUIElement->GetCGUIElement ()->GetPropertiesBegin ();
            CGUIPropertyIter iterEnd = pGUIElement->GetCGUIElement ()->GetPropertiesEnd ();
            for ( ; iter != iterEnd; iter++ )
            {
                char * szKey = (*iter)->szKey;
                char * szValue = (*iter)->szValue;

                // Add it to the table
                lua_pushstring ( luaVM, szKey );
                lua_pushstring ( luaVM, szValue );
                lua_settable ( luaVM, -3 );
            }

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGetProperties", "gui-element", 1 );
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetSize ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
        lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUISetSize (
                *pEntity, 
                CVector2D ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) ),
                lua_toboolean ( luaVM, 4 ) ? true : false
                );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiSetSize", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUISetPosition ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
        lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUISetPosition (
                *pEntity, 
                CVector2D ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) ),
                lua_toboolean ( luaVM, 4 ) ? true : false
                );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiSetPosition", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetSortingEnabled ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIGridListSetSortingEnabled ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListSetSortingEnabled", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListAddColumn ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TSTRING ) &&
        lua_istype ( luaVM, 3, LUA_TNUMBER ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            lua_pushnumber ( luaVM, CStaticFunctionDefinitions::GUIGridListAddColumn ( *pGUIElement, lua_tostring ( luaVM, 2 ), static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListAddColumn", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListRemoveColumn ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            CStaticFunctionDefinitions::GUIGridListRemoveColumn ( *pGUIElement, static_cast < int > ( lua_tonumber ( luaVM, 2 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListRemoveColumn", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GUIGridListSetColumnWidth ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
        lua_istype ( luaVM, 3, LUA_TNUMBER ) &&
        lua_istype ( luaVM, 4, LUA_TBOOLEAN ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            CStaticFunctionDefinitions::GUIGridListSetColumnWidth ( *pGUIElement, (static_cast < int > ( lua_tonumber ( luaVM, 2 ) ) ) -1, static_cast < float > ( lua_tonumber ( luaVM, 3 ) ), lua_toboolean ( luaVM, 4 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListRemoveColumn", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListAddRow ( lua_State* luaVM )
{
    int iRet;

    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            iRet = CStaticFunctionDefinitions::GUIGridListAddRow ( *pGUIElement, true );
            if ( iRet >= 0 ) {
                m_pGUIManager->DeferGridListUpdate ( pGUIElement );
                lua_pushnumber ( luaVM, iRet );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListAddRow", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListInsertRowAfter ( lua_State* luaVM )
{
    int iRet;

    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            iRet = CStaticFunctionDefinitions::GUIGridListInsertRowAfter ( *pGUIElement, static_cast < int > ( lua_tonumber ( luaVM, 2 ) ) );
            if ( iRet >= 0 ) {
                lua_pushnumber ( luaVM, iRet );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListInsertRowAfter", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListAutoSizeColumn ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            CStaticFunctionDefinitions::GUIGridListAutoSizeColumn ( *pGUIElement, static_cast < int > ( lua_tonumber ( luaVM, 2 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListAutoSizeColumn", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListClear ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIGridListClear ( *pEntity );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListClear", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetSelectionMode ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIGridListSetSelectionMode ( *pEntity, static_cast < int > ( lua_tonumber ( luaVM, 2 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListSetSelectionMode", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetSelectedItem ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            int iRow = static_cast < CGUIGridList* > ( pGUIElement->GetCGUIElement () ) -> GetSelectedItemRow ();
            int iColumn = static_cast < CGUIGridList* > ( pGUIElement->GetCGUIElement () ) -> GetSelectedItemColumn ();

            // columns need to start at 1
            iColumn++;

            lua_pushnumber ( luaVM, iRow );
            lua_pushnumber ( luaVM, iColumn );
            return 2;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListGetSelectedItem", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetSelectedItems ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            CGUIGridList* pList = static_cast < CGUIGridList* > ( pGUIElement->GetCGUIElement () );
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListGetSelectedItems", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetSelectedCount ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            int iCount = static_cast < CGUIGridList* > ( pGUIElement->GetCGUIElement () ) -> GetSelectedCount ();

            lua_pushnumber ( luaVM, iCount );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListGetSelectedCount", "gui-element", 1 );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetSelectedItem ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
        lua_istype ( luaVM, 3, LUA_TNUMBER ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            CStaticFunctionDefinitions::GUIGridListSetSelectedItem ( *pGUIElement, static_cast < int > ( lua_tonumber ( luaVM, 2 ) ), static_cast < int > ( lua_tonumber ( luaVM, 3 ) ), true );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListSetSelectedItem", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListRemoveRow ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            CStaticFunctionDefinitions::GUIGridListRemoveRow ( *pGUIElement, static_cast < int > ( lua_tonumber ( luaVM, 2 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListRemoveRow", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetItemText ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
         lua_istype ( luaVM, 3, LUA_TNUMBER ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            const char* szText = static_cast < CGUIGridList* > ( pGUIElement->GetCGUIElement () ) -> GetItemText ( static_cast < int > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < int > ( lua_tonumber ( luaVM, 3 ) ) );
            if ( szText )
                lua_pushstring ( luaVM, szText );
            else
                lua_pushnil ( luaVM );

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListGetItemText", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetItemData ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
         lua_istype ( luaVM, 3, LUA_TNUMBER ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            const char* szData = reinterpret_cast < const char* > (
                static_cast < CGUIGridList* > ( pGUIElement->GetCGUIElement () ) -> GetItemData (
                    static_cast < int > ( lua_tonumber ( luaVM, 2 ) ), 
                    static_cast < int > ( lua_tonumber ( luaVM, 3 ) )
                )
            );
            if ( szData )
                lua_pushstring ( luaVM, szData );
            else
                lua_pushnil ( luaVM );

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListGetItemData", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetItemColor ( lua_State* luaVM )
{
    if ( lua_isuserdata ( luaVM, 1 ) && lua_isnumber ( luaVM, 2 ) && lua_isnumber ( luaVM, 3 ) )
    {
        CClientGUIElement* pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement )
        {
            unsigned char ucRed = 255, ucGreen = 255, ucBlue = 255, ucAlpha = 255;
            if ( reinterpret_cast < const char* > ( static_cast < CGUIGridList* > ( pGUIElement->GetCGUIElement () ) -> GetItemColor ( lua_tointeger ( luaVM, 2 ), lua_tointeger ( luaVM, 3 ), ucRed, ucGreen, ucBlue, ucAlpha ) ) )
            {
                lua_pushnumber ( luaVM, ucRed );
                lua_pushnumber ( luaVM, ucGreen );
                lua_pushnumber ( luaVM, ucBlue );
                lua_pushnumber ( luaVM, ucAlpha );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListGetItemColor", "gui-element", 1 );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetItemText ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
         lua_istype ( luaVM, 3, LUA_TNUMBER ) &&
         lua_istype ( luaVM, 4, LUA_TSTRING ) &&
         lua_istype ( luaVM, 5, LUA_TBOOLEAN ) &&
         lua_istype ( luaVM, 6, LUA_TBOOLEAN ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            CStaticFunctionDefinitions::GUIGridListSetItemText (
                *pGUIElement,
                static_cast < int > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < int > ( lua_tonumber ( luaVM, 3 ) ),
                lua_tostring ( luaVM, 4 ),
                lua_toboolean ( luaVM, 5 ) ? true : false,
                lua_toboolean ( luaVM, 6 ) ? true : false,
                true
            );
            m_pGUIManager->DeferGridListUpdate ( pGUIElement );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListSetItemText", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetItemData ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
         lua_istype ( luaVM, 3, LUA_TNUMBER ) &&
         lua_istype ( luaVM, 4, LUA_TSTRING ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            CStaticFunctionDefinitions::GUIGridListSetItemData (
                *pGUIElement,
                static_cast < int > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < int > ( lua_tonumber ( luaVM, 3 ) ),
                lua_tostring ( luaVM, 4 )
            );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListSetItemData", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetItemColor ( lua_State* luaVM )
{
    if ( lua_isuserdata ( luaVM, 1 ) && lua_isnumber ( luaVM, 2 ) && lua_isnumber ( luaVM, 3 ) && lua_isnumber ( luaVM, 4 ) && lua_isnumber ( luaVM, 5 ) && lua_isnumber ( luaVM, 6 ) )
    {
        CClientGUIElement* pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement )
        {
            int iAlpha = 255;
            if ( lua_isnumber ( luaVM, 7 ) )
            {
                iAlpha = lua_tointeger ( luaVM, 7 );
            }
            CStaticFunctionDefinitions::GUIGridListSetItemColor( *pGUIElement, lua_tointeger ( luaVM, 2 ), lua_tointeger ( luaVM, 3 ), lua_tointeger ( luaVM, 4 ), lua_tointeger ( luaVM, 5 ), lua_tointeger ( luaVM, 6 ), iAlpha );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListSetItemColor", "gui-element", 1 );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListSetScrollBars ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) && lua_istype ( luaVM, 3, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIGridListSetScrollBars ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false, lua_toboolean ( luaVM, 3 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListSetScrollBars", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIScrollPaneSetScrollBars ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) && lua_istype ( luaVM, 3, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIScrollPaneSetScrollBars ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false, lua_toboolean ( luaVM, 3 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiScrollPaneSetScrollBars", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGridListGetRowCount ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {		
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            int iRowCount = static_cast < CGUIGridList* > ( pGUIElement->GetCGUIElement () ) -> GetRowCount ();
            lua_pushnumber ( luaVM, iRowCount );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiGridListGetRowCount", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIEditSetReadOnly ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIEditSetReadOnly ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiEditSetReadOnly", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIMemoSetReadOnly ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIMemoSetReadOnly ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiMemoSetReadOnly", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIEditSetMasked ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIEditSetMasked ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiEditSetMasked", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIEditSetMaxLength ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIEditSetMaxLength ( *pEntity, static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiEditSetMaxLength", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIEditSetCaratIndex ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientEntity* pEntity  = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIEditSetCaratIndex ( *pEntity, static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiEditSetCaratIndex", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIMemoSetCaratIndex ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientEntity* pEntity  = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIMemoSetCaratIndex ( *pEntity, static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiMemoSetCaratIndex", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIWindowSetMovable ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIWindowSetMovable ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiWindowSetMovable", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIWindowSetSizable ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIWindowSetSizable ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiWindowSetSizable", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIWindowSetCloseButtonEnabled ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIWindowSetCloseButtonEnabled ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiWindowSetCloseButtonEnabled", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIWindowSetTitleBarEnabled ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIWindowSetTitleBarEnabled ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true : false );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiWindowSetTitleBarEnabled", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUILabelGetTextExtent ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_LABEL ( pGUIElement ) )
        {
            float fExtent = static_cast < CGUILabel* > ( pGUIElement->GetCGUIElement () ) -> GetTextExtent ();
            lua_pushnumber ( luaVM, fExtent );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiLabelGetTextExtent", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUILabelGetFontHeight ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_LABEL ( pGUIElement ) )
        {
            float fHeight = static_cast < CGUILabel* > ( pGUIElement->GetCGUIElement () ) -> GetFontHeight ();
            lua_pushnumber ( luaVM, fHeight );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiLabelGetFontHeight", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUILabelSetColor ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
        lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUILabelSetColor (	*pEntity,
                static_cast < int > ( lua_tonumber ( luaVM, 2 ) ),
                static_cast < int > ( lua_tonumber ( luaVM, 3 ) ),
                static_cast < int > ( lua_tonumber ( luaVM, 4 ) ) );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiLabelSetColor", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUILabelSetVerticalAlign ( lua_State* luaVM )
{
    // Verify the argument types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TSTRING ) )
    {
        // Grab the align string
        const char* szAlign = lua_tostring ( luaVM, 2 );

        // Compare it with the possible modes and set the mode
        CGUIVerticalAlign Align = CGUI_ALIGN_TOP;
        if ( stricmp ( szAlign, "top" ) == 0 )
        {
            Align = CGUI_ALIGN_TOP;
        }
        else if ( stricmp ( szAlign, "center" ) == 0 )
        {
            Align = CGUI_ALIGN_VERTICALCENTER;
        }
        else if ( stricmp ( szAlign, "bottom" ) == 0 )
        {
            Align = CGUI_ALIGN_BOTTOM;
        }
        else
        {
            // Bad align string
            CLogger::ErrorPrintf ( "%s", "No such align for guiLabelSetVerticalAlign" );
            lua_pushboolean ( luaVM, false );
            return 0;
        }

        // Grab and verify the GUI element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CStaticFunctionDefinitions::GUILabelSetVerticalAlign ( *pEntity, Align );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiLabelSetVerticalAlign", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUILabelSetHorizontalAlign ( lua_State* luaVM )
{
    // Verify argument types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
        lua_istype ( luaVM, 2, LUA_TSTRING ) )
    {
        // Grab optional wordwrap bool
        bool bWordWrap = false;
        if ( lua_istype ( luaVM, 3, LUA_TBOOLEAN ) )
        {
            bWordWrap = lua_toboolean ( luaVM, 3 ) ? true:false;
        }

        // Grab the align string
        const char* szAlign = lua_tostring ( luaVM, 2 );

        // Compare it with the possible modes and set the mode
        CGUIHorizontalAlign Align = CGUI_ALIGN_LEFT;
        if ( stricmp ( szAlign, "left" ) == 0 )
        {
            if ( bWordWrap )
                Align = CGUI_ALIGN_LEFT_WORDWRAP;
            else
                Align = CGUI_ALIGN_LEFT;
        }
        else if ( stricmp ( szAlign, "center" ) == 0 )
        {
            if ( bWordWrap )
                Align = CGUI_ALIGN_HORIZONTALCENTER_WORDWRAP;
            else
                Align = CGUI_ALIGN_HORIZONTALCENTER;
        }
        else if ( stricmp ( szAlign, "right" ) == 0 )
        {
            if ( bWordWrap )
                Align = CGUI_ALIGN_RIGHT_WORDWRAP;
            else
                Align = CGUI_ALIGN_RIGHT;
        }
        else
        {
            // Bad align string
            CLogger::ErrorPrintf ( "%s", "No such align for guiLabelSetHorizontalAlign" );
            lua_pushboolean ( luaVM, false );
            return 0;
        }

        // Grab thhe GUI element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Set the horizontal align
            CStaticFunctionDefinitions::GUILabelSetHorizontalAlign ( *pEntity, Align );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiLabelSetHorizontalAlign", "gui-element", 1 );
    }

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GUIGetChatboxLayout ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        std::string strCVAR;
        std::string strCVARCommand = ( std::string ) lua_tostring ( luaVM, 1 );

        g_pCore->GetCVars ()->Get ( strCVARCommand, strCVAR );
        if ( !strCVAR.empty() )
        {
            char szSet [] = ".1234567890";
            if ( strspn ( strCVAR.c_str (), szSet ) == strlen ( strCVAR.c_str () ) && !strCVAR.empty () )
            {
                float fNumber = static_cast < float > ( atof ( strCVAR.c_str() ) );
                if ( fNumber >= 0 )
                {
                    lua_pushnumber ( luaVM, fNumber );
                    return 1;
                }
            }
            else if ( strCVARCommand.find("color") != std::string::npos )
            {
                char strColor[32] = { '\0' };
                strcpy ( strColor, strCVAR.c_str() );

                int iRed    = atoi ( strtok ( strColor, " " ) );
                int iGreen  = atoi ( strtok ( NULL, " " ) );
                int iBlue   = atoi ( strtok ( NULL, " " ) );
                int iAlpha  = atoi ( strtok ( NULL, "\r" ) );

                if ( iRed >= 0 && iGreen >= 0 && iBlue >= 0 && iAlpha >= 0 )
                {
                    lua_pushnumber ( luaVM, iRed );
                    lua_pushnumber ( luaVM, iGreen );
                    lua_pushnumber ( luaVM, iBlue );
                    lua_pushnumber ( luaVM, iAlpha );
                    return 4;
                }
            }
            else if ( strCVARCommand == "chat_scale" )
            {
                char strPos[16] = { '\0' };
                strcpy ( strPos, strCVAR.c_str() );

                int iX = atoi ( strtok ( strPos, " " ) );
                int iY = atoi ( strtok ( NULL, "\r" ) );

                if ( iX >= 0 && iY >= 0 )
                {
                    lua_pushnumber ( luaVM, iX );
                    lua_pushnumber ( luaVM, iY );
                    return 2;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getChatboxLayout" );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}