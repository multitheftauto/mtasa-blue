/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaFireDefs.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"
#include "CLuaPathFindDefs.h"
#include "d:\mtasa-blue_cr95\Client\game_sa\CPathFindSA.h"

void CLuaPathFindDefs::LoadFunctions(void)
{
    CLuaCFunctions::AddFunction("findClosestNode", CLuaPathFindDefs::FindNodeClosestToCoords);
}
void CLuaPathFindDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    //lua_classfunction(luaVM, "create", "createWater");

    //lua_classvariable(luaVM, "level", "setWaterLevel", "getWaterLevel");

    lua_registerclass(luaVM, "PathFind", "Element");
}

int CLuaPathFindDefs::FindNodeClosestToCoords(lua_State* luaVM)
{
    // bool findNodeClosestToCoords ( float x, float y, float z [, float size = 1.8 ] )
    
    CVector vecPosition;
    int     iNodeNumber;
    int     iType;
    float   fDistance;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(iNodeNumber, 0);
    argStream.ReadNumber(iType, 0);
    argStream.ReadNumber(fDistance, 9999.0f);

    if (!argStream.HasErrors())
    {
        CPathFind* asdf = g_pGame->GetPathFind();
        CNodeAddress* found;
        auto as = asdf->FindNthNodeClosestToCoors(&vecPosition, iNodeNumber, iType, found, fDistance);
        if ( true )
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
