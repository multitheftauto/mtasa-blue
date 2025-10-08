/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaObjectDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaObjectDefs.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"

void CLuaObjectDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Object create/destroy funcs
        {"createObject", CreateObject},
        {"respawnObject", ArgumentParser<RespawnObject>},

        // Object get funcs
        {"getObjectRotation", GetObjectRotation},
        {"getObjectScale", GetObjectScale},
        {"isObjectBreakable", ArgumentParser<IsObjectBreakable>},
        {"isObjectMoving", ArgumentParser<IsObjectMoving>},
        {"isObjectRespawnable", ArgumentParser<IsObjectRespawnable>},

        // Object set funcs
        {"setObjectRotation", SetObjectRotation},
        {"setObjectScale", SetObjectScale},
        {"setObjectBreakable", ArgumentParser<SetObjectBreakable>},
        {"moveObject", MoveObject},
        {"stopObject", StopObject},
        {"breakObject", ArgumentParser<BreakObject>},
        {"toggleObjectRespawn", ArgumentParser<ToggleObjectRespawn>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaObjectDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createObject");
    lua_classfunction(luaVM, "move", "moveObject");
    lua_classfunction(luaVM, "stop", "stopObject");
    lua_classfunction(luaVM, "break", "breakObject");
    lua_classfunction(luaVM, "respawn", "respawnObject");

    lua_classfunction(luaVM, "getScale", "getObjectScale");
    lua_classfunction(luaVM, "setScale", "setObjectScale");
    lua_classfunction(luaVM, "isBreakable", "isObjectBreakable");
    lua_classfunction(luaVM, "setBreakable", "setObjectBreakable");
    lua_classfunction(luaVM, "isMoving", "isObjectMoving");
    lua_classfunction(luaVM, "toggleRespawn", "toggleObjectRespawn");

    lua_classvariable(luaVM, "scale", "setObjectScale", "getObjectScale");
    lua_classvariable(luaVM, "breakable", "setObjectBreakable", "isObjectBreakable");
    lua_classvariable(luaVM, "moving", nullptr, "isObjectMoving");
    lua_classvariable(luaVM, "isRespawnable", nullptr, "isObjectRespawnable");

    lua_registerclass(luaVM, "Object", "Element");
}

int CLuaObjectDefs::CreateObject(lua_State* luaVM)
{
    //  object createObject ( int modelid, float x, float y, float z, [float rx, float ry, float rz, bool lowLOD] )
    ushort  usModelID;
    CVector vecPosition;
    CVector vecRotation;
    bool    bIsLowLod;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usModelID);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecRotation, CVector());
    argStream.ReadBool(bIsLowLod, false);

    if (!argStream.HasErrors())
    {
        if (CObjectManager::IsValidModel(usModelID))
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (pLuaMain)
            {
                CResource* pResource = pLuaMain->GetResource();
                if (pResource)
                {
                    CObject* pObject = CStaticFunctionDefinitions::CreateObject(pResource, usModelID, vecPosition, vecRotation, bIsLowLod);
                    if (pObject)
                    {
                        CElementGroup* pGroup = pResource->GetElementGroup();
                        if (pGroup)
                        {
                            pGroup->Add(pObject);
                        }

                        lua_pushelement(luaVM, pObject);
                        return 1;
                    }
                }
            }
        }
        else
            argStream.SetCustomError("Invalid model id");
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaObjectDefs::GetObjectRotation(lua_State* luaVM)
{
    CObject* pObject;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pObject);

    if (!argStream.HasErrors())
    {
        CVector vecRotation;
        if (CStaticFunctionDefinitions::GetObjectRotation(pObject, vecRotation))
        {
            lua_pushnumber(luaVM, vecRotation.fX);
            lua_pushnumber(luaVM, vecRotation.fY);
            lua_pushnumber(luaVM, vecRotation.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaObjectDefs::GetObjectScale(lua_State* luaVM)
{
    CObject* pObject;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pObject);

    if (!argStream.HasErrors())
    {
        const CVector& vecScale = pObject->GetScale();
        lua_pushnumber(luaVM, vecScale.fX);
        lua_pushnumber(luaVM, vecScale.fY);
        lua_pushnumber(luaVM, vecScale.fZ);
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaObjectDefs::IsObjectBreakable(CObject* const pObject)
{
    return pObject->IsBreakable();
}

int CLuaObjectDefs::SetObjectRotation(lua_State* luaVM)
{
    CElement* pElement;
    CVector   vecRotation;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadVector3D(vecRotation);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetObjectRotation(pElement, vecRotation))
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

int CLuaObjectDefs::SetObjectScale(lua_State* luaVM)
{
    CObject* pObject;
    CVector  vecScale;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pObject);

    // Caz - This function looks weird but it works
    // the function is designed to support the following syntaxes
    // setObjectScale ( obj, 2 ) -- all other components are set to 2
    // setObjectScale ( obj, 2, 1, 5 ) -- custom scaling on 3 axis

    if (argStream.NextIsVector3D())
    {
        argStream.ReadVector3D(vecScale);
    }
    else
    {
        // Caz - Here is what I am talking about.
        argStream.ReadNumber(vecScale.fX);
        argStream.ReadNumber(vecScale.fY, vecScale.fX);
        argStream.ReadNumber(vecScale.fZ, vecScale.fX);
    }

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetObjectScale(pObject, vecScale))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaObjectDefs::IsObjectMoving(CObject* const pObject)
{
    return pObject->IsMoving();
}

int CLuaObjectDefs::MoveObject(lua_State* luaVM)
{
    //  bool moveObject ( object theObject, int time,
    //      float targetx, float targety, float targetz,
    //      [ float moverx, float movery, float moverz,
    //      string strEasingType, float fEasingPeriod, float fEasingAmplitude, float fEasingOvershoot ] )
    CElement*           pElement;
    int                 iTime;
    CVector             vecTargetPosition;
    CVector             vecTargetRotation;
    CEasingCurve::eType easingType;
    float               fEasingPeriod;
    float               fEasingAmplitude;
    float               fEasingOvershoot;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(iTime);
    argStream.ReadVector3D(vecTargetPosition);
    argStream.ReadVector3D(vecTargetRotation, CVector());
    argStream.ReadEnumString(easingType, CEasingCurve::Linear);
    argStream.ReadNumber(fEasingPeriod, 0.3f);
    argStream.ReadNumber(fEasingAmplitude, 1.0f);
    argStream.ReadNumber(fEasingOvershoot, 1.70158f);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                if (CStaticFunctionDefinitions::MoveObject(pResource, pElement, iTime, vecTargetPosition, vecTargetRotation, easingType, fEasingPeriod,
                                                           fEasingAmplitude, fEasingOvershoot))
                {
                    lua_pushboolean(luaVM, true);
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

int CLuaObjectDefs::StopObject(lua_State* luaVM)
{
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::StopObject(pElement))
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

bool CLuaObjectDefs::SetObjectBreakable(CObject* const pObject, const bool bBreakable)
{
    return CStaticFunctionDefinitions::SetObjectBreakable(pObject, bBreakable);
}

bool CLuaObjectDefs::BreakObject(CObject* const pObject)
{
    return CStaticFunctionDefinitions::BreakObject(pObject);
}

bool CLuaObjectDefs::RespawnObject(CObject* const pObject) noexcept
{
    return CStaticFunctionDefinitions::RespawnObject(pObject);
}

bool CLuaObjectDefs::ToggleObjectRespawn(CObject* const pObject, const bool bRespawn) noexcept
{
    return CStaticFunctionDefinitions::ToggleObjectRespawn(pObject, bRespawn);
}

bool CLuaObjectDefs::IsObjectRespawnable(CObject* const pObject) noexcept
{
    return pObject->IsRespawnEnabled();
}
