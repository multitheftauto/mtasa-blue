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

#include "CLuaFunctionDefs.h"
#include <clocale>

static CLuaManager* m_pLuaManager;
SString CLuaMain::ms_strExpectedUndumpHash;

#define HOOK_INSTRUCTION_COUNT 1000000
#define HOOK_MAXIMUM_TIME 5000

extern CGame* g_pGame;
extern CNetServer* g_pRealNetServer;

#include "luascripts/coroutine_debug.lua.h"
#include "luascripts/exports.lua.h"
#include "luascripts/inspect.lua.h"

CLuaMain::CLuaMain ( CLuaManager* pLuaManager,
                     CObjectManager* pObjectManager,
                     CPlayerManager* pPlayerManager,
                     CVehicleManager* pVehicleManager,
                     CBlipManager* pBlipManager,
                     CRadarAreaManager* pRadarAreaManager,
                     CMapManager* pMapManager,
                     CResource* pResourceOwner, bool bEnableOOP  )
{
    // Initialise everything to be setup in the Start function
    m_pLuaManager = pLuaManager;
    m_luaVM = NULL;
    m_pResource = pResourceOwner;
    m_pResourceFile = NULL;
    m_bBeingDeleted = false;
    m_pLuaTimerManager = new CLuaTimerManager;
    m_FunctionEnterTimer.SetMaxIncrement ( 500 );
    m_WarningTimer.SetMaxIncrement ( 1000 );
    m_uiOpenFileCountWarnThresh = 10;
    m_uiOpenXMLFileCountWarnThresh = 20;

    m_pObjectManager = pObjectManager;
    m_pPlayerManager = pPlayerManager;
    m_pRadarAreaManager = pRadarAreaManager;
    m_pVehicleManager = pVehicleManager;
    m_pBlipManager = pBlipManager;
    m_pMapManager = pMapManager;

    m_bEnableOOP = bEnableOOP;

    m_iPackageLoadedRef = -1;

    CPerfStatLuaMemory::GetSingleton ()->OnLuaMainCreate ( this );
    CPerfStatLuaTiming::GetSingleton ()->OnLuaMainCreate ( this );
}


CLuaMain::~CLuaMain ( void )
{
    // remove all current remote calls originating from this VM
    g_pGame->GetRemoteCalls()->Remove ( this );
    g_pGame->GetLuaCallbackManager ()->OnLuaMainDestroy ( this );
    g_pGame->GetLatentTransferManager ()->OnLuaMainDestroy ( this );
    g_pGame->GetDebugHookManager()->OnLuaMainDestroy ( this );
    g_pGame->GetScriptDebugging()->OnLuaMainDestroy ( this );

    // Unload the current script
    UnloadScript ();
    m_bBeingDeleted = true;

    // Delete the timer manager
    delete m_pLuaTimerManager;

    // Eventually delete the XML files the LUA script didn't
    list<CXMLFile *>::iterator iterXML = m_XMLFiles.begin ();
    for ( ; iterXML != m_XMLFiles.end (); ++iterXML )
    {
        delete *iterXML;
    }

    // Eventually delete the text displays the LUA script didn't
    list<CTextDisplay *>::iterator iterDisplays = m_Displays.begin ();
    for ( ; iterDisplays != m_Displays.end (); ++iterDisplays )
    {
        delete *iterDisplays;
    }

    // Eventually delete the text items the LUA script didn't
    list<CTextItem *>::iterator iterItems = m_TextItems.begin ();
    for ( ; iterItems != m_TextItems.end (); ++iterItems )
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
    m_FunctionEnterTimer.Reset ();
}


void CLuaMain::InitSecurity ( void )
{
    lua_register ( m_luaVM, "dofile", CLuaUtilDefs::DisabledFunction );
    lua_register ( m_luaVM, "loadfile", CLuaUtilDefs::DisabledFunction );
    lua_register ( m_luaVM, "loadlib", CLuaUtilDefs::DisabledFunction );
    lua_register ( m_luaVM, "getfenv", CLuaUtilDefs::DisabledFunction );
    lua_register ( m_luaVM, "newproxy", CLuaUtilDefs::DisabledFunction );
}


void CLuaMain::InitClasses ( lua_State* luaVM )
{
    lua_initclasses             ( luaVM );
    lua_newclass                ( luaVM );
    
    // Vector and Matrix classes
    CLuaVector4Defs     ::AddClass   ( luaVM );
    CLuaVector3Defs     ::AddClass   ( luaVM );
    CLuaVector2Defs     ::AddClass   ( luaVM );
    CLuaMatrixDefs      ::AddClass   ( luaVM );

    // OOP based classes
    if ( !m_bEnableOOP )
        return;
    
    CLuaElementDefs   ::AddClass ( luaVM ); // keep this at the top because inheritance
    CLuaAccountDefs   ::AddClass ( luaVM );
    CLuaACLDefs       ::AddClass ( luaVM );
    CLuaBanDefs       ::AddClass ( luaVM );
    CLuaBlipDefs      ::AddClass ( luaVM );
    CLuaColShapeDefs  ::AddClass ( luaVM );
    CLuaDatabaseDefs  ::AddClass ( luaVM );
    CLuaFileDefs      ::AddClass ( luaVM );
    CLuaMarkerDefs    ::AddClass ( luaVM );
    CLuaObjectDefs    ::AddClass ( luaVM );
    CLuaPedDefs       ::AddClass ( luaVM );
    CLuaPickupDefs    ::AddClass ( luaVM );
    CLuaPlayerDefs    ::AddClass ( luaVM );
    CLuaRadarAreaDefs ::AddClass ( luaVM );
    CLuaResourceDefs  ::AddClass ( luaVM );
    CLuaTeamDefs      ::AddClass ( luaVM );
    CLuaTextDefs      ::AddClass ( luaVM );
    CLuaTimerDefs     ::AddClass ( luaVM );
    CLuaVehicleDefs   ::AddClass ( luaVM );
    CLuaWaterDefs     ::AddClass ( luaVM );

    CLuaShared::AddClasses ( luaVM );
}


void CLuaMain::InitVM ( void )
{
    assert( !m_luaVM );

    // Create a new VM
    m_luaVM = lua_open ();
    m_pLuaManager->OnLuaMainOpenVM( this, m_luaVM );

    // Set the instruction count hook
    lua_sethook ( m_luaVM, InstructionCountHook, LUA_MASKCOUNT, HOOK_INSTRUCTION_COUNT );

    // Load LUA libraries
    luaopen_base ( m_luaVM );
    luaopen_math ( m_luaVM );
    luaopen_string ( m_luaVM );
    luaopen_table ( m_luaVM );
    luaopen_debug ( m_luaVM );
    luaopen_utf8 ( m_luaVM );

    // Initialize packages system
    InitPackageStorage(m_luaVM);

    // Initialize security restrictions. Very important to prevent lua trojans and viruses!
    InitSecurity ();

    // Registering C functions
    CLuaCFunctions::RegisterFunctionsWithVM ( m_luaVM );

    // Create class metatables
    InitClasses ( m_luaVM );

    // Oli: Don't forget to add new ones to CLuaManager::LoadCFunctions. Thanks!

    // create global vars
    lua_pushelement ( m_luaVM, g_pGame->GetMapManager()->GetRootElement() );
    lua_setglobal ( m_luaVM, "root" );

    lua_pushresource ( m_luaVM, m_pResource );
    lua_setglobal ( m_luaVM, "resource" );

    lua_pushelement ( m_luaVM, m_pResource->GetResourceRootElement () );
    lua_setglobal ( m_luaVM, "resourceRoot" );

    // Load pre-loaded lua scripts
    LoadScript(EmbeddedLuaCode::exports);
    LoadScript(EmbeddedLuaCode::coroutine_debug);
    LoadScript(EmbeddedLuaCode::inspect);
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
        if ( pLuaMain->m_FunctionEnterTimer.Get () > HOOK_MAXIMUM_TIME )
        {
            // Print it in the console
            CLogger::ErrorPrintf ( "Infinite/too long execution (%s)\n", pLuaMain->GetScriptName () );
            
            SString strAbortInf = "Aborting; infinite running script in ";
            strAbortInf += pLuaMain->GetScriptName ();
            
            // Error out
            lua_pushstring ( luaVM, strAbortInf );
            lua_error ( luaVM );
        }
    }
}


bool CLuaMain::LoadScriptFromBuffer(const char* cpInBuffer, unsigned int uiInSize, const char* szFileName, bool bClearReturnValues )
{
    SString strNiceFilename = ConformResourcePath( szFileName );

    // Deobfuscate if required
    const char* cpBuffer;
    uint uiSize;
    if ( !g_pRealNetServer->DeobfuscateScript( cpInBuffer, uiInSize, &cpBuffer, &uiSize, strNiceFilename ) )
    {
        SString strMessage( "%s is invalid. Please re-compile at http://luac.mtasa.com/", *strNiceFilename ); 
        g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", *strMessage );
        return false;
    }

    bool bUTF8 = CLuaShared::CheckUTF8BOMAndUpdate ( &cpBuffer, &uiSize );

    // If compiled script, make sure correct chunkname is embedded
    CLuaShared::EmbedChunkName( strNiceFilename, &cpBuffer, &uiSize );

    if ( m_luaVM )
    {
        // Are we not marked as UTF-8 already, and not precompiled?
        std::string strUTFScript;
        if ( !bUTF8 && !IsLuaCompiledScript( cpBuffer, uiSize ) )
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
                g_pGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Script '%s' is not encoded in UTF-8.  Loading as ANSI...", strNiceFilename.c_str() );
            }
        }
        else
            strUTFScript = std::string(cpBuffer, uiSize);

        // Run the script
        if ( CLuaMain::LuaLoadBuffer ( m_luaVM, bUTF8 ? cpBuffer : strUTFScript.c_str(), uiSize, SString ( "@%s", *strNiceFilename ) ) )
        {
            // Print the error
            std::string strRes = lua_tostring( m_luaVM, -1 );
            if ( strRes.length () )
            {
                CLogger::LogPrintf ( "SCRIPT ERROR: %s\n", strRes.c_str () );
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", strRes.c_str () );
            }
            else
            {
                CLogger::LogPrint ( "SCRIPT ERROR: Unknown\n" );
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed for unknown reason" );
            }
        }
        else
        {
            ResetInstructionCount ();
            int luaSavedTop = lua_gettop ( m_luaVM );
            int iret = this->PCall ( m_luaVM, 0, LUA_MULTRET, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                SString strRes = lua_tostring( m_luaVM, -1 );
                g_pGame->GetScriptDebugging()->LogPCallError( m_luaVM, strRes, true );
            }
            // Cleanup any return values
            if ( bClearReturnValues && lua_gettop ( m_luaVM ) > luaSavedTop )
                lua_settop( m_luaVM, luaSavedTop );
            return true;
        }
    }

    return false;
}


bool CLuaMain::LoadScript ( const char* szLUAScript )
{
    if ( m_luaVM && !IsLuaCompiledScript( szLUAScript, strlen( szLUAScript ) ) )
    {
        // Run the script
        if ( !CLuaMain::LuaLoadBuffer ( m_luaVM, szLUAScript, strlen(szLUAScript), NULL ) )
        {
            ResetInstructionCount ();
            int luaSavedTop = lua_gettop ( m_luaVM );
            int iret = this->PCall ( m_luaVM, 0, LUA_MULTRET, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
                g_pGame->GetScriptDebugging()->LogPCallError( m_luaVM, strRes );
            }
            // Cleanup any return values
            if ( lua_gettop ( m_luaVM ) > luaSavedTop )
                lua_settop( m_luaVM, luaSavedTop );
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


// TODO: Check the purpose of this function
void CLuaMain::Start ( void )
{

}


void CLuaMain::UnloadScript ( void )
{
    // Delete all timers and events
    m_pLuaTimerManager->RemoveAllTimers ();

    // Delete all keybinds
    list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); ++iter )
    {
        if ( (*iter)->IsJoined () )
            (*iter)->GetKeyBinds ()->RemoveAllKeys ( this );
    }

    // End the lua vm
    if ( m_luaVM )
    {
        m_pLuaManager->OnLuaMainCloseVM( this, m_luaVM );
        CLuaFunctionRef::RemoveLuaFunctionRefsForVM( m_luaVM );
        lua_close( m_luaVM );
        m_luaVM = NULL;
    }
}


void CLuaMain::DoPulse ( void )
{
    m_pLuaTimerManager->DoPulse ( this );
}


// Keep count of the number of open files in this resource and issue a warning if too high
void CLuaMain::OnOpenFile( const SString& strFilename )
{
    m_OpenFilenameList.push_back( strFilename );
    if ( m_OpenFilenameList.size() >= m_uiOpenFileCountWarnThresh )
    {
        m_uiOpenFileCountWarnThresh = m_OpenFilenameList.size() * 2;
        CLogger::LogPrintf ( "Notice: There are now %d open files in resource '%s'\n", m_OpenFilenameList.size(), GetScriptName() );
    }
}


void CLuaMain::OnCloseFile( const SString& strFilename )
{
    ListRemoveFirst( m_OpenFilenameList, strFilename );
}


CXMLFile * CLuaMain::CreateXML ( const char * szFilename )
{
    CXMLFile * pFile = g_pServerInterface->GetXML ()->CreateXML ( szFilename, true );
    if ( pFile )
    {
        m_XMLFiles.push_back ( pFile );
        if ( m_XMLFiles.size() >= m_uiOpenXMLFileCountWarnThresh )
        {
            m_uiOpenXMLFileCountWarnThresh = m_XMLFiles.size() * 2;
            CLogger::LogPrintf ( "Notice: There are now %d open XML files in resource '%s'\n", m_XMLFiles.size(), GetScriptName() );
        }
    }
    return pFile;
}


void CLuaMain::DestroyXML ( CXMLFile * pFile )
{
    m_XMLFiles.remove ( pFile );
    delete pFile;
}


void CLuaMain::DestroyXML ( CXMLNode * pRootNode )
{
    list<CXMLFile *>::iterator iter;
    for ( iter = m_XMLFiles.begin(); iter != m_XMLFiles.end(); ++iter )
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
    for ( iter = m_XMLFiles.begin(); iter != m_XMLFiles.end(); ++iter )
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
        for ( ; iter != m_pResource->IterEnd () ; ++iter )
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
    m_Displays.push_back ( pDisplay );
    return pDisplay;
}


void CLuaMain::DestroyDisplay ( CTextDisplay * pDisplay )
{
    m_Displays.remove ( pDisplay );
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
    m_TextItems.remove ( pTextItem );
    delete pTextItem;
}


CTextDisplay* CLuaMain::GetTextDisplayFromScriptID ( uint uiScriptID )
{
    CTextDisplay* pTextDisplay = (CTextDisplay*) CIdArray::FindEntry ( uiScriptID, EIdClass::TEXT_DISPLAY );
    return pTextDisplay;
}


CTextItem* CLuaMain::GetTextItemFromScriptID ( uint uiScriptID )
{
    CTextItem* pTextItem = (CTextItem*) CIdArray::FindEntry ( uiScriptID, EIdClass::TEXT_ITEM );
    return pTextItem;
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


///////////////////////////////////////////////////////////////
//
// CLuaMain::PCall
//
// lua_pcall call wrapper
//
///////////////////////////////////////////////////////////////
int CLuaMain::PCall ( lua_State *L, int nargs, int nresults, int errfunc )
{
    if ( m_uiPCallDepth++ == 0 )
        m_WarningTimer.Reset();   // Only restart timer if initial call

    g_pGame->GetScriptDebugging()->PushLuaMain ( this );
    int iret = lua_pcall ( L, nargs, nresults, errfunc );
    g_pGame->GetScriptDebugging()->PopLuaMain ( this );

    --m_uiPCallDepth;
    return iret;
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::CheckExecutionTime
//
// Issue warning if execution time is too long
//
///////////////////////////////////////////////////////////////
void CLuaMain::CheckExecutionTime( void )
{
    // Do time check
    if ( m_WarningTimer.Get() < 5000 )
        return;
    m_WarningTimer.Reset();

    // No warning if no players
    if ( g_pGame->GetPlayerManager()->Count() == 0 )
        return;

    // Issue warning about script execution time
    CLogger::LogPrintf ( "WARNING: Long execution (%s)\n", GetScriptName () );
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::LuaLoadBuffer
//
// luaL_loadbuffer call wrapper
//
///////////////////////////////////////////////////////////////
int CLuaMain::LuaLoadBuffer( lua_State *L, const char *buff, size_t sz, const char *name )
{
    if ( IsLuaCompiledScript( buff, sz ) )
    {
        ms_strExpectedUndumpHash = GenerateSha256HexString( buff, sz );
    }

    int iResult = luaL_loadbuffer( L, buff, sz, name );

    ms_strExpectedUndumpHash = "";
    return iResult;
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::OnUndump
//
// Callback from Lua when loading compiled bytes
//
///////////////////////////////////////////////////////////////
int CLuaMain::OnUndump( const char* p, size_t n )
{
    SString strGotHash = GenerateSha256HexString( p, n );
    SString strExpectedHash = ms_strExpectedUndumpHash;
    ms_strExpectedUndumpHash = "";
    if ( strExpectedHash != strGotHash )
    {
        // Not right
        return 0;
    }
    return 1;
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::InitPackageStorage
//
// Create a table for storage of Lua packages - stored in the Lua VM
//
///////////////////////////////////////////////////////////////
void CLuaMain::InitPackageStorage(lua_State* L)
{
    lua_newtable(L);                                            // stack: [tbl_new]
    lua_pushstring(L, "loaded");                                // stack: [tbl_new,"loaded"]
    lua_newtable(L);                                            // stack: [tbl_new,"loaded",tbl_new2]

    // We push our default Lua libs are loaded packages
    std::vector<const char*> szDefaultLibs = { "math", "string", "table", "debug", "utf8" };
    for (auto it : szDefaultLibs)
    {
        lua_pushstring(L, it);                                  // stack: [tbl_new,"loaded",tbl_new2,"math"]
        lua_getglobal(L, it);                                   // stack: [tbl_new,"loaded",tbl_new2,"math",_G.math]
        lua_settable(L, -3);                                    // stack: [tbl_new,"loaded",tbl_new2]
    }

    // Keep our package.loaded table safely in registry
    m_iPackageLoadedRef = luaL_ref(L, LUA_REGISTRYINDEX);       // stack: [tbl_new,"loaded"]
    lua_rawgeti(L, LUA_REGISTRYINDEX, m_iPackageLoadedRef);     // stack: [tbl_new,"loaded",tbl_new2]

    // Finally, store our original table as global package.loaded
    lua_settable(L, -3);                                        // stack: [tbl_new]
    lua_setglobal(L, "package");                                // stack: []
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::SetPackage
//
// Set the top most value as a package (No pop)
//
///////////////////////////////////////////////////////////////
void CLuaMain::SetPackage(lua_State* L, SString &strName )
{
    if (m_iPackageLoadedRef < 0)
        return;
    // We set varPkg, which is already on the stack, into package.loaded.moduleName = varPkg.
                                                            // stack: [varPkg]
    int iPkg = luaL_ref(L, LUA_REGISTRYINDEX);              // stack: []
    lua_rawgeti(L, LUA_REGISTRYINDEX, m_iPackageLoadedRef); // [tbl_loaded]


    lua_pushstring(L, strName.c_str());                     // stack: [tbl_loaded,"moduleName"]
    lua_rawgeti(L, LUA_REGISTRYINDEX, iPkg);                // stack: [tbl_loaded,"moduleName",varPkg]
    lua_rawset(L, -3);                                      // stack: [tbl_loaded]
    lua_pop(L, 2);                                          // stack: []
    lua_rawgeti(L, LUA_REGISTRYINDEX, iPkg);                // stack: [varPkg]

    // Cleanup our used registry entry, i.e. REGISTRY[i] = nil.
    lua_pushnil(L);                                         // stack: [varPkg,nil]
    lua_rawseti(L, LUA_REGISTRYINDEX, iPkg);                // stack: [varPkg]
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::GetPackage
//
// Push the value of a package of name to the stack
//
///////////////////////////////////////////////////////////////
void CLuaMain::GetPackage(lua_State* L, SString &strName )
{
    if (m_iPackageLoadedRef < 0)
        return;

    lua_rawgeti(L, LUA_REGISTRYINDEX, m_iPackageLoadedRef); // stack: [tbl_loaded]
    lua_pushstring(L, strName.c_str());                     // stack: [tbl_loaded,"moduleName"]
    lua_rawget(L, -2);                                      // stack: [tbl_loaded,varPkg]
    lua_remove(L, -2);                                      // stack: [varPkg]
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::LoadLuaLib
//
// Load a Lua lib of a given name
//
///////////////////////////////////////////////////////////////
bool CLuaMain::LoadLuaLib(lua_State *L, SString strName, SString &strError)
{
    SString strPath = strName;
    // Name format shouldn't include slashes.  Subdirs are dots.
    ReplaceOccurrencesInString(strPath, "\\", "");
    ReplaceOccurrencesInString(strPath, "/", "");
    ReplaceOccurrencesInString(strPath, ".", "/");

    SString strResPath = m_pResource->IsResourceZip() ? m_pResource->GetResourceCacheDirectoryPath() : m_pResource->GetResourceDirectoryPath();

    std::vector < char > buffer;
    // Try <resource>/?.lua
    SString strFilePath = PathJoin(strResPath, strPath + ".lua");
    if (FileExists(strFilePath))
        FileLoad(strFilePath, buffer);
    else
    {
        // Don't use a format string for safety, so we construct the error by hand
        strError += "error loading module " + strName + " from file " + strFilePath + 
            ":\n\t The specified module could not be found";

        // Try <resource>/?/init.lua
        strFilePath = PathJoin(strResPath, strPath, "init.lua");
        if (FileExists(strFilePath))
            FileLoad(strFilePath, buffer);
        else
        {
            strError += "\n";
            strError += "error loading module " + strName + " from file " + strFilePath +
                ":\n\t The specified module could not be found";
            return false;
        }
    }

    if (buffer.size() > 0)
    {
        int luaSavedTop = lua_gettop(L);
        LoadScriptFromBuffer(&buffer.at(0), buffer.size(), strFilePath.c_str(), false);
        // Only keep the first return value
        if (lua_gettop(L) > luaSavedTop)
            lua_settop(L, luaSavedTop+1);

        if (lua_type(L, -1) == LUA_TNIL)
        {
            strError += "error loading module " + strName + " from file " + strFilePath +
                ":\n\t Module didn't return a value";
            return false;
        }

        SetPackage(L, strName);  // Store our package into package.loaded
        return true;
    }
    else
        strError += "error loading module " + strName + " from file " + strFilePath +
            ":\n\t Error loading script file";

    return false;
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::LoadClib
//
// Load a C lib of a given name
//
///////////////////////////////////////////////////////////////
bool CLuaMain::LoadClib(lua_State* L, SString strName, SString &strError )
{
    SString strPath = strName;
    // Name format shouldn't include slashes.  Subdirs are dots.
    ReplaceOccurrencesInString(strPath, "\\", "");
    ReplaceOccurrencesInString(strPath, "/", "");
    ReplaceOccurrencesInString(strPath, ".", "/");

    strPath = PathJoin(g_pServerInterface->GetModManager()->GetServerPath(), SERVER_BIN_PATH_MOD, "modules", strPath);

#ifdef WIN32
    strPath += ".dll";
#else
    strPath += ".so";
#endif

    if (!FileExists(strPath))
    {
        strError += "error loading module " + strName + " from file " + strPath +
            ":\n\t The specified module could not be found";
        return false;
    }

    // Check ACL permissions
    if ( !g_pGame->GetACLManager()->CanObjectUseRight(m_pResource->GetName().c_str(),
            CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
            "*",
            CAccessControlListRight::RIGHT_TYPE_MODULE,
            false) &&
         !g_pGame->GetACLManager()->CanObjectUseRight(m_pResource->GetName().c_str(),
            CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
            strName.c_str(),
            CAccessControlListRight::RIGHT_TYPE_MODULE,
            false)
        )
    {
        strError += "error loading module " + strName + " from file " + strPath +
            ":\n\t ACL access denied.  Grant \"module." + strName + "\" right to resource " + m_pResource->GetName();
        return false;
    }

    if (!luaL_loader_C(L, strName.c_str(), strPath.c_str()) || lua_type(L, -1) == LUA_TNIL)
    {
        strError += "error loading module " + strName + " from file " + strPath +
            ":\n\t Failed to load module";
        return false;
    }

    SetPackage(L, strName);
    return true;
}