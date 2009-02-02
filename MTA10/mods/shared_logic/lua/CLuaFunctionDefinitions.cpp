/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefinitions.cpp
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

#include <StdInc.h>

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
		ref = lua_tonumber ( L, -1 );
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


static CLuaManager*			        m_pLuaManager;
static CScriptDebugging*            m_pScriptDebugging;
static CClientGame*			        m_pClientGame;
static CClientManager*              m_pManager;
static CClientEntity*		        m_pRootEntity;
static CClientGUIManager*	        m_pGUIManager;
static CClientPedManager*           m_pPedManager;
static CClientPlayerManager*        m_pPlayerManager;
static CClientRadarMarkerManager*   m_pRadarMarkerManager;
static CResourceManager*            m_pResourceManager;
static CClientVehicleManager*       m_pVehicleManager;
static CClientColManager*           m_pColManager;
static CClientObjectManager*        m_pObjectManager;
static CClientTeamManager*          m_pTeamManager;
static CRenderWare*					m_pRenderWare;
static CClientMarkerManager*        m_pMarkerManager;
static CClientPickupManager*        m_pPickupManager;
static CClientDFFManager*           m_pDFFManager;
static CClientColModelManager*      m_pColModelManager;
static CRegisteredCommands*         m_pRegisteredCommands;
static CSFX*						m_pSFX;

#define lua_istype(luavm, number,type) (lua_type(luavm,number) == type)

// Ignore C4312 warning. It's because we cast from long to void* for LUA
#pragma warning(disable:4312)

CLuaFunctionDefinitions::CLuaFunctionDefinitions ( CLuaManager* pLuaManager, CScriptDebugging* pScriptDebugging, CClientGame* pClientGame )
{
    m_pLuaManager = pLuaManager;
    m_pScriptDebugging = pScriptDebugging;
    m_pClientGame = pClientGame;
    m_pManager = pClientGame->GetManager ();
    m_pRootEntity = m_pClientGame->GetRootEntity ();
    m_pGUIManager = m_pClientGame->GetGUIManager ();
    m_pPedManager = m_pClientGame->GetPedManager ();
    m_pPlayerManager = m_pClientGame->GetPlayerManager ();
    m_pRadarMarkerManager = m_pClientGame->GetRadarMarkerManager ();
    m_pResourceManager = m_pClientGame->GetResourceManager ();
    m_pColManager = m_pManager->GetColManager ();
    m_pVehicleManager = m_pManager->GetVehicleManager ();
    m_pObjectManager = m_pManager->GetObjectManager ();
    m_pTeamManager = m_pManager->GetTeamManager ();
	m_pRenderWare = g_pGame->GetRenderWare ();
    m_pMarkerManager = m_pManager->GetMarkerManager ();
    m_pPickupManager = m_pManager->GetPickupManager ();
    m_pDFFManager = m_pManager->GetDFFManager ();
    m_pColModelManager = m_pManager->GetColModelManager ();
    m_pRegisteredCommands = m_pClientGame->GetRegisteredCommands ();
	m_pSFX = pClientGame->GetSFX ();
}


int CLuaFunctionDefinitions::DisabledFunction ( lua_State* luaVM )
{
	m_pScriptDebugging->LogError ( luaVM, "Unsafe function was called." );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::Call ( lua_State* luaVM )
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
                    // The function name
                    const char* szFunctionName = lua_tostring ( luaVM, 2 );

                    // Read out the vargs
                    CLuaArguments args;
                    args.ReadArguments ( luaVM, 3 );
                    CLuaArguments returns;

                    // Call the exported function with the given name and the args
                    if ( pResource->CallExportedFunction ( szFunctionName, args, returns, *pThisResource ) )
                    {
                        // Push return arguments
                        returns.PushArguments ( luaVM );
                        return returns.Count ();
                    }
                    else
				    {
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


int CLuaFunctionDefinitions::GetThisResource ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GetResourceConfig ( lua_State* luaVM )
{
    // Resource and config name
    CResource* pResource = NULL;
    const char* szConfigName = NULL;

    // We have resource, string args passed?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TSTRING ) )
    {
        // Grab the arguments
        pResource = lua_toresource ( luaVM, 1 );
        szConfigName = lua_tostring ( luaVM, 2 );
    }

    // We have only string?
    else if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
              lua_istype ( luaVM, 2, LUA_TNONE ) )
    {
        // Grab our lua main
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Grab resource and the config name from arg
            pResource = pLuaMain->GetResource ();
            szConfigName = lua_tostring ( luaVM, 1 );
        }
    }

    // We have both a resource file to grab the config from and a config name?
    if ( pResource && szConfigName )
    {
        // Loop through the configs in that resource
        list < CResourceConfigItem* >::iterator iter = pResource->ConfigIterBegin ();
        for ( ; iter != pResource->ConfigIterEnd (); iter++ )
        {
            // Matching name?
            if ( strcmp ( (*iter)->GetShortName(), szConfigName ) == 0 )
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
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getResourceConfig" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetResourceName ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GetResourceFromName ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GetResourceRootElement ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GetResourceGUIElement ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::AddEvent ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Verify the arguments
        if ( lua_istype ( luaVM, 1, LUA_TSTRING )  )
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
            if ( CStaticFunctionDefinitions::AddEvent ( *pLuaMain, szName, bAllowRemoteTrigger ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::AddEventHandler ( lua_State* luaVM )
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Verify the arguments
        if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
             lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) &&
			 lua_istype ( luaVM, 3, LUA_TFUNCTION ) )
        {
            // Grab propagated flag if any
            bool bPropagated = true;
            if ( lua_istype ( luaVM, 4, LUA_TBOOLEAN ) )
                bPropagated = ( lua_toboolean ( luaVM, 4 ) ) ? true:false;

            // Grab the arguments
            const char* szName = lua_tostring ( luaVM, 1 );
            CClientEntity* pEntity = lua_toelement ( luaVM, 2 );
            int iLuaFunction = luaM_toref ( luaVM, 3 );

            // Verify the element
            if ( pEntity )
            {
                // Verify the function
                if ( VERIFY_FUNCTION ( iLuaFunction ) )
                {
                    // Check if the handle is in use
                    if ( pEntity->GetEventManager()->HandleExists ( pLuaMain, szName, iLuaFunction ) )
                    {
                        m_pScriptDebugging->LogCustom ( luaVM, 255, 0, 0, "addEventHandler: %s is already handled", szName );
                        lua_pushboolean ( luaVM, false );
                        return 1;
                    }

                    // Do it
                    if ( CStaticFunctionDefinitions::AddEventHandler ( *pLuaMain, const_cast < char * > ( szName ), *pEntity, iLuaFunction, bPropagated ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "addEventHandler", "function", 3 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "addEventHandler", "element", 2 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "addEventHandler" );
    }

    // Failed
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
        if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
             lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) &&
             lua_istype ( luaVM, 3, LUA_TFUNCTION ) )
        {
            // Grab the arguments
            const char* szName = lua_tostring ( luaVM, 1 );
            CClientEntity* pEntity = lua_toelement ( luaVM, 2 );
			int iLuaFunction = luaM_toref ( luaVM, 3 );

            // Verify the element
            if ( pEntity )
            {
                // Verify the function
                if ( VERIFY_FUNCTION ( iLuaFunction ) )
                {
                    // Do it
                    if ( CStaticFunctionDefinitions::RemoveEventHandler ( *pLuaMain, const_cast < char * > ( szName ), *pEntity, iLuaFunction ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "removeEventHandler", "function", 3 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "removeEventHandler", "element", 2 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "removeEventHandler" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::TriggerEvent ( lua_State* luaVM )
{
    // Verify the arguments
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
         lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the name and the element
        const char* szName = lua_tostring ( luaVM, 1 );
        CClientEntity* pEntity = lua_toelement ( luaVM, 2 );

        // Read out the additional arguments to pass to the event
        CLuaArguments Arguments;
        Arguments.ReadArguments ( luaVM, 3 );

        // Verify the element
        if ( pEntity )
        {
            // Trigger it
            bool bWasCancelled;
            if ( CStaticFunctionDefinitions::TriggerEvent ( szName, *pEntity, Arguments, bWasCancelled ) )
            {
                lua_pushboolean ( luaVM, !bWasCancelled );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "triggerEvent", "element", 2 );
    }

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::TriggerServerEvent ( lua_State* luaVM )
{
    // Verify argument types
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
         lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) )
    {
        // Grab arguments
        const char* szName = lua_tostring ( luaVM, 1 );
        CClientEntity* pCallWithEntity = lua_toelement ( luaVM, 2 );
        CLuaArguments Arguments;
        Arguments.ReadArguments ( luaVM, 3 );

        // Check entity
        if ( pCallWithEntity )
        {
            // Trigger it
            if ( CStaticFunctionDefinitions::TriggerServerEvent ( szName, *pCallWithEntity, Arguments ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "triggerServerEvent", "element", 2 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "triggerServerEvent" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CancelEvent ( lua_State* luaVM )
{
    // Cancel it
    if ( CStaticFunctionDefinitions::CancelEvent ( true ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::EngineLoadCOL ( lua_State* luaVM )
{
    // Grab the lua main and the resource belonging to this script
	CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Get the resource we belong to
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            // Grab the filename
	        const char* szFile = ( lua_istype ( luaVM, 1, LUA_TSTRING ) ? lua_tostring ( luaVM, 1 ) : NULL );

            // Is this a legal filepath?
	        if ( szFile && IsValidFilePath ( szFile ) )
            {
                // Generate the full path to the file
		        char szPath [MAX_PATH+1] = {0};
		        snprintf ( szPath, MAX_PATH, "%s/resources/%s/%s", m_pClientGame->GetModRoot (), pLuaMain->GetResource ()->GetName (), szFile );

                // Grab the resource root entity
                CClientEntity* pRoot = pResource->GetResourceCOLModelRoot ();

                // Create the col model
                CClientColModel* pCol = new CClientColModel ( m_pManager, INVALID_ELEMENT_ID );

                // Attempt loading the file
                if ( pCol->LoadCol ( szPath ) )
                {
                    // Success. Make it a child of the resource collision root
                    pCol->SetParent ( pRoot );

                    // Return the created col model
			        lua_pushelement ( luaVM, pCol );
			        return 1;
                }
                else
                {
                    // Delete it again. We failed
                    delete pCol;
                }
		    }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "engineLoadCOL" );
	    }
    }

	// We failed for some reason
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::EngineLoadDFF ( lua_State* luaVM )
{
    // Grab our virtual machine and grab our resource from that.
	CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Get this resource
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            // Grab the filename and the model ID
	        const char* szFile = ( lua_istype ( luaVM, 1, LUA_TSTRING ) ? lua_tostring ( luaVM, 1 ) : NULL );

            // Is it a valid filepath?
	        if ( szFile && lua_istype ( luaVM, 2, LUA_TNUMBER ) && IsValidFilePath ( szFile ) )
            {
                // Check the model ID
		        unsigned short usModelID = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
                if ( usModelID == 0 || CClientDFFManager::IsReplacableModel ( usModelID ) )
                {
                    // Grab the path to resource root
		            char szPath[MAX_PATH+1] = {0};
		            snprintf ( szPath, MAX_PATH, "%s/resources/%s/%s", m_pClientGame->GetModRoot (), pResource->GetName (), szFile );

                    // Grab the resource root entity
                    CClientEntity* pRoot = pResource->GetResourceDFFRoot ();

                    // Create a DFF element
                    CClientDFF* pDFF = new CClientDFF ( m_pManager, INVALID_ELEMENT_ID );

                    // Try to load the DFF file
                    if ( pDFF->LoadDFF ( szPath, usModelID ) )
                    {
                        // Success loading the file. Set parent to DFF root
                        pDFF->SetParent ( pRoot );

                        // Return the DFF
			            lua_pushelement ( luaVM, pDFF );
			            return 1;
                    }
                    else
                    {
                        // Delete it again
                        delete pDFF;
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "engineLoadDFF", "number", 2 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "engineLoadDFF", "string", 1 );
        }
    }

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::EngineLoadTXD ( lua_State* luaVM )
{
    // Grab our virtual machine and grab our resource from that.
	CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Grab this resource
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            // Grab the filename
	        const char* szFile = ( lua_istype ( luaVM, 1, LUA_TSTRING ) ? lua_tostring ( luaVM, 1 ) : NULL );

            // Is it a valid filepath?
	        if ( szFile && IsValidFilePath ( szFile ) )
            {
                // Grab the path to resource root
		        char szPath[MAX_PATH+1] = {0};
		        snprintf ( szPath, MAX_PATH, "%s/resources/%s/%s", m_pClientGame->GetModRoot (), pResource->GetName (), szFile );

                // Grab the resource root entity
                CClientEntity* pRoot = pResource->GetResourceTXDRoot ();

                // Create a TXD element
                CClientTXD* pTXD = new CClientTXD ( m_pManager, INVALID_ELEMENT_ID );

                // Try to load the TXD file
                if ( pTXD->LoadTXD ( szPath ) )
                {
                    // Success loading the file. Set parent to TXD root
                    pTXD->SetParent ( pRoot );

                    // Return the TXD
			        lua_pushelement ( luaVM, pTXD );
			        return 1;
                }
                else
                {
                    // Delete it again
                    delete pTXD;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "engineLoadTXD", "string", 1 );
        }
    }

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::EngineReplaceCOL ( lua_State* luaVM )
{
    // Grab the DFF and model ID
	CClientColModel* pCol = ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) ? lua_tocolmodel ( luaVM, 1 ) : NULL );
	unsigned short usModel = ( lua_istype ( luaVM, 2, LUA_TNUMBER ) ? ( static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) ) ) : 0 );

    // Valid collision model?
    if ( pCol )
    {
        // Valid client DFF and model?
        if ( CClientColModelManager::IsReplacableModel ( usModel ) )
        {
            // Replace the colmodel
            if ( pCol->Replace ( usModel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "engineReplaceCOL", "number", 2 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "engineReplaceCOL", "col", 1 );

    // Failed
    lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::EngineRestoreCOL ( lua_State* luaVM )
{
    // Grab the model ID we're going to restore
	unsigned short usModelID = ( lua_istype ( luaVM, 1, LUA_TNUMBER ) ? ( static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) ) ) : 0 );

    // Replacable col model ID?
    if ( CClientColModelManager::IsReplacableModel ( usModelID ) )
    {
        // Restore it
        m_pColModelManager->RestoreModel ( usModelID );

        // Success
		lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
		m_pScriptDebugging->LogBadType ( luaVM, "engineRestoreCOL" );

    // Failed.
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::EngineImportTXD ( lua_State* luaVM )
{
    // Grab the TXD and the model ID
	CClientTXD* pTXD = ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) ? lua_totxd ( luaVM, 1 ) : NULL );
	unsigned short usModelID = ( lua_istype ( luaVM, 2, LUA_TNUMBER ) ? ( static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) ) ) : 0 );

    // Valid txd?
    if ( pTXD )
    {
        // Valid importable model?
        if ( CClientTXD::IsImportableModel ( usModelID ) )
        {
            // Try to import
            if ( pTXD->Import ( usModelID ) )
            {
                // Success
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "engineImportTXD", "number", 2 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "engineImportTXD", "txd", 1 );

    // Failed
    lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::EngineReplaceModel ( lua_State* luaVM )
{
    // Grab the DFF and model ID
	CClientDFF* pDFF = ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) ? lua_todff ( luaVM, 1 ) : NULL );
	unsigned short usModelID = ( lua_istype ( luaVM, 2, LUA_TNUMBER ) ? ( static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) ) ) : 0 );

    // Valid client DFF?
    if ( pDFF )
    {
        // Valid model?
        if ( CClientDFFManager::IsReplacableModel ( usModelID ) )
        {
            // Replace the model
		    pDFF->ReplaceModel ( usModelID );

            // Success
		    lua_pushboolean ( luaVM, true );
            return true;
        }
        else
		    m_pScriptDebugging->LogBadPointer ( luaVM, "engineReplaceModel", "number", 2 );
	}
    else
		m_pScriptDebugging->LogBadPointer ( luaVM, "engineReplaceModel", "dff", 1 );

    // Failure
    lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::EngineRestoreModel ( lua_State* luaVM )
{
    // Grab the model ID
	unsigned short usModelID = ( lua_istype ( luaVM, 1, LUA_TNUMBER ) ? ( static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) ) ) : 0 );

    // Valid client DFF and model?
    if ( CClientDFFManager::IsReplacableModel ( usModelID )  )
    {
        // Restore the model
		if ( m_pDFFManager->RestoreModel ( usModelID ) )
        {
            // Success
		    lua_pushboolean ( luaVM, true );
            return true;
        }
	}
    else
    {
		m_pScriptDebugging->LogBadType ( luaVM, "engineRestoreModel" );
	}

    // Failure
    lua_pushboolean ( luaVM, false );
	return 1;
}


// TODO: int CLuaFunctionDefinitions::EngineReplaceMatchingAtomics ( lua_State* luaVM )
int CLuaFunctionDefinitions::EngineReplaceMatchingAtomics ( lua_State* luaVM )
{
    /*
	RpClump * pClump = ( lua_istype ( 1, LUA_TLIGHTUSERDATA ) ? reinterpret_cast < RpClump* > ( lua_touserdata ( luaVM, 1 ) ) : NULL );
	CClientEntity* pEntity = ( lua_istype ( 2, LUA_TLIGHTUSERDATA ) ? reinterpret_cast < CClientEntity* > ( lua_touserdata ( luaVM, 2 ) ) : NULL );
	RpAtomicContainer Atomics[MAX_ATOMICS_PER_CLUMP];
	RpClump * pEntityClump = NULL;
	unsigned int uiAtomics = 0;

	if ( pEntity ) {
		if ( IS_VEHICLE ( pEntity ) )
			pEntityClump = static_cast < CClientVehicle* > ( pEntity ) -> GetGameVehicle () -> GetRpClump ();
		else if ( IS_OBJECT ( pEntity ) )
			pEntityClump = static_cast < CClientObject* > ( pEntity ) -> GetGameObject () -> GetRpClump ();
		else {
			m_pScriptDebugging->LogWarning ( luaVM, "engineReplaceMatchingAtomics only supports vehicles and objects." );
			m_pScriptDebugging->LogBadType ( luaVM, "engineReplaceMatchingAtomics" );
		}
	}

	if ( pEntityClump && pClump ) {
		uiAtomics = m_pRenderWare->LoadAtomics ( pClump, &Atomics[0] );
		m_pRenderWare->ReplaceAllAtomicsInClump ( pEntityClump, &Atomics[0], uiAtomics );

		lua_pushboolean ( luaVM, true );
	} else {
		m_pScriptDebugging->LogBadType ( luaVM, "engineReplaceMatchingAtomics" );
		lua_pushboolean ( luaVM, false );
	}
    */
	
    lua_pushboolean ( luaVM, false );
	return 1;
}


// TODO: int CLuaFunctionDefinitions::EngineReplaceWheelAtomics ( lua_State* luaVM )
int CLuaFunctionDefinitions::EngineReplaceWheelAtomics ( lua_State* luaVM )
{
    /*
	RpClump * pClump = ( lua_istype ( 1, LUA_TLIGHTUSERDATA ) ? reinterpret_cast < RpClump* > ( lua_touserdata ( luaVM, 1 ) ) : NULL );
	CClientEntity* pEntity = ( lua_istype ( 2, LUA_TLIGHTUSERDATA ) ? reinterpret_cast < CClientEntity* > ( lua_touserdata ( luaVM, 2 ) ) : NULL );
	const char * szWheel = ( lua_istype ( 3, LUA_TSTRING ) ? lua_tostring ( luaVM, 3 ) : NULL );

	RpAtomicContainer Atomics[MAX_ATOMICS_PER_CLUMP];
	RpClump * pEntityClump = NULL;
	unsigned int uiAtomics = 0;

	if ( pEntity ) {
		if ( IS_VEHICLE ( pEntity ) )
			pEntityClump = static_cast < CClientVehicle* > ( pEntity ) -> GetGameVehicle () -> GetRpClump ();
		else if ( IS_OBJECT ( pEntity ) )
			pEntityClump = static_cast < CClientObject* > ( pEntity ) -> GetGameObject () -> GetRpClump ();
		else {
			m_pScriptDebugging->LogWarning ( luaVM, "engineReplaceWheelAtomics only supports vehicles and objects." );
		}
	}

	if ( pClump ) {
		uiAtomics = m_pRenderWare->LoadAtomics ( pClump, &Atomics[0] );
		m_pScriptDebugging->LogWarning ( luaVM, "engineReplaceWheelAtomics DFF argument was not valid." );
	}

	if ( pEntityClump && uiAtomics > 0 && szWheel ) {	
		m_pRenderWare->ReplaceWheels ( pEntityClump, &Atomics[0], uiAtomics, szWheel );

		lua_pushboolean ( luaVM, true );
	} else {
		m_pScriptDebugging->LogBadType ( luaVM, "engineReplaceWheelAtomics" );
		lua_pushboolean ( luaVM, false );
	}
    */

    lua_pushboolean ( luaVM, false );
	return 1;
}


// TODO: int CLuaFunctionDefinitions::EnginePositionAtomic ( lua_State* luaVM )
int CLuaFunctionDefinitions::EnginePositionAtomic ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
	return 1;
}


// TODO: int CLuaFunctionDefinitions::EnginePositionSeats ( lua_State* luaVM )
int CLuaFunctionDefinitions::EnginePositionSeats ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
	return 1;
}


// TODO: int CLuaFunctionDefinitions::EngineAddAllAtomics ( lua_State* luaVM )
int CLuaFunctionDefinitions::EngineAddAllAtomics ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
	return 1;
}


// TODO: int CLuaFunctionDefinitions::EngineReplaceVehiclePart ( lua_State* luaVM )
int CLuaFunctionDefinitions::EngineReplaceVehiclePart ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::dxDrawLine ( lua_State* luaVM )
{
    // dxDrawLine ( int x,int y,int x2,int y2,int color, [float width=1,bool postGUI=false] )
    // Grab all argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    int iArgument7 = lua_type ( luaVM, 7 );

    // Check required arguments. Should all be numbers.
    if ( ( iArgument1 != LUA_TNUMBER && iArgument1 != LUA_TSTRING ) ||
         ( iArgument2 != LUA_TNUMBER && iArgument2 != LUA_TSTRING ) || 
         ( iArgument3 != LUA_TNUMBER && iArgument3 != LUA_TSTRING ) ||
         ( iArgument4 != LUA_TNUMBER && iArgument4 != LUA_TSTRING ) ||
         ( iArgument5 != LUA_TNUMBER && iArgument5 != LUA_TSTRING ) )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "dxDrawLine" );
        lua_pushboolean ( luaVM, false );
        return 1;
    }    

    // Got a line width too?
    float fWidth = 1.0f;
    if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
    {
        fWidth = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
    }

    // Got a post gui specifier?
    bool bPostGUI = false;
    if ( iArgument7 == LUA_TBOOLEAN )
    {
        bPostGUI = ( lua_toboolean ( luaVM, 7 ) ) ? true:false;
    }

    // Grab the arguments
    float fX1 = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
    float fY1 = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
    float fX2 = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
    float fY2 = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
    unsigned long ulColor = static_cast < unsigned long > ( lua_tonumber ( luaVM, 5 ) );

    // Draw it
    g_pCore->GetGraphics ()->DrawLineQueued ( fX1, fY1, fX2, fY2,fWidth, ulColor, bPostGUI );

    // Success
    lua_pushboolean ( luaVM, true );
    return 1;
}


int CLuaFunctionDefinitions::dxDrawLine3D ( lua_State* luaVM )
{
    // dxDrawLine3D ( float x,float y,float z,float x2,float y2,float z2,int color, [float width,bool postGUI,float zBuffer] )

    // Grab all argument types
    int iArgument1 = lua_type ( luaVM, 1 ); // X1
    int iArgument2 = lua_type ( luaVM, 2 ); // Y1
    int iArgument3 = lua_type ( luaVM, 3 ); // Z1
    int iArgument4 = lua_type ( luaVM, 4 ); // X2
    int iArgument5 = lua_type ( luaVM, 5 ); // Y2
    int iArgument6 = lua_type ( luaVM, 6 ); // Z2
    int iArgument7 = lua_type ( luaVM, 7 ); // Color
    int iArgument8 = lua_type ( luaVM, 8 ); // Width
    int iArgument9 = lua_type ( luaVM, 9 ); // Reserved: Post GUI
    int iArgument10 = lua_type ( luaVM, 9 ); // Reserved: Z-buffer

    // Check required arguments. Should all be numbers.
    if ( ( iArgument1 != LUA_TNUMBER && iArgument1 != LUA_TSTRING ) ||
         ( iArgument2 != LUA_TNUMBER && iArgument2 != LUA_TSTRING ) || 
         ( iArgument3 != LUA_TNUMBER && iArgument3 != LUA_TSTRING ) ||
         ( iArgument4 != LUA_TNUMBER && iArgument4 != LUA_TSTRING ) ||
         ( iArgument5 != LUA_TNUMBER && iArgument5 != LUA_TSTRING ) ||
         ( iArgument6 != LUA_TNUMBER && iArgument6 != LUA_TSTRING ) ||
         ( iArgument7 != LUA_TNUMBER && iArgument7 != LUA_TSTRING ))
    {
        m_pScriptDebugging->LogBadType ( luaVM, "dxDrawLine" );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    // Got a line width too?
    float fWidth = 1.0f;
    if ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING )
    {
        fWidth = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );
    }

	// Got a post gui specifier?
    bool bPostGUI = false;
    if ( iArgument9 == LUA_TBOOLEAN )
    {
        bPostGUI = ( lua_toboolean ( luaVM, 9 ) ) ? true:false;
    }

    // Grab the arguments
    CVector vecBegin, vecEnd;
    vecBegin.fX = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
    vecBegin.fY = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
    vecBegin.fZ = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
    vecEnd.fX = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
    vecEnd.fY = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );
    vecEnd.fZ = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
    unsigned long ulColor = static_cast < unsigned long > ( lua_tonumber ( luaVM, 7 ) );

    // Draw it
    g_pCore->GetGraphics ()->DrawLine3DQueued ( vecBegin, vecEnd, fWidth, ulColor, bPostGUI );

    // Success
    lua_pushboolean ( luaVM, true );
    return 1;
}


int CLuaFunctionDefinitions::dxDrawText ( lua_State* luaVM )
{
    // dxDrawText ( string text,int left,int top [,int right=left,int bottom=top,int color=0xffffffff,float scale=1,string font="default",string alignX="left",string alignY="top",bool clip=false,bool wordBreak=false] )

    // Grab all argument types
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) && 
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        const char * szText = lua_tostring ( luaVM, 1 );
        int iLeft = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
        int iTop = static_cast < int > ( lua_tonumber ( luaVM, 3 ) );
        int iRight = iLeft;
        int iBottom = iTop;        
        unsigned long ulColor = 0xFFFFFFFF;
        float fScale = 1.0f;
        eFontType fontType = FONT_DEFAULT;
        unsigned long ulFormat = 0;
        bool bClip = false;
		bool bPostGUI = false;
        
        int iArgument4 = lua_type ( luaVM, 4 );
        if ( ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
        {
            iRight = static_cast < int > ( lua_tonumber ( luaVM, 4 ) );

            int iArgument5 = lua_type ( luaVM, 5 );
            if ( ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
            {
                iBottom = static_cast < int > ( lua_tonumber ( luaVM, 5 ) );

                int iArgument6 = lua_type ( luaVM, 6 );
                if ( ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
                {
                    ulColor = static_cast < unsigned long > ( lua_tonumber ( luaVM, 6 ) );

                    int iArgument7 = lua_type ( luaVM, 7 );
                    if ( ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING ) )
                    {
                        fScale = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );

                        int iArgument8 = lua_type ( luaVM, 8 );
                        if ( iArgument8 == LUA_TSTRING )
                        {
                            const char * szFontName = lua_tostring ( luaVM, 8 );
                            fontType = g_pCore->GetGraphics ()->GetFontType ( const_cast < char * > ( szFontName ) );
                           
                            if ( lua_type ( luaVM, 9 ) == LUA_TSTRING )
                            {
                                const char * szTemp = lua_tostring ( luaVM, 9 );
                                if ( !stricmp ( szTemp, "center" ) )
                                    ulFormat |= DT_CENTER;
                                else if ( !stricmp ( szTemp, "right" ) )
                                    ulFormat |= DT_RIGHT;

                                if ( lua_type ( luaVM, 10 ) == LUA_TSTRING )
                                {
                                    const char * szTemp = lua_tostring ( luaVM, 10 );
                                    if ( !stricmp ( szTemp, "center" ) )
                                        ulFormat |= DT_VCENTER;
                                    else if ( !stricmp ( szTemp, "bottom" ) )
                                        ulFormat |= ( DT_BOTTOM | DT_SINGLELINE );

                                    if ( lua_type ( luaVM, 11 ) == LUA_TBOOLEAN )
                                    {
                                        bClip = ( lua_toboolean ( luaVM, 11 ) ) ? true:false;

                                        if ( lua_type ( luaVM, 12 ) == LUA_TBOOLEAN )
                                        {
                                            if ( lua_toboolean ( luaVM, 12 ) )
                                                ulFormat |= DT_WORDBREAK;
											if ( lua_type ( luaVM, 13 ) == LUA_TBOOLEAN )
											{
												bPostGUI = ( lua_toboolean ( luaVM, 13 ) ) ? true:false;
											}
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if ( !bClip ) ulFormat |= DT_NOCLIP;

        ID3DXFont * pFont = g_pCore->GetGraphics ()->GetFont ( fontType );
        g_pCore->GetGraphics ()->DrawTextQueued ( iLeft, iTop, iRight, iBottom, ulColor, szText, fScale, fScale, ulFormat, pFont, bPostGUI );

        // Success
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "dxDrawText" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::dxDrawRectangle ( lua_State* luaVM )
{
    // dxDrawRectangle ( int x,int y,float width,float height,[int color=0xffffffff] )

    // Grab all argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) && 
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) && 
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) && 
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        int iX = static_cast < int > ( lua_tonumber ( luaVM, 1 ) );
        int iY = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
        int iWidth = static_cast < int > ( lua_tonumber ( luaVM, 3 ) );
        int iHeight = static_cast < int > ( lua_tonumber ( luaVM, 4 ) );
        unsigned long ulColor = 0xFFFFFFFF;

        int iArgument5 = lua_type ( luaVM, 5 );
        if ( ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
        {
            ulColor = static_cast < unsigned long > ( lua_tonumber ( luaVM, 5 ) );
        }

		// Got a post gui specifier?
		bool bPostGUI = false;
		int iArgument6 = lua_type ( luaVM, 6 );
		if ( iArgument6 == LUA_TBOOLEAN )
		{
			bPostGUI = ( lua_toboolean ( luaVM, 6 ) ) ? true:false;
		}

        g_pCore->GetGraphics ()->DrawRectQueued ( iX, iY, iWidth, iHeight, ulColor, bPostGUI );
        
        // Success
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "dxDrawRectangle" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::dxDrawImage ( lua_State* luaVM )
{
    // dxDrawImage ( float x,float y,float width,float height,string filename,[float rotation,
    //            float rotCenOffX, float rotCenOffY, int color=0xffffffff, bool postgui] )

    // Grab all argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) && 
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) && 
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) && 
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
         (                              iArgument5 == LUA_TSTRING ) )
    {
        float fX = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        float fY = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        float fWidth = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
        float fHeight = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
        const char * szFile = lua_tostring ( luaVM, 5 );
        float fRotation = 0;
        float fRotCenOffX = 0;
        float fRotCenOffY = 0;
        unsigned long ulColor = 0xFFFFFFFF;

        int iArgument6 = lua_type ( luaVM, 6 );
        if ( ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
        {
            fRotation = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
        }

        int iArgument7 = lua_type ( luaVM, 7 );
        if ( ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING ) )
        {
            fRotCenOffX = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );
        }

        int iArgument8 = lua_type ( luaVM, 8 );
        if ( ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING ) )
        {
            fRotCenOffY = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );
        }

        int iArgument9 = lua_type ( luaVM, 9 );
        if ( ( iArgument9 == LUA_TNUMBER || iArgument9 == LUA_TSTRING ) )
        {
            ulColor = static_cast < unsigned long > ( lua_tonumber ( luaVM, 9 ) );
        }

		// Got a post gui specifier?
		bool bPostGUI = false;
		int iArgument10 = lua_type ( luaVM, 10 );
		if ( iArgument10 == LUA_TBOOLEAN )
		{
			bPostGUI = ( lua_toboolean ( luaVM, 10 ) ) ? true:false;
		}

		CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		CResource* pResource = pLuaMain ? pLuaMain->GetResource() : NULL;

	    // Check for a valid (and sane) file path
	    if ( pResource && szFile && IsValidFilePath ( szFile ) )
        {
		    // Get the correct directory
		    char szPath[MAX_PATH] = {0};

		    snprintf ( szPath, MAX_PATH, "%s\\resources\\%s\\%s", m_pClientGame->GetModRoot (), pResource->GetName (), szFile );
		    szPath[MAX_PATH-1] = '\0';

            if ( g_pCore->GetGraphics ()->DrawTextureQueued ( fX, fY, fWidth, fHeight, szPath, fRotation, fRotCenOffX, fRotCenOffY, ulColor, bPostGUI ) )
            {
                // Success
                lua_pushboolean ( luaVM, true );
                return 1;
            }

            m_pScriptDebugging->LogError ( luaVM, "dxDrawImage can't load %s", szFile );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "dxDrawImage" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::dxGetTextWidth ( lua_State* luaVM )
{
    // dxGetTextWidth ( string text, [float scale=1,string font="default"] )

    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char * szText = lua_tostring ( luaVM, 1 );
        float fScale = 1.0f;
        eFontType fontType = FONT_DEFAULT;

        int iArgument2 = lua_type ( luaVM, 2 );
        if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
        {
            fScale = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            
            if ( lua_type ( luaVM, 3 ) == LUA_TSTRING )
            {
                const char * szFontName = lua_tostring ( luaVM, 3 );
                fontType = g_pCore->GetGraphics ()->GetFontType ( const_cast < char * > ( szFontName ) );
            }
        }

        ID3DXFont * pFont = g_pCore->GetGraphics ()->GetFont ( fontType );
        float fWidth = g_pCore->GetGraphics ()->GetDXTextExtent ( szText, fScale, pFont );

        // Success
        lua_pushnumber ( luaVM, fWidth );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "dxGetTextWidth" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::dxGetFontHeight ( lua_State* luaVM )
{
    // dxGetFontHeight ( [float scale=1,string font="default"] )

    float fScale = 1.0f;
    eFontType fontType = FONT_DEFAULT;

    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        fScale = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );

        if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            const char * szFontName = lua_tostring ( luaVM, 2 );
            fontType = g_pCore->GetGraphics ()->GetFontType ( const_cast < char * > ( szFontName ) );
        }
    }

    ID3DXFont * pFont = g_pCore->GetGraphics ()->GetFont ( fontType );
    float fHeight = g_pCore->GetGraphics ()->GetDXFontHeight ( fScale, pFont );

    // Success
    lua_pushnumber ( luaVM, fHeight );
    return 1;
}


int CLuaFunctionDefinitions::WasEventCancelled ( lua_State* luaVM )
{
    // Return whether the last event was cancelled or not
    lua_pushboolean ( luaVM, CStaticFunctionDefinitions::WasEventCancelled () );
    return 1;
}


int CLuaFunctionDefinitions::OutputConsole ( lua_State* luaVM )
{
    // Valid string argument?
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) || lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab it and output
        const char* szText = lua_tostring ( luaVM, 1 );
        if ( CStaticFunctionDefinitions::OutputConsole ( szText ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "outputConsole" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::OutputChatBox ( lua_State* luaVM )
{
    // Valid string argument?
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) || lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the output string
        const char* szText = lua_tostring ( luaVM, 1 );

        // Default color setup
        unsigned char ucRed = 235;
        unsigned char ucGreen = 221;
        unsigned char ucBlue = 178;
        bool bColorCoded = false;

        // Verify arguments and grab colors if they are included
        int iArgument2 = lua_type ( luaVM, 2 );
        if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
        {
            ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            int iArgument3 = lua_type ( luaVM, 3 );
            if ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING )
            {
                ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
                int iArgument4 = lua_type ( luaVM, 4 );
                if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
                {
                    ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

                    if ( lua_type ( luaVM, 5 ) == LUA_TBOOLEAN )
                        bColorCoded = ( lua_toboolean ( luaVM, 5 ) ) ? true:false;
                }
            }
        }

        // Output to the chatbox
        if ( CStaticFunctionDefinitions::OutputChatBox ( szText, ucRed, ucGreen, ucBlue, bColorCoded ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "outputChatBox" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ShowChat ( lua_State* luaVM )
{
    // Verify arguments
    if ( lua_istype ( luaVM, 1, LUA_TBOOLEAN ) )
    {
        // Grab whether to show the chatbox or not
        bool bShow = ( lua_toboolean ( luaVM, 1 ) ) ? true:false;
        if ( CStaticFunctionDefinitions::ShowChat ( bShow ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "showChat" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::OutputClientDebugString ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Default level and color
        unsigned int uiLevel = 3;
        int iRed = 255;
        int iGreen = 255;
        int iBlue = 255;

        // Grab the level and color stuff
        int iArgument2 = lua_type ( luaVM, 2 );
        int iArgument3 = lua_type ( luaVM, 3 );
        int iArgument4 = lua_type ( luaVM, 4 );
        int iArgument5 = lua_type ( luaVM, 5 );
        if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
        {
            // Level 0? Own color can be specified
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

            // Too big level?
            if ( uiLevel > 3 )
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

            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "outputDebugString" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetRootElement ( lua_State* luaVM )
{
    CClientEntity* pRoot = CStaticFunctionDefinitions::GetRootElement ();

    // Return the root element
    lua_pushelement ( luaVM, pRoot );
    return 1;
}


int CLuaFunctionDefinitions::IsElement ( lua_State* luaVM )
{
    // Correct argument type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element. If it's valid, return true.
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementID ( lua_State* luaVM )
{
    // Correct argument type specified?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Get its ID and return it
            const char* szName = pEntity->GetName ();
            if ( szName )
            {
                lua_pushstring ( luaVM, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementID", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementID" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementByID ( lua_State* luaVM )
{
    // Eventually read out the index
    int iArgument2 = lua_type ( luaVM, 2 );
    unsigned int uiIndex = 0;
    if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
    {
        uiIndex = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
    }

    // Check that we got the name in place
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        // Grab the string
        const char* szID = lua_tostring ( luaVM, 1 );

        // Try to find the element with that ID. Return it
        CClientEntity* pEntity = CStaticFunctionDefinitions::GetElementByID ( szID, uiIndex );
        if ( pEntity )
        {
            lua_pushelement ( luaVM, pEntity );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementByID" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementByIndex ( lua_State* luaVM )
{
    // Verify element type
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Grab the element type name and the index
        const char* szType = lua_tostring ( luaVM, 1 );
        unsigned int uiIndex = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );

        // Try to find it. Return it if we did.
        CClientEntity* pEntity = CStaticFunctionDefinitions::GetElementByIndex ( szType, uiIndex );
        if ( pEntity )
        {
            lua_pushelement ( luaVM, pEntity );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementByIndex" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementData ( lua_State* luaVM )
{
    // Correct arguments?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TSTRING ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        const char* szName = lua_tostring ( luaVM, 2 );
        bool bInherit = true;

        if ( lua_istype ( luaVM, 3, LUA_TBOOLEAN ) )
            bInherit = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;

        //  Valid?
        if ( pEntity )
        {
            // Grab that element data and push it back in return
            CLuaArgument* pVariable = pEntity->GetCustomData ( szName, bInherit );
            if ( pVariable )
            {
                pVariable->Push ( luaVM );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementData", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementData" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementPosition ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element, verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab the position
            CVector vecPosition;
            if ( CStaticFunctionDefinitions::GetElementPosition ( *pEntity, vecPosition ) )
            {            
                // Return it
                lua_pushnumber ( luaVM, vecPosition.fX );
                lua_pushnumber ( luaVM, vecPosition.fY );
                lua_pushnumber ( luaVM, vecPosition.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementPosition", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementPosition" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementRotation ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element, verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab the position
            CVector vecRotation;
            if ( CStaticFunctionDefinitions::GetElementRotation ( *pEntity, vecRotation ) )
            {            
                // Return it
                lua_pushnumber ( luaVM, vecRotation.fX );
                lua_pushnumber ( luaVM, vecRotation.fY );
                lua_pushnumber ( luaVM, vecRotation.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementRotation", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementRotation" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementVelocity ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element, verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab the velocity
            CVector vecVelocity;
            if ( CStaticFunctionDefinitions::GetElementVelocity ( *pEntity, vecVelocity ) )
            {
                // Return it
                lua_pushnumber ( luaVM, vecVelocity.fX );
                lua_pushnumber ( luaVM, vecVelocity.fY );
                lua_pushnumber ( luaVM, vecVelocity.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementVelocity", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementVelocity" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementType ( lua_State* luaVM )
{
    // Check the arg type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Get its typename
            const char* szTypeName = pEntity->GetTypeName ();
            if ( szTypeName )
            {
                lua_pushstring ( luaVM, szTypeName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementType", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementType" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementChildren ( lua_State* luaVM )
{
    // Find our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Correct arg type?
        if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
        {
            // Grab the argument
            CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
            if ( pEntity )
            {
                // Create a new table
                lua_newtable ( luaVM );

                // Add all the elements with a matching type to it
                pEntity->GetChildren ( pLuaMain );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "getElementChildren", "element", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getElementChildren" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementChild ( lua_State* luaVM )
{
    // Verify argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Grab entity and index
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned int uiIndex = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
        if ( pEntity )
        {
            // Grab the child
            CClientEntity* pChild = CStaticFunctionDefinitions::GetElementChild ( *pEntity, uiIndex );
            if ( pChild )
            {
                lua_pushelement ( luaVM, pChild );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementChild", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementChild" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementChildrenCount ( lua_State* luaVM )
{
    // Correct arg type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab the count
            unsigned int uiCount = pEntity->CountChildren ();
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( uiCount ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementChildrenCount", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementChildrenCount" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementParent ( lua_State* luaVM )
{
    // Correct arg type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the entity
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab the parent and return it
            CClientEntity* pParent = pEntity->GetParent ();
            if ( pParent )
            {
                lua_pushelement ( luaVM, pParent );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementParent", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementParent" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementsByType ( lua_State* luaVM )
{
    // Find our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Check arg types
        int iArgument2 = lua_type ( luaVM, 2 );
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && 
            ( iArgument2 == LUA_TNONE || iArgument2 == LUA_TLIGHTUSERDATA ) )
        {
            // see if a root argument has been specified
            CClientEntity* startAt = m_pRootEntity;
            if ( iArgument2 == LUA_TLIGHTUSERDATA )
            {
                // if its valid, use that, otherwise, produce an error
                CClientEntity* pEntity = lua_toelement ( luaVM, 2 );
                if ( pEntity )
                    startAt = pEntity;
                else
                {
                    m_pScriptDebugging->LogBadPointer ( luaVM, "getElementsByType", "element", 2 );
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }
            }

            bool bStreamedIn = false;
            if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
            {
                bStreamedIn = lua_toboolean ( luaVM, 3 ) ? true : false;
            }

            // Grab the argument
            const char* szType = lua_tostring ( luaVM, 1 );

            // Create a new table
            lua_newtable ( luaVM );

            // Add all the elements with a matching type to it
            startAt->FindAllChildrenByType ( szType, pLuaMain, bStreamedIn );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getElementsByType" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementInterior ( lua_State* luaVM )
{
    // Check arg types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab the interior and return it
            unsigned char ucInterior;
            if ( CStaticFunctionDefinitions::GetElementInterior ( *pEntity, ucInterior ) )
            {
                lua_pushnumber ( luaVM, ucInterior );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementInterior", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementInterior" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsElementWithinColShape ( lua_State* luaVM )
{
    // Check arg types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) )
    {
        // Grab elements
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        CClientColShape* pColShape = lua_tocolshape ( luaVM, 2 );

        // Valid element?
        if ( pEntity )
        {
            // Valid colshape?
            if ( pColShape )
            {
                // Check if it's within
                bool bWithin = pEntity->CollisionExists ( pColShape );
                lua_pushboolean ( luaVM, bWithin );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "isElementWithinColShape", "colshape", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isElementWithinColShape", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementWithinColShape" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::IsElementWithinMarker( lua_State* luaVM )
{
    // Check arg types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) )
    {
        // Grab elements
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        CClientMarker* pMarker = lua_tomarker ( luaVM, 2 );

        // Valid element?
        if ( pEntity )
        {
            // Valid colshape?
            if ( pMarker )
            {
                // Check if it's within
                bool bWithin = pEntity->CollisionExists ( pMarker->GetColShape() );
                lua_pushboolean ( luaVM, bWithin );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "isElementWithinMarker", "marker", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isElementWithinMarker", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementWithinMarker" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::GetElementsWithinColShape ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Correct arg type?
        if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
        {
            // Grab optional type arg
            const char* szType = NULL;
            if ( lua_istype ( luaVM, 2, LUA_TSTRING ) )
            {
                // Is it zero length? Don't use it
                szType = lua_tostring ( luaVM, 2 );
                if ( szType [ 0 ] == 0 )
                    szType = NULL;
            }

            // Grab the colshape passed
            CClientColShape* pColShape = lua_tocolshape ( luaVM, 1 );
            if ( pColShape )
            {
		        // Create a new table
		        lua_newtable ( luaVM );

                // Add all the elements within the shape to it
                unsigned int uiIndex = 0;
                list < CClientEntity* > ::iterator iter = pColShape->CollidersBegin ();
                for ( ; iter != pColShape->CollidersEnd (); iter++ )
                {
                    if ( szType == NULL || strcmp ( (*iter)->GetTypeName (), szType ) == 0 )
                    {
                        lua_pushnumber ( luaVM, ++uiIndex );
			            lua_pushelement ( luaVM, *iter );
			            lua_settable ( luaVM, -3 );
                    }
                }

                // We're returning 1 table
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "getElementsWithinColShape", "colshape", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getElementsWithinColShape" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementDimension ( lua_State* luaVM )
{
    // Correct type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element to check
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab the dimension
            unsigned short usDimension = pEntity->GetDimension ();
            lua_pushnumber ( luaVM, usDimension );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementDimension", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementDimension" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementBoundingBox ( lua_State* luaVM )
{
    // Correct type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab the bounding box and return it
            CVector vecMin, vecMax;
            if ( CStaticFunctionDefinitions::GetElementBoundingBox ( *pEntity, vecMin, vecMax ) )
            {
                lua_pushnumber ( luaVM, vecMin.fX );
                lua_pushnumber ( luaVM, vecMin.fY );
                lua_pushnumber ( luaVM, vecMin.fZ );
                lua_pushnumber ( luaVM, vecMax.fX );
                lua_pushnumber ( luaVM, vecMax.fY );
                lua_pushnumber ( luaVM, vecMax.fZ );
                return 6;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementBoundingBox", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementBoundingBox" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementRadius ( lua_State* luaVM )
{
    // Correct type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab its radius and return it
            float fRadius = 0.0f;
            if ( CStaticFunctionDefinitions::GetElementRadius ( *pEntity, fRadius ) )
            {
                lua_pushnumber ( luaVM, fRadius );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementRadius", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementRadius" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsElementAttached ( lua_State* luaVM )
{
    // Correct type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CClientEntity* pEntityAttachedTo = pEntity->GetAttachedTo();
            if ( pEntityAttachedTo )
            {
                if ( pEntityAttachedTo->IsEntityAttached ( pEntity ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isElementAttached", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementAttached" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::GetElementAttachedTo ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element to check
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        CClientEntity* pEntityAttachedTo = NULL;

        // Valid?
        if ( pEntity )
        {
            // Grab the enity attached to it
            CClientEntity* pEntityAttachedTo = CStaticFunctionDefinitions::GetElementAttachedTo ( *pEntity );
            if ( pEntityAttachedTo )
            {
                lua_pushelement ( luaVM, pEntityAttachedTo );
                return 1;
            }
            else
            {
                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementAttachedTo", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementAttachedTo" );
    
    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::GetAttachedElements ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Correct type?
        if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
        {
            // Grab the element
            CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
            if ( pEntity )
            {
		        // Create a new table
		        lua_newtable ( luaVM );

                // Add All Attached Elements
                unsigned int uiIndex = 0;
                list < CClientEntity* > ::const_iterator iter = pEntity->AttachedEntitiesBegin ();
                for ( ; iter != pEntity->AttachedEntitiesEnd () ; iter++ )
                {
                    CClientEntity * pAttached = *iter;
                    if ( pAttached->GetAttachedTo () == pEntity )
                    {
                        lua_pushnumber ( luaVM, ++uiIndex );
			            lua_pushelement ( luaVM, *iter );
			            lua_settable ( luaVM, -3 );
                    }
                }
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "getAttachedElements", "element", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getAttachedElements" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementDistanceFromCentreOfMassToBaseOfModel ( lua_State* luaVM )
{
    // Valid types?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element to check
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab the distance and return it
            float fDistance;
            if ( CStaticFunctionDefinitions::GetElementDistanceFromCentreOfMassToBaseOfModel ( *pEntity, fDistance ) )
            {
                lua_pushnumber ( luaVM, fDistance );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementDistanceFromCentreOfMassToBaseOfModel", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementDistanceFromCentreOfMassToBaseOfModel" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsElementLocal ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Return whether it's local or not
            bool bLocal = pEntity->IsLocalEntity ();;
            lua_pushboolean ( luaVM, bLocal );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isElementLocal", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementLocal" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::GetElementAlpha ( lua_State* luaVM )
{
    // Valid type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab its alpha level and return it
            unsigned char ucAlpha;
            if ( CStaticFunctionDefinitions::GetElementAlpha ( *pEntity, ucAlpha ) )
            {
                lua_pushnumber ( luaVM, ucAlpha );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementAlpha", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementAlpha" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementHealth ( lua_State* luaVM )
{
    // Same type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Get the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab the health and return it
            float fHealth;
            if ( CStaticFunctionDefinitions::GetElementHealth ( *pEntity, fHealth ) )
            {
                lua_pushnumber ( luaVM, fHealth );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementHealth", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementHealth" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementModel ( lua_State* luaVM )
{
    // Same type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Get the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned short usModel;
            if ( CStaticFunctionDefinitions::GetElementModel ( *pEntity, usModel ) )
            {
                lua_pushnumber ( luaVM, usModel );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementModel", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementModel" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetElementColShape ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Get the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
			CClientEntity* pColShape = static_cast < CClientEntity* > ( CStaticFunctionDefinitions::GetElementColShape ( pEntity ) );
            if ( pColShape )
            {
                lua_pushelement ( luaVM, pColShape );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementColShape", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementColShape" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsElementInWater ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the entity and verify it.
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            bool bInWater = false;
            if ( CStaticFunctionDefinitions::IsElementInWater ( *pEntity, bInWater ) )
            {
                lua_pushboolean ( luaVM, bInWater );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementInWater" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsElementSyncer ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the entity and verify it.
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            bool bIsSyncer = false;
            if ( CStaticFunctionDefinitions::IsElementSyncer ( *pEntity, bIsSyncer ) )
            {
                lua_pushboolean ( luaVM, bIsSyncer );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementSyncer" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsElementStreamedIn ( lua_State* luaVM )
{
    // We have a correct parameter 1?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the entity and verify it.
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Is this a streaming compatible class?
            if ( pEntity->IsStreamingCompatibleClass () )
            {
                CClientStreamElement* pStreamElement = static_cast < CClientStreamElement* > ( pEntity );

                // Return whether or not this class is streamed in
                lua_pushboolean ( luaVM, pStreamElement->IsStreamedIn () );
                return 1;
            }
            else
                m_pScriptDebugging->LogWarning ( luaVM, "isElementStreamedIn; element is not streaming compatible\n" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isElementStreamedIn", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementStreamedIn" );

    // We failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::IsElementStreamable ( lua_State* luaVM )
{
    // We have a correct parameter 1?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the entity and verify it.
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Is this a streaming compatible class?
            if ( pEntity->IsStreamingCompatibleClass () )
            {
                CClientStreamElement* pStreamElement = static_cast < CClientStreamElement* > ( pEntity );

                bool bStreamable = ( pStreamElement->GetTotalStreamReferences () == 0 );
                // Return whether or not this element is set to never be streamed out
                lua_pushboolean ( luaVM, bStreamable );
                return 1;
            }
            else
                m_pScriptDebugging->LogWarning ( luaVM, "isElementStreamable; element is not streaming compatible\n" );

            // Return false, we're not streamable
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isElementStreamable", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementStreamable" );

    // We failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::IsElementOnScreen ( lua_State* luaVM )
{
    // Valid type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Return whether we're on the screen or not
            bool bOnScreen;
            if ( CStaticFunctionDefinitions::IsElementOnScreen ( *pEntity, bOnScreen ) )
            {
                lua_pushboolean ( luaVM, bOnScreen );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isElementOnScreen", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementOnScreen" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::CreateElement ( lua_State* luaVM )
{
    // Grab our VM
	CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
	if ( pLuaMain )
	{
        // Grab its resource
        CResource * pResource = pLuaMain->GetResource();
        if ( pResource )
        {
            // Grab the optional name argument
            char szDefaultID [] = "";
            const char* szID = szDefaultID;
            int iArgument2 = lua_type ( luaVM, 2 );
            if ( iArgument2 == LUA_TSTRING )
            {
                szID = lua_tostring ( luaVM, 2 );
            }
            else if ( iArgument2 != LUA_TNONE )
            {
                m_pScriptDebugging->LogBadType ( luaVM, "createElement" );
                lua_pushboolean ( luaVM, false );
                return 1;
            }

            // Verify argument types
            if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
            {
                // Grab the string
                const char* szTypeName = lua_tostring ( luaVM, 1 );

				// Try to create
				CClientDummy* pDummy = CStaticFunctionDefinitions::CreateElement ( *pResource, szTypeName, szID );
				if ( pDummy )
				{
                    // Add it to the element group
                    // TODO: Get rid of element groups
	                CElementGroup * pGroup = pResource->GetElementGroup();
					if ( pGroup )
					{
	                    pGroup->Add ( ( CClientEntity* ) pDummy );
					}

                    // Return it
					lua_pushelement ( luaVM, pDummy );
					return 1;
				}
                else
                    m_pScriptDebugging->LogError ( luaVM, "createElement; unable to create mor elements\n" );
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "createElement" );
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::DestroyElement ( lua_State* luaVM )
{
    // Correct type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Destroy it
            if ( CStaticFunctionDefinitions::DestroyElement ( *pEntity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "destroyElement", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "destroyElement" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementData ( lua_State* luaVM )
{
    // Grab the vm
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Check that we have the 3 correct args
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
             lua_type ( luaVM, 2 ) == LUA_TSTRING &&
             lua_type ( luaVM, 3 ) != LUA_TNONE )
        {
            // Grab the element, the element data name and the data to set
            CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
            const char* szName = lua_tostring ( luaVM, 2 );
            bool bSynchronize = true;
            CLuaArgument Variable;
            if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN && lua_type ( luaVM, 4 ) != LUA_TNONE )
            {
                bSynchronize = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;            
                Variable.Read ( luaVM, 4 );
            }
            else
                Variable.Read ( luaVM, 3 );

            // Valid element?
            if ( pEntity )
            {
                // Try to change the data
                if ( CStaticFunctionDefinitions::SetElementData ( *pEntity, szName, Variable, *pLuaMain, bSynchronize ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setElementData", "element", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "setElementData" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::RemoveElementData ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Correct args?
        if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
             lua_istype ( luaVM, 2, LUA_TSTRING ) )
        {
            // Grab the element and the name
            CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
            const char* szName = lua_tostring ( luaVM, 2 );

            // Valid?
            if ( pEntity )
            {
                // Try to remove the elementdata
                if ( CStaticFunctionDefinitions::RemoveElementData ( *pEntity, szName ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "removeElementData", "element", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "removeElementData" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementPosition ( lua_State* luaVM )
{
    // Correct types?
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the element and the position to change to
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 4 ) ) );

        // Valid?
        if ( pEntity )
        {
            // Try to set the position
            if ( CStaticFunctionDefinitions::SetElementPosition ( *pEntity, vecPosition ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementPosition", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementPosition" );

    // Error
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementRotation ( lua_State* luaVM )
{
    // Correct types?
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the element and the position to change to
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        CVector vecRotation ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 4 ) ) );

        // Valid?
        if ( pEntity )
        {
            // Try to set the position
            if ( CStaticFunctionDefinitions::SetElementRotation ( *pEntity, vecRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementRotation", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementRotation" );

    // Error
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementVelocity ( lua_State* luaVM )
{
    // Verify the first argument
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Check rest of the arguments. Don't mind the last Z argument if the element is 2D
            int iArgument2 = lua_type ( luaVM, 2 );
            int iArgument3 = lua_type ( luaVM, 3 );
            int iArgument4 = lua_type ( luaVM, 4 );
            if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
                 ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                 ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING || pEntity->GetType () == CClientGame::RADAR_AREA ) )
            {
                // Grab the velocity
                CVector vecVelocity = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                                                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                                                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ) );
                // Set the velocity
                if ( CStaticFunctionDefinitions::SetElementVelocity ( *pEntity, vecVelocity ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "setElementVelocity" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementVelocity", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementVelocity" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementParent ( lua_State* luaVM )
{
    // Correct args?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) )
    {
        // Grab entity 
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab new parent
            CClientEntity* pParent = lua_toelement ( luaVM, 2 );
            if ( pParent )
            {
                // Change the parent
                if ( CStaticFunctionDefinitions::SetElementParent ( *pEntity, *pParent ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setElementParent", "element", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementParent", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementParent" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementInterior ( lua_State* luaVM )
{
    // Correct args?
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Grab the element and the interior
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned char ucInterior = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        // Grab position if supplied
        bool bSetPosition = false;
        CVector vecPosition;

        int iArgument3 = lua_type ( luaVM, 3 );
        int iArgument4 = lua_type ( luaVM, 4 );
        int iArgument5 = lua_type ( luaVM, 5 );
        if ( ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
             ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
             ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
        {
            vecPosition = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                                    static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                                    static_cast < float > ( lua_tonumber ( luaVM, 5 ) ) );
            bSetPosition = true;
        }

        // Valid element?
        if ( pEntity )
        {
            // Set the interior
            if ( CStaticFunctionDefinitions::SetElementInterior ( *pEntity, ucInterior, bSetPosition, vecPosition ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementInterior", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementInterior" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementDimension ( lua_State* luaVM )
{
    // Correct args?
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Grab the element and the dimension
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned short usDimension = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );

        // Valid?
        if ( pEntity )
        {
            // Change the dimension
            if ( CStaticFunctionDefinitions::SetElementDimension ( *pEntity, usDimension ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementDimension", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementDimension" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::AttachElements ( lua_State* luaVM )
{
    // Valid arguments?
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element to attach and the target
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        CClientEntity* pAttachedToEntity = lua_toelement ( luaVM, 2 );
        CVector vecPosition, vecRotation;

        // Grab the supplied arguments (pos: x y z, rot: x y z)
        int iArgument3 = lua_type ( luaVM, 3 );
        if ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER )
        {
            vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );

            int iArgument4 = lua_type ( luaVM, 4 );
            if ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER )
            {
                vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

                int iArgument5 = lua_type ( luaVM, 5 );
                if ( iArgument5 == LUA_TSTRING || iArgument5 == LUA_TNUMBER )
                {
                    vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );

                    int iArgument6 = lua_type ( luaVM, 6 );
                    if ( iArgument6 == LUA_TSTRING || iArgument6 == LUA_TNUMBER )
                    {
                        vecRotation.fX = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );

                        int iArgument7 = lua_type ( luaVM, 7 );
                        if ( iArgument7 == LUA_TSTRING || iArgument7 == LUA_TNUMBER )
                        {
                            vecRotation.fY = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );

                            int iArgument8 = lua_type ( luaVM, 8 );
                            if ( iArgument8 == LUA_TSTRING || iArgument8 == LUA_TNUMBER )
                            {
                                vecRotation.fZ = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );
                            }
                        }
                    }
                }
            }
        }

        // Valid element?
        if ( pEntity )
        {
            // Valid host element?
            if ( pAttachedToEntity )
            {
                // Try to attach them
                if ( CStaticFunctionDefinitions::AttachElements ( *pEntity, *pAttachedToEntity, vecPosition, vecRotation ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "attachElements", "element", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "attachElements", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "attachElements" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::DetachElements ( lua_State* luaVM )
{
    // Valid arg type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element that's attached to something
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
       
        // Grab the other element if supplied
        CClientEntity* pAttachedToEntity = NULL;
        if ( lua_istype ( luaVM, 2, LUA_TLIGHTUSERDATA ) )
            pAttachedToEntity = lua_toelement ( luaVM, 2 );

        // Valid?
        if ( pEntity )
        {
			// Try to detach
			if ( CStaticFunctionDefinitions::DetachElements ( *pEntity, pAttachedToEntity ) )
			{
				lua_pushboolean ( luaVM, true );
				return 1;
			}
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "detachElements", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "detachElements" );

    // Failure
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementAttachedOffsets ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element to attach and the target
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        CVector vecPosition, vecRotation;

        // Grab the supplied arguments (pos: x y z, rot: x y z)
        int iArgument2 = lua_type ( luaVM, 2 );
        if ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER )
        {
            vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );

            int iArgument3 = lua_type ( luaVM, 3 );
            if ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER )
            {
                vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );

                int iArgument4 = lua_type ( luaVM, 4 );
                if ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER )
                {
                    vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

                    int iArgument5 = lua_type ( luaVM, 5 );
                    if ( iArgument5 == LUA_TSTRING || iArgument5 == LUA_TNUMBER )
                    {
                        vecRotation.fX = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );

                        int iArgument6 = lua_type ( luaVM, 6 );
                        if ( iArgument6 == LUA_TSTRING || iArgument6 == LUA_TNUMBER )
                        {
                            vecRotation.fY = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );

                            int iArgument7 = lua_type ( luaVM, 7 );
                            if ( iArgument7 == LUA_TSTRING || iArgument7 == LUA_TNUMBER )
                            {
                                vecRotation.fZ = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );
                            }
                        }
                    }
                }
            }
        }

        // Valid element?
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetElementAttachedOffsets ( *pEntity, vecPosition, vecRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementAttachedOffsets", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementAttachedOffsets" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementAlpha ( lua_State* luaVM )
{
    // Valid args?
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        // Grab the element to change the alpha of
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Grab the chosen alpha and set it
            unsigned char ucAlpha = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetElementAlpha ( *pEntity, ucAlpha ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementAlpha", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementAlpha" );

    // Failure
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementHealth ( lua_State* luaVM )
{
    // Check the args
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        // Grab the element to change the health of
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Try to change the health
            float fHealth = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetElementHealth ( *pEntity, fHealth ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementHealth", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementHealth" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementModel ( lua_State* luaVM )
{
    // Check the args
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        // Grab the element to change the health of
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Try to change the health
            unsigned short usModel = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetElementModel ( *pEntity, usModel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementModel", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementModel" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementStreamable ( lua_State* luaVM )
{
    // We have a correct parameter 1 and 2?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        // Grab the entity and verify it.
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Is this a streaming compatible class?
            if ( pEntity->IsStreamingCompatibleClass () )
            {
                CClientStreamElement* pStreamElement = static_cast < CClientStreamElement* > ( pEntity );

                // Set this object's streamable state.
                bool bStreamable = lua_toboolean ( luaVM, 2 ) ? true : false;

                // TODO: maybe use a better VM-based reference system (rather than a boolean one)
                if ( bStreamable && ( pStreamElement->GetStreamReferences ( true ) > 0 ) )
                {
                    pStreamElement->RemoveStreamReference ( true );
                }
                else if ( !bStreamable && ( pStreamElement->GetStreamReferences ( true ) == 0 ) )
                {
                    pStreamElement->AddStreamReference ( true );
                }
                else
                {
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }

                // Return success
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementStreamable", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementStreamable" );

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetLocalPlayer ( lua_State* luaVM )
{
    CClientPlayer* pPlayer = CStaticFunctionDefinitions::GetLocalPlayer ();

    // Return the local player
    lua_pushelement ( luaVM, pPlayer );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerName ( lua_State* luaVM )
{
    // Right type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the player
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
		if ( pPlayer )
		{
            // Grab his nametag text and return it
            const char* szName = pPlayer->GetNickPointer ();
            if ( szName )
            {
                lua_pushstring ( luaVM, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerName", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerName" );

    // Error
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetPedVoice ( lua_State* luaVM )
{
    // Right type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            const char* szVoiceType = 0;
            const char* szVoiceBank = 0;
            pPed->GetVoice ( &szVoiceType, &szVoiceBank );
            if ( szVoiceType && szVoiceBank )
            {
                lua_pushstring ( luaVM, szVoiceType );
                lua_pushstring ( luaVM, szVoiceBank );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedVoice", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedVoice" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetPedVoice ( lua_State* luaVM )
{
    // Right type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && 
         lua_istype ( luaVM, 2, LUA_TSTRING ) && 
         lua_istype ( luaVM, 3, LUA_TSTRING ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        const char* szVoiceType = lua_tostring ( luaVM, 2 );
        const char* szVoiceBank = lua_tostring ( luaVM, 3 );

		if ( pPed && szVoiceType && szVoiceBank )
		{
            pPed->SetVoice ( szVoiceType, szVoiceBank );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else if ( !pPed )
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedVoice", "ped", 1 );
        else if ( !szVoiceType )
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedVoice", "voicetype", 1 );
        else if ( !szVoiceBank )
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedVoice", "voicebank", 1 );

    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedVoice" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedWeapon ( lua_State* luaVM )
{
    // Right type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
		if ( pPed )
		{
            // Grab the slot if specified
			unsigned char ucSlot = 0xFF;
            if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) || lua_istype ( luaVM, 2, LUA_TSTRING ) )
                ucSlot = ( unsigned char ) lua_tonumber ( luaVM, 2 );

            if ( ucSlot == 0xFF )
                ucSlot = pPed->GetCurrentWeaponSlot ();

            CWeapon* pWeapon = pPed->GetWeapon ( ( eWeaponSlot ) ucSlot );
            if ( pWeapon )
            {
                unsigned char ucWeapon = pWeapon->GetType ();
                lua_pushnumber ( luaVM, ucWeapon );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedWeapon", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedWeapon" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedWeaponSlot ( lua_State* luaVM )
{
    // Right type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
		if ( pPed )
		{
            // Grab his current slot
			int iSlot = pPed->GetCurrentWeaponSlot ();
			lua_pushnumber ( luaVM, iSlot );
			return 1;
		}
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedWeaponSlot", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedWeaponSlot" );

    // Failed
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::GetPedAmmoInClip ( lua_State* luaVM )
{
    // Right types?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped and the slot provided
        CClientPed* pPed = lua_toped ( luaVM, 1 );

        // Got a ped
        if ( pPed )
        {
            // Got a second argument too (slot)?
            unsigned char ucSlot = pPed->GetCurrentWeaponSlot ();
            int iArgument2 = lua_type ( luaVM, 2 );
            if ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER )
            {
                ucSlot = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            }

            CWeapon* pWeapon = pPed->GetWeapon ( (eWeaponSlot) ucSlot );
            if ( pWeapon )
            {
                unsigned short usAmmo = pWeapon->GetAmmoInClip ();
                lua_pushnumber ( luaVM, usAmmo );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedAmmoInClip", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedAmmoInClip" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedTotalAmmo ( lua_State* luaVM )
{
    // Right types?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped and the slot
        CClientPed* pPed = lua_toped ( luaVM, 1 );

        // Got the ped?
        if ( pPed )
        {
            // Got a slot argument too?
            unsigned char ucSlot = pPed->GetCurrentWeaponSlot ();
            int iArgument2 = lua_type ( luaVM, 2 );
            if ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER )
            {
                ucSlot = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            }

            // Grab the ammo and return
            CWeapon* pWeapon = pPed->GetWeapon ( (eWeaponSlot) ucSlot );
            if ( pWeapon )
            {
                unsigned short usAmmo = pWeapon->GetAmmoTotal ();
                lua_pushnumber ( luaVM, usAmmo );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTotalAmmo", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTotalAmmo" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedOccupiedVehicle ( lua_State* luaVM )
{
    // Correct type?
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his occupied vehicle
            CClientVehicle* pVehicle = pPed->GetOccupiedVehicle ();
            if ( pVehicle )
            {                
                lua_pushelement ( luaVM, pVehicle );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedOccupiedVehicle", "ped", 1 );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedTask ( lua_State* luaVM )
{
    // Check types
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TSTRING ) &&
         ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER ) )
    {
        // Grab the ped, the priority and the task type
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        const char* szPriority = lua_tostring ( luaVM, 2 );
        unsigned int uiTaskType = static_cast < unsigned int > ( lua_tonumber ( luaVM, 3 ) );
        int iIndex = 0;

        // Grab the index if provided
        int iArgument4 = lua_type ( luaVM, 4 );
        if ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER )
            iIndex = static_cast < int > ( lua_tonumber ( luaVM, 4 ) );

        // Valid ped?
        if ( pPed )
        {
            // Any priority specified?
            if ( szPriority && szPriority [ 0 ] )
            {
                // Primary or secondary task grabbed?
                bool bPrimary = false;
                if ( ( bPrimary = !stricmp ( szPriority, "primary" ) ) ||
                     ( !stricmp ( szPriority, "secondary" ) ) )
                {
                    // Grab the taskname and return it
                    char* szTaskName = CStaticFunctionDefinitions::GetPedTask ( *pPed, bPrimary, uiTaskType, iIndex );
                    if ( szTaskName )
                    {
                        lua_pushstring ( luaVM, szTaskName );
                        return 1;
                    }
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTask", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTask" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedSimplestTask ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his simplest task and return it
            char* szTaskName = CStaticFunctionDefinitions::GetPedSimplestTask ( *pPed );
            if ( szTaskName )
            {
                lua_pushstring ( luaVM, szTaskName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedSimplestTask", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedSimplestTask" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedDoingTask ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TSTRING ) )
    {
        // Grab the ped and the task name to check
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        const char* szTaskName = lua_tostring ( luaVM, 2 );

        // Check ped
        if ( pPed )
        {
            // Check whether he's doing that task or not
            bool bIsDoingTask;
            if ( CStaticFunctionDefinitions::IsPedDoingTask ( *pPed, szTaskName, bIsDoingTask ) )
            {
                lua_pushboolean ( luaVM, bIsDoingTask );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedDoingTask", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedDoingTask" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerPing ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the player
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            // Grab his ping
            unsigned int uiPing = pPlayer->GetPing ();
            lua_pushnumber ( luaVM, uiPing );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerPing", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerPing" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedTarget ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his target element
            CClientEntity* pEntity = CStaticFunctionDefinitions::GetPedTarget ( *pPed );
            if ( pEntity )
            {
                lua_pushelement ( luaVM, pEntity );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTarget", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTarget" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedTargetStart ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his start aim position and return it
            CVector vecStart;
            pPed->GetShotData ( &vecStart );

            lua_pushnumber ( luaVM, vecStart.fX );
            lua_pushnumber ( luaVM, vecStart.fY );
            lua_pushnumber ( luaVM, vecStart.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTargetStart", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTargetStart" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedTargetEnd ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab the ped end target position and return it
            CVector vecEnd;
            pPed->GetShotData ( NULL, &vecEnd );

            lua_pushnumber ( luaVM, vecEnd.fX );
            lua_pushnumber ( luaVM, vecEnd.fY );
            lua_pushnumber ( luaVM, vecEnd.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTargetEnd", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTargetEnd" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedTargetRange ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // TODO: getPedTargetRange
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTargetRange", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTargetRange" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedTargetCollision ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his target collision and return it
            CVector vecCollision;
            if ( CStaticFunctionDefinitions::GetPedTargetCollision ( *pPed, vecCollision ) )
            {
                lua_pushnumber ( luaVM, vecCollision.fX );
                lua_pushnumber ( luaVM, vecCollision.fY );
                lua_pushnumber ( luaVM, vecCollision.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedTargetCollision", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedTargetCollision" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedArmor ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab the armor and return it
            float fArmor = pPed->GetArmor ();
			lua_pushnumber ( luaVM, fArmor );
			return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedArmor", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedArmor" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerNametagText ( lua_State* luaVM )
{
    // Check the type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the player
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            // Grab his nametag text and return it
            const char* szName = pPlayer->GetNametagText ();
            if ( szName )
            {
                lua_pushstring ( luaVM, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerNametagText", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerNametagText" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerNametagColor ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the player
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            // Grab his nametag color and return it
            unsigned char ucR, ucG, ucB;
            pPlayer->GetNametagColor ( ucR, ucG, ucB );

            lua_pushnumber ( luaVM, ucR );
            lua_pushnumber ( luaVM, ucG );
            lua_pushnumber ( luaVM, ucB );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPlayerNametagColor", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerNametagColor" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::IsPlayerNametagShowing ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) )
    {
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            bool bIsNametagShowing = pPlayer->IsNametagShowing ();
            lua_pushboolean ( luaVM, bIsNametagShowing );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPlayerNametagShowing", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPlayerNametagShowing" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedStat ( lua_State* luaVM )
{
    // Check the types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        // Grab the ped and the stat argument
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        unsigned short usStat = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );

        // Valid ped?
        if ( pPed )
        {
            // Check the stat
            if ( usStat < NUM_PLAYER_STATS )
            {
                float fValue = pPed->GetStat ( usStat );
                lua_pushnumber ( luaVM, fValue );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedStat", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedStat" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerFromNick ( lua_State* luaVM )
{
    // Valid player nick argument?
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        // Grab the nick
        const char* szNick = lua_tostring ( luaVM, 1 );

        // Grab the player with that nick
        CClientPlayer* pPlayer = CStaticFunctionDefinitions::GetPlayerFromNick ( szNick );
        if ( pPlayer )
        {
            // Return the player
            lua_pushelement ( luaVM, pPlayer );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPlayerNick" );

    // Doesn't exist
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerMoney ( lua_State* luaVM )
{
    long lMoney;
    if ( CStaticFunctionDefinitions::GetPlayerMoney ( lMoney ) )
    {
        lua_pushnumber ( luaVM, lMoney );
        return 1;
    }            

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerWantedLevel ( lua_State* luaVM )
{
    char cWanted;
    if ( CStaticFunctionDefinitions::GetPlayerWantedLevel ( cWanted ) )
    {
        lua_pushnumber ( luaVM, cWanted );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedChoking ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped and verify him
        CClientPed* pPed = lua_toped ( luaVM, 1 );
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
    
    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::IsPlayerDead ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            // Grab his dead state and return it
            bool bDead = pPlayer->IsDead ();
            lua_pushboolean ( luaVM, bDead );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPlayerDead", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPlayerDead" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::IsPedDucked ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Grab his ducked state
            bool bDucked = pPed->IsDucked ();
            lua_pushboolean ( luaVM, bDucked );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedDucked", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedDucked" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::IsPedInVehicle ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Find out whether he's in a vehicle or not
            bool bInVehicle = pPed->GetOccupiedVehicle () != NULL;

            // Return that state
            lua_pushboolean ( luaVM, bInVehicle );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedInVehicle", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedInVehicle" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::DoesPedHaveJetPack ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Find out whether he has a jetpack or not and return it
            bool bHasJetPack = pPed->HasJetPack ();
            lua_pushboolean ( luaVM, bHasJetPack );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "doesPedHaveJetPack", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "doesPedHaveJetPack" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedOnGround ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the ped
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            // Find out whether he's on the ground or not and return it
            bool bOnGround = pPed->IsOnGround ();
            lua_pushboolean ( luaVM, bOnGround );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedOnGround", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedOnGround" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPlayerTeam ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the player
        CClientPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            // Grab his team and return it
            CClientTeam* pTeam = pPlayer->GetTeam ();
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

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedContactElement ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            CClientEntity* pEntity = pPed->GetContactEntity ();
            if ( pEntity )
            {
                lua_pushelement ( luaVM, pEntity );
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


int CLuaFunctionDefinitions::GetPedRotation ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            float fRotation = ConvertRadiansToDegrees ( pPed->GetCurrentRotation () );
            lua_pushnumber ( luaVM, fRotation );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedRotation", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedRotation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CanPedBeKnockedOffBike ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bCanBeKnockedOffBike = pPed->GetCanBeKnockedOffBike ();
            lua_pushboolean ( luaVM, bCanBeKnockedOffBike );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "canPedBeKnockedOffBike", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "canPedBeKnockedOffBike" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedBonePosition ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientPed* pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            unsigned char ucBone = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( ucBone <= BONE_RIGHTFOOT )
            {
                eBone bone = ( eBone ) ucBone;
                CVector vecPosition;
                if ( CStaticFunctionDefinitions::GetPedBonePosition ( *pPed, bone, vecPosition ) )
                {
                    lua_pushnumber ( luaVM, vecPosition.fX );
                    lua_pushnumber ( luaVM, vecPosition.fY );
                    lua_pushnumber ( luaVM, vecPosition.fZ );
                    return 3;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedBonePosition", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedBonePosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedWeaponSlot ( lua_State* luaVM )
{
    // Check type
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        // Grab the arguments
        CClientEntity* pElement = lua_toelement ( luaVM, 1 );
		int iSlot = lua_tonumber ( luaVM, 2 );

        // Valid ped?
        if ( pElement )
		{
            // Valid slot?
            if ( iSlot >= 0 )
            {
                // Set his slot
                if ( CStaticFunctionDefinitions::SetPedWeaponSlot ( *pElement, iSlot ) )
                {
			        lua_pushboolean ( luaVM, true );
                    return 1;
                }
		    }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedWeaponSlot", "ped", 1 );
	}
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedWeaponSlot" );

    // Failed
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::ShowPlayerHudComponent ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) &&
         lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        // Hud components (TODO: Move somewhere else)
        struct SHudComponent { char szName [ 15 ]; };
        static SHudComponent hudComponents [] = { { "ammo" }, { "weapon" }, { "health" },
                                                  { "breath" }, { "armour" }, { "money" },
                                                  { "vehicle_name" }, { "area_name" }, { "radar" },
                                                  { "clock" }, { NULL } };

        // Grab the component name and the bool deciding whether to show it or not
        const char* szComponent = lua_tostring ( luaVM, 1 );
        bool bShow = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        // Any component specified?
        if ( szComponent && szComponent [ 0 ] )
        {
            // Loop through the components and find the correct one (matching name)
            unsigned char ucComponent = 0xFF;
            for ( int i = 0 ; hudComponents [ i ].szName [ 0 ] != NULL ; i++ )
            {
                if ( !stricmp ( szComponent, hudComponents [ i ].szName ) )
                {
                    ucComponent = static_cast < unsigned char > ( i );
                    break;
                }
            }

            // Found it?
            if ( ucComponent != 0xFF )
            {
                // Do it
                if ( CStaticFunctionDefinitions::ShowPlayerHudComponent ( ucComponent, bShow ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "showPlayerHudComponent", "component", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "showPlayerHudComponent" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerMoney ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        long lMoney = static_cast < long > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetPlayerMoney ( lMoney ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerMoney" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GivePlayerMoney ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        long lMoney = static_cast < long > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::GivePlayerMoney ( lMoney ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "givelayerMoney" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::TakePlayerMoney ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        long lMoney = static_cast < long > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::TakePlayerMoney ( lMoney ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "takePlayerMoney" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerNametagText ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TSTRING ) )
    {
        CClientPlayer * pPlayer = lua_toplayer ( luaVM, 1 );

        const char * szText = lua_tostring ( luaVM, 2 );
        if ( pPlayer && CStaticFunctionDefinitions::SetPlayerNametagText ( *pPlayer, const_cast < char * > ( szText ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerNametagText" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPlayerNametagColor ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CClientPlayer * pPlayer = lua_toplayer ( luaVM, 1 );
        unsigned char ucR = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        unsigned char ucG = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        unsigned char ucB = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
        if ( CStaticFunctionDefinitions::SetPlayerNametagColor ( *pPlayer, ucR, ucG, ucB ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPlayerNametagColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetPlayerNametagShowing ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        // Grab the entity
        CClientPlayer * pPlayer = lua_toplayer ( luaVM, 1 );
        bool bShowing = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        // Valid pPlayer?
        if ( pPlayer )
		{
            // Set the new rotation
            if ( CStaticFunctionDefinitions::SetPlayerNametagShowing ( *pPlayer, bShowing ) )
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

    // Failed
	lua_pushboolean ( luaVM, false );
	return 1;
}



int CLuaFunctionDefinitions::GetPedClothes ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        if ( pPed )
        {
            char szTexture [ 128 ], szModel [ 128 ];
            if ( CStaticFunctionDefinitions::GetPedClothes ( *pPed, ucType, szTexture, szModel ) )
            {
                lua_pushstring ( luaVM, szTexture );
                lua_pushstring ( luaVM, szModel );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedClothes", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedClothes" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedControlState ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        const char * szControl = lua_tostring ( luaVM, 2 );
        if ( pPed )
        {
            bool bState;
            if ( CStaticFunctionDefinitions::GetPedControlState ( *pPed, szControl, bState ) )
            {
                lua_pushboolean ( luaVM, bState );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedControlState", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedControlState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedDoingGangDriveby ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bDoingGangDriveby;
            if ( CStaticFunctionDefinitions::IsPedDoingGangDriveby ( *pPed, bDoingGangDriveby ) )
            {
                lua_pushboolean ( luaVM, bDoingGangDriveby );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isPedDoingGangDriveby", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isPedDoingGangDriveby" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedAnimation ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            char szBlockName [ 128 ], szAnimName [ 128 ];
            if ( CStaticFunctionDefinitions::GetPedAnimation ( *pPed, szBlockName, szAnimName, 128 ))
            {
                lua_pushstring ( luaVM, szBlockName );
                lua_pushstring ( luaVM, szAnimName );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedAnimation", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedAnimation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPedMoveAnim ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            unsigned int iMoveAnim;
            if ( CStaticFunctionDefinitions::GetPedMoveAnim ( *pPed, iMoveAnim ) )
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


int CLuaFunctionDefinitions::IsPedHeadless ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bHeadless;
            if ( CStaticFunctionDefinitions::IsPedHeadless ( *pPed, bHeadless ) )
            {
                lua_pushboolean ( luaVM, bHeadless );
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


int CLuaFunctionDefinitions::GetPedCameraRotation ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            float fRotation = 0.0f;
            if ( CStaticFunctionDefinitions::GetPedCameraRotation ( *pPed, fRotation ) )
            {
                lua_pushnumber ( luaVM, fRotation );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPedCameraRotation", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPedCameraRotation" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPedOnFire ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientPed * pPed = lua_toped ( luaVM, 1 );
        if ( pPed )
        {
            bool bOnFire;
            if ( CStaticFunctionDefinitions::IsPedOnFire ( *pPed, bOnFire ) )
            {
                lua_pushboolean ( luaVM, bOnFire );
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

int CLuaFunctionDefinitions::SetPedOnFire ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bOnFire = ( ( lua_toboolean ( luaVM, 2 ) ) ? true:false );

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedOnFire ( *pEntity, bOnFire ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedOnFire", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedOnFire" );

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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        const char* szTexture = lua_tostring ( luaVM, 2 );
        const char* szModel = lua_tostring ( luaVM, 3 );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::AddPedClothes ( *pEntity, const_cast < char* > ( szTexture ), const_cast < char* > ( szModel ), ucType ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::RemovePedClothes ( *pEntity, ucType ) )
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


int CLuaFunctionDefinitions::SetPedControlState ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( lua_type ( luaVM, 2 ) == LUA_TSTRING ) &&
         ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        const char * szControl = lua_tostring ( luaVM, 2 );
        bool bState = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedControlState ( *pEntity, szControl, bState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedControlState", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedControlState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedDoingGangDriveby ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bDoingGangDriveby = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedDoingGangDriveby ( *pEntity, bDoingGangDriveby ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedDoingGangDriveby", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedDoingGangDriveby" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedLookAt ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        CVector vecPosition;
        vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
        vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
        int iTime = 3000;
        CClientEntity * pTarget = NULL;
        
        int iArgument5 = lua_type ( luaVM, 5 );
        if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
        {
            iTime = static_cast < int > ( lua_tonumber ( luaVM, 5 ) );

            if ( lua_type ( luaVM, 6 ) == LUA_TLIGHTUSERDATA )
                pTarget = lua_toelement ( luaVM, 6 );
        }
    
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedLookAt ( *pEntity, vecPosition, iTime, pTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedLookAt", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedLookAt" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedHeadless ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bHeadless = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedHeadless ( *pEntity, bHeadless ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedHeadless", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedHeadless" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPedCameraRotation ( lua_State* luaVM )
{
	int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        float fRotation = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedCameraRotation ( *pEntity, fRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedCameraRotation", "ped", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedCameraRotation" );

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


int CLuaFunctionDefinitions::SetPedRotation ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        // Grab the entity
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
		float fRotation = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );

        // Valid element?
        if ( pEntity )
		{
            // Set the new rotation
            if ( CStaticFunctionDefinitions::SetPedRotation ( *pEntity, fRotation ) )
            {
			    lua_pushboolean ( luaVM, true );
				return 1;
			}
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedRotation", "ped", 1 );
	}
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedRotation" );

    // Failed
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::SetPedCanBeKnockedOffBike ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        // Grab the entity
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bCanBeKnockedOffBike = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        // Valid element?
        if ( pEntity )
		{
            // Set the new rotation
            if ( CStaticFunctionDefinitions::SetPedCanBeKnockedOffBike ( *pEntity, bCanBeKnockedOffBike ) )
            {
			    lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPedCanBeKnockedOffBike", "element", 1 );
	}
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPedCanBeKnockedOffBike" );

    // Failed
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::SetPedAnimation ( lua_State* luaVM )
{
    // Check types
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
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

            if ( CStaticFunctionDefinitions::SetPedAnimation ( *pEntity, szBlockName, szAnimName, iTime, bLoop, bUpdatePosition, bInteruptable ) )
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


int CLuaFunctionDefinitions::SetPedMoveAnim ( lua_State* luaVM )
{
    // Check types
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( lua_type ( luaVM, 2 ) == LUA_TNUMBER ) )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
		{
            unsigned int iMoveAnim = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );

            if ( CStaticFunctionDefinitions::SetPedMoveAnim ( *pEntity, iMoveAnim ) )
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

    // Failed
	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::CreatePed ( lua_State* luaVM )
{
    // Valid types?
    if ( ( lua_type ( luaVM, 1 ) == LUA_TNUMBER || lua_type ( luaVM, 1 ) == LUA_TSTRING ) &&
         ( lua_type ( luaVM, 2 ) == LUA_TNUMBER || lua_type ( luaVM, 2 ) == LUA_TSTRING ) &&
         ( lua_type ( luaVM, 3 ) == LUA_TNUMBER || lua_type ( luaVM, 3 ) == LUA_TSTRING ) &&
         ( lua_type ( luaVM, 4 ) == LUA_TNUMBER || lua_type ( luaVM, 4 ) == LUA_TSTRING ) )
    {
        // Grab arguments
        CVector vecPosition;
        unsigned long ulModel = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );
        vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
        vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

		float fRotation = 0.0f;
		if ( lua_type ( luaVM, 5 ) == LUA_TNUMBER || lua_type ( luaVM, 5 ) == LUA_TSTRING )
		{
			fRotation = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );
		}

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource * pResource = pLuaMain->GetResource();
			if ( pResource )
			{
                // Create it
                CClientPed* pPed = CStaticFunctionDefinitions::CreatePed ( *pResource, ulModel, vecPosition, fRotation );
                if ( pPed )
                {
                    // Return it
                    lua_pushelement ( luaVM, pPed );
                    return 1;
                }
            }
        }
    }

    // Failed
	lua_pushboolean ( luaVM, false );
	return 1;
}

int CLuaFunctionDefinitions::GetVehicleType ( lua_State* luaVM )
{
	unsigned long ucModel = 0;
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
	{
		CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle ) 
        {
            unsigned char ucColor1;
            unsigned char ucColor2;
            unsigned char ucColor3;
            unsigned char ucColor4;
            // TODO: Clean up, pretty sure this is supposed to be unsigned
            pVehicle->GetColor ( reinterpret_cast < char& > ( ucColor1 ),
                                 reinterpret_cast < char& > ( ucColor2 ),
                                 reinterpret_cast < char& > ( ucColor3 ),
                                 reinterpret_cast < char& > ( ucColor4 ) );

            lua_pushnumber ( luaVM, ucColor1 );
            lua_pushnumber ( luaVM, ucColor2 );
            lua_pushnumber ( luaVM, ucColor3 );
            lua_pushnumber ( luaVM, ucColor4 );
            return 4;
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            // Does the vehicle have landing gears?
            if ( CClientVehicleManager::HasLandingGears ( pVehicle->GetModel () ) )
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


int CLuaFunctionDefinitions::GetVehicleMaxPassengers ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned int uiMaxPassengers = CClientVehicleManager::GetMaxPassengerCount ( pVehicle->GetModel () );
            if ( uiMaxPassengers != 255 )
            {
                lua_pushnumber ( luaVM, uiMaxPassengers );
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CClientPed* pPed = pVehicle->GetOccupant ( uiSeat );
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CClientPed* pPed = pVehicle->GetControllingPlayer ();
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


int CLuaFunctionDefinitions::GetVehicleSirensOn ( lua_State* luaVM )
{
    // Verify the argument type
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        // Grab the vehicle pointer. Is it valid?
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            // Does the vehicle have Sirens?
            if ( CClientVehicleManager::HasSirens ( pVehicle->GetModel () ) )
            {
                // Return whether it has its Sirens on or not
                bool bSirensOn = pVehicle->IsSirenOrAlarmActive ();
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CVector vecTurnVelocity;
            pVehicle->GetTurnSpeed ( vecTurnVelocity );

            lua_pushnumber ( luaVM, vecTurnVelocity.fX );
            lua_pushnumber ( luaVM, vecTurnVelocity.fY );
            lua_pushnumber ( luaVM, vecTurnVelocity.fZ );
            return 3;
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CVector2D vecPosition;
            pVehicle->GetTurretRotation ( vecPosition.fX, vecPosition.fY );

            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            return 2;
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bLocked = pVehicle->AreDoorsLocked ();
            lua_pushboolean ( luaVM, bLocked );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleLocked", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleLocked" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleUpgradeOnSlot ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
	int iArgument2 = lua_type ( luaVM, 2 );
	if ( iArgument1 == LUA_TLIGHTUSERDATA &&
	   ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {			
			unsigned char ucSlot = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
	        CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
	        if ( pUpgrades )
	        {
		        unsigned short usUpgrade = pUpgrades->GetSlotState ( ucSlot );
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


int CLuaFunctionDefinitions::GetVehicleUpgrades ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
	if ( iArgument1 == LUA_TLIGHTUSERDATA )
	{
		CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
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


int CLuaFunctionDefinitions::GetVehicleUpgradeSlotName ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
	if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
		unsigned long ulNumber = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );
        
		if ( ulNumber < 17 )
		{
			char szUpgradeName [32];
			if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( static_cast < unsigned char > ( ulNumber ), szUpgradeName, sizeof(szUpgradeName) ) )
			{
				lua_pushstring ( luaVM, szUpgradeName );
				return 1;
			}
		}
		else if ( ulNumber >= 1000 && ulNumber <= 1193 )
		{
			char szUpgradeName [32];
			if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( static_cast < unsigned short > ( ulNumber ), szUpgradeName, sizeof(szUpgradeName) ) )
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


int CLuaFunctionDefinitions::GetVehicleCompatibleUpgrades ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
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


int CLuaFunctionDefinitions::GetVehicleWheelStates ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
	if ( iArgument1 == LUA_TLIGHTUSERDATA )
	{
		CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
		if ( pVehicle )
		{
            unsigned char ucFrontLeft = pVehicle->GetWheelStatus ( 0 );
            unsigned char ucFrontRight = pVehicle->GetWheelStatus ( 1 );
            unsigned char ucRearLeft = pVehicle->GetWheelStatus ( 2 );
            unsigned char ucRearRight = pVehicle->GetWheelStatus ( 3 );
            
			lua_pushnumber ( luaVM, ucFrontLeft );
            lua_pushnumber ( luaVM, ucFrontRight );
            lua_pushnumber ( luaVM, ucRearLeft );
            lua_pushnumber ( luaVM, ucRearRight );
			return 4;
		}
		else
			m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleWheelStates", "vehicle", 1 );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "getVehicleWheelStates" );
	
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
		CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
		if ( pVehicle )
		{
			unsigned char ucDoor = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
		    unsigned char ucState = pVehicle->GetDoorStatus ( ucDoor );
			lua_pushnumber ( luaVM, ucState );
			return 1;
		}
		else
			m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleDoorState", "vehicle", 1 );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "getVehicleDoorState" );
	
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
		CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
		if ( pVehicle )
		{
			unsigned char ucLight = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
			unsigned char ucState = pVehicle->GetLightStatus ( ucLight );
			lua_pushnumber ( luaVM, ucState );
			return 1;
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
		CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
		if ( pVehicle )
		{
			unsigned char ucPanel = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
			unsigned char ucState = pVehicle->GetPanelStatus ( ucPanel );
			lua_pushnumber ( luaVM, ucState );
			return 1;
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucLights = pVehicle->GetOverrideLights ();
            lua_pushnumber ( luaVM, ucLights );
            return 1;
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CClientVehicle* pTowedVehicle = pVehicle->GetTowedVehicle ();
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CClientVehicle* pTowedByVehicle = pVehicle->GetTowedByVehicle ();
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucPaintjob = pVehicle->GetPaintjob ();
            lua_pushnumber ( luaVM, ucPaintjob );
            return 1;
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            const char* szRegPlate = pVehicle->GetRegPlate ();
            if ( szRegPlate )
            {
                lua_pushstring ( luaVM, szRegPlate );
                return 1;
            }

            return false;
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bExplodable = pVehicle->GetCanShootPetrolTank ();
            lua_pushboolean ( luaVM, bExplodable );
            return 1;
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bFrozen = pVehicle->IsFrozen ();
            lua_pushboolean ( luaVM, bFrozen );
            return 1;
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );

        if ( pVehicle )
        {
            bool bOnGround = pVehicle->IsOnGround ();
            lua_pushboolean ( luaVM, bOnGround );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleOnGround", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleOnGround" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            const char* szVehicleName = CVehicleNames::GetVehicleName ( pVehicle->GetModel () );
            if ( szVehicleName )
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


int CLuaFunctionDefinitions::GetVehicleAdjustableProperty ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( pVehicle->HasAdjustableProperty () )
            {
                unsigned short usAdjustableProperty = pVehicle->GetAdjustablePropertyValue ();
                lua_pushnumber ( luaVM, usAdjustableProperty );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleAdjustableProperty", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleAdjustableProperty" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetHelicopterRotorSpeed ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            float fSpeed;
            if ( CStaticFunctionDefinitions::GetHelicopterRotorSpeed ( *pVehicle, fSpeed ) )
            {
                lua_pushnumber ( luaVM, fSpeed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getHelicopterRotorSpeed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getHelicopterRotorSpeed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetVehicleEngineState ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bState;
            if ( CStaticFunctionDefinitions::GetVehicleEngineState ( *pVehicle, bState ) )
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


int CLuaFunctionDefinitions::GetVehicleNameFromModel ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned short usModel = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );
        char szVehicleName [32];

        if ( CStaticFunctionDefinitions::GetVehicleNameFromModel ( usModel, szVehicleName, sizeof(szVehicleName) ) )
        {
            lua_pushstring ( luaVM, szVehicleName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleNameFromID" );

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

		CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource * pResource = pLuaMain->GetResource();
			if ( pResource )
			{
				// Create the vehicle and return its handle
				CClientVehicle* pVehicle = CStaticFunctionDefinitions::CreateVehicle ( *pResource, usModel, vecPosition, vecRotation, szRegPlate );
				if ( pVehicle )
				{
					CElementGroup * pGroup = pResource->GetElementGroup();
					if ( pGroup )
					{
						pGroup->Add ( ( CClientEntity* ) pVehicle );
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


int CLuaFunctionDefinitions::FixVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::FixVehicle ( *pEntity ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::BlowVehicle ( *pEntity, bExplode ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CVector vecTurnVelocity;
            vecTurnVelocity.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            vecTurnVelocity.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
            vecTurnVelocity.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

            if ( CStaticFunctionDefinitions::SetVehicleTurnVelocity ( *pEntity, vecTurnVelocity ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
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
                if ( CStaticFunctionDefinitions::SetVehicleColor ( *pEntity, static_cast < unsigned char > ( dColor1 ), static_cast < unsigned char > ( dColor2 ), static_cast < unsigned char > ( dColor3 ), static_cast < unsigned char > ( dColor4 ) ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bLandingGearDown = lua_toboolean ( luaVM, 2 ) ? true:false;
        if ( pEntity )
        {
            CClientVehicle* pVehicle = static_cast < CClientVehicle* > ( pEntity );
            if ( CClientVehicleManager::HasLandingGears ( pVehicle->GetModel () ) )
            {
                // Do it
                if ( CStaticFunctionDefinitions::SetVehicleLandingGearDown ( *pEntity, bLandingGearDown ) )
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


int CLuaFunctionDefinitions::SetVehicleLocked ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bLocked = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleLocked ( *pEntity, bLocked ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bDoorsUndamageable = ( lua_toboolean ( luaVM, 2 ) ) ? false:true;

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleDoorsUndamageable ( *pEntity, bDoorsUndamageable ) )
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


int CLuaFunctionDefinitions::SetVehicleSirensOn ( lua_State* luaVM )
{
    // Verify the two arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA && iArgument2 == LUA_TBOOLEAN )
    {
        // Grab the element and verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bSirensOn = lua_toboolean ( luaVM, 2 ) ? true:false;
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehicleSirensOn ( *pEntity, bSirensOn ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );

        unsigned short usUpgrade = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
        if ( pEntity )
        {
            if ( iArgument2 == LUA_TSTRING )
            {
                const char* szUpgrade = lua_tostring ( luaVM, 2 );
                if ( strcmp ( szUpgrade, "all" ) == 0 )
                {
                    if ( CStaticFunctionDefinitions::AddAllVehicleUpgrades ( *pEntity ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
            }

			if ( CStaticFunctionDefinitions::AddVehicleUpgrade ( *pEntity, usUpgrade ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned short usUpgrade = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
        if ( pEntity )
        {
            // Do it
			if ( CStaticFunctionDefinitions::RemoveVehicleUpgrade ( *pEntity, usUpgrade ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned char ucDoor = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
		unsigned char ucState = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehicleDoorState ( *pEntity, ucDoor, ucState ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleWheelStates ( *pEntity, iFrontLeft, iFrontRight, iRearLeft, iRearRight ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned char ucLight = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
		unsigned char ucState = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehicleLightState ( *pEntity, ucLight, ucState ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned char ucPanel = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
		unsigned char ucState = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehiclePanelState ( *pEntity, ucPanel, ucState ) )
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


int CLuaFunctionDefinitions::SetVehicleOverrideLights ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned char ucLights = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        if ( pEntity )
        {
            if ( ucLights <= 2 )
            {
                if ( CStaticFunctionDefinitions::SetVehicleOverrideLights ( *pEntity, ucLights ) )
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        CClientVehicle* pTrailer = lua_tovehicle ( luaVM, 2 );

        if ( pVehicle )
        {
            if ( pTrailer )
            {
                if ( CStaticFunctionDefinitions::AttachTrailerToVehicle ( *pVehicle, *pTrailer ) )
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        CClientVehicle* pTrailer = NULL;

        if ( lua_type ( luaVM, 2 )  == LUA_TLIGHTUSERDATA )
            pTrailer = lua_tovehicle ( luaVM, 2 );

        if ( pVehicle )
        {
            if ( CStaticFunctionDefinitions::DetachTrailerFromVehicle ( *pVehicle, pTrailer ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bState = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleEngineState ( *pEntity, bState ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        float fDirtLevel = static_cast < float > ( lua_tonumber ( luaVM, 2 ) ); /*( atof ( lua_tostring ( luaVM, 2 ) ) );*/

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleDirtLevel ( *pEntity, fDirtLevel ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bDamageProof = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleDamageProof ( *pEntity, bDamageProof ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned char ucPaintjob = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetVehiclePaintjob ( *pEntity, ucPaintjob ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            bool bExplodable = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
            if ( CStaticFunctionDefinitions::SetVehicleFuelTankExplodable ( *pEntity, bExplodable ) )
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
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            {
                if ( CStaticFunctionDefinitions::SetVehicleFrozen ( *pVehicle, lua_toboolean ( luaVM, 2 ) ? true:false ) )
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


int CLuaFunctionDefinitions::SetVehicleAdjustableProperty ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned short usAdjustableProperty = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetVehicleAdjustableProperty ( *pEntity, usAdjustableProperty ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleAdjustableProperty", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleAdjustableProperty" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetHelicopterRotorSpeed ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            float fSpeed = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetHelicopterRotorSpeed ( *pVehicle, fSpeed ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setHelicopterRotorSpeed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setHelicopterRotorSpeed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetElementCollisionsEnabled ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            bool bEnabled = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
            if ( CStaticFunctionDefinitions::SetElementCollisionsEnabled ( *pEntity, lua_toboolean ( luaVM, 2 ) ? true:false ) )
            {
                lua_pushboolean ( luaVM, true );    
                return 1;
            }        
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementCollisionsEnabled", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementCollisionsEnabled" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::CreateObject ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned short usModelID = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );

        if ( CClientObjectManager::IsValidModel ( usModelID ) )
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

				CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
				if ( pLuaMain )
				{
					CResource* pResource = pLuaMain->GetResource ();
					if ( pResource )
					{
						CClientObject* pObject = CStaticFunctionDefinitions::CreateObject ( *pResource, usModelID, vecPosition, vecRotation );
						if ( pObject )
						{
							CElementGroup * pGroup = pResource->GetElementGroup();
							if ( pGroup )
							{
								pGroup->Add ( ( CClientEntity* ) pObject );
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


int CLuaFunctionDefinitions::IsObjectStatic ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientObject* pObject = lua_toobject ( luaVM, 1 );
        if ( pObject )
        {
            bool bStatic;
            if ( CStaticFunctionDefinitions::IsObjectStatic ( *pObject, bStatic ) )
            {
                lua_pushboolean ( luaVM, bStatic );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isObjectStatic", "object", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isObjectStatic" );

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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
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

            if ( CStaticFunctionDefinitions::MoveObject ( *pEntity, ulTime, vecTargetPosition, vecTargetRotation ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::StopObject ( *pEntity ) )
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


int CLuaFunctionDefinitions::SetObjectScale ( lua_State* luaVM )
{
    int iArgumentType1 = lua_type ( luaVM, 1 );
    int iArgumentType2 = lua_type ( luaVM, 2 );

    if ( ( iArgumentType1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgumentType2 == LUA_TNUMBER || iArgumentType2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            float fScale = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );

            if ( CStaticFunctionDefinitions::SetObjectScale ( *pEntity, fScale ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setObjectScale", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setObjectScale" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetObjectStatic ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            bool bStatic = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

            if ( CStaticFunctionDefinitions::SetObjectStatic ( *pEntity, bStatic ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setObjectStatic", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setObjectStatic" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateExplosion ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        unsigned char ucType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
        bool bMakeSound = true;
        float fCamShake = -1.0f;
        bool bDamaging = true;
        if ( lua_istype ( luaVM, 5, LUA_TBOOLEAN ) )
        {
            bMakeSound = ( lua_toboolean ( luaVM, 5 ) ) ? true:false;

            int iArgument6 = lua_type ( luaVM, 6 );
            if ( ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
            {
                fCamShake = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );

                if ( lua_istype ( luaVM, 7, LUA_TBOOLEAN ) )
                {
                    bDamaging = ( lua_toboolean ( luaVM, 7 ) ) ? true:false;
                }
            }
        }

        if ( CStaticFunctionDefinitions::CreateExplosion ( vecPosition, ucType, bMakeSound, fCamShake, bDamaging ) )
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
	// Grab the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );

	if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
		float fSize = 1.8f;
		if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
			fSize = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

        if ( CStaticFunctionDefinitions::CreateFire ( vecPosition, fSize ) )
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
    // Grab the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );

    // Correct argument types?
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the sound and the position
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( lua_tonumber ( luaVM, 3 ) ) );
        unsigned short usSound = static_cast < short > ( lua_tonumber ( luaVM, 4 ) );

        // Play the sound
		if ( CStaticFunctionDefinitions::PlayMissionAudio ( vecPosition, usSound ) )
		{
			lua_pushboolean ( luaVM, true );
			return 1;
		}
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "playMissionAudio" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::PlaySoundFrontEnd ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
	if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
	{
		unsigned long ulSound = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );
        if ( ulSound <= 101 )
        {
			if ( CStaticFunctionDefinitions::PlaySoundFrontEnd ( ulSound ) )
			{
				lua_pushboolean ( luaVM, true );
				return 1;
			}
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "Invalid sound ID specified. Valid sound IDs are 0 - 101." );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "playSoundFrontEnd" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaFunctionDefinitions::PreloadMissionAudio ( lua_State* luaVM )
{
	int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );

    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
		 ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        unsigned short usSound = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );
		unsigned short usSlot = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );

		if ( CStaticFunctionDefinitions::PreloadMissionAudio ( usSound, usSlot ) )
		{
			lua_pushboolean ( luaVM, true );
			return 1;
		}
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "preloadMissionAudio" );

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
                                }
                            }
                        }
                    }
                }
            }
        }

		CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource ();
			if ( pResource )
			{
				// Create the blip
				CClientRadarMarker* pMarker = CStaticFunctionDefinitions::CreateBlip ( *pResource, vecPosition, ucIcon, ucSize, ucRed, ucGreen, ucBlue, ucAlpha, sOrdering );
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Default colors and size
            unsigned char ucIcon = 0;
            unsigned char ucSize = 2;
            unsigned char ucRed = 255;
            unsigned char ucGreen = 0;
            unsigned char ucBlue = 0;
            unsigned char ucAlpha = 255;
            short sOrdering = 0;

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
                                    }
                                }
                            }
                        }
                    }
                }
            }

			CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
			if ( pLuaMain )
			{
				CResource* pResource = pLuaMain->GetResource ();
				if ( pResource )
				{
					// Create the blip
					CClientRadarMarker* pMarker = CStaticFunctionDefinitions::CreateBlipAttachedTo ( *pResource, *pEntity, ucIcon, ucSize, ucRed, ucGreen, ucBlue, ucAlpha, sOrdering );
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
        CClientRadarMarker* pMarker = lua_toblip ( luaVM, 1 );
        if ( pMarker )
        {
            unsigned char ucIcon = static_cast < unsigned char > ( pMarker->GetSprite () );
            lua_pushnumber ( luaVM, ucIcon );
            return 1;
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
        CClientRadarMarker* pMarker = lua_toblip ( luaVM, 1 );
        if ( pMarker )
        {
            unsigned char ucSize = static_cast < unsigned char > ( pMarker->GetScale () );
            lua_pushnumber ( luaVM, ucSize );
            return 1;
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
        CClientRadarMarker* pMarker = lua_toblip ( luaVM, 1 );
        if ( pMarker )
        {
            unsigned char ucRed = pMarker->GetColorRed ();
            unsigned char ucGreen = pMarker->GetColorGreen ();
            unsigned char ucBlue = pMarker->GetColorBlue ();
            unsigned char ucAlpha = pMarker->GetColorAlpha ();

            lua_pushnumber ( luaVM, ucRed );
            lua_pushnumber ( luaVM, ucGreen );
            lua_pushnumber ( luaVM, ucBlue );
            lua_pushnumber ( luaVM, ucAlpha );
            return 4;
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
        CClientRadarMarker* pMarker = lua_toblip ( luaVM, 1 );
        if ( pMarker )
        {
            short sOrdering = pMarker->GetOrdering ();
            lua_pushnumber ( luaVM, sOrdering );
            return 1;
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned char ucIcon = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetBlipIcon ( *pEntity, ucIcon ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned char ucSize = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetBlipSize ( *pEntity, ucSize ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned char ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            unsigned char ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
            unsigned char ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
            unsigned char ucAlpha = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );

            if ( CStaticFunctionDefinitions::SetBlipColor ( *pEntity, ucRed, ucGreen, ucBlue, ucAlpha ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            short sOrdering = static_cast < short > ( lua_tonumber ( luaVM, 2 ) );

            if ( CStaticFunctionDefinitions::SetBlipOrdering ( *pEntity, sOrdering ) )
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
        unsigned char ucType = 0;
        float fSize = 4.0f;
        unsigned char ucRed = 0;
        unsigned char ucGreen = 0;
        unsigned char ucBlue = 255;
        unsigned char ucAlpha = 255;

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

		CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource();
			{
				// Create it
				CClientMarker* pMarker = CStaticFunctionDefinitions::CreateMarker ( *pResource, vecPosition, szType, fSize, ucRed, ucGreen, ucBlue, ucAlpha );
				if ( pMarker )
				{
                    CElementGroup * pGroup = pResource->GetElementGroup();
					if ( pGroup )
					{
						pGroup->Add ( ( CClientEntity* ) pMarker );
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
    unsigned int uiCount = m_pMarkerManager->Count ();
    lua_pushnumber ( luaVM, static_cast < lua_Number > ( uiCount ) );
    return 1;
}


int CLuaFunctionDefinitions::GetMarkerType ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientMarker* pMarker = lua_tomarker ( luaVM, 1 );
        if ( pMarker )
        {
            char szMarkerType [64];
            if ( CClientMarker::TypeToString ( pMarker->GetMarkerType (), szMarkerType ) )
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
        CClientMarker* pMarker = lua_tomarker ( luaVM, 1 );
        if ( pMarker )
        {
            float fSize = pMarker->GetSize ();
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( fSize ) );
            return 1;
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
        CClientMarker* pMarker = lua_tomarker ( luaVM, 1 );
        if ( pMarker )
        {
            unsigned char ucRed;
            unsigned char ucGreen;
            unsigned char ucBlue;
            unsigned char ucAlpha;
            pMarker->GetColor ( ucRed, ucGreen, ucBlue, ucAlpha );

            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucRed ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucGreen ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucBlue ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucAlpha ) );
            return 4;
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
        CClientMarker* pMarker = lua_tomarker ( luaVM, 1 );
        if ( pMarker )
        {
            CVector vecTarget;
            if ( CStaticFunctionDefinitions::GetMarkerTarget ( *pMarker, vecTarget ) )
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
        CClientMarker* pMarker = lua_tomarker ( luaVM, 1 );
        if ( pMarker )
        {
            char szMarkerIcon [64];
            CClientCheckpoint* pCheckpoint = pMarker->GetCheckpoint ();
            if ( pCheckpoint )
            {
                CClientCheckpoint::IconToString ( pCheckpoint->GetIcon (), szMarkerIcon );
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        const char* szType = lua_tostring ( luaVM, 2 );
        if ( pEntity )
        {
            bool bSuccess = CStaticFunctionDefinitions::SetMarkerType ( *pEntity, szType );
            lua_pushboolean ( luaVM, bSuccess );
            return 1;
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetMarkerSize ( *pEntity, static_cast < float > ( lua_tonumber ( luaVM, 2 ) ) ) )
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
    int iArgument5 = lua_type ( luaVM, 5 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Convert to numbers
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        double dRed = lua_tonumber ( luaVM, 2 );
        double dGreen = lua_tonumber ( luaVM, 3 );
        double dBlue = lua_tonumber ( luaVM, 4 );
        double dAlpha = 255;
        if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
            dAlpha = lua_tonumber ( luaVM, 5 );

        // Verify the pointer
        if ( pEntity )
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
                if ( CStaticFunctionDefinitions::SetMarkerColor ( *pEntity, ucRed, ucGreen, ucBlue, ucAlpha ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
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
            if ( CStaticFunctionDefinitions::SetMarkerTarget ( *pEntity, pTargetVector ) )
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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        const char* szIcon = lua_tostring ( luaVM, 2 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetMarkerIcon ( *pEntity, szIcon ) )
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
        if ( iArgument5 != LUA_TNONE && iArgument6 != LUA_TNONE && iArgument7 != LUA_TNONE && iArgument8 != LUA_TNONE )
        {
            dRed = lua_tonumber ( luaVM, 5 );
            dGreen = lua_tonumber ( luaVM, 6 );
            dBlue = lua_tonumber ( luaVM, 7 );
            dAlpha = lua_tonumber ( luaVM, 8 );
        }


        if ( dRed >= 0 && dRed <= 255 &&
             dGreen >= 0 && dGreen <= 255 &&
             dBlue >= 0 && dBlue <= 255 &&
             dAlpha >= 0 && dAlpha <= 255 )
        {     
			CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
			if ( pLuaMain )
			{
				CResource* pResource = pLuaMain->GetResource ();
				if ( pResource )
				{
					// Create it
					CClientRadarArea* pRadarArea = CStaticFunctionDefinitions::CreateRadarArea ( *pResource, vecPosition, vecSize, static_cast < unsigned char > ( dRed ), static_cast < unsigned char > ( dGreen ), static_cast < unsigned char > ( dBlue ), static_cast < unsigned char > ( dAlpha ) );
					if ( pRadarArea )
					{
						CElementGroup * pGroup = pResource->GetElementGroup();
						if ( pGroup )
						{
							pGroup->Add ( ( CClientEntity* ) pRadarArea );
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


int CLuaFunctionDefinitions::GetRadarAreaColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
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


int CLuaFunctionDefinitions::GetRadarAreaSize ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
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


int CLuaFunctionDefinitions::IsRadarAreaFlashing ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
			bool bFlashing = false;
			CStaticFunctionDefinitions::IsRadarAreaFlashing ( pRadarArea, bFlashing );
			lua_pushboolean ( luaVM, bFlashing );
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


int CLuaFunctionDefinitions::SetRadarAreaColor ( lua_State* luaVM )
{
    int iR = lua_type ( luaVM, 2 );
    int iG = lua_type ( luaVM, 3 );
    int iB = lua_type ( luaVM, 4 );
    int iA = lua_type ( luaVM, 5 );

    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iR == LUA_TNUMBER || iR == LUA_TSTRING ) &&
         ( iG == LUA_TNUMBER || iG == LUA_TSTRING ) &&
         ( iB == LUA_TNUMBER || iB == LUA_TSTRING ) &&
         ( iA == LUA_TNUMBER || iA == LUA_TSTRING ) )
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
            CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
            if ( pRadarArea )
            {
                if ( CStaticFunctionDefinitions::SetRadarAreaColor ( pRadarArea, static_cast < unsigned char > ( dRed ), static_cast < unsigned char > ( dGreen ), static_cast < unsigned char > ( dBlue ), static_cast < unsigned char > ( dAlpha ) ) )
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


int CLuaFunctionDefinitions::SetRadarAreaSize ( lua_State* luaVM )
{
    int iX = lua_type ( luaVM, 2 );
    int iY = lua_type ( luaVM, 3 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iX == LUA_TNUMBER || iX == LUA_TSTRING ) &&
         ( iY == LUA_TNUMBER || iY == LUA_TSTRING ) )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
            CVector2D vecSize = CVector2D ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ), static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
            if ( CStaticFunctionDefinitions::SetRadarAreaSize ( pRadarArea, vecSize ) )
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


int CLuaFunctionDefinitions::SetRadarAreaFlashing ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
            if ( CStaticFunctionDefinitions::SetRadarAreaFlashing ( pRadarArea, lua_toboolean ( luaVM, 2 ) ? true:false ) )
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


int CLuaFunctionDefinitions::IsInsideRadarArea ( lua_State* luaVM )
{
    int iX = lua_type ( luaVM, 2 );
    int iY = lua_type ( luaVM, 3 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iX == LUA_TNUMBER || iX == LUA_TSTRING ) &&
         ( iY == LUA_TNUMBER || iY == LUA_TSTRING ) )
    {
        CClientRadarArea* pRadarArea = lua_toradararea ( luaVM, 1 );
        if ( pRadarArea )
        {
			bool bInside = false;
            CVector2D vecPosition = CVector2D ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ), static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
            if ( CStaticFunctionDefinitions::IsInsideRadarArea ( pRadarArea, vecPosition, bInside ) )
            {
                lua_pushboolean ( luaVM, bInside );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isInsideRadarArea", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isInsideRadarArea" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreatePickup ( lua_State* luaVM )
{
    // Grab all the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    int iArgument7 = lua_type ( luaVM, 7 );

    // The first 6 are always numeric saying position, type and weapon/health/armor
    // TODO: Check argument 7 incase type is weapon
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
         ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
         ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING || iArgument6 == LUA_TNONE ) &&
         ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING || iArgument7 == LUA_TNONE ) )
    {
        // Populate a position vector for it
        CVector vecPosition = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                                        static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                                        static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        // Is the type health or armor?
        unsigned long ulRespawnInterval = 30000;
        double dblAmmo = 50.0;
        if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
            ulRespawnInterval = static_cast < unsigned long > ( lua_tonumber ( luaVM, 6 ) );

        if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
            dblAmmo = lua_tonumber ( luaVM, 7 );

		CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource ();
			if ( pResource )
			{
				CClientPickup* pPickup = CStaticFunctionDefinitions::CreatePickup ( 
										*pResource, 
										vecPosition, 
										static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) ), 
										lua_tonumber ( luaVM, 5 ), 
										ulRespawnInterval, 
										dblAmmo );
				if ( pPickup )
				{
					CElementGroup * pGroup = pResource->GetElementGroup();
					if ( pGroup )
					{
						pGroup->Add ( ( CClientEntity* ) pPickup );
					}
					
					// Return the handle
					lua_pushelement ( luaVM, pPickup );
					return 1;
				}
			}
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createPickup" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPickupType ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientPickup* pPickup = lua_topickup ( luaVM, 1 );
        if ( pPickup )
        {
            unsigned char ucType = pPickup->m_ucType;
            lua_pushnumber ( luaVM, ucType );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPickupType", "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPickupType" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPickupWeapon ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientPickup* pPickup = lua_topickup ( luaVM, 1 );
        if ( pPickup )
        {
            unsigned char ucWeapon = pPickup->m_ucWeaponType;
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucWeapon ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPickupWeapon", "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPickupWeapon" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPickupAmount ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientPickup* pPickup = lua_topickup ( luaVM, 1 );
        if ( pPickup )
        {
            float fAmount = pPickup->m_fAmount;
            lua_pushnumber ( luaVM, fAmount );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPickupAmount", "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPickupAmount" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetPickupAmmo ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientPickup* pPickup = lua_topickup ( luaVM, 1 );
        if ( pPickup )
        {
            unsigned short usAmmo = pPickup->m_usAmmo;
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( usAmmo ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getPickupAmmo", "pickup", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getPickupAmmo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetPickupType ( lua_State* luaVM )
{
    // The first 2 are always numeric saying weapon/health/armor
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetPickupType ( *pEntity, static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) ), lua_tonumber ( luaVM, 3 ), lua_tonumber ( luaVM, 4 ) ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setPickupType", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setPickupType" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GUIGetInputEnabled ( lua_State* luaVM )
{
	lua_pushboolean ( luaVM, CStaticFunctionDefinitions::GUIGetInputEnabled () );
	return 1;
}


int CLuaFunctionDefinitions::GUISetInputEnabled ( lua_State* luaVM )
{
	bool bRet = false;

	if ( lua_istype ( luaVM, 1, LUA_TBOOLEAN ) ) {
		CStaticFunctionDefinitions::GUISetInputEnabled ( lua_toboolean ( luaVM, 1 ) ? true : false );
		bRet = true;
	}	// else: error, bad arguments

	lua_pushboolean ( luaVM, bRet );
	return 1;
}


int CLuaFunctionDefinitions::GUIIsChatBoxInputActive ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pCore->IsChatInputEnabled () );
    return 1;
}


int CLuaFunctionDefinitions::GUIIsConsoleActive ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pCore->GetConsole ()->IsVisible () );
    return 1;
}


int CLuaFunctionDefinitions::GUIIsDebugViewActive ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pCore->IsDebugVisible () );
    return 1;
}


int CLuaFunctionDefinitions::GUIIsMainMenuActive ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pCore->IsSettingsVisible () || g_pCore->IsMenuVisible () );
    return 1;
}


int CLuaFunctionDefinitions::GUIIsMTAWindowActive ( lua_State* luaVM )
{
    bool bActive = g_pCore->IsChatInputEnabled () ||
                   g_pCore->IsSettingsVisible () ||
                   g_pCore->IsMenuVisible () ||
                   g_pCore->GetConsole ()->IsVisible () ||
                   g_pClientGame->GetTransferBox ()->IsVisible ();

    lua_pushboolean ( luaVM, bActive );
    return 1;
}


int CLuaFunctionDefinitions::GUIIsTransferBoxActive ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, g_pClientGame->GetTransferBox ()->IsVisible () );
    return 1;
}


int CLuaFunctionDefinitions::GUICreateWindow ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICreateLabel ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICreateStaticImage ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
	    if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) &&
		     lua_istype ( luaVM, 3, LUA_TNUMBER ) && lua_istype ( luaVM, 4, LUA_TNUMBER ) &&
		     lua_istype ( luaVM, 5, LUA_TSTRING ) && lua_istype ( luaVM, 6, LUA_TBOOLEAN ) )
	    {
		    const char *szFile = lua_tostring ( luaVM, 5 );
		    CClientGUIElement* pParent = lua_toguielement ( luaVM, 7 );

            CResource* pResource = NULL;
            if ( lua_istype ( luaVM, 8, LUA_TLIGHTUSERDATA ) )
            {
                pResource = lua_toresource ( luaVM, 8 );
            }

            if ( !pResource )
                pResource = pLuaMain->GetResource();

		    CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateStaticImage (
			    *pLuaMain,
			    static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
			    static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
			    static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
			    static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
			    szFile,
			    lua_toboolean ( luaVM, 6 ) ? true : false,
			    pParent,
                pResource
		    );

		    // Only proceed if we have a valid element (since GUICreateStaticImage can return a NULL pointer)
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


int CLuaFunctionDefinitions::GUICreateButton ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICreateProgressBar ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICreateCheckBox ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICreateRadioButton ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICreateEdit ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICreateMemo ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICreateGridList ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICreateScrollPane ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICreateScrollBar ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICreateTabPanel ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIStaticImageLoadImage ( lua_State* luaVM )
{
    bool bRet = false;
	CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
	    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
             lua_istype ( luaVM, 2, LUA_TSTRING ) )
	    {
		    // sanitize the input
		    const char *szFile = lua_tostring ( luaVM, 2 );
		    if ( IsValidFilePath ( szFile ) ) {

			    // get the correct directory
			    char szPath[MAX_PATH+1] = {0};
			    snprintf ( szPath, MAX_PATH, "%s/resources/%s/", m_pClientGame->GetModRoot (), pLuaMain->GetResource ()->GetName () );

			    // and attempt to load the image
			    CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
                if ( pEntity )
                {
			        bRet = CStaticFunctionDefinitions::GUIStaticImageLoadImage ( *pEntity, szFile, szPath );
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


int CLuaFunctionDefinitions::GUICreateTab ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIDeleteTab ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUISetText ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUISetFont ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIBringToFront ( lua_State* luaVM )
{
	if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
	{
		CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
		if ( pEntity )
        {
            CStaticFunctionDefinitions::GUIBringToFront ( *pEntity );

		    lua_pushboolean ( luaVM, true );
		    return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "guiBringToFront", "gui-element", 1 );
	}

	// error: bad arguments
	lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GUIMoveToBack ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIRadioButtonSetSelected ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICheckBoxSetSelected ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIRadioButtonGetSelected ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUICheckBoxGetSelected ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIProgressBarSetProgress ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIScrollBarSetScrollPosition ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIScrollPaneSetHorizontalScrollPosition ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIScrollPaneSetVerticalScrollPosition ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIProgressBarGetProgress ( lua_State* luaVM )
{
	if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
	{
		CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
		if ( pGUIElement && IS_CGUIELEMENT_PROGRESSBAR ( pGUIElement ) )
        {
            int iProgress = ( int ) ( static_cast < CGUIProgressBar* > ( pGUIElement->GetCGUIElement () ) -> GetProgress () * 100.0f );
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


int CLuaFunctionDefinitions::GUIScrollPaneGetHorizontalScrollPosition ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIScrollPaneGetVerticalScrollPosition ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIScrollBarGetScrollPosition ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGetText ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGetFont ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGetSize ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGetScreenSize ( lua_State* luaVM )
{
	const CVector2D Size = CStaticFunctionDefinitions::GUIGetScreenSize ();

	lua_pushnumber ( luaVM, Size.fX );
	lua_pushnumber ( luaVM, Size.fY );
	return 2;
}


int CLuaFunctionDefinitions::GUIGetPosition ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUISetAlpha ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGetAlpha ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUISetVisible ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUISetEnabled ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUISetProperty ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGetVisible ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGetEnabled ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGetProperty ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGetProperties ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUISetSize ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUISetPosition ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListSetSortingEnabled ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListAddColumn ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListRemoveColumn ( lua_State* luaVM )
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

int CLuaFunctionDefinitions::GUIGridListSetColumnWidth ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListAddRow ( lua_State* luaVM )
{
	int iRet;

	if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
	{
		CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
			iRet = CStaticFunctionDefinitions::GUIGridListAddRow ( *pGUIElement );
			if ( iRet >= 0 ) {
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


int CLuaFunctionDefinitions::GUIGridListInsertRowAfter ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListAutoSizeColumn ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListClear ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListSetSelectionMode ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListGetSelectedItem ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListSetSelectedItem ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListRemoveRow ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListGetItemText ( lua_State* luaVM )
{
	if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) && lua_istype ( luaVM, 3, LUA_TNUMBER ) )
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


int CLuaFunctionDefinitions::GUIGridListGetItemData ( lua_State* luaVM )
{
	if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) && lua_istype ( luaVM, 3, LUA_TNUMBER ) )
	{
		CClientGUIElement *pGUIElement = lua_toguielement ( luaVM, 1 );
        if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
        {
            // TODO: Is this safe? Sure the data can't be something else?
            const char* szData = reinterpret_cast < const char* > ( static_cast < CGUIGridList* > ( pGUIElement->GetCGUIElement () ) -> GetItemData ( static_cast < int > ( lua_tonumber ( luaVM, 2 ) ), 
                                                                                                                        static_cast < int > ( lua_tonumber ( luaVM, 3 ) ) ) );
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


int CLuaFunctionDefinitions::GUIGridListSetItemText ( lua_State* luaVM )
{
	if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) && lua_istype ( luaVM, 3, LUA_TNUMBER ) &&
		 lua_istype ( luaVM, 4, LUA_TSTRING ) && lua_istype ( luaVM, 5, LUA_TBOOLEAN ) && lua_istype ( luaVM, 6, LUA_TBOOLEAN ) )
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
			    lua_toboolean ( luaVM, 6 ) ? true : false
		    );

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


int CLuaFunctionDefinitions::GUIGridListSetItemData ( lua_State* luaVM )
{
	if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) && lua_istype ( luaVM, 2, LUA_TNUMBER ) && lua_istype ( luaVM, 3, LUA_TNUMBER ) &&
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


int CLuaFunctionDefinitions::GUIGridListSetScrollBars ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIScrollPaneSetScrollBars ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIGridListGetRowCount ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIEditSetReadOnly ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIMemoSetReadOnly ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIEditSetMasked ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIEditSetMaxLength ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIEditSetCaratIndex ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIMemoSetCaratIndex ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIWindowSetMovable ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIWindowSetSizable ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIWindowSetCloseButtonEnabled ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUIWindowSetTitleBarEnabled ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUILabelGetTextExtent ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUILabelGetFontHeight ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUILabelSetColor ( lua_State* luaVM )
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


int CLuaFunctionDefinitions::GUILabelSetVerticalAlign ( lua_State* luaVM )
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
            CLogger::ErrorPrintf ( "%s", "ERROR: No such align for guiLabelSetVerticalAlign" );
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


int CLuaFunctionDefinitions::GUILabelSetHorizontalAlign ( lua_State* luaVM )
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
            CLogger::ErrorPrintf ( "%s", "ERROR: No such align for guiLabelSetHorizontalAlign" );
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
        snprintf ( szDelimiter, 31, "%c", iDelimiter );
        szDelimiter [31] = 0;

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
    snprintf ( szDelimiter, 31, "%c", iDelimiter );

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


int CLuaFunctionDefinitions::GetTickCount_ ( lua_State* luaVM )
{
    unsigned long ulCount;
    if ( CStaticFunctionDefinitions::GetTickCount_ ( ulCount ) )
    {
        lua_pushnumber ( luaVM, ulCount );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetCTime ( lua_State* luaVM )
{
    time_t timer;
    time ( &timer );
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
    

int CLuaFunctionDefinitions::GetTime_ ( lua_State* luaVM )
{
    // Get the time
    unsigned char ucHour, ucMinute;
    if ( CStaticFunctionDefinitions::GetTime ( ucHour, ucMinute ) )
    {
        // Return it
        lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucHour ) );
        lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucMinute ) );
        return 2;
    }

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::tocolor ( lua_State* luaVM )
{
    // Grab argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );

    // Got first argument?
    if ( iArgument1 == LUA_TNUMBER )
    {
        // Got second and third? (R,G,B)
        if ( iArgument2 == LUA_TNUMBER &&
             iArgument3 == LUA_TNUMBER )
        {
            // Read red, green and blue
            unsigned char ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
            unsigned char ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            unsigned char ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );

            // Got a fourth too? (R,G,B,A) Defaults to 255.
            unsigned char ucAlpha = 255;
            if ( iArgument4 == LUA_TNUMBER )
            {
                ucAlpha = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
            }

            // Make it into an unsigned long
            unsigned long ulColor = COLOR_RGBA ( ucRed, ucGreen, ucBlue, ucAlpha );
            lua_pushinteger ( luaVM, static_cast < lua_Integer > ( ulColor ) );
            return 1;
        }
    }

    // Make it black so funcs dont break
    unsigned long ulColor = COLOR_RGBA ( 0, 0, 0, 255 );
    lua_pushnumber ( luaVM, static_cast < lua_Number > ( ulColor ) );
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


int CLuaFunctionDefinitions::GetGroundPosition ( lua_State* luaVM )
{
        // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CVector Vector ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                         static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                         static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        // Get the ground position and return it
        float fGround = g_pGame->GetWorld ()->FindGroundZFor3DPosition ( &Vector );
        lua_pushnumber ( luaVM, fGround );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getGroundPositionFrom3D" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetCameraMode ( lua_State* luaVM )
{
    char szMode [ 256 ];
    if ( CStaticFunctionDefinitions::GetCameraMode ( szMode, 256 ) )
    {
        lua_pushstring ( luaVM, szMode );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetCameraMatrix ( lua_State* luaVM )
{
    CVector vecPosition, vecLookAt;
    if ( CStaticFunctionDefinitions::GetCameraMatrix ( vecPosition, vecLookAt ) )
    {
        lua_pushnumber ( luaVM, vecPosition.fX );
        lua_pushnumber ( luaVM, vecPosition.fY );
        lua_pushnumber ( luaVM, vecPosition.fZ );
        lua_pushnumber ( luaVM, vecLookAt.fX );
        lua_pushnumber ( luaVM, vecLookAt.fY );
        lua_pushnumber ( luaVM, vecLookAt.fZ );
        return 6;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetCameraTarget ( lua_State* luaVM )
{
    CClientEntity* pTarget = CStaticFunctionDefinitions::GetCameraTarget ();
    if ( pTarget )
    {
        lua_pushelement ( luaVM, pTarget );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetCameraInterior ( lua_State* luaVM )
{
    unsigned char ucInterior;
    if ( CStaticFunctionDefinitions::GetCameraInterior ( ucInterior ) )
    {
        lua_pushnumber ( luaVM, ucInterior );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetCameraMatrix ( lua_State* luaVM )
{
    // Verify the parameter types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );

    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the parameters
        CVector vecPosition;
        vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
        CVector vecLookAt;
        int iArgument4 = lua_type ( luaVM, 4 );
        int iArgument5 = lua_type ( luaVM, 5 );
        int iArgument6 = lua_type ( luaVM, 6 );
        if ( ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
             ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
             ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
        {
            vecLookAt.fX = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
            vecLookAt.fY = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );
            vecLookAt.fZ = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
        }

        if ( CStaticFunctionDefinitions::SetCameraMatrix ( vecPosition, vecLookAt ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setCameraMatrix" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetCameraTarget ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the first argument
        CClientEntity * pTarget = lua_toelement ( luaVM, 1 );
        if ( pTarget )    
        {
            if ( CStaticFunctionDefinitions::SetCameraTarget ( pTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setCameraTarget", "target", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setCameraTarget" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetCameraInterior ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned char ucInterior = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );

        if ( CStaticFunctionDefinitions::SetCameraInterior ( ucInterior ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setCameraInterior" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::FadeCamera ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 ); // fadeIn
    int iArgument2 = lua_type ( luaVM, 2 ); // fadeTime
    int iArgument3 = lua_type ( luaVM, 3 ); // red
    int iArgument4 = lua_type ( luaVM, 4 ); // green
    int iArgument5 = lua_type ( luaVM, 5 ); // blue

    if ( iArgument1 == LUA_TBOOLEAN )
    {
        bool bFadeIn = lua_toboolean (luaVM, 1 ) ? true:false;
        unsigned char ucRed = 0;
        unsigned char ucGreen = 0;
        unsigned char ucBlue = 0;
        float fFadeTime = 1.0f;

        if ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER )
        {
            fFadeTime = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

            if ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER )
            {
                ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );

                if ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER )
                {
                    ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

                    if ( iArgument5 == LUA_TSTRING || iArgument5 == LUA_TNUMBER )
                    {
                       ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );
                    }
                }
           }
        }

        if ( CStaticFunctionDefinitions::FadeCamera ( bFadeIn, fFadeTime, ucRed, ucGreen, ucBlue ) )
        {
           lua_pushboolean ( luaVM, true );
           return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fadeCamera" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetCursorPosition ( lua_State* luaVM )
{
    CVector2D vecCursor;
    CVector vecWorld;
    if ( CStaticFunctionDefinitions::GetCursorPosition ( vecCursor, vecWorld ) )
    {
        lua_pushnumber ( luaVM, ( double ) vecCursor.fX );
        lua_pushnumber ( luaVM, ( double ) vecCursor.fY );
        lua_pushnumber ( luaVM, ( double ) vecWorld.fX );
        lua_pushnumber ( luaVM, ( double ) vecWorld.fY );
        lua_pushnumber ( luaVM, ( double ) vecWorld.fZ );
        return 5;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetCursorPosition ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
	{
		if ( lua_type ( luaVM, 2 ) == LUA_TNUMBER )
		{
 
			HWND hookedWindow = g_pCore->GetHookedWindow ();

			tagPOINT windowPos = { 0 };
			ClientToScreen( hookedWindow, &windowPos );

			CVector2D vecResolution = g_pCore->GetGUI ()->GetResolution ();

			float fX = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
			float fY = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
			
			if ( fX < 0 )
				fX = 0.0f;
			else if ( fX > vecResolution.fX )
				fX = vecResolution.fX;
			if ( fY < 0 )
				fY = 0.0f;
			else if ( fY > vecResolution.fY )
				fY = vecResolution.fY;

			g_pCore->CallSetCursorPos ( ( int ) fX + ( int ) windowPos.x, ( int ) fY + ( int ) windowPos.y );
			lua_pushboolean ( luaVM, true );
			return 1;
		}
		else
			m_pScriptDebugging->LogBadPointer ( luaVM, "getMarkerColor", "integer", 2 );
	}
	else
		m_pScriptDebugging->LogBadPointer ( luaVM, "getMarkerColor", "integer", 1 );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsCursorShowing ( lua_State* luaVM )
{
    bool bShowing = false;
    if ( CStaticFunctionDefinitions::IsCursorShowing ( bShowing ) )
    {
        lua_pushboolean ( luaVM, bShowing );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ShowCursor ( lua_State* luaVM )
{
    // Get the VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Boolean type passed?
        if ( lua_type ( luaVM, 1 ) == LUA_TBOOLEAN )
        {
            // Grab the argument
            bool bShow = lua_toboolean ( luaVM, 1 ) ?true:false;

            // Grab the resource belonging to this VM
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Show/hide it inside that resource
                pResource->ShowCursor ( bShow );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "showCursor" );
    }

    // Fail
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ProcessLineOfSight ( lua_State * luaVM )
{
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
        CVector vecStart ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecEnd (   static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        CColPoint* pColPoint = NULL;
        CClientEntity* pColEntity = NULL;
        bool bCheckBuildings = true, bCheckVehicles = true, bCheckPeds = true, bCheckObjects = true, bCheckDummies = true, bSeeThroughStuff = false, bIgnoreSomeObjectsForCamera = false, bShootThroughStuff = false;
        CEntity* pIgnoredEntity = NULL;

        if ( lua_type ( luaVM, 7 ) == LUA_TBOOLEAN )
        {
            bCheckBuildings = ( lua_toboolean ( luaVM, 7 ) ) ? true:false;
            if ( lua_type ( luaVM, 8 ) == LUA_TBOOLEAN )
            {
                bCheckVehicles = ( lua_toboolean ( luaVM, 8 ) ) ? true:false;
                if ( lua_type ( luaVM, 9 ) == LUA_TBOOLEAN )
                {
                    bCheckPeds = ( lua_toboolean ( luaVM, 9 ) ) ? true:false;
                    if ( lua_type ( luaVM, 10 ) == LUA_TBOOLEAN )
                    {
                        bCheckObjects = ( lua_toboolean ( luaVM, 10 ) ) ? true:false;
                        if ( lua_type ( luaVM, 11 ) == LUA_TBOOLEAN )
                        {
                            bCheckDummies = ( lua_toboolean ( luaVM, 11 ) ) ? true:false;
                            if ( lua_type ( luaVM, 12 ) == LUA_TBOOLEAN )
                            {
                                bSeeThroughStuff = ( lua_toboolean ( luaVM, 12 ) ) ? true:false;
                                if ( lua_type ( luaVM, 13 ) == LUA_TBOOLEAN )
                                {
                                    bIgnoreSomeObjectsForCamera = ( lua_toboolean ( luaVM, 13 ) ) ? true:false;
                                    if ( lua_type ( luaVM, 14 ) == LUA_TBOOLEAN )
                                    {
                                        bShootThroughStuff = ( lua_toboolean ( luaVM, 14 ) ) ? true:false;
                                    
                                        if ( lua_type ( luaVM, 15 ) == LUA_TLIGHTUSERDATA )
                                        {
                                            CClientEntity* pEntity = lua_toelement ( luaVM, 15 );
                                            if ( pEntity )
                                            {
                                                switch ( pEntity->GetType () )
                                                {
                                                    case CCLIENTPED:
                                                    case CCLIENTPLAYER:
                                                        pIgnoredEntity = static_cast < CClientPed* > ( pEntity )->GetGamePlayer ();
                                                        break;
                                                    case CCLIENTVEHICLE:
                                                        pIgnoredEntity = static_cast < CClientVehicle* > ( pEntity )->GetGameVehicle ();
                                                        break;
                                                    case CCLIENTOBJECT:
                                                        pIgnoredEntity = static_cast < CClientObject* > ( pEntity )->GetGameObject ();
                                                        break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        bool bCollision;
        if ( CStaticFunctionDefinitions::ProcessLineOfSight ( vecStart, vecEnd, bCollision, &pColPoint, &pColEntity, bCheckBuildings, bCheckVehicles, bCheckPeds, bCheckObjects, bCheckDummies, bSeeThroughStuff, bIgnoreSomeObjectsForCamera, bShootThroughStuff, pIgnoredEntity ) )
        {    
            // Got a collision?
            CVector vecColPosition;
            if ( pColPoint )
            {
                // Get the collision position
                vecColPosition = *pColPoint->GetPosition ();

                // Delete the colpoint
                pColPoint->Destroy ();
            }

            lua_pushboolean ( luaVM, bCollision );
            if ( bCollision )
            {
                lua_pushnumber ( luaVM, vecColPosition.fX );
                lua_pushnumber ( luaVM, vecColPosition.fY );
                lua_pushnumber ( luaVM, vecColPosition.fZ );
                if ( pColEntity )
                    lua_pushelement ( luaVM, pColEntity );
                else
                    lua_pushnil ( luaVM );
                return 5;
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "processLineOfSight" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsLineOfSightClear ( lua_State * luaVM )
{
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
        CVector vecStart ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecEnd (   static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        bool bCheckBuildings = true, bCheckVehicles = true, bCheckPeds = true, bCheckObjects = true, bCheckDummies = true, bSeeThroughStuff = false, bIgnoreSomeObjectsForCamera = false, bShootThroughStuff = false;
        CEntity* pIgnoredEntity = NULL;

        if ( lua_type ( luaVM, 7 ) == LUA_TBOOLEAN )
        {
            bCheckBuildings = ( lua_toboolean ( luaVM, 7 ) ) ? true:false;
            if ( lua_type ( luaVM, 8 ) == LUA_TBOOLEAN )
            {
                bCheckVehicles = ( lua_toboolean ( luaVM, 8 ) ) ? true:false;
                if ( lua_type ( luaVM, 9 ) == LUA_TBOOLEAN )
                {
                    bCheckPeds = ( lua_toboolean ( luaVM, 9 ) ) ? true:false;
                    if ( lua_type ( luaVM, 10 ) == LUA_TBOOLEAN )
                    {
                        bCheckObjects = ( lua_toboolean ( luaVM, 10 ) ) ? true:false;
                        if ( lua_type ( luaVM, 11 ) == LUA_TBOOLEAN )
                        {
                            bCheckDummies = ( lua_toboolean ( luaVM, 11 ) ) ? true:false;
                            if ( lua_type ( luaVM, 12 ) == LUA_TBOOLEAN )
                            {
                                bSeeThroughStuff = ( lua_toboolean ( luaVM, 12 ) ) ? true:false;
                                if ( lua_type ( luaVM, 13 ) == LUA_TBOOLEAN )
                                {
                                    if ( lua_type ( luaVM, 14 ) == LUA_TLIGHTUSERDATA )
                                    {
                                        CClientEntity* pEntity = lua_toelement ( luaVM, 14 );
                                        if ( pEntity )
                                        {
                                            switch ( pEntity->GetType () )
                                            {
                                                case CCLIENTPED:
                                                case CCLIENTPLAYER:
                                                    pIgnoredEntity = static_cast < CClientPed* > ( pEntity )->GetGamePlayer ();
                                                    break;
                                                case CCLIENTVEHICLE:
                                                    pIgnoredEntity = static_cast < CClientVehicle* > ( pEntity )->GetGameVehicle ();
                                                    break;
                                                case CCLIENTOBJECT:
                                                    pIgnoredEntity = static_cast < CClientObject* > ( pEntity )->GetGameObject ();
                                                    break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        bool bIsClear;
        if ( CStaticFunctionDefinitions::IsLineOfSightClear ( vecStart, vecEnd, bIsClear, bCheckBuildings, bCheckVehicles, bCheckPeds, bCheckObjects, bCheckDummies, bSeeThroughStuff, bIgnoreSomeObjectsForCamera, pIgnoredEntity ) )
        {        
            lua_pushboolean ( luaVM, bIsClear );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isLineOfSightClear" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::TestLineAgainstWater ( lua_State* luaVM )
{
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
        CVector vecStart ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        CVector vecEnd (   static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 5 ) ),
                           static_cast < float > ( lua_tonumber ( luaVM, 6 ) ) );

        CVector vecCollision;
        bool bCollision;
        if ( CStaticFunctionDefinitions::TestLineAgainstWater ( vecStart, vecEnd, bCollision, vecCollision ) )
        {
            lua_pushboolean ( luaVM, bCollision );
            if ( bCollision )
            {
                lua_pushnumber ( luaVM, vecCollision.fX );
                lua_pushnumber ( luaVM, vecCollision.fY );
                lua_pushnumber ( luaVM, vecCollision.fZ );
                return 4;
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "testLineAgainstWater" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetWaterLevel ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
        bool bCheckWaves = false;
        if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
            bCheckWaves = ( lua_toboolean ( luaVM, 4 ) ) ? true:false;

        float fWaterLevel;
        CVector vecUnknown;
        bool bFound;
        if ( CStaticFunctionDefinitions::GetWaterLevel ( vecPosition, bFound, fWaterLevel, bCheckWaves, vecUnknown ) )
        {
            if ( bFound )
            {
                lua_pushnumber ( luaVM, fWaterLevel );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getWaterLevel" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetWorldFromScreenPosition ( lua_State * luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CVector vecScreen ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        CVector vecWorld;
        if ( CStaticFunctionDefinitions::GetWorldFromScreenPosition ( vecScreen, vecWorld ) )
        {
            lua_pushnumber ( luaVM, vecWorld.fX );
            lua_pushnumber ( luaVM, vecWorld.fY );
            lua_pushnumber ( luaVM, vecWorld.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getWorldFromScreenPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetScreenFromWorldPosition ( lua_State * luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        CVector vecWorld ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        CVector vecScreen;
        if ( CStaticFunctionDefinitions::GetScreenFromWorldPosition ( vecWorld, vecScreen ) )
        {
            lua_pushnumber ( luaVM, vecScreen.fX );
            lua_pushnumber ( luaVM, vecScreen.fY );
            lua_pushnumber ( luaVM, vecScreen.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getScreenFromWorldPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::GetWeather ( lua_State* luaVM )
{
    unsigned char ucWeather, ucWeatherBlendingTo;
    if ( CStaticFunctionDefinitions::GetWeather ( ucWeather, ucWeatherBlendingTo ) )
    {
        lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucWeather ) );

        if ( ucWeatherBlendingTo != 0xFF )
		    lua_pushnumber ( luaVM, static_cast < lua_Number > ( ucWeatherBlendingTo ) );
        else
            lua_pushnil ( luaVM );
            
        return 2;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetZoneName ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( iArgument1 == LUA_TSTRING || iArgument1 == LUA_TNUMBER ) &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) &&
         ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        bool bCitiesOnly = false;
        if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
            bCitiesOnly = ( lua_toboolean ( luaVM, 4 ) ) ? true:false;

        char szZoneName [ 128 ];
        if ( CStaticFunctionDefinitions::GetZoneName ( vecPosition, szZoneName, 128, bCitiesOnly ) )
        {
            lua_pushstring ( luaVM, szZoneName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getZoneName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetGravity ( lua_State* luaVM )
{
    float fGravity;
    if ( CStaticFunctionDefinitions::GetGravity ( fGravity ) )
    {
        lua_pushnumber ( luaVM, fGravity );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetGameSpeed ( lua_State* luaVM )
{
    float fSpeed;
    if ( CStaticFunctionDefinitions::GetGameSpeed ( fSpeed ) )
    {
        lua_pushnumber ( luaVM, fSpeed );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetMinuteDuration ( lua_State* luaVM )
{
    unsigned long ulDelay;
    if ( CStaticFunctionDefinitions::GetMinuteDuration ( ulDelay ) )
    {
        lua_pushnumber ( luaVM, ulDelay );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetWaveHeight ( lua_State* luaVM )
{
    float fHeight;
    if ( CStaticFunctionDefinitions::GetWaveHeight ( fHeight ) )
    {
        lua_pushnumber ( luaVM, fHeight );
        return 1;
    }
    
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::IsGarageOpen ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        bool bIsOpen;

        if ( CStaticFunctionDefinitions::IsGarageOpen ( ucGarageID, bIsOpen ) )
        {
            lua_pushboolean ( luaVM, bIsOpen );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isGarageOpen" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetGaragePosition ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        CVector vecPosition;

        if ( CStaticFunctionDefinitions::GetGaragePosition ( ucGarageID, vecPosition ) )
        {
            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            lua_pushnumber ( luaVM, vecPosition.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "GetGaragePosition" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "GetGaragePosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetGarageSize ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        float fDepth;
        float fWidth;
        float fHeight;

        if ( CStaticFunctionDefinitions::GetGarageSize ( ucGarageID, fHeight, fWidth, fDepth ) )
        {
            lua_pushnumber ( luaVM, fHeight );
            lua_pushnumber ( luaVM, fWidth );
            lua_pushnumber ( luaVM, fDepth );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "GetGarageSize" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "GetGarageSize" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetGarageBoundingBox ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
        float fLeft;
        float fRight;
        float fFront;
        float fBack;

        if ( CStaticFunctionDefinitions::GetGarageBoundingBox ( ucGarageID, fLeft, fRight, fFront, fBack ) )
        {
            lua_pushnumber ( luaVM, fLeft );
            lua_pushnumber ( luaVM, fRight );
            lua_pushnumber ( luaVM, fFront );
            lua_pushnumber ( luaVM, fBack );
            return 4;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "GetGarageBoundingBox" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "GetGarageBoundingBox" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetTime ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetTime ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) ), static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTime" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetSkyGradient ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );

    unsigned char ucTopRed = 0;
    unsigned char ucTopGreen = 0;
    unsigned char ucTopBlue = 0;
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) )
        ucTopRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );
    if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
        ucTopGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
    if ( ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
        ucTopBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
    unsigned char ucBottomRed = 0;
    unsigned char ucBottomGreen = 0;
    unsigned char ucBottomBlue = 0;
    if ( ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
        ucBottomRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
    if ( ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
        ucBottomGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );
    if ( ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
        ucBottomBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 6 ) );
        // Set the new sky gradient
    if ( CStaticFunctionDefinitions::SetSkyGradient ( ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue ) )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ResetSkyGradient ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::ResetSkyGradient () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetWeather ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetWeather ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWeather" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;        
}


int CLuaFunctionDefinitions::SetWeatherBlended ( lua_State* luaVM )
{
    // Verify the argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        // Set the new time
        if ( CStaticFunctionDefinitions::SetWeatherBlended ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) ) ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWeatherBlended" );

    // Return false
    lua_pushboolean ( luaVM, false );
    return 1;        
}


int CLuaFunctionDefinitions::SetGravity ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        float fGravity = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetGravity ( fGravity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setGravity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetGameSpeed ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        float fSpeed = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetGameSpeed ( fSpeed ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setGameSpeed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetMinuteDuration ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned long ulDelay = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetMinuteDuration ( ulDelay ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setMinuteDuration" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetWaveHeight ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        float fHeight = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        if ( CStaticFunctionDefinitions::SetWaveHeight ( fHeight ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setWaveHeight" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SetGarageOpen ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER && lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        unsigned char ucGarageID = static_cast < unsigned char > ( lua_tonumber( luaVM, 1 ) );
        bool bIsOpen = ( ( lua_toboolean ( luaVM, 2 ) == 0 ) ? false : true );

        if ( CStaticFunctionDefinitions::SetGarageOpen ( ucGarageID, bIsOpen ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setGarageOpen" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::BindKey ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING &&
             lua_type ( luaVM, 2 ) == LUA_TSTRING &&
             lua_type ( luaVM, 3 ) == LUA_TFUNCTION ) 
        {
            const char* szKey = lua_tostring ( luaVM, 1 );
            const char* szHitState = lua_tostring ( luaVM, 2 );  
            // Jax: grab our arguments first, luaM_toref pops the stack!
            CLuaArguments Arguments;
		    Arguments.ReadArguments ( luaVM, 4 );
			int iLuaFunction = luaM_toref ( luaVM, 3 );            
                      
			if ( VERIFY_FUNCTION ( iLuaFunction ) )
            {
				if ( CStaticFunctionDefinitions::BindKey ( szKey, szHitState, pLuaMain, iLuaFunction, Arguments ) )
				{
					lua_pushboolean ( luaVM, true );
					return 1;
				}
			}
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "bindKey", "function", 3 );
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
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
        {
            const char* szKey = lua_tostring ( luaVM, 1 );
            const char* szHitState = NULL;
            int iLuaFunction = 0;

            if ( lua_type ( luaVM, 2 ) )
                szHitState = lua_tostring ( luaVM, 2 );
            if ( lua_type ( luaVM, 3 ) == LUA_TFUNCTION )
                iLuaFunction = luaM_toref ( luaVM, 3 );

            if ( iLuaFunction == 0 || VERIFY_FUNCTION ( iLuaFunction ) )
            {
			    if ( CStaticFunctionDefinitions::UnbindKey ( szKey, pLuaMain, szHitState, iLuaFunction ) )
			    {
				    lua_pushboolean ( luaVM, true );
				    return 1;
			    }
            }
            else
            {
                m_pScriptDebugging->LogBadType ( luaVM, "unbindKey" );
            }
        }
        else
        {
            m_pScriptDebugging->LogBadType ( luaVM, "unbindKey" );
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetKeyState ( lua_State * luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        const char* szKey = lua_tostring ( luaVM, 1 );
        bool bState;
        if ( CStaticFunctionDefinitions::GetKeyState ( szKey, bState ) )
        {
            lua_pushboolean ( luaVM, bState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getKeyType" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetControlState ( lua_State * luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        const char* szControl = lua_tostring ( luaVM, 1 );
        bool bState;
        if ( CStaticFunctionDefinitions::GetControlState ( szControl , bState ) )
        {
            lua_pushboolean ( luaVM, bState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getControlType" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetAnalogControlState ( lua_State * luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        const char* szControl = lua_tostring ( luaVM, 1 );
        float fState;
        if ( CStaticFunctionDefinitions::GetAnalogControlState ( szControl , fState ) )
        {
            lua_pushnumber ( luaVM, fState );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getAnalogControlState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFunctionDefinitions::IsControlEnabled ( lua_State * luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        const char* szControl = lua_tostring ( luaVM, 1 );
        bool bEnabled;
        if ( CStaticFunctionDefinitions::IsControlEnabled ( szControl, bEnabled ) )
        {
            lua_pushboolean ( luaVM, bEnabled );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isControlEnabled" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetBoundKeys ( lua_State * luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        const char * szControl = lua_tostring ( luaVM, 1 );
        SBindableGTAControl * pControl = g_pCore->GetKeyBinds ()->GetBindableFromControl ( szControl );
        // Did we find a control matching the string given?
        if ( pControl )
        {
            list < CGTAControlBind * > controlBinds;
            g_pCore->GetKeyBinds ()->GetBoundControls ( pControl, controlBinds );
            if ( !controlBinds.empty () )
            {
                lua_newtable ( luaVM );
                unsigned int uiIndex = 0;
                list < CGTAControlBind * > ::iterator iter = controlBinds.begin ();
                for ( ; iter != controlBinds.end () ; iter++ )
                {
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushstring ( luaVM, (*iter)->boundKey->szKey );
                    lua_settable ( luaVM, -3 );
                }
            }
            else
                lua_pushboolean ( luaVM, false );
            return 1;
        }
        // If not, assume its a command
        else
        {
            list < CCommandBind * > commandBinds;
            g_pCore->GetKeyBinds ()->GetBoundCommands ( szControl, commandBinds );
            if ( !commandBinds.empty () )
            {
                lua_newtable ( luaVM );
                unsigned int uiIndex = 0;
                list < CCommandBind * > ::iterator iter = commandBinds.begin ();
                for ( ; iter != commandBinds.end () ; iter++ )
                {
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushstring ( luaVM, (*iter)->boundKey->szKey );
                    lua_settable ( luaVM, -3 );
                }
            }
            else
                lua_pushboolean ( luaVM, false );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getBoundKeys" );
        
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetControlState ( lua_State * luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        const char* szControl = lua_tostring ( luaVM, 1 );
        bool bState = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
        if ( CStaticFunctionDefinitions::SetControlState ( szControl, bState ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setControlState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ToggleControl ( lua_State * luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) && lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        const char* szControl = lua_tostring ( luaVM, 1 );
        bool bEnabled = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
        if ( CStaticFunctionDefinitions::ToggleControl ( szControl, bEnabled ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "toggleControl" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::ToggleAllControls ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TBOOLEAN )
    {
        bool bEnabled = ( lua_toboolean ( luaVM, 1 ) ) ? true:false;
        bool bGTAControls = true, bMTAControls = true;

        if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            bGTAControls = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
            bMTAControls = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;

        if ( CStaticFunctionDefinitions::ToggleAllControls ( bGTAControls, bMTAControls, bEnabled ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "toggleAllControls" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLNodeFindChild ( lua_State* luaVM )
{
    // xmlNode*, node name, index
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA ||
         lua_type ( luaVM, 2 ) != LUA_TSTRING || 
         lua_type ( luaVM, 3 ) != LUA_TNUMBER )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeFindChild" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        // Grab the XML node
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            const char * szTagName = lua_tostring ( luaVM, 2 );
            unsigned int iIndex = static_cast < unsigned int > ( lua_tonumber ( luaVM, 3 ) );
            CXMLNode * pFoundNode = pNode->FindSubNode ( szTagName, iIndex ); 
            if ( pFoundNode )
            {
                lua_pushxmlnode ( luaVM, pFoundNode );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLNodeGetChildren ( lua_State* luaVM )
{
    // xmlNode*, [index]
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            unsigned int uiIndex = 0;
            bool bGetAllChildren = true;
            int iArgument2 = lua_type ( luaVM, 2 );
            if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
            {
                uiIndex = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
                bGetAllChildren = false;
            }
            CXMLNode * pFoundNode = NULL;
            if ( !bGetAllChildren )
            {
                pFoundNode = pNode->GetSubNode ( uiIndex );
                if ( pFoundNode )
                {
                    lua_pushxmlnode ( luaVM, pFoundNode );
                    return 1;
                }
            }
            else
            {
		        lua_newtable ( luaVM );
                uiIndex = 0;
                list < CXMLNode * > ::iterator iter = pNode->ChildrenBegin ();
                for ( ; iter != pNode->ChildrenEnd () ; iter++ )
                {                
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushxmlnode ( luaVM, ( CXMLNode * ) ( *iter ) );
                    lua_settable ( luaVM, -3 );
                }
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetChildren" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLNodeGetValue ( lua_State* luaVM )
{
    // pNode, [Buffer Size]
    int iSecondVariableType = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA || 
        !( iSecondVariableType == LUA_TNONE || iSecondVariableType == LUA_TNUMBER ) )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetValue" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {  
            unsigned int iBufferSize = 1024;
            if ( iSecondVariableType == LUA_TNUMBER )
            {
                iBufferSize = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
                if ( iBufferSize > 1024 )
                    iBufferSize = 255;
            }

            lua_pushstring ( luaVM, pNode->GetTagContent ().c_str () );
            return 1;
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLNodeSetValue ( lua_State* luaVM )
{
    // pNode, Value
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA || 
         lua_type ( luaVM, 2 ) != LUA_TSTRING )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeSetValue" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {  
            const char * szTagContents = lua_tostring ( luaVM, 2 );
            pNode->SetTagContent ( szTagContents );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLNodeGetName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetName" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            if ( pNode )
            {
                lua_pushstring ( luaVM, pNode->GetTagName ().c_str () );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLNodeSetName ( lua_State* luaVM )
{
    int iType1 = lua_type ( luaVM, 1 );
    int iType2 = lua_type ( luaVM, 2 );
    if ( ( iType1 == LUA_TLIGHTUSERDATA ) && 
         ( iType2 == LUA_TSTRING ) )
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
			std::string strTagName ( lua_tostring ( luaVM, 2 ) );
			pNode->SetTagName ( strTagName );
            lua_pushboolean ( luaVM, true );
			return 1;
        }
		else
			m_pScriptDebugging->LogBadPointer ( luaVM, "xmlNodeSetName", "xml-node", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeSetName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLNodeGetAttributes ( lua_State* luaVM )
{
    // pNode, Attribute Name, [Buffer Size]
    int iThirdVariableType = lua_type ( luaVM, 3 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
	{
		CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
			lua_newtable ( luaVM );
			unsigned int uiIndex = 0;
			list < CXMLAttribute * > ::iterator iter = pNode->GetAttributes ().ListBegin ();
			for ( ; iter != pNode->GetAttributes ().ListEnd () ; iter++ )
            {
				lua_pushstring ( luaVM, ( *iter )->GetName ().c_str () );
				lua_pushstring ( luaVM, ( *iter )->GetValue ().c_str () );
				lua_settable ( luaVM, -3 );
			}
			return 1;
		}
		else
			m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetAttributes" );
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetAttributes" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLNodeGetAttribute ( lua_State* luaVM )
{
    // pNode, Attribute Name, [Buffer Size]
    int iThirdVariableType = lua_type ( luaVM, 3 );
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA || 
        !( iThirdVariableType == LUA_TNONE || iThirdVariableType == LUA_TNUMBER ) ||
        lua_type ( luaVM, 2 ) != LUA_TSTRING )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetAttribute" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        // Grab the XML node
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {  
            // Find the attribute with that name
            const char * szAttributeName = lua_tostring ( luaVM, 2 );
            CXMLAttribute * pAttribute = pNode->GetAttributes ().Find ( szAttributeName ); 
            if ( pAttribute )
            {
                // Limit reading to that many characters
                unsigned int iBufferSize = 255;
                if ( iThirdVariableType == LUA_TNUMBER )
                {
                    iBufferSize = static_cast < unsigned int > ( lua_tonumber ( luaVM, 3 ) );
                    if ( iBufferSize > 1024 )
                        iBufferSize = 255;
                }

                // Read the attribute and return the string
                lua_pushstring ( luaVM, pAttribute->GetValue ().c_str () );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLNodeSetAttribute ( lua_State* luaVM )
{
    // pNode, Attribute Name, Value
    int iType1 = lua_type ( luaVM, 1 );
    int iType2 = lua_type ( luaVM, 2 );
    int iType3 = lua_type ( luaVM, 3 );
    if ( ( iType1 == LUA_TLIGHTUSERDATA ) && 
         ( iType2 == LUA_TSTRING ) &&
         ( iType3 == LUA_TSTRING || iType3 == LUA_TNUMBER || iType3 == LUA_TNIL ) )
    {
        // Grab the xml node
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            // Grab the attribute name and value
            const char * szAttributeName = lua_tostring ( luaVM, 2 );

            // Are we going to set it to a value?
            if ( iType3 == LUA_TSTRING || iType3 == LUA_TNUMBER )
            {
                const char * szAttributeValue = lua_tostring ( luaVM, 3 );

                // Write the node
                CXMLAttribute* pAttribute = pNode->GetAttributes ().Create ( szAttributeName );
                if ( pAttribute )
                {
                    pAttribute->SetValue ( szAttributeValue );

                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
            {
                // Delete the attribute if it exists
                CXMLAttribute* pAttribute = pNode->GetAttributes ().Find ( szAttributeName );
                if ( pAttribute )
                {
                    delete pAttribute;

                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeSetAttribute" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLNodeGetParent ( lua_State* luaVM )
{
    // xmlNode*, node name, index
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA  )
    {
        // Grab the XML node
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {            
            CXMLNode * pParent = pNode->GetParent ();
            if ( pParent )
            {
                lua_pushxmlnode ( luaVM, pParent );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetParent" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLLoadFile ( lua_State* luaVM )
{
    // Filename
    if ( lua_type ( luaVM, 1 ) != LUA_TSTRING )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlLoadFile" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            //const char * szFilename = lua_tostring ( luaVM, 1 );
		    char szFilename [ MAX_STRING_LENGTH ];
            snprintf ( szFilename, MAX_STRING_LENGTH, "%s\\%s", luaMain->GetResource()->GetResourceDirectoryPath(), lua_tostring ( luaVM, 1 ) );
            //if ( IsValidFilePath ( szFilename ) ) // This would be checking the full path when we only need to check the user input
		    if ( IsValidFilePath ( lua_tostring ( luaVM, 1 ) ) )
            {
                // Create the XML
                CXMLFile * xmlFile = luaMain->CreateXML ( szFilename );
                if ( xmlFile )
                {
                    // Parse it
                    if ( xmlFile->Parse() )
                    {
                        // Create the root node if it doesn't exist
                        CXMLNode* pRootNode = xmlFile->GetRootNode ();
                        if ( !pRootNode )
                            pRootNode = xmlFile->CreateRootNode ( "root" );
                        
                        // Got a root node?
                        if ( pRootNode )
                        {
                            lua_pushxmlnode ( luaVM, pRootNode );
                            return 1;
                        }
                    }

                    // Destroy the XML
                    luaMain->DestroyXML ( xmlFile );
                }	
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::XMLCreateFile ( lua_State* luaVM )
{
    // Filename
    if ( lua_type ( luaVM, 1 ) != LUA_TSTRING ||  lua_type ( luaVM, 2 ) != LUA_TSTRING )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlCreateFile" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if ( pLuaMain )
        {
            // Construct file path
            std::string strFile = m_pClientGame->GetModRoot () + std::string ( "\\resources\\" ) + pLuaMain->GetResource ()->GetName () + std::string ( "\\" ) + lua_tostring ( luaVM, 1 );

            // Make sure the original filename is valid
            if ( IsValidFilePath ( lua_tostring ( luaVM, 1 ) ) )
            {
                char szRootName [ MAX_STRING_LENGTH ];
		        strncpy ( szRootName,  lua_tostring ( luaVM, 2 ), MAX_STRING_LENGTH - 1 );

                // Make sure the directory exists
                MakeSureDirExists ( strFile.c_str () );

                // Create the XML
                CXMLFile * xmlFile = pLuaMain->CreateXML ( strFile.c_str () );
                if ( xmlFile )
                {
                    // Create its root node
                    CXMLNode* pRootNode = xmlFile->CreateRootNode ( szRootName );
                    if ( pRootNode )
                    {
                        lua_pushxmlnode ( luaVM, pRootNode );
                        return 1;
                    }

                    // Delete the XML again
                    pLuaMain->DestroyXML ( xmlFile );
                }
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::XMLSaveFile ( lua_State* luaVM )
{
    // CXMLNode*
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlSaveFile" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( luaMain )
            {
                luaMain->SaveXML ( pNode );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLUnloadFile ( lua_State* luaVM )
{
    // CXMLNode*
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlUnloadFile" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( luaMain )
            {
                luaMain->DestroyXML ( pNode );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLCreateChild ( lua_State* luaVM )
{
    // Node name
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA ||
		 lua_type ( luaVM, 2 ) != LUA_TSTRING )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlCreateChild" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
		// Get the Node
		CXMLNode* pXMLNode = lua_toxmlnode ( luaVM, 1 );
		char szSubNodeName [ MAX_STRING_LENGTH ];
		if ( pXMLNode )
		{
			snprintf ( szSubNodeName, MAX_STRING_LENGTH, "%s", lua_tostring ( luaVM, 2 ) );
			if ( szSubNodeName )
			{
				CXMLNode* pXMLSubNode = pXMLNode->CreateSubNode ( szSubNodeName );
				lua_pushxmlnode ( luaVM, pXMLSubNode );
				return 1;
			}
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLDestroyNode ( lua_State* luaVM )
{
    // Verify the argument type
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
		// Get the Node
		CXMLNode* pXMLNode = lua_toxmlnode ( luaVM, 1 );
		if ( pXMLNode )
		{
            // Grab the parent so that we can delete this node from it
            CXMLNode* pParent = pXMLNode->GetParent ();
            if ( pParent )
            {
                // Delete it
                pParent->DeleteSubNode ( pXMLNode );

                // Return success
			    lua_pushboolean ( luaVM, true );
			    return 1;
			}
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "xmlDestroyNode" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::XMLCopyFile ( lua_State* luaVM )
{
    // Grab the virtual machine for this luastate
    CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLUA )
    {
        // Verify the argument types passed
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA  &&
             lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            // Grab the full filepath of the copied xml and make sure its legal
            char szFilename [ MAX_STRING_LENGTH ];
		    snprintf ( szFilename, MAX_STRING_LENGTH, "%s\\%s", m_pClientGame->GetModRoot(), lua_tostring ( luaVM, 2 ) );
		    if ( IsValidFilePath ( lua_tostring ( luaVM, 2 ) ) )
            {
                // Grab the source node
                CXMLNode* pSourceNode = lua_toxmlnode ( luaVM, 1 );
                if ( pSourceNode )
                {
                    // Grab the roots tag name
                    std::string strRootTagName;
                    strRootTagName = pSourceNode->GetTagName ();

                    // Grab our lua VM
                    CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine ( luaVM );

                    // Create the new XML file and its root node
                    CXMLFile* pNewXML = pLUA->CreateXML ( szFilename );
                    if ( pNewXML )
                    {
                        // Create root for new XML
                        CXMLNode* pNewRoot = pNewXML->CreateRootNode ( strRootTagName );
                        if ( pNewRoot )
                        {
                            // Copy over the attributes from the root
                            int iAttributeCount = pSourceNode->GetAttributes ().Count ();
                            int i = 0;
                            CXMLAttribute* pAttribute;
                            for ( ; i < iAttributeCount; i++ )
                            {
                                pAttribute = pSourceNode->GetAttributes ().Get ( i );
                                if ( pAttribute )
                                    pNewRoot->GetAttributes ().Create ( *pAttribute );
                            }

                            // Copy the stuff from the given source node to the destination root
                            if ( pSourceNode->CopyChildrenInto ( pNewRoot, true ) )
                            {
                                lua_pushxmlnode ( luaVM, pNewRoot );
                                return 1;
                            }
                        }

                        // Delete the XML again
                        pLUA->DestroyXML ( pNewXML );
                    }
                }
            }
            else
                CLogger::ErrorPrintf ( "Unable to copy xml file; bad filepath" );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "xmlCopyFile" );
    }

    // Error
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::CreateProjectile ( lua_State* luaVM )
{
    if ( ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) ) &&
         ( lua_istype ( luaVM, 2, LUA_TNUMBER ) || lua_istype ( luaVM, 2, LUA_TSTRING ) ) )
    {
        CClientEntity* pCreator = lua_toelement ( luaVM, 1 );
        if ( pCreator )
        {
            unsigned char ucWeaponType = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            CVector vecOrigin;
            pCreator->GetPosition ( vecOrigin );
            float fForce = 1.0f;
            CClientEntity* pTarget = NULL;
            CVector *pvecRotation = NULL, *pvecMoveSpeed = NULL;
            unsigned short usModel = 0;
            if ( ( lua_istype ( luaVM, 3, LUA_TNUMBER ) || lua_istype ( luaVM, 3, LUA_TSTRING ) ) &&
                 ( lua_istype ( luaVM, 4, LUA_TNUMBER ) || lua_istype ( luaVM, 4, LUA_TSTRING ) ) &&
                 ( lua_istype ( luaVM, 5, LUA_TNUMBER ) || lua_istype ( luaVM, 5, LUA_TSTRING ) ) )
            {
                vecOrigin = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                                      static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                                      static_cast < float > ( lua_tonumber ( luaVM, 5 ) ) );

                if ( lua_istype ( luaVM, 6, LUA_TNUMBER ) || lua_istype ( luaVM, 6, LUA_TSTRING ) )
                {
                    fForce = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );

                    if ( lua_istype ( luaVM, 7, LUA_TLIGHTUSERDATA ) )
                    {
                        CClientEntity* pTemp = lua_toelement ( luaVM, 7 );
                        if ( pTemp )
                        {
                            pTarget = pTemp;
                        }
                        else
                            m_pScriptDebugging->LogBadPointer ( luaVM, "createProjectile", "element", 7 );
                    }

                    int iArgument8 = lua_type ( luaVM, 8 );
                    int iArgument9 = lua_type ( luaVM, 9 );
                    int iArgument10 = lua_type ( luaVM, 10 );
                    if ( ( iArgument8 == LUA_TSTRING || iArgument8 == LUA_TNUMBER ) &&
                         ( iArgument9 == LUA_TSTRING || iArgument9 == LUA_TNUMBER ) &&
                         ( iArgument10 == LUA_TSTRING || iArgument10 == LUA_TNUMBER ) )
                    {
                        pvecRotation = new CVector ( static_cast < float > ( lua_tonumber ( luaVM, 8 ) ),
                                                     static_cast < float > ( lua_tonumber ( luaVM, 9 ) ),
                                                     static_cast < float > ( lua_tonumber ( luaVM, 10 ) ) );
                    }
                    int iArgument11 = lua_type ( luaVM, 11 );
                    int iArgument12 = lua_type ( luaVM, 12 );
                    int iArgument13 = lua_type ( luaVM, 13 );
                    if ( ( iArgument11 == LUA_TSTRING || iArgument11 == LUA_TNUMBER ) &&
                         ( iArgument12 == LUA_TSTRING || iArgument12 == LUA_TNUMBER ) &&
                         ( iArgument13 == LUA_TSTRING || iArgument13 == LUA_TNUMBER ) )
                    {
                        pvecMoveSpeed = new CVector ( static_cast < float > ( lua_tonumber ( luaVM, 11 ) ),
                                                     static_cast < float > ( lua_tonumber ( luaVM, 12 ) ),
                                                     static_cast < float > ( lua_tonumber ( luaVM, 13 ) ) );
                        
                        int iArgument14 = lua_type ( luaVM, 14 );
                        if ( iArgument14 == LUA_TSTRING || iArgument14 == LUA_TNUMBER )
                        {
                            usModel = static_cast < unsigned short > ( lua_tonumber ( luaVM, 14 ) );
                        }
                    }
                }
            }

            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		    if ( pLuaMain )
		    {
			    CResource * pResource = pLuaMain->GetResource();
			    if ( pResource )
			    {
                    CClientProjectile * pProjectile = CStaticFunctionDefinitions::CreateProjectile ( *pResource, *pCreator, ucWeaponType, vecOrigin, fForce, pTarget, pvecRotation, pvecMoveSpeed, usModel );
                    if ( pProjectile )
                    {
                        CElementGroup * pGroup = pResource->GetElementGroup();
					    if ( pGroup )
					    {
						    pGroup->Add ( ( CClientEntity* ) pProjectile );
					    }

                        if ( pvecRotation )
                        {
                            delete pvecRotation;
                            pvecRotation = NULL;
                        }
                        if ( pvecMoveSpeed )
                        {
                            delete pvecMoveSpeed;
                            pvecMoveSpeed = NULL;
                        }

                        lua_pushelement ( luaVM, pProjectile );
                        return 1;
                    }
                }
            }
            if ( pvecRotation )
            {
                delete pvecRotation;
                pvecRotation = NULL;
            }
            if ( pvecMoveSpeed )
            {
                delete pvecMoveSpeed;
                pvecMoveSpeed = NULL;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "createProjectile", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createProjectile" );

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

		CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource();
			if ( pResource )
			{
				// Create it and return it
				CClientColCircle* pShape = CStaticFunctionDefinitions::CreateColCircle ( *pResource, vecPosition, fRadius );
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
{
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

		CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource ();
			if ( pResource )
			{
				// Create it and return it
				CClientColCuboid* pShape = CStaticFunctionDefinitions::CreateColCuboid ( *pResource, vecPosition, vecSize );
				if ( pShape )
				{
				    CElementGroup * pGroup = pResource->GetElementGroup();
	                if ( pGroup )
					{
	                    pGroup->Add ( ( CClientEntity* ) pShape );
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

		CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource ();
			if ( pResource )
			{
				// Create it and return it
				CClientColSphere* pShape = CStaticFunctionDefinitions::CreateColSphere ( *pResource, vecPosition, fRadius );
				if ( pShape )
				{
					CElementGroup * pGroup = pResource->GetElementGroup();
					if ( pGroup )
					{
						pGroup->Add ( ( CClientEntity* ) pShape );
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
        CVector vecPosition = CVector ( float ( lua_tonumber ( luaVM, 1 ) ), float ( lua_tonumber ( luaVM, 2 ) ), float ( 0 ) );
        CVector2D vecSize = CVector2D ( float ( lua_tonumber ( luaVM, 3 ) ), float ( lua_tonumber ( luaVM, 4 ) ) );
        if ( vecSize.fX < 0.0f ) vecSize.fX = 0.1f;
        if ( vecSize.fY < 0.0f ) vecSize.fY = 0.1f;

		CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource ();
			if ( pResource )
			{
				// Create it and return it
				CClientColRectangle* pShape = CStaticFunctionDefinitions::CreateColRectangle ( *pResource, vecPosition, vecSize );
				if ( pShape )
				{
					CElementGroup * pGroup = pResource->GetElementGroup();
					if ( pGroup )
					{
						pGroup->Add ( ( CClientEntity* ) pShape );
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

		CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource();
			if ( pResource )
			{
				// Create it and return it
				CClientColPolygon* pShape = CStaticFunctionDefinitions::CreateColPolygon ( *pResource, vecPosition );
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
        m_pScriptDebugging->LogBadType ( luaVM, "createColPolygon" );

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
        if ( fHeight < 0.0f ) fRadius = 0.1f;

		CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
		if ( pLuaMain )
		{
			CResource* pResource = pLuaMain->GetResource ();
			if ( pResource )
			{
				// Create it and return it
				CClientColTube* pShape = CStaticFunctionDefinitions::CreateColTube ( *pResource, vecPosition, fRadius, fHeight );
				if ( pShape )
				{
					CElementGroup * pGroup = pResource->GetElementGroup();
					if ( pGroup )
					{
						pGroup->Add ( ( CClientEntity* ) pShape );
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


int CLuaFunctionDefinitions::GetTeamFromName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szTeamName = lua_tostring ( luaVM, 1 );
        CClientTeam* pTeam = m_pTeamManager->GetTeam ( szTeamName );
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
        CClientTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
        {
            const char* szName = pTeam->GetTeamName ();
            if ( szName )
            {
                lua_pushstring ( luaVM, szName );
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
        CClientTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
        {
            unsigned char ucRed, ucGreen, ucBlue;
            pTeam->GetColor ( ucRed, ucGreen, ucBlue );

            lua_pushnumber ( luaVM, ucRed );
            lua_pushnumber ( luaVM, ucGreen );
            lua_pushnumber ( luaVM, ucBlue );
            return 3;
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
        CClientTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
        {
            bool bFriendlyFire = pTeam->GetFriendlyFire ();
            lua_pushboolean ( luaVM, bFriendlyFire );
            return 1;
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
            CClientTeam* pTeam = lua_toteam ( luaVM, 1 );
            if ( pTeam )
            {
                lua_newtable ( luaVM );

                unsigned int uiIndex = 0;

                list < CClientPlayer* > ::const_iterator iter = pTeam->IterBegin ();
                for ( ; iter != pTeam->IterEnd () ; iter++ )
                {
                    CClientPlayer* pPlayer = *iter;
                    lua_pushnumber ( luaVM, ++uiIndex );
                    lua_pushelement ( luaVM, pPlayer );
                    lua_settable ( luaVM, -3 );
                }

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
        CClientTeam* pTeam = lua_toteam ( luaVM, 1 );
        if ( pTeam )
        {
            unsigned int uiCount = pTeam->CountPlayers ();
            lua_pushnumber ( luaVM, uiCount );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "countPlayersInTeam", "team", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "countPlayersInTeam" );

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
        if ( CStaticFunctionDefinitions::GetWeaponNameFromID ( ucID, szBuffer, sizeof(szBuffer) ) )
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


int CLuaFunctionDefinitions::AddCommandHandler ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Got two strings?
        int iArgument1 = lua_type ( luaVM, 1 );
        int iArgument2 = lua_type ( luaVM, 2 );
        if ( iArgument1 == LUA_TSTRING && iArgument2 == LUA_TFUNCTION )
        {
            // Check if the command name is case sensitive
            bool bCaseSensitive = true;
            if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
                bCaseSensitive = ( ( lua_toboolean ( luaVM, 3 ) == 0 ) ? false : true );

            // Grab the strings. Valid?
            const char* szKey = lua_tostring ( luaVM, 1 );
			int iLuaFunction = luaM_toref ( luaVM, 2 );
            if ( szKey [0] != 0 && VERIFY_FUNCTION ( iLuaFunction ) )
			{
                // Add them to our list over command handlers
                if ( m_pRegisteredCommands->AddCommand ( pLuaMain, szKey, iLuaFunction, bCaseSensitive ) )
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
                // Remove it from our list
                if ( m_pRegisteredCommands->RemoveCommand ( pLuaMain, szKey ) )
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
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING  )
        {
            // Grab the string. Valid?
            const char* szKey = lua_tostring ( luaVM, 1 );
            if ( szKey [0] )
            {                
                // Grab the args
                const char* szArgs = 0;
                if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
                    szArgs = lua_tostring ( luaVM, 2 );

                // Call it
                if ( m_pRegisteredCommands->ProcessCommand ( szKey, szArgs ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
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


int CLuaFunctionDefinitions::GetPlayerUserName ( lua_State* luaVM )
{
    std::string strUser;

    g_pCore->GetCommunity ()->GetUsername ( strUser );
    if ( !strUser.empty () )
    {
        lua_pushstring ( luaVM, strUser.c_str () );
        return 1;
    }
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


int CLuaFunctionDefinitions::IsPlayerMapForced ( lua_State* luaVM )
{    
    bool bForced;
    if ( CStaticFunctionDefinitions::IsPlayerMapForced ( bForced ) )
    {
        lua_pushboolean ( luaVM, bForced );
        return 1;
    }    

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsPlayerMapVisible ( lua_State* luaVM )
{    
    bool bForced;
    if ( CStaticFunctionDefinitions::IsPlayerMapVisible ( bForced ) )
    {
        lua_pushboolean ( luaVM, bForced );
        return 1;
    }    

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::GetPlayerMapBoundingBox ( lua_State* luaVM )
{    
    // Grab the bounding box and return it
    CVector vecMin, vecMax;
    if ( CStaticFunctionDefinitions::GetPlayerMapBoundingBox ( vecMin, vecMax ) )
    {
        lua_pushnumber ( luaVM, vecMin.fX );
        lua_pushnumber ( luaVM, vecMin.fY );
        lua_pushnumber ( luaVM, vecMax.fX );
        lua_pushnumber ( luaVM, vecMax.fY );
        return 4;
    }
    //The map is invisible
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefinitions::SynthProcessMIDI ( lua_State* luaVM )
{
	CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) && pLuaMain )
    {
		// Get the VM's synth (or let it create one if it doesn't exist)
		CSFXSynth *pSynth = pLuaMain->GetSFXSynthInstance ();

        const char *szData = lua_tostring ( luaVM, 1 );
		unsigned long nSize = lua_strlen ( luaVM, 1 );	// binary data can contain zero's, so use lua_strlen
		pSynth->ProcessMIDI ( (unsigned char*)szData, nSize );

		lua_pushboolean ( luaVM, true );
		return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::PlaySound ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) )
    {
        const char* szSound = lua_tostring ( luaVM, 1 );
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            CResource* pResource = luaMain->GetResource();
            if ( pResource )
            {
		        char szFilename [ MAX_STRING_LENGTH ];
                snprintf ( szFilename, MAX_STRING_LENGTH, "%s\\%s", luaMain->GetResource()->GetResourceDirectoryPath(), szSound );
		        if ( IsValidFilePath ( lua_tostring ( luaVM, 1 ) ) )
                {
                    bool bLoop = false;
                    if ( lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
                    {
                        bLoop = ( lua_toboolean ( luaVM, 2 ) ) ? true : false;
                    }
                    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound ( pResource, szFilename, bLoop );
                    if ( pSound )
                    {
                        lua_pushelement ( luaVM, pSound );
                        return 1;
                    }
                }
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::PlaySound3D ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_istype ( luaVM, 1, LUA_TSTRING ) ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CVector vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                              static_cast < float > ( lua_tonumber ( luaVM, 4 ) ) );

        const char* szSound = lua_tostring ( luaVM, 1 );

        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            CResource* pResource = luaMain->GetResource();
            if ( pResource )
            {
		        char szFilename [ MAX_STRING_LENGTH ];
                snprintf ( szFilename, MAX_STRING_LENGTH, "%s\\%s", luaMain->GetResource()->GetResourceDirectoryPath(), szSound );
		        if ( IsValidFilePath ( lua_tostring ( luaVM, 1 ) ) )
                {
                    bool bLoop = false;
                    if ( lua_istype ( luaVM, 5, LUA_TBOOLEAN ) )
                    {
                        bLoop = ( lua_toboolean ( luaVM, 5 ) ) ? true : false;
                    }
                    CClientSound* pSound = CStaticFunctionDefinitions::PlaySound3D ( pResource, szFilename, vecPosition, bLoop );
                    if ( pSound )
                    {
                        lua_pushelement ( luaVM, pSound );
                        return 1;
                    }
                }
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::StopSound ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            if ( CStaticFunctionDefinitions::StopSound ( *pSound ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetSoundPosition ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            unsigned int uiPosition = ( unsigned int ) lua_tonumber ( luaVM, 2 );
            if ( CStaticFunctionDefinitions::SetSoundPosition ( *pSound, uiPosition ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetSoundPosition ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            unsigned int uiPosition = 0;
            if ( CStaticFunctionDefinitions::GetSoundPosition ( *pSound, uiPosition ) )
            {
                lua_pushnumber ( luaVM, uiPosition );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetSoundLength ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            unsigned int uiLength = 0;
            if ( CStaticFunctionDefinitions::GetSoundLength ( *pSound, uiLength ) )
            {
                lua_pushnumber ( luaVM, uiLength );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetSoundPaused ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            bool bPaused = ( lua_toboolean ( luaVM, 2 ) ) ? true : false;
            if ( CStaticFunctionDefinitions::SetSoundPaused ( *pSound, bPaused ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::IsSoundPaused ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            bool bPaused = false;
            if ( CStaticFunctionDefinitions::IsSoundPaused ( *pSound, bPaused ) )
            {
                lua_pushboolean ( luaVM, bPaused );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetSoundVolume ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fVolume = ( float ) lua_tonumber ( luaVM, 2 );
            if ( CStaticFunctionDefinitions::SetSoundVolume ( *pSound, fVolume ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetSoundVolume ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fVolume = 0.0f;
            if ( CStaticFunctionDefinitions::GetSoundVolume ( *pSound, fVolume ) )
            {
                lua_pushnumber ( luaVM, fVolume );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetSoundMinDistance ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fDistance = ( float ) lua_tonumber ( luaVM, 2 );
            if ( CStaticFunctionDefinitions::SetSoundMinDistance ( *pSound, fDistance ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetSoundMinDistance ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fDistance = 0.0f;
            if ( CStaticFunctionDefinitions::GetSoundMinDistance ( *pSound, fDistance ) )
            {
                lua_pushnumber ( luaVM, fDistance );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::SetSoundMaxDistance ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) &&
         lua_istype ( luaVM, 2, LUA_TNUMBER ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fDistance = ( float ) lua_tonumber ( luaVM, 2 );
            if ( CStaticFunctionDefinitions::SetSoundMaxDistance ( *pSound, fDistance ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefinitions::GetSoundMaxDistance ( lua_State* luaVM )
{
    if ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) )
    {
        CClientSound* pSound = lua_tosound ( luaVM, 1 );
        if ( pSound )
        {
            float fDistance = 0.0f;
            if ( CStaticFunctionDefinitions::GetSoundMaxDistance ( *pSound, fDistance ) )
            {
                lua_pushnumber ( luaVM, fDistance );
                return 1;
            }
        }
    }
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


#ifdef MTA_VOICE
int CLuaFunctionDefinitions::SetVoiceInputEnabled ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TBOOLEAN )
	{
		bool bEnabled = lua_toboolean ( luaVM, 1 ) ? true : false;
		bool bResult = CStaticFunctionDefinitions::SetVoiceInputEnabled(bEnabled); 
		lua_pushboolean ( luaVM, true );
		return 1;
	}
	lua_pushboolean ( luaVM, false );
	return 1;
}

int CLuaFunctionDefinitions::SetVoiceMuteAllEnabled ( lua_State* luaVM )
{
	if ( lua_type ( luaVM, 1 ) == LUA_TBOOLEAN )
	{
		bool bEnabled = lua_toboolean ( luaVM, 1 ) ? true : false;
		bool bResult = CStaticFunctionDefinitions::SetVoiceMuteAllEnabled ( bEnabled );
		lua_pushboolean ( luaVM, true );
		return 1;
	}
	lua_pushboolean ( luaVM, false );
	return 1;
}
#endif
