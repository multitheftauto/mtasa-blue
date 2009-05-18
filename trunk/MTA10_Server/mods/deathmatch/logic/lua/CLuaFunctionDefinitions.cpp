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
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

#define MAX_STRING_LENGTH 2048

// Custom Lua stack argument->reference function
// This function should always be called last! (Since it pops the lua stack)
LUALIB_API int luaM_toref (lua_State *L, int i) {
	int ref = -1;

	// convert the function pointer to a string so we can use it as index
	char buf[10] = {0};
	char * index = itoa ( (int)lua_topointer ( L, i ), buf, 16 );

	// get the callback table we made in CLuaMain::InitVM (at location 1)
	lua_getref ( L, 1 );
		lua_getfield ( L, -1, index );
		ref = (int)lua_tonumber ( L, -1 );
		lua_pop ( L, 1 );
	lua_pop ( L, 1 );

	// if it wasn't added yet, add it to the callback table and the registry
	// else, get the reference from the table
	if ( !ref ) {
		// add a new reference (and get the id)
		lua_settop ( L, i );
		ref = lua_ref ( L, 1 );

		// and add it to the callback table
		lua_getref ( L, 1 );
			lua_pushstring ( L, index );
			lua_pushnumber ( L, ref );
			lua_settable ( L, -3 );
		lua_pop ( L, 1 );
	}
	return ref;
}


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
#define VERIFY_FUNCTION(func) (func!=LUA_REFNIL)


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
// bool callRemote ( hostname, resourceName, functionName, callback, [args...] )
int CLuaFunctionDefinitions::CallRemote ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && 
            lua_type ( luaVM, 2 ) == LUA_TSTRING && 
            lua_type ( luaVM, 3 ) == LUA_TSTRING && 
            lua_type ( luaVM, 4 ) == LUA_TFUNCTION )
        {
            char* szHost = (char *) lua_tostring ( luaVM, 1 );
            char* szResourceName = (char *) lua_tostring ( luaVM, 2 );
            char* szFunctionName = (char *) lua_tostring ( luaVM, 3 );
            
            CLuaArguments args;
            args.ReadArguments ( luaVM, 5 );
            int iLuaFunction = luaM_toref ( luaVM, 4 ); // you have to read this last or ReadArguments (above) fails

			g_pGame->GetRemoteCalls()->Call ( szHost, szResourceName, szFunctionName, &args, luaMain, iLuaFunction );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && 
            lua_type ( luaVM, 2 ) == LUA_TFUNCTION )
        {
            char* szURL = (char *) lua_tostring ( luaVM, 1 );
            
            CLuaArguments args;
            args.ReadArguments ( luaVM, 3 );
            int iLuaFunction = luaM_toref ( luaVM, 2 ); // you have to read this last or ReadArguments (above) fails

			g_pGame->GetRemoteCalls()->Call ( szURL, &args, luaMain, iLuaFunction );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "callRemote" );
    }

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
    // Check types
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        // Grab the password string
        const char* szPassword = lua_tostring ( luaVM, 1 );

        // Is it valid?
        unsigned int uiUnsupportedIndex;
        if ( szPassword && g_pGame->GetConfig ()->IsValidPassword ( szPassword, uiUnsupportedIndex ) )
        {
            // Set it
            g_pGame->GetConfig ()->SetPassword ( szPassword );
            g_pGame->GetConfig ()->Save ();
            CLogger::LogPrintf ( "Server password set to '%s'\n", szPassword );

            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "setServerPassword; password must be shorter than 32 chars and just contain visible characters" );
    }
    else if ( lua_type ( luaVM, 1 ) == LUA_TNIL )
    {
        // Remove the password
        g_pGame->GetConfig ()->SetPassword ( "" );
        g_pGame->GetConfig ()->Save ();
        CLogger::LogPrintf ( "Server password cleared\n" );

        // Success
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setServerPassword" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::shutdown ( lua_State* luaVM )
{
    // Get the VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Get the resourc
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            // Grab the reason
            char cszReason [] = "No reason specified";
            const char* szReason = cszReason;
            if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
            {
                szReason = lua_tostring ( luaVM, 1 );
            }

            // Log it
            CLogger::LogPrintf ( "Server shutdown as requested by resource %s (%s)\n", pResource->GetName ().c_str (), szReason );

            // Shut it down
            g_pGame->SetIsFinished ( true );

            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }

    // Fail
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::AddEvent ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Verify the arguments
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
        {
            // Grab the arguments
            const char* szName = lua_tostring ( luaVM, 1 );

            // Remote trigger?
            bool bAllowRemoteTrigger = false;
            if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            {
                bAllowRemoteTrigger = lua_toboolean ( luaVM, 2 ) ? true:false;
            }

            // Do it
            if ( CStaticFunctionDefinitions::AddEvent ( pLuaMain, szName, "", bAllowRemoteTrigger ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "addEvent" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::AddEventHandler ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        CResourceFile * file = pLuaMain->GetResourceFile();
        if ( file && file->GetType() == CResourceFile::RESOURCE_FILE_TYPE_HTML )
        {
            m_pScriptDebugging->LogError ( luaVM, "You cannot have event handlers in HTML scripts" );
        }
        else
        {
            // Verify the arguments
            if ( lua_type ( luaVM, 1 ) == LUA_TSTRING &&
                lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA &&
                lua_type ( luaVM, 3 ) == LUA_TFUNCTION )
            {
                // Grab the propagated bool
                bool bPropagated = true;
                if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
                    bPropagated = ( lua_toboolean ( luaVM, 4 ) ) ? true:false;

                // Grab the arguments
                const char* szName = lua_tostring ( luaVM, 1 );
                CElement* pElement = lua_toelement ( luaVM, 2 );
				int iLuaFunction = luaM_toref ( luaVM, 3 );

                // Verify the element
                if ( !pElement )
                {
                    m_pScriptDebugging->LogBadPointer ( luaVM, "addEventHandler", "element", 2 );
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }

				if ( !VERIFY_FUNCTION ( iLuaFunction ) )
					m_pScriptDebugging->LogBadPointer ( luaVM, "addEventHandler", "function", 3 );
				else {
                    // check if the handle is in use
                    if ( pElement->GetEventManager()->HandleExists ( pLuaMain, szName, iLuaFunction ) )
                    {
                        m_pScriptDebugging->LogCustom ( luaVM, 255, 0, 0, "addEventHandler: This handle already exists" );
                        lua_pushboolean ( luaVM, false );
                        return 1;
                    }
                    // Do it
                    if ( CStaticFunctionDefinitions::AddEventHandler ( pLuaMain, szName, pElement, iLuaFunction, bPropagated ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "addEventHandler" );
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemoveEventHandler ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Verify the arguments
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING &&
             lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA &&
             lua_type ( luaVM, 3 ) )
        {
            // Grab the arguments
            const char* szName = lua_tostring ( luaVM, 1 );
            CElement* pElement = lua_toelement ( luaVM, 2 );
			int iLuaFunction = luaM_toref ( luaVM, 3 );

            // Verify the element
            if ( !pElement )
				m_pScriptDebugging->LogBadPointer ( luaVM, "removeEventHandler", "element", 2 );
			else if ( !VERIFY_FUNCTION ( iLuaFunction ) )
				m_pScriptDebugging->LogBadPointer ( luaVM, "removeEventHandler", "function", 3 );
			else {
                // Do it
                if ( CStaticFunctionDefinitions::RemoveEventHandler ( pLuaMain, szName, pElement, iLuaFunction ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "removeEventHandler" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::TriggerEvent ( lua_State* luaVM )
{
    // Verify the arguments
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the name and the element
        const char* szName = lua_tostring ( luaVM, 1 );
        CElement* pElement = lua_toelement ( luaVM, 2 );

        // Read out the additional arguments to pass to the event
        CLuaArguments Arguments;
        Arguments.ReadArguments ( luaVM, 3 );

        // Verify the element
        if ( pElement )
        {
            // Trigger it
            bool bWasCancelled;
            if ( CStaticFunctionDefinitions::TriggerEvent ( szName, pElement, Arguments, bWasCancelled ) )
            {
                lua_pushboolean ( luaVM, !bWasCancelled );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "triggerEvent", "element", 2 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "triggerEvent" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::TriggerClientEvent ( lua_State* luaVM )
{
    CElement* pElement = CStaticFunctionDefinitions::GetRootElement ();

    int iStartingArgument = 1;
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        pElement = lua_toelement ( luaVM, 1 );
        iStartingArgument++;
    }

    if ( lua_type ( luaVM, iStartingArgument ) == LUA_TSTRING &&
         lua_type ( luaVM, iStartingArgument + 1 ) == LUA_TLIGHTUSERDATA )
    {
        const char* szName = lua_tostring ( luaVM, iStartingArgument );
        CElement* pCallWithElement = lua_toelement ( luaVM, iStartingArgument + 1 );

        CLuaArguments Arguments;
        Arguments.ReadArguments ( luaVM, iStartingArgument + 2 );

        if ( pElement )
        {
            if ( pCallWithElement )
            {
                if ( CStaticFunctionDefinitions::TriggerClientEvent ( pElement, szName, pCallWithElement, Arguments ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "triggerClientEvent", "element", iStartingArgument + 1 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "triggerClientEvent", "name", iStartingArgument );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "triggerClientEvent" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        char szNick[256] = { '\0' };

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::GetPlayerName ( pElement, szNick ) )
            {
                lua_pushstring ( luaVM, szNick );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerName", "client", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerIP ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        char szIP [256] = { '\0' };

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::GetPlayerIP ( pElement, szIP ) )
            {
                lua_pushstring ( luaVM, szIP );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerIP", "client", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerIP" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerAccount ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CAccount* pAccount = CStaticFunctionDefinitions::GetPlayerAccount ( pElement );
            if ( pAccount )
            {
                lua_pushaccount ( luaVM, pAccount );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerAccount", "client", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerAccount" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        const char* szName = lua_tostring ( luaVM, 2 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPlayerName ( pElement, szName ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerName", "client", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreatePed ( lua_State* luaVM )
{
    // Verify the parameters
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the vehicle id parameter
        unsigned short usModel = static_cast < unsigned short > ( atoi ( lua_tostring ( luaVM, 1 ) ) );

        // Grab the position parameters
        CVector vecPosition;
        vecPosition.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 2 ) ) );
        vecPosition.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) );
        vecPosition.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 4 ) ) );

        // Grab the rotation parameter
        float fRotation = 0.0f;     
        int iArgument5 = lua_type ( luaVM, 5 );
        if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
        {
            fRotation = static_cast < float > ( atof ( lua_tostring ( luaVM, 5 ) ) );
        }

        bool bSynced = true;
        if ( lua_type ( luaVM, 6 ) == LUA_TBOOLEAN )
        {
            bSynced = ( lua_toboolean ( luaVM, 6 ) ) ? true : false;
        }

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
        m_pScriptDebugging->LogBadType ( luaVM, "createPed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerCount ( lua_State* luaVM )
{
    unsigned int uiPlayerCount = CStaticFunctionDefinitions::GetPlayerCount ();

    lua_pushnumber ( luaVM, uiPlayerCount );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerWeapon ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            unsigned char ucWeapon;
            if ( CStaticFunctionDefinitions::GetPlayerWeapon ( pPlayer, ucWeapon ) )
            {
                lua_pushnumber ( luaVM, ucWeapon );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerCurrentWeapon", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerCurrentWeapon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedWeaponSlot ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            unsigned char ucSlot;
            if ( CStaticFunctionDefinitions::GetPedWeaponSlot ( pPed, ucSlot ) )
            {
                lua_pushnumber ( luaVM, ucSlot );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedWeaponSlot", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedWeaponSlot" );

    lua_pushnumber ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedDoingGangDriveby ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bDriveby;
            if ( CStaticFunctionDefinitions::IsPedDoingGangDriveby ( pPed, bDriveby ) )
            {
                lua_pushnumber ( luaVM, bDriveby );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedDoingGangDriveby", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedDoingGangDriveby" );

    lua_pushnumber ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedOnFire ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bIsOnFire;
            if ( CStaticFunctionDefinitions::IsPedOnFire ( pPed, bIsOnFire ) )
            {
                lua_pushboolean ( luaVM, bIsOnFire );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedOnFire", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedOnFire" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedHeadless ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bIsHeadless;
            if ( CStaticFunctionDefinitions::IsPedHeadless ( pPed, bIsHeadless ) )
            {
                lua_pushboolean ( luaVM, bIsHeadless );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedHeadless", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedHeadless" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedAnimation ( lua_State* luaVM )
{
    // Check types
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element
        CElement * pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
		{
            const char * szBlockName = NULL;
            const char * szAnimName = NULL;
            int iTime = -1;
            bool bLoop = true;
            bool bUpdatePosition = true;
            bool bInteruptable = true;
            if ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) szBlockName = lua_tostring ( luaVM, 2 );
            if ( lua_type ( luaVM, 3 ) == LUA_TSTRING ) szAnimName = lua_tostring ( luaVM, 3 );
            int iArgument4 = lua_type ( luaVM, 4 );
            if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
                iTime = static_cast < int > ( lua_tonumber ( luaVM, 4 ) );
            if ( lua_type ( luaVM, 5 ) == LUA_TBOOLEAN )
                bLoop = ( lua_toboolean ( luaVM, 5 ) ) ? true:false;
            if ( lua_type ( luaVM, 6 ) == LUA_TBOOLEAN )
                bUpdatePosition = ( lua_toboolean ( luaVM, 6 ) ) ? true:false;
            if ( lua_type ( luaVM, 6 ) == LUA_TBOOLEAN )
                bInteruptable = ( lua_toboolean ( luaVM, 7 ) ) ? true:false;

            if ( CStaticFunctionDefinitions::SetPedAnimation ( pElement, szBlockName, szAnimName, iTime, bLoop, bUpdatePosition, bInteruptable ) )
            {
			    lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedAnimation", "element", 1 );
	}
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedAnimation" );

    // Failed
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::SetPedWeaponSlot ( lua_State* luaVM )
{
    // Check types
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element
        CElement * pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
		{
		    unsigned char ucSlot = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            
            if ( CStaticFunctionDefinitions::SetPedWeaponSlot ( pElement, ucSlot ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedWeaponSlot", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedWeaponSlot" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedOnFire ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bIsOnFire = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPedOnFire ( pElement, bIsOnFire ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedOnFire", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedOnFire" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedHeadless ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bIsHeadless = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPedHeadless ( pElement, bIsHeadless ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedHeadless", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedHeadless" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerAmmoInClip ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            unsigned short usAmmo;
            if ( CStaticFunctionDefinitions::GetPlayerAmmoInClip ( pPlayer, usAmmo ) )
            {
                lua_pushnumber ( luaVM, usAmmo );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerAmmoInClip", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerAmmoInClip" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerTotalAmmo ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            unsigned short usAmmo;
            if ( CStaticFunctionDefinitions::GetPlayerTotalAmmo ( pPlayer, usAmmo ) )
            {
                lua_pushnumber ( luaVM, usAmmo );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerTotalAmmo", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerTotalAmmo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerAmmo ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        unsigned char ucSlot = 0xFF;
        unsigned short usAmmo = 0;

        if ( lua_type ( luaVM, 2 ) == LUA_TSTRING || lua_type ( luaVM, 2 ) == LUA_TNUMBER )
        {
            usAmmo = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );

            int iArgument2 = lua_type ( luaVM, 3 );
            if ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER )
            ucSlot = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
            int iArgument4 = lua_type ( luaVM, 4 );
            unsigned short usAmmoInClip = NULL;
            if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
            {
                usAmmoInClip = static_cast < unsigned short > ( lua_tonumber ( luaVM, 4 ) );
            }

            if ( pPlayer )
            {
                if ( CStaticFunctionDefinitions::SetPlayerAmmo ( pPlayer, ucSlot, usAmmo, usAmmoInClip ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerAmmo", "player", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "setPlayerAmmo" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerAmmo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerFromNick ( lua_State* luaVM )
{
    // Valid player nick argument?
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        // Grab the nick
        const char* szNick = lua_tostring ( luaVM, 1 );

        // Grab the player with that nick
        CPlayer* pPlayer = CStaticFunctionDefinitions::GetPlayerFromNick ( szNick );
        if ( pPlayer )
        {
            // Return the player
            lua_pushelement ( luaVM, pPlayer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerFromNick" );

    // Doesn't exist
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedArmor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            float fArmor;
            if ( CStaticFunctionDefinitions::GetPedArmor ( pPed, fArmor ) )
            {
                lua_pushnumber ( luaVM, fArmor );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedArmor", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedArmor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerMoney ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        long lMoney;

        if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::GetPlayerMoney ( pPlayer, lMoney ) )
            {
                lua_pushnumber ( luaVM, lMoney );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerMoney", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerMoney" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedOccupiedVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            CVehicle* pVehicle = CStaticFunctionDefinitions::GetPedOccupiedVehicle ( pPed );
            if ( pVehicle )
            {
                lua_pushelement ( luaVM, pVehicle );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedOccupiedVehicle", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedOccupiedVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedOccupiedVehicleSeat ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        unsigned int uiSeat = 0;

        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::GetPedOccupiedVehicleSeat ( pPed, uiSeat ) )
            {
                lua_pushnumber ( luaVM, uiSeat );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedOccupiedVehicleSeat", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedOccupiedVehicleSeat" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerPing ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        unsigned int uiPing = 0;

        if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::GetPlayerPing ( pPlayer, uiPing ) )
            {
                lua_pushnumber ( luaVM, uiPing );         
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerPing", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerPing" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedRotation ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        float fRotation = 0.0f;

        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::GetPedRotation ( pPed, fRotation ) )
            {
                lua_pushnumber ( luaVM, fRotation );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedRotation", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedRotation" );

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
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the ped and verify him
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Return whether he's choking or not
            lua_pushboolean ( luaVM, pPed->IsChoking () );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedChoking", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedChoking" );
    
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::IsPedDead ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        bool bDead;

        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::IsPedDead ( pPed, bDead ) )
            {
                lua_pushboolean ( luaVM, bDead );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedDead", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedDead" );
    
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::IsPedDucked ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        bool bDucked;

        if ( pPed )
        {
            if ( CStaticFunctionDefinitions::IsPedDucked ( pPed, bDucked ) )
            {
                lua_pushboolean ( luaVM, bDucked );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedDucked", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedDucked" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::IsPlayerMuted ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        bool bMuted;

        if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::IsPlayerMuted ( pPlayer, bMuted ) )
            {
                lua_pushboolean ( luaVM, bMuted );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPlayerMuted", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPlayerMuted" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::IsPedInVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        bool bInVehicle;

        if ( pPed )
        {
            bInVehicle = CStaticFunctionDefinitions::GetPedOccupiedVehicle ( pPed ) != NULL;
            
            lua_pushboolean ( luaVM, bInVehicle );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedInVehicle", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedInVehicle" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::GetPedStat ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        unsigned short usStat = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );

        if ( pPed )
        {
			float fValue;
			if ( CStaticFunctionDefinitions::GetPedStat ( pPed, usStat, fValue ) )
            {
                lua_pushnumber ( luaVM, fValue );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedStat", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedStat" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedTarget ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
			CElement* pElement = CStaticFunctionDefinitions::GetPedTarget ( pPed );
			if ( pElement )
			{
				lua_pushelement ( luaVM, pElement );
				return 1;
			}
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTarget", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTarget" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerTeam ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            CTeam* pTeam = CStaticFunctionDefinitions::GetPlayerTeam ( pPlayer );

            if ( pTeam )
            {
                lua_pushelement ( luaVM, pTeam );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerTeam", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerTeam" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedClothes ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        if ( pPed )
        {
            char szTexture [ 128 ], szModel [ 128 ];
            if ( CStaticFunctionDefinitions::GetPedClothes ( pPed, ucType, szTexture, szModel ) )
            {
                lua_pushstring ( luaVM, szTexture );
                lua_pushstring ( luaVM, szModel );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedClothes", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedClothes" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::DoesPedHaveJetPack ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );

        if ( pPed )
        {
            bool bHasJetPack;
            if ( CStaticFunctionDefinitions::DoesPedHaveJetPack ( pPed, bHasJetPack ) )
            {
                lua_pushboolean ( luaVM, bHasJetPack );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "doesPedHaveJetPack", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "doesPedHaveJetPack" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedOnGround ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bOnGround;
            if ( CStaticFunctionDefinitions::IsPedOnGround ( pPed, bOnGround ) )
            {
                lua_pushboolean ( luaVM, bOnGround );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedOnGround", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedOnGround" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CanPlayerUseFunction ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        char* szFunction = const_cast < char* > ( lua_tostring ( luaVM, 2 ) );

        if ( pPlayer )
        {
            bool bCanUse;
            if ( CStaticFunctionDefinitions::CanPlayerUseFunction ( pPlayer, szFunction, bCanUse ) )
            {
                lua_pushboolean ( luaVM, bCanUse );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "canPlayerUseFunction", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "canPlayerUseFunction" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerWantedLevel ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );

    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            unsigned int uiWantedLevel;
            if ( CStaticFunctionDefinitions::GetPlayerWantedLevel ( pPlayer, uiWantedLevel ) )
            {
                lua_pushnumber ( luaVM, uiWantedLevel );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerWantedLevel", "element", 1 );  
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerWantedLevel" );

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
        for ( ; iter != m_pPlayerManager->IterEnd () ; iter++ )
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
        for ( ; iter != m_pPlayerManager->IterEnd () ; iter++ )
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


int CLuaFunctionDefinitions::IsPlayerScoreboardForced ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            bool bForced;
            if ( CStaticFunctionDefinitions::IsPlayerScoreboardForced ( pPlayer, bForced ) )
            {
                lua_pushboolean ( luaVM, bForced );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPlayerScoreboardForced", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPlayerScoreboardForced" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPlayerMapForced ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            bool bForced;
            if ( CStaticFunctionDefinitions::IsPlayerMapForced ( pPlayer, bForced ) )
            {
                lua_pushboolean ( luaVM, bForced );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPlayerMapForced", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPlayerMapForced" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerNametagText ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            char szBuffer [ 256 ];
            if ( CStaticFunctionDefinitions::GetPlayerNametagText ( pPlayer, szBuffer, 256 ) )
            {
                lua_pushstring ( luaVM, szBuffer );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerNametagText", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerNametagText" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerNametagColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerNametagColor", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerNametagColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPlayerNametagShowing ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            bool bShowing;
            if ( CStaticFunctionDefinitions::IsPlayerNametagShowing ( pPlayer, bShowing ) )
            {
                lua_pushboolean ( luaVM, bShowing );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPlayerNametagShowing", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPlayerNametagShowing" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedFightingStyle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            unsigned char ucStyle;
            if ( CStaticFunctionDefinitions::GetPedFightingStyle ( pPed, ucStyle ) )
            {
                lua_pushnumber ( luaVM, ucStyle );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedFightingStyle", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedFightingStyle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedMoveAnim ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            unsigned int iMoveAnim;
            if ( CStaticFunctionDefinitions::GetPedMoveAnim ( pPed, iMoveAnim ) )
            {
                lua_pushnumber ( luaVM, iMoveAnim );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedWalkingStyle", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedWalkingStyle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedGravity ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            float fGravity;
            if ( CStaticFunctionDefinitions::GetPedGravity ( pPed, fGravity ) )
            {
                lua_pushnumber ( luaVM, fGravity );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedGravity", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedGravity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerSerial ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            std::string strSerial = CStaticFunctionDefinitions::GetPlayerSerial ( pPlayer );
            if ( !strSerial.empty () )
            {
                lua_pushstring ( luaVM, strSerial.c_str () );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerSerial", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerSerial" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerUserName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            std::string strUser = CStaticFunctionDefinitions::GetPlayerUserName ( pPlayer );
            if ( !strUser.empty () )
            {
                lua_pushstring ( luaVM, strUser.c_str () );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerUserName", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerUserName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerCommunityID ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            std::string strID = CStaticFunctionDefinitions::GetPlayerCommunityID ( pPlayer );
            if ( !strID.c_str () )
            {
                lua_pushstring ( luaVM, strID.c_str () );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerCommunityID", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerCommunityID" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerBlurLevel ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            unsigned char ucLevel;
            if ( CStaticFunctionDefinitions::GetPlayerBlurLevel ( pPlayer, ucLevel ) )
            {
                lua_pushnumber ( luaVM, ucLevel );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerBlurLevel", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerBlurLevel" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedContactElement ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) )
    {
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            CElement* pElement = CStaticFunctionDefinitions::GetPedContactElement ( pPed );
            if ( pElement )
            {
                lua_pushelement ( luaVM, pElement );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedContactElement", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedContactElement" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedArmor ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        float fArmor = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPedArmor ( pElement, fArmor ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedArmor", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedArmor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerMoney ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        long lMoney = static_cast < long > ( lua_tonumber ( luaVM, 2 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPlayerMoney ( pElement, lMoney ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerMoney", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerMoney" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GivePlayerMoney ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        long lMoney = static_cast < long > ( lua_tonumber ( luaVM, 2 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::GivePlayerMoney ( pElement, lMoney ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "givePlayerMoney", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "givePlayerMoney" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::TakePlayerMoney ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        long lMoney = static_cast < long > ( lua_tonumber ( luaVM, 2 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::TakePlayerMoney ( pElement, lMoney ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "takePlayerMoney", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "takePlayerMoney" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::KillPed ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CElement* pKiller = NULL;
            unsigned char ucKillerWeapon = 0xFF;
            unsigned char ucBodyPart = 0xFF;

            // Killer is specified?
            if ( lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
            {
                // Grab it and verify it
                pKiller = lua_toelement ( luaVM, 2 );
            }

            // Killer weapon specified?
            int iArgument3 = lua_type ( luaVM, 3 );
            if ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING )
            {
                ucKillerWeapon = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
            }

            // Body part specified?
            int iArgument4 = lua_type ( luaVM, 4 );
            if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
            {
                ucBodyPart = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
            }

            // Is this a stealth kill?
            bool bStealth = false;
            if ( lua_type ( luaVM, 5 ) == LUA_TBOOLEAN )
            {
                bStealth = ( lua_toboolean ( luaVM, 5 ) ) ? true:false;
            }

            // Do it
            if ( CStaticFunctionDefinitions::KillPed ( pElement, pKiller, ucKillerWeapon, ucBodyPart, bStealth ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "killPed", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "killPed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedRotation ( lua_State* luaVM )
{
    // Arguments are valid?
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        float fRotation = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );

        if ( pElement )
        {
            // Set the rotation
            if ( CStaticFunctionDefinitions::SetPedRotation ( pElement, fRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedRotation", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedRotation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SpawnPlayer ( lua_State* luaVM )
{
    // First argument is always a player
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the player and verify it
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {                        
			int iArgument2 = lua_type ( luaVM, 2 );
            int iArgument3 = lua_type ( luaVM, 3 );
            int iArgument4 = lua_type ( luaVM, 4 );
            if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
                 ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                 ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
            {
                // Grab the position
                CVector vecPosition = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                                                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                                                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ) );
                float fRotation = 0.0f;
                unsigned long ulModel = 0;
                unsigned char ucInterior = 0;
                unsigned short usDimension = 0;
                CTeam * pTeam = NULL;
                int iArgument5 = lua_type ( luaVM, 5 );
                if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
                {
                    fRotation = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );
                    
                    int iArgument6 = lua_type ( luaVM, 6 );
                    if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
                    {
                        ulModel = static_cast < unsigned long > ( lua_tonumber ( luaVM, 6 ) );   

                        int iArgument7 = lua_type ( luaVM, 7 );
                        if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
                        {
                            ucInterior = static_cast < unsigned char > ( lua_tonumber ( luaVM, 7 ) );

                            int iArgument8 = lua_type ( luaVM, 8 );
                            if ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING )
                            {
                                usDimension = static_cast < unsigned short > ( lua_tonumber ( luaVM, 8 ) );

                                if ( lua_type ( luaVM, 9 ) == LUA_TLIGHTUSERDATA )
                                {
                                    pTeam = lua_toteam ( luaVM, 9 );
                                }
                            }
                        }
                    }
                }

                if ( CStaticFunctionDefinitions::SpawnPlayer ( pPlayer, vecPosition, fRotation, ulModel, ucInterior, usDimension, pTeam ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "spawnPlayer" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "spawnPlayer", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "spawnPlayer" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedStat ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
	int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) &&
		 ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned short usStat = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
		float fValue = static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) );

        if ( pElement )
        {
			if ( CStaticFunctionDefinitions::SetPedStat ( pElement, usStat, fValue ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedStat", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedStat" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::AddPedClothes ( lua_State* luaVM )
{
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) &&
         ( lua_type ( luaVM, 3 ) == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        const char* szTexture = lua_tostring ( luaVM, 2 );
        const char* szModel = lua_tostring ( luaVM, 3 );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::AddPedClothes ( pElement, const_cast < char* > ( szTexture ), const_cast < char* > ( szModel ), ucType ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "addPedClothes", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "addPedClothes" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemovePedClothes ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        const char* szTexture = NULL;
        const char* szModel = NULL;
        
        if ( lua_type ( luaVM, 3 ) == LUA_TSTRING )
        {
            szTexture = lua_tostring ( luaVM, 3 );

            if ( lua_type ( luaVM, 4 ) == LUA_TSTRING )
                szModel = lua_tostring ( luaVM, 4 );
        }

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::RemovePedClothes ( pElement, ucType, const_cast < char* > ( szTexture ), const_cast < char* > ( szModel ) ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "removePedClothes", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "removePedClothes" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GivePedJetPack ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::GivePedJetPack ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "givePedJetPack", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "givePedJetPack" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemovePedJetPack ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::RemovePedJetPack ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "removePedJetPack", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "removePedJetPack" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ShowPlayerHudComponent ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING &&
         lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        char* szComponent = const_cast < char* > ( lua_tostring ( luaVM, 2 ) );
        bool bShow = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;
        if ( pElement )
        {
            struct SHudComponent { char szName [ 15 ]; };
            SHudComponent hudComponents [] = { { "ammo" }, { "weapon" }, { "health" },
                                               { "breath" }, { "armour" }, { "money" },
                                               { "vehicle_name" }, { "area_name" }, { "radar" },
                                               { "clock" }, { NULL } };

            if ( szComponent && szComponent [ 0 ] )
            {
                unsigned char ucComponent = 0xFF;
                for ( int i = 0 ; hudComponents [ i ].szName [ 0 ] != NULL ; i++ )
                {
                    if ( stricmp ( szComponent, hudComponents [ i ].szName ) == 0 )
                    {
                        ucComponent = static_cast < unsigned char > ( i );
                        break;
                    }
                }

                if ( ucComponent != 0xFF )
                {
                    if ( CStaticFunctionDefinitions::ShowPlayerHudComponent ( pElement, ucComponent, bShow ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "showPlayerHudComponent", "component", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "showPlayerHudComponent", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "showPlayerHudComponent" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerWantedLevel ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );

    if ( iArgument1 == LUA_TLIGHTUSERDATA && 
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned int uiWantedLevel = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPlayerWantedLevel ( pElement, uiWantedLevel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "Valid wanted levels are between 0 and 6 inclusive" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerWantedLevel", "element", 1 );        
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerWantedLevel" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ForcePlayerMap ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bVisible = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;        

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::ForcePlayerMap ( pElement, bVisible ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "forcePlayerMap", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "forcePlayerMap" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerNametagText ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        const char* szText = NULL;
        if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )        
            szText = lua_tostring ( luaVM, 2 );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPlayerNametagText ( pElement, szText ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerNametagText", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerNametagText" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerNametagColor ( lua_State* luaVM )
{
    // Check arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        // Grab the element
        CElement* pElement = lua_toelement ( luaVM, 1 );

        unsigned char ucR = 255;
        unsigned char ucG = 255;
        unsigned char ucB = 255;
        bool bRemoveOverride;

        // Read out the color override if any
        int iArgument2 = lua_type ( luaVM, 2 );
        int iArgument3 = lua_type ( luaVM, 3 );
        int iArgument4 = lua_type ( luaVM, 4 );
        if ( ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) &&
             ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER ) &&
             ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER ) )
        {
            ucR = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            ucG = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
            ucB = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
            bRemoveOverride = false;
        }

        // False passed instead? Remove the override.
        else if ( iArgument2 == LUA_TSTRING &&
                  !( lua_toboolean ( luaVM, 2 ) ? true:false ) )
        {
            bRemoveOverride = true;
        }

        // Bad arguments
        else
        {
            m_pScriptDebugging->LogBadType ( luaVM, "setPlayerNametagColor" );
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        // Valid element?
        if ( pElement )
        {
            // Change the color
            if ( CStaticFunctionDefinitions::SetPlayerNametagColor ( pElement, bRemoveOverride, ucR, ucG, ucB ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerNametagColor", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerNametagColor" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerNametagShowing ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bShowing = lua_toboolean ( luaVM, 2 ) ? true:false;

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPlayerNametagShowing ( pElement, bShowing ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerNametagShowing", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerNametagShowing" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedFightingStyle ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucStyle = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPedFightingStyle ( pElement, ucStyle ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedFightingStyle", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedFightingStyle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedMoveAnim ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        int iMoveAnim = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPedMoveAnim ( pElement, iMoveAnim ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedWalkingStyle", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedWalkingStyle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedGravity ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        float fGravity = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPedGravity ( pElement, fGravity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedGravity", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedGravity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerMuted ( lua_State* luaVM )
{
	if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
		 ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN ) )
	{
		CElement* pElement = lua_toelement ( luaVM, 1 );
		bool bMuted = lua_toboolean ( luaVM, 2 ) ?true:false;
		if ( pElement )
		{
			if ( CStaticFunctionDefinitions::SetPlayerMuted ( pElement, bMuted ) )
			{
				lua_pushboolean ( luaVM, true );
				return 1;
			}
		}
	}
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::SetPlayerBlurLevel ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucLevel = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPlayerBlurLevel ( pElement, ucLevel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerBlurLevel", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerBlurLevel" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RedirectPlayer ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );

        const char* szHost = lua_tostring ( luaVM, 2 );
        unsigned short usPort = static_cast < unsigned short > ( lua_tonumber ( luaVM, 3 ) );

        const char* szPassword = NULL;
        if ( lua_type ( luaVM, 4 ) == LUA_TSTRING )        
            szPassword = lua_tostring ( luaVM, 4 );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::RedirectPlayer ( pElement, szHost, usPort, szPassword ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "redirectPlayer", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "redirectPlayer" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedChoking ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bChoking = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPedChoking ( pElement, bChoking ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedChoking", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedChoking" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::WarpPedIntoVehicle ( lua_State* luaVM )
{
    // Grab the optional seat argument if it's there
    unsigned int uiSeat = 0;
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING )
    {
        uiSeat = static_cast < unsigned int > ( lua_tonumber ( luaVM, 3 ) );
    }

    // Verify ped and vehicle type
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the arguments
        CPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            CVehicle* pVehicle = lua_tovehicle ( luaVM, 2 );
            if ( pVehicle )
            {
                // Do it
                if ( CStaticFunctionDefinitions::WarpPedIntoVehicle ( pPed, pVehicle, uiSeat ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "warpPedIntoVehicle", "vehicle", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "warpPedIntoVehicle", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "warpPedIntoVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemovePedFromVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::RemovePedFromVehicle ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "removePedFromVehicle", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "removePedFromVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedDoingGangDriveby ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bDoingGangDriveby = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetPedDoingGangDriveby ( pElement, bDoingGangDriveby ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedDoingGangDriveby", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedDoingGangDriveby" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GiveWeapon ( lua_State* luaVM )
{
    // Verify the arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNONE ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucWeaponID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        unsigned short usAmmo = 30;
        if ( iArgument3 != LUA_TNONE )
            usAmmo = static_cast < unsigned short > ( lua_tonumber ( luaVM, 3 ) );

        bool bSetAsCurrent = false;
        if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
            bSetAsCurrent = ( lua_toboolean ( luaVM, 4 ) ) ? true:false;

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::GiveWeapon ( pElement, ucWeaponID, usAmmo, bSetAsCurrent ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "giveWeapon", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "giveWeapon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::TakeWeapon ( lua_State* luaVM )
{
    // Verify the arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Grab the arguments
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucWeaponID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        if ( pElement )
        {
            // Do it
            if ( CStaticFunctionDefinitions::TakeWeapon ( pElement, ucWeaponID ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "takeWeapon", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "takeWeapon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::TakeAllWeapons ( lua_State* luaVM )
{
    // Verify the arguments
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the arguments
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            // Do it
            if ( CStaticFunctionDefinitions::TakeAllWeapons ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "takeAllWeapons", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "takeAllWeapons" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GiveWeaponAmmo ( lua_State* luaVM )
{
    // Verify the arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucWeaponID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        unsigned short usAmmo = static_cast < unsigned short > ( lua_tonumber ( luaVM, 3 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::GiveWeaponAmmo ( pElement, ucWeaponID, usAmmo ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "giveWeaponAmmo", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "giveWeaponAmmo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::TakeWeaponAmmo ( lua_State* luaVM )
{
    // Verify the arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucWeaponID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        unsigned short usAmmo = static_cast < unsigned short > ( lua_tonumber ( luaVM, 3 ) );

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::TakeWeaponAmmo ( pElement, ucWeaponID, usAmmo ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "takeWeaponAmmo", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "takeWeaponAmmo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetWeaponAmmo ( lua_State* luaVM )
{
    // Verify the arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucWeaponID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        unsigned short usAmmo = static_cast < unsigned short > ( lua_tonumber ( luaVM, 3 ) );
        unsigned short usAmmoInClip = NULL;
        if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
        {
            usAmmoInClip = static_cast < unsigned short > ( lua_tonumber ( luaVM, 4 ) );
        }
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetWeaponAmmo ( pElement, ucWeaponID, usAmmo, usAmmoInClip ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setWeaponAmmo", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWeaponAmmo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetSlotFromWeapon ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER || lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        unsigned char ucWeaponID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        char cSlot = CWeaponNames::GetSlotFromWeapon ( ucWeaponID );
        if ( cSlot >= 0 )
            lua_pushnumber ( luaVM, cSlot );
        else
            lua_pushboolean ( luaVM, false );
        //lua_pushnumber ( luaVM, CWeaponNames::GetSlotFromWeapon ( ucWeaponID ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getSlotFromWeapon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateVehicle ( lua_State* luaVM )
{
    // Verify the parameters
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the vehicle id parameter
        unsigned short usModel = static_cast < unsigned short > ( atoi ( lua_tostring ( luaVM, 1 ) ) );

        // Grab the position parameters
        CVector vecPosition;
        vecPosition.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 2 ) ) );
        vecPosition.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) );
        vecPosition.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 4 ) ) );

        // Grab the rotation parameters
        CVector vecRotation;
        const char* szRegPlate = NULL;
        int iArgument5 = lua_type ( luaVM, 5 );
        if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
        {
            vecRotation.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 5 ) ) );

            int iArgument6 = lua_type ( luaVM, 6 );
            if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
            {
                vecRotation.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 6 ) ) );

                int iArgument7 = lua_type ( luaVM, 7 );
                if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
                {
                    vecRotation.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 7 ) ) );

                    if ( lua_type ( luaVM, 8 ) == LUA_TSTRING )
                    {
                        szRegPlate = lua_tostring ( luaVM, 8 );
                    }
                }
            }
        }

        bool bDirection = false;
        if ( lua_type ( luaVM, 9 ) == LUA_TBOOLEAN )
            bDirection = ( lua_toboolean ( luaVM, 9 ) ) ? true : false;

		CLuaMain * pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource * pResource = pLuaMain->GetResource();
			if ( pResource )
			{
				// Create the vehicle and return its handle
				CVehicle* pVehicle = CStaticFunctionDefinitions::CreateVehicle ( pResource, usModel, vecPosition, vecRotation, const_cast < char* > ( szRegPlate ), bDirection );
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
        m_pScriptDebugging->LogBadType ( luaVM, "createVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetVehicleType ( lua_State* luaVM )
{
	unsigned long ucModel = 0;
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
	{
		CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
		if ( pVehicle )
			ucModel = pVehicle -> GetModel ( );
	}
	else if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
	{
		ucModel = static_cast < unsigned long > (lua_tonumber ( luaVM, 1 ));
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "getVehicleType" );

	if ( ucModel >= 400 && ucModel < 610 )
		lua_pushstring ( luaVM, CVehicleNames::GetVehicleTypeName ( ucModel ) );
	else
		lua_pushboolean ( luaVM, false );

	return 1;
}

int CLuaFunctionDefinitions::GetVehicleColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucColor1;
            unsigned char ucColor2;
            unsigned char ucColor3;
            unsigned char ucColor4;

            if ( CStaticFunctionDefinitions::GetVehicleColor ( pVehicle, ucColor1, ucColor2, ucColor3, ucColor4 ) )
            {
                lua_pushnumber ( luaVM, ucColor1 );
                lua_pushnumber ( luaVM, ucColor2 );
                lua_pushnumber ( luaVM, ucColor3 );
                lua_pushnumber ( luaVM, ucColor4 );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleColor", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleModelFromName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szName = lua_tostring ( luaVM, 1 );
        unsigned short usModel;
        if ( CStaticFunctionDefinitions::GetVehicleModelFromName ( szName, usModel ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( usModel ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleModelFromName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleLandingGearDown ( lua_State* luaVM )
{
    // Verify the first argument
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        // Grab the vehicle. Is it valid?
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            // Does the vehicle have landing gears?
            if ( CVehicleManager::HasLandingGears ( pVehicle->GetModel () ) )
            {
                // Return whether it has landing gears or not
                bool bLandingGear = pVehicle->IsLandingGearDown ();
                lua_pushboolean ( luaVM, bLandingGear );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleLandingGear", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleLandingGearDown" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleLocked ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bLocked = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetVehicleLocked ( pElement, bLocked ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleLocked", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleLocked" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleDoorsUndamageable ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bDoorsUndamageable = ( lua_toboolean ( luaVM, 2 ) ) ? false:true;

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetVehicleDoorsUndamageable ( pElement, bDoorsUndamageable ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleDoorsUndamageable", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleDoorsUndamageable" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleMaxPassengers ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucMaxPassengers = 0;
            if ( CStaticFunctionDefinitions::GetVehicleMaxPassengers ( pVehicle, ucMaxPassengers ) )
            {
                lua_pushnumber ( luaVM, ucMaxPassengers );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleMaxPassengers", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleMaxPassengers" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            char szVehicleName [32];
            if ( CStaticFunctionDefinitions::GetVehicleName ( pVehicle, szVehicleName ) )
            {
                lua_pushstring ( luaVM, szVehicleName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleName", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleNameFromModel ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned short usModel = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );
        char szVehicleName [32];

        if ( CStaticFunctionDefinitions::GetVehicleNameFromModel ( usModel, szVehicleName ) )
        {
            lua_pushstring ( luaVM, szVehicleName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleNameFromModel" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleOccupant ( lua_State* luaVM )
{
    // Grab the seat argument if exists
    unsigned int uiSeat = 0;
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
    {
        uiSeat = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
    }

    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CPed* pPed = CStaticFunctionDefinitions::GetVehicleOccupant ( pVehicle, uiSeat );
            if ( pPed )
            {
                lua_pushelement ( luaVM, pPed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleOccupant", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleOccupant" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleController ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CPed* pPed = CStaticFunctionDefinitions::GetVehicleController ( pVehicle );
            if ( pPed )
            {
                lua_pushelement ( luaVM, pPed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleController", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleController" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleRotation ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleRotation", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleRotation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleSirensOn ( lua_State* luaVM )
{
    // Verify the argument type
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        // Grab the vehicle pointer. Is it valid?
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            // Does the vehicle have Sirens?
            if ( CVehicleManager::HasSirens ( pVehicle->GetModel () ) )
            {
                // Return whether it has its Sirens on or not
                bool bSirensOn = pVehicle->IsSirenActive ();
                lua_pushboolean ( luaVM, bSirensOn );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleSirensOn", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleSirensOn" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleTurnVelocity ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleTurnVelocity", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleTurnVelocity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleTurretPosition ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleTurretPosition", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleTurretPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsVehicleLocked ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bLocked;
            if ( CStaticFunctionDefinitions::IsVehicleLocked ( pVehicle, bLocked ) )
            {
                lua_pushboolean ( luaVM, bLocked );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleLocked", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleLocked" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::GetVehiclesOfType ( lua_State* luaVM )
{
    // Find our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
		int iArgument1 = lua_type ( luaVM, 1 );
		if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
        {
            // Grab the type/model
            unsigned int uiModel = static_cast < unsigned int > ( lua_tonumber ( luaVM, 1 ) );
            
            // Create a new table
            lua_newtable ( luaVM );

            // Add all the vehicles with a matching model
			m_pVehicleManager->GetVehiclesOfType ( uiModel, pLuaMain );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehiclesOfType", "model", 1 );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleUpgradeOnSlot ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
	int iArgument2 = lua_type ( luaVM, 2 );
	if ( iArgument1 == LUA_TLIGHTUSERDATA &&
	   ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {			
			unsigned char ucSlot = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
			unsigned short usUpgrade;
			if ( CStaticFunctionDefinitions::GetVehicleUpgradeOnSlot ( pVehicle, ucSlot, usUpgrade ) )
			{
				lua_pushnumber ( luaVM, usUpgrade );
				return 1;
			}
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleUpgradeOnSlot", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleUpgradeOnSlot" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleUpgradeSlotName ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
	if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
		unsigned long ulNumber = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );
        
		if ( ulNumber < 17 )
		{
			char szUpgradeName [32];
			if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( static_cast < unsigned char > ( ulNumber ), szUpgradeName ) )
			{
				lua_pushstring ( luaVM, szUpgradeName );
				return 1;
			}
		}
		else if ( ulNumber >= 1000 && ulNumber <= 1193 )
		{
			char szUpgradeName [32];
			if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( static_cast < unsigned short > ( ulNumber ), szUpgradeName ) )
			{
				lua_pushstring ( luaVM, szUpgradeName );
				return 1;
			}
		}
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleUpgradeSlotName", "slot/upgrade", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleUpgradeSlotName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleUpgrades ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
	if ( iArgument1 == LUA_TLIGHTUSERDATA )
	{
		CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
		// If the vehicle is valid
		if ( pVehicle )
		{			
			CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
			if ( pUpgrades )
			{
				// Create a new table
				lua_newtable ( luaVM );

				// Add all the upgrades to the table
				unsigned short* usSlotStates = pUpgrades->GetSlotStates ();

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
			m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleUpgrades", "vehicle", 1 );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "getVehicleUpgrades" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleCompatibleUpgrades ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        unsigned char ucSlot = 0xFF;
        int iArgument2 = lua_type ( luaVM, 2 );
        if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
            ucSlot = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        if ( pVehicle )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleCompatibleUpgrades", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleCompatibleUpgrades" );

    lua_pushboolean ( luaVM, false );
    return 1;
}
        

int CLuaFunctionDefinitions::GetVehicleDoorState ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
	int iArgument2 = lua_type ( luaVM, 2 );
	if ( iArgument1 == LUA_TLIGHTUSERDATA &&
	   ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
	{
		CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
		if ( pVehicle )
		{
			unsigned char ucDoor = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
			unsigned char ucState;
			if ( CStaticFunctionDefinitions::GetVehicleDoorState ( pVehicle, ucDoor, ucState ) )
			{
				lua_pushnumber ( luaVM, ucState );
				return 1;
			}
		}
		else
			m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleDoorState", "vehicle", 1 );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "getVehicleDoorState" );
	
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::GetVehicleWheelStates ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
	if ( iArgument1 == LUA_TLIGHTUSERDATA )
	{
		CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
		if ( pVehicle )
		{
			unsigned char ucFrontLeft, ucFrontRight, ucRearLeft, ucRearRight;
			if ( CStaticFunctionDefinitions::GetVehicleWheelStates ( pVehicle, ucFrontLeft, ucFrontRight, ucRearLeft, ucRearRight ) )
			{
				lua_pushnumber ( luaVM, ucFrontLeft );
                lua_pushnumber ( luaVM, ucFrontRight );
                lua_pushnumber ( luaVM, ucRearLeft );
                lua_pushnumber ( luaVM, ucRearRight );
				return 4;
			}
		}
		else
			m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleWheelStates", "vehicle", 1 );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "getVehicleWheelStates" );
	
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::GetVehicleLightState ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
	int iArgument2 = lua_type ( luaVM, 2 );
	if ( iArgument1 == LUA_TLIGHTUSERDATA &&
	   ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
	{
		CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
		if ( pVehicle )
		{
			unsigned char ucLight = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
			unsigned char ucState;
			if ( CStaticFunctionDefinitions::GetVehicleLightState ( pVehicle, ucLight, ucState ) )
			{
				lua_pushnumber ( luaVM, ucState );
				return 1;
			}
		}
		else
			m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleLightState", "vehicle", 1 );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "getVehicleLightState" );
	
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::GetVehiclePanelState ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
	int iArgument2 = lua_type ( luaVM, 2 );
	if ( iArgument1 == LUA_TLIGHTUSERDATA &&
	   ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
	{
		CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
		if ( pVehicle )
		{
			unsigned char ucPanel = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
			unsigned char ucState;
			if ( CStaticFunctionDefinitions::GetVehiclePanelState ( pVehicle, ucPanel, ucState ) )
			{
				lua_pushnumber ( luaVM, ucState );
				return 1;
			}
		}
		else
			m_pScriptDebugging->LogBadPointer ( luaVM, "getVehiclePanelState", "vehicle", 1 );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "getVehiclePanelState" );
	
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::GetVehicleOverrideLights ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucLights;
            if ( CStaticFunctionDefinitions::GetVehicleOverrideLights ( pVehicle, ucLights ) )
            {
                lua_pushnumber ( luaVM, ucLights );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleOverrideLights", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleOverrideLights" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleTowedByVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CVehicle* pTowedVehicle = pVehicle->GetTowedVehicle ();
            if ( pTowedVehicle )
            {
                lua_pushelement ( luaVM, pTowedVehicle );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleTowedByVehicle", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleTowedByVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleTowingVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CVehicle* pTowedByVehicle = pVehicle->GetTowedByVehicle ();
            if ( pTowedByVehicle )
            {
                lua_pushelement ( luaVM, pTowedByVehicle );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleTowingVehicle", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleTowingVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehiclePaintjob ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucPaintjob;
            if ( CStaticFunctionDefinitions::GetVehiclePaintjob ( pVehicle, ucPaintjob ) )
            {
                lua_pushnumber ( luaVM, ucPaintjob );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehiclePaintjob", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehiclePaintjob" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehiclePlateText ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            char szPlateText [ 9 ] = { 0 }; // 8 chars + \0
            //char* szPlateText = NULL;
            if ( CStaticFunctionDefinitions::GetVehiclePlateText ( pVehicle, szPlateText ) )
            {
                lua_pushstring ( luaVM, szPlateText );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehiclePlateText", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehiclePlateText" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsVehicleFuelTankExplodable ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bExplodable;
            if ( CStaticFunctionDefinitions::IsVehicleFuelTankExplodable ( pVehicle, bExplodable ) )
            {
                lua_pushboolean ( luaVM, bExplodable );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleFuelTankExplodable", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleFuelTankExplodable" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsVehicleFrozen ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bFrozen = false;
            if ( CStaticFunctionDefinitions::IsVehicleFrozen ( pVehicle, bFrozen ) )
            {
                lua_pushboolean ( luaVM, bFrozen );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleFrozen", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleFrozen" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsVehicleOnGround ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bOnGround;
            if ( CStaticFunctionDefinitions::IsVehicleOnGround ( pVehicle, bOnGround ) )
            {
                lua_pushboolean ( luaVM, bOnGround );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleOnGround", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleOnGround" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleEngineState ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bState;
            if ( CStaticFunctionDefinitions::GetVehicleEngineState ( pVehicle, bState ) )
            {
                lua_pushboolean ( luaVM, bState );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleEngineState", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleEngineState" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsTrainDerailed ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bDerailed;
            if ( CStaticFunctionDefinitions::IsTrainDerailed ( pVehicle, bDerailed ) )
            {
                lua_pushboolean ( luaVM, bDerailed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isTrainDerailed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isTrainDerailed" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsTrainDerailable ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bDerailable;
            if ( CStaticFunctionDefinitions::IsTrainDerailable ( pVehicle, bDerailable ) )
            {
                lua_pushboolean ( luaVM, bDerailable );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isTrainDerailable", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isTrainDerailable" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTrainDirection ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bDirection;
            if ( CStaticFunctionDefinitions::GetTrainDirection ( pVehicle, bDirection ) )
            {
                lua_pushboolean ( luaVM, bDirection );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getTrainDirection", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getTrainDirection" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTrainSpeed ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            float fSpeed;
            if ( CStaticFunctionDefinitions::GetTrainSpeed ( pVehicle, fSpeed ) )
            {
                lua_pushnumber ( luaVM, fSpeed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getTrainSpeed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getTrainSpeed" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::FixVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::FixVehicle ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fixVehicle", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fixVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::BlowVehicle ( lua_State* luaVM )
{
    // Read out whether to explode or not
    bool bExplode = true;
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument2 == LUA_TBOOLEAN )
    {
        bExplode = lua_toboolean ( luaVM, 2 ) ? true:false;
    }
    else if ( iArgument2 != LUA_TNONE )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "blowVehicle" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }

    // Verify the element pointer argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::BlowVehicle ( pElement, bExplode ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blowVehicle", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "blowVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::IsVehicleBlown ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle ) {
            if ( CStaticFunctionDefinitions::IsVehicleBlown(pVehicle) ) {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        } 
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleBlown", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleBlown" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetVehicleRotation ( lua_State* luaVM )
{
    // Verify the arguments
    int iArgumentType2 = lua_type ( luaVM, 2 );
    int iArgumentType3 = lua_type ( luaVM, 3 );
    int iArgumentType4 = lua_type ( luaVM, 4 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgumentType2 == LUA_TNUMBER || iArgumentType2 == LUA_TSTRING ) &&
         ( iArgumentType3 == LUA_TNUMBER || iArgumentType3 == LUA_TSTRING ) &&
         ( iArgumentType4 == LUA_TNUMBER || iArgumentType4 == LUA_TSTRING ) )
    {
        // Grab the arguments
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CVector vecRotation;
            vecRotation.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            vecRotation.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
            vecRotation.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
        
            if ( CStaticFunctionDefinitions::SetVehicleRotation ( pElement, vecRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleRotation", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleRotation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleTurnVelocity ( lua_State* luaVM )
{
    int iArgumentType1 = lua_type ( luaVM, 1 );
    int iArgumentType2 = lua_type ( luaVM, 2 );
    int iArgumentType3 = lua_type ( luaVM, 3 );
    int iArgumentType4 = lua_type ( luaVM, 4 );
    if ( ( iArgumentType1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgumentType2 == LUA_TNUMBER || iArgumentType2 == LUA_TSTRING ) &&
         ( iArgumentType3 == LUA_TNUMBER || iArgumentType3 == LUA_TSTRING ) &&
         ( iArgumentType4 == LUA_TNUMBER || iArgumentType4 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CVector vecTurnVelocity;
            vecTurnVelocity.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            vecTurnVelocity.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
            vecTurnVelocity.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

            if ( CStaticFunctionDefinitions::SetVehicleTurnVelocity ( pElement, vecTurnVelocity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleTurnVelocity", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleTurnVelocity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleColor ( lua_State* luaVM )
{
    int iArgumentType2 = lua_type ( luaVM, 2 );
    int iArgumentType3 = lua_type ( luaVM, 3 );
    int iArgumentType4 = lua_type ( luaVM, 4 );
    int iArgumentType5 = lua_type ( luaVM, 5 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgumentType2 == LUA_TNUMBER || iArgumentType2 == LUA_TSTRING ) &&
         ( iArgumentType3 == LUA_TNUMBER || iArgumentType3 == LUA_TSTRING ) &&
         ( iArgumentType4 == LUA_TNUMBER || iArgumentType4 == LUA_TSTRING ) &&
         ( iArgumentType5 == LUA_TNUMBER || iArgumentType5 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            double dColor1 = lua_tonumber ( luaVM, 2 );
            double dColor2 = lua_tonumber ( luaVM, 3 );
            double dColor3 = lua_tonumber ( luaVM, 4 );
            double dColor4 = lua_tonumber ( luaVM, 5 );

            if ( dColor1 >= 0 && dColor1 <= 255 &&
                 dColor2 >= 0 && dColor2 <= 255 &&
                 dColor3 >= 0 && dColor3 <= 255 &&
                 dColor4 >= 0 && dColor4 <= 255 )
            {
                if ( CStaticFunctionDefinitions::SetVehicleColor ( pElement, static_cast < unsigned char > ( dColor1 ), static_cast < unsigned char > ( dColor2 ), static_cast < unsigned char > ( dColor3 ), static_cast < unsigned char > ( dColor4 ) ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleColor", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleLandingGearDown ( lua_State* luaVM )
{
    // Verify the two arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA && iArgument2 == LUA_TBOOLEAN )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bLandingGearDown = lua_toboolean ( luaVM, 2 ) ? true:false;
        if ( pElement )
        {
            CVehicle* pVehicle = static_cast < CVehicle* > ( pElement );
            if ( CVehicleManager::HasLandingGears ( pVehicle->GetModel () ) )
            {
                // Do it
                if ( CStaticFunctionDefinitions::SetVehicleLandingGearDown ( pElement, bLandingGearDown ) )
                    lua_pushboolean ( luaVM, true );
                else
                    lua_pushboolean ( luaVM, false );
            }
            else
                lua_pushboolean ( luaVM, false );

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleLandingGearDown", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleLandingGearDown" );

    lua_pushboolean ( luaVM, false );
    return 1;
}
int CLuaFunctionDefinitions::IsVehicleTaxiLightOn ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            lua_pushboolean ( luaVM, pVehicle->IsTaxiLightOn() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleTaxiLightOn", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleTaxiLightOn" );

    lua_pushboolean ( luaVM, false );
    return 1;
}
int CLuaFunctionDefinitions::SetVehicleTaxiLightOn ( lua_State* luaVM )
{
    // Verify the two arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA && iArgument2 == LUA_TBOOLEAN )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bTaxiLightOn = lua_toboolean ( luaVM, 2 ) ? true:false;
        if ( pElement )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehicleTaxiLightOn ( pElement, bTaxiLightOn ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleTaxiLightOn", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleTaxiLightOn" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetVehicleSirensOn ( lua_State* luaVM )
{
    // Verify the two arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA && iArgument2 == LUA_TBOOLEAN )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bSirensOn = lua_toboolean ( luaVM, 2 ) ? true:false;
        if ( pElement )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehicleSirensOn ( pElement, bSirensOn ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleSirensOn", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleSirensOn" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::AddVehicleUpgrade ( lua_State* luaVM )
{
	// Verify the two arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA && 
	   ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
		
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );

        unsigned short usUpgrade = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
        if ( pElement )
        {
            if ( iArgument2 == LUA_TSTRING )
            {
                const char* szUpgrade = lua_tostring ( luaVM, 2 );
                if ( strcmp ( szUpgrade, "all" ) == 0 )
                {
                    if ( CStaticFunctionDefinitions::AddAllVehicleUpgrades ( pElement ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
            }

			if ( CStaticFunctionDefinitions::AddVehicleUpgrade ( pElement, usUpgrade ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "addVehicleUpgrade", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "addVehicleUpgrade" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemoveVehicleUpgrade ( lua_State* luaVM )
{
	// Verify the two arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA && 
	   ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
		
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned short usUpgrade = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
        if ( pElement )
        {
            // Do it
			if ( CStaticFunctionDefinitions::RemoveVehicleUpgrade ( pElement, usUpgrade ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "removeVehicleUpgrade", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "removeVehicleUpgrade" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleDoorState ( lua_State* luaVM )
{
	// Verify the three arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
	int iArgument3 = lua_type ( luaVM, 3 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
	   ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
	   ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucDoor = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
		unsigned char ucState = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        if ( pElement )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehicleDoorState ( pElement, ucDoor, ucState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleDoorState", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleDoorState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleWheelStates ( lua_State* luaVM )
{
	// Verify the three arguments
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        int iFrontLeft = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
        int iFrontRight = -1, iRearLeft = -1, iRearRight = -1;

        int iArgument3 = lua_type ( luaVM, 3 ), iArgument4 = lua_type ( luaVM, 4 ),
            iArgument5 = lua_type ( luaVM, 5 );

        if ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER )
            iFrontRight = static_cast < int > ( lua_tonumber ( luaVM, 3 ) );

        if ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER )
            iRearLeft = static_cast < int > ( lua_tonumber ( luaVM, 4 ) );

        if ( iArgument5 == LUA_TSTRING || iArgument5 == LUA_TNUMBER )
            iRearRight = static_cast < int > ( lua_tonumber ( luaVM, 5 ) );

        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetVehicleWheelStates ( pElement, iFrontLeft, iFrontRight, iRearLeft, iRearRight ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleWheelStates", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleWheelStates" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleLightState ( lua_State* luaVM )
{
	// Verify the three arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
	int iArgument3 = lua_type ( luaVM, 3 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
	   ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
	   ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucLight = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
		unsigned char ucState = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        if ( pElement )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehicleLightState ( pElement, ucLight, ucState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleLightState", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleLightState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehiclePanelState ( lua_State* luaVM )
{
	// Verify the three arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
	int iArgument3 = lua_type ( luaVM, 3 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
	   ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
	   ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucPanel = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
		unsigned char ucState = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        if ( pElement )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehiclePanelState ( pElement, ucPanel, ucState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehiclePanelState", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehiclePanelState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ToggleVehicleRespawn ( lua_State* luaVM )
{
	// Verify the three arguments
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bRespawn = lua_toboolean ( luaVM, 2 ) ? true:false;
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::ToggleVehicleRespawn ( pElement, bRespawn ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "toggleVehicleRespawn", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "toggleVehicleRespawn" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleRespawnDelay ( lua_State* luaVM )
{
	// Verify the three arguments
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned long ulTime = static_cast < unsigned long > ( lua_tonumber ( luaVM, 2 ) );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetVehicleRespawnDelay ( pElement, ulTime ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleRespawnDelay", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleRespawnDelay" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleRespawnPosition ( lua_State* luaVM )
{
	// Verify the three arguments
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the element and verify it
        CElement* pElement =lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            // Grab the position
            CVector vecPosition, vecRotation;
            vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
            vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

            int iArgument5 = lua_type ( luaVM, 5 );
            int iArgument6 = lua_type ( luaVM, 6 );
            int iArgument7 = lua_type ( luaVM, 7 );
            if ( ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
                 ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) &&
                 ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING ) )
            {
                vecRotation.fX = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );
                vecRotation.fY = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
                vecRotation.fZ = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );
            }


            if ( CStaticFunctionDefinitions::SetVehicleRespawnPosition ( pElement, vecPosition, vecRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleRespawnPosition", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleRespawnPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleIdleRespawnDelay ( lua_State* luaVM )
{
	// Verify the three arguments
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned long ulTime = static_cast < unsigned long > ( lua_tonumber ( luaVM, 2 ) );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetVehicleIdleRespawnDelay ( pElement, ulTime ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleIdleRespawnDelay", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleIdleRespawnDelay" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RespawnVehicle ( lua_State* luaVM )
{
	// Verify the three arguments
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::RespawnVehicle ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "respawnVehicle", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "respawnVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ResetVehicleExplosionTime ( lua_State* luaVM )
{
	// Verify the three arguments
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::ResetVehicleExplosionTime ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "resetVehicleExplosionTime", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "resetVehicleExplosionTime" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ResetVehicleIdleTime ( lua_State* luaVM )
{
	// Verify the three arguments
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::ResetVehicleIdleTime ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "resetVehicleIdleTime", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "resetVehicleIdleTime" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SpawnVehicle ( lua_State* luaVM )
{
	// Verify the arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    int iArgument7 = lua_type ( luaVM, 7 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) ||
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) ||
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) ||
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) ||
         ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) ||
         ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) ||
         ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING ) )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            // Grab the position and rotation
            CVector vecPosition;
            CVector vecRotation;

            vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
            vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
            vecRotation.fX = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );
            vecRotation.fY = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
            vecRotation.fZ = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );

            if ( CStaticFunctionDefinitions::SpawnVehicle ( pElement, vecPosition, vecRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "spawnVehicle", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "spawnVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleOverrideLights ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucLights = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        if ( pElement )
        {
            if ( ucLights <= 2 )
            {
                if ( CStaticFunctionDefinitions::SetVehicleOverrideLights ( pElement, ucLights ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleOverrideLights", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleOverrideLights" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::AttachTrailerToVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        CVehicle* pTrailer = lua_tovehicle ( luaVM, 2 );

        if ( pVehicle )
        {
            if ( pTrailer )
            {
                if ( CStaticFunctionDefinitions::AttachTrailerToVehicle ( pVehicle, pTrailer ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "attachTrailerToVehicle", "trailer", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "attachTrailerToVehicle", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "attachTrailerToVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::DetachTrailerFromVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        CVehicle* pTrailer = NULL;

        if ( lua_type ( luaVM, 2 )  == LUA_TLIGHTUSERDATA )
            pTrailer = lua_tovehicle ( luaVM, 2 );

        if ( pVehicle )
        {
            if ( CStaticFunctionDefinitions::DetachTrailerFromVehicle ( pVehicle, pTrailer ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "detachTrailerFromVehicle", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "detachTrailerFromVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleEngineState ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bState = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetVehicleEngineState ( pElement, bState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleEngineState", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleEngineState" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleDirtLevel ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( /*iArgument2 == LUA_TSTRING ||*/ iArgument2 == LUA_TNUMBER ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        float fDirtLevel = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );/*( atof ( lua_tostring ( luaVM, 2 ) ) );*/

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetVehicleDirtLevel ( pElement, fDirtLevel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleDirtLevel", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleDirtLevel" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleDamageProof ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bDamageProof = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetVehicleDamageProof ( pElement, bDamageProof ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleDamageProof", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleDamageProof" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehiclePaintjob ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned char ucPaintjob = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetVehiclePaintjob ( pElement, ucPaintjob ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehiclePaintjob", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehiclePaintjob" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleFuelTankExplodable ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            bool bExplodable = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
            if ( CStaticFunctionDefinitions::SetVehicleFuelTankExplodable ( pElement, bExplodable ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleFuelTankExplodable", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleFuelTankExplodable" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetVehicleFrozen ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            {
                if ( CStaticFunctionDefinitions::SetVehicleFrozen ( pVehicle, lua_toboolean ( luaVM, 2 ) ? true:false ) )
                {
                        lua_pushboolean ( luaVM, true );
                }
                return 1;
                    
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "setVehicleFrozen" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleFrozen", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleFrozen" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetTrainDerailed ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            {
                if ( CStaticFunctionDefinitions::SetTrainDerailed ( pVehicle, lua_toboolean ( luaVM, 2 ) ? true : false ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }   
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "setTrainDerailed" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setTrainDerailed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTrainDerailed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetTrainDerailable ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            {
                if ( CStaticFunctionDefinitions::SetTrainDerailable ( pVehicle, lua_toboolean ( luaVM, 2 ) ? true : false ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }   
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "setTrainDerailable" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setTrainDerailable", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTrainDerailable" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetTrainDirection ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            {
                if ( CStaticFunctionDefinitions::SetTrainDirection ( pVehicle, lua_toboolean ( luaVM, 2 ) ? true : false ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "setTrainDirection" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setTrainDirection", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTrainDirection" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetTrainSpeed ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( lua_type ( luaVM, 2 ) == LUA_TNUMBER || lua_type ( luaVM, 2 ) == LUA_TSTRING )
            {
                float fSpeed = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
                if ( CStaticFunctionDefinitions::SetTrainSpeed ( pVehicle, fSpeed ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "setTrainSpeed" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setTrainSpeed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTrainSpeed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateMarker ( lua_State* luaVM )
{
    // Valid position arguments?
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the position
        CVector vecPosition;
        vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );

        // Other defaulted arguments
        unsigned char ucType = CMarker::TYPE_CHECKPOINT;
        float fSize = 4.0f;
        unsigned char ucRed = 0;
        unsigned char ucGreen = 0;
        unsigned char ucBlue = 255;
        unsigned char ucAlpha = 255;
        CElement* pVisibleTo = m_pRootElement;

        // Optional type argument
        char szDefaultType [] = "default";
        const char* szType = szDefaultType;
        int iArgument4 = lua_type ( luaVM, 4 );
        if ( iArgument4 == LUA_TSTRING )
        {
            szType = lua_tostring ( luaVM, 4 );
        }

        // Optional size argument
        int iArgument5 = lua_type ( luaVM, 5 );
        if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
        {
            lua_Number nSize = lua_tonumber ( luaVM, 5 );
            if ( nSize > 0 )
            {
                fSize = static_cast < float > ( nSize );
            }
        }

        // Optional red argument
        int iArgument6 = lua_type ( luaVM, 6 );
        if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
        {
            lua_Number nRed = lua_tonumber ( luaVM, 6 );
            if ( nRed >= 0 && nRed <= 255 )
            {
                ucRed = static_cast < unsigned char > ( nRed );
            }
        }

        // Optional green argument
        int iArgument7 = lua_type ( luaVM, 7 );
        if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
        {
            lua_Number nGreen = lua_tonumber ( luaVM, 7 );
            if ( nGreen >= 0 && nGreen <= 255 )
            {
                ucGreen = static_cast < unsigned char > ( nGreen );
            }
        }

        // Optional blue argument
        int iArgument8 = lua_type ( luaVM, 8 );
        if ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING )
        {
            lua_Number nBlue = lua_tonumber ( luaVM, 8 );
            if ( nBlue >= 0 && nBlue <= 255 )
            {
                ucBlue = static_cast < unsigned char > ( nBlue );
            }
        }

        // Optional alpha argument
        int iArgument9 = lua_type ( luaVM, 9 );
        if ( iArgument9 == LUA_TNUMBER || iArgument9 == LUA_TSTRING )
        {
            lua_Number nAlpha = lua_tonumber ( luaVM, 9 );
            if ( nAlpha >= 0 && nAlpha <= 255 )
            {
                ucAlpha = static_cast < unsigned char > ( nAlpha );
            }
        }

        // Optional visibleto argument
        int iArgument10 = lua_type ( luaVM, 10 );
        if ( iArgument10 == LUA_TLIGHTUSERDATA )
        {
            pVisibleTo = lua_toelement ( luaVM, 10 );
        }
        else if ( iArgument10 == LUA_TBOOLEAN || iArgument10 == LUA_TNIL )
        {
            pVisibleTo = NULL;
        }


		CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource();
			if ( pResource )
			{
				// Create it
				CMarker* pMarker = CStaticFunctionDefinitions::CreateMarker ( pResource, vecPosition, szType, fSize, ucRed, ucGreen, ucBlue, ucAlpha, pVisibleTo );
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
        m_pScriptDebugging->LogBadType ( luaVM, "createMarker" );

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
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CMarker* pMarker = lua_tomarker ( luaVM, 1 );
        if ( pMarker )
        {
            char szMarkerType [64];
            if ( CStaticFunctionDefinitions::GetMarkerType ( pMarker, szMarkerType ) )
            {
                lua_pushstring ( luaVM, szMarkerType );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getMarkerType", "marker", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getMarkerType" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetMarkerSize ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CMarker* pMarker = lua_tomarker ( luaVM, 1 );
        if ( pMarker )
        {
            float fSize;
            if ( CStaticFunctionDefinitions::GetMarkerSize ( pMarker, fSize ) )
            {
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( fSize ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getMarkerSize", "marker", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getMarkerSize" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetMarkerColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CMarker* pMarker = lua_tomarker ( luaVM, 1 );
        if ( pMarker )
        {
            unsigned char ucRed;
            unsigned char ucGreen;
            unsigned char ucBlue;
            unsigned char ucAlpha;
            if ( CStaticFunctionDefinitions::GetMarkerColor ( pMarker, ucRed, ucGreen, ucBlue, ucAlpha ) )
            {
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucRed ) );
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucGreen ) );
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucBlue ) );
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucAlpha ) );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getMarkerColor", "marker", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getMarkerColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetMarkerTarget ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CMarker* pMarker = lua_tomarker ( luaVM, 1 );
        if ( pMarker )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getMarkerTarget", "marker", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getMarkerTarget" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetMarkerIcon ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CMarker* pMarker = lua_tomarker ( luaVM, 1 );
        if ( pMarker )
        {
            char szMarkerIcon [64];
            if ( CStaticFunctionDefinitions::GetMarkerType ( pMarker, szMarkerIcon ) )
            {
                lua_pushstring ( luaVM, szMarkerIcon );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getMarkerIcon", "marker", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getMarkerIcon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetMarkerType ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
         iArgument2 == LUA_TSTRING )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        const char* szType = lua_tostring ( luaVM, 2 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetMarkerType ( pElement, szType ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setMarkerType", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setMarkerType" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetMarkerSize ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetMarkerSize ( pElement, static_cast < float > ( lua_tonumber ( luaVM, 2 ) ) ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setMarkerSize", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setMarkerSize" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetMarkerColor ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Convert to numbers
        CElement* pElement = lua_toelement ( luaVM, 1 );
        double dRed = lua_tonumber ( luaVM, 2 );
        double dGreen = lua_tonumber ( luaVM, 3 );
        double dBlue = lua_tonumber ( luaVM, 4 );
        double dAlpha = lua_tonumber ( luaVM, 5 );

        // Verify the pointer
        if ( pElement )
        {
            // Check the ranges
            if ( dRed >= 0 && dRed <= 255 &&
                 dGreen >= 0 && dGreen <= 255 &&
                 dBlue >= 0 && dBlue <= 255 &&
                 dAlpha >= 0 && dAlpha <= 255 )
            {
                unsigned char ucRed = static_cast < unsigned char > ( dRed );
                unsigned char ucGreen = static_cast < unsigned char > ( dGreen );
                unsigned char ucBlue = static_cast < unsigned char > ( dBlue );
                unsigned char ucAlpha = static_cast < unsigned char > ( dAlpha );

                // Set the new color
                if ( CStaticFunctionDefinitions::SetMarkerColor ( pElement, ucRed, ucGreen, ucBlue, ucAlpha ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setMarkerColor", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setMarkerColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetMarkerTarget ( lua_State* luaVM )
{
    // Verify the element argument
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        // Grab and verify the element
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CVector vecTarget;
            CVector* pTargetVector = NULL;

            // Valid target vector arguments?
            int iArgument2 = lua_type ( luaVM, 2 );
            int iArgument3 = lua_type ( luaVM, 3 );
            int iArgument4 = lua_type ( luaVM, 4 );
            if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
                 ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                 ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
            {
                // Grab the target vector
                vecTarget.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
                vecTarget.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
                vecTarget.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

                pTargetVector = &vecTarget;
            }
            else if ( iArgument2 != LUA_TNONE )
            {
                m_pScriptDebugging->LogBadType ( luaVM, "setMarkerTarget" );

                lua_pushboolean ( luaVM, false );
                return 1;
            }

            // Do it
            if ( CStaticFunctionDefinitions::SetMarkerTarget ( pElement, pTargetVector ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setMarkerTarget", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setMarkerTarget" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetMarkerIcon ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
         iArgument2 == LUA_TSTRING )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        const char* szIcon = lua_tostring ( luaVM, 2 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetMarkerIcon ( pElement, szIcon ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setMarkerIcon", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setMarkerIcon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateBlip ( lua_State* luaVM )
{
    // Position arguments in place?
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );

    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the vector
        CVector vecPosition = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                                        static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                                        static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        // Default colors and size
        unsigned char ucIcon = 0;
        unsigned char ucSize = 2;
        unsigned char ucRed = 255;
        unsigned char ucGreen = 0;
        unsigned char ucBlue = 0;
        unsigned char ucAlpha = 255;
        short sOrdering = 0;
        CElement* pVisibleTo = m_pRootElement;

        // Read out the optional args
        int iArgument4 = lua_type ( luaVM, 4 );
        if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
        {
            ucIcon = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

            int iArgument5 = lua_type ( luaVM, 5 );
            if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
            {
                ucSize = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );

                int iArgument6 = lua_type ( luaVM, 6 );
                if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
                {
                    ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 6 ) );

                    int iArgument7 = lua_type ( luaVM, 7 );
                    if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
                    {
                        ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 7 ) );

                        int iArgument8 = lua_type ( luaVM, 8 );
                        if ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING )
                        {
                            ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 8 ) );

                            int iArgument9 = lua_type ( luaVM, 9 );
                            if ( iArgument9 == LUA_TNUMBER || iArgument9 == LUA_TSTRING )
                            {
                                ucAlpha = static_cast < unsigned char > ( lua_tonumber ( luaVM, 9 ) );

                                int iArgument10 = lua_type ( luaVM, 10 );
                                if ( iArgument10 == LUA_TNUMBER || iArgument10 == LUA_TSTRING )
                                {
                                    sOrdering = static_cast < short > ( lua_tonumber ( luaVM, 10 ) );

                                    // Read out the optional visibleto argument
                                    int iArgument11 = lua_type ( luaVM, 11 );
                                    if ( iArgument11 == LUA_TLIGHTUSERDATA )
                                    {
                                        pVisibleTo = lua_toelement ( luaVM, 11 );
                                    }
                                    else if ( iArgument11 == LUA_TBOOLEAN || iArgument11 == LUA_TNIL )
                                    {
                                        pVisibleTo = NULL;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

		CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource();
			if ( pResource )
			{
				// Create the blip
				CBlip* pBlip = CStaticFunctionDefinitions::CreateBlip ( pResource, vecPosition, ucIcon, ucSize, ucRed, ucGreen, ucBlue, ucAlpha, sOrdering, pVisibleTo );
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
        m_pScriptDebugging->LogBadType ( luaVM, "createBlip" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateBlipAttachedTo ( lua_State* luaVM )
{
    // Element in place?
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            // Default colors and size
            unsigned char ucIcon = 0;
            unsigned char ucSize = 2;
            unsigned char ucRed = 255;
            unsigned char ucGreen = 0;
            unsigned char ucBlue = 0;
            unsigned char ucAlpha = 255;
            short sOrdering = 0;
            CElement* pVisibleTo = m_pRootElement;

            // Read out the optional args
            int iArgument2 = lua_type ( luaVM, 2 );
            if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
            {
                ucIcon = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

                int iArgument3 = lua_type ( luaVM, 3 );
                if ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING )
                {
                    ucSize = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );

                    int iArgument4 = lua_type ( luaVM, 4 );
                    if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
                    {
                        ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

                        int iArgument5 = lua_type ( luaVM, 5 );
                        if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
                        {
                            ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );

                            int iArgument6 = lua_type ( luaVM, 6 );
                            if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
                            {
                                ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 6 ) );

                                int iArgument7 = lua_type ( luaVM, 7 );
                                if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
                                {
                                    ucAlpha = static_cast < unsigned char > ( lua_tonumber ( luaVM, 7 ) );

                                    int iArgument8 = lua_type ( luaVM, 8 );
                                    if ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING )
                                    {
                                        sOrdering = static_cast < short > ( lua_tonumber ( luaVM, 8 ) );
                                    
                                        // Read out the optional visibleto argument
                                        int iArgument9 = lua_type ( luaVM, 9 );
                                        if ( iArgument9 == LUA_TLIGHTUSERDATA )
                                        {
                                            pVisibleTo = lua_toelement ( luaVM, 9 );
                                        }
                                        else if ( iArgument9 == LUA_TBOOLEAN || iArgument9 == LUA_TNIL )
                                        {
                                            pVisibleTo = NULL;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
				CResource * resource = pLuaMain->GetResource();
                if ( resource )
                {
					// Create the blip
					CBlip* pBlip = CStaticFunctionDefinitions::CreateBlipAttachedTo ( resource, pElement, ucIcon, ucSize, ucRed, ucGreen, ucBlue, ucAlpha, sOrdering, pVisibleTo );
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
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "createBlipAttachedTo", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createBlipAttachedTo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBlipIcon ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBlip* pBlip = lua_toblip ( luaVM, 1 );
        if ( pBlip )
        {
            unsigned char ucIcon;
            if ( CStaticFunctionDefinitions::GetBlipIcon ( pBlip, ucIcon ) )
            {
                lua_pushnumber ( luaVM, ucIcon );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getBlipIcon", "blip", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBlipIcon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBlipSize ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBlip* pBlip = lua_toblip ( luaVM, 1 );
        if ( pBlip )
        {
            unsigned char ucSize;
            if ( CStaticFunctionDefinitions::GetBlipSize ( pBlip, ucSize ) )
            {
                lua_pushnumber ( luaVM, ucSize );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getBlipSize", "blip", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBlipSize" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBlipColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBlip* pBlip = lua_toblip ( luaVM, 1 );
        if ( pBlip )
        {
            unsigned char ucRed;
            unsigned char ucGreen;
            unsigned char ucBlue;
            unsigned char ucAlpha;

            if ( CStaticFunctionDefinitions::GetBlipColor ( pBlip, ucRed, ucGreen, ucBlue, ucAlpha ) )
            {
                lua_pushnumber ( luaVM, ucRed );
                lua_pushnumber ( luaVM, ucGreen );
                lua_pushnumber ( luaVM, ucBlue );
                lua_pushnumber ( luaVM, ucAlpha );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getBlipColor", "blip", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBlipColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBlipOrdering ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBlip* pBlip = lua_toblip ( luaVM, 1 );
        if ( pBlip )
        {
            short sOrdering;
            if ( CStaticFunctionDefinitions::GetBlipOrdering ( pBlip, sOrdering ) )
            {
                lua_pushnumber ( luaVM, sOrdering );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getBlipOrdering", "blip", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBlipOrdering" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetBlipIcon ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned char ucIcon = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetBlipIcon ( pElement, ucIcon ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setBlipIcon", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setBlipIcon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetBlipSize ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned char ucSize = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetBlipSize ( pElement, ucSize ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setBlipSize", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setBlipSize" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetBlipColor ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
         ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned char ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            unsigned char ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
            unsigned char ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
            unsigned char ucAlpha = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );

            if ( CStaticFunctionDefinitions::SetBlipColor ( pElement, ucRed, ucGreen, ucBlue, ucAlpha ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setBlipColor", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setBlipColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetBlipOrdering ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            short sOrdering = static_cast < short > ( lua_tonumber ( luaVM, 2 ) );

            if ( CStaticFunctionDefinitions::SetBlipOrdering ( pElement, sOrdering ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setBlipOrdering", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setBlipOrdering" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateObject ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned short usModelID = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );

        if ( CObjectManager::IsValidModel  ( usModelID ) )
        {
            int iArgument2 = lua_type ( luaVM, 2 );
            int iArgument3 = lua_type ( luaVM, 3 );
            int iArgument4 = lua_type ( luaVM, 4 );

            if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
                 ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                 ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
            {
                CVector vecPosition;
                vecPosition.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 2 ) ) );
                vecPosition.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) );
                vecPosition.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 4 ) ) );

                CVector vecRotation;
                int iArgument5 = lua_type ( luaVM, 5 );

                if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
                {
                    vecRotation.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 5 ) ) );
                    int iArgument6 = lua_type ( luaVM, 6 );

                    if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
                    {
                        vecRotation.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 6 ) ) );
                        int iArgument7 = lua_type ( luaVM, 7 );

                        if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
                        {
                            vecRotation.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 7 ) ) );
                        }
                    }
                }

				CLuaMain * pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
				if ( pLuaMain )
				{
					CResource * pResource = pLuaMain->GetResource();
					if ( pResource )
					{
						CObject* pObject = CStaticFunctionDefinitions::CreateObject ( pResource, usModelID, vecPosition, vecRotation );
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
                m_pScriptDebugging->LogBadType ( luaVM, "createObject" );
        }
        else
            m_pScriptDebugging->LogWarning ( luaVM, "Bad model id passed to createObject" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createObject" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetObjectRotation ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CObject* pObject = lua_toobject ( luaVM, 1 );
        if ( pObject )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getObjectRotation", "object", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getObjectRotation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetObjectRotation ( lua_State* luaVM )
{
    int iArgumentType1 = lua_type ( luaVM, 1 );
    int iArgumentType2 = lua_type ( luaVM, 2 );
    int iArgumentType3 = lua_type ( luaVM, 3 );
    int iArgumentType4 = lua_type ( luaVM, 4 );

    if ( ( iArgumentType1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgumentType2 == LUA_TNUMBER || iArgumentType2 == LUA_TSTRING ) &&
         ( iArgumentType3 == LUA_TNUMBER || iArgumentType3 == LUA_TSTRING ) &&
         ( iArgumentType4 == LUA_TNUMBER || iArgumentType4 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CVector vecRotation;
            vecRotation.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            vecRotation.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
            vecRotation.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

            if ( CStaticFunctionDefinitions::SetObjectRotation ( pElement, vecRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setObjectRotation", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setObjectRotation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::MoveObject ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );

    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
         ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CVector vecTargetPosition;
            CVector vecTargetRotation;
            unsigned long ulTime = static_cast < unsigned long > ( lua_tonumber ( luaVM, 2 ) );
            vecTargetPosition.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) );
            vecTargetPosition.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 4 ) ) );
            vecTargetPosition.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 5 ) ) );

            int iArgument6 = lua_type ( luaVM, 6 );
            if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
            {
                vecTargetRotation.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 6 ) ) );

                int iArgument7 = lua_type ( luaVM, 7 );
                if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
                {
                    vecTargetRotation.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 7 ) ) );

                    int iArgument8 = lua_type ( luaVM, 8 );
                    if ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING )
                    {
                        vecTargetRotation.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 8 ) ) );
                    }
                }
            }

            if ( CStaticFunctionDefinitions::MoveObject ( pElement, ulTime, vecTargetPosition, vecTargetRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "moveObject", "object", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "moveObject" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::StopObject ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::StopObject ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "stopObject", "object", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "stopObject" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateRadarArea ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    int iArgument7 = lua_type ( luaVM, 7 );
    int iArgument8 = lua_type ( luaVM, 8 );

    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
         ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING || iArgument5 == LUA_TNONE ) &&
         ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING || iArgument6 == LUA_TNONE ) &&
         ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING || iArgument7 == LUA_TNONE ) &&
         ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING || iArgument8 == LUA_TNONE ) )
    {
        CVector2D vecPosition = CVector2D ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ), static_cast < float > ( lua_tonumber ( luaVM, 2 ) ) );
        CVector2D vecSize = CVector2D ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ), static_cast < float > ( lua_tonumber ( luaVM, 4 ) ) );

        double dRed = 255;
        double dGreen = 0;
        double dBlue = 0;
        double dAlpha = 255;
        if ( iArgument5 != LUA_TNONE ) dRed = lua_tonumber ( luaVM, 5 );
        if ( iArgument6 != LUA_TNONE ) dGreen = lua_tonumber ( luaVM, 6 );
        if ( iArgument7 != LUA_TNONE ) dBlue = lua_tonumber ( luaVM, 7 );
        if ( iArgument8 != LUA_TNONE ) dAlpha = lua_tonumber ( luaVM, 8 );

        if ( dRed >= 0 && dRed <= 255 &&
             dGreen >= 0 && dGreen <= 255 &&
             dBlue >= 0 && dBlue <= 255 &&
             dAlpha >= 0 && dAlpha <= 255 )
        {
            // Read out optional visibleTo argument
            CElement* pVisibleTo = m_pRootElement;
            int iArgument9 = lua_type ( luaVM, 9 );
            if ( iArgument9 == LUA_TLIGHTUSERDATA )
            {
                pVisibleTo = lua_toelement ( luaVM, 9 );
            }
            else if ( iArgument9 == LUA_TBOOLEAN || iArgument9 == LUA_TNIL )
            {
                pVisibleTo = NULL;
            }

			CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
			if ( pLuaMain )
			{
				CResource* pResource = pLuaMain->GetResource();
				if ( pResource )
				{
					// Create it
					CRadarArea* pRadarArea = CStaticFunctionDefinitions::CreateRadarArea ( pResource, vecPosition, vecSize, static_cast < unsigned char > ( dRed ), static_cast < unsigned char > ( dGreen ), static_cast < unsigned char > ( dBlue ), static_cast < unsigned char > ( dAlpha ), pVisibleTo );
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
        }
        else
            m_pScriptDebugging->LogWarning ( luaVM, "Bad color number sent to createRadarArea (0-255)" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createRadarArea" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetRadarAreaSize ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getRadarAreaSize", "radararea", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getRadarAreaSize" );


    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetRadarAreaColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
            unsigned char ucRed = 0;
            unsigned char ucGreen = 0;
            unsigned char ucBlue = 0;
            unsigned char ucAlpha = 0;

            if ( CStaticFunctionDefinitions::GetRadarAreaColor ( pRadarArea, ucRed, ucGreen, ucBlue, ucAlpha ) )
            {
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucRed ) );
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucGreen ) );
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucBlue ) );
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucAlpha ) );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getRadarAreaColor", "radararea", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getRadarAreaColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsRadarAreaFlashing ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
            lua_pushboolean ( luaVM, CStaticFunctionDefinitions::IsRadarAreaFlashing ( pRadarArea ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isRadarAreaFlashing", "radararea", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isRadarAreaFlashing" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsInsideRadarArea ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) &&
         ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER ) )
    {
        CRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
            CVector2D vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                                    static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
            bool bInside = false;
            if ( CStaticFunctionDefinitions::IsInsideRadarArea ( pRadarArea, vecPosition, bInside ) )
            {
                lua_pushboolean ( luaVM, bInside );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isInsideRadarArea", "radararea", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isInsideRadarArea" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetRadarAreaSize ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CVector2D vecSize = CVector2D ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ), static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
            if ( CStaticFunctionDefinitions::SetRadarAreaSize ( pElement, vecSize ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setRadarAreaSize", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setRadarAreaSize" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetRadarAreaColor ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );

    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
         ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
    {
        double dRed = lua_tonumber ( luaVM, 2 );
        double dGreen = lua_tonumber ( luaVM, 3 );
        double dBlue = lua_tonumber ( luaVM, 4 );
        double dAlpha = lua_tonumber ( luaVM, 5 );

        if ( dRed >= 0 && dRed <= 255 &&
             dGreen >= 0 && dGreen <= 255 &&
             dBlue >= 0 && dBlue <= 255 &&
             dAlpha >= 0 && dAlpha <= 255 )
        {
            CElement* pElement = lua_toelement ( luaVM, 1 );
            if ( pElement )
            {
                if ( CStaticFunctionDefinitions::SetRadarAreaColor ( pElement, static_cast < unsigned char > ( dRed ), static_cast < unsigned char > ( dGreen ), static_cast < unsigned char > ( dBlue ), static_cast < unsigned char > ( dAlpha ) ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setRadarAreaColor", "element", 1 );
        }
        else
            m_pScriptDebugging->LogWarning ( luaVM, "Bad color values sent to setRadarAreaColor (0-255)" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setRadarAreaColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetRadarAreaFlashing ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetRadarAreaFlashing ( pElement, lua_toboolean ( luaVM, 2 ) ? true:false ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setRadarAreaFlashing", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setRadarAreaFlashing" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateExplosion ( lua_State* luaVM )
{
	// Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
	int iArgument5 = lua_type ( luaVM, 5 );
	CElement* pCreator = NULL;
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( lua_tonumber ( luaVM, 3 ) ) );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

		if ( iArgument5 == LUA_TLIGHTUSERDATA )
		{
			pCreator = lua_toelement ( luaVM, 5 );
		}

		if ( CStaticFunctionDefinitions::CreateExplosion ( vecPosition, ucType, pCreator ) )
		{
			lua_pushboolean ( luaVM, true );
			return 1;
		}
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "createExplosion" );
	
	lua_pushboolean ( luaVM, false );
	return 1;
}
		

int CLuaFunctionDefinitions::CreateFire ( lua_State* luaVM )
{
	// Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
	int iArgument5 = lua_type ( luaVM, 5 );
	CElement* pCreator = NULL;
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( lua_tonumber ( luaVM, 3 ) ) );
        float fSize = 1.8f;
		if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
		{
			fSize = float ( lua_tonumber ( luaVM, 4 ) );
		}

		if ( iArgument5 == LUA_TLIGHTUSERDATA )
		{
			pCreator = lua_toelement ( luaVM, 5 );
		}

		if ( CStaticFunctionDefinitions::CreateFire ( vecPosition, fSize, pCreator ) )
		{
			lua_pushboolean ( luaVM, true );
			return 1;
		}
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "createFire" );
	
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::PlayMissionAudio ( lua_State* luaVM )
{
	int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );

    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
			unsigned short usSlot = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );

			if ( ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
				 ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
				 ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
			{
	            CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 3 ) ), float ( lua_tonumber ( luaVM, 4 ) ), float ( lua_tonumber ( luaVM, 5 ) ) );

				if ( CStaticFunctionDefinitions::PlayMissionAudio ( pElement, &vecPosition, usSlot ) )
				{
					lua_pushboolean ( luaVM, true );
					return 1;
				}
			} else
			{
				if ( CStaticFunctionDefinitions::PlayMissionAudio ( pElement, NULL, usSlot ) )
				{
					lua_pushboolean ( luaVM, true );
					return 1;
				}
			}
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "playMissionAudio", "element", 1 );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "playMissionAudio" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::PreloadMissionAudio ( lua_State* luaVM )
{
	int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );

    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
		 ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned short usSound = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
			unsigned short usSlot = static_cast < unsigned short > ( lua_tonumber ( luaVM, 3 ) );

			if ( CStaticFunctionDefinitions::PreloadMissionAudio ( pElement, usSound, usSlot ) )
		    {
			    lua_pushboolean ( luaVM, true );
			    return 1;
		    }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "preloadMissionAudio", "element", 1 );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "preloadMissionAudio" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::PlaySoundFrontEnd ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
	if ( iArgument1 == LUA_TLIGHTUSERDATA &&
		 iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
	{
		CElement* pElement = lua_toelement ( luaVM, 1 );
		if ( pElement )
		{
			unsigned long ulSound = static_cast < unsigned long > ( lua_tonumber ( luaVM, 2 ) );

            if ( ulSound <= 101 )
            {
			    if ( CStaticFunctionDefinitions::PlaySoundFrontEnd ( pElement, ulSound ) )
			    {
				    lua_pushboolean ( luaVM, true );
				    return 1;
			    }
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "Invalid sound ID specified. Valid sound IDs are 0 - 101." );
		}
		else
			m_pScriptDebugging->LogBadPointer ( luaVM, "playSoundFrontEnd", "element", 1 );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "playSoundFrontEnd" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::BindKey ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
             lua_type ( luaVM, 2 ) == LUA_TSTRING &&
             lua_type ( luaVM, 3 ) == LUA_TSTRING &&
             lua_type ( luaVM, 4 ) == LUA_TFUNCTION )
        {
            CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
            const char* szKey = lua_tostring ( luaVM, 2 );
            const char* szHitState = lua_tostring ( luaVM, 3 );

            CLuaArguments Arguments;
            Arguments.ReadArguments ( luaVM, 5 ); 

            int iLuaFunction = luaM_toref ( luaVM, 4 );     

			if ( pPlayer )
            {				
			    if ( VERIFY_FUNCTION ( iLuaFunction ) )
                {				
                    if ( CStaticFunctionDefinitions::BindKey ( pPlayer, szKey, szHitState, pLuaMain, iLuaFunction, Arguments ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "bindKey", "function", 4 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "bindKey", "player", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "bindKey" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::UnbindKey ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
            lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
            const char* szKey = lua_tostring ( luaVM, 2 );
            const char* szHitState = NULL;
			int iLuaFunction = LUA_REFNIL;

			if ( lua_type ( luaVM, 3 ) )
                szHitState = lua_tostring ( luaVM, 3 );
            if ( lua_type ( luaVM, 4 ) == LUA_TFUNCTION )
				iLuaFunction = luaM_toref ( luaVM, 4 );
            
            if ( pPlayer )
            {				
                if ( CStaticFunctionDefinitions::UnbindKey ( pPlayer, szKey, pLuaMain, szHitState, iLuaFunction ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "unbindKey", "player", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "unbindKey" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsKeyBound ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
            lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
            const char* szKey = lua_tostring ( luaVM, 2 );
            const char* szHitState = NULL;
			int iLuaFunction = LUA_REFNIL;

            if ( lua_type ( luaVM, 3 ) )
                szHitState = lua_tostring ( luaVM, 3 );
            if ( lua_type ( luaVM, 4 ) )
				iLuaFunction = luaM_toref ( luaVM, 4 );
            
            if ( !pPlayer )
                m_pScriptDebugging->LogBadPointer ( luaVM, "isKeyBound", "player", 1 );
			else {
                bool bBound;
                if ( CStaticFunctionDefinitions::IsKeyBound ( pPlayer, szKey, pLuaMain, szHitState, iLuaFunction, bBound ) )
                {
                    lua_pushboolean ( luaVM, bBound );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "isKeyBound" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetFunctionsBoundToKey ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
            lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
            const char* szKey = lua_tostring ( luaVM, 2 );
            const char* szHitState = NULL;
            if ( lua_type ( luaVM, 3 ) )
            {
                szHitState = lua_tostring ( luaVM, 3 );
            }
            
            if ( pPlayer )
            {
                bool bCheckHitState = false, bHitState = true;
                if ( szHitState )
                {
                    if ( stricmp ( szHitState, "down" ) == 0 )
                        bCheckHitState = true, bHitState = true;
                    else if ( stricmp ( szHitState, "up" ) == 0 )
                        bCheckHitState = true, bHitState = false;
                }

                // Create a new table
		        lua_newtable ( luaVM );

                // Add all the bound functions to it
                unsigned int uiIndex = 0;
                list < CKeyBind* > ::iterator iter = pPlayer->GetKeyBinds ()->IterBegin ();
                for ( ; iter != pPlayer->GetKeyBinds ()->IterEnd (); iter++ )
                {                                   
                    CKeyBind* pKeyBind = *iter;
                    if ( !pKeyBind->bIsBeingDeleted )
                    {
                        switch ( pKeyBind->GetType () )
                        {
                            case KEY_BIND_FUNCTION:
                            {
                                CKeyFunctionBind* pBind = static_cast < CKeyFunctionBind* > ( pKeyBind );
                                if ( !bCheckHitState || pBind->bHitState == bHitState )
                                {
                                    if ( strcmp ( szKey, pBind->boundKey->szKey ) == 0 )
                                    {
                                        lua_pushnumber ( luaVM, ++uiIndex );
										lua_rawgeti ( luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction );
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
                                    if ( strcmp ( szKey, pBind->boundControl->szControl ) == 0 )
                                    {
                                        lua_pushnumber ( luaVM, ++uiIndex );
										lua_rawgeti ( luaVM, LUA_REGISTRYINDEX, pBind->m_iLuaFunction );
			                            lua_settable ( luaVM, -3 );
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "getFunctionsBoundToKey", "player", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getFunctionsBoundToKey" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetKeyBoundToFunction ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
            lua_type ( luaVM, 2 ) == LUA_TFUNCTION )
        {
            CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
			int iLuaFunction = luaM_toref ( luaVM, 2 );
            
            if ( pPlayer )
            {
                // get the key
                unsigned int uiIndex = 0;
                list < CKeyBind* > ::iterator iter = pPlayer->GetKeyBinds ()->IterBegin ();
                for ( ; iter != pPlayer->GetKeyBinds ()->IterEnd (); iter++ )
                {                                   
                    CKeyBind* pKeyBind = *iter;
                    if ( !pKeyBind->bIsBeingDeleted )
                    {
                        switch ( pKeyBind->GetType () )
                        {
                            case KEY_BIND_FUNCTION:
                            {
                                CKeyFunctionBind* pBind = static_cast < CKeyFunctionBind* > ( pKeyBind );
								// ACHTUNG: DOES IT FIND THE CORRECT LUA REF HERE?
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
								// ACHTUNG: DOES IT FIND THE CORRECT LUA REF HERE?
                                if ( iLuaFunction == pBind->m_iLuaFunction )
                                {
                                    lua_pushstring ( luaVM, pBind->boundKey->szKey );
                                    return 1;
                                }
                                break;
                            }
                        }
                    }
                }
                lua_pushboolean ( luaVM, false );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "getKeyBoundToFunction", "player", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getKeyBoundToFunction" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetControlState ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        const char* szControl = lua_tostring ( luaVM, 2 );

        if ( pPlayer )
        {
            bool bState;
            if ( CStaticFunctionDefinitions::GetControlState ( pPlayer, const_cast < char* > ( szControl ), bState ) )
            {
                lua_pushboolean ( luaVM, bState );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getControlState", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getControlState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsControlEnabled ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        const char* szControl = lua_tostring ( luaVM, 2 );

        if ( pPlayer )
        {
            bool bEnabled;
            if ( CStaticFunctionDefinitions::IsControlEnabled ( pPlayer, const_cast < char* > ( szControl ), bEnabled ) )
            {
                lua_pushboolean ( luaVM, bEnabled );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isControlEnabled", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isControlEnabled" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetControlState ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING &&
         lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        const char* szControl = lua_tostring ( luaVM, 2 );
        bool bState = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;

        if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::SetControlState ( pPlayer, const_cast < char* > ( szControl ), bState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setControlState", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setControlState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ToggleControl ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING &&
         lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        const char* szControl = lua_tostring ( luaVM, 2 );
        bool bEnabled = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;

        if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::ToggleControl ( pPlayer, const_cast < char* > ( szControl ), bEnabled ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "enableControl", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "enableControl" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ToggleAllControls ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        bool bEnabled = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
        bool bGTAControls = true, bMTAControls = true;

        if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
            bGTAControls = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;

        if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
            bMTAControls = ( lua_toboolean ( luaVM, 4 ) ) ? true:false;

        if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::ToggleAllControls ( pPlayer, bGTAControls, bMTAControls, bEnabled ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "toggleAllControls", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "toggleAllControls" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateTeam ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szName = lua_tostring ( luaVM, 1 );
        unsigned char ucRed = 235;
        unsigned char ucGreen = 221;
        unsigned char ucBlue = 178;

        int iArgument2 = lua_type ( luaVM, 2 );
        int iArgument3 = lua_type ( luaVM, 3 );
        int iArgument4 = lua_type ( luaVM, 4 );
        if ( ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) &&
             ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER ) &&
             ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER ) )
        {
            ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
            ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
        }

		CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource();
			if ( pResource )
			{
				CTeam* pTeam = CStaticFunctionDefinitions::CreateTeam ( pResource, const_cast < char* > ( szName ), ucRed, ucGreen, ucBlue );
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
        m_pScriptDebugging->LogBadType ( luaVM, "createTeam" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTeamFromName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szTeamName = lua_tostring ( luaVM, 1 );

        CTeam* pTeam = CStaticFunctionDefinitions::GetTeamFromName ( szTeamName );
        if ( pTeam )
        {
            lua_pushelement ( luaVM, pTeam );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getTeamFromName" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTeamName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
        {
            char szTeamName [ 128 ];
            if ( CStaticFunctionDefinitions::GetTeamName ( pTeam, szTeamName, 128 ) )
            {
                lua_pushstring ( luaVM, szTeamName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getTeamName", "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getTeamName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTeamColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getTeamColor", "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getTeamColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTeamFriendlyFire ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
        {
            bool bFriendlyFire;
            if ( CStaticFunctionDefinitions::GetTeamFriendlyFire ( pTeam, bFriendlyFire ) )
            {
                lua_pushboolean ( luaVM, bFriendlyFire );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getTeamFriendlyFire", "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getTeamFriendlyFire" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayersInTeam ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CTeam* pTeam = lua_toteam ( luaVM, 1 );
            if ( pTeam )
            {
                lua_newtable ( luaVM );

                pTeam->GetPlayers ( pLuaMain );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayersInTeam", "team", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getPlayersInTeam" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CountPlayersInTeam ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
        {
            unsigned int uiCount;
            if ( CStaticFunctionDefinitions::CountPlayersInTeam ( pTeam, uiCount ) )
            {
                lua_pushnumber ( luaVM, uiCount );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "countPlayersInTeam", "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "countPlayersInTeam" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerTeam ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        CTeam* pTeam = NULL;
        if ( lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
        {
            pTeam = lua_toteam ( luaVM, 2 );
        }

        if ( pPlayer )
        {
            if ( CStaticFunctionDefinitions::SetPlayerTeam ( pPlayer, pTeam ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPlayerTeam", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerTeam" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetTeamName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CTeam* pTeam = lua_toteam ( luaVM, 1 );
        const char* szName = lua_tostring ( luaVM, 2 );

        if ( pTeam )
        {
            if ( CStaticFunctionDefinitions::SetTeamName ( pTeam, const_cast < char* > ( szName ) ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setTeamName", "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTeamName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetTeamColor ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) &&
         ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER ) &&
         ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER ) )
    {
        CTeam* pTeam = lua_toteam ( luaVM, 1 );
        unsigned char ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        unsigned char ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        unsigned char ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

        if ( pTeam )
        {
            if ( CStaticFunctionDefinitions::SetTeamColor ( pTeam, ucRed, ucGreen, ucBlue ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setTeamColor", "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTeamColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetTeamFriendlyFire ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CTeam* pTeam = lua_toteam ( luaVM, 1 );
        bool bFriendlyFire = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pTeam )
        {
            if ( CStaticFunctionDefinitions::SetTeamFriendlyFire ( pTeam, bFriendlyFire ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setTeamFriendlyFire", "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTeamFriendlyFire" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateWater ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            int iArgument1  = lua_type ( luaVM, 1 );
            int iArgument2  = lua_type ( luaVM, 2 );
            int iArgument3  = lua_type ( luaVM, 3 );
            int iArgument4  = lua_type ( luaVM, 4 );
            int iArgument5  = lua_type ( luaVM, 5 );
            int iArgument6  = lua_type ( luaVM, 6 );
            int iArgument7  = lua_type ( luaVM, 7 );
            int iArgument8  = lua_type ( luaVM, 8 );
            int iArgument9  = lua_type ( luaVM, 9 );
            int iArgument10 = lua_type ( luaVM, 10 );
            int iArgument11 = lua_type ( luaVM, 11 );
            int iArgument12 = lua_type ( luaVM, 12 );
            if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
                 ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
                 ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                 ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
                 ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
                 ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) &&
                 ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING ) &&
                 ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING ) &&
                 ( iArgument9 == LUA_TNUMBER || iArgument9 == LUA_TSTRING ) )
            {
                CVector v1 ( (float)lua_tonumber(luaVM, 1), (float)lua_tonumber(luaVM, 2), (float)lua_tonumber(luaVM, 3) );
                CVector v2 ( (float)lua_tonumber(luaVM, 4), (float)lua_tonumber(luaVM, 5), (float)lua_tonumber(luaVM, 6) );
                CVector v3 ( (float)lua_tonumber(luaVM, 7), (float)lua_tonumber(luaVM, 8), (float)lua_tonumber(luaVM, 9) );
                if ( ( iArgument10 == LUA_TNUMBER || iArgument10 == LUA_TSTRING ) &&
                     ( iArgument11 == LUA_TNUMBER || iArgument11 == LUA_TSTRING ) &&
                     ( iArgument12 == LUA_TNUMBER || iArgument12 == LUA_TSTRING ) )
                {
                    CVector v4 ( (float)lua_tonumber(luaVM, 10),
                                 (float)lua_tonumber(luaVM, 11),
                                 (float)lua_tonumber(luaVM, 12) );
                    CWater* pWater = CStaticFunctionDefinitions::CreateWater (
                        pResource, &v1, &v2, &v3, &v4 );
                    if ( pWater )
                    {
                        CElementGroup * pGroup = pResource->GetElementGroup();
                        if ( pGroup )
                        {
                            pGroup->Add ( pWater );
                        }
                        lua_pushelement ( luaVM, pWater );
                        return 1;
                    }
                }
                else
                {
                    CWater* pWater = CStaticFunctionDefinitions::CreateWater (
                        pResource, &v1, &v2, &v3, NULL );
                    if ( pWater )
                    {
                        CElementGroup * pGroup = pResource->GetElementGroup();
                        if ( pGroup )
                        {
                            pGroup->Add ( pWater );
                        }
                        lua_pushelement ( luaVM, pWater );
                        return 1;
                    }
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "createWater" );
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetWaterLevel ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            int iArgument1 = lua_type ( luaVM, 1 );
            int iArgument2 = lua_type ( luaVM, 2 );
            int iArgument3 = lua_type ( luaVM, 3 );
            int iArgument4 = lua_type ( luaVM, 4 );

            if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
            {
                if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
                     ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                     ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
                {
                    // (x, y, z, level)
                    /*CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                                          static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                                          static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                    float fLevel = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
                    if ( CStaticFunctionDefinitions::SetWaterLevel ( &vecPosition, fLevel, pResource ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }*/
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }
                else
                {
                    // (level)
                    float fLevel = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
                    if ( CStaticFunctionDefinitions::SetWaterLevel ( (CVector *)NULL, fLevel ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
            }
            else if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
                      ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
            {
                // (water, level)
                CWater* pWater = lua_towater ( luaVM, 1 );
                float fLevel = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
                if ( pWater )
                {
                    if ( CStaticFunctionDefinitions::SetWaterLevel ( pWater, fLevel ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "setWaterLevel", "water", 1 );
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "setWaterLevel" );
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetWaterVertexPosition ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CWater* pWater = lua_towater ( luaVM, 1 );
        if ( pWater )
        {
            int iVertexIndex = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getWaterVertexPosition", "water", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getWaterVertexPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetWaterVertexPosition ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );

    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
       ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
       ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
       ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
       ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
    {
        CWater* pWater = lua_towater ( luaVM, 1 );
        if ( pWater )
        {
            int iVertexIndex = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
            CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                                  static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                                  static_cast < float > ( lua_tonumber ( luaVM, 5 ) ) );
            if ( CStaticFunctionDefinitions::SetWaterVertexPosition ( pWater, iVertexIndex, vecPosition ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setWaterVertexPosition", "water", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWaterVertexPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateColCircle ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), 0.0f );
        float fRadius = float ( lua_tonumber ( luaVM, 3 ) );
        if ( fRadius < 0.0f ) fRadius = 0.1f;

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
        m_pScriptDebugging->LogBadType ( luaVM, "createColCircle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateColCuboid ( lua_State* luaVM )
{ // Formerly createColCube
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
         ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
         ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecSize = CVector ( float ( lua_tonumber ( luaVM, 4 ) ), float ( lua_tonumber ( luaVM, 5 ) ), float ( lua_tonumber ( luaVM, 6 ) ) );
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
        m_pScriptDebugging->LogBadType ( luaVM, "createColCuboid" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateColSphere ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( lua_tonumber ( luaVM, 3 ) ) );
        float fRadius = float ( lua_tonumber ( luaVM, 4 ) );
        if ( fRadius < 0.0f ) fRadius = 0.1f;

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
        m_pScriptDebugging->LogBadType ( luaVM, "createColSphere" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateColRectangle ( lua_State* luaVM )
{ // Formerly createColSquare
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( 0 ) );
        CVector2D vecSize = CVector2D ( float ( lua_tonumber ( luaVM, 3 ) ), float ( lua_tonumber ( luaVM, 4 ) ) );
        if ( vecSize.fX < 0.0f ) vecSize.fX = 0.1f;
        if ( vecSize.fY < 0.0f ) vecSize.fY = 0.1f;

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
        m_pScriptDebugging->LogBadType ( luaVM, "createColPolygon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateColPolygon ( lua_State* luaVM )
{ // Formerly createColSquare
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( ( float ) lua_tonumber ( luaVM, 1 ), ( float ) lua_tonumber ( luaVM, 2 ), 0.0f );

		CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource();
			if ( pResource )
			{
				// Create it and return it
				CColPolygon* pShape = CStaticFunctionDefinitions::CreateColPolygon ( pResource, vecPosition );
				if ( pShape )
				{
                    // Get the points
                    int iArgument = 3;
                    int iArgumentX = lua_type ( luaVM, iArgument++ );
                    int iArgumentY = lua_type ( luaVM, iArgument++ );
                    while ( iArgumentX != LUA_TNONE && iArgumentY != LUA_TNONE )
                    {
                        if ( ( iArgumentX == LUA_TNUMBER || iArgumentX == LUA_TSTRING ) &&
                             ( iArgumentY == LUA_TNUMBER || iArgumentY == LUA_TSTRING ) )
                        {
                            pShape->AddPoint ( CVector2D ( ( float ) lua_tonumber ( luaVM, iArgument - 2 ),
                                                           ( float ) lua_tonumber ( luaVM, iArgument - 1 ) ) );

                            iArgumentX = lua_type ( luaVM, iArgument++ );
                            iArgumentY = lua_type ( luaVM, iArgument++ );
                        }
                        else
                        {
                            break;
                        }
                    }

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
        m_pScriptDebugging->LogBadType ( luaVM, "createColRectangle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateColTube ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
         ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
    {
        // Grab the values
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( lua_tonumber ( luaVM, 3 ) ) );
        float fRadius = float ( lua_tonumber ( luaVM, 4 ) );
        float fHeight = float ( lua_tonumber ( luaVM, 5 ) );
        if ( fRadius < 0.0f ) fRadius = 0.1f;
        if ( fHeight < 0.0f ) fHeight = 0.1f;

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
        m_pScriptDebugging->LogBadType ( luaVM, "createColTube" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetWeaponNameFromID ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned char ucID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
    
        char szBuffer [256];
        if ( CStaticFunctionDefinitions::GetWeaponNameFromID ( ucID, szBuffer ) )
        {
            lua_pushstring ( luaVM, szBuffer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getWeaponNameFromID" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetWeaponIDFromName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szName = lua_tostring ( luaVM, 1 );
        unsigned char ucID;

        if ( CStaticFunctionDefinitions::GetWeaponIDFromName ( szName, ucID ) )
        {
            lua_pushnumber ( luaVM, ucID );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getWeaponIDFromName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBodyPartName ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned char ucID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
    
        char szBuffer [256];
        if ( CStaticFunctionDefinitions::GetBodyPartName ( ucID, szBuffer ) )
        {
            lua_pushstring ( luaVM, szBuffer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBodyPartName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetClothesByTypeIndex ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        unsigned char ucIndex = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        char szTexture [ 128 ], szModel [ 128 ];
        if ( CStaticFunctionDefinitions::GetClothesByTypeIndex ( ucType, ucIndex, szTexture, szModel ) )
        {
            lua_pushstring ( luaVM, szTexture );
            lua_pushstring ( luaVM, szModel );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getClothesByTypeIndex" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTypeIndexFromClothes ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szTexture = lua_tostring ( luaVM, 1 );
        const char* szModel = NULL;
        if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
            szModel = lua_tostring ( luaVM, 2 );

        unsigned char ucType, ucIndex;
        if ( CStaticFunctionDefinitions::GetTypeIndexFromClothes ( const_cast < char* > ( szTexture ), const_cast < char* > ( szModel ), ucType, ucIndex ) )
        {
            lua_pushnumber ( luaVM, ucType );
            lua_pushnumber ( luaVM, ucIndex );
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getTypeIndexFromClothes" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetClothesTypeName ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );

        char szName [ 40 ];
        if ( CStaticFunctionDefinitions::GetClothesTypeName ( ucType, szName ) )
        {
            lua_pushstring ( luaVM, szName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getClothesTypeName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::AddCommandHandler ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( type ( 1, LUA_TSTRING ) &&
			 lua_type ( luaVM, 2 ) == LUA_TFUNCTION )
        {
            // Check if we have a restricted argument. Read it out if neccessary
            bool bRestricted = false;
            if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
                bRestricted = ( lua_toboolean ( luaVM, 3 ) ) ? true : false;

            bool bCaseSensitive = true;
            if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
                bCaseSensitive = ( ( lua_toboolean ( luaVM, 4 ) == 0 ) ? false : true );


            // Grab the strings. Valid?
            const char* szKey = lua_tostring ( luaVM, 1 );
            int iLuaFunction = luaM_toref ( luaVM, 2 );

            if ( szKey [0] != 0 && VERIFY_FUNCTION ( iLuaFunction ) )
			{
                // Add them to our list over command handlers
                if ( m_pRegisteredCommands->AddCommand ( pLuaMain, szKey, iLuaFunction, bRestricted, bCaseSensitive ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogWarning ( luaVM, "Empty key or handler strings sent to addCommandHandler" );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "addCommandHandler" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemoveCommandHandler ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Got a string?
        int iArgument1 = lua_type ( luaVM, 1 );
        if ( iArgument1 == LUA_TSTRING )
        {
            // Grab the string. Valid?
            const char* szKey = lua_tostring ( luaVM, 1 );
            if ( szKey [0] )
            {
				int iLuaFunction = NULL;
				if ( lua_type ( luaVM, 2 ) == LUA_TFUNCTION )
					iLuaFunction = luaM_toref ( luaVM, 2 );

                // Remove it from our list
                if ( m_pRegisteredCommands->RemoveCommand ( pLuaMain, szKey, iLuaFunction ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogWarning ( luaVM, "Empty key string sent to removeCommandHandler" );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "removeCommandHandler" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ExecuteCommandHandler ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Got a string?
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING &&
             lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
        {
            // Grab the string. Valid?
            const char* szKey = lua_tostring ( luaVM, 1 );
            if ( szKey [0] )
            {
                // Grab the element
                CElement* pElement = lua_toelement ( luaVM, 2 );
                if ( pElement )
                {
                    CClient* pClient = NULL;
                    if ( pElement->GetType () == CElement::PLAYER )
                        pClient = static_cast < CClient* > ( static_cast < CPlayer* > ( pElement ) );

                    if ( pClient )
                    {
                        // Grab the args
                        const char* szArgs = 0;
                        if ( lua_type ( luaVM, 3 ) == LUA_TSTRING )
                            szArgs = lua_tostring ( luaVM, 3 );

                        // Call it
                        if ( m_pRegisteredCommands->ProcessCommand ( szKey, szArgs, pClient ) )
                        {
                            lua_pushboolean ( luaVM, true );
                            return 1;
                        }
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "executeCommandHandler", "element", 2 );
            }
            else
                m_pScriptDebugging->LogWarning ( luaVM, "Empty key string sent to executeCommandHandler" );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "executeCommandHandler" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::toJSON ( lua_State* luaVM )
{
    // Got a string argument?
    if ( lua_type ( luaVM, 1 ) > LUA_TNIL )
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
    // Got a string argument?
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        // Grab the JSON string
        const char* szJSONString = lua_tostring ( luaVM, 1 );

        // Read it into lua arguments
        CLuaArguments Converted;
        if ( Converted.ReadFromJSONString ( szJSONString ) )
        {
            // Return it as data
            Converted.PushArguments ( luaVM );
            return Converted.Count ();
        }
    }

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::GetMaxPlayers ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, CStaticFunctionDefinitions::GetMaxPlayers () );
    return 1;
}


int CLuaFunctionDefinitions::OutputChatBox ( lua_State* luaVM )
{
    // Read out optional element and color arguments
    CElement* pElement = m_pRootElement;
    unsigned char ucRed = 235;
    unsigned char ucGreen = 221;
    unsigned char ucBlue = 178;
    bool bColorCoded = false;
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument2 == LUA_TLIGHTUSERDATA )
    {
        // Grab the element. Return false if it's invalid
        pElement = lua_toelement ( luaVM, 2 );
        if ( !pElement )
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        int iArgument3 = lua_type ( luaVM, 3 );
        if ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER )
        {
            ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
            int iArgument4 = lua_type ( luaVM, 4 );
            if ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER )
            {
                ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
                int iArgument5 = lua_type ( luaVM, 5 );
                if ( iArgument5 == LUA_TSTRING || iArgument5 == LUA_TNUMBER )
                {
                    ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );

                    if ( lua_type ( luaVM, 6 ) == LUA_TBOOLEAN )
                        bColorCoded = ( lua_toboolean ( luaVM, 6 ) ) ? true:false;
                }
            }
        }
    }

    // Verify argument
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TSTRING || iArgument1 == LUA_TNUMBER )
    {
        // Grab it
        const char* szChat = lua_tostring ( luaVM, 1 );

        // Do it
        if ( CStaticFunctionDefinitions::OutputChatBox ( szChat, pElement, ucRed, ucGreen, ucBlue, bColorCoded ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "outputChatBox" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::OutputConsole ( lua_State* luaVM )
{
    // Read out optional element and color arguments
    CElement* pElement = m_pRootElement;
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument2 == LUA_TLIGHTUSERDATA )
    {
        // Grab the element. Return false if it's invalid
        pElement = lua_toelement ( luaVM, 2 );
        if ( !pElement )
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }
    }

    // Verify argument
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TSTRING || iArgument1 == LUA_TNUMBER )
    {
        // Grab it
        const char* szChat = lua_tostring ( luaVM, 1 );

        // Do it
        bool bTemp = false;
        if ( CStaticFunctionDefinitions::OutputConsole ( szChat, pElement ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "outputConsole" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::OutputDebugString ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
        {
        // Grab the optional level argument
        unsigned int uiLevel = 3;
        int iRed = 255;
        int iGreen = 255;
        int iBlue = 255;
        int iArgument2 = lua_type ( luaVM, 2 );
        int iArgument3 = lua_type ( luaVM, 3 );
        int iArgument4 = lua_type ( luaVM, 4 );
        int iArgument5 = lua_type ( luaVM, 5 );
        if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
        {
            uiLevel = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
            if ( uiLevel == 0 )
            {
                // level 0 can specify its own color, check if they have
                if ( iArgument3 != LUA_TNONE )
                {
                    // if they have, check that they've specified 3 potential numbers
                    if ( ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
                        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
                    {
                        // read out the numbers
                        iRed = static_cast < unsigned int> ( lua_tonumber ( luaVM, 3 ) );
                        iGreen = static_cast < unsigned int> ( lua_tonumber ( luaVM, 4 ) );
                        iBlue = static_cast < unsigned int> ( lua_tonumber ( luaVM, 5 ) );

                        // check they're in range
                        if ( iRed > 255 || iRed < 0 ) 
                            m_pScriptDebugging->LogWarning ( luaVM, "Specify a red value between 0 and 255" );
                        else if ( iGreen > 255 || iGreen < 0 )
                            m_pScriptDebugging->LogWarning ( luaVM, "Specify a green value between 0 and 255" );
                        else if ( iBlue >  255 || iBlue <  0 )
                            m_pScriptDebugging->LogWarning ( luaVM, "Specify a blue value between 0 and 255" );
                    }
                    else
                    {
                        // specified something as the 3rd argument, but it can't be a number
                        m_pScriptDebugging->LogBadType ( luaVM, "outputDebugString" );
                    }
                } // didn't spec a color
            } // wasn't level 0

            if ( uiLevel < 0 || uiLevel > 3 )
            {
                m_pScriptDebugging->LogWarning ( luaVM, "Bad level argument sent to outputDebugString (0-3)" );

                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }

        // Valid string?
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
        {
            // Output it
            const char* szString = lua_tostring ( luaVM, 1 );
            if ( uiLevel == 1 )
            {
                m_pScriptDebugging->LogError ( luaVM, "%s", szString );
            }
            else if ( uiLevel == 2 )
            {
                m_pScriptDebugging->LogWarning ( luaVM, "%s", szString );
            }
            else if ( uiLevel == 3 )
            {
                m_pScriptDebugging->LogInformation ( luaVM, "%s", szString );
            }
            else if ( uiLevel == 0 )
            {
                m_pScriptDebugging->LogCustom ( luaVM, iRed, iGreen, iBlue, "%s", szString );
            }

            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "outputDebugString" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::OutputServerLog ( lua_State* luaVM )
{
    // Verify the argument
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TSTRING || iArgument1 == LUA_TNUMBER )
    {
        // Grab it
        const char* szText = lua_tostring ( luaVM, 1 );

        // Print it
        CLogger::LogPrintf ( "%s\n", szText );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "outputServerLog" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetDistanceBetweenPoints2D ( lua_State* luaVM )
{
    // We got 6 valid float arguments?
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Put them into two vectors
        CVector vecPointA ( static_cast < float > ( atof ( lua_tostring ( luaVM, 1 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 2 ) ) ), 0.0f );
        CVector vecPointB ( static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 4 ) ) ), 0.0f );

        // Return the distance
        lua_pushnumber ( luaVM, DistanceBetweenPoints2D ( vecPointA, vecPointB ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getDistanceBetweenPoints2D" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetDistanceBetweenPoints3D ( lua_State* luaVM )
{
    // We got 6 valid float arguments?
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
         ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
         ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
    {
        // Put them into two vectors
        CVector vecPointA ( static_cast < float > ( atof ( lua_tostring ( luaVM, 1 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 2 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) ) );
        CVector vecPointB ( static_cast < float > ( atof ( lua_tostring ( luaVM, 4 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 5 ) ) ), static_cast < float > ( atof ( lua_tostring ( luaVM, 6 ) ) ) );

        // Return the distance
        lua_pushnumber ( luaVM, DistanceBetweenPoints3D ( vecPointA, vecPointB ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getDistanceBetweenPoints3D" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetTickCount_ ( lua_State* luaVM )
{
    double dTime = GetSecondCount () * 1000.0;
    lua_pushnumber ( luaVM, dTime );
    return 1;
}


int CLuaFunctionDefinitions::GetCTime ( lua_State* luaVM )
{
    time_t timer;
    time ( &timer );
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER || lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        timer = ( time_t ) lua_tonumber ( luaVM, 1 );
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

    ret.PushAsTable(luaVM);

    return 1;
}


int CLuaFunctionDefinitions::Split ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) != LUA_TSTRING ) || ( lua_type ( luaVM, 2 ) != LUA_TNUMBER ) )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "split" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }

    const char* szText = lua_tostring ( luaVM, 1 );
    int iDelimiter = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );

    // Copy the string
    char* strText = new char [ strlen ( szText ) + 1 ];
    strcpy ( strText, szText );

    char szDelimiter [32];
    szDelimiter [31] = 0;
    _snprintf ( szDelimiter, 31, "%c", iDelimiter );

    unsigned int uiCount = 0;
    char* szToken = strtok ( strText, szDelimiter );

	// Create a new table
	lua_newtable ( luaVM );

	// Add our first token
	lua_pushnumber ( luaVM, ++uiCount );
	lua_pushstring ( luaVM, szToken );
	lua_settable ( luaVM, -3 );

    // strtok until we're out of tokens
	while ( szToken = strtok ( NULL, szDelimiter ) )
	{
		// Add the token to the table
		lua_pushnumber ( luaVM, ++uiCount );
		lua_pushstring ( luaVM, szToken );
		lua_settable ( luaVM, -3 );
	}

    // Delete the text
    delete [] strText;

	return 1;
}


int CLuaFunctionDefinitions::GetTok ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) != LUA_TSTRING ) || ( lua_type ( luaVM, 2 ) != LUA_TNUMBER ) || ( lua_type ( luaVM, 3 ) != LUA_TNUMBER ) )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "gettok" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }

    const char* szText = lua_tostring ( luaVM, 1 );
    int iToken = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
    int iDelimiter = static_cast < int > ( lua_tonumber ( luaVM, 3 ) );

    unsigned int uiCount = 0;

    if ( iToken > 0 && iToken < 1024 )
    {
        // Copy the string
        char* strText = new char [ strlen ( szText ) + 1 ];
        strcpy ( strText, szText );

        char szDelimiter [32];
        szDelimiter [31] = 0;
        _snprintf ( szDelimiter, 31, "%c", iDelimiter );

        unsigned int uiCount = 1;
        char* szToken = strtok ( strText, szDelimiter );

        // We're looking for the first part?
        if ( iToken != 1 )
        {
            // strtok count number of times
            do
            {
                uiCount++;
                szToken = strtok ( NULL, szDelimiter );
            }
            while ( uiCount != iToken );
        }

        // Found it?
        if ( szToken )
        {
            // Return it
            lua_pushstring ( luaVM, szToken );
            delete [] strText;
            return 1;
        }

        // Delete the text
        delete [] strText;
    }
    else
        m_pScriptDebugging->LogWarning ( luaVM, "Token parameter sent to split must be greater than 0 and smaller than 1024" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetTimer ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TFUNCTION )
        {
			CLuaTimer* pLuaTimer = luaMain->GetTimerManager ()->AddTimer ( luaVM );
			if ( pLuaTimer )
            {
				lua_pushtimer ( luaVM, pLuaTimer );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "setTimer" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::KillTimer ( lua_State* luaVM )
{
	CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
		CLuaTimer* pLuaTimer = lua_totimer ( luaVM, 1 );
		if ( pLuaTimer )
		{
			luaMain->GetTimerManager ()->RemoveTimer ( pLuaTimer );
			
			lua_pushboolean ( luaVM, true );
			return 1;
		}
		else
			m_pScriptDebugging->LogBadType ( luaVM, "killTimer" );
	}

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::GetTimers ( lua_State* luaVM )
{
	// Find our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
		unsigned long ulTime = 0;
		int iArgument1 = lua_type ( luaVM, 1 );
		if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
        {
			ulTime = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );
		}            
		// Create a new table
		lua_newtable ( luaVM );

        // Add all the timers with less than ulTime left
		pLuaMain->GetTimerManager ()->GetTimers ( ulTime, pLuaMain );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetColorFromString ( lua_State* luaVM )
{
    unsigned char ucColorRed, ucColorGreen, ucColorBlue, ucColorAlpha;
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TSTRING )
    {
        const char* szColor = lua_tostring ( luaVM, 1 );

        if ( XMLColorToInt ( szColor, ucColorRed, ucColorGreen, ucColorBlue, ucColorAlpha ) )
        {
            lua_pushnumber ( luaVM, ucColorRed );
            lua_pushnumber ( luaVM, ucColorGreen );
            lua_pushnumber ( luaVM, ucColorBlue );
            lua_pushnumber ( luaVM, ucColorAlpha );
            return 4;
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::Reference ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TTABLE || iArgument1 == LUA_TFUNCTION ||
         iArgument1 == LUA_TUSERDATA || iArgument1 == LUA_TTHREAD ||
         iArgument1 == LUA_TLIGHTUSERDATA )
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
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        int iPointer = static_cast < int > ( lua_tonumber ( luaVM, 1 ) );
        lua_getref ( luaVM, iPointer );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
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
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Verify arguments
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
             lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
        {
            // Grab the xml node
            CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
            if ( pNode )
            {
                // Grab the parent
                CElement* pParent = lua_toelement ( luaVM, 2 );
                if ( pParent )
                {
                    // Do it
                    CElement* pLoadedRoot = CStaticFunctionDefinitions::LoadMapData ( pLuaMain, pParent, pNode );
                    if ( pLoadedRoot )
                    {
                        lua_pushelement ( luaVM, pLoadedRoot );
                        return 1;
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "loadMapData", "element", 2 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "loadMapData", "xmlnode", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "loadMapData" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SaveMapData ( lua_State* luaVM )
{
    // Verify arguments
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the pointers
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        CElement* pParent = lua_toelement ( luaVM, 2 );
        if ( pNode && pParent )
        {
			bool bChildren = false;
			if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
                bChildren = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;

            CXMLNode* pSavedNode = CStaticFunctionDefinitions::SaveMapData ( pParent, pNode, bChildren );
            if ( pSavedNode )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "saveMapData", "element", 2 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "saveMapData" );

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
        // Grab the gametype string. Account the "MTA:SA: " prefix
        const char* szGameType = pASE->GetGameType ();
        if ( strlen ( szGameType ) > 8 ) 
        {
            // Return the gametype string excluding our prefix
            lua_pushstring ( luaVM, ( szGameType + 8 ) );
            return 1;
        }
    }

    // No game type
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::SetGameType ( lua_State* luaVM )
{
    // Grab the gametype if specified
    const char * szGameType = NULL;
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        // Grab the game type string.
        szGameType = lua_tostring ( luaVM, 1 );
    }

    // Set the game type.
    if ( CStaticFunctionDefinitions::SetGameType ( szGameType ) )
    {
        // Return true.
        lua_pushboolean ( luaVM, true );
	    return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetMapName ( lua_State* luaVM )
{
    // Grab the mapname if specified
    const char * szMapName = NULL;
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        // Grab the game type string.
        szMapName = lua_tostring ( luaVM, 1 );
    }

    // Set the game type.
    CStaticFunctionDefinitions::SetMapName ( szMapName );

    // Return true.
    lua_pushboolean ( luaVM, true );
	return 1;
}


int CLuaFunctionDefinitions::GetRuleValue ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        char* szKey = const_cast < char* > ( lua_tostring ( luaVM, 1 ) );

        char* szRule = CStaticFunctionDefinitions::GetRuleValue ( szKey );
        if ( szRule )
        {
            lua_pushstring ( luaVM, szRule );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getRuleValue" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetRuleValue ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        char* szKey = const_cast < char* > ( lua_tostring ( luaVM, 1 ) );
        char* szValue = NULL;

        if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
            szValue = const_cast < char* > ( lua_tostring ( luaVM, 2 ) );

        if ( CStaticFunctionDefinitions::SetRuleValue ( szKey, szValue ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setRuleValue" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemoveRuleValue ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        char* szKey = const_cast < char* > ( lua_tostring ( luaVM, 1 ) );

        if ( CStaticFunctionDefinitions::RemoveRuleValue ( szKey ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "removeRuleValue" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLCreateTable ( lua_State* luaVM )
{
	if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && lua_type ( luaVM, 2 ) == LUA_TSTRING )
	{
        CStaticFunctionDefinitions::ExecuteSQLCreateTable ( std::string ( lua_tostring ( luaVM, 1 ) ), std::string ( lua_tostring ( luaVM, 2 ) ) );
		lua_pushboolean ( luaVM, true );
		return 1;
	}
	else
        m_pScriptDebugging->LogBadType ( luaVM, "executeSQLCreateTable" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLDropTable ( lua_State* luaVM )
{
	if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
	{
        CStaticFunctionDefinitions::ExecuteSQLDropTable ( lua_tostring ( luaVM, 1 ) );
		lua_pushboolean ( luaVM, true );
		return 1;
	}
	else
        m_pScriptDebugging->LogBadType ( luaVM, "executeSQLDropTable" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLDelete ( lua_State* luaVM )
{
	bool bSuccess = false;

	if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && lua_type ( luaVM, 2 ) == LUA_TSTRING )
	{
        bSuccess = CStaticFunctionDefinitions::ExecuteSQLDelete ( std::string ( lua_tostring ( luaVM, 1 ) ), std::string ( lua_tostring ( luaVM, 2 ) ) );
		if ( !bSuccess ) {
			m_pScriptDebugging->LogError ( luaVM, "Database query failed: %s", CStaticFunctionDefinitions::SQLGetLastError ().c_str () );
		} else {
			lua_pushboolean ( luaVM, true );
			return 1;
		}
	}
	else
        m_pScriptDebugging->LogBadType ( luaVM, "executeSQLDelete" );

    lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLInsert ( lua_State* luaVM )
{
	bool bSuccess = false;

	if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && lua_type ( luaVM, 2 ) == LUA_TSTRING )
	{
        std::string strColumns;

		if ( lua_type ( luaVM, 3 ) == LUA_TSTRING )
            strColumns = std::string ( lua_tostring ( luaVM, 3 ) );

        bSuccess = CStaticFunctionDefinitions::ExecuteSQLInsert ( std::string ( lua_tostring ( luaVM, 1 ) ), std::string ( lua_tostring ( luaVM, 2 ) ), strColumns );
		if ( !bSuccess ) {
			m_pScriptDebugging->LogError ( luaVM, "Database query failed: %s", CStaticFunctionDefinitions::SQLGetLastError ().c_str () );
		} else {
			lua_pushboolean ( luaVM, true );
			return 1;
		}
	}
	else
        m_pScriptDebugging->LogBadType ( luaVM, "executeSQLInsert" );

    lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLQuery ( lua_State* luaVM )
{
	if ( lua_type ( luaVM, 1 ) == LUA_TSTRING ) {
		CLuaArguments Args;
		CRegistryResult Result;
        std::string strQuery = std::string ( lua_tostring ( luaVM, 1 ) );

		Args.ReadArguments ( luaVM, 2 );

		if ( CStaticFunctionDefinitions::ExecuteSQLQuery ( strQuery, &Args, &Result ) ) {
			lua_newtable ( luaVM );
			for ( int i = 0; i < Result.nRows; i++ ) {
				lua_newtable ( luaVM );								// new table
				lua_pushnumber ( luaVM, i+1 );						// row index number (starting at 1, not 0)
				lua_pushvalue ( luaVM, -2 );						// value
				lua_settable ( luaVM, -4 );						    // refer to the top level table
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
                            lua_pushnumber ( luaVM, cell.nVal );
                            break;
                        case SQLITE_FLOAT:
                            lua_pushnumber ( luaVM, cell.fVal );
                            break;
                        case SQLITE_BLOB:
                            lua_pushlstring ( luaVM, (char *)cell.pVal, cell.nLength );
                            break;
                        case SQLITE_TEXT:
                            lua_pushlstring ( luaVM, (char *)cell.pVal, cell.nLength - 1 );
                            break;
                        default:
                            lua_pushnil ( luaVM );
                    }
					lua_settable ( luaVM, -3 );
				}
				lua_pop ( luaVM, 1 );							    // pop the inner table
			}
			return 1;
		} else {
			m_pScriptDebugging->LogError ( luaVM, "Database query failed: %s", CStaticFunctionDefinitions::SQLGetLastError ().c_str () );
		}
	}
    else
        m_pScriptDebugging->LogBadType ( luaVM, "executeSQLQuery" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLSelect ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
		CRegistryResult Result;
        std::string strTable = std::string ( lua_tostring ( luaVM, 1 ) );
        std::string strColumns = std::string ( lua_tostring ( luaVM, 2 ) );
        std::string strWhere;
		unsigned int uiLimit = 0;

		if ( lua_type ( luaVM, 3 ) == LUA_TSTRING )
            strWhere = std::string ( lua_tostring ( luaVM, 3 ) );
		if ( lua_type ( luaVM, 4 ) == LUA_TNUMBER )
			uiLimit = static_cast < unsigned int > ( lua_tonumber ( luaVM, 4 ) );

		if ( CStaticFunctionDefinitions::ExecuteSQLSelect ( strTable, strColumns, strWhere, uiLimit, &Result ) )
        {
			lua_newtable ( luaVM );
			for ( int i = 0; i < Result.nRows; i++ ) {
				lua_newtable ( luaVM );								// new table
				lua_pushnumber ( luaVM, i+1 );						// row index number (starting at 1, not 0)
				lua_pushvalue ( luaVM, -2 );						// value
				lua_settable ( luaVM, -4 );						    // refer to the top level table
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
                            lua_pushnumber ( luaVM, cell.nVal );
                            break;
                        case SQLITE_FLOAT:
                            lua_pushnumber ( luaVM, cell.fVal );
                            break;
                        case SQLITE_BLOB:
                            lua_pushlstring ( luaVM, (char *)cell.pVal, cell.nLength );
                            break;
                        case SQLITE_TEXT:
                            lua_pushlstring ( luaVM, (char *)cell.pVal, cell.nLength - 1 );
                            break;
                        default:
                            lua_pushnil ( luaVM );
                    }
					lua_settable ( luaVM, -3 );
				}
				lua_pop ( luaVM, 1 );							    // pop the inner table
			}
			return 1;
		}
        else
        {
			m_pScriptDebugging->LogError ( luaVM, "Database query failed: %s", CStaticFunctionDefinitions::SQLGetLastError ().c_str () );
		}
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "executeSQLSelect" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ExecuteSQLUpdate ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && lua_type ( luaVM, 2 ) == LUA_TSTRING && lua_type ( luaVM, 3 ) == LUA_TSTRING )
    {
        std::string strTable    = std::string ( lua_tostring ( luaVM, 1 ) );
		std::string strSet      = std::string ( lua_tostring ( luaVM, 2 ) );
        std::string strWhere    = std::string ( lua_tostring ( luaVM, 3 ) );

        if ( CStaticFunctionDefinitions::ExecuteSQLUpdate ( strTable, strSet, strWhere ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
		} else {
			m_pScriptDebugging->LogError ( luaVM, "Database query failed: %s", CStaticFunctionDefinitions::SQLGetLastError ().c_str () );
		}
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "executeSQLUpdate" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetAccountName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CAccount* pAccount = lua_toaccount ( luaVM, 1 );
        if ( pAccount )
        {
            std::string strName = pAccount->GetName ();
            if ( !strName.empty () )
            {
                lua_pushstring ( luaVM, strName.c_str () );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getAccountName", "account", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getAccountName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetAccountPlayer ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CAccount* pAccount = lua_toaccount ( luaVM, 1 );
        if ( pAccount )
        {
            CClient* pClient = CStaticFunctionDefinitions::GetAccountPlayer ( pAccount );
            if ( pClient )
            {
                lua_pushelement ( luaVM, pClient->GetElement () );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getAccountPlayer", "account", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getAccountPlayer" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsGuestAccount ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CAccount* pAccount = lua_toaccount ( luaVM, 1 );
        if ( pAccount )
        {
            bool bGuest;
            if ( CStaticFunctionDefinitions::IsGuestAccount ( pAccount, bGuest ) )
            {
                lua_pushboolean ( luaVM, bGuest );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isGuestAccount", "account", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isGuestAccount" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetAccountData ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CAccount* pAccount = lua_toaccount ( luaVM, 1 );
        char* szKey = const_cast < char* > ( lua_tostring ( luaVM, 2 ) );
        if ( pAccount )
        {
            CLuaArgument * pArgument = CStaticFunctionDefinitions::GetAccountData ( pAccount, szKey );
            if ( pArgument )
            {
                pArgument->Push ( luaVM );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getAccountData", "account", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getAccountData" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetAccount ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING &&
			( lua_type ( luaVM, 2 ) == LUA_TSTRING || lua_type ( luaVM, 2 ) == LUA_TNONE ) )
        {
            const char* szName = lua_tostring ( luaVM, 1 );
			const char* szPassword = NULL;
			if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
				szPassword = lua_tostring ( luaVM, 2 );

			CAccount* pAccount = CStaticFunctionDefinitions::GetAccount ( szName, szPassword );
			if ( pAccount )
            {
                lua_pushaccount ( luaVM, pAccount );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getAccount" );
    }

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
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING &&
                lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            const char* szName = lua_tostring ( luaVM, 1 );
            const char* szPassword = lua_tostring ( luaVM, 2 );

            CAccount* pAccount;
            if ( pAccount = CStaticFunctionDefinitions::AddAccount ( szName, szPassword ) )
            {
                lua_pushaccount ( luaVM, pAccount );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "addAccount" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}
                     

int CLuaFunctionDefinitions::RemoveAccount ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CAccount* pAccount = lua_toaccount ( luaVM, 1 );
            if ( pAccount )
            {
                if ( CStaticFunctionDefinitions::RemoveAccount ( pAccount ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "removeAccount", "account", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "removeAccount" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetAccountPassword ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
                lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            CAccount* pAccount = lua_toaccount ( luaVM, 1 );
            const char* szPassword = lua_tostring ( luaVM, 2 );

            if ( pAccount )
            {
                if ( CStaticFunctionDefinitions::SetAccountPassword ( pAccount, szPassword ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setAccountPassword", "account", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "setAccountPassword" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetAccountData ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING &&
         lua_type ( luaVM, 3 ) != LUA_TNONE )
    {
        CAccount* pAccount = lua_toaccount ( luaVM, 1 );
        char* szKey = const_cast < char* > ( lua_tostring ( luaVM, 2 ) );
        CLuaArgument Variable;
        Variable.Read ( luaVM, 3 );
        if ( Variable.GetType () != LUA_TNONE )
        {
            if ( pAccount )
            {
                if ( CStaticFunctionDefinitions::SetAccountData ( pAccount, szKey, &Variable ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setAccountData", "account", 1 );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setAccountData" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CopyAccountData ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CAccount* pAccount = lua_toaccount ( luaVM, 1 );
        CAccount* pFromAccount = lua_toaccount ( luaVM, 2 );
        if ( pAccount )
        {
            if ( pFromAccount )
            {
                if ( CStaticFunctionDefinitions::CopyAccountData ( pAccount, pFromAccount ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "copyAccountData", "account", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "copyAccountData", "account", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "copyAccountData" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::LogIn ( lua_State* luaVM )
{
    // Verify argument types
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 3 ) == LUA_TSTRING )
    {
        // Grab player, account and password
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        CAccount* pAccount = lua_toaccount ( luaVM, 2 );
        const char* szPassword = lua_tostring ( luaVM, 3 );

        // Verify player
        if ( pPlayer )
        {
            // Verify account
            if ( pAccount )
            {
                // Log him in
                if ( CStaticFunctionDefinitions::LogIn ( pPlayer, pAccount, szPassword ) )
                {
                    // Success
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "logIn", "account", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "logIn", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "logIn" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::LogOut ( lua_State* luaVM )
{
    // Verify argument types
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA  )
    {
        // Grab player, account and password
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );

        // Verify player
        if ( pPlayer )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "logOut", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "logOut" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CancelEvent ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TBOOLEAN ||
         iArgument1 == LUA_TNONE )
    {       
        bool bCancel = true;

        if ( iArgument1 == LUA_TBOOLEAN ) 
            bCancel = ( lua_toboolean ( luaVM, 1 ) ) ? true:false;

		const char* szReason = NULL;
		if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
			szReason = lua_tostring ( luaVM, 2 );

        CStaticFunctionDefinitions::CancelEvent ( bCancel, szReason );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "cancelEvent" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetCancelReason ( lua_State* luaVM )
{
	char* szReason = NULL;
	if ( CStaticFunctionDefinitions::GetCancelReason ( szReason ) )
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
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
            CPlayer* pResponsible = NULL;
            const char* szReason = NULL;

            if ( lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
            {
                pResponsible = lua_toplayer ( luaVM, 2 );

				if ( lua_type ( luaVM, 3 ) == LUA_TSTRING )
					szReason = lua_tostring ( luaVM, 3 );
            }
            else if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
                szReason = lua_tostring ( luaVM, 2 );

            if ( pPlayer )
            {
                if ( CStaticFunctionDefinitions::KickPlayer ( pPlayer, pResponsible, szReason ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "kickPlayer", "player", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "kickPlayer" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::BanPlayer ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
            CPlayer* pResponsible = NULL;
            const char* szReason = NULL;

            bool bIP = true;
            bool bUsername = false;
            bool bSerial = false;

            if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            {
                bIP = ( lua_toboolean ( luaVM, 2 ) ) ? true : false;
            }

            if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
            {
                bUsername = ( lua_toboolean ( luaVM, 3 ) ) ? true : false;
            }

            if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
            {
                bSerial = ( lua_toboolean ( luaVM, 4 ) ) ? true : false;
            }

            if ( lua_type ( luaVM, 5 ) == LUA_TLIGHTUSERDATA )
            {
                pResponsible = lua_toplayer ( luaVM, 5 );
            }
            if ( lua_type ( luaVM, 6 ) == LUA_TSTRING )
                szReason = lua_tostring ( luaVM, 6 );

            time_t tUnban = 0;
            if ( lua_type ( luaVM, 7 ) == LUA_TNUMBER || lua_type ( luaVM, 7 ) == LUA_TSTRING )
            {
                tUnban = ( time_t ) atoi ( lua_tostring ( luaVM, 7 ) );
                if ( tUnban > 0 ) tUnban += time ( NULL );
            }

            if ( pPlayer )
            {
                CBan* pBan = NULL;
                if ( pBan = CStaticFunctionDefinitions::BanPlayer ( pPlayer, bIP, bUsername, bSerial, pResponsible, szReason, tUnban ) )
                {
                    lua_pushban ( luaVM, pBan );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "banPlayer", "player", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "banPlayer" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::AddBan ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TSTRING ) ||
         ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) ||
         ( lua_type ( luaVM, 3 ) == LUA_TSTRING ) )
    {
        const char* szIP = NULL;
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
        {
            szIP = lua_tostring ( luaVM, 1 );
        }

        const char* szUsername = NULL;
        if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            szUsername = lua_tostring ( luaVM, 2 );
        }

        const char* szSerial = NULL;
        if ( lua_type ( luaVM, 3 ) == LUA_TSTRING )
        {
            szSerial = lua_tostring ( luaVM, 3 );
        }

        CPlayer* pResponsible = NULL;
        if ( lua_type ( luaVM, 4 ) == LUA_TLIGHTUSERDATA )
        {
            pResponsible = lua_toplayer ( luaVM, 4 );
        }

        const char* szReason = NULL;
        if ( lua_type ( luaVM, 5 ) == LUA_TSTRING )
        {
            szReason = lua_tostring ( luaVM, 5 );
        }

        time_t tUnban = 0;
        if ( lua_type ( luaVM, 6 ) == LUA_TNUMBER || lua_type ( luaVM, 6 ) == LUA_TSTRING )
        {
            tUnban = ( time_t ) atoi ( lua_tostring ( luaVM, 6 ) );
            if ( tUnban > 0 ) tUnban += time ( NULL );
        }

        CBan* pBan = NULL;
		if ( pBan = CStaticFunctionDefinitions::AddBan ( szIP, szUsername, szSerial, pResponsible, szReason, tUnban ) )
        {
            lua_pushban ( luaVM, pBan );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "addBan" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemoveBan ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBan* pBan = lua_toban ( luaVM, 1 );

        if ( pBan )
        {
            CPlayer* pResponsible = NULL;
            if ( lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
            {
                pResponsible = lua_toplayer ( luaVM, 2 );
            }

            if ( CStaticFunctionDefinitions::RemoveBan ( pBan, pResponsible ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "removeBan" );

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
        
        CStaticFunctionDefinitions::GetBans ( pLuaMain );

        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBanIP ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBan* pBan = lua_toban ( luaVM, 1 );

        if ( pBan )
        {
            char szIP [32];
            if ( CStaticFunctionDefinitions::GetBanIP ( pBan, szIP, 31 ) )
            {
                lua_pushstring ( luaVM, szIP );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBanIP" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int	CLuaFunctionDefinitions::GetBanSerial ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBan* pBan = lua_toban ( luaVM, 1 );

        if ( pBan )
        {
            char szSerial [64];
            if ( CStaticFunctionDefinitions::GetBanSerial ( pBan, szSerial, 63 ) )
            {
                lua_pushstring ( luaVM, szSerial );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBanSerial" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int	CLuaFunctionDefinitions::GetBanUsername ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBan* pBan = lua_toban ( luaVM, 1 );

        if ( pBan )
        {
            char szUsername [32];
            if ( CStaticFunctionDefinitions::GetBanUsername ( pBan, szUsername, 31 ) )
            {
                lua_pushstring ( luaVM, szUsername );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBanUsername" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int	CLuaFunctionDefinitions::GetBanNick ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBan* pBan = lua_toban ( luaVM, 1 );

        if ( pBan )
        {
            char szNick [32];
            if ( CStaticFunctionDefinitions::GetBanNick ( pBan, szNick, 31 ) )
            {
                lua_pushstring ( luaVM, szNick );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBanNick" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int	CLuaFunctionDefinitions::GetBanTime ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBan* pBan = lua_toban ( luaVM, 1 );

        if ( pBan )
        {
            time_t tTime;
            if ( CStaticFunctionDefinitions::GetBanTime ( pBan, tTime ) )
            {
                lua_pushnumber ( luaVM, ( double ) tTime );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBanTime" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int	CLuaFunctionDefinitions::GetUnbanTime ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBan* pBan = lua_toban ( luaVM, 1 );

        if ( pBan )
        {
            time_t tTime;
            if ( CStaticFunctionDefinitions::GetUnbanTime ( pBan, tTime ) )
            {
                lua_pushnumber ( luaVM, ( double ) tTime );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getUnbanTime" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int	CLuaFunctionDefinitions::GetBanReason ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBan* pBan = lua_toban ( luaVM, 1 );

        if ( pBan )
        {
            char szReason [256];
            if ( CStaticFunctionDefinitions::GetBanReason ( pBan, szReason, 255 ) )
            {
                lua_pushstring ( luaVM, szReason );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBanReason" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int	CLuaFunctionDefinitions::GetBanAdmin ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CBan* pBan = lua_toban ( luaVM, 1 );

        if ( pBan )
        {
            char szAdmin [32];
            if ( CStaticFunctionDefinitions::GetBanAdmin ( pBan, szAdmin, 255 ) )
            {
                lua_pushstring ( luaVM, szAdmin );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBanAdmin" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsCursorShowing ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            bool bShowing;
            if ( CStaticFunctionDefinitions::IsCursorShowing ( pPlayer, bShowing ) )
            {
                lua_pushboolean ( luaVM, bShowing );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isCursorShowing", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isCursorShowing" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ShowCursor ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
            lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
        {
            CElement* pElement = lua_toelement ( luaVM, 1 );
            bool bShow = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
           
            if ( pElement )
            {
                bool bToggleControls = true;
                if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
                    bToggleControls = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;
                if ( CStaticFunctionDefinitions::ShowCursor ( pElement, pLuaMain, bShow, bToggleControls ) )
                {                  
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "showCursor", "element", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "showCursor" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ShowChat ( lua_State* luaVM )
{
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
		CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bShow = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
           
        if ( pElement )
        {
			if ( CStaticFunctionDefinitions::ShowChat ( pElement, bShow ) )
            {
				lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
			m_pScriptDebugging->LogBadPointer ( luaVM, "showChat", "element", 1 );
    }
    else
		m_pScriptDebugging->LogBadType ( luaVM, "showChat" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ResetMapInfo ( lua_State* luaVM )
{
    CElement* pElement = NULL;
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        pElement = lua_toelement ( luaVM, 1 );
        if ( !pElement )
        {
            m_pScriptDebugging->LogBadPointer ( luaVM, "resetMapInfo", "element", 1 );
            lua_pushboolean ( luaVM, false );
            return 1;
        }
    }

    if ( CStaticFunctionDefinitions::ResetMapInfo ( pElement ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    
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

	if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && lua_type ( luaVM, 2 ) > LUA_TNIL ) {
		CLuaArguments Args;
        for ( int i = 2; i <= lua_gettop ( luaVM ); i++ )
		    Args.ReadArgument ( luaVM, i );

        std::string strSetting = lua_tostring ( luaVM, 1 );
        std::string strResourceName = pResource->GetName ();
        std::string strJSON;
        Args.WriteToJSONString ( strJSON );

		if ( g_pGame->GetSettings ()->Set ( strResourceName.c_str (), strSetting.c_str (), strJSON.c_str () ) )
		{
			lua_pushboolean ( luaVM, true );
			return 1;
		}
		else
			m_pScriptDebugging->LogWarning ( luaVM, "Resource '%s' cannot access setting '%s'", strResourceName.c_str (), strSetting.c_str () );
	} else
		m_pScriptDebugging->LogBadType ( luaVM, "set" );

	lua_pushboolean ( luaVM, false );
	return 1;
}

#define PUSH_SETTING(x,buf) \
	pAttributes = &(x->GetAttributes ()); \
	Args.PushString ( pAttributes->Find ( "name" )->GetValue ().c_str () ); \
	buf = const_cast < char* > ( pAttributes->Find ( "value" )->GetValue ().c_str () ); \
	if ( !Args.ReadFromJSONString ( buf ) ) { \
		Args.PushString ( buf ); \
	}

int CLuaFunctionDefinitions::Get ( lua_State* luaVM )
{
	CResource* pResource = m_pResourceManager->GetResourceFromLuaState ( luaVM );

	if ( lua_type ( luaVM, 1 ) == LUA_TSTRING ) {
		CLuaArguments Args;
		CXMLAttributes *pAttributes;
		unsigned int uiIndex = 0;
		bool bDeleteNode;

		// Get the setting
		CXMLNode *pSubNode, *pNode = g_pGame->GetSettings ()->Get ( pResource->GetName ().c_str (), lua_tostring ( luaVM, 1 ), bDeleteNode );

		// Only proceed if we have a valid node
		if ( pNode ) {
            // Argument count
            unsigned int uiArgCount = 1;

			// See if we need to return a table with single or multiple entries
			if ( pNode->GetSubNodeCount () == 0 ) {
				// We only have a single entry for a specific setting, so output a string
				char *szDataValue = const_cast < char* > ( pNode->GetAttributes().Find ( "value" )->GetValue ().c_str () );
				if ( !Args.ReadFromJSONString ( szDataValue ) ) {
					// No valid JSON? Parse as plain text
					Args.PushString ( szDataValue );
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
				char *szDataValue;
				while ( pSubNode = pNode->FindSubNode ( "setting", uiIndex++ ) ) {
					PUSH_SETTING ( pSubNode, szDataValue );
				}
				// Push a table and return
				Args.PushAsTable ( luaVM );
			}

			// Check if we have to delete the node
			if ( bDeleteNode )
				delete pNode;

			return uiArgCount;
		}
	} else
		m_pScriptDebugging->LogBadType ( luaVM, "get" );

	lua_pushboolean ( luaVM, false );
	return 1;
}

int CLuaFunctionDefinitions::Md5 ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING || lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        MD5 md5bytes;
        char szResult[33];
        CMD5Hasher hasher;
        hasher.Calculate ( lua_tostring ( luaVM, 1 ), lua_objlen ( luaVM, 1 ), md5bytes );
		hasher.ConvertToHex ( md5bytes, szResult );
        lua_pushstring ( luaVM, szResult );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogBadType ( luaVM, "md5" );
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPacketInfo ( lua_State* luaVM )
{
    unsigned long ulBytes [ 256 ];
    unsigned long ulCount [ 256 ];
    g_pNetServer->GetPacketLogData ( ulBytes, ulCount );
    lua_createtable ( luaVM, 256, 1 );

    for ( unsigned int i = 0; i < 256; ++i )
    {
        lua_createtable ( luaVM, 0, 2 );

        lua_pushstring ( luaVM, "bytes" );
        lua_pushnumber ( luaVM, ulBytes [ i ] );
        lua_rawset ( luaVM, -3 );

        lua_pushstring ( luaVM, "count" );
        lua_pushnumber ( luaVM, ulCount [ i ] );
        lua_rawset ( luaVM, -3 );

        lua_rawseti ( luaVM, -2, i );
    }

    return 1;
}


int CLuaFunctionDefinitions::GetVersion ( lua_State* luaVM )
{
    lua_createtable ( luaVM, 0, 5 );

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

    return 1;
}

int CLuaFunctionDefinitions::GetModuleInfo ( lua_State* luaVM )
{
    if (lua_type( luaVM, 1 ) == LUA_TSTRING) {
        vector < FunctionInfo > func_LoadedModules = m_pLuaModuleManager->GetLoadedModules();
        vector < FunctionInfo > ::iterator iter = func_LoadedModules.begin ();
        SString strAttribute = lua_tostring( luaVM, 2 );
        SString strModuleName = lua_tostring( luaVM, 1 );
        for ( ; iter != func_LoadedModules.end (); iter++ )
        {
            if ( stricmp ( strModuleName, (*iter).szFileName ) == 0 ) {
                lua_newtable ( luaVM );

                lua_pushstring ( luaVM, "name" );
                lua_pushstring ( luaVM, (*iter).szModuleName );
                lua_settable ( luaVM, -3 );

                lua_pushstring ( luaVM, "author" );
                lua_pushstring ( luaVM, (*iter).szAuthor );
                lua_settable ( luaVM, -3 );

                lua_pushstring ( luaVM, "version" );
                SString strVersion ( "%.2f", (*iter).fVersion );
                lua_pushstring ( luaVM, strVersion );
                lua_settable ( luaVM, -3 );

                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    m_pScriptDebugging->LogBadType ( luaVM, "getModuleInfo" );
    return 1;
}

int CLuaFunctionDefinitions::GetModules ( lua_State* luaVM )
{
    lua_newtable ( luaVM );
    vector < FunctionInfo > func_LoadedModules = m_pLuaModuleManager->GetLoadedModules();
    vector < FunctionInfo > ::iterator iter = func_LoadedModules.begin ();
    unsigned int uiIndex = 1;
    for ( ; iter != func_LoadedModules.end (); iter++ )
    {
        lua_pushnumber ( luaVM, uiIndex++ );
        lua_pushstring ( luaVM, (*iter).szFileName );
        lua_settable ( luaVM, -3 );
    }
    return 1;
}
