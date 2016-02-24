/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaAccountDefs.cpp
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaAccountDefs::LoadFunctions ()
{
    // Shape create funcs
    CLuaCFunctions::AddFunction ( "createColCircle", CreateColCircle );
    CLuaCFunctions::AddFunction ( "createColCuboid", CreateColCuboid );
    CLuaCFunctions::AddFunction ( "createColSphere", CreateColSphere );
    CLuaCFunctions::AddFunction ( "createColRectangle", CreateColRectangle );
    CLuaCFunctions::AddFunction ( "createColPolygon", CreateColPolygon );
    CLuaCFunctions::AddFunction ( "createColTube", CreateColTube );
}

void CLuaAccountDefs::AddClass ( lua_State* luaVM )
{

}