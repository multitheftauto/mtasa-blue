/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaTrainTrackDefs.cpp
*  PURPOSE:     Lua train track definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaTrainTrackDefs::LoadFunctions ()
{
    CLuaCFunctions::AddFunction ( "setTrainTrackPosition", SetTrainTrackPosition );
    CLuaCFunctions::AddFunction ( "getTrainTrackNumberOfNodes", GetTrainTrackNumberOfNodes );
    CLuaCFunctions::AddFunction ( "getTrainTrackID", GetTrainTrackID );
    CLuaCFunctions::AddFunction ( "getTrainTrackLength", GetTrainTrackLength );

    CLuaCFunctions::AddFunction ( "createTrainTrack", CreateTrainTrack );
    CLuaCFunctions::AddFunction ( "getTrainTrackPosition", GetTrainTrackPosition );
    CLuaCFunctions::AddFunction ( "setTrainTrackLength", SetTrainTrackLength );
    CLuaCFunctions::AddFunction ( "setTrainTrackNumberOfNodes", SetTrainTrackNumberOfNodes );
}


void CLuaTrainTrackDefs::AddClass ( lua_State* luaVM )
{
}


int CLuaTrainTrackDefs::SetTrainTrackPosition ( lua_State* luaVM )
{
    unsigned char ucTrackID = 0;
    unsigned int uiTrackNode = 0;
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ucTrackID );
    argStream.ReadNumber ( uiTrackNode );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainTrackPosition ( ucTrackID, uiTrackNode, vecPosition ) )
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
            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            lua_pushnumber ( luaVM, vecPosition.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTrainTrackDefs::CreateTrainTrack ( lua_State* luaVM )
{
    unsigned int uiTrackNodes = 0;
    float fLength = 0.0f;
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiTrackNodes );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource * pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                CTrainTrack * pTrainTrack = CStaticFunctionDefinitions::CreateTrainTrack ( pResource, uiTrackNodes );
                if ( pTrainTrack != NULL )
                {
                    lua_pushelement ( luaVM, pTrainTrack );
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


int CLuaTrainTrackDefs::SetTrainTrackLength ( lua_State* luaVM )
{
    float fLength = 0.0f;
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );
    argStream.ReadNumber ( fLength );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainTrackLength ( pTrainTrack, fLength ) )
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

int CLuaTrainTrackDefs::GetTrainTrackLength ( lua_State* luaVM )
{
    float fLength = 0.0f;
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackLength ( pTrainTrack, fLength ) )
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

int CLuaTrainTrackDefs::SetTrainTrackNumberOfNodes ( lua_State* luaVM )
{
    unsigned int uiNodes = 0;
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );
    argStream.ReadNumber ( uiNodes );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainTrackNumberOfNodes ( pTrainTrack, uiNodes ) )
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

int CLuaTrainTrackDefs::GetTrainTrackNumberOfNodes ( lua_State* luaVM )
{
    unsigned int uiNodes = 0;
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackNumberOfNodes ( pTrainTrack, uiNodes ) )
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
    CTrainTrack * pTrainTrack = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrainTrack );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackID ( pTrainTrack, ucTrack ) )
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
