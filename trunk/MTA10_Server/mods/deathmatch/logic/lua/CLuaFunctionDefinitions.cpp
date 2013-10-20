/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefinitions.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define MIN_SERVER_REQ_CALLREMOTE_CONNECTION_ATTEMPTS       "1.3.0-9.04563"
#define MIN_SERVER_REQ_TRIGGERCLIENTEVENT_SENDLIST          "1.3.0-9.04570"
#define MIN_SERVER_REQ_CALLREMOTE_CONNECT_TIMEOUT           "1.3.5"

extern CGame* g_pGame;

CTimeUsMarker < 20 > markerLatentEvent; // For timing triggerLatentClientEvent

static CBlipManager*                                    m_pBlipManager = NULL;
static CLuaManager*                                     m_pLuaManager = NULL;
static CMarkerManager*                                  m_pMarkerManager = NULL;
static CObjectManager*                                  m_pObjectManager = NULL;
static CPickupManager*                                  m_pPickupManager = NULL;
static CPlayerManager*                                  m_pPlayerManager = NULL;
static CRadarAreaManager*                               m_pRadarAreaManager = NULL;
static CRegisteredCommands*                             m_pRegisteredCommands;
static CElement*                                        m_pRootElement = NULL;
static CScriptDebugging*                                m_pScriptDebugging = NULL;
static CVehicleManager*                                 m_pVehicleManager = NULL;
static CTeamManager*                                    m_pTeamManager = NULL;
static CAccountManager*                                 m_pAccountManager = NULL;
static CColManager*                                     m_pColManager = NULL;
static CResourceManager*                                m_pResourceManager = NULL;
static CAccessControlListManager*                       m_pACLManager = NULL;
static CLuaModuleManager*                               m_pLuaModuleManager = NULL;
#define type(number,type) (lua_type(luaVM,number) == type)


void CLuaFunctionDefinitions::SetBlipManager ( CBlipManager* pBlipManager )
{
    m_pBlipManager = pBlipManager;
}


void CLuaFunctionDefinitions::SetLuaManager ( CLuaManager* pLuaManager )
{
    m_pLuaManager = pLuaManager;
    m_pLuaModuleManager = pLuaManager->GetLuaModuleManager();
}


void CLuaFunctionDefinitions::SetMarkerManager ( CMarkerManager* pMarkerManager )
{
    m_pMarkerManager = pMarkerManager;
}


void CLuaFunctionDefinitions::SetObjectManager ( CObjectManager* pObjectManager )
{
    m_pObjectManager = pObjectManager;
}


void CLuaFunctionDefinitions::SetPickupManager ( CPickupManager* pPickupManager )
{
    m_pPickupManager = pPickupManager;
}


void CLuaFunctionDefinitions::SetPlayerManager ( CPlayerManager* pPlayerManager )
{
    m_pPlayerManager = pPlayerManager;
}


void CLuaFunctionDefinitions::SetRadarAreaManager ( CRadarAreaManager* pRadarAreaManager )
{
    m_pRadarAreaManager = pRadarAreaManager;
}


void CLuaFunctionDefinitions::SetRegisteredCommands ( CRegisteredCommands* pRegisteredCommands )
{
    m_pRegisteredCommands = pRegisteredCommands;
}


void CLuaFunctionDefinitions::SetRootElement ( CElement* pElement )
{
    m_pRootElement = pElement;
}


void CLuaFunctionDefinitions::SetScriptDebugging ( CScriptDebugging* pScriptDebugging )
{
    m_pScriptDebugging = pScriptDebugging;
}

void CLuaFunctionDefinitions::SetVehicleManager ( CVehicleManager* pVehicleManager )
{
    m_pVehicleManager = pVehicleManager;
}


void CLuaFunctionDefinitions::SetTeamManager ( CTeamManager* pTeamManager )
{
    m_pTeamManager = pTeamManager;
}


void CLuaFunctionDefinitions::SetAccountManager ( CAccountManager* pAccountManager )
{
    m_pAccountManager = pAccountManager;
}


void CLuaFunctionDefinitions::SetColManager ( CColManager* pColManager )
{
    m_pColManager = pColManager;
}


void CLuaFunctionDefinitions::SetResourceManager ( CResourceManager* pResourceManager )
{
    m_pResourceManager = pResourceManager;
}


void CLuaFunctionDefinitions::SetACL ( CAccessControlListManager* pACLManager )
{
    m_pACLManager = pACLManager;
}


int CLuaFunctionDefinitions::DisabledFunction ( lua_State* luaVM )
{
    m_pScriptDebugging->LogError ( luaVM, "Unsafe function was called." );

    lua_pushboolean ( luaVM, false );
    return 1;
}


// Call a function on a remote server
int CLuaFunctionDefinitions::CallRemote ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    if ( !argStream.NextIsFunction ( 1 ) && !argStream.NextIsFunction ( 2 ) )
    {
        // Call type 1
        //  bool callRemote ( string host [, int connectionAttempts = 10, int connectTimeout = 10000 ], string resourceName, string functionName, callback callbackFunction, [ arguments... ] )
        SString strHost; uint uiConnectionAttempts; uint uiConnectTimeoutMs; SString strResourceName; SString strFunctionName; CLuaFunctionRef iLuaFunction; CLuaArguments args;

        argStream.ReadString ( strHost );
        if ( argStream.NextIsNumber () )
            MinServerReqCheck ( argStream, MIN_SERVER_REQ_CALLREMOTE_CONNECTION_ATTEMPTS, "'connection attempts' is being used" );
        argStream.ReadIfNextIsNumber ( uiConnectionAttempts, 10 );
        if ( argStream.NextIsNumber () )
            MinServerReqCheck ( argStream, MIN_SERVER_REQ_CALLREMOTE_CONNECT_TIMEOUT, "'connect timeout' is being used" );
        argStream.ReadIfNextIsNumber ( uiConnectTimeoutMs, 10000 );
        argStream.ReadString ( strResourceName );
        argStream.ReadString ( strFunctionName );
        argStream.ReadFunction ( iLuaFunction );
        argStream.ReadLuaArguments ( args );
        argStream.ReadFunctionComplete ();

        if ( !argStream.HasErrors () )
        {
            CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( luaMain )
            {
                g_pGame->GetRemoteCalls()->Call ( strHost, strResourceName, strFunctionName, &args, luaMain, iLuaFunction, uiConnectionAttempts, uiConnectTimeoutMs );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
    {
        // Call type 2
        //  bool callRemote ( string URL [, int connectionAttempts = 10, int connectTimeout = 10000 ], callback callbackFunction, [ arguments... ] )
        SString strURL; uint uiConnectionAttempts; uint uiConnectTimeoutMs; CLuaFunctionRef iLuaFunction; CLuaArguments args;

        argStream.ReadString ( strURL );
        if ( argStream.NextIsNumber () )
            MinServerReqCheck ( argStream, MIN_SERVER_REQ_CALLREMOTE_CONNECTION_ATTEMPTS, "'connection attempts' is being used" );
        argStream.ReadIfNextIsNumber ( uiConnectionAttempts, 10 );
        if ( argStream.NextIsNumber () )
            MinServerReqCheck ( argStream, MIN_SERVER_REQ_CALLREMOTE_CONNECT_TIMEOUT, "'connect timeout' is being used" );
        argStream.ReadIfNextIsNumber ( uiConnectTimeoutMs, 10000 );
        argStream.ReadFunction ( iLuaFunction );
        argStream.ReadLuaArguments ( args );
        argStream.ReadFunctionComplete ();

        if ( !argStream.HasErrors () )
        {
            CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( luaMain )
            {
                g_pGame->GetRemoteCalls()->Call ( strURL, &args, luaMain, iLuaFunction, uiConnectionAttempts, uiConnectTimeoutMs );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


// Call a function on a remote server
int CLuaFunctionDefinitions::FetchRemote ( lua_State* luaVM )
{
//  bool fetchRemote ( string URL [, int connectionAttempts = 10, int connectTimeout = 10000 ], callback callbackFunction, [ string postData, bool bPostBinary, arguments... ] )
    CScriptArgReader argStream ( luaVM );
    SString strURL; CLuaFunctionRef iLuaFunction; SString strPostData; bool bPostBinary; CLuaArguments args; uint uiConnectionAttempts; uint uiConnectTimeoutMs;

    argStream.ReadString ( strURL );
    if ( argStream.NextIsNumber () )
        MinServerReqCheck ( argStream, MIN_SERVER_REQ_CALLREMOTE_CONNECTION_ATTEMPTS, "'connection attempts' is being used" );
    argStream.ReadIfNextIsNumber ( uiConnectionAttempts, 10 );
    if ( argStream.NextIsNumber () )
        MinServerReqCheck ( argStream, MIN_SERVER_REQ_CALLREMOTE_CONNECT_TIMEOUT, "'connect timeout' is being used" );
    argStream.ReadIfNextIsNumber ( uiConnectTimeoutMs, 10000 );
    argStream.ReadFunction ( iLuaFunction );
    argStream.ReadString ( strPostData, "" );
    argStream.ReadBool ( bPostBinary, false );
    argStream.ReadLuaArguments ( args );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            g_pGame->GetRemoteCalls()->Call ( strURL, &args, strPostData, bPostBinary, luaMain, iLuaFunction, uiConnectionAttempts, uiConnectTimeoutMs );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetServerName ( lua_State* luaVM )
{
    lua_pushstring ( luaVM, g_pGame->GetConfig()->GetServerName().c_str () );
    return 1;
}


int CLuaFunctionDefinitions::GetServerHttpPort ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pGame->GetConfig()->GetHTTPPort() );
    return 1;
}


int CLuaFunctionDefinitions::GetServerIP ( lua_State* luaVM )
{
    lua_pushstring ( luaVM, "moo" );
    return 1;
}


int CLuaFunctionDefinitions::GetServerPassword ( lua_State* luaVM )
{
    // We have a password? Return it.
    if ( g_pGame->GetConfig ()->HasPassword () )
    {
        // Return it
        lua_pushstring ( luaVM, g_pGame->GetConfig ()->GetPassword ().c_str () );
        return 1;
    }

    // Otherwize return nil for no password
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::SetServerPassword ( lua_State* luaVM )
{
//  bool setServerPassword ( [ string password ] )
    SString strPassword;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strPassword, "" );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetServerPassword ( strPassword, true ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            argStream.SetCustomError( "password must be shorter than 32 chars and just contain visible characters" );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetServerConfigSetting ( lua_State* luaVM )
{
//  string getServerConfigSetting ( string name )
    SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors () )
    {
        SString strValue;
        // Try as single setting
        if ( g_pGame->GetConfig ()->GetSetting ( strName, strValue ) )
        {
            lua_pushstring ( luaVM, strValue );
            return 1;
        }
        // Try as multiple setting
        CLuaArguments result;
        if ( g_pGame->GetConfig ()->GetSettingTable ( strName, &result ) )
        {
            result.PushAsTable( luaVM );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetServerConfigSetting ( lua_State* luaVM )
{
//  bool setServerConfigSetting ( string name, string value [, bool save = false ] )
    SString strName; SString strValue; bool bSave;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadString ( strValue );
    argStream.ReadBool ( bSave, false );

    if ( !argStream.HasErrors () )
    {
        if ( g_pGame->GetConfig ()->SetSetting ( strName, strValue, bSave ) )
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


int CLuaFunctionDefinitions::shutdown ( lua_State* luaVM )
{
    SString strReason;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strReason, "No reason specified" );

    if ( !argStream.HasErrors ( ) )
    {
        // Get the VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Get the resource
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Log it
                CLogger::LogPrintf ( "Server shutdown as requested by resource %s (%s)\n", pResource->GetName ().c_str (), *strReason );

                // Shut it down
                g_pGame->SetIsFinished ( true );

                // Success
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else 
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Fail
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::AddEvent ( lua_State* luaVM )
{
//  bool addEvent ( string eventName [, bool allowRemoteTrigger = false ] )
    SString strName; bool bAllowRemoteTrigger;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadBool ( bAllowRemoteTrigger, false );

    if ( !argStream.HasErrors () )
    {
        // Grab our virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Do it
            if ( CStaticFunctionDefinitions::AddEvent ( pLuaMain, strName, "", bAllowRemoteTrigger ) )
            {
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


int CLuaFunctionDefinitions::AddEventHandler ( lua_State* luaVM )
{
//  bool addEventHandler ( string eventName, element attachedTo, function handlerFunction [, bool getPropagated = true, string priority = "normal" ] )
    SString strName; CElement* pElement; CLuaFunctionRef iLuaFunction; bool bPropagated; SString strPriority;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadUserData ( pElement );
    argStream.ReadFunction ( iLuaFunction );
    argStream.ReadBool ( bPropagated, true );
    argStream.ReadString ( strPriority, "normal" );
    argStream.ReadFunctionComplete ();

    // Check if strPriority has a number as well. e.g. name+1 or name-1.32
    float fPriorityMod = 0;
    EEventPriorityType eventPriority;
    {
        uint iPos = strPriority.find_first_of ( "-+" );
        if ( iPos != SString::npos )
        {
            fPriorityMod = static_cast < float > ( atof ( strPriority.SubStr ( iPos ) ) );
            strPriority = strPriority.Left ( iPos );
        }

        if ( !StringToEnum ( strPriority, eventPriority ) )
            argStream.SetTypeError ( GetEnumTypeName ( eventPriority ), 5 );    // priority is argument #5
    }

    if ( !argStream.HasErrors () )
    {
        // Grab our virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResourceFile * file = pLuaMain->GetResourceFile();
            if ( file && file->GetType() == CResourceFile::RESOURCE_FILE_TYPE_HTML )
            {
                argStream.SetCustomError( "You cannot have event handlers in HTML scripts" );
            }
            else
            {
                // check if the handle is in use
                if ( pElement->GetEventManager()->HandleExists ( pLuaMain, strName, iLuaFunction ) )
                {
                    argStream.SetCustomError( SString( "'%s' with this function is already handled", *strName ) );
                }
                // Do it
                if ( CStaticFunctionDefinitions::AddEventHandler ( pLuaMain, strName, pElement, iLuaFunction, bPropagated, eventPriority, fPriorityMod ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemoveEventHandler ( lua_State* luaVM )
{
//  bool removeEventHandler ( string eventName, element attachedTo, function functionVar )
    SString strName; CElement* pElement; CLuaFunctionRef iLuaFunction;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadUserData ( pElement );
    argStream.ReadFunction ( iLuaFunction );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Grab our virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            if ( CStaticFunctionDefinitions::RemoveEventHandler ( pLuaMain, strName, pElement, iLuaFunction ) )
            {
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


int CLuaFunctionDefinitions::GetEventHandlers ( lua_State* luaVM )
{
//  table getEventHandlers ( string eventName, element attachedTo )
    SString strName; CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        // Grab our virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Create a new table
            lua_newtable ( luaVM );

            pElement->GetEventManager ()->GetHandles ( pLuaMain, (const char*)strName, luaVM );

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::TriggerEvent ( lua_State* luaVM )
{
//  bool triggerEvent ( string eventName, element baseElement, [ var argument1, ... ] )
    SString strName; CElement* pElement; CLuaArguments Arguments;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadUserData ( pElement );
    argStream.ReadLuaArguments ( Arguments );

    if ( !argStream.HasErrors () )
    {
        // Trigger it
        bool bWasCancelled;
        if ( CStaticFunctionDefinitions::TriggerEvent ( strName, pElement, Arguments, bWasCancelled ) )
        {
            lua_pushboolean ( luaVM, !bWasCancelled );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::TriggerClientEvent ( lua_State* luaVM )
{
//  int triggerClientEvent ( [element/table triggerFor,] string name, element theElement, [arguments...] )
    std::vector < CPlayer* > sendList; SString strName; CElement* pCallWithElement; CLuaArguments Arguments;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsTable () )
    {
        MinServerReqCheck ( argStream, MIN_SERVER_REQ_TRIGGERCLIENTEVENT_SENDLIST, "a send list is being used" );
        argStream.ReadUserDataTable ( sendList );
    }
    else
    {
        CElement* pElement;
        argStream.ReadIfNextIsUserData ( pElement, CStaticFunctionDefinitions::GetRootElement () );
        if ( !argStream.HasErrors () )
            pElement->GetDescendantsByType ( sendList, CElement::PLAYER );
    }
    argStream.ReadString ( strName );
    argStream.ReadUserData ( pCallWithElement );
    argStream.ReadLuaArguments ( Arguments );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::TriggerClientEvent ( sendList, strName, pCallWithElement, Arguments ) )
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


int CLuaFunctionDefinitions::TriggerLatentClientEvent ( lua_State* luaVM )
{
    markerLatentEvent = CTimeUsMarker < 20 > ();
    markerLatentEvent.Set ( "Start" );

//  int triggerLatentClientEvent ( [element/table triggerFor,] string name, [int bandwidth=50000,] [bool persist=false,] element theElement, [arguments...] )
    std::vector < CPlayer* > sendList; SString strName; int iBandwidth; bool bPersist; CElement* pCallWithElement; CLuaArguments Arguments;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsTable () )
    {
        MinServerReqCheck ( argStream, MIN_SERVER_REQ_TRIGGERCLIENTEVENT_SENDLIST, "a send list is being used" );
        argStream.ReadUserDataTable ( sendList );
    }
    else
    {
        CElement* pElement;
        argStream.ReadIfNextIsUserData ( pElement, CStaticFunctionDefinitions::GetRootElement () );
        if ( !argStream.HasErrors () )
            pElement->GetDescendantsByType ( sendList, CElement::PLAYER );
        markerLatentEvent.Set ( "GetDescendantsByType" );
    }
    argStream.ReadString ( strName );
    argStream.ReadIfNextCouldBeNumber ( iBandwidth, 50000 );
    argStream.ReadIfNextIsBool ( bPersist, false );
    argStream.ReadUserData ( pCallWithElement );
    argStream.ReadLuaArguments ( Arguments );

    if ( !argStream.HasErrors () )
    {
        // Get resource details if transfer should be stopped when resource stops
        CLuaMain* pLuaMain = NULL;
        ushort usResourceNetId = 0xFFFF;
        if ( !bPersist )
        {
            pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CResource* pResource = pLuaMain->GetResource();
                if ( pResource )
                {
                    usResourceNetId = pResource->GetNetID ();
                }
            }
        }

        markerLatentEvent.SetAndStoreString ( SString ( "Get args (%d,%s)", sendList.size (), *strName ) );

        // Trigger it
        if ( CStaticFunctionDefinitions::TriggerLatentClientEvent ( sendList, strName, pCallWithElement, Arguments, iBandwidth, pLuaMain, usResourceNetId ) )
        {
            markerLatentEvent.Set ( "End" );

            // Add debug info if wanted
            if ( CPerfStatDebugInfo::GetSingleton ()->IsActive ( "TriggerLatentClientEvent" ) )
                CPerfStatDebugInfo::GetSingleton ()->AddLine ( "TriggerLatentClientEvent", markerLatentEvent.GetString () );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetLatentEventHandles ( lua_State* luaVM )
{
//  table getLatentEventHandles ( element player )
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        std::vector < uint > resultList;
        g_pGame->GetLatentTransferManager ()->GetSendHandles ( pPlayer->GetSocket (), resultList );

        lua_createtable ( luaVM, 0, resultList.size () );
        for ( uint i = 0 ; i < resultList.size () ; i++ )
        {
            lua_pushnumber ( luaVM, i + 1 );
            lua_pushnumber ( luaVM, resultList[i] );
            lua_settable   ( luaVM, -3 );
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetLatentEventStatus ( lua_State* luaVM )
{
//  int start,end = getLatentEventStatus ( element player, int handle )
    CPlayer* pPlayer; int iHandle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadNumber ( iHandle );

    if ( !argStream.HasErrors () )
    {
        SSendStatus sendStatus;
        if ( g_pGame->GetLatentTransferManager ()->GetSendStatus ( pPlayer->GetSocket (), iHandle, &sendStatus ) )
        {
            lua_createtable ( luaVM, 0, 4 );

            lua_pushstring  ( luaVM, "tickStart" );
            lua_pushinteger ( luaVM, sendStatus.iStartTimeMsOffset );
            lua_settable    ( luaVM, -3 );

            lua_pushstring  ( luaVM, "tickEnd" );
            lua_pushinteger ( luaVM, sendStatus.iEndTimeMsOffset );
            lua_settable    ( luaVM, -3 );

            lua_pushstring  ( luaVM, "totalSize" );
            lua_pushinteger ( luaVM, sendStatus.iTotalSize );
            lua_settable    ( luaVM, -3 );

            lua_pushstring  ( luaVM, "percentComplete" );
            lua_pushinteger ( luaVM, sendStatus.iPercentComplete );
            lua_settable    ( luaVM, -3 );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CancelLatentEvent ( lua_State* luaVM )
{
//  bool cancelLatentEvent ( element player, int handle )
    CPlayer* pPlayer; int iHandle;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadNumber ( iHandle );

    if ( !argStream.HasErrors () )
    {
        if ( g_pGame->GetLatentTransferManager ()->CancelSend ( pPlayer->GetSocket (), iHandle ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerName ( lua_State* luaVM )
{
//  string getPlayerName ( player thePlayer )
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        SString strNick;
        if ( CStaticFunctionDefinitions::GetPlayerName ( pElement, strNick ) )
        {
            lua_pushstring ( luaVM, strNick );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerIP ( lua_State* luaVM )
{
//  string getPlayerIP ( player thePlayer )
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        SString strIP;
        if ( CStaticFunctionDefinitions::GetPlayerIP ( pElement, strIP ) )
        {
            lua_pushstring ( luaVM, strIP );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerVersion ( lua_State* luaVM )
{
//  string getPlayerVersion ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
            SString strPlayerVersion = CStaticFunctionDefinitions::GetPlayerVersion ( pPlayer );
            lua_pushstring ( luaVM, strPlayerVersion );
            return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerAccount ( lua_State* luaVM )
{
//  account getPlayerAccount ( player thePlayer )
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        CAccount* pAccount = CStaticFunctionDefinitions::GetPlayerAccount ( pElement );
        if ( pAccount )
        {
            lua_pushaccount ( luaVM, pAccount );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerName ( lua_State* luaVM )
{
//  bool setPlayerName ( player thePlayer, string newName )
    CElement* pElement; SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerName ( pElement, strName ) )
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

int CLuaFunctionDefinitions::DetonateSatchels ( lua_State* luaVM )
{
//  bool detonateSatchels ( element Player )
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::DetonateSatchels ( pElement ) )
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

int CLuaFunctionDefinitions::SetWeaponProperty ( lua_State* luaVM )
{
//  bool setWeaponProperty ( int weaponID/string weaponName, string weaponSkill, string property/int property, int/float theValue )

    eWeaponSkill eWepSkill = WEAPONSKILL_STD;
    eWeaponType eWep = WEAPONTYPE_BRASSKNUCKLE;
    eWeaponProperty eProp = WEAPON_ACCURACY;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserData ( ) )
    {

        CCustomWeapon * pWeapon;
        eWeaponProperty weaponProperty;
        CScriptArgReader argStream ( luaVM );
        argStream.ReadUserData ( pWeapon );
        argStream.ReadEnumString ( weaponProperty );

        if ( !argStream.HasErrors () )
        {
            if ( weaponProperty == WEAPON_DAMAGE )
            {
                short sData = 0;
                argStream.ReadNumber( sData );
                if ( !argStream.HasErrors( ) )
                {
                    if ( CStaticFunctionDefinitions::SetWeaponProperty ( pWeapon, weaponProperty, sData ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
            }
            else
            {
                float fData = 0.0f;
                argStream.ReadNumber( fData );
                if ( !argStream.HasErrors( ) )
                {
                    if ( CStaticFunctionDefinitions::SetWeaponProperty ( pWeapon, weaponProperty, fData ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
            }
        }
        if ( argStream.HasErrors () )
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }
    else
    {
        argStream.ReadEnumStringOrNumber ( eWep );
        argStream.ReadEnumStringOrNumber ( eWepSkill );
        argStream.ReadEnumString ( eProp );
        if ( !argStream.HasErrors () )
        {
            switch ( eProp )
            {
            case WEAPON_WEAPON_RANGE:
            case WEAPON_TARGET_RANGE:
            case WEAPON_ACCURACY:
            case WEAPON_MOVE_SPEED:
            case WEAPON_ANIM_LOOP_START:
            case WEAPON_ANIM_LOOP_STOP:
            case WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME:
            case WEAPON_ANIM2_LOOP_START:
            case WEAPON_ANIM2_LOOP_STOP:
            case WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME:
            case WEAPON_ANIM_BREAKOUT_TIME:
                {
                    float fWeaponInfo = 0.0f;
                    argStream.ReadNumber ( fWeaponInfo );
                    if ( !argStream.HasErrors () )
                    {
                        if ( CStaticFunctionDefinitions::SetWeaponProperty ( eProp, eWep, eWepSkill, fWeaponInfo ) )
                        {
                            lua_pushboolean ( luaVM, true );
                            return 1;
                        }
                    }
                    break;
                }
            case WEAPON_DAMAGE:
            case WEAPON_MAX_CLIP_AMMO:
            case WEAPON_FLAGS:
                {
                    short sWeaponInfo = 0;
                    argStream.ReadNumber ( sWeaponInfo );
                    if ( !argStream.HasErrors () )
                    {
                        if ( CStaticFunctionDefinitions::SetWeaponProperty ( eProp, eWep, eWepSkill, sWeaponInfo ) )
                        {
                            lua_pushboolean ( luaVM, true );
                            return 1;
                        }
                    }
                    break;
                }
            default:
                {
                    argStream.SetCustomError( "unsupported weapon property at argument 3" );
                    break;
                }

            }
        }
        if ( argStream.HasErrors () )
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }
    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetWeaponProperty ( lua_State* luaVM )
{
//  int getWeaponProperty ( int weaponID/string weaponName, string weaponSkill, string property )

    eWeaponSkill eWepSkill = WEAPONSKILL_STD;
    eWeaponType eWep = WEAPONTYPE_UNARMED;
    eWeaponProperty eProp = WEAPON_INVALID_PROPERTY;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumStringOrNumber ( eWep );
    argStream.ReadEnumStringOrNumber ( eWepSkill );
    argStream.ReadEnumString ( eProp );
    if ( !argStream.HasErrors () )
    {
        switch ( eProp )
        {
        case WEAPON_WEAPON_RANGE:
        case WEAPON_TARGET_RANGE:
        case WEAPON_ACCURACY:
        case WEAPON_FIRING_SPEED:
        case WEAPON_LIFE_SPAN:
        case WEAPON_SPREAD:
        case WEAPON_MOVE_SPEED:
            // Get only
        case WEAPON_REQ_SKILL_LEVEL:
        case WEAPON_ANIM_LOOP_START:
        case WEAPON_ANIM_LOOP_STOP:
        case WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME:
        case WEAPON_ANIM2_LOOP_START:
        case WEAPON_ANIM2_LOOP_STOP:
        case WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME:
        case WEAPON_ANIM_BREAKOUT_TIME:
        case WEAPON_RADIUS:
            {
                float fWeaponInfo = 0.0f;

                if ( CStaticFunctionDefinitions::GetWeaponProperty ( eProp, eWep, eWepSkill, fWeaponInfo ) )
                {
                    lua_pushnumber ( luaVM, fWeaponInfo );
                    return 1;
                }
                break;
            }
        case WEAPON_DAMAGE:
        case WEAPON_MAX_CLIP_AMMO:
        case WEAPON_FLAGS:
        case WEAPON_ANIM_GROUP:
        case WEAPON_FIRETYPE:
        case WEAPON_MODEL:
        case WEAPON_MODEL2:
        case WEAPON_SLOT:
        case WEAPON_AIM_OFFSET:
        case WEAPON_SKILL_LEVEL:
        case WEAPON_DEFAULT_COMBO:
        case WEAPON_COMBOS_AVAILABLE:
            {
                short sWeaponInfo = 0;

                if ( CStaticFunctionDefinitions::GetWeaponProperty ( eProp, eWep, eWepSkill, sWeaponInfo ) )
                {
                    lua_pushinteger ( luaVM, sWeaponInfo );
                    return 1;
                }
                break;
            }
        case WEAPON_FIRE_OFFSET:
            {
                CVector vecWeaponInfo;

                if ( CStaticFunctionDefinitions::GetWeaponProperty ( eProp, eWep, eWepSkill, vecWeaponInfo ) )
                {
                    lua_pushnumber ( luaVM, vecWeaponInfo.fX );
                    lua_pushnumber ( luaVM, vecWeaponInfo.fY );
                    lua_pushnumber ( luaVM, vecWeaponInfo.fZ );
                    return 3;
                }
                break;
            }
        default:
            {
                argStream.SetCustomError( "invalid weapon property at argument 3" );
                break;
            }
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );


    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetOriginalWeaponProperty ( lua_State* luaVM )
{
    eWeaponSkill eWepSkill = WEAPONSKILL_STD;
    eWeaponType eWep = WEAPONTYPE_UNARMED;
    eWeaponProperty eProp = WEAPON_INVALID_PROPERTY;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumStringOrNumber ( eWep );
    argStream.ReadEnumStringOrNumber ( eWepSkill );
    argStream.ReadEnumString ( eProp );
    if ( !argStream.HasErrors () )
    {
        switch ( eProp )
        {
        case WEAPON_WEAPON_RANGE:
        case WEAPON_TARGET_RANGE:
        case WEAPON_ACCURACY:
        case WEAPON_FIRING_SPEED:
        case WEAPON_LIFE_SPAN:
        case WEAPON_SPREAD:
        case WEAPON_MOVE_SPEED:
            // Get only
        case WEAPON_REQ_SKILL_LEVEL:
        case WEAPON_ANIM_LOOP_START:
        case WEAPON_ANIM_LOOP_STOP:
        case WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME:
        case WEAPON_ANIM2_LOOP_START:
        case WEAPON_ANIM2_LOOP_STOP:
        case WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME:
        case WEAPON_ANIM_BREAKOUT_TIME:
        case WEAPON_RADIUS:
            {
                float fWeaponInfo = 0.0f;

                if ( CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eProp, eWep, eWepSkill, fWeaponInfo ) )
                {
                    lua_pushnumber ( luaVM, fWeaponInfo );
                    return 1;
                }
                break;
            }
        case WEAPON_DAMAGE:
        case WEAPON_MAX_CLIP_AMMO:
        case WEAPON_FLAGS:
        case WEAPON_ANIM_GROUP:
        case WEAPON_FIRETYPE:
        case WEAPON_MODEL:
        case WEAPON_MODEL2:
        case WEAPON_SLOT:
        case WEAPON_AIM_OFFSET:
        case WEAPON_SKILL_LEVEL:
        case WEAPON_DEFAULT_COMBO:
        case WEAPON_COMBOS_AVAILABLE:
            {
                short sWeaponInfo = 0;

                if ( CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eProp, eWep, eWepSkill, sWeaponInfo ) )
                {
                    lua_pushinteger ( luaVM, sWeaponInfo );
                    return 1;
                }
                break;
            }
        case WEAPON_FIRE_OFFSET:
            {
                CVector vecWeaponInfo;

                if ( CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eProp, eWep, eWepSkill, vecWeaponInfo ) )
                {
                    lua_pushnumber ( luaVM, vecWeaponInfo.fX );
                    lua_pushnumber ( luaVM, vecWeaponInfo.fY );
                    lua_pushnumber ( luaVM, vecWeaponInfo.fZ );
                    return 3;
                }
                break;
            }
        default:
            {
                argStream.SetCustomError( "unsupported weapon property at argument 3" );
                break;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );


    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::CreatePed ( lua_State* luaVM )
{
    unsigned short usModel;
    CVector vecPosition;
    float fRotation;
    bool bSynced;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(usModel);
    argStream.ReadNumber(vecPosition.fX);
    argStream.ReadNumber(vecPosition.fY);
    argStream.ReadNumber(vecPosition.fZ);
    argStream.ReadNumber(fRotation, 0.0f);
    argStream.ReadBool(bSynced, true);

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain * pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource * pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create the ped and return its handle
                CPed* pPed = CStaticFunctionDefinitions::CreatePed ( pResource, usModel, vecPosition, fRotation, bSynced );
                if ( pPed )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pPed );
                    }
                    lua_pushelement ( luaVM, pPed );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerCount ( lua_State* luaVM )
{
    unsigned int uiPlayerCount = CStaticFunctionDefinitions::GetPlayerCount ();

    lua_pushnumber ( luaVM, uiPlayerCount );
    return 1;
}


int CLuaFunctionDefinitions::GetPedWeapon ( lua_State* luaVM )
{
    CPed* pPed;
    unsigned char ucSlot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);
    argStream.ReadNumber(ucSlot, 0xFF);

    if ( !argStream.HasErrors ( ) )
    {
        if ( ucSlot == 0xFF )
            ucSlot = pPed->GetWeaponSlot ();
        
        CWeapon* pWeapon = pPed->GetWeapon ( ucSlot );
        if ( pWeapon )
        {
            unsigned char ucWeapon = pWeapon->ucType;
            lua_pushnumber ( luaVM, ucWeapon );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedWeaponSlot ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucSlot;
        if ( CStaticFunctionDefinitions::GetPedWeaponSlot ( pPed, ucSlot ) )
        {
            lua_pushnumber ( luaVM, ucSlot );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::reloadPedWeapon ( lua_State* luaVM )
{
    CElement* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::reloadPedWeapon ( pPed ) )
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

int CLuaFunctionDefinitions::IsPedDoingGangDriveby ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        bool bDriveby;
        if ( CStaticFunctionDefinitions::IsPedDoingGangDriveby ( pPed, bDriveby ) )
        {
            lua_pushboolean ( luaVM, bDriveby );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedOnFire ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        bool bOnFire;
        if ( CStaticFunctionDefinitions::IsPedOnFire ( pPed, bOnFire ) )
        {
            lua_pushboolean ( luaVM, bOnFire );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
} 


int CLuaFunctionDefinitions::IsPedHeadless ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        bool bHeadless;
        if ( CStaticFunctionDefinitions::IsPedHeadless ( pPed, bHeadless ) )
        {
            lua_pushboolean ( luaVM, bHeadless );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
} 


int CLuaFunctionDefinitions::IsPedFrozen ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        bool bFrozen;
        if ( CStaticFunctionDefinitions::IsPedFrozen ( pPed, bFrozen ) )
        {
            lua_pushboolean ( luaVM, bFrozen );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
} 


int CLuaFunctionDefinitions::SetPedAnimation ( lua_State* luaVM )
{
// bool setPedAnimation ( ped thePed [, string block=nil, string anim=nil, int time=-1, bool loop=true, bool updatePosition=true, bool interruptable=true, bool freezeLastFrame = true] )
    CElement * pPed;
    SString strBlockName, strAnimName;
    int iTime;
    bool bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame;
    bool bDummy;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);
    if ( argStream.NextIsBool() )
        argStream.ReadBool ( bDummy );      // Wiki used setPedAnimation(source,false) as an example
    else
    if ( argStream.NextIsNil() )
        argStream.m_iIndex++;               // Wiki docs said blockName could be nil
    else
        argStream.ReadString ( strBlockName, "" );
    argStream.ReadString(strAnimName, "");
    if ( argStream.NextCouldBeNumber() )    // Freeroam skips the time arg sometimes
        argStream.ReadNumber(iTime, -1);
    else
        iTime = -1;
    argStream.ReadBool(bLoop, true);
    argStream.ReadBool(bUpdatePosition, true);
    argStream.ReadBool(bInterruptable, true);
    argStream.ReadBool(bFreezeLastFrame, true);

    if ( !argStream.HasErrors ( ) )
    {
        const char *szBlock, *szAnim;
        szBlock = strBlockName.empty() ? NULL : strBlockName.c_str();
        szAnim = strAnimName.empty() ? NULL : strAnimName.c_str();

        if ( CStaticFunctionDefinitions::SetPedAnimation ( pPed, szBlock, szAnim, iTime, bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame ) )
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

int CLuaFunctionDefinitions::SetPedAnimationProgress ( lua_State* luaVM )
{
    CElement* pElement;
    SString strAnimName;
    float fProgress;


    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadString(strAnimName, "");
    argStream.ReadNumber(fProgress, 0.0f);

    if ( !argStream.HasErrors ( ) )
    {
        const char *szAnim;
        szAnim = strAnimName.empty() ? NULL : strAnimName.c_str();

        if ( CStaticFunctionDefinitions::SetPedAnimationProgress ( pElement, szAnim, fProgress ) )
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



int CLuaFunctionDefinitions::SetPedWeaponSlot ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucSlot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucSlot);

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPedWeaponSlot ( pElement, ucSlot ) )
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



int CLuaFunctionDefinitions::SetPedOnFire ( lua_State* luaVM )
{
    CElement* pElement;
    bool bOnFire;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bOnFire);

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPedOnFire ( pElement, bOnFire ) )
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



int CLuaFunctionDefinitions::SetPedHeadless ( lua_State* luaVM )
{
    CElement* pElement;
    bool bHeadless;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bHeadless);

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPedHeadless ( pElement, bHeadless ) )
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


int CLuaFunctionDefinitions::SetPedFrozen ( lua_State* luaVM )
{
    CElement* pElement;
    bool bFrozen;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bFrozen);

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPedFrozen ( pElement, bFrozen ) )
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

int CLuaFunctionDefinitions::GetPedAmmoInClip ( lua_State* luaVM )
{
    CPed* pPed;
    unsigned char ucSlot = 0;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
        argStream.ReadNumber(ucSlot, pPed->GetWeaponSlot());
    
    if ( !argStream.HasErrors ( ) )
    {
        CWeapon* pWeapon = pPed->GetWeapon(ucSlot);
        if ( pWeapon ) 
        {
            lua_pushnumber( luaVM, pWeapon->usAmmoInClip );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedTotalAmmo ( lua_State* luaVM )
{
    CPed* pPed;
    unsigned char ucSlot = 0;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
        argStream.ReadNumber(ucSlot, pPed->GetWeaponSlot());
    
    if ( !argStream.HasErrors ( ) )
    {
        CWeapon* pWeapon = pPed->GetWeapon(ucSlot);
        if ( pWeapon ) 
        {
            lua_pushnumber( luaVM, pWeapon->usAmmo );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerAmmo ( lua_State* luaVM )
{
    CPlayer * pElement;
    unsigned char ucSlot;
    unsigned short usAmmo, usAmmoInClip;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(usAmmo);
    argStream.ReadNumber(ucSlot, 0xFF);
    argStream.ReadNumber(usAmmoInClip, 0);

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPlayerAmmo ( pElement, ucSlot, usAmmo, usAmmoInClip ) )
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


int CLuaFunctionDefinitions::GetPlayerFromName ( lua_State* luaVM )
{
    SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strName);
    
    if ( !argStream.HasErrors ( ) )
    {
        CPlayer* pPlayer = CStaticFunctionDefinitions::GetPlayerFromName ( strName );
        if ( pPlayer )
        {
            // Return the player
            lua_pushelement ( luaVM, pPlayer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetPedArmor ( lua_State* luaVM )
{   
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        float fArmor;
        if ( CStaticFunctionDefinitions::GetPedArmor ( pPed, fArmor ) )
        {
            lua_pushnumber ( luaVM, fArmor );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerMoney ( lua_State* luaVM )
{    
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        long lMoney;

        if ( CStaticFunctionDefinitions::GetPlayerMoney ( pPlayer, lMoney ) )
        {
            lua_pushnumber ( luaVM, lMoney );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedOccupiedVehicle ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        CVehicle* pVehicle = CStaticFunctionDefinitions::GetPedOccupiedVehicle ( pPed );
        if ( pVehicle )
        {
            lua_pushelement ( luaVM, pVehicle );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedOccupiedVehicleSeat ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        unsigned int uiVehicleSeat;
        if ( CStaticFunctionDefinitions::GetPedOccupiedVehicleSeat ( pPed, uiVehicleSeat ) )
        {
            lua_pushnumber ( luaVM, uiVehicleSeat );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetPlayerPing ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        unsigned int uiPing;
        if ( CStaticFunctionDefinitions::GetPlayerPing ( pPlayer, uiPing ) )
        {
            lua_pushnumber ( luaVM, uiPing );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerACInfo ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        lua_newtable ( luaVM );

        lua_pushstring ( luaVM, "DetectedAC" );
        lua_pushstring ( luaVM, pPlayer->m_strDetectedAC );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "d3d9Size" );
        lua_pushnumber ( luaVM, pPlayer->m_uiD3d9Size );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "d3d9MD5" );
        lua_pushstring ( luaVM, pPlayer->m_strD3d9Md5 );
        lua_settable   ( luaVM, -3 );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedRotation ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        float fRotation;
        if ( CStaticFunctionDefinitions::GetPedRotation ( pPed, fRotation ) )
        {
            lua_pushnumber ( luaVM, fRotation );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::GetRandomPlayer ( lua_State* luaVM )
{
    CPlayer* pPlayer = CStaticFunctionDefinitions::GetRandomPlayer ();
    if ( pPlayer )
    {
        lua_pushelement ( luaVM, pPlayer );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedChoking ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushboolean ( luaVM, pPed->IsChoking() );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedDead ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushboolean ( luaVM, pPed->IsDead() );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedDucked ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushboolean ( luaVM, pPed->IsDucked() );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPlayerMuted ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        bool bMuted;
        if ( CStaticFunctionDefinitions::IsPlayerMuted ( pPlayer, bMuted ) )
        {
            lua_pushboolean ( luaVM, bMuted );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedInVehicle ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushboolean ( luaVM, CStaticFunctionDefinitions::GetPedOccupiedVehicle ( pPed ) != NULL );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedStat ( lua_State* luaVM )
{    
    CPed* pPed;
    unsigned short usStat;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);
    argStream.ReadNumber(usStat);

    if ( !argStream.HasErrors ( ) )
    {
        float fValue;
        if ( CStaticFunctionDefinitions::GetPedStat ( pPed, usStat, fValue ) )
        {
            lua_pushnumber ( luaVM, fValue );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedTarget ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        CElement *pTarget = CStaticFunctionDefinitions::GetPedTarget ( pPed );
        if ( pTarget )
        {
            lua_pushelement ( luaVM, pTarget );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::GetPlayerTeam ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        CTeam *pTeam = CStaticFunctionDefinitions::GetPlayerTeam ( pPlayer );
        if ( pTeam )
        {
            lua_pushelement ( luaVM, pTeam );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::GetPedClothes ( lua_State* luaVM )
{
    CPed* pPed;
    unsigned char ucType;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);
    argStream.ReadNumber(ucType);

    if ( !argStream.HasErrors ( ) )
    {
        SString strTexture, strModel;
        if ( CStaticFunctionDefinitions::GetPedClothes ( pPed, ucType, strTexture, strModel ) )
        {
            lua_pushstring ( luaVM, strTexture );
            lua_pushstring ( luaVM, strModel );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::DoesPedHaveJetPack ( lua_State* luaVM )
{    
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        bool bHasJetPack;
        if ( CStaticFunctionDefinitions::DoesPedHaveJetPack ( pPed, bHasJetPack ) )
        {
            lua_pushboolean ( luaVM, bHasJetPack );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedOnGround ( lua_State* luaVM )
{    
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        bool bOnGround;
        if ( CStaticFunctionDefinitions::IsPedOnGround ( pPed, bOnGround ) )
        {
            lua_pushboolean ( luaVM, bOnGround );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CanPlayerUseFunction ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strFunction;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);
    argStream.ReadString(strFunction);

    if ( !argStream.HasErrors ( ) )
    {
        bool bCanUse;
        if ( CStaticFunctionDefinitions::CanPlayerUseFunction ( pPlayer, strFunction, bCanUse ) )
        {
            lua_pushboolean ( luaVM, bCanUse );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerWantedLevel ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        unsigned int uiWantedLevel;
        if ( CStaticFunctionDefinitions::GetPlayerWantedLevel ( pPlayer, uiWantedLevel ) )
        {
            lua_pushnumber ( luaVM, uiWantedLevel );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetAlivePlayers ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Create a new table
        lua_newtable ( luaVM );

        // Add all alive players
        unsigned int uiIndex = 0;
        list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
        for ( ; iter != m_pPlayerManager->IterEnd () ; ++iter )
        {
            if ( (*iter)->IsSpawned () )
            {
                lua_pushnumber ( luaVM, ++uiIndex );
                lua_pushelement ( luaVM, *iter );
                lua_settable ( luaVM, -3 );
            }
        }
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetDeadPlayers ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Create a new table
        lua_newtable ( luaVM );

        // Add all alive players
        unsigned int uiIndex = 0;
        list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
        for ( ; iter != m_pPlayerManager->IterEnd () ; ++iter )
        {
            if ( !(*iter)->IsSpawned () )
            {
                lua_pushnumber ( luaVM, ++uiIndex );
                lua_pushelement ( luaVM, *iter );
                lua_settable ( luaVM, -3 );
            }
        }
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerIdleTime ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        long long llLastPositionChanged = pPlayer->GetPositionLastChanged ();
        if ( llLastPositionChanged == 0 )
        {
            // DO NOT REMOVE THIS AND DEFAULT THE POSITION LAST CHANGED TO THE CURRENT TIME OR YOU WILL BREAK EVERYTHING.
            // He hasn't idled since he just joined so give them 0 idle time
            lua_pushnumber ( luaVM, 0.0 );
            return 1;
        }
        else
        {
            lua_pushnumber ( luaVM, static_cast <double> ( GetTickCount64_ () - llLastPositionChanged ) );
            return 1;
        }
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::IsPlayerScoreboardForced ( lua_State* luaVM )
{    
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        bool bForced;
        if ( CStaticFunctionDefinitions::IsPlayerScoreboardForced ( pPlayer, bForced ) )
        {
            lua_pushboolean ( luaVM, bForced );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPlayerMapForced ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        bool bForced;
        if ( CStaticFunctionDefinitions::IsPlayerMapForced ( pPlayer, bForced ) )
        {
            lua_pushboolean ( luaVM, bForced );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerNametagText ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        SString strText;
        if ( CStaticFunctionDefinitions::GetPlayerNametagText ( pPlayer, strText ) )
        {
            lua_pushstring ( luaVM, strText );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerNametagColor ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {      
        
        unsigned char ucR, ucG, ucB;
        if ( CStaticFunctionDefinitions::GetPlayerNametagColor ( pPlayer, ucR, ucG, ucB ) )
        {
            lua_pushnumber ( luaVM, ucR );
            lua_pushnumber ( luaVM, ucG );
            lua_pushnumber ( luaVM, ucB );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPlayerNametagShowing ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        bool bShowing;
        if ( CStaticFunctionDefinitions::IsPlayerNametagShowing ( pPlayer, bShowing ) )
        {
            lua_pushboolean ( luaVM, bShowing );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedFightingStyle ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucStyle;
        if ( CStaticFunctionDefinitions::GetPedFightingStyle ( pPed, ucStyle ) )
        {
            lua_pushboolean ( luaVM, ucStyle );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedMoveAnim ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        unsigned int uiMoveAnim;
        if ( CStaticFunctionDefinitions::GetPedMoveAnim ( pPed, uiMoveAnim ) )
        {
            lua_pushnumber ( luaVM, uiMoveAnim );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedGravity ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        float fGravity;
        if ( CStaticFunctionDefinitions::GetPedGravity ( pPed, fGravity ) )
        {
            lua_pushnumber ( luaVM, fGravity );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerSerial ( lua_State* luaVM )
{
    CPlayer* pPlayer; uint uiIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData( pPlayer );
    argStream.ReadNumber( uiIndex, 0 );

    if ( !argStream.HasErrors ( ) )
    {
        SString strSerial = CStaticFunctionDefinitions::GetPlayerSerial ( pPlayer, uiIndex );
        if ( !strSerial.empty () )
        {
            lua_pushstring ( luaVM, strSerial );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerUserName ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        SString strUser = CStaticFunctionDefinitions::GetPlayerUserName ( pPlayer );
        if ( !strUser.empty () )
        {
            lua_pushstring ( luaVM, strUser );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetPlayerCommunityID ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        SString strID = CStaticFunctionDefinitions::GetPlayerCommunityID ( pPlayer );
        if ( !strID.empty () )
        {
            lua_pushstring ( luaVM, strID );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerBlurLevel ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucLevel;
        if ( CStaticFunctionDefinitions::GetPlayerBlurLevel ( pPlayer, ucLevel ) )
        {
            lua_pushnumber ( luaVM, ucLevel );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::GetPedContactElement ( lua_State* luaVM )
{
    CPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);

    if ( !argStream.HasErrors ( ) )
    {
        CElement* pContactElement = CStaticFunctionDefinitions::GetPedContactElement ( pPed );
        if ( pContactElement)
        {
            lua_pushelement ( luaVM, pContactElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedArmor ( lua_State* luaVM )
{
    CElement* pElement;
    float fArmor;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(fArmor);

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPedArmor ( pElement, fArmor ) )
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


int CLuaFunctionDefinitions::SetPlayerMoney ( lua_State* luaVM )
{
    CElement* pElement;
    long lMoney;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(lMoney);

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPlayerMoney ( pElement, lMoney ) )
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



int CLuaFunctionDefinitions::GivePlayerMoney ( lua_State* luaVM )
{
    CElement* pElement;
    long lMoney;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(lMoney);

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::GivePlayerMoney ( pElement, lMoney ) )
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



int CLuaFunctionDefinitions::TakePlayerMoney ( lua_State* luaVM )
{
    CElement* pElement;
    long lMoney;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(lMoney);

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::TakePlayerMoney ( pElement, lMoney ) )
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


int CLuaFunctionDefinitions::KillPed ( lua_State* luaVM )
{
    CElement* pElement, *pKiller;
    unsigned char ucKillerWeapon;
    unsigned char ucBodyPart;
    bool bStealth;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadUserData(pKiller, NULL);
    argStream.ReadNumber(ucKillerWeapon, 0xFF);
    argStream.ReadNumber(ucBodyPart, 0xFF);
    argStream.ReadBool(bStealth, false);


    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::KillPed ( pElement, pKiller, ucKillerWeapon, ucBodyPart, bStealth ) )
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


int CLuaFunctionDefinitions::SetPedRotation ( lua_State* luaVM )
{
//  setPedRotation ( element ped, float rotation [, bool fixPedRotation = false ] )
    CElement* pElement; float fRotation; bool bNewWay;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( fRotation );
    argStream.ReadBool ( bNewWay, false );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedRotation ( pElement, fRotation, bNewWay ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SpawnPlayer ( lua_State* luaVM )
{
    CPlayer * pElement;
    CVector vecPosition;
    float fRotation;
    unsigned long ulModel;
    unsigned char ucInterior;
    unsigned short usDimension;
    CTeam * pTeam;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(vecPosition.fX);
    argStream.ReadNumber(vecPosition.fY);
    argStream.ReadNumber(vecPosition.fZ);
    argStream.ReadNumber(fRotation, 0.0f);
    argStream.ReadNumber(ulModel, 0);
    argStream.ReadNumber(ucInterior, 0);
    argStream.ReadNumber(usDimension, 0);
    argStream.ReadUserData(pTeam, NULL);


    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SpawnPlayer ( pElement, vecPosition, fRotation, ulModel, ucInterior, usDimension, pTeam ) )
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

int CLuaFunctionDefinitions::SetPedStat ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned short usStat;
    float fValue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(usStat);
    argStream.ReadNumber(fValue);

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPedStat ( pElement, usStat, fValue ) )
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

int CLuaFunctionDefinitions::AddPedClothes ( lua_State* luaVM )
{
    CElement* pElement;
    SString strTexture;
    SString strModel;
    unsigned char ucType;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadString(strTexture);
    argStream.ReadString(strModel);
    argStream.ReadNumber(ucType);


    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::AddPedClothes ( pElement, strTexture, strModel, ucType ) )
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

int CLuaFunctionDefinitions::RemovePedClothes ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucType; 
    SString strTexture; 
    SString strModel; 
    

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucType); 
    argStream.ReadString(strTexture); 
    argStream.ReadString(strModel); 
    

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::RemovePedClothes( pElement , ucType, strTexture.empty() ? NULL : strTexture.c_str(), strModel.empty() ? NULL : strModel.c_str() ) )
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

int CLuaFunctionDefinitions::GivePedJetPack ( lua_State* luaVM )
{
    CElement* pElement;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::GivePedJetPack( pElement  ) )
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

int CLuaFunctionDefinitions::RemovePedJetPack ( lua_State* luaVM )
{
    CElement* pElement;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::RemovePedJetPack( pElement  ) )
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


int CLuaFunctionDefinitions::ShowPlayerHudComponent ( lua_State* luaVM )
{
//  bool showPlayerHudComponent ( player thePlayer, string component, bool show )
    CElement* pElement; eHudComponent component; bool bShow;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData( pElement );
    argStream.ReadEnumString ( component );
    argStream.ReadBool ( bShow );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ShowPlayerHudComponent ( pElement, component, bShow ) )
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


int CLuaFunctionDefinitions::TakePlayerScreenShot ( lua_State* luaVM )
{
//  bool takePlayerScreenShot ( player thePlayer, int sizeX, int sizeY, string tag, int quality, int maxBandwidth, int maxPacketSize )
    CElement* pElement; uint sizeX; uint sizeY; SString tag; uint quality; uint maxBandwidth; uint maxPacketSize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData( pElement );
    argStream.ReadNumber ( sizeX );
    argStream.ReadNumber ( sizeY );
    argStream.ReadString ( tag, "" );
    argStream.ReadNumber ( quality, 30 );
    argStream.ReadNumber ( maxBandwidth, 5000 );
    argStream.ReadNumber ( maxPacketSize, 500 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        CResource * pResource = pLuaMain ? pLuaMain->GetResource() : NULL;
        if ( pResource )
        {
            if ( CStaticFunctionDefinitions::TakePlayerScreenShot ( pElement, sizeX, sizeY, tag, quality, maxBandwidth, maxPacketSize, pResource->GetName () ) )
            {
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

int CLuaFunctionDefinitions::SetPlayerWantedLevel ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned int uiWantedLevel; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(uiWantedLevel); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPlayerWantedLevel( pElement, uiWantedLevel) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "Valid wanted levels are between 0 and 6 inclusive" );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::ForcePlayerMap ( lua_State* luaVM )
{
    CElement* pElement;
    bool bForce; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bForce); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::ForcePlayerMap( pElement, bForce) )
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


int CLuaFunctionDefinitions::SetPlayerNametagText ( lua_State* luaVM )
{
    CElement* pElement;
    SString strText; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadString(strText); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPlayerNametagText( pElement, strText) )
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


int CLuaFunctionDefinitions::SetPlayerNametagColor ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucR; 
    unsigned char ucG; 
    unsigned char ucB; 
    bool bRemoveOverride = false; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);

    if ( argStream.NextIsBool() )
    {
        argStream.ReadBool(bRemoveOverride); 
        bRemoveOverride =  !bRemoveOverride ? true : false;
    }
    else
    {
        argStream.ReadNumber(ucR); 
        argStream.ReadNumber(ucG); 
        argStream.ReadNumber(ucB); 
    }
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPlayerNametagColor( pElement, bRemoveOverride, ucR, ucG, ucB) )
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

int CLuaFunctionDefinitions::SetPlayerNametagShowing ( lua_State* luaVM )
{
    CElement* pElement;
    bool bShowing; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bShowing); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPlayerNametagShowing( pElement, bShowing) )
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


int CLuaFunctionDefinitions::SetPedFightingStyle ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucStyle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucStyle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPedFightingStyle( pElement, ucStyle) )
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


int CLuaFunctionDefinitions::SetPedMoveAnim ( lua_State* luaVM )
{
    CElement* pElement;
    int iMoveAnim; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(iMoveAnim); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPedMoveAnim( pElement, iMoveAnim) )
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

int CLuaFunctionDefinitions::SetPedGravity ( lua_State* luaVM )
{
    CElement* pElement;
    float fGravity; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(fGravity); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPedGravity( pElement, fGravity) )
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

int CLuaFunctionDefinitions::SetPlayerMuted ( lua_State* luaVM )
{
    CElement* pElement;
    bool bMuted; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bMuted); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPlayerMuted( pElement, bMuted) )
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

int CLuaFunctionDefinitions::SetPlayerBlurLevel ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucLevel; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucLevel); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPlayerBlurLevel( pElement, ucLevel) )
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

int CLuaFunctionDefinitions::RedirectPlayer ( lua_State* luaVM )
{
    CElement* pElement;
    SString strHost; 
    unsigned short usPort; 
    SString strPassword; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadString(strHost); 
    argStream.ReadNumber(usPort); 
    argStream.ReadString(strPassword, ""); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::RedirectPlayer( pElement, strHost, usPort, strPassword.empty() ? NULL : strPassword.c_str() ) )
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


int CLuaFunctionDefinitions::SetPedChoking ( lua_State* luaVM )
{
    CElement* pElement;
    bool bChoking; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bChoking); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPedChoking( pElement, bChoking) )
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


int CLuaFunctionDefinitions::WarpPedIntoVehicle ( lua_State* luaVM )
{
    CPed* pPed;
    CVehicle* pVehicle; 
    unsigned int uiSeat; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPed);
    argStream.ReadUserData(pVehicle); 
    argStream.ReadNumber(uiSeat, 0); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::WarpPedIntoVehicle( pPed, pVehicle, uiSeat) )
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


int CLuaFunctionDefinitions::RemovePedFromVehicle ( lua_State* luaVM )
{
    CElement* pElement;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::RemovePedFromVehicle( pElement) )
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

int CLuaFunctionDefinitions::SetPedDoingGangDriveby ( lua_State* luaVM )
{
    CElement* pElement;
    bool bDoingGangDriveby; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bDoingGangDriveby); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPedDoingGangDriveby( pElement, bDoingGangDriveby) )
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


int CLuaFunctionDefinitions::GiveWeapon ( lua_State* luaVM )
{
// bool giveWeapon ( ped thePlayer, int weapon [, int ammo=30, bool setAsCurrent=false ] )
    CElement* pElement; eWeaponType weaponType; ushort usAmmo; bool bSetAsCurrent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadEnumStringOrNumber ( weaponType );
    argStream.ReadNumber ( usAmmo, 30 );
    argStream.ReadBool ( bSetAsCurrent, false );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::GiveWeapon ( pElement, weaponType, usAmmo, bSetAsCurrent ) )
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


int CLuaFunctionDefinitions::TakeWeapon ( lua_State* luaVM )
{
// bool takeWeapon ( player thePlayer, int weaponId [, int ammo ] )
    CElement* pElement; eWeaponType weaponType; ushort usAmmo;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadEnumStringOrNumber ( weaponType );
    argStream.ReadNumber ( usAmmo, 9999 );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::TakeWeapon ( pElement, weaponType, usAmmo ) )
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

int CLuaFunctionDefinitions::TakeAllWeapons ( lua_State* luaVM )
{
    CElement* pElement;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::TakeAllWeapons( pElement) )
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

int CLuaFunctionDefinitions::SetWeaponAmmo ( lua_State* luaVM )
{
// bool setWeaponAmmo ( player thePlayer, int weapon, int totalAmmo, [int ammoInClip = 0] )
    CElement* pElement; 
    eWeaponType weaponType; 
    ushort usAmmo; 
    ushort usAmmoInClip;
    CCustomWeapon * pWeapon = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    if ( pElement->GetType() != CElement::WEAPON )
    {
        argStream.ReadEnumStringOrNumber ( weaponType );
        argStream.ReadNumber ( usAmmo );
        argStream.ReadNumber ( usAmmoInClip, 0 );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWeaponAmmo ( pElement, weaponType, usAmmo, usAmmoInClip ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }
    else
    {
        pWeapon = static_cast < CCustomWeapon * > ( pElement );
        argStream.ReadNumber ( usAmmo );

        if ( !argStream.HasErrors() )
        {
            if ( CStaticFunctionDefinitions::SetWeaponAmmo ( pWeapon, usAmmo ) )
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
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetSlotFromWeapon ( lua_State* luaVM )
{
    eWeaponType weaponType;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumStringOrNumber( weaponType );

    if ( !argStream.HasErrors ( ) )
    {
        char cSlot = CWeaponNames::GetSlotFromWeapon ( weaponType );
        if ( cSlot >= 0 )
            lua_pushnumber ( luaVM, cSlot );
        else
            lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateVehicle ( lua_State* luaVM )
{
//  vehicle createVehicle ( int model, float x, float y, float z, [float rx, float ry, float rz, string numberplate, bool bDirection, int variant1, int variant2] )
    ushort usModel; CVector vecPosition; CVector vecRotation; SString strNumberPlate; uchar ucVariant; uchar ucVariant2;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( usModel );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( vecRotation.fX, 0 );
    argStream.ReadNumber ( vecRotation.fY, 0 );
    argStream.ReadNumber ( vecRotation.fZ, 0 );
    argStream.ReadString ( strNumberPlate, "" );
    if ( argStream.NextIsBool () )
    {
        bool bDirection;
        argStream.ReadBool ( bDirection );
    }
    argStream.ReadNumber ( ucVariant, 254 );
    argStream.ReadNumber ( ucVariant2, 254 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource * pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create the vehicle and return its handle
                CVehicle* pVehicle = CStaticFunctionDefinitions::CreateVehicle ( pResource, usModel, vecPosition, vecRotation, strNumberPlate, ucVariant, ucVariant2 );
                if ( pVehicle )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pVehicle );
                    }
                    lua_pushelement ( luaVM, pVehicle );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleType ( lua_State* luaVM )
{
    unsigned long ulModel; 
    
    CScriptArgReader argStream ( luaVM );

    if ( !argStream.NextIsNumber ( ) )
    {
        CVehicle* pVehicle;
        argStream.ReadUserData(pVehicle);
        ulModel = pVehicle->GetModel ( );
    }
    else
        argStream.ReadNumber(ulModel); 
    
    if ( !argStream.HasErrors ( ) )
    {   
        if ( ulModel >= 400 && ulModel < 610 )
            lua_pushstring ( luaVM, CVehicleNames::GetVehicleTypeName ( ulModel ) );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetVehicleVariant ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    unsigned char ucVariant; 
    unsigned char ucVariant2; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(ucVariant, 0xFE); 
    argStream.ReadNumber(ucVariant2, 0xFE); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleVariant( pVehicle, ucVariant, ucVariant2) )
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

int CLuaFunctionDefinitions::GetVehicleVariant ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucVariant = -1;
        unsigned char ucVariant2 = -1;
        if ( CStaticFunctionDefinitions::GetVehicleVariant ( pVehicle, ucVariant, ucVariant2 ) )
        {
            lua_pushnumber ( luaVM, ucVariant );
            lua_pushnumber ( luaVM, ucVariant2 );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleColor ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    bool bRGB; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    argStream.ReadBool(bRGB); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CVehicleColor color;
        if ( CStaticFunctionDefinitions::GetVehicleColor ( pVehicle, color ) )
        {

            if ( bRGB )
            {
                for ( uint i = 0 ; i < 4 ; i++ )
                {
                    SColor RGBColor = color.GetRGBColor ( i );
                    lua_pushnumber ( luaVM, RGBColor.R );
                    lua_pushnumber ( luaVM, RGBColor.G );
                    lua_pushnumber ( luaVM, RGBColor.B );
                }
                return 12;
            }
            else
            {
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 0 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 1 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 2 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 3 ) );
                return 4;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleModelFromName ( lua_State* luaVM )
{
    SString strName; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strName); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned short usModel;
        if ( CStaticFunctionDefinitions::GetVehicleModelFromName ( strName, usModel ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( usModel ) );
            return 1;
        }	
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleLandingGearDown ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CVehicleManager::HasLandingGears ( pVehicle->GetModel () ) )
        {
            lua_pushboolean ( luaVM, pVehicle->IsLandingGearDown () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetVehicleLocked ( lua_State* luaVM )
{
    CElement* pElement;
    bool bLocked; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bLocked); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleLocked( pElement, bLocked) )
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

int CLuaFunctionDefinitions::SetVehicleDoorsUndamageable ( lua_State* luaVM )
{
    CElement* pElement;
    bool bDoorsUndamageable; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bDoorsUndamageable); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDoorsUndamageable( pElement, bDoorsUndamageable) )
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

int CLuaFunctionDefinitions::RemoveVehicleSirens ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;

    argStream.ReadUserData ( pVehicle );
    
    if ( argStream.HasErrors ( ) == false )
    {
        if ( pVehicle )
        {
            if ( CStaticFunctionDefinitions::RemoveVehicleSirens ( pVehicle ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetVehicleSirens( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    unsigned char ucSirenID = 0;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucSirenID );
    if ( ucSirenID > 0 && ucSirenID < 9 )
    {
        // Array indicies start at 0 so compensate here. This way all code works properly and we get nice 1-8 numbers for API
        ucSirenID--;
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_vecSirenPositions.fX );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_vecSirenPositions.fY );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_vecSirenPositions.fZ );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_RGBBeaconColour.R );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_RGBBeaconColour.G );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_RGBBeaconColour.B );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_RGBBeaconColour.A, 255 );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_dwMinSirenAlpha, 0 );
        if ( argStream.HasErrors ( ) == false )
        {
            if ( pVehicle )
            {
                if ( CStaticFunctionDefinitions::SetVehicleSirens ( pVehicle, ucSirenID, tSirenInfo ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}
int CLuaFunctionDefinitions::GetVehicleSirenParams( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    if ( argStream.HasErrors ( ) == false )
    {
        if ( pVehicle )
        {
            tSirenInfo = pVehicle->m_tSirenBeaconInfo;// Create a new table
            lua_newtable ( luaVM );

            lua_pushstring( luaVM, "SirenCount" );
            lua_pushnumber ( luaVM, tSirenInfo.m_ucSirenCount );
            lua_settable ( luaVM, -3 ); // End of SirenCount Property

            lua_pushstring( luaVM, "SirenType" );
            lua_pushnumber ( luaVM, tSirenInfo.m_ucSirenType );
            lua_settable ( luaVM, -3 ); // End of SirenType Property

            lua_pushstring( luaVM, "Flags" );
            lua_newtable ( luaVM );

            lua_pushstring ( luaVM, "360" );
            lua_pushboolean ( luaVM, tSirenInfo.m_b360Flag );
            lua_settable ( luaVM, -3 ); // End of 360 Property

            lua_pushstring ( luaVM, "DoLOSCheck" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bDoLOSCheck );
            lua_settable ( luaVM, -3 ); // End of DoLOSCheck Property

            lua_pushstring ( luaVM, "UseRandomiser" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bUseRandomiser );
            lua_settable ( luaVM, -3 ); // End of UseRandomiser Property

            lua_pushstring ( luaVM, "Silent" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bSirenSilent );
            lua_settable ( luaVM, -3 ); // End of Silent Property

            lua_settable ( luaVM, -3 ); // End of Flags table

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleSirens( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    if ( argStream.HasErrors ( ) == false )
    {
        if ( pVehicle )
        {
            tSirenInfo = pVehicle->m_tSirenBeaconInfo;// Create a new table
            lua_newtable ( luaVM );

            for ( int i = 0; i < tSirenInfo.m_ucSirenCount;i++ )
            {
                lua_pushnumber ( luaVM, i+1 );
                lua_newtable ( luaVM );

                SSirenBeaconInfo info = tSirenInfo.m_tSirenInfo[i];

                lua_pushstring( luaVM, "Min_Alpha" );
                lua_pushnumber ( luaVM, info.m_dwMinSirenAlpha );
                lua_settable ( luaVM, -3 ); // End of Min_Alpha property

                lua_pushstring( luaVM, "Red" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.R );
                lua_settable ( luaVM, -3 ); // End of Red property

                lua_pushstring( luaVM, "Green" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.G );
                lua_settable ( luaVM, -3 ); // End of Green property

                lua_pushstring( luaVM, "Blue" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.B );
                lua_settable ( luaVM, -3 ); // End of Blue property

                lua_pushstring( luaVM, "Alpha" );
                lua_pushnumber ( luaVM, info.m_RGBBeaconColour.A );
                lua_settable ( luaVM, -3 ); // End of Alpha property

                lua_pushstring( luaVM, "x" );
                lua_pushnumber ( luaVM, info.m_vecSirenPositions.fX );
                lua_settable ( luaVM, -3 ); // End of X property

                lua_pushstring( luaVM, "y" );
                lua_pushnumber ( luaVM, info.m_vecSirenPositions.fY );
                lua_settable ( luaVM, -3 ); // End of Y property

                lua_pushstring( luaVM, "z" );
                lua_pushnumber ( luaVM, info.m_vecSirenPositions.fZ );
                lua_settable ( luaVM, -3 ); // End of Z property

                lua_settable ( luaVM, -3 ); // End of Table
            }

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GiveVehicleSirens ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CVehicle* pVehicle = NULL;
    unsigned char ucSirenType = 0;
    unsigned char ucSirenCount = 0;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucSirenCount );
    argStream.ReadNumber ( ucSirenType );
    if ( ucSirenCount > 0)
    {
        argStream.ReadBool ( tSirenInfo.m_b360Flag, false );
        argStream.ReadBool ( tSirenInfo.m_bDoLOSCheck, true );
        argStream.ReadBool ( tSirenInfo.m_bUseRandomiser, true );
        argStream.ReadBool ( tSirenInfo.m_bSirenSilent, false );
        if ( argStream.HasErrors ( ) == false )
        {
            if ( pVehicle )
            {
                if ( CStaticFunctionDefinitions::GiveVehicleSirens ( pVehicle, ucSirenType, ucSirenCount, tSirenInfo ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleMaxPassengers ( lua_State* luaVM )
{
    unsigned int uiModel = 0; 
    
    CScriptArgReader argStream ( luaVM );
    
    if ( argStream.NextIsUserData ( ) )
    {
        CVehicle * pVehicle;
        argStream.ReadUserData(pVehicle);
        
        if ( !argStream.HasErrors ( ) )
            uiModel = pVehicle->GetModel();
    }
    else
        argStream.ReadNumber(uiModel); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if (!CVehicleManager::IsValidModel(uiModel))
        {
            m_pScriptDebugging->LogBadType ( luaVM );
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        unsigned int uiMaxPassengers = CVehicleManager::GetMaxPassengers ( uiModel );

        if (uiMaxPassengers != 0xFF)
        {
            lua_pushnumber ( luaVM, uiMaxPassengers );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleName ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        SString strVehicleName;
        if ( CStaticFunctionDefinitions::GetVehicleName ( pVehicle, strVehicleName ) )
        {
            lua_pushstring ( luaVM, strVehicleName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleNameFromModel ( lua_State* luaVM )
{
    unsigned short usModel; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(usModel); 
    
    if ( !argStream.HasErrors ( ) )
    {
        SString strVehicleName;

        if ( CStaticFunctionDefinitions::GetVehicleNameFromModel ( usModel, strVehicleName ) )
        {
            lua_pushstring ( luaVM, strVehicleName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleOccupant ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    unsigned int uiSeat; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    argStream.ReadNumber(uiSeat, 0); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CPed* pPed = CStaticFunctionDefinitions::GetVehicleOccupant ( pVehicle, uiSeat );
        if ( pPed )
        {
            lua_pushelement ( luaVM, pPed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleOccupants ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        // Create a new table
        lua_newtable ( luaVM );

        // Get the maximum amount of passengers
        unsigned char ucMaxPassengers = pVehicle->GetMaxPassengers ();

        // Make sure that if the vehicle doesn't have any seats, the function returns false
        if ( ucMaxPassengers == 255 )
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        // Add All Occupants
        for ( unsigned char ucSeat = 0; ucSeat <= ucMaxPassengers; ++ ucSeat )
        {
            CPed* pPed = pVehicle->GetOccupant ( ucSeat );
            if ( pPed )
            {
                lua_pushnumber ( luaVM, ucSeat );
                lua_pushelement ( luaVM, pPed );
                lua_settable ( luaVM, -3 );
            }
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleController ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle);
    
    if ( !argStream.HasErrors ( ) )
    {
        CPed* pPed = CStaticFunctionDefinitions::GetVehicleController ( pVehicle );
        if ( pPed )
        {
            lua_pushelement ( luaVM, pPed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleRotation ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CVector vecRotation;
        if ( CStaticFunctionDefinitions::GetVehicleRotation ( pVehicle, vecRotation ) )
        {
            lua_pushnumber ( luaVM, vecRotation.fX );
            lua_pushnumber ( luaVM, vecRotation.fY );
            lua_pushnumber ( luaVM, vecRotation.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleSirensOn ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        // Does the vehicle have Sirens?
        if ( CVehicleManager::HasSirens ( pVehicle->GetModel () ) || pVehicle->DoesVehicleHaveSirens () )
        {
            lua_pushboolean ( luaVM, pVehicle->IsSirenActive () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleTurnVelocity ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CVector vecTurnVelocity;
        if ( CStaticFunctionDefinitions::GetVehicleTurnVelocity ( pVehicle, vecTurnVelocity ) )
        {
            lua_pushnumber ( luaVM, vecTurnVelocity.fX );
            lua_pushnumber ( luaVM, vecTurnVelocity.fY );
            lua_pushnumber ( luaVM, vecTurnVelocity.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleTurretPosition ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CVector2D vecPosition;
        if ( CStaticFunctionDefinitions::GetVehicleTurretPosition ( pVehicle, vecPosition ) )
        {
            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsVehicleLocked ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        bool bLocked;
        if ( CStaticFunctionDefinitions::IsVehicleLocked ( pVehicle, bLocked ) )
        {
            lua_pushboolean ( luaVM, bLocked );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehiclesOfType ( lua_State* luaVM )
{
    unsigned int uiModel;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(uiModel); 
    
    if ( !argStream.HasErrors ( ) )
    {
        // Create a new table
        lua_newtable ( luaVM );

        // Add all the vehicles with a matching model
        m_pVehicleManager->GetVehiclesOfType ( uiModel, luaVM );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleUpgradeOnSlot ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    unsigned char ucSlot; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    argStream.ReadNumber(ucSlot); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned short usUpgrade;
        if ( CStaticFunctionDefinitions::GetVehicleUpgradeOnSlot ( pVehicle, ucSlot, usUpgrade ) )
        {
            lua_pushnumber ( luaVM, usUpgrade );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleUpgradeSlotName ( lua_State* luaVM )
{
    unsigned short usNumber; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(usNumber); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( usNumber < 17 )
        {
            SString strUpgradeName;
            if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( static_cast < unsigned char >( usNumber ), strUpgradeName ) )
            {
                lua_pushstring ( luaVM, strUpgradeName );
                return 1;
            }
        }
        else if ( usNumber >= 1000 && usNumber <= 1193 )
        {
            SString strUpgradeName;
            if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( usNumber, strUpgradeName )  )
            {
                lua_pushstring ( luaVM, strUpgradeName );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleUpgrades ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
        if ( pUpgrades )
        {
            // Create a new table
            lua_newtable ( luaVM );

            // Add all the upgrades to the table
            const SSlotStates& usSlotStates = pUpgrades->GetSlotStates ();

            unsigned int uiIndex = 0;
            unsigned char ucSlot = 0;
            for ( ; ucSlot < VEHICLE_UPGRADE_SLOTS ; ucSlot++ )
            {
                if ( usSlotStates [ucSlot] != 0 )
                {
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushnumber ( luaVM, usSlotStates [ ucSlot ] );
                    lua_settable ( luaVM, -3 );
                }
            }

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleCompatibleUpgrades ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    unsigned char ucSlot; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    argStream.ReadNumber(ucSlot, 0xFF); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
        if ( pUpgrades )
        {
            // Create a new table
            lua_newtable ( luaVM );

            unsigned int uiIndex = 0;
            for ( unsigned short usUpgrade = 1000 ; usUpgrade <= 1193 ; usUpgrade++ )
            {
                if ( pUpgrades->IsUpgradeCompatible ( usUpgrade ) )
                {
                    if ( ucSlot != 0xFF )
                    {
                        unsigned char ucUpgradeSlot;
                        if ( !pUpgrades->GetSlotFromUpgrade ( usUpgrade, ucUpgradeSlot ) )
                            continue;

                        if ( ucUpgradeSlot != ucSlot )
                            continue;
                    }

                    // Add this one to a list
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushnumber ( luaVM, usUpgrade );
                    lua_settable ( luaVM, -3 );
                }
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleDoorState ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    unsigned char ucDoor; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    argStream.ReadNumber(ucDoor); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucState;
        if ( CStaticFunctionDefinitions::GetVehicleDoorState ( pVehicle, ucDoor, ucState ) )
        {
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleWheelStates ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucFrontLeft, ucRearLeft, ucFrontRight, ucRearRight;
        if ( CStaticFunctionDefinitions::GetVehicleWheelStates ( pVehicle, ucFrontLeft, ucRearLeft, ucFrontRight, ucRearRight ) )
        {
            lua_pushnumber ( luaVM, ucFrontLeft );
            lua_pushnumber ( luaVM, ucRearLeft );
            lua_pushnumber ( luaVM, ucFrontRight );
            lua_pushnumber ( luaVM, ucRearRight );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleLightState ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    unsigned char ucLight; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    argStream.ReadNumber(ucLight); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucState;
        if ( CStaticFunctionDefinitions::GetVehicleLightState ( pVehicle, ucLight, ucState ) )
        {
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::GetVehiclePanelState ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    unsigned char ucPanel; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    argStream.ReadNumber(ucPanel); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucState;
        if ( CStaticFunctionDefinitions::GetVehiclePanelState ( pVehicle, ucPanel, ucState ) )
        {
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleOverrideLights ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucLights;
        if ( CStaticFunctionDefinitions::GetVehicleOverrideLights ( pVehicle, ucLights ) )
        {
            lua_pushnumber ( luaVM, ucLights );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleTowedByVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CVehicle* pTowedVehicle = pVehicle->GetTowedVehicle ();
        if ( pTowedVehicle )
        {
            lua_pushelement ( luaVM, pTowedVehicle );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleTowingVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CVehicle* pTowedByVehicle = pVehicle->GetTowedByVehicle ();
        if ( pTowedByVehicle )
        {
            lua_pushelement ( luaVM, pTowedByVehicle );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehiclePaintjob ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucPaintjob;
        if ( CStaticFunctionDefinitions::GetVehiclePaintjob ( pVehicle, ucPaintjob ) )
        {
            lua_pushnumber ( luaVM, ucPaintjob );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehiclePlateText ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        char szPlateText [ 9 ] = { 0 }; // 8 chars + \0
        if ( CStaticFunctionDefinitions::GetVehiclePlateText ( pVehicle, szPlateText ) )
        {
            lua_pushstring ( luaVM, szPlateText );
            return 1;
        }	
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::IsVehicleDamageProof ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        bool bDamageProof;
        if ( CStaticFunctionDefinitions::IsVehicleDamageProof ( pVehicle, bDamageProof ) )
        {
            lua_pushboolean ( luaVM, bDamageProof );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::IsVehicleFuelTankExplodable ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        bool bExplodable;
        if ( CStaticFunctionDefinitions::IsVehicleFuelTankExplodable ( pVehicle, bExplodable ) )
        {
            lua_pushboolean ( luaVM, bExplodable );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::IsVehicleFrozen ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        bool bFrozen = false;
        if ( CStaticFunctionDefinitions::IsVehicleFrozen ( pVehicle, bFrozen ) )
        {
            lua_pushboolean ( luaVM, bFrozen );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::IsVehicleOnGround ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        bool bOnGround;
        if ( CStaticFunctionDefinitions::IsVehicleOnGround ( pVehicle, bOnGround ) )
        {
            lua_pushboolean ( luaVM, bOnGround );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleEngineState ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        bool bState;
        if ( CStaticFunctionDefinitions::GetVehicleEngineState ( pVehicle, bState ) )
        {
            lua_pushboolean ( luaVM, bState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsTrainDerailed ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        bool bDerailed;
        if ( CStaticFunctionDefinitions::IsTrainDerailed ( pVehicle, bDerailed ) )
        {
            lua_pushboolean ( luaVM, bDerailed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::IsTrainDerailable ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        bool bDerailable;
        if ( CStaticFunctionDefinitions::IsTrainDerailable ( pVehicle, bDerailable ) )
        {
            lua_pushboolean ( luaVM, bDerailable );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTrainDirection ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    { 
        bool bDirection;
        if ( CStaticFunctionDefinitions::GetTrainDirection ( pVehicle, bDirection ) )
        {
            lua_pushboolean ( luaVM, bDirection );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetTrainSpeed ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        float fSpeed;
        if ( CStaticFunctionDefinitions::GetTrainSpeed ( pVehicle, fSpeed ) )
        {
            lua_pushnumber ( luaVM, fSpeed );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::FixVehicle ( lua_State* luaVM )
{
    CElement* pElement;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FixVehicle( pElement) )
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


int CLuaFunctionDefinitions::BlowVehicle ( lua_State* luaVM )
{
    CElement* pElement;
    bool bExplode; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bExplode, true); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::BlowVehicle( pElement, bExplode) )
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


int CLuaFunctionDefinitions::IsVehicleBlown ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::IsVehicleBlown(pVehicle) ) 
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

int CLuaFunctionDefinitions::GetVehicleHeadLightColor ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        SColor color;
        if ( CStaticFunctionDefinitions::GetVehicleHeadLightColor ( pVehicle, color ) )
        {
            lua_pushnumber ( luaVM, color.R );
            lua_pushnumber ( luaVM, color.G );
            lua_pushnumber ( luaVM, color.B );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetVehicleRotation ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecRotation;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(vecRotation.fX); 
    argStream.ReadNumber(vecRotation.fY); 
    argStream.ReadNumber(vecRotation.fZ); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleRotation( pElement, vecRotation) )
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

int CLuaFunctionDefinitions::SetVehicleTurnVelocity ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecTurnVelocity;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(vecTurnVelocity.fX); 
    argStream.ReadNumber(vecTurnVelocity.fY); 
    argStream.ReadNumber(vecTurnVelocity.fZ); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleTurnVelocity( pElement, vecTurnVelocity) )
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

int CLuaFunctionDefinitions::SetVehicleColor ( lua_State* luaVM )
{
    CElement* pElement;
    uchar ucParams[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);

    int i = 0;
    for (; i < 12; ++i )
    {
        if ( argStream.NextIsNumber ( ) )
        {
            argStream.ReadNumber(ucParams[i]);
        }
        else
            break;
    }

    if ( !argStream.HasErrors ( ) )
    {
        CVehicleColor color;

        if ( i == 4 )
        {
            // 4 args mean palette colours
            color.SetPaletteColors ( ucParams[0], ucParams[1], ucParams[2], ucParams[3] );
        }
        else if ( i % 3 == 0)
        {
            // 3,6,9 or 12 args mean rgb colours
            color.SetRGBColors ( SColorRGBA ( ucParams[0], ucParams[1], ucParams[2], 0 ),
                SColorRGBA ( ucParams[3], ucParams[4], ucParams[5], 0 ),
                SColorRGBA ( ucParams[6], ucParams[7], ucParams[8], 0 ),
                SColorRGBA ( ucParams[9], ucParams[10], ucParams[11], 0 ) );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );

        if ( CStaticFunctionDefinitions::SetVehicleColor ( pElement, color ) )
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


int CLuaFunctionDefinitions::SetVehicleLandingGearDown ( lua_State* luaVM )
{
    CElement* pElement;
    bool bLandingGearDown; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bLandingGearDown); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleLandingGearDown( pElement, bLandingGearDown) )
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


int CLuaFunctionDefinitions::IsVehicleTaxiLightOn ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    
    if ( !argStream.HasErrors ( ) )
    {
        lua_pushboolean ( luaVM, pVehicle->IsTaxiLightOn() );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleTaxiLightOn ( lua_State* luaVM )
{
    CElement* pElement;
    bool bTaxiLightOn; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bTaxiLightOn); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleTaxiLightOn( pElement, bTaxiLightOn) )
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

int CLuaFunctionDefinitions::SetVehicleSirensOn ( lua_State* luaVM )
{
    CElement* pElement;
    bool bSirensOn; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bSirensOn); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleSirensOn( pElement, bSirensOn) )
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

int CLuaFunctionDefinitions::AddVehicleUpgrade ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned short usUpgrade; 
    bool bAll = false;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);

    if ( argStream.NextIsString ( ) )
    {
        SString strAll;
        argStream.ReadString(strAll);
        bAll = strAll == "all";
        usUpgrade = 0;
    }
    else
        argStream.ReadNumber(usUpgrade);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( bAll)
        {
            if ( CStaticFunctionDefinitions::AddAllVehicleUpgrades ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else if ( CStaticFunctionDefinitions::AddVehicleUpgrade( pElement, usUpgrade) )
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


int CLuaFunctionDefinitions::RemoveVehicleUpgrade ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned short usUpgrade; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(usUpgrade); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::RemoveVehicleUpgrade( pElement, usUpgrade) )
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

int CLuaFunctionDefinitions::SetVehicleDoorState ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucDoor; 
    unsigned char ucState; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucDoor); 
    argStream.ReadNumber(ucState); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDoorState( pElement, ucDoor, ucState) )
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


int CLuaFunctionDefinitions::SetVehicleWheelStates ( lua_State* luaVM )
{
    CElement* pElement;
    int iFrontLeft; 
    int iRearLeft; 
    int iFrontRight; 
    int iRearRight; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(iFrontLeft); 
    argStream.ReadNumber(iRearLeft, -1); 
    argStream.ReadNumber(iFrontRight, -1); 
    argStream.ReadNumber(iRearRight, -1); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleWheelStates( pElement, iFrontLeft, iRearLeft, iFrontRight, iRearRight) )
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


int CLuaFunctionDefinitions::SetVehicleLightState ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucLight; 
    unsigned char ucState; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucLight); 
    argStream.ReadNumber(ucState); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleLightState( pElement, ucLight, ucState) )
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


int CLuaFunctionDefinitions::SetVehiclePanelState ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucPanel; 
    unsigned char ucState; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucPanel); 
    argStream.ReadNumber(ucState); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehiclePanelState( pElement, ucPanel, ucState) )
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


int CLuaFunctionDefinitions::ToggleVehicleRespawn ( lua_State* luaVM )
{
    CElement* pElement;
    bool bRespawn; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bRespawn); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::ToggleVehicleRespawn( pElement, bRespawn) )
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

int CLuaFunctionDefinitions::SetVehicleRespawnDelay ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned long ulTime; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ulTime); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleRespawnDelay( pElement, ulTime) )
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

int CLuaFunctionDefinitions::SetVehicleRespawnPosition ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecPosition; 
    CVector vecRotation; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(vecPosition.fZ); 
    argStream.ReadNumber(vecRotation.fX, 0.0f); 
    argStream.ReadNumber(vecRotation.fY, 0.0f); 
    argStream.ReadNumber(vecRotation.fZ, 0.0f); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleRespawnPosition( pElement, vecPosition, vecRotation) )
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

int CLuaFunctionDefinitions::SetVehicleIdleRespawnDelay ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned long ulTime; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ulTime); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleIdleRespawnDelay( pElement, ulTime) )
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


int CLuaFunctionDefinitions::RespawnVehicle ( lua_State* luaVM )
{
    CElement* pElement;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::RespawnVehicle( pElement) )
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


int CLuaFunctionDefinitions::ResetVehicleExplosionTime ( lua_State* luaVM )
{
    CElement* pElement;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::ResetVehicleExplosionTime( pElement) )
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

int CLuaFunctionDefinitions::ResetVehicleIdleTime ( lua_State* luaVM )
{
    CElement* pElement;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::ResetVehicleIdleTime( pElement) )
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


int CLuaFunctionDefinitions::SpawnVehicle ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecPosition; 
    CVector vecRotation; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(vecPosition.fZ); 
    argStream.ReadNumber(vecRotation.fX, 0.0f); 
    argStream.ReadNumber(vecRotation.fY, 0.0f); 
    argStream.ReadNumber(vecRotation.fZ, 0.0f); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SpawnVehicle ( pElement, vecPosition, vecRotation ) )
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


int CLuaFunctionDefinitions::SetVehicleOverrideLights ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucLights; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucLights); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( ucLights <= 2 && CStaticFunctionDefinitions::SetVehicleOverrideLights( pElement, ucLights) )
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

int CLuaFunctionDefinitions::AttachTrailerToVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    CVehicle* pTrailer; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle);
    argStream.ReadUserData(pTrailer); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::AttachTrailerToVehicle( pVehicle, pTrailer) )
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


int CLuaFunctionDefinitions::DetachTrailerFromVehicle ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    CVehicle* pTrailer; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle);
    argStream.ReadUserData(pTrailer, NULL); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::DetachTrailerFromVehicle( pVehicle, pTrailer) )
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

int CLuaFunctionDefinitions::SetVehicleEngineState ( lua_State* luaVM )
{
    CElement* pElement;
    bool bState; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bState); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleEngineState( pElement, bState) )
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


int CLuaFunctionDefinitions::SetVehicleDirtLevel ( lua_State* luaVM )
{
    CElement* pElement;
    float fDirtLevel; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(fDirtLevel); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDirtLevel( pElement, fDirtLevel) )
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

int CLuaFunctionDefinitions::SetVehicleDamageProof ( lua_State* luaVM )
{
    CElement* pElement;
    bool bDamageProof; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bDamageProof); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDamageProof( pElement, bDamageProof) )
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


int CLuaFunctionDefinitions::SetVehiclePaintjob ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucPaintjob; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucPaintjob); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehiclePaintjob( pElement, ucPaintjob) )
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

int CLuaFunctionDefinitions::SetVehicleFuelTankExplodable ( lua_State* luaVM )
{
    CElement* pElement;
    bool bExplodable; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bExplodable); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleFuelTankExplodable( pElement, bExplodable) )
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

int CLuaFunctionDefinitions::SetVehicleFrozen ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bFrozen; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle);
    argStream.ReadBool(bFrozen); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleFrozen( pVehicle, bFrozen) )
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

int CLuaFunctionDefinitions::SetTrainDerailed ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bDerailed; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle);
    argStream.ReadBool(bDerailed); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetTrainDerailed( pVehicle, bDerailed) )
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


int CLuaFunctionDefinitions::SetTrainDerailable ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bDerailable; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle);
    argStream.ReadBool(bDerailable); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetTrainDerailable( pVehicle, bDerailable) )
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


int CLuaFunctionDefinitions::SetTrainDirection ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    bool bDirection; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle);
    argStream.ReadBool(bDirection); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetTrainDirection( pVehicle, bDirection) )
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


int CLuaFunctionDefinitions::SetTrainSpeed ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    float fSpeed; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(fSpeed); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetTrainSpeed( pVehicle, fSpeed) )
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

int CLuaFunctionDefinitions::SetVehicleHeadLightColor ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    SColor color;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(color.R); 
    argStream.ReadNumber(color.G); 
    argStream.ReadNumber(color.B); 
    color.A = 0xFF;
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleHeadLightColor( pVehicle, color) )
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

int CLuaFunctionDefinitions::SetVehicleTurretPosition ( lua_State* luaVM )
{
    CVehicle* pVehicle;
    float fHorizontal; 
    float fVertical; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(fHorizontal); 
    argStream.ReadNumber(fVertical); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleTurretPosition( pVehicle, fHorizontal, fVertical) )
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

int CLuaFunctionDefinitions::SetVehicleDoorOpenRatio ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucDoor; 
    float fRatio; 
    unsigned long ulTime; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucDoor); 
    argStream.ReadNumber(fRatio); 
    argStream.ReadNumber(ulTime, 0); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetVehicleDoorOpenRatio( pElement, ucDoor, fRatio, ulTime) )
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


int CLuaFunctionDefinitions::GetVehicleDoorOpenRatio ( lua_State* luaVM )
{
    CVehicle* pVehicle; 
    unsigned char ucDoor; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pVehicle); 
    argStream.ReadNumber(ucDoor); 
    
    if ( !argStream.HasErrors ( ) )
    {
        float fRatio;

        if ( CStaticFunctionDefinitions::GetVehicleDoorOpenRatio ( pVehicle, ucDoor, fRatio ) )
        {
            lua_pushnumber ( luaVM, fRatio );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehiclePlateText ( lua_State* luaVM )
{
//  bool setVehiclePlateText ( vehicle theVehicle, string plateText )
    CElement* pElement; SString strText;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strText );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::SetVehiclePlateText ( pElement, strText ) )
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

int CLuaFunctionDefinitions::CreateMarker ( lua_State* luaVM )
{
    CVector vecPosition; 
    float fSize; 
    SColorRGBA color ( 0, 0, 255, 255 );
    SString strType; 
    CElement* pVisibleTo; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(vecPosition.fZ); 
    argStream.ReadString(strType, "default"); 
    argStream.ReadNumber(fSize, 4.0f); 
    argStream.ReadNumber(color.R, color.R); 
    argStream.ReadNumber(color.G, color.G); 
    argStream.ReadNumber(color.B, color.B); 
    argStream.ReadNumber(color.A, color.A); 

    if ( argStream.NextIsBool() || argStream.NextIsNil () )
    {
        pVisibleTo = NULL;
    }
    else
        argStream.ReadUserData(pVisibleTo, m_pRootElement); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create it
                CMarker* pMarker = CStaticFunctionDefinitions::CreateMarker ( pResource, vecPosition, strType, fSize, color, pVisibleTo );
                if ( pMarker )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pMarker );
                    }
                    lua_pushelement ( luaVM, pMarker );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetMarkerCount ( lua_State* luaVM )
{
    unsigned int uiCount = 0;
    if ( CStaticFunctionDefinitions::GetMarkerCount ( uiCount ) )
    {
        lua_pushnumber ( luaVM, static_cast < lua_Number > ( uiCount ) );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetMarkerType ( lua_State* luaVM )
{
    CMarker* pMarker; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pMarker); 
    
    if ( !argStream.HasErrors ( ) )
    {
        char szMarkerType [64];
        if ( CStaticFunctionDefinitions::GetMarkerType ( pMarker, szMarkerType ) )
        {
            lua_pushstring ( luaVM, szMarkerType );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetMarkerSize ( lua_State* luaVM )
{
    CMarker* pMarker; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pMarker); 
    
    if ( !argStream.HasErrors ( ) )
    {
        float fSize;
        if ( CStaticFunctionDefinitions::GetMarkerSize ( pMarker, fSize ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( fSize ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetMarkerColor ( lua_State* luaVM )
{
    CMarker* pMarker; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pMarker); 
    
    if ( !argStream.HasErrors ( ) )
    {
        SColor color;
        if ( CStaticFunctionDefinitions::GetMarkerColor ( pMarker, color ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.R ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.G ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.B ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.A ) );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetMarkerTarget ( lua_State* luaVM )
{
    CMarker* pMarker; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pMarker); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CVector vecTarget;
        if ( CStaticFunctionDefinitions::GetMarkerTarget ( pMarker, vecTarget ) )
        {
            lua_pushnumber ( luaVM, vecTarget.fX );
            lua_pushnumber ( luaVM, vecTarget.fY );
            lua_pushnumber ( luaVM, vecTarget.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetMarkerIcon ( lua_State* luaVM )
{
    CMarker* pMarker; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pMarker); 
    
    if ( !argStream.HasErrors ( ) )
    {
        char szMarkerIcon [64];
        if ( CStaticFunctionDefinitions::GetMarkerType ( pMarker, szMarkerIcon ) )
        {
            lua_pushstring ( luaVM, szMarkerIcon );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetMarkerType ( lua_State* luaVM )
{
    CElement* pElement;
    SString strType; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadString(strType); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetMarkerType( pElement, strType) )
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


int CLuaFunctionDefinitions::SetMarkerSize ( lua_State* luaVM )
{
    CElement* pElement;
    float fSize; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(fSize); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetMarkerSize( pElement, fSize) )
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

int CLuaFunctionDefinitions::SetMarkerColor ( lua_State* luaVM )
{
    CElement* pElement;
    SColor color;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(color.R); 
    argStream.ReadNumber(color.G); 
    argStream.ReadNumber(color.B); 
    argStream.ReadNumber(color.A); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetMarkerColor( pElement, color) )
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

int CLuaFunctionDefinitions::SetMarkerTarget ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecTarget;     
    CVector *pvecTarget = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);

    if ( argStream.NextIsNumber(0) &&
         argStream.NextIsNumber(1) &&
         argStream.NextIsNumber(2) )
    {
        argStream.ReadNumber(vecTarget.fX); 
        argStream.ReadNumber(vecTarget.fY); 
        argStream.ReadNumber(vecTarget.fZ); 
        pvecTarget = &vecTarget;
    }
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetMarkerTarget( pElement, pvecTarget) )
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

int CLuaFunctionDefinitions::SetMarkerIcon ( lua_State* luaVM )
{
    CElement* pElement;
    SString strIcon; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadString(strIcon); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetMarkerIcon( pElement, strIcon) )
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


int CLuaFunctionDefinitions::CreateBlip ( lua_State* luaVM )
{
    CVector vecPosition; 
    unsigned char ucIcon, ucSize;
    SColorRGBA color ( 255, 0, 0, 255 );
    short sOrdering;
    unsigned short usVisibleDistance;
    CElement* pVisibleTo; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(vecPosition.fZ); 
    argStream.ReadNumber(ucIcon, 0); 
    argStream.ReadNumber(ucSize, 2); 
    argStream.ReadNumber(color.R, color.R); 
    argStream.ReadNumber(color.G, color.G); 
    argStream.ReadNumber(color.B, color.B); 
    argStream.ReadNumber(color.A, color.A); 
    argStream.ReadNumber(sOrdering, 0);
    argStream.ReadNumber(usVisibleDistance, 16383);
    if ( argStream.NextIsBool() || argStream.NextIsNil())
        pVisibleTo = NULL;
    else
        argStream.ReadUserData(pVisibleTo, m_pRootElement); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create the blip
                CBlip* pBlip = CStaticFunctionDefinitions::CreateBlip ( pResource, vecPosition, ucIcon, ucSize, color, sOrdering, usVisibleDistance, pVisibleTo );
                if ( pBlip )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pBlip );
                    }
                    lua_pushelement ( luaVM, pBlip );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateBlipAttachedTo ( lua_State* luaVM )
{
    CElement* pElement; 
    unsigned char ucIcon, ucSize;
    SColorRGBA color ( 255, 0, 0, 255 );
    short sOrdering;
    unsigned short usVisibleDistance;
    CElement* pVisibleTo; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement); 
    argStream.ReadNumber(ucIcon, 0); 
    argStream.ReadNumber(ucSize, 2); 
    argStream.ReadNumber(color.R, color.R); 
    argStream.ReadNumber(color.G, color.G); 
    argStream.ReadNumber(color.B, color.B); 
    argStream.ReadNumber(color.A, color.A); 
    argStream.ReadNumber(sOrdering, 0);
    argStream.ReadNumber(usVisibleDistance, 16383);
    if ( argStream.NextIsBool() || argStream.NextIsNil())
        pVisibleTo = NULL;
    else
        argStream.ReadUserData(pVisibleTo, m_pRootElement); 
    
    CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        CResource * resource = pLuaMain->GetResource();
        if ( resource )
        {
            // Create the blip
            CBlip* pBlip = CStaticFunctionDefinitions::CreateBlipAttachedTo ( resource, pElement, ucIcon, ucSize, color, sOrdering, usVisibleDistance, pVisibleTo );
            if ( pBlip )
            {
                pBlip->SetParentObject ( resource->GetDynamicElementRoot() );
                CElementGroup * group = resource->GetElementGroup();
                if ( group )
                {
                    group->Add ( pBlip );
                }
                lua_pushelement ( luaVM, pBlip );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBlipIcon ( lua_State* luaVM )
{
    CBlip* pBlip; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pBlip); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucIcon;
        if ( CStaticFunctionDefinitions::GetBlipIcon ( pBlip, ucIcon ) )
        {
            lua_pushnumber ( luaVM, ucIcon );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetBlipSize ( lua_State* luaVM )
{
    CBlip* pBlip; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pBlip); 
    
    if ( !argStream.HasErrors ( ) )
    { 
        unsigned char ucSize;
        if ( CStaticFunctionDefinitions::GetBlipSize ( pBlip, ucSize ) )
        {
            lua_pushnumber ( luaVM, ucSize );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetBlipColor ( lua_State* luaVM )
{
    CBlip* pBlip; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pBlip); 
    
    if ( !argStream.HasErrors ( ) )
    {
        SColor color;
        if ( CStaticFunctionDefinitions::GetBlipColor ( pBlip, color ) )
        {
            lua_pushnumber ( luaVM, color.R );
            lua_pushnumber ( luaVM, color.G );
            lua_pushnumber ( luaVM, color.B );
            lua_pushnumber ( luaVM, color.A );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBlipOrdering ( lua_State* luaVM )
{
    CBlip* pBlip; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pBlip); 
    
    if ( !argStream.HasErrors ( ) )
    {
        short sOrdering;
        if ( CStaticFunctionDefinitions::GetBlipOrdering ( pBlip, sOrdering ) )
        {
            lua_pushnumber ( luaVM, sOrdering );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBlipVisibleDistance ( lua_State* luaVM )
{
    CBlip* pBlip; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pBlip); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned short usVisibleDistance;
        if ( CStaticFunctionDefinitions::GetBlipVisibleDistance ( pBlip, usVisibleDistance ) )
        {
            lua_pushnumber ( luaVM, usVisibleDistance );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetBlipIcon ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucIcon; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucIcon); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetBlipIcon( pElement, ucIcon) )
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

int CLuaFunctionDefinitions::SetBlipSize ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucSize; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucSize); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetBlipSize( pElement, ucSize) )
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

int CLuaFunctionDefinitions::SetBlipColor ( lua_State* luaVM )
{
    CElement* pElement;
    SColor color; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(color.R); 
    argStream.ReadNumber(color.G); 
    argStream.ReadNumber(color.B); 
    argStream.ReadNumber(color.A); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetBlipColor( pElement, color) )
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

int CLuaFunctionDefinitions::SetBlipOrdering ( lua_State* luaVM )
{
    CElement* pElement;
    short sOrdering; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(sOrdering); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetBlipOrdering( pElement, sOrdering) )
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

int CLuaFunctionDefinitions::SetBlipVisibleDistance ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned short usVisibleDistance; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(usVisibleDistance); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetBlipVisibleDistance( pElement, usVisibleDistance) )
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


int CLuaFunctionDefinitions::CreateObject ( lua_State* luaVM )
{
//  object createObject ( int modelid, float x, float y, float z, [float rx, float ry, float rz, bool lowLOD] )
    ushort usModelID; CVector vecPosition; CVector vecRotation; bool bIsLowLod;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( usModelID );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( vecRotation.fX, 0 );
    argStream.ReadNumber ( vecRotation.fY, 0 );
    argStream.ReadNumber ( vecRotation.fZ, 0 );
    argStream.ReadBool ( bIsLowLod, false );

    if ( !argStream.HasErrors () )
    {
        if ( CObjectManager::IsValidModel  ( usModelID ) )
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CResource* pResource = pLuaMain->GetResource ();
                if ( pResource )
                {
                    CObject* pObject = CStaticFunctionDefinitions::CreateObject ( pResource, usModelID, vecPosition, vecRotation, bIsLowLod );
                    if ( pObject )
                    {
                        CElementGroup * pGroup = pResource->GetElementGroup();
                        if ( pGroup )
                        {
                            pGroup->Add ( pObject );
                        }

                        lua_pushelement ( luaVM, pObject );
                        return 1;
                    }
                }
            }
        }
        else
            argStream.SetCustomError( "Invalid model id" );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetObjectRotation ( lua_State* luaVM )
{
    CObject* pObject; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pObject); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CVector vecRotation;
        if ( CStaticFunctionDefinitions::GetObjectRotation ( pObject, vecRotation ) )
        {
            lua_pushnumber ( luaVM, vecRotation.fX );
            lua_pushnumber ( luaVM, vecRotation.fY );
            lua_pushnumber ( luaVM, vecRotation.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetObjectScale ( lua_State* luaVM )
{
    CObject* pObject; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pObject); 
    
    if ( !argStream.HasErrors ( ) )
    {
        const CVector& vecScale = pObject->GetScale ();
        lua_pushnumber ( luaVM, vecScale.fX );
        lua_pushnumber ( luaVM, vecScale.fY );
        lua_pushnumber ( luaVM, vecScale.fZ );
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetObjectRotation ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecRotation; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(vecRotation.fX); 
    argStream.ReadNumber(vecRotation.fY); 
    argStream.ReadNumber(vecRotation.fZ); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetObjectRotation( pElement, vecRotation) )
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

int CLuaFunctionDefinitions::SetObjectScale ( lua_State* luaVM )
{
    CObject* pObject; CVector vecScale;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pObject );
    argStream.ReadNumber ( vecScale.fX );
    argStream.ReadNumber ( vecScale.fY, vecScale.fX );
    argStream.ReadNumber ( vecScale.fZ, vecScale.fX );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetObjectScale ( pObject, vecScale ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }

    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::MoveObject ( lua_State* luaVM )
{
//  bool moveObject ( object theObject, int time,
//      float targetx, float targety, float targetz, 
//      [ float moverx, float movery, float moverz,
//      string strEasingType, float fEasingPeriod, float fEasingAmplitude, float fEasingOvershoot ] )
    CElement* pElement; int iTime; CVector vecTargetPosition; CVector vecTargetRotation;
    CEasingCurve::eType easingType; float fEasingPeriod; float fEasingAmplitude; float fEasingOvershoot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( iTime );
    argStream.ReadNumber ( vecTargetPosition.fX );
    argStream.ReadNumber ( vecTargetPosition.fY );
    argStream.ReadNumber ( vecTargetPosition.fZ );
    argStream.ReadNumber ( vecTargetRotation.fX, 0 );
    argStream.ReadNumber ( vecTargetRotation.fY, 0 );
    argStream.ReadNumber ( vecTargetRotation.fZ, 0 );
    argStream.ReadEnumString ( easingType, CEasingCurve::Linear );
    argStream.ReadNumber ( fEasingPeriod, 0.3f );
    argStream.ReadNumber ( fEasingAmplitude, 1.0f );
    argStream.ReadNumber ( fEasingOvershoot, 1.70158f );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource * pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                if ( CStaticFunctionDefinitions::MoveObject ( pResource, pElement, iTime, vecTargetPosition, vecTargetRotation, easingType, fEasingPeriod, fEasingAmplitude, fEasingOvershoot ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::StopObject ( lua_State* luaVM )
{
    CElement* pElement;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::StopObject( pElement) )
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


int CLuaFunctionDefinitions::CreateRadarArea ( lua_State* luaVM )
{
//  radararea createRadarArea ( float leftX, float bottomY, float sizeX, float sizeY, [ int r = 255, int g = 0, int b = 0, int a = 255, element visibleTo = getRootElement() ] )
    CVector2D vecPosition; CVector2D vecSize; float dRed; float dGreen; float dBlue; float dAlpha; CElement* pVisibleTo;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecSize.fX );
    argStream.ReadNumber ( vecSize.fY );
    argStream.ReadNumber ( dRed, 255 );
    argStream.ReadNumber ( dGreen, 0 );
    argStream.ReadNumber ( dBlue, 0 );
    argStream.ReadNumber ( dAlpha, 255 );
    argStream.ReadUserData ( pVisibleTo, m_pRootElement );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        CResource* pResource = pLuaMain ? pLuaMain->GetResource() : NULL;
        if ( pResource )
        {
            SColorRGBA color ( dRed, dGreen, dBlue, dAlpha );

            // Create it
            CRadarArea* pRadarArea = CStaticFunctionDefinitions::CreateRadarArea ( pResource, vecPosition, vecSize, color, pVisibleTo );
            if ( pRadarArea )
            {
                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add ( pRadarArea );
                }
                lua_pushelement ( luaVM, pRadarArea );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetRadarAreaSize ( lua_State* luaVM )
{
//  float, float getRadarAreaSize ( radararea theRadararea )
    CRadarArea* pRadarArea;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRadarArea );

    if ( !argStream.HasErrors () )
    {
        CVector2D vecSize;
        if ( CStaticFunctionDefinitions::GetRadarAreaSize ( pRadarArea, vecSize ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( vecSize.fX ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( vecSize.fY ) );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetRadarAreaColor ( lua_State* luaVM )
{
//  int, int, int, int getRadarAreaColor ( radararea theRadararea )
    CRadarArea* pRadarArea;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRadarArea );

    if ( !argStream.HasErrors () )
    {
        SColor color;
        if ( CStaticFunctionDefinitions::GetRadarAreaColor ( pRadarArea, color ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.R ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.G ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.B ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.A ) );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsRadarAreaFlashing ( lua_State* luaVM )
{
//  bool isRadarAreaFlashing ( radararea theRadararea )
    CRadarArea* pRadarArea;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRadarArea );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, CStaticFunctionDefinitions::IsRadarAreaFlashing ( pRadarArea ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsInsideRadarArea ( lua_State* luaVM )
{
//  bool isInsideRadarArea ( radararea theArea, float posX, float posY )
    CRadarArea* pRadarArea; CVector2D vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRadarArea );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );

    if ( !argStream.HasErrors () )
    {
        bool bInside = false;
        if ( CStaticFunctionDefinitions::IsInsideRadarArea ( pRadarArea, vecPosition, bInside ) )
        {
            lua_pushboolean ( luaVM, bInside );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetRadarAreaSize ( lua_State* luaVM )
{
//  bool setRadarAreaSize ( radararea theRadararea, float x, float y )
    CElement* pElement; CVector2D vecSize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( vecSize.fX );
    argStream.ReadNumber ( vecSize.fY );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetRadarAreaSize ( pElement, vecSize ) )
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


int CLuaFunctionDefinitions::SetRadarAreaColor ( lua_State* luaVM )
{
//  bool setRadarAreaColor ( radararea theRadarArea, int r, int g, int b, int a )
    CElement* pElement; float dRed; float dGreen; float dBlue; float dAlpha;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( dRed );
    argStream.ReadNumber ( dGreen );
    argStream.ReadNumber ( dBlue );
    argStream.ReadNumber ( dAlpha, 255 );

    if ( !argStream.HasErrors () )
    {
        SColorRGBA color ( dRed, dGreen, dBlue, dAlpha );
        if ( CStaticFunctionDefinitions::SetRadarAreaColor ( pElement, color ) )
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


int CLuaFunctionDefinitions::SetRadarAreaFlashing ( lua_State* luaVM )
{
//  bool setRadarAreaFlashing ( radararea theRadarArea, bool flash )
    CElement* pElement; bool bFlash;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadBool ( bFlash );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetRadarAreaFlashing ( pElement, lua_toboolean ( luaVM, 2 ) ? true:false ) )
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

int CLuaFunctionDefinitions::CreateExplosion ( lua_State* luaVM )
{
    CVector vecPosition; 
    unsigned char ucType; 
    CElement* pCreator; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(vecPosition.fZ); 
    argStream.ReadNumber(ucType); 
    argStream.ReadUserData(pCreator, NULL); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::CreateExplosion ( vecPosition, ucType, pCreator ) )
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


int CLuaFunctionDefinitions::CreateFire ( lua_State* luaVM )
{
    CVector vecPosition; 
    float fSize; 
    CElement* pCreator; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(vecPosition.fZ); 
    argStream.ReadNumber(fSize, 1.8f); 
    argStream.ReadUserData(pCreator, NULL); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::CreateFire ( vecPosition, fSize, pCreator ) )
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


int CLuaFunctionDefinitions::PlayMissionAudio ( lua_State* luaVM )
{
    CElement* pElement; 
    CVector vecPosition; 
    unsigned short usSlot; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(usSlot); 

    if ( argStream.NextIsNumber() )
    {
        argStream.ReadNumber(vecPosition.fX); 
        argStream.ReadNumber(vecPosition.fY); 
        argStream.ReadNumber(vecPosition.fZ); 
    
        if ( !argStream.HasErrors ( ) )
        {
            if ( CStaticFunctionDefinitions::PlayMissionAudio ( pElement, &vecPosition, usSlot ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }
    else if ( !argStream.HasErrors ( ) ) 
    {
        if ( CStaticFunctionDefinitions::PlayMissionAudio ( pElement, NULL, usSlot ) )
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

int CLuaFunctionDefinitions::PreloadMissionAudio ( lua_State* luaVM )
{
    CElement* pElement; 
    unsigned short usSound; 
    unsigned short usSlot; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement); 
    argStream.ReadNumber(usSound); 
    argStream.ReadNumber(usSlot); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::PreloadMissionAudio ( pElement, usSound, usSlot ) )
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

int CLuaFunctionDefinitions::PlaySoundFrontEnd ( lua_State* luaVM )
{
    CElement* pElement; 
    unsigned char ucSound; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement); 
    argStream.ReadNumber(ucSound); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( ucSound <= 101 )
        {
            if ( CStaticFunctionDefinitions::PlaySoundFrontEnd ( pElement, ucSound ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "Invalid sound ID specified. Valid sound IDs are 0 - 101." );
	
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::BindKey ( lua_State* luaVM )
{
    CPlayer* pPlayer; 
    SString strKey; 
    SString strHitState; 
    SString strCommand; 
    SString strArguments; 

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( !pLuaMain )
    {
        lua_pushboolean ( luaVM, false );
        return 1;
    }
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer); 
    argStream.ReadString(strKey); 
    argStream.ReadString(strHitState); 

    if ( argStream.NextIsString ( ) )
    {
        // bindKey ( player thePlayer, string key, string keyState, string commandName, [ string arguments ] )
        argStream.ReadString(strCommand); 
        argStream.ReadString(strArguments, ""); 	
        if ( !argStream.HasErrors ( ) )
        {
            if ( CStaticFunctionDefinitions::BindKey ( pPlayer, strKey, strHitState, strCommand, strArguments, pLuaMain->GetResource()->GetName().c_str() ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else if ( !argStream.HasErrors ( ) )
    {
        // bindKey ( player thePlayer, string key, string keyState, function handlerFunction,  [ var arguments, ... ] )
        CLuaFunctionRef iLuaFunction;
        CLuaArguments Arguments;
        argStream.ReadFunction ( iLuaFunction );
        argStream.ReadLuaArguments ( Arguments );
        argStream.ReadFunctionComplete ();
        if ( !argStream.HasErrors ( ) )
        {
            if ( CStaticFunctionDefinitions::BindKey ( pPlayer, strKey, strHitState, pLuaMain, iLuaFunction, Arguments ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    if ( argStream.HasErrors ( ) )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::UnbindKey ( lua_State* luaVM )
{
    CPlayer* pPlayer; 
    SString strKey; 
    SString strHitState; 
    SString strCommand; 

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( !pLuaMain )
    {
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer); 
    argStream.ReadString(strKey); 

    if ( argStream.NextIsString ( 1 ) ) // Check if has command
    {
        // bool unbindKey ( player thePlayer, string key, string keyState, string command )
        argStream.ReadString(strHitState); 
        argStream.ReadString(strCommand); 
        if ( !argStream.HasErrors ( ) )
        {
            if ( CStaticFunctionDefinitions::UnbindKey ( pPlayer, strKey, strHitState, strCommand, pLuaMain->GetResource()->GetName().c_str() ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else if ( !argStream.HasErrors ( ) )
    {
        // bool unbindKey ( player thePlayer, string key, [ string keyState, function handler  ] )
        CLuaFunctionRef iLuaFunction;
        argStream.ReadString ( strHitState, "" );
        argStream.ReadFunction ( iLuaFunction, LUA_REFNIL );
        argStream.ReadFunctionComplete ();
        if ( !argStream.HasErrors ( ) )
        {
            if ( CStaticFunctionDefinitions::UnbindKey ( pPlayer, strKey, pLuaMain, strHitState, iLuaFunction ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    if ( argStream.HasErrors ( ) )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::IsKeyBound ( lua_State* luaVM )
{
    CPlayer* pPlayer; 
    SString strKey; 
    SString strHitState; 
    SString strCommand; 
    SString strArguments; 
    CLuaFunctionRef iLuaFunction;

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( !pLuaMain )
    {
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer); 
    argStream.ReadString(strKey); 
    argStream.ReadString(strHitState, ""); 

    if ( lua_type ( luaVM, 4 ) )
        iLuaFunction = luaM_toref ( luaVM, 4 );

    if ( !argStream.HasErrors ( ) )
    {
        bool bBound;
        if ( CStaticFunctionDefinitions::IsKeyBound ( pPlayer, strKey, pLuaMain, strHitState.empty() ? NULL : strHitState.c_str() , iLuaFunction, bBound ) )
        {
            lua_pushboolean ( luaVM, bBound );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetFunctionsBoundToKey ( lua_State* luaVM )
{
    CPlayer* pPlayer; 
    SString strKey; 
    SString strHitState; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer); 
    argStream.ReadString(strKey); 
    argStream.ReadString(strHitState, ""); 

    if ( !argStream.HasErrors ( ) )
    {
        bool bCheckHitState = false, bHitState = false;
        if ( strHitState == "down" )
        {
            bCheckHitState = true;
            bHitState = true;
        }
        else if ( strHitState == "up" )
        {
            bCheckHitState = true;
            bHitState = false;
        } 
        
        // Create a new table
        lua_newtable ( luaVM );

        // Add all the bound functions to it
        unsigned int uiIndex = 0;
        list < CKeyBind* > ::iterator iter = pPlayer->GetKeyBinds ()->IterBegin ();
        for ( ; iter != pPlayer->GetKeyBinds ()->IterEnd (); ++iter )
        {
            CKeyBind* pKeyBind = *iter;
            if ( !pKeyBind->IsBeingDeleted () )
            {
                switch ( pKeyBind->GetType () )
                {
                case KEY_BIND_FUNCTION:
                    {
                        CKeyFunctionBind* pBind = static_cast < CKeyFunctionBind* > ( pKeyBind );
                        if ( !bCheckHitState || pBind->bHitState == bHitState )
                        {
                            if ( strKey == pBind->boundKey->szKey )
                            {
                                lua_pushnumber ( luaVM, ++uiIndex );
                                lua_rawgeti ( luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction.ToInt () );
                                lua_settable ( luaVM, -3 );
                            }
                        }
                        break;
                    }
                case KEY_BIND_CONTROL_FUNCTION:
                    {
                        CControlFunctionBind* pBind = static_cast < CControlFunctionBind* > ( pKeyBind );
                        if ( !bCheckHitState || pBind->bHitState == bHitState )
                        {
                            if ( strKey == pBind->boundControl->szControl )
                            {
                                lua_pushnumber ( luaVM, ++uiIndex );
                                lua_rawgeti ( luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction.ToInt () );
                                lua_settable ( luaVM, -3 );
                            }
                        }
                        break;
                    }
                default:
                    break;
                }
            }
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
        
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetKeyBoundToFunction ( lua_State* luaVM )
{   
    CPlayer* pPlayer; 
    SString strKey; 
    CLuaFunctionRef iLuaFunction;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer); 
    argStream.ReadString(strKey); 

    if (!argStream.HasErrors ( ) )
    {
        if (lua_type ( luaVM, 2 ) != LUA_TFUNCTION )
        {
            m_pScriptDebugging->LogBadType ( luaVM );
            lua_pushboolean( luaVM, false );
            return 1;
        }

        iLuaFunction = luaM_toref ( luaVM, 2 );

        list < CKeyBind* > ::iterator iter = pPlayer->GetKeyBinds ()->IterBegin ();
        for ( ; iter != pPlayer->GetKeyBinds ()->IterEnd (); ++iter )
        {
            CKeyBind* pKeyBind = *iter;
            if ( !pKeyBind->IsBeingDeleted () )
            {
                switch ( pKeyBind->GetType () )
                {
                case KEY_BIND_FUNCTION:
                    {
                        CKeyFunctionBind* pBind = static_cast < CKeyFunctionBind* > ( pKeyBind );
                        if ( iLuaFunction == pBind->m_iLuaFunction )
                        {
                            lua_pushstring ( luaVM, pBind->boundKey->szKey );
                            return 1;
                        }
                        break;
                    }
                case KEY_BIND_CONTROL_FUNCTION:
                    {
                        CControlFunctionBind* pBind = static_cast < CControlFunctionBind* > ( pKeyBind );
                        if ( iLuaFunction == pBind->m_iLuaFunction )
                        {
                            lua_pushstring ( luaVM, pBind->boundKey->szKey );
                            return 1;
                        }
                        break;
                    }
                default:
                    break;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );


    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetControlState ( lua_State* luaVM )
{
    CPlayer* pPlayer; 
    SString strControl; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer); 
    argStream.ReadString(strControl); 
    
    if ( !argStream.HasErrors ( ) )
    {
        bool bState;
        if ( CStaticFunctionDefinitions::GetControlState ( pPlayer, strControl, bState ) )
        {
            lua_pushboolean ( luaVM, bState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsControlEnabled ( lua_State* luaVM )
{
    CPlayer* pPlayer; 
    SString strControl; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer); 
    argStream.ReadString(strControl); 
    
    if ( !argStream.HasErrors ( ) )
    {
        bool bState;
        if ( CStaticFunctionDefinitions::IsControlEnabled ( pPlayer, strControl, bState ) )
        {
            lua_pushboolean ( luaVM, bState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetControlState ( lua_State* luaVM )
{
    CPlayer* pPlayer; 
    SString strControl; 
    bool bState;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer); 
    argStream.ReadString(strControl); 
    argStream.ReadBool(bState);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetControlState ( pPlayer, strControl, bState ) )
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

int CLuaFunctionDefinitions::ToggleControl ( lua_State* luaVM )
{
    CPlayer* pPlayer; 
    SString strControl; 
    bool bState;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer); 
    argStream.ReadString(strControl); 
    argStream.ReadBool(bState);
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::ToggleControl ( pPlayer, strControl, bState ) )
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


int CLuaFunctionDefinitions::ToggleAllControls ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    bool bEnabled; 
    bool bGTAControls; 
    bool bMTAControls; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);
    argStream.ReadBool(bEnabled); 
    argStream.ReadBool(bGTAControls, true); 
    argStream.ReadBool(bMTAControls, true); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::ToggleAllControls( pPlayer, bGTAControls, bMTAControls, bEnabled ) )
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

int CLuaFunctionDefinitions::CreateTeam ( lua_State* luaVM )
{
    SString strName; 
    unsigned char ucRed; 
    unsigned char ucGreen; 
    unsigned char ucBlue; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strName); 
    argStream.ReadNumber(ucRed, 235); 
    argStream.ReadNumber(ucGreen, 221); 
    argStream.ReadNumber(ucBlue, 178); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                CTeam* pTeam = CStaticFunctionDefinitions::CreateTeam ( pResource, strName, ucRed, ucGreen, ucBlue );
                if ( pTeam )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pTeam );
                    }
                    lua_pushelement ( luaVM, pTeam );
                    return 1;
                }
            }
        }
	
	
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTeamFromName ( lua_State* luaVM )
{
    SString strName; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strName); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CTeam* pTeam = CStaticFunctionDefinitions::GetTeamFromName ( strName );
        if ( pTeam )
        {
            lua_pushelement ( luaVM, pTeam );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetTeamName ( lua_State* luaVM )
{
    CTeam *pTeam; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pTeam); 
    
    if ( !argStream.HasErrors ( ) )
    {
        SString strTeamName;
        if ( CStaticFunctionDefinitions::GetTeamName ( pTeam, strTeamName ) )
        {
            lua_pushstring ( luaVM, strTeamName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetTeamColor ( lua_State* luaVM )
{
    CTeam *pTeam; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pTeam); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucRed, ucGreen, ucBlue;
        if ( CStaticFunctionDefinitions::GetTeamColor ( pTeam, ucRed, ucGreen, ucBlue ) )
        {
            lua_pushnumber ( luaVM, ucRed );
            lua_pushnumber ( luaVM, ucGreen );
            lua_pushnumber ( luaVM, ucBlue );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTeamFriendlyFire ( lua_State* luaVM )
{
    CTeam *pTeam; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pTeam); 
    
    if ( !argStream.HasErrors ( ) )
    {
        bool bFriendlyFire;
        if ( CStaticFunctionDefinitions::GetTeamFriendlyFire ( pTeam, bFriendlyFire ) )
        {
            lua_pushboolean ( luaVM, bFriendlyFire );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetPlayersInTeam ( lua_State* luaVM )
{
    CTeam *pTeam; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pTeam); 
    
    if ( !argStream.HasErrors ( ) )
    {
        lua_newtable ( luaVM );

        pTeam->GetPlayers ( luaVM );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::CountPlayersInTeam ( lua_State* luaVM )
{
    CTeam *pTeam; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pTeam); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned int uiCount;
        if ( CStaticFunctionDefinitions::CountPlayersInTeam ( pTeam, uiCount ) )
        {
            lua_pushnumber ( luaVM, uiCount );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerTeam ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    CTeam* pTeam; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pPlayer);
    argStream.ReadUserData(pTeam, NULL); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetPlayerTeam( pPlayer, pTeam) )
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


int CLuaFunctionDefinitions::SetTeamName ( lua_State* luaVM )
{
    CTeam* pElement;
    SString strName; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadString(strName); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetTeamName( pElement, strName) )
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


int CLuaFunctionDefinitions::SetTeamColor ( lua_State* luaVM )
{
    CTeam* pElement;
    unsigned char ucRed; 
    unsigned char ucGreen; 
    unsigned char ucBlue; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucRed); 
    argStream.ReadNumber(ucGreen); 
    argStream.ReadNumber(ucBlue); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetTeamColor( pElement, ucRed, ucGreen, ucBlue) )
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


int CLuaFunctionDefinitions::SetTeamFriendlyFire ( lua_State* luaVM )
{
    CTeam* pElement;
    bool bFriendlyFire; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bFriendlyFire); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetTeamFriendlyFire( pElement, bFriendlyFire) )
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


int CLuaFunctionDefinitions::CreateWater ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( !pLuaMain )
    {
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CVector v1, v2, v3, v4;
    CVector* pv4 = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(v1.fX); 
    argStream.ReadNumber(v1.fY); 
    argStream.ReadNumber(v1.fZ); 
    argStream.ReadNumber(v2.fX); 
    argStream.ReadNumber(v2.fY); 
    argStream.ReadNumber(v2.fZ); 
    argStream.ReadNumber(v3.fX); 
    argStream.ReadNumber(v3.fY); 
    argStream.ReadNumber(v3.fZ); 

    if ( argStream.NextIsNumber () )
    {
        argStream.ReadNumber(v4.fX); 
        argStream.ReadNumber(v4.fY); 
        argStream.ReadNumber(v4.fZ); 
        pv4 = &v4;
    }
    
    if ( !argStream.HasErrors ( ) )
    {
        CWater* pWater = CStaticFunctionDefinitions::CreateWater ( pLuaMain->GetResource(), &v1, &v2, &v3, pv4);
        if ( pWater )
        {
            CElementGroup * pGroup = pLuaMain->GetResource()->GetElementGroup();
            if ( pGroup )
            {
                pGroup->Add ( pWater );
            }
            lua_pushelement ( luaVM, pWater );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetWaterLevel ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    CResource* pResource = pLuaMain ? pLuaMain->GetResource () : NULL;
    if ( !pResource )
    {
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserData () )
    {
        // Call type 1
        //  bool setWaterLevel ( [water theWater,] float level )
        CWater* pWater; float fLevel;

        argStream.ReadUserData ( pWater );
        argStream.ReadNumber ( fLevel );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetElementWaterLevel ( pWater, fLevel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
    {
        // Call type 2
        //  bool setWaterLevel ( float level, bool bIncludeWorldNonSeaLevel, bool bIncludeAllWaterElements )
        float fLevel; bool bIncludeWorldNonSeaLevel; bool bIncludeAllWaterElements;

        argStream.ReadNumber ( fLevel );
        argStream.ReadBool ( bIncludeWorldNonSeaLevel, true );
        argStream.ReadBool ( bIncludeAllWaterElements, true );

        if ( !argStream.HasErrors () )
        {
            if ( bIncludeAllWaterElements )
                CStaticFunctionDefinitions::SetAllElementWaterLevel ( fLevel );
            if ( CStaticFunctionDefinitions::SetWorldWaterLevel ( fLevel, bIncludeWorldNonSeaLevel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ResetWaterLevel ( lua_State* luaVM )
{
    CStaticFunctionDefinitions::ResetWorldWaterLevel ();
    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefinitions::GetWaterVertexPosition ( lua_State* luaVM )
{
    CWater *pWater;
    int iVertexIndex;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pWater); 
    argStream.ReadNumber(iVertexIndex);
    
    if ( !argStream.HasErrors ( ) )
    {
        CVector vecPosition;
        if ( CStaticFunctionDefinitions::GetWaterVertexPosition ( pWater, iVertexIndex, vecPosition ) )
        {
            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            lua_pushnumber ( luaVM, vecPosition.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetWaterVertexPosition ( lua_State* luaVM )
{
    CWater* pElement;
    int iVertex; 
    CVector vecPosition; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(iVertex); 
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(vecPosition.fZ); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetWaterVertexPosition( pElement, iVertex, vecPosition) )
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


int CLuaFunctionDefinitions::GetWaterColor ( lua_State* luaVM )
{
    unsigned char ucRed, ucGreen, ucBlue, ucAlpha;

    bool bColorOverride = CStaticFunctionDefinitions::GetWaterColor ( ucRed, ucGreen, ucBlue, ucAlpha );

    if ( bColorOverride )
    {
        lua_pushnumber ( luaVM, ucRed );
        lua_pushnumber ( luaVM, ucGreen );
        lua_pushnumber ( luaVM, ucBlue );
        lua_pushnumber ( luaVM, ucAlpha );

        return 4;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetWaterColor ( lua_State* luaVM )
{
    unsigned char ucR; 
    unsigned char ucG; 
    unsigned char ucB; 
    unsigned char ucA; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(ucR); 
    argStream.ReadNumber(ucG); 
    argStream.ReadNumber(ucB); 
    argStream.ReadNumber(ucA, 200); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetWaterColor( ucR, ucG, ucB, ucA) )
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


int CLuaFunctionDefinitions::ResetWaterColor ( lua_State *luaVM )
{
    bool bSuccess = CStaticFunctionDefinitions::ResetWaterColor ( );

    lua_pushboolean ( luaVM, bSuccess );
    return 1;
}


int CLuaFunctionDefinitions::CreateColCircle ( lua_State* luaVM )
{
    CVector vecPosition; 
    float fRadius; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(fRadius); 
    
    if ( !argStream.HasErrors ( ) )
    {
        vecPosition.fZ = 0.0f;
        if(fRadius < 0.0f) fRadius = 0.1f;
       
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create it and return it
                CColCircle* pShape = CStaticFunctionDefinitions::CreateColCircle ( pResource, vecPosition, fRadius );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;

}


int CLuaFunctionDefinitions::CreateColCuboid ( lua_State* luaVM )
{
    CVector vecPosition; 
    CVector vecSize; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(vecPosition.fZ); 
    argStream.ReadNumber(vecSize.fX); 
    argStream.ReadNumber(vecSize.fY); 
    argStream.ReadNumber(vecSize.fZ); 

    
    if ( !argStream.HasErrors ( ) )
    {
        if ( vecSize.fX < 0.0f ) vecSize.fX = 0.1f;
        if ( vecSize.fY < 0.0f ) vecSize.fY = 0.1f;
        if ( vecSize.fZ < 0.0f ) vecSize.fZ = 0.1f;
       
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create it and return it
                CColCuboid* pShape = CStaticFunctionDefinitions::CreateColCuboid ( pResource, vecPosition, vecSize );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;

}


int CLuaFunctionDefinitions::CreateColSphere ( lua_State* luaVM )
{
    CVector vecPosition; 
    float fRadius; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(vecPosition.fZ); 
    argStream.ReadNumber(fRadius); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if(fRadius < 0.0f) fRadius = 0.1f;
       
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create it and return it
                CColSphere* pShape = CStaticFunctionDefinitions::CreateColSphere ( pResource, vecPosition, fRadius );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;

}


int CLuaFunctionDefinitions::CreateColRectangle ( lua_State* luaVM )
{ 
    CVector vecPosition; 
    CVector2D vecSize; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(vecSize.fX); 
    argStream.ReadNumber(vecSize.fY); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if(vecSize.fX < 0.0f) vecSize.fX = 0.1f;
        if(vecSize.fY < 0.0f) vecSize.fY = 0.1f;
       
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create it and return it
                CColRectangle* pShape = CStaticFunctionDefinitions::CreateColRectangle ( pResource, vecPosition, vecSize );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;

}

int CLuaFunctionDefinitions::CreateColPolygon ( lua_State* luaVM )
{
//  colshape createColPolygon ( float fX, float fY, float fX1, float fY1, float fX2, float fY2, float fX3, float fY3, ... )
    std::vector < CVector2D > vecPointList; 
    
    CScriptArgReader argStream ( luaVM );
    for( uint i = 0 ; i < 4 || argStream.NextCouldBeNumber() ; i++ )
    {
        CVector2D vecPoint;
        argStream.ReadNumber( vecPoint.fX ); 
        argStream.ReadNumber( vecPoint.fY );
        vecPointList.push_back( vecPoint );
    }

    if ( !argStream.HasErrors ( ) )
    {  
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                CColPolygon* pShape = CStaticFunctionDefinitions::CreateColPolygon ( pResource, vecPointList );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;

}

int CLuaFunctionDefinitions::CreateColTube ( lua_State* luaVM )
{
    CVector vecPosition; 
    float fHeight, fRadius;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(vecPosition.fX); 
    argStream.ReadNumber(vecPosition.fY); 
    argStream.ReadNumber(vecPosition.fZ); 
    argStream.ReadNumber(fRadius); 
    argStream.ReadNumber(fHeight); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if(fRadius < 0.0f) fRadius = 0.1f;
        if(fHeight < 0.0f) fHeight = 0.1f;
       
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create it and return it
                CColTube* pShape = CStaticFunctionDefinitions::CreateColTube ( pResource, vecPosition, fRadius, fHeight );
                if ( pShape )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pShape );
                    }
                    lua_pushelement ( luaVM, pShape );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetWeaponNameFromID ( lua_State* luaVM )
{
    unsigned char ucID;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(ucID);
    
    if ( !argStream.HasErrors ( ) )
    {   
        char szBuffer [256];
        if ( CStaticFunctionDefinitions::GetWeaponNameFromID ( ucID, szBuffer ) )
        {
            lua_pushstring ( luaVM, szBuffer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetWeaponIDFromName ( lua_State* luaVM )
{
    SString strName = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucID;

        if ( CStaticFunctionDefinitions::GetWeaponIDFromName ( strName, ucID ) )
        {
            lua_pushnumber ( luaVM, ucID );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateWeapon ( lua_State* luaVM )
{
    CVector vecPos;
    eWeaponType weaponType;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumStringOrNumber ( weaponType );
    argStream.ReadNumber ( vecPos.fX );
    argStream.ReadNumber ( vecPos.fY );
    argStream.ReadNumber ( vecPos.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ( );
            if ( pResource )
            {
                CCustomWeapon * pWeapon = CStaticFunctionDefinitions::CreateWeapon ( pResource, weaponType, vecPos );
                if ( pWeapon )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CElement* ) pWeapon );
                    }

                    lua_pushelement ( luaVM, pWeapon );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::FireWeapon ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::FireWeapon ( pWeapon ) )
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

int CLuaFunctionDefinitions::SetWeaponState ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon;
    eWeaponState weaponState;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadEnumString ( weaponState );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetWeaponState ( pWeapon, weaponState ) )
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

int CLuaFunctionDefinitions::GetWeaponState ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon;
    eWeaponState weaponState;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );

    if ( !argStream.HasErrors () )
    {
        weaponState = pWeapon->GetWeaponState ( );
        SString strValue = EnumToString ( weaponState );
        lua_pushstring ( luaVM, strValue );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetWeaponTarget ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon;
    CElement * pTarget;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    if ( argStream.NextIsUserData() )
    {
        int targetBone;
        argStream.ReadUserData ( pTarget );
        argStream.ReadNumber ( targetBone, 255 );
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWeaponTarget ( pWeapon, pTarget, targetBone ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else if ( argStream.NextIsNumber() )
    {
        CVector vecTarget;
        argStream.ReadNumber( vecTarget.fX );
        argStream.ReadNumber( vecTarget.fY );
        argStream.ReadNumber( vecTarget.fZ );
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWeaponTarget ( pWeapon, vecTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else if ( argStream.NextIsNil() )
    {
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::ClearWeaponTarget ( pWeapon ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        argStream.SetCustomError( "Expected element, number or nil at argument 2" );

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetWeaponTarget ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon;
    CElement * pTarget;
    CVector vecTarget;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    if ( !argStream.HasErrors () )
    {
        switch ( pWeapon->GetTargetType ( ) )
        {
        case TARGET_TYPE_VECTOR:
            vecTarget = pWeapon->GetVectorTarget ( );
            lua_pushnumber ( luaVM, vecTarget.fX );
            lua_pushnumber ( luaVM, vecTarget.fY );
            lua_pushnumber ( luaVM, vecTarget.fZ );
            return 3;
        case TARGET_TYPE_ENTITY:
            pTarget = pWeapon->GetElementTarget ( );
            lua_pushelement ( luaVM, pTarget );
            return 1;
        case TARGET_TYPE_FIXED:
            lua_pushnil ( luaVM );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetWeaponOwner ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    if ( !argStream.HasErrors () )
    {
        CPlayer* pOwner = pWeapon->GetOwner();
        if ( pOwner )
        {
            lua_pushelement ( luaVM, pOwner );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetWeaponOwner ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon;
    CPlayer * pPlayer;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    if ( argStream.NextIsUserData() )
    {
        argStream.ReadUserData ( pPlayer );
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWeaponOwner ( pWeapon, pPlayer ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else if ( argStream.NextIsNil() )
    {
        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWeaponOwner ( pWeapon, NULL ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    if ( argStream.HasErrors() )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFunctionDefinitions::SetWeaponFlags ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon = NULL;
    SLineOfSightFlags flags;
    eWeaponFlags flag;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadEnumString ( flag );
    if ( !argStream.HasErrors() )
    {
        if ( flag != WEAPONFLAGS_FLAGS )
        {
            bool bData;
            argStream.ReadBool ( bData );
            if ( CStaticFunctionDefinitions::SetWeaponFlags( pWeapon, flag, bData ) )
            {
                lua_pushboolean( luaVM, bData );
                return 1;
            }
        }
        else
        {
            argStream.ReadBool ( flags.bCheckBuildings );
            argStream.ReadBool ( flags.bCheckCarTires );
            argStream.ReadBool ( flags.bCheckDummies );
            argStream.ReadBool ( flags.bCheckObjects );
            argStream.ReadBool ( flags.bCheckPeds );
            argStream.ReadBool ( flags.bCheckVehicles );
            argStream.ReadBool ( flags.bSeeThroughStuff );
            argStream.ReadBool ( flags.bShootThroughStuff );
            if ( !argStream.HasErrors() )
            {
                if ( CStaticFunctionDefinitions::SetWeaponFlags( pWeapon, flags ) )
                {
                    lua_pushboolean( luaVM, true );
                }
            }
        }
    }
    if ( argStream.HasErrors() )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetWeaponFlags ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon = NULL;
    SLineOfSightFlags flags;
    eWeaponFlags flag;
    bool bData;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadEnumString ( flag );
    if ( !argStream.HasErrors() )
    {
        if ( flag != WEAPONFLAGS_FLAGS )
        {
            if ( CStaticFunctionDefinitions::GetWeaponFlags( pWeapon, flag, bData ) )
            {
                lua_pushboolean( luaVM, bData );
                return 1;
            }
        }
        else
        {
            if ( CStaticFunctionDefinitions::GetWeaponFlags( pWeapon, flags ) )
            {
                lua_pushboolean( luaVM, flags.bCheckBuildings );
                lua_pushboolean( luaVM, flags.bCheckCarTires );
                lua_pushboolean( luaVM, flags.bCheckDummies );
                lua_pushboolean( luaVM, flags.bCheckObjects );
                lua_pushboolean( luaVM, flags.bCheckPeds );
                lua_pushboolean( luaVM, flags.bCheckVehicles );
                lua_pushboolean( luaVM, flags.bSeeThroughStuff );
                lua_pushboolean( luaVM, flags.bShootThroughStuff );
                return 8;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetWeaponFiringRate ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon = NULL;
    int iFiringRate = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadNumber ( iFiringRate );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::SetWeaponFiringRate( pWeapon, iFiringRate ) )
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

int CLuaFunctionDefinitions::GetWeaponFiringRate ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon = NULL;
    int iFiringRate = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::GetWeaponFiringRate( pWeapon, iFiringRate ) )
        {
            lua_pushnumber ( luaVM, iFiringRate );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::ResetWeaponFiringRate ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::ResetWeaponFiringRate( pWeapon ) )
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

int CLuaFunctionDefinitions::GetWeaponClipAmmo ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon = NULL;
    int iClipAmmo = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::GetWeaponClipAmmo( pWeapon, iClipAmmo ) )
        {
            lua_pushnumber ( luaVM, iClipAmmo );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetWeaponAmmo ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon = NULL;
    int iAmmo = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::GetWeaponAmmo( pWeapon, iAmmo ) )
        {
            lua_pushnumber ( luaVM, iAmmo );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetWeaponClipAmmo ( lua_State* luaVM )
{
    CCustomWeapon * pWeapon = NULL;
    int iAmmo = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pWeapon );
    argStream.ReadNumber ( iAmmo );

    if ( !argStream.HasErrors() )
    {
        if ( CStaticFunctionDefinitions::SetWeaponClipAmmo( pWeapon, iAmmo ) )
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

int CLuaFunctionDefinitions::GetBodyPartName ( lua_State* luaVM )
{
    unsigned char ucID; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(ucID); 
    
    if ( !argStream.HasErrors ( ) )
    {
        char szBuffer [256];
        if ( CStaticFunctionDefinitions::GetBodyPartName ( ucID, szBuffer ) )
        {
            lua_pushstring ( luaVM, szBuffer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetClothesByTypeIndex ( lua_State* luaVM )
{
    unsigned char ucType; 
    unsigned char ucIndex; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(ucType); 
    argStream.ReadNumber(ucIndex); 
    
    if ( !argStream.HasErrors ( ) )
    {
        char szTexture [ 128 ], szModel [ 128 ];
        if ( CStaticFunctionDefinitions::GetClothesByTypeIndex ( ucType, ucIndex, szTexture, szModel ) )
        {
            lua_pushstring ( luaVM, szTexture );
            lua_pushstring ( luaVM, szModel );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetTypeIndexFromClothes ( lua_State* luaVM )
{
    SString strTexture; 
    SString strModel; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strTexture); 
    argStream.ReadString(strModel, ""); 
    
    if ( !argStream.HasErrors ( ) )
    {

        unsigned char ucType, ucIndex;
        if ( CStaticFunctionDefinitions::GetTypeIndexFromClothes ( strTexture, strModel.empty() ? NULL : strModel.c_str(), ucType, ucIndex ) )
        {
            lua_pushnumber ( luaVM, ucType );
            lua_pushnumber ( luaVM, ucIndex );
            return 2;
        }
	
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetClothesTypeName ( lua_State* luaVM )
{
    unsigned char ucType; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(ucType); 
    
    if ( !argStream.HasErrors ( ) )
    {
        char szName [ 40 ];
        if ( CStaticFunctionDefinitions::GetClothesTypeName ( ucType, szName ) )
        {
            lua_pushstring ( luaVM, szName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::AddCommandHandler ( lua_State* luaVM )
{
//  bool addCommandHandler ( string commandName, function handlerFunction, [bool restricted = false, bool caseSensitive = true] )
    SString strKey; CLuaFunctionRef iLuaFunction; bool bRestricted; bool bCaseSensitive;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadFunction ( iLuaFunction );
    argStream.ReadBool ( bRestricted, false );
    argStream.ReadBool ( bCaseSensitive, true );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Add them to our list over command handlers
            if ( m_pRegisteredCommands->AddCommand ( pLuaMain, strKey, iLuaFunction, bRestricted, bCaseSensitive ) )
            {
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


int CLuaFunctionDefinitions::RemoveCommandHandler ( lua_State* luaVM )
{
//  bool removeCommandHandler ( string commandName [, function handler] )
    SString strKey; CLuaFunctionRef iLuaFunction;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadFunction ( iLuaFunction, LUA_REFNIL );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Remove it from our list
            if ( m_pRegisteredCommands->RemoveCommand ( pLuaMain, strKey, iLuaFunction ) )
            {
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


int CLuaFunctionDefinitions::ExecuteCommandHandler ( lua_State* luaVM )
{
//  bool executeCommandHandler ( string commandName, player thePlayer, [ string args ] )
    SString strKey; CElement* pElement; SString strArgs;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strArgs, "" );

    if ( !argStream.HasErrors () )
    {

        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CClient* pClient = NULL;
            if ( pElement->GetType () == CElement::PLAYER )
                pClient = static_cast < CClient* > ( static_cast < CPlayer* > ( pElement ) );

            if ( pClient )
            {

                // Call it
                if ( m_pRegisteredCommands->ProcessCommand ( strKey, strArgs, pClient ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::toJSON ( lua_State* luaVM )
{
    // Got a string argument?
    CScriptArgReader argStream ( luaVM );
    if ( !argStream.NextIsNil () )
    {
        // Read the argument
        CLuaArguments JSON;
        JSON.ReadArgument ( luaVM, 1 );

        // Convert it to a JSON string
        std::string strJSON;
        if ( JSON.WriteToJSONString ( strJSON ) )
        {
            // Return the JSON string
            lua_pushstring ( luaVM, strJSON.c_str () );
            return 1;
        }
    }

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::fromJSON ( lua_State* luaVM )
{
    SString strJSON; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strJSON); 
    
    if ( !argStream.HasErrors ( ) )
    {
        // Read it into lua arguments
        CLuaArguments Converted;
        if ( Converted.ReadFromJSONString ( strJSON ) )
        {
            // Return it as data
            Converted.PushArguments ( luaVM );
            return Converted.Count ();
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFunctionDefinitions::GetMaxPlayers ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, CStaticFunctionDefinitions::GetMaxPlayers () );
    return 1;
}

int CLuaFunctionDefinitions::SetMaxPlayers ( lua_State* luaVM )
{
    unsigned int uiMaxPlayers; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(uiMaxPlayers); 
    
    if ( !argStream.HasErrors ( ) )
    {
        lua_pushboolean ( luaVM, CStaticFunctionDefinitions::SetMaxPlayers ( uiMaxPlayers ) );
        return 1;        
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::OutputChatBox ( lua_State* luaVM )
{
    // bool outputChatBox ( string text [, element visibleTo=getRootElement(), int r=231, int g=217, int b=176, bool colorCoded=false ] )
    SString ssChat;
    CElement* pElement;
    bool bColorCoded;
    // Default
    unsigned char ucRed = 231;
    unsigned char ucGreen = 217;
    unsigned char ucBlue = 176;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( ssChat );
    argStream.ReadUserData ( pElement, m_pRootElement );

    if ( argStream.NextIsNumber() && argStream.NextIsNumber( 1 ) && argStream.NextIsNumber( 2 ) )
    {
        argStream.ReadNumber ( ucRed );
        argStream.ReadNumber ( ucGreen );
        argStream.ReadNumber ( ucBlue );
    }
    else
        argStream.m_iIndex = argStream.m_iIndex + 3;

    argStream.ReadBool ( bColorCoded, false );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CStaticFunctionDefinitions::OutputChatBox ( (const char*)ssChat, pElement, ucRed, ucGreen, ucBlue, bColorCoded, pLuaMain );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;   
}

int CLuaFunctionDefinitions::OutputConsole ( lua_State* luaVM )
{
    SString strMessage; 
    CElement* pElement; 
    
    CScriptArgReader argStream ( luaVM );
    
    argStream.ReadString(strMessage); 
    argStream.ReadUserData(pElement, m_pRootElement); 
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::OutputConsole ( strMessage, pElement ) )
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


int CLuaFunctionDefinitions::OutputDebugString ( lua_State* luaVM )
{
    SString strMessage; 
    unsigned int uiLevel;
    unsigned char ucR, ucG, ucB; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strMessage); 
    argStream.ReadNumber(uiLevel, 3); 
    
    if ( uiLevel == 0 )
    {
        argStream.ReadNumber(ucR, 0xFF);
        argStream.ReadNumber(ucG, 0xFF);
        argStream.ReadNumber(ucB, 0xFF);
    }

    if ( !argStream.HasErrors ( ) )
    {
        if ( uiLevel < 0 || uiLevel > 3 )
        {
            m_pScriptDebugging->LogWarning ( luaVM, "Bad level argument sent to %s (0-3)", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );

            lua_pushboolean ( luaVM, false );
            return 1;
        }

        if ( uiLevel == 1 )
        {
            m_pScriptDebugging->LogError ( luaVM, "%s", strMessage.c_str() );
        }
        else if ( uiLevel == 2 )
        {
            m_pScriptDebugging->LogWarning ( luaVM, "%s", strMessage.c_str() );
        }
        else if ( uiLevel == 3 )
        {
            m_pScriptDebugging->LogInformation ( luaVM, "%s", strMessage.c_str() );
        }
        else if ( uiLevel == 0 )
        {
            m_pScriptDebugging->LogCustom ( luaVM, ucR, ucG, ucB, "%s", strMessage.c_str() );
        }
        lua_pushboolean ( luaVM, true );
        return 1;   
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::OutputServerLog ( lua_State* luaVM )
{
    SString strMessage; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strMessage); 
    
    if ( !argStream.HasErrors ( ) )
    {
        // Print it
        CLogger::LogPrintf ( LOGLEVEL_LOW, "%s\n", strMessage.c_str() );
        lua_pushboolean ( luaVM, true );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::GetDistanceBetweenPoints2D ( lua_State* luaVM )
{
    CVector vecA, vecB;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(vecA.fX);
    argStream.ReadNumber(vecA.fY);
    argStream.ReadNumber(vecB.fX);
    argStream.ReadNumber(vecB.fY);

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushnumber ( luaVM, DistanceBetweenPoints2D ( vecA, vecB ) );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetDistanceBetweenPoints3D ( lua_State* luaVM )
{
    CVector vecA, vecB;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(vecA.fX);
    argStream.ReadNumber(vecA.fY);
    argStream.ReadNumber(vecA.fZ);
    argStream.ReadNumber(vecB.fX);
    argStream.ReadNumber(vecB.fY);
    argStream.ReadNumber(vecB.fZ);

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushnumber ( luaVM, DistanceBetweenPoints3D ( vecA, vecB ) );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetEasingValue ( lua_State* luaVM )
{
//  float getEasingValue( float fProgress, string strEasingType [, float fEasingPeriod, float fEasingAmplitude, float fEasingOvershoot] )
    float fProgress; CEasingCurve::eType easingType; float fEasingPeriod; float fEasingAmplitude; float fEasingOvershoot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fProgress );
    argStream.ReadEnumString ( easingType );
    argStream.ReadNumber ( fEasingPeriod, 0.3f );
    argStream.ReadNumber ( fEasingAmplitude, 1.0f );
    argStream.ReadNumber ( fEasingOvershoot, 1.70158f );

    if ( argStream.HasErrors () )
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CEasingCurve easingCurve ( easingType );
    easingCurve.SetParams ( fEasingPeriod, fEasingAmplitude, fEasingOvershoot );
    lua_pushnumber ( luaVM, easingCurve.ValueForProgress ( fProgress ) ); 
    return 1;
}

int CLuaFunctionDefinitions::InterpolateBetween ( lua_State* luaVM )
{
//  float float float interpolateBetween ( float x1, float y1, float z1, 
//      float x2, float y2, float z2, 
//      float fProgress, string strEasingType, 
//      [ float fEasingPeriod, float fEasingAmplitude, float fEasingOvershoot ] )
    CVector vecPointA; CVector vecPointB;
    float fProgress; CEasingCurve::eType easingType;
    float fEasingPeriod; float fEasingAmplitude; float fEasingOvershoot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPointA.fX );
    argStream.ReadNumber ( vecPointA.fY );
    argStream.ReadNumber ( vecPointA.fZ );
    argStream.ReadNumber ( vecPointB.fX );
    argStream.ReadNumber ( vecPointB.fY );
    argStream.ReadNumber ( vecPointB.fZ );
    argStream.ReadNumber ( fProgress );
    argStream.ReadEnumString ( easingType );
    argStream.ReadNumber ( fEasingPeriod, 0.3f );
    argStream.ReadNumber ( fEasingAmplitude, 1.0f );
    argStream.ReadNumber ( fEasingOvershoot, 1.70158f );

    if ( argStream.HasErrors () )
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    CVector vecResult = TInterpolation < CVector >::Interpolate ( vecPointA, vecPointB, fProgress, easingType, fEasingPeriod, fEasingAmplitude, fEasingOvershoot );
    lua_pushnumber ( luaVM, vecResult.fX );
    lua_pushnumber ( luaVM, vecResult.fY );
    lua_pushnumber ( luaVM, vecResult.fZ );
    return 3;
}

int CLuaFunctionDefinitions::GetTickCount_ ( lua_State* luaVM )
{
    double dTime = GetSecondCount () * 1000.0;
    lua_pushnumber ( luaVM, dTime );
    return 1;
}

int CLuaFunctionDefinitions::GetCTime ( lua_State* luaVM )
{
    // Verify the argument
    time_t timer;
    time ( &timer );
    CScriptArgReader argStream ( luaVM );

    if ( argStream.NextIsNumber ( ) )
    {
        argStream.ReadNumber ( timer );
        if ( timer < 0 )
        {
            lua_pushboolean ( luaVM, 0 );
            return 1;
        }
    }
    tm * time = localtime ( &timer );

    CLuaArguments ret;
    ret.PushString("second");
    ret.PushNumber(time->tm_sec);
    ret.PushString("minute");
    ret.PushNumber(time->tm_min);
    ret.PushString("hour");
    ret.PushNumber(time->tm_hour);
    ret.PushString("monthday");
    ret.PushNumber(time->tm_mday);
    ret.PushString("month");
    ret.PushNumber(time->tm_mon);
    ret.PushString("year");
    ret.PushNumber(time->tm_year);
    ret.PushString("weekday");
    ret.PushNumber(time->tm_wday);
    ret.PushString("yearday");
    ret.PushNumber(time->tm_yday);
    ret.PushString("isdst");
    ret.PushNumber(time->tm_isdst);
    ret.PushString("timestamp");
    ret.PushNumber((double) timer);

    ret.PushAsTable(luaVM);

    return 1;
}


int CLuaFunctionDefinitions::Split ( lua_State* luaVM )
{
    SString strInput = "";
    unsigned int uiDelimiter = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strInput );


    if ( !argStream.HasErrors ( ) )
    {
        SString strDelimiter;
        if ( argStream.NextIsNumber ( ) )
        {
            argStream.ReadNumber ( uiDelimiter );
            wchar_t wUNICODE[2] = { uiDelimiter, '\0' };
            strDelimiter = UTF16ToMbUTF8(wUNICODE);
        }
        else  // It's already a string
            argStream.ReadString ( strDelimiter );

        // Copy the string
        char* strText = new char [ strInput.length ( ) + 1 ];
        strcpy ( strText, strInput );

        unsigned int uiCount = 0;
        char* szToken = strtok ( strText, strDelimiter );

        // Create a new table
        lua_newtable ( luaVM );

        // Add our first token
        lua_pushnumber ( luaVM, ++uiCount );
        lua_pushstring ( luaVM, szToken );
        lua_settable ( luaVM, -3 );

        // strtok until we're out of tokens
        while ( szToken = strtok ( NULL, strDelimiter ) )
        {
            // Add the token to the table
            lua_pushnumber ( luaVM, ++uiCount );
            lua_pushstring ( luaVM, szToken );
            lua_settable ( luaVM, -3 );
        }

        // Delete the text
        delete [] strText;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    return 1;
}

int CLuaFunctionDefinitions::GetTok ( lua_State* luaVM )
{
    SString strInput = "";
    unsigned int uiToken = 0;
    unsigned int uiDelimiter = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strInput );
    argStream.ReadNumber ( uiToken );


    if ( !argStream.HasErrors ( ) )
    {
        SString strDelimiter;
        if ( argStream.NextIsNumber ( ) )
        {
            argStream.ReadNumber ( uiDelimiter );
            wchar_t wUNICODE[2] = { uiDelimiter, '\0' };
            strDelimiter = UTF16ToMbUTF8(wUNICODE);
        }
        else  // It's already a string
            argStream.ReadString ( strDelimiter );

        if ( uiToken > 0 && uiToken < 1024 )
        {
            unsigned int uiCount = 1;
            char* szText = new char [ strInput.length ( ) + 1 ];
            strcpy ( szText, strInput );
            char* szToken = strtok ( szText, strDelimiter );

            // We're looking for the first part?
            if ( uiToken != 1 )
            {
                // strtok count number of times
                do
                {
                    uiCount++;
                    szToken = strtok ( NULL, strDelimiter );
                }
                while ( uiCount != uiToken );
            }

            // Found it?
            if ( szToken )
            {
                // Return it
                lua_pushstring ( luaVM, szToken );
                delete [] szText;
                return 1;
            }

            // Delete the text
            delete [] szText;
        }
        else
            m_pScriptDebugging->LogWarning ( luaVM, "Token parameter sent to split must be greater than 0 and smaller than 1024" );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetTimer ( lua_State* luaVM )
{
//  timer setTimer ( function theFunction, int timeInterval, int timesToExecute, [ var arguments... ] )
    CLuaFunctionRef iLuaFunction; double dTimeInterval; uint uiTimesToExecute; CLuaArguments Arguments;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadFunction ( iLuaFunction );
    argStream.ReadNumber ( dTimeInterval );
    argStream.ReadNumber ( uiTimesToExecute );
    argStream.ReadLuaArguments ( Arguments );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            // Check for the minimum interval
            if ( dTimeInterval < LUA_TIMER_MIN_INTERVAL )
            {
                argStream.SetCustomError( "Interval is below 50" );
            }
            else
            {
                lua_Debug debugInfo;
                SString strDebugOutput = "";
                // Check our luaVM and get a debug info from the stack
                if ( luaVM && lua_getstack ( luaVM, 1, &debugInfo ) )
                {
                    // Get name line and... no idea. for debug
                    lua_getinfo ( luaVM, "nlS", &debugInfo );
                    if ( debugInfo.source[0] == '@' )
                    {
                        SString strFile = ConformResourcePath ( debugInfo.source );
                        // Get and store the location of the setTimer call in case the contents error
                        strDebugOutput = SString ( "%s:%i ", strFile.c_str ( ), debugInfo.currentline );
                    }
                    else
                    {
                        // Get and store the location of the setTimer call in case the contents error
                        strDebugOutput = SString ( "%s ", debugInfo.short_src );
                    }
                }
                CLuaTimer* pLuaTimer = luaMain->GetTimerManager ()->AddTimer ( iLuaFunction, CTickCount ( dTimeInterval ), uiTimesToExecute, Arguments );

                if ( pLuaTimer )
                {
                    // Set our timer debug info (in case we don't have any debug info which is usually when you do setTimer(destroyElement, 50, 1) or such)
                    pLuaTimer->SetDebugInfo ( strDebugOutput );

                    lua_pushtimer ( luaVM, pLuaTimer );
                    return 1;
                }
            }
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::KillTimer ( lua_State* luaVM )
{
//  bool killTimer ( timer theTimer )
    CLuaTimer* pLuaTimer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pLuaTimer );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            luaMain->GetTimerManager ()->RemoveTimer ( pLuaTimer );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ResetTimer ( lua_State* luaVM )
{
//  bool resetTimer ( timer theTimer )
    CLuaTimer* pLuaTimer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pLuaTimer );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            luaMain->GetTimerManager ()->ResetTimer ( pLuaTimer );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsTimer ( lua_State* luaVM )
{
//  bool isTimer ( timer theTimer )
    CLuaTimer* pLuaTimer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pLuaTimer );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTimers ( lua_State* luaVM )
{
//  table getTimers ( [ time ] )
    double dTime;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( dTime, 0 );

    if ( !argStream.HasErrors () )
    {
        // Find our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Create a new table
            lua_newtable ( luaVM );

            // Add all the timers with less than ulTime left
            CLuaTimerManager* pLuaTimerManager = pLuaMain->GetTimerManager ();
            CTickCount llCurrentTime = CTickCount::Now ();
            unsigned int uiIndex = 0;
            CFastList < CLuaTimer* > ::const_iterator iter = pLuaTimerManager->IterBegin ();
            for ( ; iter != pLuaTimerManager->IterEnd () ; ++iter )
            {
                CLuaTimer* pLuaTimer = *iter;

                // If the time left is less than the time specified, or the time specifed is 0
                CTickCount llTimeLeft = ( pLuaTimer->GetStartTime () + pLuaTimer->GetDelay () ) - llCurrentTime;
                if ( dTime == 0 || llTimeLeft.ToDouble () <= dTime )
                {
                    // Add it to the table
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushtimer ( luaVM, pLuaTimer );
                    lua_settable ( luaVM, -3 );
                }
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTimerDetails ( lua_State* luaVM )
{
//  int, int, int getTimerDetails ( timer theTimer )
    CLuaTimer* pLuaTimer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pLuaTimer );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber( luaVM, pLuaTimer->GetTimeLeft ().ToDouble () );
        lua_pushnumber( luaVM, pLuaTimer->GetRepeats () );
        lua_pushnumber( luaVM, pLuaTimer->GetDelay ().ToDouble () );
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetColorFromString ( lua_State* luaVM )
{
    SString strColor;
    
    CScriptArgReader argStream ( luaVM );;
    argStream.ReadString(strColor); 
    
    if ( !argStream.HasErrors ( ) )
    {
        unsigned char ucColorRed, ucColorGreen, ucColorBlue, ucColorAlpha;
        if ( XMLColorToInt ( strColor, ucColorRed, ucColorGreen, ucColorBlue, ucColorAlpha ) )
        {
            lua_pushnumber ( luaVM, ucColorRed );
            lua_pushnumber ( luaVM, ucColorGreen );
            lua_pushnumber ( luaVM, ucColorBlue );
            lua_pushnumber ( luaVM, ucColorAlpha );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::Reference ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    
    if ( !argStream.NextIsNil() && ! argStream.NextIsNone() )
    {
        int iPointer = lua_ref ( luaVM, 1 );
        lua_pushnumber ( luaVM, iPointer );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::Dereference ( lua_State* luaVM )
{
    int iPointer;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(iPointer);
    
    if ( !argStream.HasErrors ( ) )
    {
        lua_getref ( luaVM, iPointer );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::UtfLen ( lua_State* luaVM )
{
    SString strInput; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strInput); 
    
    if ( !argStream.HasErrors ( ) )
    {
       lua_pushnumber ( luaVM, MbUTF8ToUTF16(strInput).size() );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::UtfSeek ( lua_State* luaVM )
{
    SString strInput; 
    int iPos; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strInput); 
    argStream.ReadNumber(iPos); 
    
    if ( !argStream.HasErrors ( ) )
    {    
        std::wstring strUTF = MbUTF8ToUTF16(strInput);
        if ( iPos <= static_cast < int >(strUTF.size()) && iPos >= 0 )
        {
            strUTF = strUTF.substr(0,iPos);
            lua_pushnumber ( luaVM, UTF16ToMbUTF8(strUTF).size() );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::UtfSub ( lua_State* luaVM )
{
    SString strInput; 
    ptrdiff_t iStart; 
    ptrdiff_t iEnd; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strInput); 
    argStream.ReadNumber(iStart); 
    argStream.ReadNumber(iEnd, -1); 
    
    if ( !argStream.HasErrors ( ) )
    {   
        std::wstring strUTF = MbUTF8ToUTF16(strInput);
        size_t l = static_cast < int > ( strUTF.size() );

        //posrelat them both
        if (iStart < 0) iStart += (ptrdiff_t)l + 1;
        iStart = (iStart >= 0) ? iStart : 0;

        if (iEnd < 0) iEnd += (ptrdiff_t)l + 1;
        iEnd = (iEnd >= 0) ? iEnd : 0;

        if (iStart < 1) iStart = 1;
        if (iEnd > (ptrdiff_t)l) iEnd = (ptrdiff_t)l;
        if (iStart <= iEnd)
        {
            strUTF = strUTF.substr(iStart-1, iEnd-iStart+1);
            lua_pushstring( luaVM, UTF16ToMbUTF8(strUTF).c_str());
        }
        else 
            lua_pushliteral( luaVM, "");
        
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::UtfChar ( lua_State* luaVM )
{
    int iCode; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber(iCode); 
    
    if ( !argStream.HasErrors ( ) )
    {   
        if ( iCode > 65534 || iCode < 32 )
        {
            m_pScriptDebugging->LogBadType ( luaVM );
            lua_pushnil ( luaVM );
            return 1;
        }

        // Generate a null-terminating string for our character
        wchar_t wUNICODE[2] = { iCode, '\0' };

        // Convert our UTF character into an ANSI string
        SString strANSI = UTF16ToMbUTF8(wUNICODE);

        lua_pushstring ( luaVM, strANSI.c_str() );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::UtfCode ( lua_State* luaVM )
{
    SString strInput;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strInput); 
    
    if ( !argStream.HasErrors ( ) )
    {   
        std::string strInput = lua_tostring ( luaVM, 1 );
        std::wstring strUTF = MbUTF8ToUTF16(strInput);
        unsigned long ulCode = strUTF.c_str()[0];

        lua_pushnumber ( luaVM, ulCode );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetValidPedModels ( lua_State* luaVM )
{
    int iIndex = 0;
    lua_newtable ( luaVM );
    for( int i = 0; i <= 312; i++)
    {
        if ( CPlayerManager::IsValidPlayerModel(i) )
        {
            lua_pushnumber ( luaVM , ++iIndex);
            lua_pushnumber ( luaVM , i);
            lua_settable ( luaVM , -3);
        }
    }

    return 1;
}


int CLuaFunctionDefinitions::GetRootElement ( lua_State* luaVM )
{
    CElement* pRoot = CStaticFunctionDefinitions::GetRootElement ();
    if ( pRoot )
    {
        lua_pushelement ( luaVM, pRoot );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::LoadMapData ( lua_State* luaVM )
{
    CXMLNode* pXML; 
    CElement* pParent; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pXML); 
    argStream.ReadUserData(pParent); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CElement* pLoadedRoot = CStaticFunctionDefinitions::LoadMapData ( pLuaMain, pParent, pXML );
            if ( pLoadedRoot )
            {
                lua_pushelement ( luaVM, pLoadedRoot );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SaveMapData ( lua_State* luaVM )
{
    CXMLNode* pXML; 
    CElement* pParent; 
    bool bChildren;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData(pXML); 
    argStream.ReadUserData(pParent); 
    argStream.ReadBool(bChildren, false);
    
    if ( !argStream.HasErrors ( ) )
    {  
        CXMLNode* pSavedNode = CStaticFunctionDefinitions::SaveMapData ( pParent, pXML, bChildren );
        if ( pSavedNode )
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


int CLuaFunctionDefinitions::GetMapName ( lua_State* luaVM )
{
    // Grab ASE
    ASE * pASE = ASE::GetInstance ();
    if ( pASE )
    {
        // Grab the mapname string
        const char* szMapName = pASE->GetMapName ();
        if ( szMapName [0] != 0 )
        {
            // Return the gametype string excluding our prefix
            lua_pushstring ( luaVM, szMapName );
            return 1;
        }
    }

    // No game type
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::GetGameType ( lua_State* luaVM )
{
    // Grab ASE
    ASE * pASE = ASE::GetInstance ();
    if ( pASE )
    {
        // Grab the gametype string.
        const char* szGameType = pASE->GetGameType ();

        // Return the gametype string if it's not "MTA:SA"
        if ( strcmp ( szGameType, GAME_TYPE_STRING ) )
        {
            lua_pushstring ( luaVM, szGameType );
            return 1;
        }
    }

    // No game type
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::SetGameType ( lua_State* luaVM )
{
//  bool setGameType ( string gameType )
    SString strGameType;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadIfNextIsString ( strGameType, "" );    // Default to empty for backward compat with previous implementation

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetGameType ( strGameType ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetMapName ( lua_State* luaVM )
{
//  bool setMapName ( string mapName )
    SString strMapName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadIfNextIsString ( strMapName, "" );    // Default to empty for backward compat with previous implementation

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetMapName ( strMapName ) )
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


int CLuaFunctionDefinitions::GetRuleValue ( lua_State* luaVM )
{
//  string getRuleValue ( string key )
    SString strKey;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        const char* szRule = CStaticFunctionDefinitions::GetRuleValue ( strKey );
        if ( szRule )
        {
            lua_pushstring ( luaVM, szRule );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetRuleValue ( lua_State* luaVM )
{
//  bool setRuleValue ( string key, string value )
    SString strKey; SString strValue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strValue );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetRuleValue ( strKey, strValue ) )
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


int CLuaFunctionDefinitions::RemoveRuleValue ( lua_State* luaVM )
{
//  bool removeRuleValue ( string key )
    SString strKey;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RemoveRuleValue ( strKey ) )
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


int CLuaFunctionDefinitions::GetPlayerAnnounceValue ( lua_State* luaVM )
{
// string getPlayerAnnounceValue ( element thePlayer, string key )
    CPlayer* pPlayer; SString strKey;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        SString strValue;
        if ( CStaticFunctionDefinitions::GetPlayerAnnounceValue ( pPlayer, strKey, strValue ) )
        {
            lua_pushstring ( luaVM, strValue );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerAnnounceValue ( lua_State* luaVM )
{
// bool setPlayerAnnounceValue ( element thePlayer, string key, string value )
    CPlayer* pPlayer; SString strKey; SString strValue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strValue );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPlayerAnnounceValue ( pPlayer, strKey, strValue ) )
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


int CLuaFunctionDefinitions::ResendPlayerModInfo ( lua_State* luaVM )
{
    // bool getPlayerModInfo ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        g_pNetServer->ResendModPackets ( pPlayer->GetSocket () );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLCreateTable ( lua_State* luaVM )
{
    SString strTable; 
    SString strDefinition; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strTable); 
    argStream.ReadString(strDefinition); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        CStaticFunctionDefinitions::ExecuteSQLCreateTable(strTable, strDefinition);
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLDropTable ( lua_State* luaVM )
{
    SString strTable; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strTable); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        CStaticFunctionDefinitions::ExecuteSQLDropTable(strTable);
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::ExecuteSQLDelete ( lua_State* luaVM )
{
    SString strTable; 
    SString strDefinition; 
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strTable); 
    argStream.ReadString(strDefinition); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        if ( CStaticFunctionDefinitions::ExecuteSQLDelete(strTable, strDefinition) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
        {
            SString strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError ();
            m_pScriptDebugging->LogError ( luaVM, strError.c_str () );
            
            lua_pushstring ( luaVM, strError );
            lua_pushboolean ( luaVM, false );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLInsert ( lua_State* luaVM )
{
    SString strTable; 
    SString strDefinition; 
    SString strColumns;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strTable); 
    argStream.ReadString(strDefinition); 
    argStream.ReadString(strColumns, ""); 
    
    if ( !argStream.HasErrors ( ) )
    {
        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        if ( CStaticFunctionDefinitions::ExecuteSQLInsert(strTable, strDefinition, strColumns) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
        {
            SString strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError ();
            m_pScriptDebugging->LogError ( luaVM, strError.c_str () );
            
            lua_pushstring ( luaVM, strError );
            lua_pushboolean ( luaVM, false );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


//
// Db function callbacks
//

// Handle directing callback for DbQuery
static void DbQueryCallback ( CDbJobData* pJobData, void* pContext )
{
    CLuaCallback* pLuaCallback = (CLuaCallback*)pContext;
    if ( pJobData->stage == EJobStage::RESULT )
    {
        if ( pLuaCallback )
            pLuaCallback->Call ();
    }
    g_pGame->GetLuaCallbackManager ()->DestroyCallback ( pLuaCallback );
}

// Handle callback for DbExec
static void DbExecCallback ( CDbJobData* pJobData, void* pContext )
{
    assert ( pContext == NULL );
    if ( pJobData->stage >= EJobStage::RESULT && pJobData->result.status == EJobResult::FAIL )
    {
        if ( !pJobData->result.bErrorSuppressed )
            m_pScriptDebugging->LogWarning ( NULL, "%s: dbExec failed; (%d) %s", *pJobData->m_strDebugInfo, pJobData->result.uiErrorCode, *pJobData->result.strReason );
    }
}

// Handle callback for DbFree
static void DbFreeCallback ( CDbJobData* pJobData, void* pContext )
{
    assert ( pContext == NULL );
    if ( pJobData->stage >= EJobStage::RESULT && pJobData->result.status == EJobResult::FAIL )
    {
        if ( !pJobData->result.bErrorSuppressed )
            m_pScriptDebugging->LogWarning ( NULL, "%s: dbFree failed; (%d) %s", *pJobData->m_strDebugInfo, pJobData->result.uiErrorCode, *pJobData->result.strReason );
    }
}

int CLuaFunctionDefinitions::DbConnect ( lua_State* luaVM )
{
//  element dbConnect ( string type, string host, string username, string password, string options )
    SString strType; SString strHost; SString strUsername; SString strPassword; SString strOptions;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strType );
    argStream.ReadString ( strHost );
    argStream.ReadString ( strUsername, "" );
    argStream.ReadString ( strPassword, "" );
    argStream.ReadString ( strOptions, "" );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pThisResource = pLuaMain->GetResource ();

            // If type is sqlite, and has a host, try to resolve path
            if ( strType == "sqlite" && !strHost.empty () )
            {
                // If path starts with :/ then use global database directory
                if ( strHost.BeginsWith ( ":/" ) )
                {
                    strHost = strHost.SubStr ( 1 );
                    if ( !IsValidFilePath ( strHost ) )
                    {
                        m_pScriptDebugging->LogError ( luaVM, "%s failed; host path not valid", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
                        lua_pushboolean ( luaVM, false );
                        return 1;
                    }
                    strHost = PathJoin ( g_pGame->GetConfig ()->GetGlobalDatabasesPath (), strHost );
                }
                else
                {
                    std::string strAbsPath;

                    // Parse path
                    CResource* pPathResource = pThisResource;
                    if ( CResourceManager::ParseResourcePathInput ( strHost, pPathResource, &strAbsPath, NULL ) )
                    {
                        if ( pPathResource == pThisResource ||
                             m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                                CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                                "ModifyOtherObjects",
                                                                CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                                false ) )
                        {
                            strHost = strAbsPath;
                        }
                        else
                        {
                            m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pPathResource->GetName ().c_str () );
                            lua_pushboolean ( luaVM, false );
                            return 1;
                        }
                    }
                    else
                    {
                        m_pScriptDebugging->LogError ( luaVM, "%s failed; host path %s not found", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *strHost );
                        lua_pushboolean ( luaVM, false );
                        return 1;
                    }
                }
            }
            // Add logging options
            bool bLoggingEnabled;
            SString strLogTag;
            // Read value of 'log' and 'tag' if already set, otherwise use default
            GetOption < CDbOptionsMap > ( strOptions, "log", bLoggingEnabled, 1 );
            GetOption < CDbOptionsMap > ( strOptions, "tag", strLogTag, "script" );
            SetOption < CDbOptionsMap > ( strOptions, "log", bLoggingEnabled );
            SetOption < CDbOptionsMap > ( strOptions, "tag", strLogTag );
            // Do connect
            SConnectionHandle connection = g_pGame->GetDatabaseManager ()->Connect ( strType, strHost, strUsername, strPassword, strOptions );
            if ( connection == INVALID_DB_HANDLE )
            {
                m_pScriptDebugging->LogError ( luaVM, "%s failed; %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *g_pGame->GetDatabaseManager ()->GetLastErrorMessage () );
                lua_pushboolean ( luaVM, false );
                return 1;
            }

            // Use an element to wrap the connection for auto disconnected when the resource stops
            CDatabaseConnectionElement* pElement = new CDatabaseConnectionElement ( NULL, connection );
            CElementGroup * pGroup = pThisResource->GetElementGroup();
            if ( pGroup )
            {
                pGroup->Add ( pElement );
            }

            lua_pushelement ( luaVM, pElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::DbQuery ( lua_State* luaVM )
{
//  handle dbQuery ( [ function callbackFunction, [ table callbackArguments, ] ] element connection, string query, ... )
    CLuaFunctionRef iLuaFunction; CLuaArguments callbackArgs; CDatabaseConnectionElement* pElement; SString strQuery; CLuaArguments Args;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsFunction () )
    {
        argStream.ReadFunction ( iLuaFunction );
        if ( argStream.NextIsTable () )
        {
            lua_pushnil ( luaVM );      // Loop through our table, beginning at the first key
            while ( lua_next ( luaVM, argStream.m_iIndex ) != 0 )
            {
                callbackArgs.ReadArgument ( luaVM, -1 );    // Ignore the index at -2, and just read the value
                lua_pop ( luaVM, 1 );                       // Remove the item and keep the key for the next iteration
            }
            argStream.m_iIndex++;
        }
    }
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strQuery );
    argStream.ReadLuaArguments ( Args );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Start async query
        CDbJobData* pJobData = g_pGame->GetDatabaseManager ()->QueryStart ( pElement->GetConnectionHandle (), strQuery, &Args );
        if ( !pJobData )
        {
            if ( !g_pGame->GetDatabaseManager ()->IsLastErrorSuppressed () )
                m_pScriptDebugging->LogWarning ( luaVM, "%s failed; %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *g_pGame->GetDatabaseManager ()->GetLastErrorMessage () );
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        // Make callback function if required
        if ( VERIFY_FUNCTION( iLuaFunction ) )
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CLuaArguments Arguments;
                Arguments.PushDbQuery ( pJobData );
                Arguments.PushArguments ( callbackArgs );
                pJobData->SetCallback ( DbQueryCallback, g_pGame->GetLuaCallbackManager ()->CreateCallback ( pLuaMain, iLuaFunction, Arguments ) );
            }
        }
        // Add debug info incase query result does not get collected
        pJobData->SetDebugInfo ( GetDebugMessage ( luaVM ) );
        lua_pushquery ( luaVM, pJobData );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::DbExec ( lua_State* luaVM )
{
//  bool dbExec ( element connection, string query, ... )
    CDatabaseConnectionElement* pElement; SString strQuery; CLuaArguments Args;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strQuery );
    argStream.ReadLuaArguments ( Args );

    if ( !argStream.HasErrors () )
    {
        // Start async query
        CDbJobData* pJobData = g_pGame->GetDatabaseManager ()->Exec ( pElement->GetConnectionHandle (), strQuery, &Args );
        if ( !pJobData )
        {
            if ( !g_pGame->GetDatabaseManager ()->IsLastErrorSuppressed () )
                m_pScriptDebugging->LogError ( luaVM, "%s failed: %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *g_pGame->GetDatabaseManager ()->GetLastErrorMessage () );
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        // Add callback for tracking errors
        pJobData->SetCallback ( DbExecCallback, NULL );
        pJobData->SetDebugInfo ( GetDebugMessage ( luaVM ) );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::DbFree ( lua_State* luaVM )
{
//  bool dbFree ( handle query )
    CDbJobData* pJobData;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pJobData );

    if ( !argStream.HasErrors () )
    {
        // Add callback for tracking errors
        pJobData->SetCallback ( DbFreeCallback, NULL );
        pJobData->SetDebugInfo ( GetDebugMessage ( luaVM ) );

        bool bResult = g_pGame->GetDatabaseManager ()->QueryFree ( pJobData );
        lua_pushboolean ( luaVM, bResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::DbPoll ( lua_State* luaVM )
{
//  table dbPoll ( handle query, int timeout )
    CDbJobData* pJobData; uint uiTimeout;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pJobData );
    argStream.ReadNumber ( uiTimeout );

    if ( !argStream.HasErrors () )
    {
        // Extra input validation
        if ( pJobData->stage > EJobStage::RESULT )
            argStream.SetCustomError( "Previous dbPoll already returned result" );
        if ( pJobData->result.bIgnoreResult )
            argStream.SetCustomError( "Cannot call dbPoll after dbFree" );
    }

    if ( !argStream.HasErrors () )
    {
        if ( !g_pGame->GetDatabaseManager ()->QueryPoll ( pJobData, uiTimeout ) )
        {
            // Not ready yet
            lua_pushnil ( luaVM );
            return 1;
        }

        if ( pJobData->result.status == EJobResult::FAIL )
        {
            if ( !g_pGame->GetDatabaseManager ()->IsLastErrorSuppressed () )
                m_pScriptDebugging->LogWarning ( luaVM, "%s failed; %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *g_pGame->GetDatabaseManager ()->GetLastErrorMessage () );
            lua_pushboolean ( luaVM, false );
            lua_pushnumber ( luaVM, pJobData->result.uiErrorCode );
            lua_pushstring ( luaVM, pJobData->result.strReason );
            return 3;
        }

        const CRegistryResult& Result = pJobData->result.registryResult;

        // Make table!
        lua_newtable ( luaVM );
        for ( int i = 0; i < Result.nRows; i++ ) {
            lua_newtable ( luaVM );                             // new table
            lua_pushnumber ( luaVM, i+1 );                      // row index number (starting at 1, not 0)
            lua_pushvalue ( luaVM, -2 );                        // value
            lua_settable ( luaVM, -4 );                         // refer to the top level table
            for ( int j = 0; j < Result.nColumns; j++ )
            {
                const CRegistryResultCell& cell = Result.Data[i][j];

                // Push the column name
                lua_pushlstring ( luaVM, Result.ColNames[j].c_str (), Result.ColNames[j].size () );
                switch ( cell.nType )                           // push the value with the right type
                {
                    case SQLITE_INTEGER:
                        lua_pushnumber ( luaVM, static_cast < double > ( cell.nVal ) );
                        break;
                    case SQLITE_FLOAT:
                        lua_pushnumber ( luaVM, cell.fVal );
                        break;
                    case SQLITE_BLOB:
                        lua_pushlstring ( luaVM, (const char *)cell.pVal, cell.nLength );
                        break;
                    case SQLITE_TEXT:
                        lua_pushlstring ( luaVM, (const char *)cell.pVal, cell.nLength - 1 );
                        break;
                    default:
                        lua_pushboolean ( luaVM, false );
                }
                lua_settable ( luaVM, -3 );
            }
            lua_pop ( luaVM, 1 );                               // pop the inner table
        }
        lua_pushnumber ( luaVM, pJobData->result.uiNumAffectedRows );
        lua_pushnumber ( luaVM, static_cast < double > ( pJobData->result.ullLastInsertId ) );
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLQuery ( lua_State* luaVM )
{
    SString strQuery;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strQuery );
    

    if ( !argStream.HasErrors () )
    {
        CLuaArguments Args;
        CRegistryResult Result;

        Args.ReadArguments ( luaVM, 2 );

        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        if ( CStaticFunctionDefinitions::ExecuteSQLQuery ( strQuery, &Args, &Result ) ) {
            lua_newtable ( luaVM );
            for ( int i = 0; i < Result.nRows; i++ ) {
                lua_newtable ( luaVM );                             // new table
                lua_pushnumber ( luaVM, i+1 );                      // row index number (starting at 1, not 0)
                lua_pushvalue ( luaVM, -2 );                        // value
                lua_settable ( luaVM, -4 );                         // refer to the top level table
                for ( int j = 0; j < Result.nColumns; j++ )
                {
                    CRegistryResultCell& cell = Result.Data[i][j];
                    if ( cell.nType == SQLITE_NULL )
                        continue;

                    // Push the column name
                    lua_pushlstring ( luaVM, Result.ColNames[j].c_str (), Result.ColNames[j].size () );
                    switch ( cell.nType )                           // push the value with the right type
                    {
                        case SQLITE_INTEGER:
                            lua_pushnumber ( luaVM, static_cast < double > ( cell.nVal ) );
                            break;
                        case SQLITE_FLOAT:
                            lua_pushnumber ( luaVM, cell.fVal );
                            break;
                        case SQLITE_BLOB:
                            lua_pushlstring ( luaVM, (const char *)cell.pVal, cell.nLength );
                            break;
                        case SQLITE_TEXT:
                            lua_pushlstring ( luaVM, (const char *)cell.pVal, cell.nLength - 1 );
                            break;
                        default:
                            lua_pushnil ( luaVM );
                    }
                    lua_settable ( luaVM, -3 );
                }
                lua_pop ( luaVM, 1 );                               // pop the inner table
            }
            return 1;
        } 
        else
        {
            SString strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError ();
            m_pScriptDebugging->LogError ( luaVM, strError.c_str () );
            
            lua_pushstring ( luaVM, strError );
            lua_pushboolean ( luaVM, false );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::ExecuteSQLSelect ( lua_State* luaVM )
{
    SString strTable; 
    SString strColumns; 
    SString strWhere; 
    SString strError;
    unsigned int uiLimit;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strTable); 
    argStream.ReadString(strColumns); 
    argStream.ReadString(strWhere, ""); 
    argStream.ReadNumber(uiLimit, 0);
    
    if ( !argStream.HasErrors ( ) )
    {
        CRegistryResult Result;
        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        if ( CStaticFunctionDefinitions::ExecuteSQLSelect ( strTable, strColumns, strWhere, uiLimit, &Result ) )
        {
            lua_newtable ( luaVM );
            for ( int i = 0; i < Result.nRows; i++ ) {
                lua_newtable ( luaVM );                             // new table
                lua_pushnumber ( luaVM, i+1 );                      // row index number (starting at 1, not 0)
                lua_pushvalue ( luaVM, -2 );                        // value
                lua_settable ( luaVM, -4 );                         // refer to the top level table
                for ( int j = 0; j < Result.nColumns; j++ )
                {
                    CRegistryResultCell& cell = Result.Data[i][j];
                    if ( cell.nType == SQLITE_NULL )
                        continue;

                    // Push the column name
                    lua_pushlstring ( luaVM, Result.ColNames[j].c_str (), Result.ColNames[j].size () );
                    switch ( cell.nType )                           // push the value with the right type
                    {
                        case SQLITE_INTEGER:
                            lua_pushnumber ( luaVM, static_cast < double > ( cell.nVal ) );
                            break;
                        case SQLITE_FLOAT:
                            lua_pushnumber ( luaVM, cell.fVal );
                            break;
                        case SQLITE_BLOB:
                            lua_pushlstring ( luaVM, (const char *)cell.pVal, cell.nLength );
                            break;
                        case SQLITE_TEXT:
                            lua_pushlstring ( luaVM, (const char *)cell.pVal, cell.nLength - 1 );
                            break;
                        default:
                            lua_pushnil ( luaVM );
                    }
                    lua_settable ( luaVM, -3 );
                }
                lua_pop ( luaVM, 1 );                               // pop the inner table
            }
            return 1;
        }
        else
        {
            strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError ();
            m_pScriptDebugging->LogError ( luaVM, strError.c_str () );
                        
            lua_pushstring ( luaVM, strError );
            lua_pushboolean ( luaVM, false );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLUpdate ( lua_State* luaVM )
{
    SString strTable; 
    SString strSet; 
    SString strWhere; 
    SString strError;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString(strTable); 
    argStream.ReadString(strSet); 
    argStream.ReadString(strWhere, ""); 
    
    if ( !argStream.HasErrors ( ) )
    {
   
        CPerfStatSqliteTiming::GetSingleton ()->SetCurrentResource ( luaVM );
        if ( CStaticFunctionDefinitions::ExecuteSQLUpdate ( strTable, strSet, strWhere ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        } else {
            strError = "Database query failed: " + CStaticFunctionDefinitions::SQLGetLastError ();
            m_pScriptDebugging->LogError ( luaVM, strError.c_str () );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    lua_pushstring ( luaVM, strError.c_str () );
    return 2;
}


int CLuaFunctionDefinitions::GetAccountName ( lua_State* luaVM )
{
//  string getAccountName ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );

    if ( !argStream.HasErrors () )
    {
        std::string strName = pAccount->GetName ();
        if ( !strName.empty () )
        {
            lua_pushstring ( luaVM, strName.c_str () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetAccountPlayer ( lua_State* luaVM )
{
//  player getAccountPlayer ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );

    if ( !argStream.HasErrors () )
    {
        CClient* pClient = CStaticFunctionDefinitions::GetAccountPlayer ( pAccount );
        if ( pClient )
        {
            lua_pushelement ( luaVM, pClient->GetElement () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsGuestAccount ( lua_State* luaVM )
{
//  bool isGuestAccount ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );

    if ( !argStream.HasErrors () )
    {
        bool bGuest;
        if ( CStaticFunctionDefinitions::IsGuestAccount ( pAccount, bGuest ) )
        {
            lua_pushboolean ( luaVM, bGuest );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetAccountData ( lua_State* luaVM )
{
//  string getAccountData ( account theAccount, string key )
    CAccount* pAccount; SString strKey;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        CLuaArgument * pArgument = CStaticFunctionDefinitions::GetAccountData ( pAccount, strKey );
        if ( pArgument )
        {
            pArgument->Push ( luaVM );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetAllAccountData ( lua_State* luaVM )
{
    //  string getAllAccountData ( account theAccount )
    CAccount* pAccount;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );
    if ( !argStream.HasErrors () )
    {
        lua_newtable ( luaVM );
        CStaticFunctionDefinitions::GetAllAccountData ( luaVM, pAccount );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetAccount ( lua_State* luaVM )
{
//  account getAccount ( string username, [ string password ] )
    SString strName; SString strPassword;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadString ( strPassword, "" );

    if ( !argStream.HasErrors () )
    {
        CAccount* pAccount = CStaticFunctionDefinitions::GetAccount ( strName, strPassword.empty () ? NULL : strPassword.c_str() );
        if ( pAccount )
        {
            lua_pushaccount ( luaVM, pAccount );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetAccounts ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        lua_newtable ( luaVM );

        CStaticFunctionDefinitions::GetAccounts ( pLuaMain );

        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::AddAccount ( lua_State* luaVM )
{
//  account addAccount ( string name, string pass )
    SString strName; SString strPassword;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strName );
    argStream.ReadString ( strPassword );

    if ( !argStream.HasErrors () )
    {
        CAccount* pAccount;
        if ( ( pAccount = CStaticFunctionDefinitions::AddAccount ( strName, strPassword ) ) )
        {
            lua_pushaccount ( luaVM, pAccount );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemoveAccount ( lua_State* luaVM )
{
//  bool removeAccount ( account theAccount )
    CAccount* pAccount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RemoveAccount ( pAccount ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }

        CClient* pClient = pAccount->GetClient ();
        if ( pClient )
            argStream.SetCustomError( "Unable to remove account as unable to log out client. (Maybe onPlayerLogout is cancelled)" );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetAccountPassword ( lua_State* luaVM )
{
//  bool setAccountPassword ( account theAccount, string password )
    CAccount* pAccount; SString strPassword;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );
    argStream.ReadString ( strPassword );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetAccountPassword ( pAccount, strPassword ) )
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


int CLuaFunctionDefinitions::SetAccountData ( lua_State* luaVM )
{
//  bool setAccountData ( account theAccount, string key, string value )
    CAccount* pAccount; SString strKey; CLuaArgument Variable;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );
    argStream.ReadString ( strKey );
    argStream.ReadLuaArgument ( Variable );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetAccountData ( pAccount, strKey, &Variable ) )
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


int CLuaFunctionDefinitions::CopyAccountData ( lua_State* luaVM )
{
//  bool copyAccountData ( account theAccount, account fromAccount )
    CAccount* pAccount; CAccount* pFromAccount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pAccount );
    argStream.ReadUserData ( pFromAccount );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::CopyAccountData ( pAccount, pFromAccount ) )
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


int CLuaFunctionDefinitions::LogIn ( lua_State* luaVM )
{
//  bool logIn ( player thePlayer, account theAccount, string thePassword )
    CPlayer* pPlayer; CAccount* pAccount; SString strPassword;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadUserData ( pAccount );
    argStream.ReadString ( strPassword );

    if ( !argStream.HasErrors () )
    {
        // Log him in
        if ( CStaticFunctionDefinitions::LogIn ( pPlayer, pAccount, strPassword ) )
        {
            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::LogOut ( lua_State* luaVM )
{
//  bool logOut ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        // Log out
        if ( CStaticFunctionDefinitions::LogOut ( pPlayer ) )
        {
            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CancelEvent ( lua_State* luaVM )
{
//  bool cancelEvent ( [ bool cancel = true, string reason = "" ] )
    bool bCancel; SString strReason;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bCancel, true );
    argStream.ReadString ( strReason, "" );

    if ( !argStream.HasErrors () )
    {
        CStaticFunctionDefinitions::CancelEvent ( bCancel, strReason );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetCancelReason ( lua_State* luaVM )
{
    const char* szReason = CStaticFunctionDefinitions::GetCancelReason ( );
    if ( szReason )
    {
        lua_pushstring ( luaVM, szReason );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::WasEventCancelled ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, CStaticFunctionDefinitions::WasEventCancelled () );
    return 1;
}


int CLuaFunctionDefinitions::KickPlayer ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strResponsible;
    SString strReason;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if ( argStream.NextIsUserData() )
    {
        CPlayer *pResp;
        argStream.ReadUserData(pResp);
        strResponsible = pResp->GetNick();

        argStream.ReadString(strReason, "");
    }
    else
    {
        if ( argStream.NextIsString(1) ) 
        {
            argStream.ReadString ( strResponsible );
            argStream.ReadString ( strReason );
        }
        else
        {
            argStream.ReadString ( strReason, "" );
            strResponsible = "";
        }
    }

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::KickPlayer ( pPlayer, strResponsible, strReason ) )
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


int CLuaFunctionDefinitions::BanPlayer ( lua_State* luaVM )
{
    CPlayer* pPlayer;
    SString strResponsible;
    SString strReason;
    CPlayer* pResponsible = NULL;
    
    bool bIP;
    bool bUsername;
    bool bSerial;

    time_t tUnban;


    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);
    argStream.ReadBool(bIP, true);
    argStream.ReadBool(bUsername, false);
    argStream.ReadBool(bSerial, false);
    if (argStream.NextIsUserData())
    {
        argStream.ReadUserData(pResponsible, NULL);
        if(!argStream.HasErrors() && pResponsible)
            strResponsible = pResponsible->GetNick();
    }
    else
        argStream.ReadString(strResponsible, "Console");

    argStream.ReadString(strReason, "");

    if ( argStream.NextIsString() )
    {
        SString strTime;
        argStream.ReadString(strTime);
        tUnban = atoi(strTime);
    }
    else
        argStream.ReadNumber(tUnban, 0);

    if ( !argStream.HasErrors () )
    {
        CBan* pBan = NULL;
        if ( ( pBan = CStaticFunctionDefinitions::BanPlayer ( pPlayer, bIP, bUsername, bSerial, pResponsible, strResponsible, strReason, tUnban ) ) )
        {
            lua_pushban ( luaVM, pBan );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::AddBan ( lua_State* luaVM )
{
    SString strIP          = "";
    SString strUsername    = "";
    SString strSerial      = "";
    SString strResponsible = "Console";
    CPlayer * pResponsible = NULL; 
    SString strReason      = "";
    time_t tUnban;


    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strIP, "");
    argStream.ReadString(strUsername, "");
    argStream.ReadString(strSerial, "");
    if (argStream.NextIsUserData())
    {
        argStream.ReadUserData(pResponsible, NULL);
        if(!argStream.HasErrors() && pResponsible)
            strResponsible = pResponsible->GetNick();
    }
    else
        argStream.ReadString(strResponsible, "Console");

    argStream.ReadString(strReason, "");

    if ( argStream.NextIsString() )
    {
        SString strTime;
        argStream.ReadString(strTime);
        tUnban = atoi(strTime);
    }
    else
        argStream.ReadNumber(tUnban, 0);



    if ( !argStream.HasErrors () )
    {
        CBan* pBan = NULL;
        if ( ( pBan = CStaticFunctionDefinitions::AddBan ( strIP, strUsername, strSerial, pResponsible, strResponsible, strReason, tUnban ) ) )
        {
            lua_pushban ( luaVM, pBan );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemoveBan ( lua_State* luaVM )
{
    CBan* pBan;
    CPlayer* pResponsible;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBan);
    argStream.ReadUserData(pResponsible, NULL);

    if ( !argStream.HasErrors( ) )
    {
        if ( CStaticFunctionDefinitions::RemoveBan ( pBan, pResponsible ) )
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


int CLuaFunctionDefinitions::GetBans ( lua_State* luaVM )
{
    // Grab its lua
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        lua_newtable ( luaVM );

        CStaticFunctionDefinitions::GetBans ( luaVM );

        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ReloadBanList ( lua_State* luaVM )
{
    bool bSuccess = CStaticFunctionDefinitions::ReloadBanList ();
    if ( !bSuccess )
        m_pScriptDebugging->LogError ( luaVM, "%s: Ban List failed to reload, fix any errors and run again", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
    lua_pushboolean ( luaVM, bSuccess );
    return 1;
}


int CLuaFunctionDefinitions::GetBanIP ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBan);

    if ( !argStream.HasErrors( ) )
    {
            SString strIP;
            if ( CStaticFunctionDefinitions::GetBanIP ( pBan, strIP ) )
            {
                lua_pushstring ( luaVM, strIP );
                return 1;
            }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetBanSerial ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBan);

    if ( !argStream.HasErrors( ) )
    {
        SString strSerial;
        if ( CStaticFunctionDefinitions::GetBanSerial ( pBan, strSerial ) )
        {
            lua_pushstring ( luaVM, strSerial );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBanUsername ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBan);

    if ( !argStream.HasErrors( ) )
    {
        SString strUser;
        if ( CStaticFunctionDefinitions::GetBanUsername ( pBan, strUser ) )
        {
            lua_pushstring ( luaVM, strUser );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::GetBanNick ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBan);

    if ( !argStream.HasErrors( ) )
    {
        SString strNick;
        if ( CStaticFunctionDefinitions::GetBanNick ( pBan, strNick ) )
        {
            lua_pushstring ( luaVM, strNick );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBanTime ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBan);

    if ( !argStream.HasErrors( ) )
    {
        time_t tTime;
        if ( CStaticFunctionDefinitions::GetBanTime ( pBan, tTime ) )
        {
            lua_pushnumber ( luaVM, (double)tTime );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetUnbanTime ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBan);

    if ( !argStream.HasErrors( ) )
    {
        time_t tTime;
        if ( CStaticFunctionDefinitions::GetUnbanTime ( pBan, tTime ) )
        {
            lua_pushnumber ( luaVM, (double)tTime );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBanReason ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBan);

    if ( !argStream.HasErrors( ) )
    {
        SString strReason;
        if ( CStaticFunctionDefinitions::GetBanReason ( pBan, strReason ) )
        {
            lua_pushstring ( luaVM, strReason );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBanAdmin ( lua_State* luaVM )
{
    CBan* pBan;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pBan);

    if ( !argStream.HasErrors( ) )
    {
        SString strAdmin;
        if ( CStaticFunctionDefinitions::GetBanAdmin ( pBan, strAdmin ) )
        {
            lua_pushstring ( luaVM, strAdmin );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsCursorShowing ( lua_State* luaVM )
{
    CPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if ( !argStream.HasErrors( ) )
    {
        bool bShowing;
        if ( CStaticFunctionDefinitions::IsCursorShowing ( pPlayer, bShowing ) )
        {
            lua_pushboolean ( luaVM, bShowing );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ShowCursor ( lua_State* luaVM )
{
    CElement* pPlayer;
    bool bShow;
    bool bToggleControls;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);
    argStream.ReadBool(bShow);
    argStream.ReadBool(bToggleControls, true);

    if ( !argStream.HasErrors( ) )
    {     
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( CStaticFunctionDefinitions::ShowCursor ( pPlayer, luaMain, bShow, bToggleControls ) )
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


int CLuaFunctionDefinitions::ShowChat ( lua_State* luaVM )
{
    CElement* pPlayer;
    bool bShow;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);
    argStream.ReadBool(bShow);

    if ( !argStream.HasErrors( ) )
    {     
        if ( CStaticFunctionDefinitions::ShowChat ( pPlayer, bShow) )
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



int CLuaFunctionDefinitions::ResetMapInfo ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement, NULL);

    if ( !argStream.HasErrors( ) )
    {     
        if ( CStaticFunctionDefinitions::ResetMapInfo ( pElement ) )
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


int CLuaFunctionDefinitions::GetServerPort ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pGame->GetConfig()->GetServerPort() );
    return 1;
}

int CLuaFunctionDefinitions::Set ( lua_State* luaVM )
{
    CResource* pResource = m_pResourceManager->GetResourceFromLuaState ( luaVM );
    SString strSetting;
    CLuaArguments Args;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strSetting);
    argStream.ReadLuaArguments(Args);

    if ( !argStream.HasErrors( ) )
    {      
        std::string strResourceName = pResource->GetName ();
        std::string strJSON;
        Args.WriteToJSONString ( strJSON );

        if ( g_pGame->GetSettings ()->Set ( strResourceName.c_str (), strSetting.c_str (), strJSON.c_str () ) )
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


/* #define PUSH_SETTING(x,buf) \
    pAttributes = &(x->GetAttributes ()); \
    Args.PushString ( pAttributes->Find ( "name" )->GetValue ().c_str () ); \
    buf = const_cast < char* > ( pAttributes->Find ( "value" )->GetValue ().c_str () ); \
    if ( !Args.ReadFromJSONString ( buf ) ) { \
        Args.PushString ( buf ); \
    }
*/

int CLuaFunctionDefinitions::Get ( lua_State* luaVM )
{
    CResource* pResource = m_pResourceManager->GetResourceFromLuaState ( luaVM );
    SString strSetting;
    CLuaArguments Args;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strSetting);

    if ( !argStream.HasErrors( ) )
    {        
        unsigned int uiIndex = 0;
        bool bDeleteNode;

        // Extract attribute name if setting to be gotten has three parts i.e. resname.settingname.attributename
        SString strAttribute = "value";
        vector < SString > Result;
        strSetting.Split ( ".", Result );
        if ( Result.size () == 3 && Result[2].length () )
        {
            strAttribute = Result[2];
        }

        // Get the setting
        CXMLNode *pSubNode, *pNode = g_pGame->GetSettings ()->Get ( pResource->GetName ().c_str (), strSetting.c_str (), bDeleteNode );

        // Only proceed if we have a valid node
        if ( pNode ) {
            // Argument count
            unsigned int uiArgCount = 1;

            // See if we need to return a table with single or multiple entries
            if ( pNode->GetSubNodeCount () == 0 ) {
                // See if required attribute exists
                CXMLAttribute *pAttribute = pNode->GetAttributes().Find ( strAttribute.c_str () );
                if ( !pAttribute )
                {
                    if ( bDeleteNode )
                        delete pNode;
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }
                // We only have a single entry for a specific setting, so output a string
                const std::string& strDataValue = pAttribute->GetValue ();
                if ( !Args.ReadFromJSONString ( strDataValue.c_str () ) ) {
                    // No valid JSON? Parse as plain text
                    Args.PushString ( strDataValue );
                }

                Args.PushArguments ( luaVM );
                uiArgCount = Args.Count ();

                /* Don't output a table because although it is more consistent with the multiple values output below,
                ** due to lua's implementation of associative arrays (assuming we use the "setting-name", "value" key-value pairs)
                ** it would require the scripter to walk through an array that only has a single entry which is a Bad Thing, performance wise.
                **
                PUSH_SETTING ( pNode );
                Args.PushAsTable ( luaVM );
                **/
            } else {
                // We need to return multiply entries, so push all subnodes
                while ( ( pSubNode = pNode->FindSubNode ( "setting", uiIndex++ ) ) )
                {
                    CXMLAttributes& attributes = pSubNode->GetAttributes ();
                    Args.PushString ( attributes.Find ( "name" )->GetValue () );
                    const std::string& strDataValue = attributes.Find ( "value" )->GetValue ();
                    if ( !Args.ReadFromJSONString ( strDataValue.c_str () ) )
                    {
                        Args.PushString ( strDataValue );
                    }
                }
                // Push a table and return
                Args.PushAsTable ( luaVM );
            }

            // Check if we have to delete the node
            if ( bDeleteNode )
                delete pNode;

            return uiArgCount;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::Md5 ( lua_State* luaVM )
{    
    SString strSetting;
    
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strSetting);
    
    if ( !argStream.HasErrors( ) )
    {      
        MD5 md5bytes;
        char szResult[33];
        CMD5Hasher hasher;
        hasher.Calculate ( strSetting, strSetting.length(), md5bytes );
        hasher.ConvertToHex ( md5bytes, szResult );

        lua_pushstring ( luaVM, szResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::Sha256 ( lua_State* luaVM )
{
//  string sha256 ( string str )
    SString strSourceData;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strSourceData );

    if ( !argStream.HasErrors () )
    {
        SString strResult = GenerateSha256HexString ( strSourceData );
        lua_pushstring ( luaVM, strResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::TeaEncode ( lua_State* luaVM )
{
    SString str;
    SString key;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( str );
    argStream.ReadString ( key );

    if ( !argStream.HasErrors() )
    {
        SString result;
        SString humanReadableResult;
        SharedUtil::TeaEncode ( str, key, &result );
        humanReadableResult = Base64::encode ( result );
        lua_pushstring ( luaVM, humanReadableResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::TeaDecode ( lua_State* luaVM )
{
    SString str;
    SString key;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( str );
    argStream.ReadString ( key );

    if ( !argStream.HasErrors() )
    {
        SString result;
        Base64::decode ( str, result );
        SharedUtil::TeaDecode ( result, key, &str );
        lua_pushstring ( luaVM, str );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::Base64encode ( lua_State* luaVM )
{
    SString str;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( str );

    if ( !argStream.HasErrors() )
    {
        SString result = Base64::encode ( str );
        lua_pushstring ( luaVM, result );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::Base64decode ( lua_State* luaVM )
{
    SString str;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( str );

    if ( !argStream.HasErrors() )
    {
        SString result;
        Base64::decode ( str, result );
        lua_pushstring ( luaVM, result );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetNetworkUsageData ( lua_State* luaVM )
{
    SPacketStat m_PacketStats [ 2 ] [ 256 ];
    memcpy ( m_PacketStats, g_pNetServer->GetPacketStats (), sizeof ( m_PacketStats ) );

    lua_createtable ( luaVM, 0, 2 );

    lua_pushstring ( luaVM, "in" );
    lua_createtable ( luaVM, 0, 2 );
    {
        lua_pushstring ( luaVM, "bits" );
        lua_createtable ( luaVM, 255, 1 );
        for ( unsigned int i = 0; i < 256; ++i )
        {
            const SPacketStat& statIn = m_PacketStats [ CNetServer::STATS_INCOMING_TRAFFIC ] [ i ];
            lua_pushnumber ( luaVM, statIn.iTotalBytes * 8 );
            lua_rawseti ( luaVM, -2, i );
        }
        lua_rawset ( luaVM, -3 );

        lua_pushstring ( luaVM, "count" );
        lua_createtable ( luaVM, 255, 1 );
        for ( unsigned int i = 0; i < 256; ++i )
        {
            const SPacketStat& statIn = m_PacketStats [ CNetServer::STATS_INCOMING_TRAFFIC ] [ i ];
            lua_pushnumber ( luaVM, statIn.iCount );
            lua_rawseti ( luaVM, -2, i );
        }
        lua_rawset ( luaVM, -3 );
    }
    lua_rawset ( luaVM, -3 );

    lua_pushstring ( luaVM, "out" );
    lua_createtable ( luaVM, 0, 2 );
    {
        lua_pushstring ( luaVM, "bits" );
        lua_createtable ( luaVM, 255, 1 );
        for ( unsigned int i = 0; i < 256; ++i )
        {
            const SPacketStat& statOut = m_PacketStats [ CNetServer::STATS_OUTGOING_TRAFFIC ] [ i ];
            lua_pushnumber ( luaVM, statOut.iTotalBytes * 8 );
            lua_rawseti ( luaVM, -2, i );
        }
        lua_rawset ( luaVM, -3 );

        lua_pushstring ( luaVM, "count" );
        lua_createtable ( luaVM, 255, 1 );
        for ( unsigned int i = 0; i < 256; ++i )
        {
            const SPacketStat& statOut = m_PacketStats [ CNetServer::STATS_OUTGOING_TRAFFIC ] [ i ];
            lua_pushnumber ( luaVM, statOut.iCount );
            lua_rawseti ( luaVM, -2, i );
        }
        lua_rawset ( luaVM, -3 );
    }
    lua_rawset ( luaVM, -3 );

    return 1;
}


int CLuaFunctionDefinitions::GetNetworkStats ( lua_State* luaVM )
{
//  table getNetworkStats ( [element player] )
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer, NULL );

    if ( !argStream.HasErrors () )
    {
        NetServerPlayerID PlayerID = pPlayer ? pPlayer->GetSocket () : NetServerPlayerID ();
        NetStatistics stats;
        if ( g_pNetServer->GetNetworkStatistics ( &stats, PlayerID ) )
        {
            lua_createtable ( luaVM, 0, 11 );

            lua_pushstring ( luaVM, "bytesReceived" );
            lua_pushnumber ( luaVM, static_cast < double > ( stats.bytesReceived ) );
            lua_settable   ( luaVM, -3 );

            lua_pushstring ( luaVM, "bytesSent" );
            lua_pushnumber ( luaVM, static_cast < double > ( stats.bytesSent ) );
            lua_settable   ( luaVM, -3 );

            lua_pushstring ( luaVM, "packetsReceived" );
            lua_pushnumber ( luaVM, stats.packetsReceived );
            lua_settable   ( luaVM, -3 );

            lua_pushstring ( luaVM, "packetsSent" );
            lua_pushnumber ( luaVM, stats.packetsSent );
            lua_settable   ( luaVM, -3 );

            lua_pushstring ( luaVM, "packetlossTotal" );
            lua_pushnumber ( luaVM, stats.packetlossTotal );
            lua_settable   ( luaVM, -3 );

            lua_pushstring ( luaVM, "packetlossLastSecond" );
            lua_pushnumber ( luaVM, stats.packetlossLastSecond );
            lua_settable   ( luaVM, -3 );

            lua_pushstring ( luaVM, "messagesInSendBuffer" );
            lua_pushnumber ( luaVM, stats.messagesInSendBuffer );
            lua_settable   ( luaVM, -3 );

            lua_pushstring ( luaVM, "messagesInResendBuffer" );
            lua_pushnumber ( luaVM, stats.messagesInResendBuffer );
            lua_settable   ( luaVM, -3 );

            lua_pushstring ( luaVM, "isLimitedByCongestionControl" );
            lua_pushnumber ( luaVM, stats.isLimitedByCongestionControl ? 1 : 0 );
            lua_settable   ( luaVM, -3 );

            lua_pushstring ( luaVM, "isLimitedByOutgoingBandwidthLimit" );
            lua_pushnumber ( luaVM, stats.isLimitedByOutgoingBandwidthLimit ? 1 : 0 );
            lua_settable   ( luaVM, -3 );

            lua_pushstring ( luaVM, "encryptionStatus" );
            lua_pushnumber ( luaVM, stats.encryptionStatus );
            lua_settable   ( luaVM, -3 );

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVersion ( lua_State* luaVM )
{
    lua_createtable ( luaVM, 0, 8 );

    lua_pushstring ( luaVM, "number" );
    lua_pushnumber ( luaVM, CStaticFunctionDefinitions::GetVersion () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "mta" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionString () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "name" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionName () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "netcode" );
    lua_pushnumber ( luaVM, CStaticFunctionDefinitions::GetNetcodeVersion () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "os" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetOperatingSystemName () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "type" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionBuildType () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "tag" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionBuildTag () );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "sortable" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionSortable () );
    lua_settable   ( luaVM, -3 );

    return 1;
}

int CLuaFunctionDefinitions::GetModuleInfo ( lua_State* luaVM )
{
    if (lua_type( luaVM, 1 ) == LUA_TSTRING) {
        list < CLuaModule* > lua_LoadedModules = m_pLuaModuleManager->GetLoadedModules();
        list < CLuaModule* > ::iterator iter = lua_LoadedModules.begin ();
        SString strAttribute = lua_tostring( luaVM, 2 );
        SString strModuleName = lua_tostring( luaVM, 1 );
        for ( ; iter != lua_LoadedModules.end (); ++iter )
        {
            if ( stricmp ( strModuleName, (*iter)->_GetName().c_str() ) == 0 ) {
                lua_newtable ( luaVM );

                lua_pushstring ( luaVM, "name" );
                lua_pushstring ( luaVM, (*iter)->_GetFunctions().szModuleName );
                lua_settable ( luaVM, -3 );

                lua_pushstring ( luaVM, "author" );
                lua_pushstring ( luaVM, (*iter)->_GetFunctions().szAuthor );
                lua_settable ( luaVM, -3 );

                lua_pushstring ( luaVM, "version" );
                SString strVersion ( "%.2f", (*iter)->_GetFunctions().fVersion );
                lua_pushstring ( luaVM, strVersion );
                lua_settable ( luaVM, -3 );

                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    m_pScriptDebugging->LogBadType ( luaVM );
    return 1;
}

int CLuaFunctionDefinitions::GetModules ( lua_State* luaVM )
{
    lua_newtable ( luaVM );
    list < CLuaModule* > lua_LoadedModules = m_pLuaModuleManager->GetLoadedModules();
    list < CLuaModule* > ::iterator iter = lua_LoadedModules.begin ();
    unsigned int uiIndex = 1;
    for ( ; iter != lua_LoadedModules.end (); ++iter )
    {
        lua_pushnumber ( luaVM, uiIndex++ );
        lua_pushstring ( luaVM, (*iter)->_GetFunctions().szFileName );
        lua_settable ( luaVM, -3 );
    }
    return 1;
}

int CLuaFunctionDefinitions::GetPerformanceStats ( lua_State* luaVM )
{
    SString strCategory, strOptions, strFilter;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strCategory );
    argStream.ReadString ( strOptions, "" );
    argStream.ReadString ( strFilter, "" );

    if ( !argStream.HasErrors () )
    {
        CPerfStatResult Result;
        CPerfStatManager::GetSingleton ()->GetStats ( &Result, strCategory, strOptions, strFilter );

        lua_newtable ( luaVM );
        for ( int c = 0; c < Result.ColumnCount () ; c++ )
        {
            const SString& name = Result.ColumnName ( c );
            lua_pushnumber ( luaVM, c+1 );                      // row index number (starting at 1, not 0)
            lua_pushlstring ( luaVM, name.c_str (), name.length() );
            lua_settable ( luaVM, -3 );
        }

        lua_newtable ( luaVM );
        for ( int r = 0; r < Result.RowCount () ; r++ )
        {
            lua_newtable ( luaVM );                             // new table
            lua_pushnumber ( luaVM, r+1 );                      // row index number (starting at 1, not 0)
            lua_pushvalue ( luaVM, -2 );                        // value
            lua_settable ( luaVM, -4 );                         // refer to the top level table

            for ( int c = 0; c < Result.ColumnCount () ; c++ )
            {
                SString& cell = Result.Data ( c, r );
                lua_pushnumber ( luaVM, c+1 );
                lua_pushlstring ( luaVM, cell.c_str (), cell.length () );
                lua_settable ( luaVM, -3 );
            }
            lua_pop ( luaVM, 1 );                               // pop the inner table
        }
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::PregFind ( lua_State* luaVM )
{
//  bool pregFind ( string base, string pattern )
    SString strBase, strPattern;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strBase );
    argStream.ReadString ( strPattern );

    if ( !argStream.HasErrors () )
    {
        pcrecpp::RE pPattern ( strPattern );

        if ( pPattern.PartialMatch ( strBase ) )
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


int CLuaFunctionDefinitions::PregReplace ( lua_State* luaVM )
{
//  string pregReplace ( string base, string pattern, string replace )
    SString strBase, strPattern, strReplace;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strBase );
    argStream.ReadString ( strPattern );
    argStream.ReadString ( strReplace );

    if ( !argStream.HasErrors () )
    {
        pcrecpp::RE pPattern ( strPattern );

        string strNew = strBase;
        if ( pPattern.GlobalReplace ( strReplace, &strNew ) )
        {
            lua_pushstring ( luaVM, strNew.c_str () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::PregMatch ( lua_State* luaVM )
{
//  table pregMatch ( string base, string pattern )
    SString strBase, strPattern;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strBase );
    argStream.ReadString ( strPattern );

    if ( !argStream.HasErrors () )
    {
        lua_newtable ( luaVM );

        pcrecpp::RE pPattern ( strPattern );
        pcrecpp::StringPiece strInput ( strBase );

        string strGet; int i = 1;
        while ( pPattern.FindAndConsume ( &strInput, &strGet ) ) 
        {
            lua_pushnumber ( luaVM, i );
            lua_pushstring ( luaVM, strGet.c_str () );
            lua_settable ( luaVM, -3 );   
            i++;
        }
        
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}