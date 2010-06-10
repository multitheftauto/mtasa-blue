/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Resource.cpp
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
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

int CLuaFunctionDefs::Call ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Grab this resource
        CResource* pThisResource = pLuaMain->GetResource ();
        if ( pThisResource )
        {
            // Typechecking
            if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
                lua_istype ( luaVM, 2, LUA_TSTRING ) )
            {
                // Grab the resource
                CResource* pResource = lua_toresource ( luaVM, 1 );
                if ( pResource )
                {
                    //Get the target Lua VM
                    lua_State* targetLuaVM = pResource->GetVM()->GetVM();

                    // The function name
                    const char* szFunctionName = lua_tostring ( luaVM, 2 );

                    // Read out the vargs
                    CLuaArguments args;
                    args.ReadArguments ( luaVM, 3 );
                    CLuaArguments returns;

                    //Lets grab the original hidden variables so we can restore them later
                    lua_getglobal ( targetLuaVM, "sourceResource" );
                    CLuaArgument OldResource ( luaVM, -1 );
                    lua_pop( targetLuaVM, 1 );

                    lua_getglobal ( targetLuaVM, "sourceResourceRoot" );
                    CLuaArgument OldResourceRoot ( luaVM, -1 );
                    lua_pop( targetLuaVM, 1 );

                    //Set the new values for the current sourceResource, and sourceResourceRoot
                    lua_pushresource ( targetLuaVM, pThisResource );
                    lua_setglobal ( targetLuaVM, "sourceResource" );

                    lua_pushelement ( targetLuaVM, pThisResource->GetResourceEntity() );
                    lua_setglobal ( targetLuaVM, "sourceResourceRoot" );

                    // Call the exported function with the given name and the args
                    if ( pResource->CallExportedFunction ( szFunctionName, args, returns, *pThisResource ) )
                    {
                        // Push return arguments
                        returns.PushArguments ( luaVM );
                        //Restore the old variables
                        OldResource.Push ( targetLuaVM );
                        lua_setglobal ( targetLuaVM, "sourceResource" );

                        OldResourceRoot.Push ( targetLuaVM );
                        lua_setglobal ( targetLuaVM, "sourceResourceRoot" );

                        return returns.Count ();
                    }
                    else
                    {
                        //Restore the old variables
                        OldResource.Push ( targetLuaVM );
                        lua_setglobal ( targetLuaVM, "sourceResource" );

                        OldResourceRoot.Push ( targetLuaVM );
                        lua_setglobal ( targetLuaVM, "sourceResourceRoot" );
                        m_pScriptDebugging->LogError ( luaVM, "call: failed to call '%s:%s'", pResource->GetName (), szFunctionName );
                    }
                }
                else
                {
                    m_pScriptDebugging->LogBadPointer ( luaVM, "call", "resource", 1 );
                }
            }
            else
            {
                m_pScriptDebugging->LogBadType ( luaVM, "call" );
            }
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetThisResource ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Return the resource if any
        CResource* pThisResource = pLuaMain->GetResource ();
        if ( pThisResource )
        {
            lua_pushresource ( luaVM, pThisResource );
            return 1;
        }
    }

    // No this resource (heh that'd be strange)
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetResourceConfig ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
        m_pScriptDebugging->LogCustom ( luaVM, "getResourceConfig may be using an outdated syntax. Please check and update." );

    // Resource and config name
    CResource* pResource = NULL;
    const char* szInput = NULL;
    std::string strAbsPath;
    std::string strMetaPath;

    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        // Grab our lua main
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Grab resource and the config name from arg
            pResource = pLuaMain->GetResource ();
            szInput = lua_tostring ( luaVM, 1 );
        }
    }

    // We have both a resource file to grab the config from and a config name?
    if ( pResource && szInput )
    {
        if ( CResourceManager::ParseResourcePathInput ( szInput, pResource, strAbsPath, strMetaPath ) )
        {
            // Loop through the configs in that resource
            list < CResourceConfigItem* >::iterator iter = pResource->ConfigIterBegin ();
            for ( ; iter != pResource->ConfigIterEnd (); iter++ )
            {
                // Matching name?
                if ( strcmp ( (*iter)->GetShortName(), strMetaPath.c_str() ) == 0 )
                {
                    // Return it
                    CResourceConfigItem* pConfig = (CResourceConfigItem*) (*iter);
                    CXMLNode* pNode = pConfig->GetRoot ();
                    if ( pNode )
                    {
                        lua_pushxmlnode ( luaVM, pNode );
                        return 1;
                    }
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceConfig" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetResourceName ( lua_State* luaVM )
{
    // Verify arguments
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the resource argument
        CResource* pResource = lua_toresource ( luaVM, 1 );
        if ( pResource )
        {
            // Grab its name and return it
            char* szName = pResource->GetName ();
            if ( szName )
            {
                lua_pushstring ( luaVM, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceName", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceName" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetResourceFromName ( lua_State* luaVM )
{
    // Verify arguments
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        // Grab the argument
        const char* szResource = lua_tostring ( luaVM, 1 );

        // Try to find a resource with that name
        CResource* pResource = m_pResourceManager->GetResource ( szResource );
        if ( pResource )
        {
            lua_pushresource ( luaVM, pResource );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceFromName" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetResourceRootElement ( lua_State* luaVM )
{
    // Resource given?
    CResource* pResource = NULL;
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        pResource = lua_toresource ( luaVM, 1 );
    }

    // No resource given, get this resource's root
    else if ( lua_istype ( luaVM, 1, LUA_TNONE ) )
    {
        // Find our vm and get the root
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            pResource = pLuaMain->GetResource ();
        }
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceRootElement", "resource", 1 );

    // Did we find a resource?
    if ( pResource )
    {
        // Grab the root element of it and return it if it existed
        CClientEntity* pEntity = pResource->GetResourceEntity ();
        if ( pEntity )
        {
            lua_pushelement ( luaVM, pEntity );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceRootElement" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetResourceGUIElement ( lua_State* luaVM )
{
    // Resource given?
    CResource* pResource = NULL;
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        pResource = lua_toresource ( luaVM, 1 );
    }

    // No resource given, get this resource's root
    else if ( lua_istype ( luaVM, 1, LUA_TNONE ) )
    {
        // Find our vm and get the root
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            pResource = pLuaMain->GetResource ();
        }
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceGUIElement", "resource", 1 );

    // Did we get a resource?
    if ( pResource )
    {
        // Grab the gui entity. If it exists, return it
        CClientEntity* pEntity = pResource->GetResourceGUIEntity ();
        if ( pEntity )
        {
            lua_pushelement ( luaVM, pEntity );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceGUIElement" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetResourceDynamicElementRoot ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CResource* pResource = lua_toresource ( luaVM, 1 );
        if ( pResource )
        {
            CClientEntity* pEntity = pResource->GetResourceDynamicEntity();
            if ( pEntity )
            {
                lua_pushelement ( luaVM, pEntity );
                return 1;
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "getResourceDynamicElementRoot: Resource %s Is Not Currently Running", pResource->GetName() );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceDynamicElementRoot", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceDynamicElementRoot" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


