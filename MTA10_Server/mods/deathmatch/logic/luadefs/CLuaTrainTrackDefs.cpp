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
    CLuaCFunctions::AddFunction ( "createTrack", CreateTrainTrack );
    CLuaCFunctions::AddFunction ( "getDefaultTrack", GetDefaultTrack );

    CLuaCFunctions::AddFunction ( "getTrackID", GetTrainTrackID );
    CLuaCFunctions::AddFunction ( "getTrackLength", GetTrainTrackLength );
    CLuaCFunctions::AddFunction ( "getTrackNode", GetTrainTrackPosition );
    CLuaCFunctions::AddFunction ( "getTrackNodes", GetTrainTrackNumberOfNodes );

    CLuaCFunctions::AddFunction ( "setTrackLength", SetTrainTrackLength );
    CLuaCFunctions::AddFunction ( "setTrackNode", SetTrainTrackPosition );
    CLuaCFunctions::AddFunction ( "setTrackNodes", SetTrainTrackNumberOfNodes );
}


void CLuaTrainTrackDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createTrack" );
    lua_classfunction ( luaVM, "getDefault", "getDefaultTrack" );

    lua_classfunction ( luaVM, "getTrackID", "getTrackID" );
    lua_classfunction ( luaVM, "getLength", "getTrackLength" );
    lua_classfunction ( luaVM, "getNode", "getTrackNode" );
    lua_classfunction ( luaVM, "getNodes", "getTrackNodes" );

    lua_classfunction ( luaVM, "setLength", "setTrackLength" );
    lua_classfunction ( luaVM, "setNode", "setTrackNode" );
    lua_classfunction ( luaVM, "setNodes", "setTrackNodes" );

    lua_classvariable ( luaVM, "length", "setTrackLength", "getTrackLength" );
    lua_classvariable ( luaVM, "trackID", NULL, "getTrackID" );

    lua_registerclass ( luaVM, "TrainTrack", "Element" );
}

int CLuaTrainTrackDefs::SetTrainTrackPosition ( lua_State* luaVM )
{
    unsigned int uiTrackNode = 0;
    CVector vecPosition;
    CTrainTrack* pTrack;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrack );
    argStream.ReadNumber ( uiTrackNode );
    argStream.ReadVector3D ( vecPosition );
    
    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetTrainTrackPosition ( pTrack, uiTrackNode, vecPosition ) )
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
    unsigned int uiTrackNode = 0;
    CVector vecPosition;
    CTrainTrack* pTrack;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTrack );
    argStream.ReadNumber ( uiTrackNode );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::GetTrainTrackPosition ( pTrack, uiTrackNode, vecPosition ) )
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

// DEBUG: Gives ID of track
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

// DEBUG: Gets track by ID
int CLuaTrainTrackDefs::GetDefaultTrack ( lua_State* luaVM )
{
    uchar ucTrack = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ucTrack );

    //if (ucTrack > 3 )
    //    argStream.SetCustomError ( "Invalid track number range (0-3)" );

    if ( !argStream.HasErrors () )
    {
        CTrainTrack* pTrack = CStaticFunctionDefinitions::GetDefaultTrack ( ucTrack );
        if ( pTrack != nullptr )
        {
            lua_pushuserdata ( luaVM, pTrack );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
