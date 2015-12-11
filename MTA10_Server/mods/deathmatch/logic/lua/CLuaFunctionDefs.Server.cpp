/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Server.cpp
*  PURPOSE:     Lua special server function definitions
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
#define MIN_SERVER_REQ_CALLREMOTE_CONNECT_TIMEOUT           "1.3.5"


int CLuaFunctionDefs::GetMaxPlayers ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, CStaticFunctionDefinitions::GetMaxPlayers () );
    return 1;
}

int CLuaFunctionDefs::SetMaxPlayers ( lua_State* luaVM )
{
    unsigned int uiMaxPlayers;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiMaxPlayers );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, CStaticFunctionDefinitions::SetMaxPlayers ( uiMaxPlayers ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::OutputChatBox ( lua_State* luaVM )
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

    if ( argStream.NextIsNumber () && argStream.NextIsNumber ( 1 ) && argStream.NextIsNumber ( 2 ) )
    {
        argStream.ReadNumber ( ucRed );
        argStream.ReadNumber ( ucGreen );
        argStream.ReadNumber ( ucBlue );
    }
    else
        argStream.Skip ( 3 );

    argStream.ReadBool ( bColorCoded, false );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CStaticFunctionDefinitions::OutputChatBox ( (const char*) ssChat, pElement, ucRed, ucGreen, ucBlue, bColorCoded, pLuaMain );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::OOP_OutputChatBox ( lua_State * luaVM )
{
    // bool Player:outputChat ( string text [, int r=231, int g=217, int b=176, bool colorCoded=false ] )
    CElement* pElement; SString strText; uchar ucRed = 231; uchar ucGreen = 217; uchar ucBlue = 176; bool bColorCoded;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strText );

    if ( argStream.NextIsNumber ( 0 ) && argStream.NextIsNumber ( 1 ) && argStream.NextIsNumber ( 2 ) )
    {
        argStream.ReadNumber ( ucRed );
        argStream.ReadNumber ( ucGreen );
        argStream.ReadNumber ( ucBlue );
    }
    else
        argStream.Skip ( 3 );

    argStream.ReadBool ( bColorCoded, false );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CStaticFunctionDefinitions::OutputChatBox ( strText, pElement, ucRed, ucGreen, ucBlue, bColorCoded, pLuaMain );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::OutputConsole ( lua_State* luaVM )
{
    SString strMessage;
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );

    argStream.ReadString ( strMessage );
    argStream.ReadUserData ( pElement, m_pRootElement );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::OutputConsole ( strMessage, pElement ) )
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


int CLuaFunctionDefs::OutputDebugString ( lua_State* luaVM )
{
    SString strMessage;
    unsigned int uiLevel;
    unsigned char ucR, ucG, ucB;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strMessage );
    argStream.ReadNumber ( uiLevel, 3 );

    if ( uiLevel == 0 )
    {
        argStream.ReadNumber ( ucR, 0xFF );
        argStream.ReadNumber ( ucG, 0xFF );
        argStream.ReadNumber ( ucB, 0xFF );
    }

    if ( !argStream.HasErrors () )
    {
        if ( uiLevel > 3 )
        {
            m_pScriptDebugging->LogWarning ( luaVM, "Bad level argument sent to %s (0-3)", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );

            lua_pushboolean ( luaVM, false );
            return 1;
        }

        if ( uiLevel == 1 )
        {
            m_pScriptDebugging->LogError ( luaVM, "%s", strMessage.c_str () );
        }
        else if ( uiLevel == 2 )
        {
            m_pScriptDebugging->LogWarning ( luaVM, "%s", strMessage.c_str () );
        }
        else if ( uiLevel == 3 )
        {
            m_pScriptDebugging->LogInformation ( luaVM, "%s", strMessage.c_str () );
        }
        else if ( uiLevel == 0 )
        {
            m_pScriptDebugging->LogCustom ( luaVM, ucR, ucG, ucB, "%s", strMessage.c_str () );
        }
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::AddCommandHandler ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveCommandHandler ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ExecuteCommandHandler ( lua_State* luaVM )
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
                pClient = static_cast <CClient*> ( static_cast <CPlayer*> ( pElement ) );

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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::OutputServerLog ( lua_State* luaVM )
{
    SString strMessage;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strMessage );

    if ( !argStream.HasErrors () )
    {
        // Print it
        CLogger::LogPrintf ( LOGLEVEL_MEDIUM, "%s\n", strMessage.c_str () );
        lua_pushboolean ( luaVM, true );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetServerPort ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pGame->GetConfig ()->GetServerPort () );
    return 1;
}

int CLuaFunctionDefs::Set ( lua_State* luaVM )
{
    SString strSetting;
    CLuaArguments Args;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strSetting );
    argStream.ReadLuaArguments ( Args );

    if ( !argStream.HasErrors () )
    {
        CResource* pResource = m_pLuaManager->GetVirtualMachineResource ( luaVM );
        if ( pResource )
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
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

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

int CLuaFunctionDefs::Get ( lua_State* luaVM )
{
    SString strSetting;
    CLuaArguments Args;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strSetting );

    if ( !argStream.HasErrors () )
    {
        CResource* pResource = m_pLuaManager->GetVirtualMachineResource ( luaVM );
        if ( pResource )
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
                    CXMLAttribute *pAttribute = pNode->GetAttributes ().Find ( strAttribute.c_str () );
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
                }
                else {
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
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

// Call a function on a remote server
int CLuaFunctionDefs::CallRemote ( lua_State* luaVM )
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
                g_pGame->GetRemoteCalls ()->Call ( strHost, strResourceName, strFunctionName, &args, luaMain, iLuaFunction, uiConnectionAttempts, uiConnectTimeoutMs );
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
                g_pGame->GetRemoteCalls ()->Call ( strURL, &args, luaMain, iLuaFunction, uiConnectionAttempts, uiConnectTimeoutMs );
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
int CLuaFunctionDefs::FetchRemote ( lua_State* luaVM )
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
            g_pGame->GetRemoteCalls ()->Call ( strURL, &args, strPostData, bPostBinary, luaMain, iLuaFunction, uiConnectionAttempts, uiConnectTimeoutMs );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetServerName ( lua_State* luaVM )
{
    lua_pushstring ( luaVM, g_pGame->GetConfig ()->GetServerName ().c_str () );
    return 1;
}


int CLuaFunctionDefs::GetServerHttpPort ( lua_State* luaVM )
{
    lua_pushnumber ( luaVM, g_pGame->GetConfig ()->GetHTTPPort () );
    return 1;
}


int CLuaFunctionDefs::GetServerIP ( lua_State* luaVM )
{
    lua_pushstring ( luaVM, "moo" );
    return 1;
}


int CLuaFunctionDefs::GetServerPassword ( lua_State* luaVM )
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


int CLuaFunctionDefs::SetServerPassword ( lua_State* luaVM )
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
            argStream.SetCustomError ( "password must be shorter than 32 chars and just contain visible characters" );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetServerConfigSetting ( lua_State* luaVM )
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
            result.PushAsTable ( luaVM );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetServerConfigSetting ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::shutdown ( lua_State* luaVM )
{
    SString strReason;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strReason, "No reason specified" );

    if ( !argStream.HasErrors () )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Fail
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetMapName ( lua_State* luaVM )
{
    // Grab ASE
    ASE * pASE = ASE::GetInstance ();
    if ( pASE )
    {
        // Grab the mapname string
        const char* szMapName = pASE->GetMapName ();
        if ( szMapName[0] != 0 )
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


int CLuaFunctionDefs::GetGameType ( lua_State* luaVM )
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


int CLuaFunctionDefs::SetGameType ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetMapName ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetRuleValue ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetRuleValue ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveRuleValue ( lua_State* luaVM )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetNetworkUsageData ( lua_State* luaVM )
{
    SPacketStat m_PacketStats[2][256];
    memcpy ( m_PacketStats, g_pNetServer->GetPacketStats (), sizeof ( m_PacketStats ) );

    lua_createtable ( luaVM, 0, 2 );

    lua_pushstring ( luaVM, "in" );
    lua_createtable ( luaVM, 0, 2 );
    {
        lua_pushstring ( luaVM, "bits" );
        lua_createtable ( luaVM, 255, 1 );
        for ( unsigned int i = 0; i < 256; ++i )
        {
            const SPacketStat& statIn = m_PacketStats[CNetServer::STATS_INCOMING_TRAFFIC][i];
            lua_pushnumber ( luaVM, statIn.iTotalBytes * 8 );
            lua_rawseti ( luaVM, -2, i );
        }
        lua_rawset ( luaVM, -3 );

        lua_pushstring ( luaVM, "count" );
        lua_createtable ( luaVM, 255, 1 );
        for ( unsigned int i = 0; i < 256; ++i )
        {
            const SPacketStat& statIn = m_PacketStats[CNetServer::STATS_INCOMING_TRAFFIC][i];
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
            const SPacketStat& statOut = m_PacketStats[CNetServer::STATS_OUTGOING_TRAFFIC][i];
            lua_pushnumber ( luaVM, statOut.iTotalBytes * 8 );
            lua_rawseti ( luaVM, -2, i );
        }
        lua_rawset ( luaVM, -3 );

        lua_pushstring ( luaVM, "count" );
        lua_createtable ( luaVM, 255, 1 );
        for ( unsigned int i = 0; i < 256; ++i )
        {
            const SPacketStat& statOut = m_PacketStats[CNetServer::STATS_OUTGOING_TRAFFIC][i];
            lua_pushnumber ( luaVM, statOut.iCount );
            lua_rawseti ( luaVM, -2, i );
        }
        lua_rawset ( luaVM, -3 );
    }
    lua_rawset ( luaVM, -3 );

    return 1;
}


int CLuaFunctionDefs::GetNetworkStats ( lua_State* luaVM )
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
            lua_pushnumber ( luaVM, static_cast <double> ( stats.bytesReceived ) );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "bytesSent" );
            lua_pushnumber ( luaVM, static_cast <double> ( stats.bytesSent ) );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "packetsReceived" );
            lua_pushnumber ( luaVM, stats.packetsReceived );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "packetsSent" );
            lua_pushnumber ( luaVM, stats.packetsSent );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "packetlossTotal" );
            lua_pushnumber ( luaVM, stats.packetlossTotal );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "packetlossLastSecond" );
            lua_pushnumber ( luaVM, stats.packetlossLastSecond );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "messagesInSendBuffer" );
            lua_pushnumber ( luaVM, stats.messagesInSendBuffer );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "messagesInResendBuffer" );
            lua_pushnumber ( luaVM, stats.messagesInResendBuffer );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "isLimitedByCongestionControl" );
            lua_pushnumber ( luaVM, stats.isLimitedByCongestionControl ? 1 : 0 );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "isLimitedByOutgoingBandwidthLimit" );
            lua_pushnumber ( luaVM, stats.isLimitedByOutgoingBandwidthLimit ? 1 : 0 );
            lua_settable ( luaVM, -3 );

            lua_pushstring ( luaVM, "encryptionStatus" );
            lua_pushnumber ( luaVM, 1 );
            lua_settable ( luaVM, -3 );

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVersion ( lua_State* luaVM )
{
    lua_createtable ( luaVM, 0, 8 );

    lua_pushstring ( luaVM, "number" );
    lua_pushnumber ( luaVM, CStaticFunctionDefinitions::GetVersion () );
    lua_settable ( luaVM, -3 );

    lua_pushstring ( luaVM, "mta" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionString () );
    lua_settable ( luaVM, -3 );

    lua_pushstring ( luaVM, "name" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionName () );
    lua_settable ( luaVM, -3 );

    lua_pushstring ( luaVM, "netcode" );
    lua_pushnumber ( luaVM, CStaticFunctionDefinitions::GetNetcodeVersion () );
    lua_settable ( luaVM, -3 );

    lua_pushstring ( luaVM, "os" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetOperatingSystemName () );
    lua_settable ( luaVM, -3 );

    lua_pushstring ( luaVM, "type" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionBuildType () );
    lua_settable ( luaVM, -3 );

    lua_pushstring ( luaVM, "tag" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionBuildTag () );
    lua_settable ( luaVM, -3 );

    lua_pushstring ( luaVM, "sortable" );
    lua_pushstring ( luaVM, CStaticFunctionDefinitions::GetVersionSortable () );
    lua_settable ( luaVM, -3 );

    return 1;
}

int CLuaFunctionDefs::GetModuleInfo ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING ) {
        list < CLuaModule* > lua_LoadedModules = m_pLuaModuleManager->GetLoadedModules ();
        list < CLuaModule* > ::iterator iter = lua_LoadedModules.begin ();
        SString strAttribute = lua_tostring ( luaVM, 2 );
        SString strModuleName = lua_tostring ( luaVM, 1 );
        for ( ; iter != lua_LoadedModules.end (); ++iter )
        {
            if ( stricmp ( strModuleName, ( *iter )->_GetName ().c_str () ) == 0 ) {
                lua_newtable ( luaVM );

                lua_pushstring ( luaVM, "name" );
                lua_pushstring ( luaVM, ( *iter )->_GetFunctions ().szModuleName );
                lua_settable ( luaVM, -3 );

                lua_pushstring ( luaVM, "author" );
                lua_pushstring ( luaVM, ( *iter )->_GetFunctions ().szAuthor );
                lua_settable ( luaVM, -3 );

                lua_pushstring ( luaVM, "version" );
                SString strVersion ( "%.2f", ( *iter )->_GetFunctions ().fVersion );
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

int CLuaFunctionDefs::GetModules ( lua_State* luaVM )
{
    lua_newtable ( luaVM );
    list < CLuaModule* > lua_LoadedModules = m_pLuaModuleManager->GetLoadedModules ();
    list < CLuaModule* > ::iterator iter = lua_LoadedModules.begin ();
    unsigned int uiIndex = 1;
    for ( ; iter != lua_LoadedModules.end (); ++iter )
    {
        lua_pushnumber ( luaVM, uiIndex++ );
        lua_pushstring ( luaVM, ( *iter )->_GetFunctions ().szFileName );
        lua_settable ( luaVM, -3 );
    }
    return 1;
}

int CLuaFunctionDefs::IsOOPEnabled ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
        lua_pushboolean ( luaVM, pLuaMain->IsOOPEnabled () );
    else
        lua_pushnil ( luaVM );

    return 1;
}

int CLuaFunctionDefs::GetPerformanceStats ( lua_State* luaVM )
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
        for ( int c = 0; c < Result.ColumnCount (); c++ )
        {
            const SString& name = Result.ColumnName ( c );
            lua_pushnumber ( luaVM, c + 1 );                      // row index number (starting at 1, not 0)
            lua_pushlstring ( luaVM, name.c_str (), name.length () );
            lua_settable ( luaVM, -3 );
        }

        lua_newtable ( luaVM );
        for ( int r = 0; r < Result.RowCount (); r++ )
        {
            lua_newtable ( luaVM );                             // new table
            lua_pushnumber ( luaVM, r + 1 );                      // row index number (starting at 1, not 0)
            lua_pushvalue ( luaVM, -2 );                        // value
            lua_settable ( luaVM, -4 );                         // refer to the top level table

            for ( int c = 0; c < Result.ColumnCount (); c++ )
            {
                SString& cell = Result.Data ( c, r );
                lua_pushnumber ( luaVM, c + 1 );
                lua_pushlstring ( luaVM, cell.c_str (), cell.length () );
                lua_settable ( luaVM, -3 );
            }
            lua_pop ( luaVM, 1 );                               // pop the inner table
        }
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
