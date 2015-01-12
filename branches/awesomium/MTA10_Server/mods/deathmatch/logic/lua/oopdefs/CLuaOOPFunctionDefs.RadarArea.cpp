/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/logic/oopdefs/CLuaOOPFunctionDefs.RadarArea.cpp
*  PURPOSE:     Lua OOP specific function definitions class
*  DEVELOPERS:  Qais "qaisjp" Patankar <qaisjp@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaOOPDefs::GetRadarAreaSize ( lua_State* luaVM )
{
    //  float, float getRadarAreaSize ( radararea theRadararea )
    CRadarArea* pRadarArea;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRadarArea );

    if ( !argStream.HasErrors () )
    {
        CVector2D vecSize;
        if ( CStaticFunctionDefinitions::GetRadarAreaSize ( pRadarArea, vecSize ) )
        {
            lua_pushvector ( luaVM, vecSize );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );


    lua_pushboolean ( luaVM, false );
    return 1;
}