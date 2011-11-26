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

#include <clocale>

static CLuaManager* m_pLuaManager;

#define HOOK_INSTRUCTION_COUNT 1000000
#define HOOK_MAXIMUM_TIME 5000

extern CGame* g_pGame;

// This script is loaded into all VM's created.
const char szPreloadedScript [] = ""\

    // Code for allowing this syntax:   exports.resourceName:exportedFunctionName (...)
    //                                  exports["resourceName"]:exportedFunctionName (...)
    //                                  exports[resourcePointer]:exportedFunctionName (...)
    // Aswell as the old:               call ( getResourceFromName ( "resourceName" ), "exportedFunctionName", ... )
    "local rescallMT = {}\n" \
    "function rescallMT:__index(k)\n" \
    "        if type(k) ~= 'string' then k = tostring(k) end\n" \
    "        self[k] = function(resExportTable, ...)\n" \
    "                if type(self.res) == 'userdata' and getResourceRootElement(self.res) then\n" \
    "                        return call(self.res, k, ...)\n" \
    "                else\n" \
    "                        return nil\n" \
    "                end\n" \
    "        end\n" \
    "        return self[k]\n" \
    "end\n" \
    "local exportsMT = {}\n" \
    "function exportsMT:__index(k)\n" \
    "        if type(k) == 'userdata' and getResourceRootElement(k) then\n" \
    "                return setmetatable({ res = k }, rescallMT)\n" \
    "        elseif type(k) ~= 'string' then\n" \
    "                k = tostring(k)\n" \
    "        end\n" \
    "        local res = getResourceFromName(k)\n" \
    "        if res and getResourceRootElement(res) then\n" \
    "                return setmetatable({ res = res }, rescallMT)\n" \
    "        else\n" \
    "                outputDebugString('exports: Call to non-running server resource (' .. k .. ')', 1)\n" \
    "                return setmetatable({}, rescallMT)\n" \
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

    CPerfStatLuaMemory::GetSingleton ()->OnLuaMainCreate ( this );
    CPerfStatLuaTiming::GetSingleton ()->OnLuaMainCreate ( this );
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

    CPerfStatLuaMemory::GetSingleton ()->OnLuaMainDestroy ( this );
    CPerfStatLuaTiming::GetSingleton ()->OnLuaMainDestroy ( this );
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
    lua_register ( m_luaVM, "dofile", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "loadfile", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "require", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "loadlib", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "getfenv", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "newproxy", CLuaFunctionDefinitions::DisabledFunction );
}


void CLuaMain::InitVM ( void )
{
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

    // Initialize security restrictions. Very important to prevent lua trojans and viruses!
    InitSecurity ();

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
            CLogger::ErrorPrintf ( "Infinite/too long execution (%s)\n", pLuaMain->GetScriptNamePointer () );
            
            SString strAbortInf = "Aborting; infinite running script in ";
            strAbortInf += pLuaMain->GetScriptNamePointer ();
            
            // Error out
            lua_pushstring ( luaVM, strAbortInf );
            lua_error ( luaVM );
        }
    }
}


bool CLuaMain::LoadScriptFromFile ( const char* szLUAScript )
{
    if ( m_luaVM )
    {
        // Load the script
        if ( luaL_loadfile ( m_luaVM, szLUAScript ) )
        {
            // Print the error
            std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
            if ( strRes.length () )
            {
                CLogger::LogPrintf ( "SCRIPT ERROR: %s\n", strRes.c_str () );
                g_pGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Loading script failed: %s", strRes.c_str () );
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
                SString strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
        
                vector <SString> vecSplit;
                strRes.Split ( ":", vecSplit );
                
                if ( vecSplit.size ( ) >= 3 )
                {
                    SString strFile = vecSplit[0];
                    int     iLine   = atoi ( vecSplit[1].c_str ( ) );
                    SString strMsg  = vecSplit[2].substr ( 1 );
                    
                    g_pGame->GetScriptDebugging()->LogError ( strFile, iLine, strMsg );
                }
                else
                    g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "%s", strRes.c_str () );
            }
            return true;
        }
    }

    return false;
}

bool CLuaMain::LoadScriptFromBuffer ( const char* cpBuffer, unsigned int uiSize, const char* szFileName, bool bUTF8 )
{
    if ( m_luaVM )
    {
        // Are we not marked as UTF-8 already, and not precompiled?
        std::string strUTFScript;
        if ( !bUTF8 && ( uiSize < 5 || cpBuffer[0] != 27 || cpBuffer[1] != 'L' || cpBuffer[2] != 'u' || cpBuffer[3] != 'a' || cpBuffer[4] != 'Q' ) )
        {
            std::string strBuffer = std::string(cpBuffer, uiSize);
#ifdef WIN32
            std::setlocale(LC_CTYPE,""); // Temporarilly use locales to read the script
            strUTFScript = UTF16ToMbUTF8(ANSIToUTF16( strBuffer ));
            std::setlocale(LC_CTYPE,"C");
#else
            strUTFScript = UTF16ToMbUTF8(ANSIToUTF16( strBuffer ));
#endif

            if ( uiSize != strUTFScript.size() )
            {
                uiSize = strUTFScript.size();
                g_pGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Script '%s' is not encoded in UTF-8.  Loading as ANSI...", ConformResourcePath(szFileName).c_str() );
            }
        }
        else
            strUTFScript = std::string(cpBuffer, uiSize);

        // Run the script
        if ( luaL_loadbuffer ( m_luaVM, bUTF8 ? cpBuffer : strUTFScript.c_str(), uiSize, SString ( "@%s", szFileName ) ) )
        {
            // Print the error
            std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
            if ( strRes.length () )
            {
                CLogger::LogPrintf ( "SCRIPT ERROR: %s\n", strRes.c_str () );
                g_pGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Loading script failed: %s", strRes.c_str () );
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
                SString strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
        
                std::vector <SString> vecSplit;
                strRes.Split ( ":", vecSplit );
                
                if ( vecSplit.size ( ) >= 3 )
                {
                    SString strFile = vecSplit[0];
                    int     iLine   = atoi ( vecSplit[1].c_str ( ) );
                    SString strMsg  = vecSplit[2].substr ( 1 );
                    
                    g_pGame->GetScriptDebugging()->LogError ( strFile, iLine, strMsg );
                }
                else
                    g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "%s", strRes.c_str () );
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
                std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Executing in-line script failed: %s", strRes.c_str () );
            }
        }
        else
        {
            std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
            g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading in-line script failed: %s", strRes.c_str () );
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
    // Delete all timers and events
    m_pLuaTimerManager->RemoveAllTimers ();

    // Delete all keybinds
    list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
    {
        if ( (*iter)->IsJoined () )
            (*iter)->GetKeyBinds ()->RemoveAllKeys ( this );
    }

    // End the lua vm
    if ( m_luaVM )
    {
        lua_close( m_luaVM );
        m_luaVM = NULL;
    }
}


void CLuaMain::DoPulse ( void )
{
    m_pLuaTimerManager->DoPulse ( this );
}


CXMLFile * CLuaMain::CreateXML ( const char * szFilename )
{
    CXMLFile * pFile = g_pServerInterface->GetXML ()->CreateXML ( szFilename, true );
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


CTextItem * CLuaMain::CreateTextItem ( const char* szText, float fX, float fY, eTextPriority priority, const SColor color, float fScale, unsigned char format, unsigned char ucShadowAlpha )
{
    CTextItem * pTextItem = new CTextItem( szText, CVector2D ( fX, fY ), priority, color, fScale, format, ucShadowAlpha );
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


///////////////////////////////////////////////////////////////
//
// CLuaMain::GetFunctionTag
//
// Turn iFunctionNumber into something human readable
//
///////////////////////////////////////////////////////////////
const SString& CLuaMain::GetFunctionTag ( int iLuaFunction )
{
    // Find existing
    SString* pTag = MapFind ( m_FunctionTagMap, iLuaFunction );
#ifndef CHECK_FUNCTION_TAG
    if ( !pTag )
#endif
    {
        // Create if required
        SString strText;

        lua_Debug debugInfo;
        lua_getref ( m_luaVM, iLuaFunction );
        if ( lua_getinfo( m_luaVM, ">nlS", &debugInfo ) )
        {
            // Make sure this function isn't defined in a string
            if ( debugInfo.source[0] == '@' )
            {
                //std::string strFilename2 = ConformResourcePath ( debugInfo.source );
                SString strFilename = debugInfo.source;

                int iPos = strFilename.find_last_of ( "/\\" );
                if ( iPos >= 0 )
                    strFilename = strFilename.substr ( iPos + 1 );

                strText = SString ( "@%s:%d", strFilename.c_str (), debugInfo.currentline != -1 ? debugInfo.currentline : debugInfo.linedefined, iLuaFunction );
            }
            else
            {
                strText = SString ( "@func_%d %s", iLuaFunction, debugInfo.short_src );
            }
        }
        else
        {
            strText = SString ( "@func_%d NULL", iLuaFunction );
        }

    #ifdef CHECK_FUNCTION_TAG
        if ( pTag )
        {
            // Check tag remains unchanged
            assert ( strText == *pTag );
            return *pTag;
        }
    #endif

        MapSet ( m_FunctionTagMap, iLuaFunction, strText );
        pTag = MapFind ( m_FunctionTagMap, iLuaFunction );
    }
    return *pTag;
}
