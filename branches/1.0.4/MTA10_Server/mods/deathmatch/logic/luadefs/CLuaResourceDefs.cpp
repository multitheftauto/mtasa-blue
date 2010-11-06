/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaResourceDefs.cpp
*  PURPOSE:     Lua resource function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaResourceDefs::LoadFunctions ( void )
{
    // Create/edit functions
    CLuaCFunctions::AddFunction ( "createResource", CLuaResourceDefs::createResource );
    CLuaCFunctions::AddFunction ( "copyResource", CLuaResourceDefs::copyResource );

    CLuaCFunctions::AddFunction ( "addResourceMap", CLuaResourceDefs::addResourceMap );
    CLuaCFunctions::AddFunction ( "addResourceConfig", CLuaResourceDefs::addResourceConfig );
    CLuaCFunctions::AddFunction ( "removeResourceFile", CLuaResourceDefs::removeResourceFile );

    CLuaCFunctions::AddFunction ( "setResourceDefaultSetting", CLuaResourceDefs::setResourceDefaultSetting );
    CLuaCFunctions::AddFunction ( "removeResourceDefaultSetting", CLuaResourceDefs::removeResourceDefaultSetting );

    // Start/stop management
    CLuaCFunctions::AddFunction ( "startResource", CLuaResourceDefs::startResource );
    CLuaCFunctions::AddFunction ( "stopResource", CLuaResourceDefs::stopResource );
    CLuaCFunctions::AddFunction ( "restartResource", CLuaResourceDefs::restartResource );

    // Get stuff
    CLuaCFunctions::AddFunction ( "getThisResource", CLuaResourceDefs::getThisResource );
    CLuaCFunctions::AddFunction ( "getResourceFromName", CLuaResourceDefs::getResourceFromName );
    CLuaCFunctions::AddFunction ( "getResources", CLuaResourceDefs::getResources );

    CLuaCFunctions::AddFunction ( "getResourceState", CLuaResourceDefs::getResourceState );
    CLuaCFunctions::AddFunction ( "getResourceInfo", CLuaResourceDefs::getResourceInfo );
    CLuaCFunctions::AddFunction ( "getResourceConfig", CLuaResourceDefs::getResourceConfig );
    CLuaCFunctions::AddFunction ( "getResourceLoadFailureReason", CLuaResourceDefs::getResourceLoadFailureReason );
    CLuaCFunctions::AddFunction ( "getResourceLastStartTime", CLuaResourceDefs::getResourceLastStartTime );
    CLuaCFunctions::AddFunction ( "getResourceLoadTime", CLuaResourceDefs::getResourceLoadTime );
    CLuaCFunctions::AddFunction ( "getResourceName", CLuaResourceDefs::getResourceName );
    CLuaCFunctions::AddFunction ( "getResourceRootElement", CLuaResourceDefs::getResourceRootElement );
    CLuaCFunctions::AddFunction ( "getResourceDynamicElementRoot", CLuaResourceDefs::getResourceDynamicElementRoot );
    CLuaCFunctions::AddFunction ( "getResourceMapRootElement", CLuaResourceDefs::getResourceMapRootElement );
    CLuaCFunctions::AddFunction ( "getResourceExportedFunctions", CLuaResourceDefs::getResourceExportedFunctions );

    // Set stuff
    CLuaCFunctions::AddFunction ( "setResourceInfo", CLuaResourceDefs::setResourceInfo );

    // Misc
    CLuaCFunctions::AddFunction ( "call", CLuaResourceDefs::call );
}


int CLuaResourceDefs::createResource ( lua_State* luaVM )
{
    // Verify the arguments
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        // Grab the resource string and try to create it
        const char* szResource = lua_tostring ( luaVM, 1 );
        CResource* pResource = m_pResourceManager->CreateResource ( (char*) szResource );
        if ( pResource )
        {
            lua_pushresource ( luaVM, pResource );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createResource" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::copyResource ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CResource* pResource = lua_toresource ( luaVM, 1 );
        if ( pResource )
        {
            const char* szResource = lua_tostring ( luaVM, 2 );
            CResource* pNewResource = m_pResourceManager->CopyResource ( pResource, szResource );
            if ( pNewResource )
            {
                lua_pushresource ( luaVM, pNewResource );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "copyResource" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::addResourceMap ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        m_pScriptDebugging->LogCustom ( luaVM, "addResourceMap may be using an outdated syntax. Please check and update." );

    // Grab our LUA instance
    CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLUA )
    {
        // Check that our passed arguments are of the correct type
        if ( ( lua_type ( luaVM, 1 ) == LUA_TSTRING ) )
        {
            // Read out the resource and make sure it exists
            CResource* pResource = pLUA->GetResource();
            CResource* pThisResource = pResource;
            if ( pResource )
            {
                // Grab the mapname string
                std::string strMapName = lua_tostring ( luaVM, 1 );
                std::string strPath;
                std::string strMetaName;

                // Eventually read out dimension
                int iDimension = 0;
                if ( ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) || ( lua_type ( luaVM, 2 ) == LUA_TNUMBER ) )
                {
                    iDimension = static_cast < int > ( lua_tonumber ( luaVM, 3 ) );
                    if ( ( iDimension < 0 ) || ( iDimension > 65535 ) )
                    {
                        iDimension = 0;
                    }
                }
                if ( CResourceManager::ParseResourcePathInput ( strMapName, pResource, &strPath, NULL ) )
                {
                    // Do we have permissions?
                    if ( pResource == pThisResource ||
                         m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                        CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                        "ModifyOtherObjects",
                                                        CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                        false ) )
                    {
                        // Add the resource map and return it if we succeeded
                        CXMLNode* pXMLNode = CStaticFunctionDefinitions::AddResourceMap ( pResource, strPath, strMetaName, iDimension, pLUA );
                        if ( pXMLNode )
                        {
                            lua_pushxmlnode ( luaVM, pXMLNode );
                            return 1;
                        }
                    }
                }
                else
                    m_pScriptDebugging->LogError ( luaVM, "addResourceMap failed; ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "addResourceMap" );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "addResourceMap" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::addResourceConfig ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        m_pScriptDebugging->LogCustom ( luaVM, "addResourceConfig may be using an outdated syntax. Please check and update." );

    // Grab our LUA instance
    CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLUA )
    {
        // Check that our passed arguments are of the correct type
        if ( ( lua_type ( luaVM, 1 ) == LUA_TSTRING ) )
        {
            // Read out the resource and make sure it exists
            CResource* pResource = pLUA->GetResource();
            CResource* pThisResource = pResource;
            if ( pResource )
            {
                // Grab the mapname string
                std::string strMapName = lua_tostring ( luaVM, 1 );
                std::string strPath;
                std::string strConfigName;

                if ( CResourceManager::ParseResourcePathInput ( strMapName, pResource, &strPath, &strConfigName ) )
                {
                    // Eventually read out the type string
                    int iType = CResourceFile::RESOURCE_FILE_TYPE_CONFIG;
                    if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
                    {
                        const char* szType = lua_tostring ( luaVM, 2 );
                        if ( stricmp ( szType, "client" ) == 0 )
                            iType = CResourceFile::RESOURCE_FILE_TYPE_CLIENT_CONFIG;
                        else if ( stricmp ( szType, "server" ) != 0 )
                            CLogger::LogPrintf ( "WARNING: Unknown config file type specified for addResourceConfig. Defaulting to 'server'" );
                    }
                    // Do we have permissions?
                    if ( pResource == pThisResource ||
                         m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                        CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                        "ModifyOtherObjects",
                                                        CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                        false ) )
                    {
                        // Add the resource map and return it if we succeeded
                        CXMLNode* pXMLNode = CStaticFunctionDefinitions::AddResourceConfig ( pResource, strPath, strConfigName, iType, pLUA );
                        if ( pXMLNode )
                        {
                            lua_pushxmlnode ( luaVM, pXMLNode );
                            return 1;
                        }
                    }
                }
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "addResourceConfig" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::removeResourceFile ( lua_State* luaVM )
{
    // Check that our passed arguments are of the correct type
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) )
    {
        // Read out the resource and make sure it exists
        CResource* pResource = lua_toresource ( luaVM, 1 );
        if ( pResource )
        {
            // Grab the configname string
            const char* szFileName = lua_tostring ( luaVM, 2 );

            // Remove the resource file and see if we succeeded
            if ( CStaticFunctionDefinitions::RemoveResourceFile ( pResource, szFileName ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "addResourceConfig" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::setResourceDefaultSetting  ( lua_State* luaVM )
{
    // Check that our passed arguments are of the correct type
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) &&
         ( lua_type ( luaVM, 3 ) == LUA_TSTRING || ( lua_type ( luaVM, 3 ) == LUA_TNUMBER ) ) )
    {
        // Read out the resource and make sure it exists
        CResource* pResource = lua_toresource ( luaVM, 1 );
        if ( pResource )
        {
            // Read out the settings name and value
            const char* szSettingName = lua_tostring ( luaVM, 2 );
            const char* szSettingValue = lua_tostring ( luaVM, 3 );

            // Set the setting
            if ( pResource->SetDefaultSetting ( szSettingName, szSettingValue ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::removeResourceDefaultSetting ( lua_State* luaVM )
{
    // Check that our passed arguments are of the correct type
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) )
    {
        // Read out the resource and make sure it exists
        CResource* pResource = lua_toresource ( luaVM, 1 );
        if ( pResource )
        {
            // Read out the settings name
            const char* szSettingName = lua_tostring ( luaVM, 2 );

            // Set the setting
            if ( pResource->RemoveDefaultSetting ( szSettingName ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::startResource ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        bool bPersistent = false;
        if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
        {
            bPersistent = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
        }
        bool bStartIncludedResources = true;
        if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
        {
            bStartIncludedResources = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;
        }
        bool bConfigs = true;
        if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )    // start configs
        {
            bConfigs = lua_toboolean ( luaVM, 4 ) ? true:false;
        }
        bool bMaps = true;
        if ( lua_type ( luaVM, 5 ) == LUA_TBOOLEAN )    // start maps?
        {
            bMaps = lua_toboolean ( luaVM, 5 ) ? true:false;
        }
        bool bScripts = true;
        if ( lua_type ( luaVM, 6 ) == LUA_TBOOLEAN )    // start scripts?
        {
            bScripts = lua_toboolean ( luaVM, 6 ) ? true:false;
        }
        bool bHTML = true;
        if ( lua_type ( luaVM, 7 ) == LUA_TBOOLEAN )    // start html files?
        {
            bHTML = lua_toboolean ( luaVM, 7 ) ? true:false;
        }
        bool bClientConfigs = true;
        if ( lua_type ( luaVM, 8 ) == LUA_TBOOLEAN )    // start client configs?
        {
            bClientConfigs = lua_toboolean ( luaVM, 8 ) ? true:false;
        }
        bool bClientScripts = true;
        if ( lua_type ( luaVM, 9 ) == LUA_TBOOLEAN )    // start client scripts?
        {
            bClientScripts = lua_toboolean ( luaVM, 9 ) ? true:false;
        }
        bool bClientFiles = true;
        if ( lua_type ( luaVM, 10 ) == LUA_TBOOLEAN )   // start client files?
        {
            bClientFiles = lua_toboolean ( luaVM, 10 ) ? true:false;
        }

        // @@@@@ Check if they can start resources
        
        CResource* resource = lua_toresource ( luaVM, 1 );
        if ( resource )
        {
            if ( resource->IsLoaded() )
            {
                if ( !resource->IsActive() && !resource->IsStarting() )
                {
                    std::string strResourceName = resource->GetName ();

                    if ( !m_pResourceManager->StartResource ( resource, NULL, bPersistent, bStartIncludedResources, bConfigs, bMaps, bScripts, bHTML, bClientConfigs, bClientScripts, bClientFiles ) )
                    {
                        CLogger::LogPrintf ( "start: Failed to start resource '%s'\n", strResourceName.c_str () );
                        lua_pushboolean ( luaVM, false );
                        return 1;  
                    }
               
                    if ( resource->IsActive() )
                    {
                        // if the resource is persistent, set that flag
                        resource->SetPersistent ( bPersistent );

                        if ( !bPersistent )
                        {
                            // Add the new resource to the list of included resources so that when
                            // we unload this resource, the new resource goes with it
                            CLuaMain * main = m_pLuaManager->GetVirtualMachine ( luaVM );
                            if ( main )
                            {
                                CResource * thisresource = main->GetResource();
                                if ( thisresource )
                                {
                                    thisresource->AddTemporaryInclude ( resource );
                                    // Make sure the new resource is dependent on this one
                                    resource->AddDependent ( thisresource );
                                }
                            }           
                        }
                        CLogger::LogPrintf ( "start: Resource '%s' started\n", resource->GetName().c_str () );
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "startResource", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "startResource" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::stopResource ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine( luaVM );
    if ( pLuaMain && pLuaMain->GetResource () )
    {   
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CResource* resource = lua_toresource ( luaVM, 1 );
            if ( resource )
            {
                if ( resource->IsActive() )
                {
                    if ( resource->IsProtected() )
                    {
                        if ( !m_pACLManager->CanObjectUseRight ( pLuaMain->GetResource()->GetName().c_str (),
                                                                CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                                "stopResource.protected",
                                                                CAccessControlListRight::RIGHT_TYPE_FUNCTION,
                                                                false ) )
                        {
                            m_pScriptDebugging->LogError ( luaVM, "stop: Resource could not be stopped as it is protected" );
                            lua_pushboolean ( luaVM, false );
                            return 1;
                        }
                    }

                    // Schedule it for a stop
                    m_pResourceManager->QueueResource ( resource, CResourceManager::QUEUE_STOP, NULL );
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "stopResource", "resource", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "stopResource" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::restartResource ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CResource* resource = lua_toresource ( luaVM, 1 );
        if ( resource )
        {
            if ( resource->IsActive() )
            {
                bool bPersistent = false;
                if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
                {
                    bPersistent = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
                    if ( bPersistent )
                    {
                        // @@@@@ Need to check for persistent access?
                    }
                }
                bool bConfigs = true;
                if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )    // start configs
                {
                    bConfigs = lua_toboolean ( luaVM, 3 ) ? true:false;
                }
                bool bMaps = true;
                if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )    // start maps?
                {
                    bMaps = lua_toboolean ( luaVM, 4 ) ? true:false;
                }
                bool bScripts = true;
                if ( lua_type ( luaVM, 5 ) == LUA_TBOOLEAN )    // start scripts?
                {
                    bScripts = lua_toboolean ( luaVM, 5 ) ? true:false;
                }
                bool bHTML = true;
                if ( lua_type ( luaVM, 6 ) == LUA_TBOOLEAN )    // start html files?
                {
                    bHTML = lua_toboolean ( luaVM, 6 ) ? true:false;
                }
                bool bClientConfigs = true;
                if ( lua_type ( luaVM, 7 ) == LUA_TBOOLEAN )    // start client configs?
                {
                    bClientConfigs = lua_toboolean ( luaVM, 7 ) ? true:false;
                }
                bool bClientScripts = true;
                if ( lua_type ( luaVM, 8 ) == LUA_TBOOLEAN )    // start client scripts?
                {
                    bClientScripts = lua_toboolean ( luaVM, 8 ) ? true:false;
                }
                bool bClientFiles = true;
                if ( lua_type ( luaVM, 9 ) == LUA_TBOOLEAN )    // start client files?
                {
                    bClientFiles = lua_toboolean ( luaVM, 9 ) ? true:false;
                }

                // Queue it up for a restart
                CResourceManager::sResourceStartFlags sFlags;
                sFlags.bClientConfigs = bClientConfigs;
                sFlags.bClientFiles = bClientFiles;
                sFlags.bClientScripts = bClientScripts;
                sFlags.bHTML = bHTML;
                sFlags.bScripts = bScripts;
                sFlags.bMaps = bMaps;
                sFlags.bConfigs = bConfigs;
                m_pResourceManager->QueueResource ( resource, CResourceManager::QUEUE_RESTART, &sFlags );

                // Success
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "restartResource", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "restartResource" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getThisResource ( lua_State* luaVM )
{
    CLuaMain * amain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( amain )
    {
        CResource * thisResource = amain->GetResource();
        lua_pushresource ( luaVM, thisResource );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceFromName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szResource = lua_tostring ( luaVM, 1 );
        CResource* pResource = m_pResourceManager->GetResource ( const_cast < char* > ( szResource ) );
        if ( pResource )
        {
            lua_pushresource ( luaVM, pResource );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceFromName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResources ( lua_State* luaVM )
{
    unsigned int uiIndex = 0;
    lua_newtable ( luaVM );
    list < CResource* > ::const_iterator iter = m_pResourceManager->IterBegin ();
    for ( ; iter != m_pResourceManager->IterEnd (); iter++ )
    {
        lua_pushnumber ( luaVM, ++uiIndex );
        lua_pushresource ( luaVM, *iter );
        lua_settable ( luaVM, -3 );
    }
    return 1;
}


int CLuaResourceDefs::getResourceState ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CResource* resource = lua_toresource ( luaVM, 1 );
        if ( resource )
        {
            if ( resource->IsStarting() )
                lua_pushstring ( luaVM, "starting" );
            else if ( resource->IsStopping() )
                lua_pushstring ( luaVM, "stopping" );
            else if ( resource->IsActive() )
                lua_pushstring ( luaVM, "running" );
            else if ( resource->IsLoaded() )
                lua_pushstring ( luaVM, "loaded" );
            else 
                lua_pushstring ( luaVM, "failed to load" );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceState", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceState" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceInfo ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA && 
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CResource* resource = lua_toresource ( luaVM, 1 );
        if ( resource )
        {
            if ( resource->IsLoaded() )
            {
                const char * szInfoValueKey = lua_tostring ( luaVM, 2 );
                std::string strValue;
                resource->GetInfoValue ( szInfoValueKey, strValue );
                if ( !strValue.empty () )
                {
                    lua_pushstring ( luaVM, strValue.c_str () );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceInfo", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceInfo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::setResourceInfo ( lua_State* luaVM )
{
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA && 
         lua_type ( luaVM, 2 ) == LUA_TSTRING &&
         ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNIL ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pResource = lua_toresource ( luaVM, 1 );
            if ( pResource )
            {
                if ( pResource == pThisResource ||
                    m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                        CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                        "ModifyOtherObjects",
                                                        CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                        false ) )
                {
                    if ( pResource->IsLoaded() )
                    {
                        const char * szInfoKey = lua_tostring ( luaVM, 2 );
                        const char * szInfoValue = NULL;
                        if ( iArgument3 == LUA_TSTRING ) szInfoValue = lua_tostring ( luaVM, 3 );
                        pResource->SetInfoValue ( (char*)szInfoKey, (char*)szInfoValue );

                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                    m_pScriptDebugging->LogError ( luaVM, "setResourceInfo failed; ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setResourceInfo", "resource", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "setResourceInfo" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setResourceInfo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceConfig ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    CResource* pResource = pLuaMain->GetResource();
    std::string strConfigName;
    std::string strMetaPath;

    if ( argtype ( 1, LUA_TSTRING ) )
    {
        strConfigName = lua_tostring ( luaVM, 1 );
        if ( CResourceManager::ParseResourcePathInput ( strConfigName, pResource, NULL, &strMetaPath ) )
        {
            list<CResourceFile *> * resourceFileList = pResource->GetFiles();
            list<CResourceFile *>::iterator iterd = resourceFileList->begin();
            for ( ; iterd != resourceFileList->end(); iterd++ )
            {
                CResourceConfigItem* config = (CResourceConfigItem *)(*iterd);

                if ( config &&
                     config->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CONFIG &&
                     strcmp ( config->GetName(), strMetaPath.c_str() ) == 0 )
                {
                    CXMLNode* pNode = config->GetRoot();
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

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceLoadFailureReason ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CResource* resource = lua_toresource ( luaVM, 1 );
        if ( resource )
        {
            if ( !resource->GetFailureReason ().empty () )
                lua_pushstring ( luaVM, resource->GetFailureReason ().c_str () );
            else
                lua_pushstring ( luaVM, "" );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceLoadFailureReason", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceLoadFailureReason" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceLastStartTime ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CResource* resource = lua_toresource ( luaVM, 1 );
        if ( resource )
        {
            time_t timestarted = resource->GetTimeStarted();
            if ( timestarted )
            {
                lua_pushnumber ( luaVM, ( double ) timestarted );
            }
            else
                lua_pushstring ( luaVM, "never" );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceLastStartTime", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceLastStartTime" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceLoadTime ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CResource* resource = lua_toresource ( luaVM, 1 );
        if ( resource )
        {
            time_t timeloaded = resource->GetTimeLoaded();
            if ( timeloaded )
            {
                lua_pushnumber ( luaVM, ( double ) timeloaded );
            }
            else
                lua_pushboolean ( luaVM, false );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceLoadTime", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceLoadTime" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CResource* pResource = lua_toresource ( luaVM, 1 );
        if ( pResource )
        {
            std::string strName = pResource->GetName ();
            if ( !strName.empty () )
            {
                lua_pushstring ( luaVM, strName.c_str () );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceName", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceRootElement ( lua_State* luaVM )
{
    // Resource given?
    CResource* pResource = NULL;
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        pResource = lua_toresource ( luaVM, 1 );         
    }

    // No resource given, get this resource's root
    else if ( lua_type ( luaVM, 1 ) == LUA_TNONE )
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
        // Is it running?
        if ( pResource->IsActive () )
        {
            // Grab the root element of it and return it if it existed
            CElement * pElement = pResource->GetResourceRootElement ();
            if ( pElement )
            {
                lua_pushelement ( luaVM, pElement );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceRootElement" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaResourceDefs::getResourceDynamicElementRoot ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CResource* pResource = lua_toresource ( luaVM, 1 );
        if ( pResource )
        {
            if ( pResource->IsActive () )
            {
                CElement* pElement = pResource->GetDynamicElementRoot ();
                if ( pElement )
                {
                    lua_pushelement ( luaVM, pElement );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "getResourceDynamicElementRoot: Resource %s Is Not Currently Running", pResource->GetName().c_str () );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceDynamicElementRoot", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceDynamicElementRoot" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceMapRootElement ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CResource* pResource = lua_toresource ( luaVM, 1 );
        const char* szMap = lua_tostring ( luaVM, 2 );
        if ( pResource )
        {
            if ( pResource->IsActive () )
            {
                if ( szMap )
                {
                    CElement* pMapRoot = CStaticFunctionDefinitions::GetResourceMapRootElement ( pResource, const_cast < char* > ( szMap ) );
                    if ( pMapRoot )
                    {
                        lua_pushelement ( luaVM, pMapRoot );
                        return 1;
                    }
                }   
                else
                    m_pScriptDebugging->LogError ( luaVM, "getResourceMapRootElement: A Map Name Must Be Specified" );
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "getResourceMapRootElement: Resource %s Is Not Currently Running", pResource->GetName().c_str () );
        }
        else    
            m_pScriptDebugging->LogBadPointer ( luaVM, "getResourceMapRootElement", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceMapRootElement" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceExportedFunctions ( lua_State* luaVM )
{
    CResource* resource = NULL;
    char * szConfigName = NULL;
    // resource
    if ( argtype ( 1, LUA_TLIGHTUSERDATA ) )
    {
        resource = lua_toresource ( luaVM, 1 );
        szConfigName = (char *)lua_tostring ( luaVM, 2 );
    }
    else if ( argtype ( 1, LUA_TNONE ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            resource = pLuaMain->GetResource ();
        }
    }

    if ( resource )
    {
        lua_newtable ( luaVM );
        unsigned int uiIndex = 0;
        list<CExportedFunction *>::iterator iterd = resource->IterBeginExportedFunctions();
        for ( ; iterd != resource->IterEndExportedFunctions(); iterd++ )
        {
            lua_pushnumber ( luaVM, ++uiIndex );
            lua_pushstring ( luaVM, (*iterd)->GetFunctionName ().c_str () );
            lua_settable ( luaVM, -3 );
        }
        return 1;
    }
    
    m_pScriptDebugging->LogBadType ( luaVM, "getResourceExportedFunctions" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::call ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CResource* resource = lua_toresource ( luaVM, 1 );
        char* szFunctionName = (char *) lua_tostring ( luaVM, 2 );

        // Is this an internal function that's restricted? To make sure you can't
        // call an MTA function in an external resource that's restricted and not
        // defined in ACL.
        bool bRestricted = false;
        CLuaCFunction* pFunction = CLuaCFunctions::GetFunction ( szFunctionName );
        if ( pFunction )
            bRestricted = pFunction->IsRestricted ();

        // Check this resource can use the function call to the called resource
        if ( !CanUseFunction ( szFunctionName, luaVM, bRestricted ) )
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        if ( resource )
        {
            if ( resource->IsActive() )
            {
                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
                if ( pLuaMain )
                {
                    //Get the target Lua VM
                    lua_State* targetLuaVM = resource->GetVirtualMachine()->GetVirtualMachine();

                    CResource * resourceThis = pLuaMain->GetResource();
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
                    lua_pushresource ( targetLuaVM, resourceThis );
                    lua_setglobal ( targetLuaVM, "sourceResource" );

                    lua_pushelement ( targetLuaVM, resourceThis->GetResourceRootElement() );
                    lua_setglobal ( targetLuaVM, "sourceResourceRoot" );
                    
                    if ( resource->CallExportedFunction ( szFunctionName, args, returns, *resourceThis ) )
                    {
                        returns.PushArguments ( luaVM );
                        //Restore the old variables
                        OldResource.Push ( targetLuaVM );
                        lua_setglobal ( targetLuaVM, "sourceResource" );
                        OldResourceRoot.Push ( targetLuaVM );
                        lua_setglobal ( targetLuaVM, "sourceResourceRoot" );

                        return returns.Count();
                    }
                    else
                    {
                        //Restore the old variables
                        OldResource.Push ( targetLuaVM );
                        lua_setglobal ( targetLuaVM, "sourceResource" );
                        OldResourceRoot.Push ( targetLuaVM );
                        lua_setglobal ( targetLuaVM, "sourceResourceRoot" );

                        m_pScriptDebugging->LogError ( luaVM, "call: failed to call '%s:%s'", resource->GetName ().c_str (), szFunctionName );
                    }
                }
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "call: Failed, the resource %s isn't running", resource->GetName ().c_str () );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "call", "resource", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "call" );

    lua_pushboolean ( luaVM, false );
    return 1;
}
