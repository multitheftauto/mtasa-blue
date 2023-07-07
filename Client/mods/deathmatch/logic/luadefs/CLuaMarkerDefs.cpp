/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaMarkerDefs.cpp
 *  PURPOSE:     Lua marker definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"

void CLuaMarkerDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createMarker", CreateMarker},

        {"getMarkerCount", GetMarkerCount},
        {"getMarkerType", GetMarkerType},
        {"getMarkerSize", GetMarkerSize},
        {"getMarkerColor", GetMarkerColor},
        {"getMarkerTarget", GetMarkerTarget},
        {"getMarkerIcon", GetMarkerIcon},

        {"setMarkerType", SetMarkerType},
        {"setMarkerSize", SetMarkerSize},
        {"setMarkerColor", SetMarkerColor},
        {"setMarkerTarget", SetMarkerTarget},
        {"setMarkerIcon", SetMarkerIcon},

        {"setCoronaReflectionEnabled", ArgumentParser<SetCoronaReflectionEnabled>},
        {"isCoronaReflectionEnabled", ArgumentParser<IsCoronaReflectionEnabled>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaMarkerDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createMarker");
    lua_classfunction(luaVM, "getCount", "getMarkerCount");

    lua_classfunction(luaVM, "getMarkerType", "getMarkerType");
    lua_classfunction(luaVM, "getIcon", "getMarkerIcon");
    lua_classfunction(luaVM, "getSize", "getMarkerSize");
    lua_classfunction(luaVM, "getTarget", OOP_GetMarkerTarget);
    lua_classfunction(luaVM, "getColor", "getMarkerColor");
    lua_classfunction(luaVM, "isCoronaReflectionEnabled", "isCoronaReflectionEnabled");

    lua_classfunction(luaVM, "setType", "setMarkerType");
    lua_classfunction(luaVM, "setIcon", "setMarkerIcon");
    lua_classfunction(luaVM, "setSize", "setMarkerSize");
    lua_classfunction(luaVM, "setTarget", "setMarkerTarget");
    lua_classfunction(luaVM, "setColor", "setMarkerColor");
    lua_classfunction(luaVM, "setCoronaReflectionEnabled", "setCoronaReflectionEnabled");

    lua_classvariable(luaVM, "markerType", "setMarkerType", "getMarkerType");
    lua_classvariable(luaVM, "icon", "setMarkerIcon", "getMarkerIcon");
    lua_classvariable(luaVM, "size", "setMarkerSize", "getMarkerSize");

    lua_classvariable(luaVM, "target", SetMarkerTarget, OOP_GetMarkerTarget);

    lua_registerclass(luaVM, "Marker", "Element");
}

int CLuaMarkerDefs::CreateMarker(lua_State* luaVM)
{
    CVector          vecPosition;
    float            fSize = 4.0f;
    SColorRGBA       color(0, 0, 255, 255);
    SString          strType = "default";
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadString(strType, "default");
    argStream.ReadNumber(fSize, 4.0f);
    argStream.ReadNumber(color.R, 0);
    argStream.ReadNumber(color.G, 0);
    argStream.ReadNumber(color.B, 255);
    argStream.ReadNumber(color.A, 255);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            {
                // Create it
                CClientMarker* pMarker = CStaticFunctionDefinitions::CreateMarker(*pResource, vecPosition, strType, fSize, color);
                if (pMarker)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add((CClientEntity*)pMarker);
                    }

                    lua_pushelement(luaVM, pMarker);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaMarkerDefs::GetMarkerCount(lua_State* luaVM)
{
    unsigned int uiCount = m_pMarkerManager->Count();
    lua_pushnumber(luaVM, static_cast<lua_Number>(uiCount));
    return 1;
}

int CLuaMarkerDefs::GetMarkerType(lua_State* luaVM)
{
    // Verify the argument
    CClientMarker*   pMarker = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        SString strMarkerType;
        if (CClientMarker::TypeToString(pMarker->GetMarkerType(), strMarkerType))
        {
            lua_pushstring(luaVM, strMarkerType);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaMarkerDefs::GetMarkerSize(lua_State* luaVM)
{
    // Verify the argument
    CClientMarker*   pMarker = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        float fSize = pMarker->GetSize();
        lua_pushnumber(luaVM, static_cast<lua_Number>(fSize));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaMarkerDefs::GetMarkerColor(lua_State* luaVM)
{
    // Verify the argument
    CClientMarker*   pMarker = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        SColor color = pMarker->GetColor();
        lua_pushnumber(luaVM, static_cast<lua_Number>(color.R));
        lua_pushnumber(luaVM, static_cast<lua_Number>(color.G));
        lua_pushnumber(luaVM, static_cast<lua_Number>(color.B));
        lua_pushnumber(luaVM, static_cast<lua_Number>(color.A));
        return 4;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaMarkerDefs::GetMarkerTarget(lua_State* luaVM)
{
    // Verify the argument
    CClientMarker*   pMarker = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        CVector vecTarget;
        if (CStaticFunctionDefinitions::GetMarkerTarget(*pMarker, vecTarget))
        {
            lua_pushnumber(luaVM, vecTarget.fX);
            lua_pushnumber(luaVM, vecTarget.fY);
            lua_pushnumber(luaVM, vecTarget.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaMarkerDefs::OOP_GetMarkerTarget(lua_State* luaVM)
{
    CClientMarker* pMarker = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        CVector vecPosition;
        if (CStaticFunctionDefinitions::GetMarkerTarget(*pMarker, vecPosition))
        {
            lua_pushvector(luaVM, vecPosition);
        }
        else
            lua_pushboolean(luaVM, false);

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaMarkerDefs::GetMarkerIcon(lua_State* luaVM)
{
    // Verify the argument
    CClientMarker*   pMarker = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        CClientCheckpoint* pCheckpoint = pMarker->GetCheckpoint();
        if (pCheckpoint)
        {
            SString strMarkerIcon;
            CClientCheckpoint::IconToString(pCheckpoint->GetIcon(), strMarkerIcon);
            lua_pushstring(luaVM, strMarkerIcon);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaMarkerDefs::SetMarkerType(lua_State* luaVM)
{
    // Verify the argument
    SString          strType = "";
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadString(strType);

    if (!argStream.HasErrors())
    {
        bool bSuccess = CStaticFunctionDefinitions::SetMarkerType(*pEntity, strType);
        lua_pushboolean(luaVM, bSuccess);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaMarkerDefs::SetMarkerSize(lua_State* luaVM)
{
    // Verify the argument
    float            fSize = 0.0f;
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(fSize);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetMarkerSize(*pEntity, fSize))
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

int CLuaMarkerDefs::SetMarkerColor(lua_State* luaVM)
{
    // Verify the argument
    double           dRed = 0;
    double           dGreen = 0;
    double           dBlue = 0;
    double           dAlpha = 255;
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(dRed);
    argStream.ReadNumber(dGreen);
    argStream.ReadNumber(dBlue);
    argStream.ReadNumber(dAlpha, 255);

    if (!argStream.HasErrors())
    {
        // Check the ranges
        if (dRed >= 0 && dRed <= 255 && dGreen >= 0 && dGreen <= 255 && dBlue >= 0 && dBlue <= 255 && dAlpha >= 0 && dAlpha <= 255)
        {
            SColor color;
            color.R = static_cast<unsigned char>(dRed);
            color.G = static_cast<unsigned char>(dGreen);
            color.B = static_cast<unsigned char>(dBlue);
            color.A = static_cast<unsigned char>(dAlpha);

            // Set the new color
            if (CStaticFunctionDefinitions::SetMarkerColor(*pEntity, color))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaMarkerDefs::SetMarkerTarget(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CVector          vecTarget;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadVector3D(vecTarget);

    if (!argStream.HasErrors())
    {
        // Do it
        if (CStaticFunctionDefinitions::SetMarkerTarget(*pEntity, &vecTarget))
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

int CLuaMarkerDefs::SetMarkerIcon(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    SString          strIcon = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadString(strIcon);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetMarkerIcon(*pEntity, strIcon))
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

bool CLuaMarkerDefs::SetCoronaReflectionEnabled(CClientMarker* pMarker, bool bEnabled)
{
    CClientCorona* pCorona = pMarker->GetCorona();
    if (!pCorona)
        return false;

    pCorona->SetReflectionEnabled(bEnabled);
    return true;
}

bool CLuaMarkerDefs::IsCoronaReflectionEnabled(CClientMarker* pMarker)
{
    CClientCorona* pCorona = pMarker->GetCorona();
    if (!pCorona)
        return false;

    return pCorona->IsReflectionEnabled();
}
