/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaTrainTrackDefs.cpp
*  PURPOSE:     Lua train track definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
using std::list;

void CLuaTrainTrackDefs::LoadFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "getTrackNodes", GetTrainTrackNumberOfNodes );
    CLuaCFunctions::AddFunction ( "getTrackID", GetTrainTrackID );
    CLuaCFunctions::AddFunction ( "getTrackLength", GetTrainTrackLength );
    CLuaCFunctions::AddFunction ( "getTrackNode", GetTrainTrackPosition );
}

void CLuaTrainTrackDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "getTrackID", "getTrackID" );
    lua_classfunction ( luaVM, "getLength", "getTrackLength" );
    lua_classfunction ( luaVM, "getNode", "getTrackNode" );
    lua_classfunction ( luaVM, "getNodes", "getTrackNodes" );

    lua_classvariable ( luaVM, "length", NULL, "getTrackLength" );
    lua_classvariable ( luaVM, "trackID", NULL, "getTrackID" );

    lua_registerclass ( luaVM, "TrainTrack", "Element" );
}

int CLuaTrainTrackDefs::GetTrainTrackPosition ( lua_State* luaVM )
{
    unsigned char ucTrackID = 0;
    unsigned int uiTrackNode = 0;
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ucTrackID );
    argStream.ReadNumber ( uiTrackNode );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackPosition ( ucTrackID, uiTrackNode, vecPosition ) )
        {
            lua_pushvector ( luaVM, vecPosition );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTrainTrackDefs::GetTrainTrackLength ( lua_State* luaVM )
{
    float fLength = 0.0f;
    CClientTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackLength ( *pTrainTrack, fLength ) )
        {
            lua_pushnumber ( luaVM, fLength );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTrainTrackDefs::GetTrainTrackNumberOfNodes ( lua_State* luaVM )
{
    unsigned int uiNodes = 0;
    CClientTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackNumberOfNodes ( *pTrainTrack, uiNodes ) )
        {
            lua_pushnumber ( luaVM, uiNodes );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTrainTrackDefs::GetTrainTrackID ( lua_State* luaVM )
{
    unsigned char ucTrack = 0;
    CClientTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackID ( *pTrainTrack, ucTrack ) )
        {
            lua_pushnumber ( luaVM, ucTrack );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
