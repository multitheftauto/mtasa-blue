/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaWaterDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaWaterDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createWater", CreateWater},
        {"testLineAgainstWater", TestLineAgainstWater},
        {"resetWaterColor", ResetWaterColor},
        {"resetWaterLevel", ResetWaterLevel},

        {"setWaterColor", SetWaterColor},
        {"setWaterLevel", SetWaterLevel},
        {"setWaterVertexPosition", SetWaterVertexPosition},
        {"setWaterDrawnLast", SetWaterDrawnLast},

        {"getWaterColor", GetWaterColor},
        {"getWaterLevel", GetWaterLevel},
        {"getWaterVertexPosition", GetWaterVertexPosition},
        {"isWaterDrawnLast", IsWaterDrawnLast},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaWaterDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createWater");
    lua_classfunction(luaVM, "testLineAgainst", "testLineAgainstWater");
    lua_classfunction(luaVM, "isDrawnLast", "isWaterDrawnLast");            // Drawn last property affects all water instances
    lua_classfunction(luaVM, "setDrawnLast", "setWaterDrawnLast");

    // Static variables (not implemented yet!)
    // lua_classvariable ( luaVM, "drawnLast", "setWaterDrawnLast", "isWaterDrawnLast" );

    lua_classfunction(luaVM, "getLevel", "getWaterLevel");
    lua_classfunction(luaVM, "getVertexPosition", "getWaterVertexPosition");
    lua_classfunction(luaVM, "getWaveHeight", "getWaveHeight");
    lua_classfunction(luaVM, "getColor", "getWaterColor");

    lua_classfunction(luaVM, "setWaveHeight", "setWaveHeight");
    lua_classfunction(luaVM, "setColor", "setWaterColor");
    lua_classfunction(luaVM, "setVertexPosition", "setWaterVertexPosition");
    lua_classfunction(luaVM, "setLevel", "setWaterLevel");

    lua_classfunction(luaVM, "resetColor", "resetWaterColor");
    lua_classfunction(luaVM, "resetLevel", "resetWaterLevel");

    lua_classvariable(luaVM, "level", "setWaterLevel", "getWaterLevel");
    lua_classvariable(luaVM, "height", "setWaveHeight", "getWaveHeight");

    lua_registerclass(luaVM, "Water", "Element");
}

int CLuaWaterDefs::CreateWater(lua_State* luaVM)
{
    //  water createWater ( float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3 [, float x4, float y4, float z4 ] [, bool
    //  bShallow = false ] )
    CVector v1;
    CVector v2;
    CVector v3;
    CVector v4;
    bool    bShallow;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(v1);
    argStream.ReadVector3D(v2);
    argStream.ReadVector3D(v3);
    bool bIsQuad = argStream.NextCouldBeNumber(2);            // Check for existence of v4.fZ
    if (bIsQuad)
    {
        argStream.ReadVector3D(v4);
    }
    argStream.ReadBool(bShallow, false);

    if (!argStream.HasErrors())
    {
        CLuaMain*  pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        CResource* pResource = pLuaMain ? pLuaMain->GetResource() : NULL;
        if (pResource)
        {
            CClientWater* pWaterElement;

            if (bIsQuad)
                pWaterElement = CStaticFunctionDefinitions::CreateWater(*pResource, &v1, &v2, &v3, &v4, bShallow);
            else
                pWaterElement = CStaticFunctionDefinitions::CreateWater(*pResource, &v1, &v2, &v3, NULL, bShallow);

            lua_pushelement(luaVM, pWaterElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWaterDefs::TestLineAgainstWater(lua_State* luaVM)
{
    //  bool float float float testLineAgainstWater ( float startX, float startY, float startZ, float endX, float endY, float endZ )
    CVector vecStart;
    CVector vecEnd;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecStart);
    argStream.ReadVector3D(vecEnd);

    if (!argStream.HasErrors())
    {
        CVector vecCollision;
        if (CStaticFunctionDefinitions::TestLineAgainstWater(vecStart, vecEnd, vecCollision))
        {
            lua_pushboolean(luaVM, true);
            lua_pushnumber(luaVM, vecCollision.fX);
            lua_pushnumber(luaVM, vecCollision.fY);
            lua_pushnumber(luaVM, vecCollision.fZ);
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWaterDefs::ResetWaterLevel(lua_State* luaVM)
{
    CStaticFunctionDefinitions::ResetWorldWaterLevel();
    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWaterDefs::ResetWaterColor(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetWaterColor())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWaterDefs::SetWaterColor(lua_State* luaVM)
{
    //  bool setWaterColor ( int red, int green, int blue, [ int alpha = 200 ] )
    float r;
    float g;
    float b;
    float alpha;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(r);
    argStream.ReadNumber(g);
    argStream.ReadNumber(b);
    argStream.ReadNumber(alpha, 200);

    if (!argStream.HasErrors())
    {
        // Set the new water colour
        if (CStaticFunctionDefinitions::SetWaterColor(r, g, b, alpha))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWaterDefs::SetWaterLevel(lua_State* luaVM)
{
    CLuaMain*  pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    CResource* pResource = pLuaMain ? pLuaMain->GetResource() : NULL;
    if (!pResource)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserData())
    {
        // Call type 1
        //  bool setWaterLevel ( [water theWater,] float level )
        CClientWater* pWater;
        float         fLevel;

        argStream.ReadUserData(pWater);
        argStream.ReadNumber(fLevel);

        if (!argStream.HasErrors())
        {
            if (pWater->SetLevel(fLevel, pResource))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else if (argStream.NextCouldBeNumber(1))
    {
        // Call type 2
        //  bool setWaterLevel ( [float x, float y, float z,] float level )
        CVector vecPosition;
        float   fLevel;

        argStream.ReadVector3D(vecPosition);
        argStream.ReadNumber(fLevel);

        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::SetPositionWaterLevel(vecPosition, fLevel, pResource))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
    {
        // Call type 3
        //  bool setWaterLevel ( float level, bool bIncludeWorldNonSeaLevel, bool bIncludeAllWaterElements, bool bIncludeWorldSeaLevel, bool
        //  bIncludeOutsideWorldLevel )
        float fLevel;
        bool  bIncludeWorldNonSeaLevel;
        bool  bIncludeAllWaterElements;
        bool  bIncludeWorldSeaLevel;
        bool  bIncludeOutsideWorldLevel;

        argStream.ReadNumber(fLevel);
        argStream.ReadBool(bIncludeWorldNonSeaLevel, true);
        argStream.ReadBool(bIncludeAllWaterElements, true);
        argStream.ReadBool(bIncludeWorldSeaLevel, true);
        argStream.ReadBool(bIncludeOutsideWorldLevel, false);

        if (!argStream.HasErrors())
        {
            if (bIncludeAllWaterElements)
                CStaticFunctionDefinitions::SetAllElementWaterLevel(fLevel, pResource);
            if (CStaticFunctionDefinitions::SetWorldWaterLevel(fLevel, pResource, bIncludeWorldNonSeaLevel, bIncludeWorldSeaLevel, bIncludeOutsideWorldLevel))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWaterDefs::SetWaterDrawnLast(lua_State* luaVM)
{
    //  bool setWaterDrawnLast ( bool bEnabled )
    bool bEnabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWaterDrawnLast(bEnabled))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWaterDefs::SetWaterVertexPosition(lua_State* luaVM)
{
    //  bool setWaterVertexPosition ( water theWater, int vertexIndex, float x, float y, float z )
    CClientWater* pWater;
    int           iVertexIndex;
    CVector       vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWater);
    argStream.ReadNumber(iVertexIndex);
    argStream.ReadVector3D(vecPosition);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWaterVertexPosition(pWater, iVertexIndex, vecPosition))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWaterDefs::GetWaterColor(lua_State* luaVM)
{
    float fRed, fGreen, fBlue, fAlpha;

    CStaticFunctionDefinitions::GetWaterColor(fRed, fGreen, fBlue, fAlpha);

    lua_pushnumber(luaVM, fRed);
    lua_pushnumber(luaVM, fGreen);
    lua_pushnumber(luaVM, fBlue);
    lua_pushnumber(luaVM, fAlpha);
    return 4;
}

int CLuaWaterDefs::IsWaterDrawnLast(lua_State* luaVM)
{
    //  bool getWaterDrawnLast ()
    bool bEnabled;
    if (CStaticFunctionDefinitions::IsWaterDrawnLast(bEnabled))
    {
        lua_pushboolean(luaVM, bEnabled);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWaterDefs::GetWaterLevel(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    if (!argStream.NextIsUserData())
    {
        // Call type 1
        //  float getWaterLevel ( float posX, float posY, float posZ [ , bool ignoreDistanceToWaterThreshold = false ] )
        CVector vecPosition;
        bool    ignoreDistanceToWaterThreshold;

        argStream.ReadVector3D(vecPosition);
        argStream.ReadBool(ignoreDistanceToWaterThreshold, false);

        if (!argStream.HasErrors())
        {
            float   fWaterLevel;
            CVector vecUnknown;
            if (CStaticFunctionDefinitions::GetWaterLevel(vecPosition, fWaterLevel, ignoreDistanceToWaterThreshold, vecUnknown))
            {
                lua_pushnumber(luaVM, fWaterLevel);
                return 1;
            }
        }
    }
    else
    {
        // Call type 2
        //  float getWaterLevel ( water theWater )
        CClientWater* pWater;

        argStream.ReadUserData(pWater);

        if (!argStream.HasErrors())
        {
            float fLevel;
            if (CStaticFunctionDefinitions::GetWaterLevel(pWater, fLevel))
            {
                lua_pushnumber(luaVM, fLevel);
                return 1;
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWaterDefs::GetWaterVertexPosition(lua_State* luaVM)
{
    //  float float float getWaterVertexPosition ( water theWater, int vertexIndex )
    CClientWater* pWater;
    int           iVertexIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWater);
    argStream.ReadNumber(iVertexIndex);

    if (!argStream.HasErrors())
    {
        CVector vecPosition;
        if (CStaticFunctionDefinitions::GetWaterVertexPosition(pWater, iVertexIndex, vecPosition))
        {
            lua_pushnumber(luaVM, vecPosition.fX);
            lua_pushnumber(luaVM, vecPosition.fY);
            lua_pushnumber(luaVM, vecPosition.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
