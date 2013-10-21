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
    CResource * pResource = NULL;
    SString strFunctionName = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );
    argStream.ReadString ( strFunctionName );
    if ( !argStream.HasErrors ( ) )
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Grab this resource
            CResource* pThisResource = pLuaMain->GetResource ();
            if ( pThisResource )
            {
                if ( pResource )
                {
                    //Get the target Lua VM
                    lua_State* targetLuaVM = pResource->GetVM()->GetVM();

                    // Read out the vargs
                    CLuaArguments args;
                    args.ReadArguments ( luaVM, 3 );
                    CLuaArguments returns;

                    LUA_CHECKSTACK ( targetLuaVM, 1 );   // Ensure some room

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
                    if ( pResource->CallExportedFunction ( strFunctionName, args, returns, *pThisResource ) )
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
                        m_pScriptDebugging->LogError ( luaVM, "call: failed to call '%s:%s'", pResource->GetName (), strFunctionName );
                    }
                }
                else
                {
                    m_pScriptDebugging->LogBadType ( luaVM );
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );


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
    // Indeed it would. 08/04/2013
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetResourceConfig ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserData ( ) )
        m_pScriptDebugging->LogCustom ( luaVM, "getResourceConfig may be using an outdated syntax. Please check and update." );

    // Resource and config name
    CResource* pResource = NULL;
    SString strInput;
    SString strAbsPath;
    SString strMetaPath;

    argStream.ReadString ( strInput );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua main
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Grab resource and the config name from arg
            pResource = pLuaMain->GetResource ();

            // We have both a resource file to grab the config from and a config name?
            if ( pResource )
            {
                if ( CResourceManager::ParseResourcePathInput ( strInput, pResource, strAbsPath, strMetaPath ) )
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
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetResourceName ( lua_State* luaVM )
{
    // Verify arguments
    CResource* pResource = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pResource )
        {
            // Grab its name and return it
            const char* szName = pResource->GetName ();
            if ( szName )
            {
                lua_pushstring ( luaVM, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "resource", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetResourceFromName ( lua_State* luaVM )
{
    // Verify arguments
    SString strResourceName = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strResourceName );

    if ( !argStream.HasErrors ( ) )
    {
        // Try to find a resource with that name
        CResource* pResource = m_pResourceManager->GetResource ( strResourceName );
        if ( pResource )
        {
            lua_pushresource ( luaVM, pResource );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetResourceRootElement ( lua_State* luaVM )
{
    // Verify arguments
    CResource* pResource = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource, NULL );

    // No resource given, get this resource's root
    if ( pResource == NULL )
    {
        // Find our vm and get the root
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            pResource = pLuaMain->GetResource ();
        }
    }

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
        m_pScriptDebugging->LogBadType ( luaVM );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetResourceGUIElement ( lua_State* luaVM )
{
    // Verify arguments
    CResource* pResource = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource, NULL );

    // No resource given, get this resource's root
    if ( pResource == NULL )
    {
        // Find our vm and get the root
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            pResource = pLuaMain->GetResource ();
        }
    }

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
        m_pScriptDebugging->LogBadType ( luaVM );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetResourceDynamicElementRoot ( lua_State* luaVM )
{
    CResource* pResource = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );

    if ( !argStream.HasErrors () )
    {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "resource", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetResourceExportedFunctions ( lua_State *luaVM )
{
    CResource* pResource = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource, NULL );
    
    // No resource given, get this resource's root
    if ( pResource == NULL )
    {
        // Find our vm and get the root
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            pResource = pLuaMain->GetResource ();
        }
    }

    if ( pResource )
    {
        lua_newtable ( luaVM );
        unsigned int uiIndex = 0;
        list<CExportedFunction *>::iterator iterd = pResource->IterBeginExportedFunctions();
        for ( ; iterd != pResource->IterEndExportedFunctions(); iterd++ )
        {
            lua_pushnumber ( luaVM, ++uiIndex );
            lua_pushstring ( luaVM, (*iterd)->GetFunctionName () );
            lua_settable ( luaVM, -3 );
        }
        return 1;
    }
    
    m_pScriptDebugging->LogBadType ( luaVM );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::LoadString( lua_State* luaVM )
{
//  func,err loadstring( string text[, string name] )
    SString strInput; SString strName;

    CScriptArgReader argStream( luaVM );
    argStream.ReadString( strInput );
    argStream.ReadString( strName, "" );

    if ( !argStream.HasErrors() )
    {
        const char* szChunkname = strName.empty() ? *strInput : *strName;
        const char* cpInBuffer = strInput;
        uint uiInSize = strInput.length();

        // Decrypt if required
        const char* cpBuffer;
        uint uiSize;
        if ( !g_pNet->DecryptScript( cpInBuffer, uiInSize, &cpBuffer, &uiSize ) )
        {
            // Problems problems
    #if MTA_DM_VERSION < 0x135 
            SString strMessage( "loadstring argument 1 is invalid and will not work in future versions. Please re-compile at http://luac.mtasa.com/", 0 ); 
            m_pScriptDebugging->LogCustom( luaVM, strMessage );
            // cpBuffer is always valid after call to DecryptScript
    #else
            SString strMessage( "argument 1 is invalid. Please re-compile at http://luac.mtasa.com/", 0 ); 
            argStream.SetCustomError( strMessage );
            cpBuffer = NULL;
    #endif
            g_pClientGame->TellServerSomethingImportant( 1004, argStream.GetFullErrorMessage(), true );
        }

        if ( !argStream.HasErrors() )
        {
            if ( !luaL_loadbuffer( luaVM, cpBuffer, uiSize, szChunkname ) )
            {
                // Ok
                return 1;
            }
            else
            {
                lua_pushnil( luaVM );
                lua_insert( luaVM, -2 );  /* put before error message */
                return 2;  /* return nil plus error message */
            }
        }
    }
    if ( argStream.HasErrors() )
        m_pScriptDebugging->LogCustom( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::Load( lua_State* luaVM )
{
//  func,err load( callback callbackFunction[, string name] )
    CLuaFunctionRef iLuaFunction; SString strName;

    CScriptArgReader argStream( luaVM );
    argStream.ReadFunction( iLuaFunction );
    argStream.ReadString( strName, "=(load)" );
    argStream.ReadFunctionComplete();

    if ( !argStream.HasErrors() )
    {
        // Call supplied function to get all the bits
        // Should apply some limit here?
        SString strInput;
        CLuaArguments callbackArguments;
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine( luaVM );
        while( pLuaMain )
        {
            CLuaArguments returnValues;
            callbackArguments.Call( pLuaMain, iLuaFunction, &returnValues );
            if ( returnValues.Count() )
            {
                CLuaArgument* returnedValue = *returnValues.IterBegin();
                if ( returnedValue->GetType() == LUA_TSTRING )
                {
                    strInput += returnedValue->GetString();
                    continue;
                }
            }
            break;
        }

        const char* szChunkname = *strName;
        const char* cpInBuffer = strInput;
        uint uiInSize = strInput.length();

        // Decrypt if required
        const char* cpBuffer;
        uint uiSize;
        if ( !g_pNet->DecryptScript( cpInBuffer, uiInSize, &cpBuffer, &uiSize ) )
        {
            // Problems problems
    #if MTA_DM_VERSION < 0x135 
            SString strMessage( "loadstring argument 1 is invalid and will not work in future versions. Please re-compile at http://luac.mtasa.com/", 0 ); 
            m_pScriptDebugging->LogCustom( luaVM, strMessage );
            // cpBuffer is always valid after call to DecryptScript
    #else
            SString strMessage( "argument 1 is invalid. Please re-compile at http://luac.mtasa.com/", 0 ); 
            argStream.SetCustomError( strMessage );
            cpBuffer = NULL;
    #endif
            g_pClientGame->TellServerSomethingImportant( 1005, argStream.GetFullErrorMessage(), true );
        }

        if ( !argStream.HasErrors() )
        {
            if ( !luaL_loadbuffer( luaVM, cpBuffer, uiSize, szChunkname ) )
            {
                // Ok
                return 1;
            }
            else
            {
                lua_pushnil( luaVM );
                lua_insert( luaVM, -2 );  /* put before error message */
                return 2;  /* return nil plus error message */
            }
        }
    }
    if ( argStream.HasErrors() )
        m_pScriptDebugging->LogCustom( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean( luaVM, false );
    return 1;
}
