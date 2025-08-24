/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaRadarAreaDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaRadarAreaDefs.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"

void CLuaRadarAreaDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Radar area create/destroy funcs
        {"createRadarArea", CreateRadarArea},

        // Radar area get funcs
        {"getRadarAreaSize", GetRadarAreaSize},
        {"getRadarAreaColor", GetRadarAreaColor},
        {"isRadarAreaFlashing", IsRadarAreaFlashing},
        {"isInsideRadarArea", IsInsideRadarArea},

        // Radar area set funcs
        {"setRadarAreaSize", SetRadarAreaSize},
        {"setRadarAreaColor", SetRadarAreaColor},
        {"setRadarAreaFlashing", SetRadarAreaFlashing},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaRadarAreaDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createRadarArea");
    lua_classfunction(luaVM, "isInside", "isInsideRadarArea");

    lua_classfunction(luaVM, "isFlashing", "isRadarAreaFlashing");
    lua_classfunction(luaVM, "getSize", "getRadarAreaSize");
    lua_classfunction(luaVM, "getColor", "getRadarAreaColor");

    lua_classfunction(luaVM, "setSize", "setRadarAreaSize");
    lua_classfunction(luaVM, "setFlashing", "setRadarAreaFlashing");
    lua_classfunction(luaVM, "setColor", "setRadarAreaColor");

    lua_classvariable(luaVM, "flashing", "setRadarAreaFlashing", "isRadarAreaFlashing");
    lua_classvariable(luaVM, "size", "setRadarAreaSize", "getRadarAreaSize", SetRadarAreaSize, OOP_GetRadarAreaSize);

    lua_registerclass(luaVM, "RadarArea", "Element");
}

int CLuaRadarAreaDefs::CreateRadarArea(lua_State* luaVM)
{
    //  radararea createRadarArea ( float startPosX, float startPosY, float sizeX, float sizeY, [ int r = 255, int g = 0, int b = 0, int a = 255, element
    //  visibleTo = getRootElement() ] )
    CVector2D vecPosition;
    CVector2D vecSize;
    float     dRed;
    float     dGreen;
    float     dBlue;
    float     dAlpha;
    CElement* pVisibleTo;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadVector2D(vecSize);
    argStream.ReadNumber(dRed, 255);
    argStream.ReadNumber(dGreen, 0);
    argStream.ReadNumber(dBlue, 0);
    argStream.ReadNumber(dAlpha, 255);
    argStream.ReadUserData(pVisibleTo, m_pRootElement);

    if (!argStream.HasErrors())
    {
        CLuaMain*  pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        CResource* pResource = pLuaMain ? pLuaMain->GetResource() : NULL;
        if (pResource)
        {
            SColorRGBA color(dRed, dGreen, dBlue, dAlpha);

            // Create it
            CRadarArea* pRadarArea = CStaticFunctionDefinitions::CreateRadarArea(pResource, vecPosition, vecSize, color, pVisibleTo);
            if (pRadarArea)
            {
                CElementGroup* pGroup = pResource->GetElementGroup();
                if (pGroup)
                {
                    pGroup->Add(pRadarArea);
                }
                lua_pushelement(luaVM, pRadarArea);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaRadarAreaDefs::GetRadarAreaSize(lua_State* luaVM)
{
    //  float, float getRadarAreaSize ( radararea theRadararea )
    CRadarArea* pRadarArea;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pRadarArea);

    if (!argStream.HasErrors())
    {
        CVector2D vecSize;
        if (CStaticFunctionDefinitions::GetRadarAreaSize(pRadarArea, vecSize))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(vecSize.fX));
            lua_pushnumber(luaVM, static_cast<lua_Number>(vecSize.fY));
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaRadarAreaDefs::OOP_GetRadarAreaSize(lua_State* luaVM)
{
    //  vector2 getRadarAreaSize ( radararea theRadararea )
    CRadarArea* pRadarArea;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pRadarArea);

    if (!argStream.HasErrors())
    {
        CVector2D vecSize;
        if (CStaticFunctionDefinitions::GetRadarAreaSize(pRadarArea, vecSize))
        {
            lua_pushvector(luaVM, vecSize);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaRadarAreaDefs::GetRadarAreaColor(lua_State* luaVM)
{
    //  int, int, int, int getRadarAreaColor ( radararea theRadararea )
    CRadarArea* pRadarArea;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pRadarArea);

    if (!argStream.HasErrors())
    {
        SColor color;
        if (CStaticFunctionDefinitions::GetRadarAreaColor(pRadarArea, color))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(color.R));
            lua_pushnumber(luaVM, static_cast<lua_Number>(color.G));
            lua_pushnumber(luaVM, static_cast<lua_Number>(color.B));
            lua_pushnumber(luaVM, static_cast<lua_Number>(color.A));
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaRadarAreaDefs::IsRadarAreaFlashing(lua_State* luaVM)
{
    //  bool isRadarAreaFlashing ( radararea theRadararea )
    CRadarArea* pRadarArea;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pRadarArea);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, CStaticFunctionDefinitions::IsRadarAreaFlashing(pRadarArea));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaRadarAreaDefs::IsInsideRadarArea(lua_State* luaVM)
{
    //  bool isInsideRadarArea ( radararea theArea, float posX, float posY )
    CRadarArea* pRadarArea;
    CVector2D   vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pRadarArea);
    argStream.ReadVector2D(vecPosition);

    if (!argStream.HasErrors())
    {
        bool bInside = false;
        if (CStaticFunctionDefinitions::IsInsideRadarArea(pRadarArea, vecPosition, bInside))
        {
            lua_pushboolean(luaVM, bInside);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaRadarAreaDefs::SetRadarAreaSize(lua_State* luaVM)
{
    //  bool setRadarAreaSize ( radararea theRadararea, float x, float y )
    CElement* pElement;
    CVector2D vecSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadVector2D(vecSize);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetRadarAreaSize(pElement, vecSize))
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

int CLuaRadarAreaDefs::SetRadarAreaColor(lua_State* luaVM)
{
    //  bool setRadarAreaColor ( radararea theRadarArea, int r, int g, int b, int a )
    CElement* pElement;
    float     dRed;
    float     dGreen;
    float     dBlue;
    float     dAlpha;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(dRed);
    argStream.ReadNumber(dGreen);
    argStream.ReadNumber(dBlue);
    argStream.ReadNumber(dAlpha, 255);

    if (!argStream.HasErrors())
    {
        SColorRGBA color(dRed, dGreen, dBlue, dAlpha);
        if (CStaticFunctionDefinitions::SetRadarAreaColor(pElement, color))
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

int CLuaRadarAreaDefs::SetRadarAreaFlashing(lua_State* luaVM)
{
    //  bool setRadarAreaFlashing ( radararea theRadarArea, bool flash )
    CElement* pElement;
    bool      bFlash;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bFlash);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetRadarAreaFlashing(pElement, lua_toboolean(luaVM, 2) ? true : false))
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
