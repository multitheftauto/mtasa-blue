/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaMain.cpp
*  PURPOSE:     Lua virtual machine container class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Cecill Etheredge <>
*               Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static CLuaManager* m_pLuaManager;

#define HOOK_INSTRUCTION_COUNT 1000000
#define HOOK_MAXIMUM_TIME 5000

extern CGame* g_pGame;

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

CLuaMain::CLuaMain ( CLuaManager* pLuaManager,
                     CObjectManager* pObjectManager,
                     CPlayerManager* pPlayerManager,
                     CVehicleManager* pVehicleManager,
                     CBlipManager* pBlipManager,
                     CRadarAreaManager* pRadarAreaManager,
                     CMapManager* pMapManager,
                     CResource* pResourceOwner  )
{
    // Initialise everything to be setup in the Start function
    m_pLuaManager = pLuaManager;
    m_luaVM = NULL;
    m_pResource = pResourceOwner;
    m_pResourceFile = NULL;
    m_bBeingDeleted = false;
    m_pLuaTimerManager = new CLuaTimerManager;

    // Set up the name of our script
    m_ulFunctionEnterTime = 0;
    m_iOwner = OWNER_SERVER;
    m_szScriptName [0] = 0;
    m_szScriptName [MAX_SCRIPTNAME_LENGTH] = 0;

    m_pObjectManager = pObjectManager;
    m_pPlayerManager = pPlayerManager;
    m_pRadarAreaManager = pRadarAreaManager;
    m_pVehicleManager = pVehicleManager;
    m_pBlipManager = pBlipManager;
    m_pMapManager = pMapManager;

    // Add all our events and functions
    InitVM();

	// Initialize security restrictions. Very important to prevent lua trojans and viruses!
	InitSecurity();
}


CLuaMain::~CLuaMain ( void )
{
    // remove all current remote calls originating from this VM
    g_pGame->GetRemoteCalls()->Remove ( this );

    // Unload the current script
    UnloadScript ();
    m_bBeingDeleted = true;

    // Delete the timer manager
    delete m_pLuaTimerManager;

    // Eventually delete the XML files the LUA script didn't
    list<CXMLFile *>::iterator iterXML = m_XMLFiles.begin ();
    for ( ; iterXML != m_XMLFiles.end (); iterXML++ )
    {
        delete *iterXML;
    }

    // Eventually delete the text displays the LUA script didn't
    list<CTextDisplay *>::iterator iterDisplays = m_Displays.begin ();
    for ( ; iterDisplays != m_Displays.end (); iterDisplays++ )
    {
        delete *iterDisplays;
    }

    // Eventually delete the text items the LUA script didn't
    list<CTextItem *>::iterator iterItems = m_TextItems.begin ();
    for ( ; iterItems != m_TextItems.end (); iterItems++ )
    {
        delete *iterItems;
    }
}

bool CLuaMain::BeingDeleted ( void )
{
    return m_bBeingDeleted;
}


void CLuaMain::ResetInstructionCount ( void )
{
    m_ulFunctionEnterTime = GetTime ();
}


void CLuaMain::InitSecurity ( void )
{
	// Disable unsafe native lua functions
	lua_register ( m_luaVM, "dofile", CLuaFunctionDefinitions::DisabledFunction );
	lua_register ( m_luaVM, "loadfile", CLuaFunctionDefinitions::DisabledFunction );
	lua_register ( m_luaVM, "require", CLuaFunctionDefinitions::DisabledFunction );
	lua_register ( m_luaVM, "loadlib", CLuaFunctionDefinitions::DisabledFunction );
}


void CLuaMain::InitVM ( void )
{
    // Create a new VM
    m_luaVM = lua_open ();

    // Set the instruction count hook
    lua_sethook ( m_luaVM, InstructionCountHook, LUA_MASKCOUNT, HOOK_INSTRUCTION_COUNT );

    // Load LUA libraries
    luaopen_base ( m_luaVM );
    luaopen_table ( m_luaVM );
    luaopen_string ( m_luaVM );
    luaopen_math ( m_luaVM );
    luaopen_debug ( m_luaVM );

	// Create the callback table (should be at location 1 in the registry)
	lua_newtable ( m_luaVM );
	lua_ref ( m_luaVM, 1 );
	lua_settable ( m_luaVM, -1 );

    // Registering C functions
    CLuaCFunctions::RegisterFunctionsWithVM ( m_luaVM );

    // Oli: Don't forget to add new ones to CLuaManager::LoadCFunctions. Thanks!

	// create global vars
	lua_pushelement ( m_luaVM, g_pGame->GetMapManager()->GetRootElement() );
	lua_setglobal ( m_luaVM, "root" );

	lua_pushresource ( m_luaVM, m_pResource );
	lua_setglobal ( m_luaVM, "resource" );

	lua_pushelement ( m_luaVM, m_pResource->GetResourceRootElement () );
	lua_setglobal ( m_luaVM, "resourceRoot" );

    // Load pre-loaded lua code
    LoadScript ( szPreloadedScript );
}

// Special function(s) that are only visible to HTMLD scripts
void CLuaMain::RegisterHTMLDFunctions ( void )
{
    CLuaHTTPDefs::LoadFunctions ( m_luaVM );
}

void CLuaMain::InstructionCountHook ( lua_State* luaVM, lua_Debug* pDebug )
{
    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Above max time?
        if ( GetTime () >= pLuaMain->m_ulFunctionEnterTime + HOOK_MAXIMUM_TIME )
        {
            // Print it in the console
            CLogger::ErrorPrintf ( "ERROR: Infinite/too long execution (%s)\n", pLuaMain->GetScriptNamePointer () );

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
                g_pGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Loading script failed: %s", szRes );
            }
            else
            {
                CLogger::LogPrint ( "SCRIPT ERROR: Unknown\n" );
                g_pGame->GetScriptDebugging()->LogInformation ( m_luaVM, "Loading script failed for unknown reason" );
            }
        }
        else
        {
            ResetInstructionCount ();
            int iret = lua_pcall ( m_luaVM, 0, 0, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                szRes = lua_tostring(m_luaVM, -1);
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "%s", szRes );
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
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Executing in-line script failed: %s", szRes );
            }
        }
        else
        {
            const char* szRes = lua_tostring(m_luaVM, -1);
            g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading in-line script failed: %s", szRes );
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
    // End the lua vm
    if ( m_luaVM )
    {
        lua_close( m_luaVM );
        m_luaVM = NULL;
    }

    // Delete all timers and events
    m_pLuaTimerManager->RemoveAllTimers ();

	// Delete all keybinds
    list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
    {
        if ( (*iter)->IsJoined () )
			(*iter)->GetKeyBinds ()->RemoveAllKeys ( this );
	}
}


void CLuaMain::DoPulse ( void )
{
    m_pLuaTimerManager->DoPulse ( this );
}


void CLuaMain::AddXML ( CXMLFile* pFile )
{
    if ( pFile )
    {
        m_XMLFiles.push_back ( pFile );
    }
}


CXMLFile * CLuaMain::CreateXML ( const char * szFilename )
{
    CXMLFile * pFile = g_pServerInterface->GetXML ()->CreateXML ( szFilename );
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
                m_XMLFiles.erase ( iter );
                delete file;
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
        list < CResourceFile* > ::iterator iter = m_pResource->IterBegin ();
        for ( ; iter != m_pResource->IterEnd () ; iter++ )
        {
            CResourceFile* pResourceFile = *iter;
            if ( pResourceFile->GetType () == CResourceFile::RESOURCE_FILE_TYPE_CONFIG )
            {
                CResourceConfigItem* pConfigItem = static_cast < CResourceConfigItem* > ( pResourceFile );
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
}


CTextDisplay * CLuaMain::CreateDisplay ( )
{
    CTextDisplay * pDisplay = new CTextDisplay;
    if ( pDisplay )
        m_Displays.push_back ( pDisplay );
    return pDisplay;
}


void CLuaMain::DestroyDisplay ( CTextDisplay * pDisplay )
{
    if ( !m_Displays.empty() ) m_Displays.remove ( pDisplay );
    delete pDisplay;
}


CTextItem * CLuaMain::CreateTextItem ( const char* szText, float fX, float fY, eTextPriority priority, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, float fScale, unsigned char format )
{
    CTextItem * pTextItem = new CTextItem( szText, CVector2D ( fX, fY ), priority, red, green, blue, alpha, fScale, format );
    m_TextItems.push_back ( pTextItem );
    return pTextItem;
}


void CLuaMain::DestroyTextItem ( CTextItem * pTextItem )
{
    if ( !m_TextItems.empty() ) m_TextItems.remove ( pTextItem );
    delete pTextItem;
}


bool CLuaMain::TextDisplayExists ( CTextDisplay* pDisplay )
{
    list < CTextDisplay* > ::const_iterator iter = m_Displays.begin ();
    for ( ; iter != m_Displays.end (); iter++ )
    {
        if ( *iter == pDisplay )
        {
            return true;
        }
    }

    return false;
}


bool CLuaMain::TextItemExists ( CTextItem* pTextItem )
{
    list < CTextItem* > ::const_iterator iter = m_TextItems.begin ();
    for ( ; iter != m_TextItems.end (); iter++ )
    {
        if ( *iter == pTextItem )
        {
            return true;
        }
    }

    return false;
}
