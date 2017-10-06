/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Util.cpp
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

#include "StdInc.h"

int CLuaFunctionDefs::GetTok ( lua_State* luaVM )
{
    SString strInput = "";
    unsigned int uiToken = 0;
    unsigned int uiDelimiter = 0;
    SString strDelimiter;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strInput );
    argStream.ReadNumber ( uiToken );

    if ( argStream.NextIsNumber ( ) )
    {
        argStream.ReadNumber ( uiDelimiter );
        wchar_t wUNICODE[2] = { uiDelimiter, '\0' };
        strDelimiter = UTF16ToMbUTF8(wUNICODE);
    }
    else  // It's already a string
        argStream.ReadString ( strDelimiter );

    if ( !argStream.HasErrors ( ) )
    {
        unsigned int uiCount = 0;

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

int CLuaFunctionDefs::tocolor ( lua_State* luaVM )
{
//  int tocolor ( int red, int green, int blue [, int alpha = 255] )
    int iRed; int iGreen; int iBlue; int iAlpha;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iRed );
    argStream.ReadNumber ( iGreen );
    argStream.ReadNumber ( iBlue );
    argStream.ReadNumber ( iAlpha, 255 );

    if ( !argStream.HasErrors () )
    {
        // Make it into an unsigned long
        unsigned long ulColor = COLOR_RGBA ( iRed, iGreen, iBlue, iAlpha );
        lua_pushinteger ( luaVM, static_cast < lua_Integer > ( ulColor ) );
        return 1;
    }

    // Make it black so funcs dont break
    unsigned long ulColor = COLOR_RGBA ( 0, 0, 0, 255 );
    lua_pushnumber ( luaVM, static_cast < lua_Number > ( ulColor ) );
    return 1;
}

int CLuaFunctionDefs::GetValidPedModels ( lua_State* luaVM )
{
    int iIndex = 0;
    lua_newtable ( luaVM );
    for( int i = 0; i <= 312; i++)
    {
        if ( CClientPlayerManager::IsValidModel(i) )
        {
            lua_pushnumber ( luaVM , ++iIndex);
            lua_pushnumber ( luaVM , i);
            lua_settable ( luaVM , -3);
        }
    }

    return 1;
}

int CLuaFunctionDefs::GetNetworkUsageData ( lua_State* luaVM )
{
    SPacketStat m_PacketStats [ 2 ] [ 256 ];
    memcpy ( m_PacketStats, g_pNet->GetPacketStats (), sizeof ( m_PacketStats ) );

    lua_createtable ( luaVM, 0, 2 );

    lua_pushstring ( luaVM, "in" );
    lua_createtable ( luaVM, 0, 2 );
    {
        lua_pushstring ( luaVM, "bits" );
        lua_createtable ( luaVM, 255, 1 );
        for ( unsigned int i = 0; i < 256; ++i )
        {
            const SPacketStat& statIn = m_PacketStats [ CNet::STATS_INCOMING_TRAFFIC ] [ i ];
            lua_pushnumber ( luaVM, statIn.iTotalBytes * 8 );
            lua_rawseti ( luaVM, -2, i );
        }
        lua_rawset ( luaVM, -3 );

        lua_pushstring ( luaVM, "count" );
        lua_createtable ( luaVM, 255, 1 );
        for ( unsigned int i = 0; i < 256; ++i )
        {
            const SPacketStat& statIn = m_PacketStats [ CNet::STATS_INCOMING_TRAFFIC ] [ i ];
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
            const SPacketStat& statOut = m_PacketStats [ CNet::STATS_OUTGOING_TRAFFIC ] [ i ];
            lua_pushnumber ( luaVM, statOut.iTotalBytes * 8 );
            lua_rawseti ( luaVM, -2, i );
        }
        lua_rawset ( luaVM, -3 );

        lua_pushstring ( luaVM, "count" );
        lua_createtable ( luaVM, 255, 1 );
        for ( unsigned int i = 0; i < 256; ++i )
        {
            const SPacketStat& statOut = m_PacketStats [ CNet::STATS_OUTGOING_TRAFFIC ] [ i ];
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
    NetStatistics stats;
    if ( g_pNet->GetNetworkStatistics ( &stats ) )
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
        lua_pushnumber ( luaVM, 1 );
        lua_settable   ( luaVM, -3 );

        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVersion ( lua_State* luaVM )
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

int CLuaFunctionDefs::GetLocalization ( lua_State* luaVM )
{
    lua_createtable ( luaVM, 0, 2 );

    lua_pushstring ( luaVM, "code" );
    lua_pushstring ( luaVM, g_pCore->GetLocalization()->GetLanguageCode ().c_str() );
    lua_settable   ( luaVM, -3 );

    lua_pushstring ( luaVM, "name" );
    lua_pushstring ( luaVM, g_pCore->GetLocalization()->GetLanguageName ().c_str() );
    lua_settable   ( luaVM, -3 );

    return 1;
}

int CLuaFunctionDefs::GetPerformanceStats ( lua_State* luaVM )
{
//  table getPerformanceStats ( string category, string options, string filter )
    SString strCategory, strOptions, strFilter;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strCategory );
    argStream.ReadString ( strOptions, "" );
    argStream.ReadString ( strFilter, "" );

    if ( !argStream.HasErrors () )
    {
        CClientPerfStatResult Result;
        CClientPerfStatManager::GetSingleton ()->GetStats ( &Result, strCategory, strOptions, strFilter );

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


int CLuaFunctionDefs::SetDevelopmentMode ( lua_State* luaVM )
{
//  bool setDevelopmentMode ( bool enable )
//  bool setDevelopmentMode ( string command )
    bool bEnable; bool bEnableWeb; SString strCommand;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsString () )
    {
        argStream.ReadString ( strCommand );
        //g_pClientGame->SetDevSetting ( strCommand );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    argStream.ReadBool ( bEnable );
    argStream.ReadBool ( bEnableWeb, false );

    if ( !argStream.HasErrors () )
    {
        g_pClientGame->SetDevelopmentMode ( bEnable, bEnableWeb );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetDevelopmentMode ( lua_State* luaVM )
{
//  bool getDevelopmentMode ()
    bool bResult = g_pClientGame->GetDevelopmentMode ();
    lua_pushboolean ( luaVM, bResult );
    return 1;
}


int CLuaFunctionDefs::DownloadFile ( lua_State* luaVM )
{
    SString strFile = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strFile );

    if ( !argStream.HasErrors ( ) )
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Grab its resource
            CResource * pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                std::list < CResourceFile* > ::const_iterator iter = pResource->IterBeginResourceFiles();
                for ( ; iter != pResource->IterEndResourceFiles() ; iter++ ) 
                {
                    if ( strcmp ( strFile, (*iter)->GetShortName() ) == 0 )
                    {
                        if ( CStaticFunctionDefinitions::DownloadFile ( pResource, strFile, (*iter)->GetServerChecksum() ) )
                        {
                            lua_pushboolean ( luaVM, true );
                            return 1;
                        }
                    }
                }
                m_pScriptDebugging->LogCustom ( luaVM, 255, 255, 255, "%s: File doesn't exist", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::AddDebugHook ( lua_State* luaVM )
{
//  bool AddDebugHook ( string hookType, function callback[, table allowedNames ] )
    EDebugHookType hookType; CLuaFunctionRef callBack; std::vector < SString > allowedNames;

    CScriptArgReader argStream( luaVM );
    argStream.ReadEnumString( hookType );
    argStream.ReadFunction( callBack );
    if ( argStream.NextIsTable() )
        argStream.ReadStringTable( allowedNames );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors() )
    {
        if ( g_pClientGame->GetDebugHookManager()->AddDebugHook( hookType, callBack, allowedNames ) )
        {
            lua_pushboolean( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveDebugHook ( lua_State* luaVM )
{
//  bool RemoveDebugHook ( string hookType, function callback )
    EDebugHookType hookType; CLuaFunctionRef callBack;

    CScriptArgReader argStream( luaVM );
    argStream.ReadEnumString( hookType );
    argStream.ReadFunction( callBack );
    argStream.ReadFunctionComplete();

    if ( !argStream.HasErrors() )
    {
        if ( g_pClientGame->GetDebugHookManager()->RemoveDebugHook( hookType, callBack ) )
        {
            lua_pushboolean( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean( luaVM, false );
    return 1;
}
