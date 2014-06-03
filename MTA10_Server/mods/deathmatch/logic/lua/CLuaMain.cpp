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

#include "CLuaFunctionDefinitions.h"

#include <clocale>

static CLuaManager* m_pLuaManager;

#define HOOK_INSTRUCTION_COUNT 1000000
#define HOOK_MAXIMUM_TIME 5000

extern CGame* g_pGame;
extern CNetServer* g_pRealNetServer;

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
    lua_register ( m_luaVM, "dofile", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "loadfile", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "require", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "loadlib", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "getfenv", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "newproxy", CLuaFunctionDefinitions::DisabledFunction );
}



void CLuaMain::AddVector3DClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classmetamethod ( luaVM, "__tostring", CLuaVectorDefs::ToString );
    lua_classmetamethod ( luaVM, "__gc", CLuaVectorDefs::Destroy );

    lua_classmetamethod ( luaVM, "__add", CLuaVectorDefs::Add );
    lua_classmetamethod ( luaVM, "__sub", CLuaVectorDefs::Sub );
    lua_classmetamethod ( luaVM, "__mul", CLuaVectorDefs::Mul );
    lua_classmetamethod ( luaVM, "__div", CLuaVectorDefs::Div );
    lua_classmetamethod ( luaVM, "__pow", CLuaVectorDefs::Pow );
    lua_classmetamethod ( luaVM, "__unm", CLuaVectorDefs::Unm );
    lua_classmetamethod ( luaVM, "__eq", CLuaVectorDefs::Eq );

    lua_classfunction ( luaVM, "create", "", CLuaVectorDefs::Create );
    lua_classfunction ( luaVM, "normalize", "", CLuaVectorDefs::Normalize );
    lua_classfunction ( luaVM, "cross", "", CLuaVectorDefs::Cross );
    lua_classfunction ( luaVM, "dot", "", CLuaVectorDefs::Dot );

    lua_classfunction ( luaVM, "getLength", "", CLuaVectorDefs::GetLength );
    lua_classfunction ( luaVM, "getSquaredLength", "", CLuaVectorDefs::GetLengthSquared );
    lua_classfunction ( luaVM, "getNormalized", "", CLuaVectorDefs::GetNormalized );
    lua_classfunction ( luaVM, "getX", "", CLuaVectorDefs::GetX );
    lua_classfunction ( luaVM, "getY", "", CLuaVectorDefs::GetY );
    lua_classfunction ( luaVM, "getZ", "", CLuaVectorDefs::GetZ );

    lua_classfunction ( luaVM, "setX", "", CLuaVectorDefs::SetX );
    lua_classfunction ( luaVM, "setY", "", CLuaVectorDefs::SetY );
    lua_classfunction ( luaVM, "setZ", "", CLuaVectorDefs::SetZ );

    lua_classvariable ( luaVM, "x", "", "", CLuaVectorDefs::SetX, CLuaVectorDefs::GetX );
    lua_classvariable ( luaVM, "y", "", "", CLuaVectorDefs::SetY, CLuaVectorDefs::GetY );
    lua_classvariable ( luaVM, "z", "", "", CLuaVectorDefs::SetZ, CLuaVectorDefs::GetZ );
    lua_classvariable ( luaVM, "length", "", "", NULL, CLuaVectorDefs::GetLength );
    lua_classvariable ( luaVM, "squaredLength", "", "", NULL, CLuaVectorDefs::GetLengthSquared );
    lua_classvariable ( luaVM, "normalized", "", "", NULL, CLuaVectorDefs::GetNormalized );

    lua_registerclass ( luaVM, "Vector3" );
}


void CLuaMain::AddVector2DClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Vector2" );
}


void CLuaMain::AddMatrixClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classmetamethod ( luaVM, "__tostring", CLuaMatrixDefs::ToString );
    lua_classmetamethod ( luaVM, "__gc", CLuaMatrixDefs::Destroy );

    lua_classmetamethod ( luaVM, "__add", CLuaMatrixDefs::Add );
    lua_classmetamethod ( luaVM, "__sub", CLuaMatrixDefs::Sub );
    lua_classmetamethod ( luaVM, "__mul", CLuaMatrixDefs::Mul );
    lua_classmetamethod ( luaVM, "__div", CLuaMatrixDefs::Div );

    lua_classfunction ( luaVM, "create", "", CLuaMatrixDefs::Create );

    lua_classfunction ( luaVM, "getPosition", "", CLuaMatrixDefs::GetPosition );
    lua_classfunction ( luaVM, "getRotation", "", CLuaMatrixDefs::GetRotation );
    lua_classfunction ( luaVM, "getFront", "", CLuaMatrixDefs::GetFront );
    lua_classfunction ( luaVM, "getRight", "", CLuaMatrixDefs::GetRight );
    lua_classfunction ( luaVM, "getUp", "", CLuaMatrixDefs::GetUp );

    lua_classfunction ( luaVM, "setPosition", "", CLuaMatrixDefs::SetPosition );
    lua_classfunction ( luaVM, "setFront", "", CLuaMatrixDefs::SetFront );
    lua_classfunction ( luaVM, "setRight", "", CLuaMatrixDefs::SetRight );
    lua_classfunction ( luaVM, "setUp", "", CLuaMatrixDefs::SetUp );

    lua_classvariable ( luaVM, "position", "", "", CLuaMatrixDefs::SetPosition, CLuaMatrixDefs::GetPosition );
    lua_classvariable ( luaVM, "rotation", "", "", NULL, CLuaMatrixDefs::GetRotation );
    lua_classvariable ( luaVM, "front", "", "", CLuaMatrixDefs::SetFront, CLuaMatrixDefs::GetFront );
    lua_classvariable ( luaVM, "right", "", "", CLuaMatrixDefs::SetRight, CLuaMatrixDefs::GetRight );
    lua_classvariable ( luaVM, "up", "", "", CLuaMatrixDefs::SetUp, CLuaMatrixDefs::GetUp );

    lua_registerclass ( luaVM, "Matrix" );
}

void CLuaMain::AddElementClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_registerclass ( luaVM, "Element" );
}

void CLuaMain::AddACLClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "ACL" );
}

void CLuaMain::AddACLGroupClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "ACLGroup" );
}

void CLuaMain::AddAccountClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Account" );
    
}

void CLuaMain::AddBanClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Ban" );
}

void CLuaMain::AddBlipClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Blip", "Element" );
    
}

void CLuaMain::AddColShapeClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "ColShape", "Element" );
}

void CLuaMain::AddFileClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "File" );
}

void CLuaMain::AddMarkerClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Marker", "Element" );
}

void CLuaMain::AddObjectClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Object", "Element" );
}

void CLuaMain::AddPedClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Ped", "Element" );
}

void CLuaMain::AddPickupClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Pickup", "Element" );
}

void CLuaMain::AddPlayerClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Player", "Ped" );
}

void CLuaMain::AddRadarAreaClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "RadarArea", "Element" );
}

void CLuaMain::AddResourceClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Resource" );
}

void CLuaMain::AddConnectionClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Connection" );
}

void CLuaMain::AddQueryHandleClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "QueryHandle" );
}

void CLuaMain::AddTeamClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Team", "Element" );
}

void CLuaMain::AddTextDisplayClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "TextDisplay" );
}

void CLuaMain::AddTextItemClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "TextItem" );
}

void CLuaMain::AddVehicleClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Vehicle", "Element" );
}

void CLuaMain::AddWaterClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Water", "Element" );
}

void CLuaMain::AddXMLNodeClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "XMLNode" );
}

void CLuaMain::InitClasses ( lua_State* luaVM )
{
    lua_initclasses             ( luaVM );


    // Element
    lua_newclass ( luaVM );

    AddVector3DClass            ( luaVM );
    //AddVector2DClass          ( luaVM );
    AddMatrixClass              ( luaVM );

    if ( !m_bEnableOOP )
        return;

    AddElementClass             ( luaVM );
    AddACLClass                 ( luaVM );
    AddACLGroupClass            ( luaVM );
    AddAccountClass             ( luaVM );
    AddAdminClass               ( luaVM );
    AddBlipClass                ( luaVM );
    AddColShapeClass            ( luaVM );
    AddFileClass                ( luaVM );
    AddMarkerClass              ( luaVM );
    AddObjectClass              ( luaVM );
    AddPedClass                 ( luaVM );
    AddPickupClass              ( luaVM );
    AddPlayerClass              ( luaVM );
    AddRadarAreaClass           ( luaVM );
    AddResourceClass            ( luaVM );
    AddConnectionClass          ( luaVM );
    AddQueryHandleClass         ( luaVM );
    AddTeamClass                ( luaVM );
    AddTextDisplayClass         ( luaVM );
    AddTextItemClass            ( luaVM );
    AddVehicleClass             ( luaVM );
    AddWaterClass               ( luaVM );
    AddXMLNodeClass             ( luaVM );
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

bool CLuaMain::LoadScriptFromBuffer ( const char* cpInBuffer, unsigned int uiInSize, const char* szFileName )
{
    SString strNiceFilename = ConformResourcePath( szFileName );

    // Decrypt if required
    const char* cpBuffer;
    uint uiSize;
    if ( !g_pRealNetServer->DecryptScript( cpInBuffer, uiInSize, &cpBuffer, &uiSize, strNiceFilename ) )
    {
        // Problems problems
        if ( GetTimeString( true ) <= INVALID_COMPILED_SCRIPT_CUTOFF_DATE )
        {
            SString strMessage( "%s is invalid and will not work after %s. Please re-compile at http://luac.mtasa.com/", *strNiceFilename, INVALID_COMPILED_SCRIPT_CUTOFF_DATE ); 
            g_pGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Script warning: %s", *strMessage );
            // cpBuffer is always valid after call to DecryptScript
        }
        else
        {
            SString strMessage( "%s is invalid. Please re-compile at http://luac.mtasa.com/", *strNiceFilename ); 
            g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", *strMessage );
            return false;
        }
    }

    bool bUTF8;

    // UTF-8 BOM?  Compare by checking the standard UTF-8 BOM
    if ( IsUTF8BOM( cpBuffer, uiSize ) == false )
    {
        // Maybe not UTF-8, if we have a >80% heuristic detection confidence, assume it is
        bUTF8 = ( GetUTF8Confidence ( (const unsigned char*)cpBuffer, uiSize ) >= 80 );
    }
    else
    {
        // If there's a BOM, load ignoring the first 3 bytes
        bUTF8 = true;
        cpBuffer += 3;
        uiSize -= 3;
    }

    // If compiled script, make sure correct chunkname is embedded
    EmbedChunkName( strNiceFilename, &cpBuffer, &uiSize );

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
        if ( luaL_loadbuffer ( m_luaVM, bUTF8 ? cpBuffer : strUTFScript.c_str(), uiSize, SString ( "@%s", *strNiceFilename ) ) )
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
                {
                    if ( !strRes.ContainsI ( ExtractFilename ( strNiceFilename ) ) )
                    {
                        // Add filename to error message, if not already present
                        strRes = SString ( "%s (global scope) - %s", *strNiceFilename, *strRes );
                    }
                    g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "%s", strRes.c_str () );
                }
            }
            // Cleanup any return values
            if ( lua_gettop ( m_luaVM ) > luaSavedTop )
                lua_settop( m_luaVM, luaSavedTop );
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
            int luaSavedTop = lua_gettop ( m_luaVM );
            int iret = this->PCall ( m_luaVM, 0, LUA_MULTRET, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Executing in-line script failed: %s", strRes.c_str () );
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
    dassert ( !pTextDisplay || ListContains ( m_Displays, pTextDisplay ) );
    return pTextDisplay;
}

CTextItem* CLuaMain::GetTextItemFromScriptID ( uint uiScriptID )
{
    CTextItem* pTextItem = (CTextItem*) CIdArray::FindEntry ( uiScriptID, EIdClass::TEXT_ITEM );
    dassert ( !pTextItem || ListContains ( m_TextItems, pTextItem ) );
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
