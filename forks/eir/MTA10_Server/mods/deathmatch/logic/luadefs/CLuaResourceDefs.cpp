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
#include "../utils/CFunctionUseLogger.h"

extern CNetServer* g_pRealNetServer;

void CLuaResourceDefs::LoadFunctions ( void )
{
    // Create/edit functions
    CLuaCFunctions::AddFunction ( "createResource", CLuaResourceDefs::createResource );
    CLuaCFunctions::AddFunction ( "copyResource", CLuaResourceDefs::copyResource );
    CLuaCFunctions::AddFunction ( "renameResource", CLuaResourceDefs::renameResource );
    CLuaCFunctions::AddFunction ( "deleteResource", CLuaResourceDefs::deleteResource );

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
    CLuaCFunctions::AddFunction ( "refreshResources", CLuaResourceDefs::refreshResources );

    CLuaCFunctions::AddFunction ( "getResourceACLRequests", CLuaResourceDefs::getResourceACLRequests );
    CLuaCFunctions::AddFunction ( "updateResourceACLRequest", CLuaResourceDefs::updateResourceACLRequest, true );
    CLuaCFunctions::AddFunction ( "loadstring", CLuaResourceDefs::LoadString );
    CLuaCFunctions::AddFunction ( "load", CLuaResourceDefs::Load );
}


int CLuaResourceDefs::createResource ( lua_State* luaVM )
{
//  resource createResource ( string toName[, string organizationalDir ] )
    SString strNewResourceName; SString strNewOrganizationalPath;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strNewResourceName );
    argStream.ReadString ( strNewOrganizationalPath, "" );

    if ( !argStream.HasErrors () )
    {
        SString strStatus;
        CResource* pNewResource = m_pResourceManager->CreateResource ( strNewResourceName, strNewOrganizationalPath, strStatus );

        if ( !strStatus.empty () )
            argStream.SetCustomError( strStatus );
        else
        if ( pNewResource )
        {
            lua_pushresource ( luaVM, pNewResource );
            return 1;
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::copyResource ( lua_State* luaVM )
{
//  resource copyResource ( mixed fromResource, string toName[, string organizationalDir ] )
    CResource* pResource; SString strNewResourceName; SString strNewOrganizationalPath;

    CScriptArgReader argStream ( luaVM );
    MixedReadResourceString ( argStream, pResource );
    argStream.ReadString ( strNewResourceName );
    argStream.ReadString ( strNewOrganizationalPath, "" );

    if ( !argStream.HasErrors () )
    {
        SString strStatus;
        CResource* pNewResource = m_pResourceManager->CopyResource ( pResource, strNewResourceName, strNewOrganizationalPath, strStatus );

        if ( !strStatus.empty () )
            argStream.SetCustomError( strStatus );
        else
        if ( pNewResource )
        {
            lua_pushresource ( luaVM, pNewResource );
            return 1;
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::renameResource ( lua_State* luaVM )
{
//  resource renameResource ( mixed fromResource, string newName[, string organizationalDir ] )
    CResource* pResource; SString strNewResourceName; SString strNewOrganizationalPath;

    CScriptArgReader argStream ( luaVM );
    MixedReadResourceString ( argStream, pResource );
    argStream.ReadString ( strNewResourceName );
    argStream.ReadString ( strNewOrganizationalPath, "" );

    if ( !argStream.HasErrors () )
    {
        SString strStatus;
        CResource* pNewResource = m_pResourceManager->RenameResource ( pResource, strNewResourceName, strNewOrganizationalPath, strStatus );

        if ( !strStatus.empty () )
            argStream.SetCustomError( strStatus );
        else
        if ( pNewResource )
        {
            lua_pushresource ( luaVM, pNewResource );
            return 1;
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::deleteResource ( lua_State* luaVM )
{
//  bool deleteResource ( mixed resource )
    SString strResourceName;

    CScriptArgReader argStream ( luaVM );
    MixedReadResourceString ( argStream, strResourceName );

    if ( !argStream.HasErrors () )
    {
        SString strStatus;
        bool bResult = m_pResourceManager->DeleteResource ( strResourceName, strStatus );

        if ( !strStatus.empty () )
            argStream.SetCustomError( strStatus );
        else
        {
            lua_pushboolean ( luaVM, bResult );
            return 1;
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::addResourceMap ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        m_pScriptDebugging->LogCustom ( luaVM, "addResourceMap may be using an outdated syntax. Please check and update." );
    
    SString strMapName;
    unsigned short usDimension;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strMapName );
    argStream.ReadNumber ( usDimension, 0 );

    if ( !argStream.HasErrors ( ) )
    {
        // Grab our LUA instance
        CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLUA )
        {
            // Read out the resource and make sure it exists
            CResource* pResource = pLUA->GetResource();
            CResource* pThisResource = pResource;
            if ( pResource )
            {
                // Grab the mapname string
                SString strPath;
                SString strMetaName;

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
                        CXMLNode* pXMLNode = CStaticFunctionDefinitions::AddResourceMap ( pResource, strPath, strMetaName, usDimension, pLUA );
                        if ( pXMLNode )
                        {
                            lua_pushxmlnode ( luaVM, pXMLNode );
                            return 1;
                        }
                    }
                }
                else
                    m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::addResourceConfig ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        m_pScriptDebugging->LogCustom ( luaVM, "addResourceConfig may be using an outdated syntax. Please check and update." );
        
    SString strMapName, strType;
    CResourceFile::eResourceType iType;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strMapName );
    argStream.ReadString ( strType, "server" );

    if ( strType == "client" )  iType = CResourceFile::RESOURCE_FILE_TYPE_CLIENT_CONFIG;
    else 
    {
        iType = CResourceFile::RESOURCE_FILE_TYPE_CONFIG;
        if ( strType != "server" )  
        {
            CLogger::LogPrintf ( "WARNING: Unknown config file type specified for %s. Defaulting to 'server'", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
        }
    }


    if ( !argStream.HasErrors ( ) )
    {
        // Grab our LUA instance
        CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLUA )
        {
            // Read out the resource and make sure it exists
            CResource* pResource = pLUA->GetResource();
            CResource* pThisResource = pResource;
            if ( pResource )
            {
                // Grab the mapname string
                std::string strPath;
                std::string strConfigName;

                if ( CResourceManager::ParseResourcePathInput ( strMapName, pResource, &strPath, &strConfigName ) )
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
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::removeResourceFile ( lua_State* luaVM )
{
    CResource * pResource;
    SString strFileName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );
    argStream.ReadString ( strFileName );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::RemoveResourceFile ( pResource, strFileName ) )
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


int CLuaResourceDefs::setResourceDefaultSetting  ( lua_State* luaVM )
{
    CResource * pResource;
    SString strSettingName, strSettingValue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );
    argStream.ReadString ( strSettingName );
    argStream.ReadString ( strSettingValue );

    if ( !argStream.HasErrors ( ) )
    { 
        if ( pResource->SetDefaultSetting ( strSettingName, strSettingValue ) ) 
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

int CLuaResourceDefs::removeResourceDefaultSetting ( lua_State* luaVM )
{
    CResource * pResource;
    SString strSettingName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );
    argStream.ReadString ( strSettingName );

    if ( !argStream.HasErrors ( ) )
    { 
        if ( pResource->RemoveDefaultSetting ( strSettingName ) ) 
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

int CLuaResourceDefs::startResource ( lua_State* luaVM )
{
    CResource* pResource;
    bool bPersistent, bStartIncludedResources, bConfigs, bClientFiles;
    bool bMaps, bScripts, bHTML, bClientConfigs, bClientScripts;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );
    argStream.ReadBool ( bPersistent, false );
    argStream.ReadBool ( bStartIncludedResources, true );
    argStream.ReadBool ( bConfigs, true );
    argStream.ReadBool ( bMaps, true );
    argStream.ReadBool ( bScripts, true );
    argStream.ReadBool ( bHTML, true );
    argStream.ReadBool ( bClientConfigs, true );
    argStream.ReadBool ( bClientScripts, true );
    argStream.ReadBool ( bClientFiles, true );
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( pResource->IsLoaded() && !pResource->IsActive() && !pResource->IsStarting() )
        {
            std::string strResourceName = pResource->GetName ();

            if ( !m_pResourceManager->StartResource ( pResource, NULL, bPersistent, bStartIncludedResources, bConfigs, bMaps, bScripts, bHTML, bClientConfigs, bClientScripts, bClientFiles ) )
            {
                CLogger::LogPrintf ( "%s: Failed to start resource '%s'\n", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), strResourceName.c_str () );
                lua_pushboolean ( luaVM, false );
                return 1;  
            }
               
            if ( pResource->IsActive() )
            {
                // if the resource is persistent, set that flag
                pResource->SetPersistent ( bPersistent );

                if ( !bPersistent )
                {
                    // Add the new resource to the list of included resources so that when
                    // we unload this resource, the new resource goes with it
                    CLuaMain * main = m_pLuaManager->GetVirtualMachine ( luaVM );
                    if ( main )
                    {
                        CResource * pThisResource = main->GetResource();
                        if ( pThisResource )
                        {
                            pThisResource->AddTemporaryInclude ( pResource );
                            // Make sure the new resource is dependent on this one
                            pResource->AddDependent ( pThisResource );
                        }
                    }           
                }
                CLogger::LogPrintf ( "%s: Resource '%s' started\n", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pResource->GetName().c_str () );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::stopResource ( lua_State* luaVM )
{
    CResource* pResource;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pResource->IsActive() )
        {
            if ( pResource->IsProtected() )
            {

                if ( !m_pACLManager->CanObjectUseRight ( m_pLuaManager->GetVirtualMachine ( luaVM )->GetResource()->GetName().c_str (),
                    CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                    "stopResource.protected",
                    CAccessControlListRight::RIGHT_TYPE_FUNCTION,
                    false ) )
                {
                    m_pScriptDebugging->LogError ( luaVM, "%s: Resource could not be stopped as it is protected", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }
            }

            // Schedule it for a stop
            m_pResourceManager->QueueResource ( pResource, CResourceManager::QUEUE_STOP, NULL );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::restartResource ( lua_State* luaVM )
{
    CResource* pResource;
    bool bPersistent; // unused
    CResourceManager::sResourceStartFlags sFlags;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );
    argStream.ReadBool ( bPersistent, false );
    argStream.ReadBool ( sFlags.bConfigs, true );
    argStream.ReadBool ( sFlags.bMaps, true );
    argStream.ReadBool ( sFlags.bScripts, true );
    argStream.ReadBool ( sFlags.bHTML, true );
    argStream.ReadBool ( sFlags.bClientConfigs, true );
    argStream.ReadBool ( sFlags.bClientScripts, true );
    argStream.ReadBool ( sFlags.bClientFiles, true );
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( pResource->IsActive() )
        {
            m_pResourceManager->QueueResource ( pResource, CResourceManager::QUEUE_RESTART, &sFlags );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
     
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
    SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors ( ) ) 
    {
        CResource* pResource = m_pResourceManager->GetResource ( strName );
        if ( pResource )
        {
            lua_pushresource ( luaVM, pResource );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResources ( lua_State* luaVM )
{
    unsigned int uiIndex = 0;
    lua_newtable ( luaVM );
    list < CResource* > ::const_iterator iter = m_pResourceManager->IterBegin ();
    for ( ; iter != m_pResourceManager->IterEnd (); ++iter )
    {
        lua_pushnumber ( luaVM, ++uiIndex );
        lua_pushresource ( luaVM, *iter );
        lua_settable ( luaVM, -3 );
    }
    return 1;
}


int CLuaResourceDefs::getResourceState ( lua_State* luaVM )
{
    CResource* pResource;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pResource->IsStarting() )
            lua_pushstring ( luaVM, "starting" );
        else if ( pResource->IsStopping() )
            lua_pushstring ( luaVM, "stopping" );
        else if ( pResource->IsActive() )
            lua_pushstring ( luaVM, "running" );
        else if ( pResource->IsLoaded() )
            lua_pushstring ( luaVM, "loaded" );
        else 
            lua_pushstring ( luaVM, "failed to load" );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceInfo ( lua_State* luaVM )
{
    CResource* pResource;
    SString strInfoValueKey;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );
    argStream.ReadString ( strInfoValueKey );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pResource->IsLoaded() )
        {
            SString strInfoValue;
            pResource->GetInfoValue(strInfoValueKey, strInfoValue);

            if ( !strInfoValue.empty() )
            {
                lua_pushstring ( luaVM, strInfoValue );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::setResourceInfo ( lua_State* luaVM )
{
    CResource* pResource;
    SString strInfoValueKey;
    SString strTempInfoValue;
    const char *szInfoValue = NULL;
    bool bSave;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );
    argStream.ReadString ( strInfoValueKey );
    
    if ( argStream.NextIsString ( ) )
    {
        argStream.ReadString ( strTempInfoValue );
        szInfoValue = strTempInfoValue.c_str();
    }

    argStream.ReadBool ( bSave, true );


    if ( !argStream.HasErrors ( ) )
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
                        pResource->SetInfoValue ( strInfoValueKey, szInfoValue, bSave );

                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                    m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaResourceDefs::getResourceConfig ( lua_State* luaVM )
{
    SString strConfigName, strMetaPath;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strConfigName );

    if ( !argStream.HasErrors ( ) )
    {
        CResource* pResource = m_pLuaManager->GetVirtualMachine ( luaVM )->GetResource();

        if ( CResourceManager::ParseResourcePathInput ( strConfigName, pResource, NULL, &strMetaPath ) )
        {
            list<CResourceFile *> * resourceFileList = pResource->GetFiles();
            list<CResourceFile *>::iterator iterd = resourceFileList->begin();
            for ( ; iterd != resourceFileList->end(); ++iterd )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceLoadFailureReason ( lua_State* luaVM )
{
    CResource* pResource;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );

    if ( !argStream.HasErrors ( ) )
    {
        SString strFailureReason = pResource->GetFailureReason ();
        lua_pushstring ( luaVM, strFailureReason );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceLastStartTime ( lua_State* luaVM )
{
    CResource* pResource;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );

    if ( !argStream.HasErrors ( ) )
    {
        time_t timestarted = pResource->GetTimeStarted();
        if ( timestarted )
            lua_pushnumber ( luaVM, ( double ) timestarted );
        else
            lua_pushstring ( luaVM, "never" );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceLoadTime ( lua_State* luaVM )
{
    CResource* pResource;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );

    if ( !argStream.HasErrors ( ) )
    {
        time_t timeloaded = pResource->GetTimeLoaded();
        if ( timeloaded )
            lua_pushnumber ( luaVM, ( double ) timeloaded );
        else
            lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaResourceDefs::getResourceName ( lua_State* luaVM )
{
    CResource* pResource;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushstring ( luaVM, pResource->GetName ().c_str() );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceRootElement ( lua_State* luaVM )
{
    CResource* pResource;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource, NULL );

    if ( !argStream.HasErrors ( ) )
    {
        if ( !pResource )
        {
            CLuaMain *pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {            
                pResource = pLuaMain->GetResource();
            }
            
            // No Lua VM or no assigned resource?
            if ( !pResource )
            {
                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }


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
        else
            m_pScriptDebugging->LogError ( luaVM, "%s: Resource %s is not currently running", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pResource->GetName().c_str () );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceDynamicElementRoot ( lua_State* luaVM )
{
    CResource* pResource;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource, NULL );

    if ( !argStream.HasErrors ( ) )
    {
        if ( !pResource )
        {
            CLuaMain *pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {            
                pResource = pLuaMain->GetResource();
            }
            
            // No Lua VM or no assigned resource?
            if ( !pResource )
            {
                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }

        if ( pResource->IsActive () )
        {
            CElement * pElement = pResource->GetDynamicElementRoot ();
            if ( pElement )
            {
                lua_pushelement ( luaVM, pElement );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "%s: Resource %s is not currently running", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pResource->GetName().c_str () );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaResourceDefs::getResourceMapRootElement ( lua_State* luaVM )
{
    CResource* pResource;
    SString strMapName;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource, NULL );
    argStream.ReadString ( strMapName );

    if ( !argStream.HasErrors ( ) )
    {

        if ( pResource->IsActive () )
        {
            CElement* pMapRoot = CStaticFunctionDefinitions::GetResourceMapRootElement ( pResource, strMapName );
            if ( pMapRoot )
            {
                lua_pushelement ( luaVM, pMapRoot );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "%s: Resource %s is not currently running", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pResource->GetName().c_str () );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaResourceDefs::getResourceExportedFunctions ( lua_State* luaVM )
{
    CResource* pResource;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource, NULL );

    if ( !argStream.HasErrors ( ) )
    {   
        if ( !pResource )
        {
            CLuaMain *pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {            
                pResource = pLuaMain->GetResource();
            }
            
            // No Lua VM or no assigned resource?
            if ( !pResource )
            {
                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }

        lua_newtable ( luaVM );
        unsigned int uiIndex = 0;
        list < CExportedFunction >::iterator iterd = pResource->IterBeginExportedFunctions();
        for ( ; iterd != pResource->IterEndExportedFunctions(); ++iterd )
        {
            lua_pushnumber ( luaVM, ++uiIndex );
            lua_pushstring ( luaVM, iterd->GetFunctionName ().c_str () );
            lua_settable ( luaVM, -3 );
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaResourceDefs::call ( lua_State* luaVM )
{
    CResource* pResource;
    SString strFunctionName;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );
    argStream.ReadString ( strFunctionName );

    if ( !argStream.HasErrors ( ) )
    {
        // Is this an internal function that's restricted? To make sure you can't
        // call an MTA function in an external resource that's restricted and not
        // defined in ACL.
        bool bRestricted = false;
        CLuaCFunction* pFunction = CLuaCFunctions::GetFunction ( strFunctionName );
        if ( pFunction )
            bRestricted = pFunction->IsRestricted (); 
        
        // Check this resource can use the function call to the called resource
        if ( !CanUseFunction ( strFunctionName, luaVM, bRestricted ) )
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        if ( pResource->IsActive() )
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                //Get the target Lua VM
                lua_State* targetLuaVM = pResource->GetVirtualMachine()->GetVirtualMachine();

                CResource * resourceThis = pLuaMain->GetResource();
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
                lua_pushresource ( targetLuaVM, resourceThis );
                lua_setglobal ( targetLuaVM, "sourceResource" );

                lua_pushelement ( targetLuaVM, resourceThis->GetResourceRootElement() );
                lua_setglobal ( targetLuaVM, "sourceResourceRoot" );

                if ( pResource->CallExportedFunction ( strFunctionName, args, returns, *resourceThis ) )
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

                    m_pScriptDebugging->LogError ( luaVM, "%s: failed to call '%s:%s'", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pResource->GetName ().c_str (), *strFunctionName );
                }
            }
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "%s: Failed, the resource %s isn't running", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pResource->GetName ().c_str () );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::refreshResources ( lua_State* luaVM )
{
    bool bRefreshAll;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bRefreshAll, false );

    if ( !argStream.HasErrors ( ) )
    {
        m_pResourceManager->Refresh ( bRefreshAll );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::getResourceACLRequests  ( lua_State* luaVM )
{
//  table getResourceACLRequests ( resource theResource )
    CResource* pResource;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );

    if ( !argStream.HasErrors () )
    {
        std::vector < SAclRequest > Result;
        pResource->GetAclRequests ( Result );

        // Make table!
        lua_newtable ( luaVM );
        for ( uint i = 0; i < Result.size (); i++ )
        {
            lua_newtable ( luaVM );                             // new table
            lua_pushnumber ( luaVM, i+1 );                      // row index number (starting at 1, not 0)
            lua_pushvalue ( luaVM, -2 );                        // value
            lua_settable ( luaVM, -4 );                         // refer to the top level table

            const SAclRequest& request = Result[i];
            lua_pushstring ( luaVM, "name" );
            lua_pushstring ( luaVM, request.rightName.GetFullName () );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "access" );
            lua_pushboolean ( luaVM, request.bAccess );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "pending" );
            lua_pushboolean ( luaVM, request.bPending );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "who" );
            lua_pushstring ( luaVM, request.strWho );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "date" );
            lua_pushstring ( luaVM, request.strDate );
            lua_settable ( luaVM, -3 );

            lua_pop ( luaVM, 1 );                               // pop the inner table
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaResourceDefs::updateResourceACLRequest ( lua_State* luaVM )
{
//  bool updateResourceACLRequest ( resource theResource, string rightName, bool access, string byWho )
    CResource* pResource; SString strRightName; bool bAccess; SString strUserName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pResource );
    argStream.ReadString ( strRightName );
    argStream.ReadBool ( bAccess );
    argStream.ReadString ( strUserName, "" );

    if ( !argStream.HasErrors () )
    {
        CResource* pThisResource = m_pResourceManager->GetResourceFromLuaState ( luaVM );
        if ( strUserName.empty () && pThisResource )
            strUserName = pThisResource->GetName ();

        if ( pResource->HandleAclRequestChange ( CAclRightName ( strRightName ), bAccess, strUserName ) )
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


int CLuaResourceDefs::LoadString( lua_State* luaVM )
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
        if ( !g_pRealNetServer->DecryptScript( cpInBuffer, uiInSize, &cpBuffer, &uiSize, m_pResourceManager->GetResourceName( luaVM ) + "/loadstring" ) )
        {
            // Problems problems
            if ( GetTimeString( true ) <= INVALID_COMPILED_SCRIPT_CUTOFF_DATE )
            {
                SString strMessage( "loadstring argument 1 is invalid and will not work after %s. Please re-compile at http://luac.mtasa.com/", INVALID_COMPILED_SCRIPT_CUTOFF_DATE ); 
                m_pScriptDebugging->LogCustom( luaVM, strMessage );
                // cpBuffer is always valid after call to DecryptScript
            }
            else
            {
                SString strMessage( "argument 1 is invalid. Please re-compile at http://luac.mtasa.com/", 0 ); 
                argStream.SetCustomError( strMessage );
                cpBuffer = NULL;
            }
        }

        if ( !argStream.HasErrors() )
        {
            if ( !luaL_loadbuffer( luaVM, cpBuffer, uiSize, szChunkname ) )
            {
                // Ok
                if ( g_pGame->GetConfig()->GetLoadstringLogEnabled() )
                    g_pGame->GetFunctionUseLogger()->OnFunctionUse( luaVM, "loadstring", cpBuffer, uiSize );
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean( luaVM, false );
    return 1;
}


int CLuaResourceDefs::Load( lua_State* luaVM )
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
        if ( !g_pRealNetServer->DecryptScript( cpInBuffer, uiInSize, &cpBuffer, &uiSize, m_pResourceManager->GetResourceName( luaVM ) + "/load" ) )
        {
            // Problems problems
            if ( GetTimeString( true ) <= INVALID_COMPILED_SCRIPT_CUTOFF_DATE )
            {
                SString strMessage( "load argument 2 is invalid and will not work after %s. Please re-compile at http://luac.mtasa.com/", INVALID_COMPILED_SCRIPT_CUTOFF_DATE ); 
                m_pScriptDebugging->LogCustom( luaVM, strMessage );
                // cpBuffer is always valid after call to DecryptScript
            }
            else
            {
                SString strMessage( "argument 2 is invalid. Please re-compile at http://luac.mtasa.com/", 0 ); 
                argStream.SetCustomError( strMessage );
                cpBuffer = NULL;
            }
        }

        if ( !argStream.HasErrors() )
        {
            if ( !luaL_loadbuffer( luaVM, cpBuffer, uiSize, szChunkname ) )
            {
                // Ok
                if ( g_pGame->GetConfig()->GetLoadstringLogEnabled() )
                    g_pGame->GetFunctionUseLogger()->OnFunctionUse( luaVM, "load", cpBuffer, uiSize );
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
