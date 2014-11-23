/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/oopdefs/CLuaOOPFunctionDefs.Marker.cpp
*  PURPOSE:     Lua OOP specific function definitions class
*  DEVELOPERS:  qaisjp <qaisjp@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaOOPDefs::GetMarkerTarget ( lua_State* luaVM )
{
    CMarker* pMarker = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        CVector vecPosition;
        if ( CStaticFunctionDefinitions::GetMarkerTarget ( pMarker, vecPosition ) )
        {
            lua_pushvector ( luaVM, vecPosition );
        }
        else
            lua_pushboolean ( luaVM, false );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaOOPDefs::IsElementWithinMarker ( lua_State* luaVM )
{
    //  bool isWithinMarker ( marker theMarker, element theElement )
    CElement* pElement; CMarker* pMarker;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );
    argStream.ReadUserData ( pElement );

    if ( !argStream.HasErrors () )
    {
        bool bWithin;
        if ( CStaticFunctionDefinitions::IsElementWithinMarker ( pElement, pMarker, bWithin ) )
        {
            lua_pushboolean ( luaVM, bWithin );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}