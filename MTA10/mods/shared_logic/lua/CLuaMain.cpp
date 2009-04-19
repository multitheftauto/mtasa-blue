/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaMain.cpp
*  PURPOSE:     Lua main
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Jax <>
*               Ed Lyons <eai@opencoding.net>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

extern CClientGame* g_pClientGame;

static CLuaManager* m_pLuaManager;

#define HOOK_INSTRUCTION_COUNT 1000000
#define HOOK_MAXIMUM_TIME 5000


// This script is loaded into all VM's created.
const char szPreloadedScript [] = ""\

    // Code for allowing this syntax:  call.resource:exportedFunction (...)
    // Aswell as the old:              call ( getResourceFromName ( "resource" ), ... )
    "local rescallMT = {}\n" \
    "function rescallMT:__index(k)\n" \
    "        self[k] = function(res, ...) return call(self.res, k, ...) end\n" \
    "        return self[k]\n" \
    "end\n" \
    "local exportsMT = {}\n" \
    "function exportsMT:__index(k)\n" \
    "        local res = getResourceFromName(k)\n" \
    "        if res then\n" \
    "                self[k] = setmetatable({ res = res }, rescallMT)\n" \
    "                return self[k]\n" \
    "        else\n" \
    "                outputDebugString(\"exports: Call to non-existing resource (\" .. k .. \")\", 1)\n" \
    "        end\n" \
    "end\n" \
    "exports = setmetatable({}, exportsMT)\n";


CLuaMain::CLuaMain ( CLuaManager* pLuaManager, CResource* pResourceOwner )
{
    // Initialise everything to be setup in the Start function
    m_pLuaManager = pLuaManager;
    m_luaVM = NULL;
    m_bBeingDeleted = false;
    m_pLuaTimerManager = new CLuaTimerManager;

    // Set up the name of our script
    m_ulFunctionEnterTime = 0;
    m_iOwner = OWNER_SERVER;
    m_szScriptName [0] = 0;
    m_szScriptName [MAX_SCRIPTNAME_LENGTH] = 0;
    
    m_pResource = pResourceOwner;

    // Add all our events and functions
    InitVM();

	// Initialize security restrictions. Very important to prevent lua trojans and viruses!
	InitSecurity();
}


CLuaMain::~CLuaMain ( void )
{
    // Unload the current script
    UnloadScript ();
    m_bBeingDeleted = true;

    // Delete the timer manager
    delete m_pLuaTimerManager;
}

bool CLuaMain::BeingDeleted ( void )
{
    return m_bBeingDeleted;
}


void CLuaMain::ResetInstructionCount ( void )
{
    m_ulFunctionEnterTime = timeGetTime ();
}


void CLuaMain::InitSecurity ( void )
{
	lua_register ( m_luaVM, "dofile", CLuaFunctionDefinitions::DisabledFunction );
	lua_register ( m_luaVM, "loadfile", CLuaFunctionDefinitions::DisabledFunction );
	lua_register ( m_luaVM, "require", CLuaFunctionDefinitions::DisabledFunction );
	lua_register ( m_luaVM, "loadlib", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "getfenv", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "newproxy", CLuaFunctionDefinitions::DisabledFunction );
}


CSFXSynth* CLuaMain::GetSFXSynthInstance ( void )
{
	// Create a synth if it doesn't exist yet
	if ( !m_pSFXSynth ) {
		m_pSFXSynth = g_pClientGame->GetSFX ()->CreateSynth ();
	}
	return m_pSFXSynth;
}

void CLuaMain::InitVM ( void )
{
	// Null VM-dependant classes
	m_pSFXSynth = NULL;

    // Create a new VM
    m_luaVM = lua_open ();

    // Set the instruction count hook
    lua_sethook ( m_luaVM, InstructionCountHook, LUA_MASKCOUNT, HOOK_INSTRUCTION_COUNT );

    // Load LUA libraries
    luaopen_base ( m_luaVM );
    luaopen_math ( m_luaVM );
    luaopen_string ( m_luaVM );
    luaopen_table ( m_luaVM );
    luaopen_debug ( m_luaVM );

	// Create the callback table (at location 1 in the registry)
    lua_pushnumber ( m_luaVM, 1 );
    lua_newtable ( m_luaVM );
    lua_settable ( m_luaVM, LUA_REGISTRYINDEX );

	// Register module functions
    CLuaCFunctions::RegisterFunctionsWithVM ( m_luaVM );

    // Update global variables
    lua_pushelement ( m_luaVM, g_pClientGame->GetRootEntity () );
	lua_setglobal ( m_luaVM, "root" );

    lua_pushresource ( m_luaVM, m_pResource );
	lua_setglobal ( m_luaVM, "resource" );

	lua_pushelement ( m_luaVM, m_pResource->GetResourceEntity () );
	lua_setglobal ( m_luaVM, "resourceRoot" );

    lua_pushelement ( m_luaVM, m_pResource->GetResourceGUIEntity () );
    lua_setglobal ( m_luaVM, "guiRoot" );

    // Load pre-loaded lua code
    LoadScript ( szPreloadedScript );
}


void CLuaMain::InstructionCountHook ( lua_State* luaVM, lua_Debug* pDebug )
{
    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Above max time?
        if ( timeGetTime () >= pLuaMain->m_ulFunctionEnterTime + HOOK_MAXIMUM_TIME )
        {
            // Print it in the console
            CLogger::ErrorPrintf ( "ERROR: Infinite/too long execution (%s)", pLuaMain->GetScriptNamePointer () );

            // Error out
            lua_pushstring ( luaVM, "Aborting; infinite running script" );
            lua_error ( luaVM );
        }
    }
}


bool CLuaMain::LoadScriptFromFile ( const char* szLUAScript )
{
    const char* szRes = NULL;
    if ( m_luaVM )
    {
        // Load the script
        if ( luaL_loadfile ( m_luaVM, szLUAScript ) )
        {
            // Print the error
            szRes = lua_tostring(m_luaVM, -1);
            if ( szRes )
            {
                CLogger::LogPrintf ( "SCRIPT ERROR: %s\n", szRes );
                g_pClientGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Loading script failed: %s", szRes );
            }
            else
            {
                CLogger::LogPrint ( "SCRIPT ERROR: Unknown\n" );
                g_pClientGame->GetScriptDebugging()->LogInformation ( m_luaVM, "Loading script failed for unknown reason" );
            }
        }
        else
        {
            ResetInstructionCount ();
            int iret = lua_pcall ( m_luaVM, 0, 0, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                szRes = lua_tostring(m_luaVM, -1);
                g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "%s", szRes );
            }
            return true;
        }
    }

    return false;
}

bool CLuaMain::LoadScript ( const char* szLUAScript )
{
    if ( m_luaVM )
    {
        // Run the script
        if ( !luaL_loadbuffer ( m_luaVM, szLUAScript, strlen(szLUAScript), NULL ) )
        {
            ResetInstructionCount ();
            int iret = lua_pcall ( m_luaVM, 0, 0, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                const char* szRes = lua_tostring(m_luaVM, -1);
                g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "Executing in-line script failed: %s", szRes );
            }
        }
        else
        {
            const char* szRes = lua_tostring(m_luaVM, -1);
            g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading in-line script failed: %s", szRes );
        }
    }
    else
        return false;

    return true;
}

void CLuaMain::Start ( void )
{

}


void CLuaMain::UnloadScript ( void )
{
	// ACHTUNG: UNLOAD MODULES!

	// Delete our sound classes
	if ( m_pSFXSynth )
		g_pClientGame->GetSFX ()->DeleteSynth ( m_pSFXSynth );

    // End the lua vm
    if ( m_luaVM )
    {
        lua_close( m_luaVM );
        m_luaVM = NULL;
    }

    // Delete all timers and events
    m_pLuaTimerManager->RemoveAllTimers ();

	// Delete all GUI elements
	//m_pLuaManager->m_pGUIManager->DeleteAll ( this );
}


void CLuaMain::DoPulse ( void )
{
    m_pLuaTimerManager->DoPulse ( this );
}


CXMLFile * CLuaMain::CreateXML ( const char * szFilename )
{
    CXMLFile * pFile = g_pCore->GetXML ()->CreateXML ( szFilename );
    if ( pFile )
        m_XMLFiles.push_back ( pFile );
    return pFile;
}

void CLuaMain::DestroyXML ( CXMLFile * pFile )
{
    if ( !m_XMLFiles.empty() ) m_XMLFiles.remove ( pFile );
    delete pFile;
}

void CLuaMain::DestroyXML ( CXMLNode * pRootNode )
{
    list<CXMLFile *>::iterator iter;
    for ( iter = m_XMLFiles.begin(); iter != m_XMLFiles.end(); iter++ )
    {
        CXMLFile * file = (*iter);
        if ( file )
        {
            if ( file->GetRootNode() == pRootNode )
            {
                delete file;
                m_XMLFiles.erase ( iter );
                break;
            }
        }
    }
}

void CLuaMain::SaveXML ( CXMLNode * pRootNode )
{
    list<CXMLFile *>::iterator iter;
    for ( iter = m_XMLFiles.begin(); iter != m_XMLFiles.end(); iter++ )
    {
        CXMLFile * file = (*iter);
        if ( file )
        {
            if ( file->GetRootNode() == pRootNode )
            {
                file->Write();
                break;
            }
        }
    }
    if ( m_pResource )
    {
        list < CResourceConfigItem* > ::iterator iter = m_pResource->ConfigIterBegin ();
        for ( ; iter != m_pResource->ConfigIterEnd () ; iter++ )
        {
            CResourceConfigItem* pConfigItem = *iter;
            if ( pConfigItem->GetRoot () == pRootNode )
            {
                CXMLFile* pFile = pConfigItem->GetFile ();
                if ( pFile )
                {
                    pFile->Write ();
                }
                break;
            }
        }
    }
}
