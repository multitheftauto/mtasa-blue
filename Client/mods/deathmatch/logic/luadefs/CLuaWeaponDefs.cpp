/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaWeaponDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define MIN_CLIENT_REQ_WEAPON_PROPERTY_FLAG     "1.3.5-9.06139"

void CLuaWeaponDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"getWeaponNameFromID", GetWeaponNameFromID},
        {"getWeaponIDFromName", GetWeaponIDFromName},
        {"getSlotFromWeapon", GetSlotFromWeapon},
        {"createWeapon", CreateWeapon},
        {"setWeaponProperty", SetWeaponProperty},
        {"getWeaponProperty", GetWeaponProperty},
        {"getOriginalWeaponProperty", GetOriginalWeaponProperty},
        {"fireWeapon", FireWeapon},
        {"setWeaponState", SetWeaponState},
        {"getWeaponState", GetWeaponState},
        {"setWeaponTarget", SetWeaponTarget},
        {"getWeaponTarget", GetWeaponTarget},
        // {"setWeaponOwner", SetWeaponOwner},
        {"getWeaponOwner", GetWeaponOwner},
        {"setWeaponFlags", SetWeaponFlags},
        {"getWeaponFlags", GetWeaponFlags},
        {"setWeaponFiringRate", SetWeaponFiringRate},
        {"getWeaponFiringRate", GetWeaponFiringRate},
        {"resetWeaponFiringRate", ResetWeaponFiringRate},
        {"getWeaponAmmo", GetWeaponAmmo},
        {"getWeaponClipAmmo", GetWeaponClipAmmo},
        {"setWeaponAmmo", SetWeaponAmmo},
        {"setWeaponClipAmmo", SetWeaponClipAmmo},
        {"setWeaponRenderEnabled", ArgumentParser<SetWeaponRenderEnabled>},
        {"isWeaponRenderEnabled", ArgumentParser<IsWeaponRenderEnabled>}
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaWeaponDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createWeapon");
    lua_classfunction(luaVM, "fire", "fireWeapon");
    lua_classfunction(luaVM, "resetFiringRate", "resetWeaponFiringRate");

    lua_classfunction(luaVM, "setProperty", "setWeaponProperty");
    lua_classfunction(luaVM, "setTarget", "setWeaponTarget");
    lua_classfunction(luaVM, "setFiringRate", "setWeaponFiringRate");
    lua_classfunction(luaVM, "setState", "setWeaponState");
    lua_classfunction(luaVM, "setFlags", "setWeaponFlags");
    lua_classfunction(luaVM, "setAmmo", "setWeaponAmmo");
    lua_classfunction(luaVM, "setClipAmmo", "setWeaponClipAmmo");

    lua_classfunction(luaVM, "getProperty", "getWeaponProperty");
    lua_classfunction(luaVM, "getOwner", "getWeaponOwner");
    lua_classfunction(luaVM, "getTarget", "getWeaponTarget");
    lua_classfunction(luaVM, "getFiringRate", "getWeaponFiringRate");
    lua_classfunction(luaVM, "getState", "getWeaponState");
    lua_classfunction(luaVM, "getFlags", "getWeaponFlags");
    lua_classfunction(luaVM, "getAmmo", "getWeaponAmmo");
    lua_classfunction(luaVM, "getClipAmmo", "getWeaponClipAmmo");

    lua_classvariable(luaVM, "target", NULL, "getWeaponTarget");
    lua_classvariable(luaVM, "firingRate", "setWeaponFiringRate", "getWeaponFiringRate");
    lua_classvariable(luaVM, "state", "setWeaponState", "getWeaponState");
    lua_classvariable(luaVM, "ammo", "setWeaponAmmo", "getWeaponAmmo");
    lua_classvariable(luaVM, "clipAmmo", "setWeaponClipAmmo", "getWeaponClipAmmo");

    lua_registerclass(luaVM, "Weapon", "Element");
}

int CLuaWeaponDefs::GetWeaponNameFromID(lua_State* luaVM)
{
    unsigned char    ucID = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucID);

    if (!argStream.HasErrors())
    {
        SString strBuffer;
        if (CStaticFunctionDefinitions::GetWeaponNameFromID(ucID, strBuffer))
        {
            lua_pushstring(luaVM, strBuffer);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::GetSlotFromWeapon(lua_State* luaVM)
{
    eWeaponType      weaponType;
    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumStringOrNumber(weaponType);

    if (!argStream.HasErrors())
    {
        char cSlot = CWeaponNames::GetSlotFromWeapon(weaponType);
        if (cSlot >= 0)
            lua_pushnumber(luaVM, cSlot);
        else
            lua_pushboolean(luaVM, false);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::GetWeaponIDFromName(lua_State* luaVM)
{
    SString          strName = "";
    unsigned char    ucID = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GetWeaponIDFromName(strName, ucID))
        {
            lua_pushnumber(luaVM, ucID);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::CreateWeapon(lua_State* luaVM)
{
    CVector          vecPos;
    eWeaponType      weaponType;
    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumStringOrNumber(weaponType);
    argStream.ReadVector3D(vecPos);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                CClientWeapon* pWeapon = CStaticFunctionDefinitions::CreateWeapon(*pResource, weaponType, vecPos);
                if (pWeapon)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add((CClientEntity*)pWeapon);
                    }

                    lua_pushelement(luaVM, pWeapon);
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
int CLuaWeaponDefs::FireWeapon(lua_State* luaVM)
{
    CClientWeapon*   pWeapon;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FireWeapon(pWeapon))
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

int CLuaWeaponDefs::SetWeaponProperty(lua_State* luaVM)
{
    CClientWeapon*   pWeapon;
    eWeaponProperty  weaponProperty;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);
    argStream.ReadEnumString(weaponProperty);

    if (!argStream.HasErrors())
    {
        if (weaponProperty == WEAPON_DAMAGE)
        {
            short sData = 0;
            argStream.ReadNumber(sData);
            if (!argStream.HasErrors())
            {
                if (CStaticFunctionDefinitions::SetWeaponProperty(pWeapon, weaponProperty, sData))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
        }
        else if (weaponProperty == WEAPON_FIRE_ROTATION)
        {
            CVector vecRotation;
            argStream.ReadVector3D(vecRotation);
            if (!argStream.HasErrors())
            {
                if (CStaticFunctionDefinitions::SetWeaponProperty(pWeapon, weaponProperty, vecRotation))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
        }
        else
        {
            float fData = 0.0f;
            argStream.ReadNumber(fData);
            if (!argStream.HasErrors())
            {
                if (CStaticFunctionDefinitions::SetWeaponProperty(pWeapon, weaponProperty, fData))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::SetWeaponState(lua_State* luaVM)
{
    CClientWeapon*   pWeapon;
    eWeaponState     weaponState;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);
    argStream.ReadEnumString(weaponState);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWeaponState(pWeapon, weaponState))
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

int CLuaWeaponDefs::GetWeaponState(lua_State* luaVM)
{
    CClientWeapon*   pWeapon;
    eWeaponState     weaponState;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);

    if (!argStream.HasErrors())
    {
        weaponState = pWeapon->GetWeaponState();
        SString strValue = EnumToString(weaponState);
        lua_pushstring(luaVM, strValue);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::SetWeaponTarget(lua_State* luaVM)
{
    CClientWeapon*   pWeapon;
    CClientEntity*   pTarget;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);
    if (argStream.NextIsUserData())
    {
        int targetBone;
        argStream.ReadUserData(pTarget);
        argStream.ReadNumber(targetBone, 255);
        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::SetWeaponTarget(pWeapon, pTarget, targetBone))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else if (argStream.NextIsNumber() || argStream.NextIsUserDataOfType<CLuaVector3D>())
    {
        CVector vecTarget;
        argStream.ReadVector3D(vecTarget);
        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::SetWeaponTarget(pWeapon, vecTarget))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else if (argStream.NextIsNil())
    {
        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::ClearWeaponTarget(pWeapon))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        argStream.SetCustomError("Expected element, number or nil at argument 2");

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::GetWeaponTarget(lua_State* luaVM)
{
    CClientWeapon*   pWeapon;
    CClientEntity*   pTarget;
    CVector          vecTarget;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);
    if (!argStream.HasErrors())
    {
        switch (pWeapon->GetWeaponTargetType())
        {
            case TARGET_TYPE_VECTOR:
                vecTarget = pWeapon->GetWeaponVectorTarget();
                lua_pushnumber(luaVM, vecTarget.fX);
                lua_pushnumber(luaVM, vecTarget.fY);
                lua_pushnumber(luaVM, vecTarget.fZ);
                return 3;
            case TARGET_TYPE_ENTITY:
                pTarget = pWeapon->GetWeaponEntityTarget();
                lua_pushelement(luaVM, pTarget);
                return 1;
            case TARGET_TYPE_FIXED:
                lua_pushnil(luaVM);
                return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::GetWeaponOwner(lua_State* luaVM)
{
    CClientWeapon*   pWeapon;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);
    if (!argStream.HasErrors())
    {
        CClientPlayer* pOwner = pWeapon->GetOwner();
        if (pOwner)
        {
            lua_pushelement(luaVM, pOwner);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::SetWeaponOwner(lua_State* luaVM)
{
    CClientWeapon*   pWeapon;
    CClientPlayer*   pPlayer;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);
    if (argStream.NextIsUserData())
    {
        argStream.ReadUserData(pPlayer);
        if (!argStream.HasErrors())
        {
            pWeapon->SetOwner(pPlayer);

            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else if (argStream.NextIsNil())
    {
        if (!argStream.HasErrors())
        {
            pWeapon->SetOwner(NULL);

            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaWeaponDefs::SetWeaponFlags(lua_State* luaVM)
{
    CClientWeapon*    pWeapon = NULL;
    SLineOfSightFlags flags;
    eWeaponFlags      flag;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pWeapon);
    argStream.ReadEnumString(flag);
    if (!argStream.HasErrors())
    {
        if (flag != WEAPONFLAGS_FLAGS)
        {
            bool bData;
            argStream.ReadBool(bData);
            if (CStaticFunctionDefinitions::SetWeaponFlags(pWeapon, flag, bData))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
        {
            argStream.ReadBool(flags.bCheckBuildings);
            argStream.ReadBool(flags.bCheckCarTires);
            argStream.ReadBool(flags.bCheckDummies);
            argStream.ReadBool(flags.bCheckObjects);
            argStream.ReadBool(flags.bCheckPeds);
            argStream.ReadBool(flags.bCheckVehicles);
            argStream.ReadBool(flags.bSeeThroughStuff);
            argStream.ReadBool(flags.bShootThroughStuff);
            if (!argStream.HasErrors())
            {
                if (CStaticFunctionDefinitions::SetWeaponFlags(pWeapon, flags))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::GetWeaponFlags(lua_State* luaVM)
{
    CClientWeapon*    pWeapon = NULL;
    SLineOfSightFlags flags;
    eWeaponFlags      flag;
    bool              bData;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pWeapon);
    argStream.ReadEnumString(flag);
    if (!argStream.HasErrors())
    {
        if (flag != WEAPONFLAGS_FLAGS)
        {
            if (CStaticFunctionDefinitions::GetWeaponFlags(pWeapon, flag, bData))
            {
                lua_pushboolean(luaVM, bData);
                return 1;
            }
        }
        else
        {
            if (CStaticFunctionDefinitions::GetWeaponFlags(pWeapon, flags))
            {
                lua_pushboolean(luaVM, flags.bCheckBuildings);
                lua_pushboolean(luaVM, flags.bCheckCarTires);
                lua_pushboolean(luaVM, flags.bCheckDummies);
                lua_pushboolean(luaVM, flags.bCheckObjects);
                lua_pushboolean(luaVM, flags.bCheckPeds);
                lua_pushboolean(luaVM, flags.bCheckVehicles);
                lua_pushboolean(luaVM, flags.bSeeThroughStuff);
                lua_pushboolean(luaVM, flags.bShootThroughStuff);
                return 8;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::SetWeaponFiringRate(lua_State* luaVM)
{
    CClientWeapon*   pWeapon = NULL;
    int              iFiringRate = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);
    argStream.ReadNumber(iFiringRate);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWeaponFiringRate(pWeapon, iFiringRate))
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

int CLuaWeaponDefs::GetWeaponFiringRate(lua_State* luaVM)
{
    CClientWeapon*   pWeapon = NULL;
    int              iFiringRate = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GetWeaponFiringRate(pWeapon, iFiringRate))
        {
            lua_pushnumber(luaVM, iFiringRate);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::ResetWeaponFiringRate(lua_State* luaVM)
{
    CClientWeapon*   pWeapon = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::ResetWeaponFiringRate(pWeapon))
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

int CLuaWeaponDefs::GetWeaponClipAmmo(lua_State* luaVM)
{
    CClientWeapon*   pWeapon = NULL;
    int              iClipAmmo = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GetWeaponClipAmmo(pWeapon, iClipAmmo))
        {
            lua_pushnumber(luaVM, iClipAmmo);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::GetWeaponAmmo(lua_State* luaVM)
{
    CClientWeapon*   pWeapon = NULL;
    int              iAmmo = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GetWeaponAmmo(pWeapon, iAmmo))
        {
            lua_pushnumber(luaVM, iAmmo);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::SetWeaponAmmo(lua_State* luaVM)
{
    CClientWeapon*   pWeapon = NULL;
    int              iAmmo = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);
    argStream.ReadNumber(iAmmo);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWeaponAmmo(pWeapon, iAmmo))
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

int CLuaWeaponDefs::SetWeaponClipAmmo(lua_State* luaVM)
{
    CClientWeapon*   pWeapon = NULL;
    int              iAmmo = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pWeapon);
    argStream.ReadNumber(iAmmo);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWeaponClipAmmo(pWeapon, iAmmo))
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

int CLuaWeaponDefs::GetWeaponProperty(lua_State* luaVM)
{
    eWeaponSkill     eWepSkill = eWeaponSkill::WEAPONSKILL_STD;
    eWeaponType      eWep = eWeaponType::WEAPONTYPE_UNARMED;
    eWeaponProperty  eProp = eWeaponProperty::WEAPON_INVALID_PROPERTY;
    CClientWeapon*   pWeapon;
    CScriptArgReader argStream(luaVM);

    if (argStream.NextIsUserData())
    {
        argStream.ReadUserData(pWeapon);
        argStream.ReadEnumString(eProp);

        if (!argStream.HasErrors())
        {
            if (eProp == WEAPON_DAMAGE)
            {
                short sData = 0;
                if (CStaticFunctionDefinitions::GetWeaponProperty(pWeapon, eProp, sData))
                {
                    lua_pushnumber(luaVM, sData);
                    return 1;
                }
            }
            else if (eProp == WEAPON_FIRE_ROTATION)
            {
                CVector vecWeaponInfo;
                if (CStaticFunctionDefinitions::GetWeaponProperty(pWeapon, eProp, vecWeaponInfo))
                {
                    lua_pushnumber(luaVM, vecWeaponInfo.fX);
                    lua_pushnumber(luaVM, vecWeaponInfo.fY);
                    lua_pushnumber(luaVM, vecWeaponInfo.fZ);
                    return 1;
                }
            }
            else
            {
                float fData = 0;
                if (CStaticFunctionDefinitions::GetWeaponProperty(pWeapon, eProp, fData))
                {
                    lua_pushnumber(luaVM, fData);
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

        lua_pushboolean(luaVM, false);
        return 1;
    }

    argStream.ReadEnumStringOrNumber(eWep);
    argStream.ReadEnumStringOrNumber(eWepSkill);
    argStream.ReadEnumString(eProp);
    if (!argStream.HasErrors())
    {
        switch (eProp)
        {
            case WEAPON_WEAPON_RANGE:
            case WEAPON_TARGET_RANGE:
            case WEAPON_ACCURACY:
            case WEAPON_FIRING_SPEED:
            case WEAPON_LIFE_SPAN:
            case WEAPON_SPREAD:
            case WEAPON_MOVE_SPEED:
                // Get only
            case WEAPON_REQ_SKILL_LEVEL:
            case WEAPON_ANIM_LOOP_START:
            case WEAPON_ANIM_LOOP_STOP:
            case WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME:
            case WEAPON_ANIM2_LOOP_START:
            case WEAPON_ANIM2_LOOP_STOP:
            case WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME:
            case WEAPON_ANIM_BREAKOUT_TIME:
            case WEAPON_RADIUS:
            {
                float fWeaponInfo = 0.0f;

                if (CStaticFunctionDefinitions::GetWeaponProperty(eProp, eWep, eWepSkill, fWeaponInfo))
                {
                    lua_pushnumber(luaVM, fWeaponInfo);
                    return 1;
                }
                break;
            }
            case WEAPON_DAMAGE:
            case WEAPON_MAX_CLIP_AMMO:
            case WEAPON_FLAGS:
            case WEAPON_ANIM_GROUP:
            case WEAPON_FIRETYPE:
            case WEAPON_MODEL:
            case WEAPON_MODEL2:
            case WEAPON_SLOT:
            case WEAPON_AIM_OFFSET:
            case WEAPON_SKILL_LEVEL:
            case WEAPON_DEFAULT_COMBO:
            case WEAPON_COMBOS_AVAILABLE:
            {
                int sWeaponInfo = 0;

                if (CStaticFunctionDefinitions::GetWeaponProperty(eProp, eWep, eWepSkill, sWeaponInfo))
                {
                    lua_pushinteger(luaVM, sWeaponInfo);
                    return 1;
                }
                break;
            }
            case WEAPON_FIRE_OFFSET:
            {
                CVector vecWeaponInfo;

                if (CStaticFunctionDefinitions::GetWeaponProperty(eProp, eWep, eWepSkill, vecWeaponInfo))
                {
                    lua_pushnumber(luaVM, vecWeaponInfo.fX);
                    lua_pushnumber(luaVM, vecWeaponInfo.fY);
                    lua_pushnumber(luaVM, vecWeaponInfo.fZ);
                    return 3;
                }
                break;
            }
            case WEAPON_FLAG_AIM_NO_AUTO:
            case WEAPON_FLAG_AIM_ARM:
            case WEAPON_FLAG_AIM_1ST_PERSON:
            case WEAPON_FLAG_AIM_FREE:
            case WEAPON_FLAG_MOVE_AND_AIM:
            case WEAPON_FLAG_MOVE_AND_SHOOT:
            case WEAPON_FLAG_TYPE_THROW:
            case WEAPON_FLAG_TYPE_HEAVY:
            case WEAPON_FLAG_TYPE_CONSTANT:
            case WEAPON_FLAG_TYPE_DUAL:
            case WEAPON_FLAG_ANIM_RELOAD:
            case WEAPON_FLAG_ANIM_CROUCH:
            case WEAPON_FLAG_ANIM_RELOAD_LOOP:
            case WEAPON_FLAG_ANIM_RELOAD_LONG:
            case WEAPON_FLAG_SHOT_SLOWS:
            case WEAPON_FLAG_SHOT_RAND_SPEED:
            case WEAPON_FLAG_SHOT_ANIM_ABRUPT:
            case WEAPON_FLAG_SHOT_EXPANDS:
            {
                MinClientReqCheck(argStream, MIN_CLIENT_REQ_WEAPON_PROPERTY_FLAG, "flag name is being used");
                if (!argStream.HasErrors())
                {
                    bool bEnable;
                    if (CStaticFunctionDefinitions::GetWeaponPropertyFlag(eProp, eWep, eWepSkill, bEnable))
                    {
                        lua_pushboolean(luaVM, bEnable);
                        return 1;
                    }
                }
                break;
            }
            default:
            {
                argStream.SetCustomError("unsupported weapon property at argument 3");
                break;
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWeaponDefs::GetOriginalWeaponProperty(lua_State* luaVM)
{
    eWeaponSkill    eWepSkill = eWeaponSkill::WEAPONSKILL_STD;
    eWeaponType     eWep = eWeaponType::WEAPONTYPE_UNARMED;
    eWeaponProperty eProp = eWeaponProperty::WEAPON_INVALID_PROPERTY;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumStringOrNumber(eWep);
    argStream.ReadEnumStringOrNumber(eWepSkill);
    argStream.ReadEnumString(eProp);
    if (!argStream.HasErrors())
    {
        switch (eProp)
        {
            case WEAPON_WEAPON_RANGE:
            case WEAPON_TARGET_RANGE:
            case WEAPON_ACCURACY:
            case WEAPON_FIRING_SPEED:
            case WEAPON_LIFE_SPAN:
            case WEAPON_SPREAD:
            case WEAPON_MOVE_SPEED:
                // Get only
            case WEAPON_REQ_SKILL_LEVEL:
            case WEAPON_ANIM_LOOP_START:
            case WEAPON_ANIM_LOOP_STOP:
            case WEAPON_ANIM_LOOP_RELEASE_BULLET_TIME:
            case WEAPON_ANIM2_LOOP_START:
            case WEAPON_ANIM2_LOOP_STOP:
            case WEAPON_ANIM2_LOOP_RELEASE_BULLET_TIME:
            case WEAPON_ANIM_BREAKOUT_TIME:
            case WEAPON_RADIUS:
            {
                float fWeaponInfo = 0.0f;

                if (CStaticFunctionDefinitions::GetOriginalWeaponProperty(eProp, eWep, eWepSkill, fWeaponInfo))
                {
                    lua_pushnumber(luaVM, fWeaponInfo);
                    return 1;
                }
                break;
            }
            case WEAPON_DAMAGE:
            case WEAPON_MAX_CLIP_AMMO:
            case WEAPON_FLAGS:
            case WEAPON_ANIM_GROUP:
            case WEAPON_FIRETYPE:
            case WEAPON_MODEL:
            case WEAPON_MODEL2:
            case WEAPON_SLOT:
            case WEAPON_AIM_OFFSET:
            case WEAPON_SKILL_LEVEL:
            case WEAPON_DEFAULT_COMBO:
            case WEAPON_COMBOS_AVAILABLE:
            {
                int sWeaponInfo = 0;

                if (CStaticFunctionDefinitions::GetOriginalWeaponProperty(eProp, eWep, eWepSkill, sWeaponInfo))
                {
                    lua_pushinteger(luaVM, sWeaponInfo);
                    return 1;
                }
                break;
            }
            case WEAPON_FIRE_OFFSET:
            {
                CVector vecWeaponInfo;

                if (CStaticFunctionDefinitions::GetOriginalWeaponProperty(eProp, eWep, eWepSkill, vecWeaponInfo))
                {
                    lua_pushnumber(luaVM, vecWeaponInfo.fX);
                    lua_pushnumber(luaVM, vecWeaponInfo.fY);
                    lua_pushnumber(luaVM, vecWeaponInfo.fZ);
                    return 3;
                }
                break;
            }
            case WEAPON_FLAG_AIM_NO_AUTO:
            case WEAPON_FLAG_AIM_ARM:
            case WEAPON_FLAG_AIM_1ST_PERSON:
            case WEAPON_FLAG_AIM_FREE:
            case WEAPON_FLAG_MOVE_AND_AIM:
            case WEAPON_FLAG_MOVE_AND_SHOOT:
            case WEAPON_FLAG_TYPE_THROW:
            case WEAPON_FLAG_TYPE_HEAVY:
            case WEAPON_FLAG_TYPE_CONSTANT:
            case WEAPON_FLAG_TYPE_DUAL:
            case WEAPON_FLAG_ANIM_RELOAD:
            case WEAPON_FLAG_ANIM_CROUCH:
            case WEAPON_FLAG_ANIM_RELOAD_LOOP:
            case WEAPON_FLAG_ANIM_RELOAD_LONG:
            case WEAPON_FLAG_SHOT_SLOWS:
            case WEAPON_FLAG_SHOT_RAND_SPEED:
            case WEAPON_FLAG_SHOT_ANIM_ABRUPT:
            case WEAPON_FLAG_SHOT_EXPANDS:
            {
                MinClientReqCheck(argStream, MIN_CLIENT_REQ_WEAPON_PROPERTY_FLAG, "flag name is being used");
                if (!argStream.HasErrors())
                {
                    bool bEnable;
                    if (CStaticFunctionDefinitions::GetOriginalWeaponPropertyFlag(eProp, eWep, eWepSkill, bEnable))
                    {
                        lua_pushboolean(luaVM, bEnable);
                        return 1;
                    }
                }
                break;
            }
            default:
            {
                argStream.SetCustomError("unsupported weapon property at argument 3");
                break;
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaWeaponDefs::SetWeaponRenderEnabled(bool enabled)
{
    g_pClientGame->SetWeaponRenderEnabled(enabled);
    return true;
}

bool CLuaWeaponDefs::IsWeaponRenderEnabled()
{
    return g_pClientGame->IsWeaponRenderEnabled();
}
