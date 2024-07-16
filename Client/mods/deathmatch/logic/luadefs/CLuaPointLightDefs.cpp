/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPointLightDefs.cpp
 *  PURPOSE:     Lua point light definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaPointLightDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]
    {
        {"createLight", ArgumentParser<CreateLight>},

        {"getLightType", GetLightType},
        {"getLightRadius", GetLightRadius},
        {"getLightColor", GetLightColor},
        {"getLightDirection", GetLightDirection},

        {"setLightRadius", SetLightRadius},
        {"setLightColor", SetLightColor},
        {"setLightDirection", SetLightDirection},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaPointLightDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createLight");

    lua_classfunction(luaVM, "getType", "getLightType");
    lua_classfunction(luaVM, "getRadius", "getLightRadius");
    lua_classfunction(luaVM, "getColor", "getLightColor");
    lua_classfunction(luaVM, "getDirection", "getLightDirection");

    lua_classfunction(luaVM, "setRadius", "setLightRadius");
    lua_classfunction(luaVM, "setColor", "setLightColor");
    lua_classfunction(luaVM, "setDirection", "setLightDirection");

    lua_classvariable(luaVM, "type", nullptr, "getLightType");
    lua_classvariable(luaVM, "radius", "setLightRadius", "getLightRadius");
    lua_classvariable(luaVM, "direction", "setLightDirection", "getLightDirection");

    lua_registerclass(luaVM, "Light", "Element");
}

std::variant<bool, CClientPointLights*> CLuaPointLightDefs::CreateLight(lua_State* luaVM, int mode, CVector pos, std::optional<float> radius, std::optional<SColor> color, std::optional<CVector> direction) noexcept
{
    //  light createLight ( int lightType, float posX, float posY, float posX, [ float radius = 3, int r = 255, int g = 0, int b = 0, float dirX = 0, float dirY
    //  = 0, float dirZ = 0 ] )

    if(!radius.has_value())
        radius = 3.0f;
    if(!color.has_value())
        color = SColorRGBA(255, 0, 0, 255);
    if(!direction.has_value())
        direction = CVector();
    
    CResource* resource = &lua_getownerresource(luaVM);
    
    CClientPointLights* light = CStaticFunctionDefinitions::CreateLight(*resource, mode, pos, radius.value(), color.value(), direction.value());
    if (!light)
        return false;
        
    CElementGroup* group = resource->GetElementGroup();
    if (group)
        group->Add(light);

    return light;
}

int CLuaPointLightDefs::GetLightType(lua_State* luaVM)
{
    //  int getLightType ( light theLight )
    CClientPointLights* pLight;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pLight);

    if (!argStream.HasErrors())
    {
        int iMode;
        if (CStaticFunctionDefinitions::GetLightType(pLight, iMode))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(iMode));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, SString("Bad argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)), *argStream.GetErrorMessage()));

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPointLightDefs::GetLightRadius(lua_State* luaVM)
{
    //  float getLightRadius ( light theLight )
    CClientPointLights* pLight;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pLight);

    if (!argStream.HasErrors())
    {
        float fRadius;
        if (CStaticFunctionDefinitions::GetLightRadius(pLight, fRadius))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(fRadius));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, SString("Bad argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)), *argStream.GetErrorMessage()));

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPointLightDefs::GetLightColor(lua_State* luaVM)
{
    //  int, int, int getLightColor ( light theLight )
    CClientPointLights* pLight;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pLight);

    if (!argStream.HasErrors())
    {
        SColor color;
        if (CStaticFunctionDefinitions::GetLightColor(pLight, color))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(color.R));
            lua_pushnumber(luaVM, static_cast<lua_Number>(color.G));
            lua_pushnumber(luaVM, static_cast<lua_Number>(color.B));
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, SString("Bad argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)), *argStream.GetErrorMessage()));

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPointLightDefs::GetLightDirection(lua_State* luaVM)
{
    //  float, float, float getLightDirection ( light theLight )
    CClientPointLights* pLight;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pLight);

    if (!argStream.HasErrors())
    {
        CVector vecDirection;
        if (CStaticFunctionDefinitions::GetLightDirection(pLight, vecDirection))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(vecDirection.fX));
            lua_pushnumber(luaVM, static_cast<lua_Number>(vecDirection.fY));
            lua_pushnumber(luaVM, static_cast<lua_Number>(vecDirection.fZ));
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, SString("Bad argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)), *argStream.GetErrorMessage()));

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPointLightDefs::SetLightRadius(lua_State* luaVM)
{
    //  bool setLightRadius ( Light theLight, float radius )
    CClientPointLights* pLight;
    float               fRadius;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pLight);
    argStream.ReadNumber(fRadius);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetLightRadius(pLight, fRadius))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, SString("Bad argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)), *argStream.GetErrorMessage()));

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPointLightDefs::SetLightColor(lua_State* luaVM)
{
    //  bool setLightColor ( light theLight, float r, float g, float b )
    CClientPointLights* pLight;
    float               fRed;
    float               fGreen;
    float               fBlue;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pLight);
    argStream.ReadNumber(fRed);
    argStream.ReadNumber(fGreen);
    argStream.ReadNumber(fBlue);

    if (!argStream.HasErrors())
    {
        SColorRGBA color(fRed, fGreen, fBlue, 0);
        if (CStaticFunctionDefinitions::SetLightColor(pLight, color))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, SString("Bad argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)), *argStream.GetErrorMessage()));

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPointLightDefs::SetLightDirection(lua_State* luaVM)
{
    //  bool setLightDirection ( light theLight, float x, float y, float z )
    CClientPointLights* pLight;
    CVector             vecDirection;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pLight);
    argStream.ReadVector3D(vecDirection);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetLightDirection(pLight, vecDirection))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, SString("Bad argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)), *argStream.GetErrorMessage()));

    lua_pushboolean(luaVM, false);
    return 1;
}
