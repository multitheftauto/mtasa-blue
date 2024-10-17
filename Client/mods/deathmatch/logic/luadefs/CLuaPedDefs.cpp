/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPedDefs.cpp
 *  PURPOSE:     Lua ped definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CWeapon.h>
#include "lua/CLuaFunctionParser.h"
#include <game/CTasks.h>
#include <game/TaskBasic.h>
#include <game/CAnimManager.h>

#define MIN_CLIENT_REQ_REMOVEPEDFROMVEHICLE_CLIENTSIDE "1.3.0-9.04482"
#define MIN_CLIENT_REQ_WARPPEDINTOVEHICLE_CLIENTSIDE "1.3.0-9.04482"

void CLuaPedDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createPed", CreatePed},
        {"detonateSatchels", DetonateSatchels},
        {"killPed", KillPed},
        {"resetPedVoice", ArgumentParser<ResetPedVoice>},
        {"updateElementRpHAnim", ArgumentParser<UpdateElementRpHAnim>},
        {"addPedClothes", AddPedClothes},
        {"removePedClothes", RemovePedClothes},
        {"warpPedIntoVehicle", WarpPedIntoVehicle},
        {"removePedFromVehicle", RemovePedFromVehicle},
        {"givePedWeapon", GivePedWeapon},

        {"setPedVoice", SetPedVoice},
        {"setElementBonePosition", ArgumentParser<SetElementBonePosition>},
        {"setElementBoneRotation", ArgumentParser<SetElementBoneRotation>},
        {"setElementBoneQuaternion", ArgumentParser<SetElementBoneQuaternion>},
        {"setElementBoneMatrix", ArgumentParser<SetElementBoneMatrix>},
        {"setPedRotation", SetPedRotation},
        {"setPedWeaponSlot", SetPedWeaponSlot},
        {"setPedCanBeKnockedOffBike", SetPedCanBeKnockedOffBike},
        {"setPedAnimation", SetPedAnimation},
        {"setPedAnimationProgress", SetPedAnimationProgress},
        {"setPedAnimationSpeed", SetPedAnimationSpeed},
        {"setPedWalkingStyle", SetPedMoveAnim},
        {"setPedControlState", SetPedControlState},
        {"setPedAnalogControlState", SetPedAnalogControlState},
        {"setPedDoingGangDriveby", SetPedDoingGangDriveby},
        {"setPedFightingStyle", ArgumentParser<SetPedFightingStyle>},
        {"setPedLookAt", SetPedLookAt},
        {"setPedHeadless", SetPedHeadless},
        {"setPedFrozen", SetPedFrozen},
        {"setPedFootBloodEnabled", SetPedFootBloodEnabled},
        {"setPedCameraRotation", SetPedCameraRotation},
        {"setPedAimTarget", SetPedAimTarget},
        {"setPedStat", SetPedStat},
        {"setPedOxygenLevel", SetPedOxygenLevel},
        {"setPedArmor", ArgumentParser<SetPedArmor>},
        {"setPedEnterVehicle", ArgumentParser<SetPedEnterVehicle>},
        {"setPedExitVehicle", ArgumentParser<SetPedExitVehicle>},
        {"setPedBleeding", ArgumentParser<SetPedBleeding>},

        {"getPedVoice", GetPedVoice},
        {"getElementBonePosition", ArgumentParser<GetElementBonePosition>},
        {"getElementBoneRotation", ArgumentParser<GetElementBoneRotation>},
        {"getElementBoneQuaternion", ArgumentParser<GetElementBoneQuaternion>},
        {"getElementBoneMatrix", ArgumentParser<GetElementBoneMatrix>},
        {"getPedRotation", GetPedRotation},
        {"getPedWeaponSlot", GetPedWeaponSlot},
        {"canPedBeKnockedOffBike", CanPedBeKnockedOffBike},
        {"getPedAnimation", GetPedAnimation},
        {"getPedAnimationProgress", ArgumentParser<GetPedAnimationProgress>},
        {"getPedAnimationSpeed", ArgumentParser<GetPedAnimationSpeed>},
        {"getPedAnimationLength", ArgumentParser<GetPedAnimationLength>},        
        {"getPedWalkingStyle", GetPedMoveAnim},
        {"getPedControlState", GetPedControlState},
        {"getPedAnalogControlState", GetPedAnalogControlState},
        {"isPedDoingGangDriveby", IsPedDoingGangDriveby},
        {"getPedFightingStyle", GetPedFightingStyle},

        {"isPedHeadless", IsPedHeadless},
        {"isPedFrozen", IsPedFrozen},
        {"isPedFootBloodEnabled", IsPedFootBloodEnabled},
        {"getPedCameraRotation", GetPedCameraRotation},

        {"getPedStat", GetPedStat},
        {"getPedOxygenLevel", GetPedOxygenLevel},
        {"getPedArmor", GetPedArmor},
        {"isPedBleeding", ArgumentParser<IsPedBleeding>},

        {"getPedContactElement", GetPedContactElement},
        {"getPedTask", GetPedTask},
        {"getPedSimplestTask", GetPedSimplestTask},
        {"getPedTarget", GetPedTarget},
        {"getPedTargetStart", GetPedTargetStart},
        {"getPedTargetEnd", GetPedTargetEnd},
        {"getPedTargetCollision", GetPedTargetCollision},
        {"getPedWeapon", GetPedWeapon},
        {"getPedAmmoInClip", GetPedAmmoInClip},
        {"getPedTotalAmmo", GetPedTotalAmmo},
        {"getPedOccupiedVehicle", GetPedOccupiedVehicle},
        {"getPedOccupiedVehicleSeat", GetPedOccupiedVehicleSeat},
        {"getPedBonePosition", GetPedBonePosition},
        {"getPedClothes", GetPedClothes},
        {"getPedMoveState", GetPedMoveState},

        {"doesPedHaveJetPack", DoesPedHaveJetPack},
        {"isPedInVehicle", IsPedInVehicle},
        {"isPedWearingJetpack", DoesPedHaveJetPack},
        {"isPedOnGround", IsPedOnGround},
        {"isPedDoingTask", IsPedDoingTask},
        {"isPedChoking", IsPedChoking},
        {"isPedDucked", IsPedDucked},
        {"isPedDead", IsPedDead},
        {"isPedReloadingWeapon", IsPedReloadingWeapon},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaPedDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createPed");
    lua_classfunction(luaVM, "kill", "killPed");

    lua_classfunction(luaVM, "getBodyPartName", "getBodyPartName");
    lua_classfunction(luaVM, "getClothesTypeName", "getClothesTypeName");
    lua_classfunction(luaVM, "getValidModels", "getValidPedModels");
    lua_classfunction(luaVM, "getTypeIndexFromClothes", "getTypeIndexFromClothes");
    lua_classfunction(luaVM, "getClothesByTypeIndex", "getClothesByTypeIndex");
    lua_classvariable(luaVM, "validModels", NULL, "getValidPedModels");

    lua_classfunction(luaVM, "canBeKnockedOffBike", "canPedBeKnockedOffBike");
    lua_classfunction(luaVM, "doesHaveJetPack", "doesPedHaveJetPack");
    lua_classfunction(luaVM, "isWearingJetpack", "isPedWearingJetpack");            // introduced in 1.5.5-9.13846
    lua_classfunction(luaVM, "getAmmoInClip", "getPedAmmoInClip");
    lua_classfunction(luaVM, "getAnalogControlState", "getPedAnalogControlState");
    lua_classfunction(luaVM, "getAnimation", "getPedAnimation");
    lua_classfunction(luaVM, "getArmor", "getPedArmor");
    lua_classfunction(luaVM, "getFightingStyle", "getPedFightingStyle");
    lua_classfunction(luaVM, "getClothes", "getPedClothes");
    lua_classfunction(luaVM, "addClothes", "addPedClothes");
    lua_classfunction(luaVM, "removeClothes", "removePedClothes");
    lua_classfunction(luaVM, "getContactElement", "getPedContactElement");
    lua_classfunction(luaVM, "getControlState", "getPedControlState");
    lua_classfunction(luaVM, "getMoveState", "getPedMoveState");
    lua_classfunction(luaVM, "getOccupiedVehicle", "getPedOccupiedVehicle");
    lua_classfunction(luaVM, "getOccupiedVehicleSeat", "getPedOccupiedVehicleSeat");
    lua_classfunction(luaVM, "getOxygenLevel", "getPedOxygenLevel");
    lua_classfunction(luaVM, "getStat", "getPedStat");
    lua_classfunction(luaVM, "getTarget", "getPedTarget");
    lua_classfunction(luaVM, "getTargetCollision", OOP_GetPedTargetCollision);
    lua_classfunction(luaVM, "getSimplestTask", "getPedSimplestTask");
    lua_classfunction(luaVM, "getTask", "getPedTask");
    lua_classfunction(luaVM, "getTotalAmmo", "getPedTotalAmmo");
    lua_classfunction(luaVM, "getVoice", "getPedVoice");
    lua_classfunction(luaVM, "resetVoice", "resetPedVoice");
    lua_classfunction(luaVM, "getWeapon", "getPedWeapon");
    lua_classfunction(luaVM, "isChocking", "isPedChoking");
    lua_classfunction(luaVM, "isDoingGangDriveby", "isPedDoingGangDriveby");
    lua_classfunction(luaVM, "isDoingTask", "isPedDoingTask");
    lua_classfunction(luaVM, "isDucked", "isPedDucked");
    lua_classfunction(luaVM, "isHeadless", "isPedHeadless");
    lua_classfunction(luaVM, "isInVehicle", "isPedInVehicle");
    lua_classfunction(luaVM, "isOnFire", "isPedOnFire");
    lua_classfunction(luaVM, "isOnGround", "isPedOnGround");
    lua_classfunction(luaVM, "isTargetingMarkerEnabled", "isPedTargetingMarkerEnabled");
    lua_classfunction(luaVM, "isDead", "isPedDead");
    lua_classfunction(luaVM, "setFootBloodEnabled", "setPedFootBloodEnabled");
    lua_classfunction(luaVM, "getTargetEnd", OOP_GetPedTargetEnd);
    lua_classfunction(luaVM, "getTargetStart", OOP_GetPedTargetStart);
    lua_classfunction(luaVM, "getWeaponMuzzlePosition", "getPedWeaponMuzzlePosition");
    lua_classfunction(luaVM, "getBonePosition", OOP_GetPedBonePosition);
    lua_classfunction(luaVM, "getCameraRotation", "getPedCameraRotation");
    lua_classfunction(luaVM, "getWeaponSlot", "getPedWeaponSlot");
    lua_classfunction(luaVM, "getWalkingStyle", "getPedWalkingStyle");
    lua_classfunction(luaVM, "isBleeding", "isPedBleeding");

    lua_classfunction(luaVM, "setCanBeKnockedOffBike", "setPedCanBeKnockedOffBike");
    lua_classfunction(luaVM, "setAnalogControlState", "setPedAnalogControlState");
    lua_classfunction(luaVM, "setAnimation", "setPedAnimation");
    lua_classfunction(luaVM, "setAnimationProgress", "setPedAnimationProgress");
    lua_classfunction(luaVM, "setAnimationSpeed", "setPedAnimationSpeed");
    lua_classfunction(luaVM, "setCameraRotation", "setPedCameraRotation");
    lua_classfunction(luaVM, "setControlState", "setPedControlState");
    lua_classfunction(luaVM, "warpIntoVehicle", "warpPedIntoVehicle");
    lua_classfunction(luaVM, "setOxygenLevel", "setPedOxygenLevel");
    lua_classfunction(luaVM, "setArmor", "setPedArmor");
    lua_classfunction(luaVM, "setWeaponSlot", "setPedWeaponSlot");
    lua_classfunction(luaVM, "setDoingGangDriveby", "setPedDoingGangDriveby");
    lua_classfunction(luaVM, "setFightingStyle", "setPedFightingStyle");
    lua_classfunction(luaVM, "setHeadless", "setPedHeadless");
    lua_classfunction(luaVM, "setOnFire", "setPedOnFire");
    lua_classfunction(luaVM, "setTargetingMarkerEnabled", "setPedTargetingMarkerEnabled");
    lua_classfunction(luaVM, "setVoice", "setPedVoice");
    lua_classfunction(luaVM, "removeFromVehicle", "removePedFromVehicle");
    lua_classfunction(luaVM, "setAimTarget", "setPedAimTarget");
    lua_classfunction(luaVM, "setLookAt", "setPedLookAt");
    lua_classfunction(luaVM, "setWalkingStyle", "setPedWalkingStyle");
    lua_classfunction(luaVM, "setStat", "setPedStat");
    lua_classfunction(luaVM, "giveWeapon", "givePedWeapon");
    lua_classfunction(luaVM, "isReloadingWeapon", "isPedReloadingWeapon");
    lua_classfunction(luaVM, "setEnterVehicle", "setPedEnterVehicle");
    lua_classfunction(luaVM, "setExitVehicle", "setPedExitVehicle");
    lua_classfunction(luaVM, "setBleeding", "setPedBleeding");

    lua_classvariable(luaVM, "vehicle", OOP_WarpPedIntoVehicle, GetPedOccupiedVehicle);
    lua_classvariable(luaVM, "vehicleSeat", NULL, "getPedOccupiedVehicleSeat");
    lua_classvariable(luaVM, "canBeKnockedOffBike", "setPedCanBeKnockedOffBike", "canPedBeKnockedOffBike");
    lua_classvariable(luaVM, "hasJetPack", NULL, "doesPedHaveJetPack");
    lua_classvariable(luaVM, "jetpack", NULL, "isPedWearingJetpack");            // introduced in 1.5.5-9.13846
    lua_classvariable(luaVM, "armor", "setPedArmor", "getPedArmor");
    lua_classvariable(luaVM, "fightingStyle", "setPedFightingStyle", "getPedFightingStyle");
    lua_classvariable(luaVM, "cameraRotation", "setPedCameraRotation", "getPedCameraRotation");
    lua_classvariable(luaVM, "contactElement", NULL, "getPedContactElement");
    lua_classvariable(luaVM, "moveState", NULL, "getPedMoveState");
    lua_classvariable(luaVM, "oxygenLevel", "setPedOxygenLevel", "getPedOxygenLevel");
    lua_classvariable(luaVM, "target", NULL, "getPedTarget");
    lua_classvariable(luaVM, "simplestTask", NULL, "getPedSimplestTask");
    lua_classvariable(luaVM, "choking", NULL, "isPedChoking");
    lua_classvariable(luaVM, "doingGangDriveby", "setPedDoingGangDriveby", "isPedDoingGangDriveby");
    lua_classvariable(luaVM, "ducked", NULL, "isPedDucked");
    lua_classvariable(luaVM, "headless", "setPedHeadless", "isPedHeadless");
    lua_classvariable(luaVM, "inVehicle", NULL, "isPedInVehicle");
    lua_classvariable(luaVM, "onFire", "setPedOnFire", "isPedOnFire");
    lua_classvariable(luaVM, "onGround", NULL, "isPedOnGround");
    lua_classvariable(luaVM, "dead", NULL, "isPedDead");
    lua_classvariable(luaVM, "targetingMarker", "setPedTargetingMarkerEnabled", "isPedTargetingMarkerEnabled");
    lua_classvariable(luaVM, "footBlood", "setPedFootBloodEnabled", NULL);
    lua_classvariable(luaVM, "bleeding", "setPedBleeding", "isPedBleeding");
    lua_classvariable(luaVM, "targetCollision", nullptr, OOP_GetPedTargetCollision);
    lua_classvariable(luaVM, "targetEnd", nullptr, OOP_GetPedTargetEnd);
    lua_classvariable(luaVM, "targetStart", nullptr, OOP_GetPedTargetStart);
    // lua_classvariable ( luaVM, "muzzlePosition", NULL, "getPedWeaponMuzzlePosition" ); // TODO: needs to return a vector3 for oop
    lua_classvariable(luaVM, "weaponSlot", "setPedWeaponSlot", "getPedWeaponSlot");
    lua_classvariable(luaVM, "walkingStyle", "setPedWalkingStyle", "getPedWalkingStyle");
    lua_classvariable(luaVM, "reloadingWeapon", nullptr, "isPedReloadingWeapon");

    lua_registerclass(luaVM, "Ped", "Element");
}

bool CLuaPedDefs::ResetPedVoice(CClientPed* ped)
{
    short szOldType, szNewType, szOldVoice, szNewVoice;
    ped->GetVoice(&szOldType, &szOldVoice);
    ped->ResetVoice();
    ped->GetVoice(&szNewType, &szNewVoice);
    return szNewType != szOldType && szNewVoice != szOldVoice;
}

int CLuaPedDefs::GetPedVoice(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        if (!pPed->IsSpeechEnabled())
        {
            lua_pushstring(luaVM, "PED_TYPE_DISABLED");
            return 1;
        }
        else
        {
            const char* szVoiceType = 0;
            const char* szVoiceBank = 0;
            pPed->GetVoice(&szVoiceType, &szVoiceBank);
            if (szVoiceType && szVoiceBank)
            {
                lua_pushstring(luaVM, szVoiceType);
                lua_pushstring(luaVM, szVoiceBank);
                return 2;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::SetPedVoice(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    SString          strVoiceType = "", strVoiceBank = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadString(strVoiceType);
    argStream.ReadString(strVoiceBank, "");

    if (!argStream.HasErrors())
    {
        const char* szVoiceType = strVoiceType.c_str();
        const char* szVoiceBank = strVoiceBank == "" ? NULL : strVoiceBank.c_str();

        if (szVoiceType)
        {
            if (!stricmp(szVoiceType, "PED_TYPE_DISABLED"))
            {
                pPed->SetSpeechEnabled(false);
            }

            else if (szVoiceBank)
            {
                pPed->SetSpeechEnabled(true);
                pPed->SetVoice(szVoiceType, szVoiceBank);
            }

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

int CLuaPedDefs::GetPedWeapon(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    unsigned char    ucSlot = 0xFF;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadNumber(ucSlot, 0xFF);

    if (!argStream.HasErrors())
    {
        if (ucSlot == 0xFF)
            ucSlot = pPed->GetCurrentWeaponSlot();

        unsigned char ucWeapon = pPed->GetWeaponType((eWeaponSlot)ucSlot);
        lua_pushnumber(luaVM, ucWeapon);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedWeaponSlot(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Grab his current slot
        int iSlot = pPed->GetCurrentWeaponSlot();
        lua_pushnumber(luaVM, iSlot);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedAmmoInClip(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    unsigned char    ucSlot = 0xFF;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadNumber(ucSlot, 0xFF);

    if (!argStream.HasErrors())
    {
        // Got a second argument too (slot)?
        ucSlot = ucSlot == 0xFF ? pPed->GetCurrentWeaponSlot() : ucSlot;

        CWeapon* pWeapon = pPed->GetWeapon((eWeaponSlot)ucSlot);
        if (pWeapon)
        {
            unsigned short usAmmo = static_cast<unsigned short>(pWeapon->GetAmmoInClip());
            lua_pushnumber(luaVM, usAmmo);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedTotalAmmo(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    unsigned char    ucSlot = 0xFF;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadNumber(ucSlot, 0xFF);

    if (!argStream.HasErrors())
    {
        // Got a second argument too (slot)?
        ucSlot = ucSlot == 0xFF ? pPed->GetCurrentWeaponSlot() : ucSlot;

        // Grab the ammo and return
        CWeapon* pWeapon = pPed->GetWeapon((eWeaponSlot)ucSlot);
        if (pWeapon)
        {
            // Keep server and client synced
            unsigned short usAmmo = 1;
            if (CWeaponNames::DoesSlotHaveAmmo(ucSlot))
                usAmmo = static_cast<unsigned short>(pWeapon->GetAmmoTotal());

            lua_pushnumber(luaVM, usAmmo);
            return 1;
        }
        else if (ucSlot < WEAPONSLOT_MAX && pPed->m_usWeaponAmmo[ucSlot])
        {
            // The ped musn't be streamed in, so we can get the stored value instead
            ushort usAmmo = 1;

            if (CWeaponNames::DoesSlotHaveAmmo(ucSlot))
                usAmmo = pPed->m_usWeaponAmmo[ucSlot];

            lua_pushnumber(luaVM, usAmmo);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedWeaponMuzzlePosition(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        CVector vecMuzzlePos;
        if (CStaticFunctionDefinitions::GetPedWeaponMuzzlePosition(*pPed, vecMuzzlePos))
        {
            lua_pushnumber(luaVM, vecMuzzlePos.fX);
            lua_pushnumber(luaVM, vecMuzzlePos.fY);
            lua_pushnumber(luaVM, vecMuzzlePos.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedOccupiedVehicle(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Grab his occupied vehicle
        CClientVehicle* pVehicle = pPed->GetOccupiedVehicle();
        if (pVehicle)
        {
            lua_pushelement(luaVM, pVehicle);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedOccupiedVehicleSeat(lua_State* luaVM)
{
    CClientPed* pPed = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        unsigned int uiVehicleSeat;
        if (CStaticFunctionDefinitions::GetPedOccupiedVehicleSeat(*pPed, uiVehicleSeat))
        {
            lua_pushnumber(luaVM, uiVehicleSeat);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedTask(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    SString          strPriority = "";
    unsigned int     uiTaskType = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadString(strPriority);
    argStream.ReadNumber(uiTaskType);

    if (!argStream.HasErrors())
    {
        // Any priority specified?
        if (strPriority != "")
        {
            // Primary or secondary task grabbed?
            bool bPrimary = false;
            if ((bPrimary = !stricmp(strPriority.c_str(), "primary")) || (!stricmp(strPriority.c_str(), "secondary")))
            {
                // Grab the taskname list and return it
                std::vector<SString> taskHierarchy;
                if (CStaticFunctionDefinitions::GetPedTask(*pPed, bPrimary, uiTaskType, taskHierarchy))
                {
                    for (uint i = 0; i < taskHierarchy.size(); i++)
                        lua_pushstring(luaVM, taskHierarchy[i]);
                    return taskHierarchy.size();
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedSimplestTask(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Grab his simplest task and return it
        const char* szTaskName = CStaticFunctionDefinitions::GetPedSimplestTask(*pPed);
        if (szTaskName)
        {
            lua_pushstring(luaVM, szTaskName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::IsPedDoingTask(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    SString          strTaskName = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadString(strTaskName);

    if (!argStream.HasErrors())
    {
        // Check whether he's doing that task or not
        bool bIsDoingTask;
        if (CStaticFunctionDefinitions::IsPedDoingTask(*pPed, strTaskName.c_str(), bIsDoingTask))
        {
            lua_pushboolean(luaVM, bIsDoingTask);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedTarget(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Grab his target element
        CClientEntity* pEntity = CStaticFunctionDefinitions::GetPedTarget(*pPed);
        if (pEntity)
        {
            lua_pushelement(luaVM, pEntity);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedTargetStart(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Grab his start aim position and return it
        CVector vecStart;
        pPed->GetShotData(&vecStart);

        lua_pushnumber(luaVM, vecStart.fX);
        lua_pushnumber(luaVM, vecStart.fY);
        lua_pushnumber(luaVM, vecStart.fZ);
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::OOP_GetPedTargetStart(lua_State* luaVM)
{
    CClientPed*      pPed = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    CVector vecStart;

    if (pPed->GetShotData(&vecStart))
    {
        lua_pushvector(luaVM, vecStart);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedTargetEnd(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Grab the ped end target position and return it
        CVector vecEnd;
        pPed->GetShotData(nullptr, &vecEnd);

        lua_pushnumber(luaVM, vecEnd.fX);
        lua_pushnumber(luaVM, vecEnd.fY);
        lua_pushnumber(luaVM, vecEnd.fZ);
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::OOP_GetPedTargetEnd(lua_State* luaVM)
{
    CClientPed*      pPed = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    CVector vecEnd;

    if (pPed->GetShotData(nullptr, &vecEnd))
    {
        lua_pushvector(luaVM, vecEnd);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedTargetCollision(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Grab his target collision and return it
        CVector vecCollision;
        if (CStaticFunctionDefinitions::GetPedTargetCollision(*pPed, vecCollision))
        {
            lua_pushnumber(luaVM, vecCollision.fX);
            lua_pushnumber(luaVM, vecCollision.fY);
            lua_pushnumber(luaVM, vecCollision.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::OOP_GetPedTargetCollision(lua_State* luaVM)
{
    CClientPed*      pPed = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    CVector vecCollision;
    if (CStaticFunctionDefinitions::GetPedTargetCollision(*pPed, vecCollision))
    {
        lua_pushvector(luaVM, vecCollision);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedArmor(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Grab the armor and return it
        float fArmor = pPed->GetArmor();
        lua_pushnumber(luaVM, fArmor);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedStat(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    unsigned short   usStat = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadNumber(usStat);

    if (!argStream.HasErrors())
    {
        // Check the stat
        if (usStat < NUM_PLAYER_STATS)
        {
            float fValue = pPed->GetStat(usStat);
            lua_pushnumber(luaVM, fValue);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::IsPedChoking(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Return whether he's choking or not
        lua_pushboolean(luaVM, pPed->IsChoking());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushnil(luaVM);
    return 1;
}

int CLuaPedDefs::IsPedDucked(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Grab his ducked state
        bool bDucked = pPed->IsDucked();
        lua_pushboolean(luaVM, bDucked);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushnil(luaVM);
    return 1;
}

int CLuaPedDefs::IsPedInVehicle(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Find out whether he's in a vehicle or not
        bool bInVehicle;
        if (CStaticFunctionDefinitions::IsPedInVehicle(*pPed, bInVehicle))
        {
            // Return that state
            lua_pushboolean(luaVM, bInVehicle);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushnil(luaVM);
    return 1;
}

int CLuaPedDefs::DoesPedHaveJetPack(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Find out whether he has a jetpack or not and return it
        bool bHasJetPack = pPed->HasJetPack();
        lua_pushboolean(luaVM, bHasJetPack);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::IsPedOnGround(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Find out whether he's on the ground or not and return it
        bool bOnGround = pPed->IsOnGround();
        lua_pushboolean(luaVM, bOnGround);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedContactElement(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        CClientEntity* pEntity = pPed->GetContactEntity();
        if (pEntity)
        {
            lua_pushelement(luaVM, pEntity);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedRotation(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        float fRotation = ConvertRadiansToDegrees(pPed->GetCurrentRotation());
        lua_pushnumber(luaVM, fRotation);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::CanPedBeKnockedOffBike(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        bool bCanBeKnockedOffBike = pPed->GetCanBeKnockedOffBike();
        lua_pushboolean(luaVM, bCanBeKnockedOffBike);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaPedDefs::SetElementBonePosition(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId, CVector position)
{
    CEntity* theEntity = entity->GetGameEntity();
    if (!theEntity)
        return false;
    return theEntity->SetBonePosition(static_cast<eBone>(boneId), position);
}

bool CLuaPedDefs::SetElementBoneRotation(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId, float yaw, float pitch, float roll)
{
    if (boneId > BONE_RIGHTFOOT)
        throw LuaFunctionError("Invalid bone ID", false);

    CEntity* theEntity = entity->GetGameEntity();
    if (!theEntity)
        return false;
    return theEntity->SetBoneRotation(static_cast<eBone>(boneId), yaw, pitch, roll);
}

bool CLuaPedDefs::SetElementBoneQuaternion(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId, float x, float y, float z, float w)
{
    if (boneId > BONE_RIGHTFOOT)
        throw LuaFunctionError("Invalid bone ID", false);

    CEntity* theEntity = entity->GetGameEntity();
    return theEntity ? theEntity->SetBoneRotationQuat(static_cast<eBone>(boneId), x, y, z, w) : false;
}

std::variant<bool, CLuaMultiReturn<float, float, float>> CLuaPedDefs::GetElementBonePosition(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId)
{
    CEntity* theEntity = entity->GetGameEntity();
    CVector  position;
    if (!theEntity || !theEntity->GetBonePosition(static_cast<eBone>(boneId), position))
        return false;

    return std::make_tuple(position.fX, position.fY, position.fZ);
}

std::variant<bool, CLuaMultiReturn<float, float, float>> CLuaPedDefs::GetElementBoneRotation(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId)
{
    if (boneId > BONE_RIGHTFOOT)
        throw LuaFunctionError("Invalid bone ID", false);

    float yaw = 0.0f, pitch = 0.0f, roll = 0.0f;
    CEntity* theEntity = entity->GetGameEntity();
    if (!theEntity || !theEntity->GetBoneRotation(static_cast<eBone>(boneId), yaw, pitch, roll))
        return false;

    return std::make_tuple(yaw, pitch, roll);
}

std::variant<bool, CLuaMultiReturn<float, float, float, float>> CLuaPedDefs::GetElementBoneQuaternion(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId)
{
    if (boneId > BONE_RIGHTFOOT)
        throw LuaFunctionError("Invalid bone ID", false);

    float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
    CEntity* theEntity = entity->GetGameEntity();
    if (!theEntity || !theEntity->GetBoneRotationQuat(static_cast<eBone>(boneId), x, y, z, w))
        return false;

    return std::make_tuple(x, y, z, w);
}

bool CLuaPedDefs::SetElementBoneMatrix(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId, CMatrix boneMatrix)
{
    CEntity* theEntity = entity->GetGameEntity();
    return theEntity ? theEntity->SetBoneMatrix(static_cast<eBone>(boneId), boneMatrix) : false;
}

std::variant<bool, std::array<std::array<float, 4>, 4>> CLuaPedDefs::GetElementBoneMatrix(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId)
{
    CEntity* theEntity = entity->GetGameEntity();
    if (theEntity)
    {
        RwMatrix* boneRwMatrix = theEntity->GetBoneRwMatrix(static_cast<eBone>(boneId));
        if (boneRwMatrix)
        {
            CMatrix matrix;
            g_pGame->GetRenderWare()->RwMatrixToCMatrix(*boneRwMatrix, matrix);
            return matrix.To4x4Array();
        }
    }
    return false;
}

bool CLuaPedDefs::UpdateElementRpHAnim(lua_State* const luaVM, CClientEntity* entity)
{
    CEntity* theEntity = entity->GetGameEntity();
    if (theEntity)
    {
        theEntity->UpdateRpHAnim();

        if (theEntity->GetModelIndex() == 0)            // CJ skin
        {
            RpClump* clump = theEntity->GetRpClump();
            if (clump)
            {
                ((void(__cdecl*)(RpClump*))0x5DF560)(clump);            // CPed::ShoulderBoneRotation
            }
        }
        return true;
    }
    return false;
}

int CLuaPedDefs::GetPedBonePosition(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    unsigned char    ucBone = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadNumber(ucBone);

    if (!argStream.HasErrors())
    {
        if (ucBone <= BONE_RIGHTFOOT)
        {
            eBone   bone = (eBone)ucBone;
            CVector vecPosition;
            if (CStaticFunctionDefinitions::GetPedBonePosition(*pPed, bone, vecPosition))
            {
                lua_pushnumber(luaVM, vecPosition.fX);
                lua_pushnumber(luaVM, vecPosition.fY);
                lua_pushnumber(luaVM, vecPosition.fZ);
                return 3;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::OOP_GetPedBonePosition(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    unsigned char    ucBone = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadNumber(ucBone);

    if (!argStream.HasErrors())
    {
        if (ucBone <= BONE_RIGHTFOOT)
        {
            eBone   bone = (eBone)ucBone;
            CVector vecPosition;
            if (CStaticFunctionDefinitions::GetPedBonePosition(*pPed, bone, vecPosition))
            {
                lua_pushvector(luaVM, vecPosition);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::SetPedWeaponSlot(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pElement = NULL;
    int              iSlot = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(iSlot);

    if (!argStream.HasErrors())
    {
        // Valid slot?
        if (iSlot >= 0)
        {
            // Set his slot
            if (CStaticFunctionDefinitions::SetPedWeaponSlot(*pElement, iSlot))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GivePedWeapon(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    eWeaponType      weaponType;
    ushort           usAmmo = 0;
    bool             bSetAsCurrent = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadEnumStringOrNumber(weaponType);
    argStream.ReadNumber(usAmmo, 30);
    argStream.ReadBool(bSetAsCurrent, false);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GivePedWeapon(*pEntity, weaponType, usAmmo, bSetAsCurrent))
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

int CLuaPedDefs::IsPedReloadingWeapon(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pPed->IsReloadingWeapon());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedClothes(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    unsigned char    ucType = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadNumber(ucType);

    if (!argStream.HasErrors())
    {
        SString strTexture, strModel;
        if (CStaticFunctionDefinitions::GetPedClothes(*pPed, ucType, strTexture, strModel))
        {
            lua_pushstring(luaVM, strTexture);
            lua_pushstring(luaVM, strModel);
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedControlState(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = CStaticFunctionDefinitions::GetLocalPlayer();
    SString          strControl = "";
    CScriptArgReader argStream(luaVM);

    if (argStream.NextIsUserData())
    {
        argStream.ReadUserData(pPed);
    }
    argStream.ReadString(strControl);

    if (!argStream.HasErrors())
    {
        bool bState;
        if (CStaticFunctionDefinitions::GetPedControlState(*pPed, strControl, bState))
        {
            lua_pushboolean(luaVM, bState);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedAnalogControlState(lua_State* luaVM)
{
    SString          strControlState = "";
    float            fState = 0.0f;
    CClientPed*      pPed = NULL;
    bool             bRawInput;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadString(strControlState);
    argStream.ReadBool(bRawInput, false);

    if (!argStream.HasErrors())
    {
        float fState;
        if (CStaticFunctionDefinitions::GetPedAnalogControlState(*pPed, strControlState, fState, bRawInput))
        {
            lua_pushnumber(luaVM, fState);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::IsPedDoingGangDriveby(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        bool bDoingGangDriveby;
        if (CStaticFunctionDefinitions::IsPedDoingGangDriveby(*pPed, bDoingGangDriveby))
        {
            lua_pushboolean(luaVM, bDoingGangDriveby);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedFightingStyle(lua_State* luaVM)
{
    CClientPed*      pPed;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        unsigned char ucStyle;
        if (CStaticFunctionDefinitions::GetPedFightingStyle(*pPed, ucStyle))
        {
            lua_pushnumber(luaVM, ucStyle);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::SetPedAnalogControlState(lua_State* luaVM)
{
    SString          strControlState = "";
    float            fState = 0.0f;
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadString(strControlState);
    argStream.ReadNumber(fState);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedAnalogControlState(*pEntity, strControlState, fState))
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

int CLuaPedDefs::GetPedAnimation(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        SString strBlockName, strAnimName;
        if (pPed->GetRunningAnimationName(strBlockName, strAnimName))
        {
            const SAnimationCache& animationCache = pPed->GetAnimationCache();
            lua_pushstring(luaVM, strBlockName);
            lua_pushstring(luaVM, strAnimName);
            lua_newtable(luaVM);
            lua_pushinteger(luaVM, animationCache.iTime);
            lua_setfield(luaVM, -2, "time");
            lua_pushboolean(luaVM, animationCache.bLoop);
            lua_setfield(luaVM, -2, "loop");
            lua_pushboolean(luaVM, animationCache.bUpdatePosition);
            lua_setfield(luaVM, -2, "updatePosition");
            lua_pushboolean(luaVM, animationCache.bInterruptable);
            lua_setfield(luaVM, -2, "interruptable");
            lua_pushboolean(luaVM, animationCache.bFreezeLastFrame);
            lua_setfield(luaVM, -2, "freezeLastFrame");
            lua_pushinteger(luaVM, animationCache.iBlend);
            lua_setfield(luaVM, -2, "blendTime");
            lua_pushboolean(luaVM, pPed->IsTaskToBeRestoredOnAnimEnd());
            lua_setfield(luaVM, -2, "restoreTaskOnAnimEnd");
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedMoveState(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        std::string strMoveState;
        if (CStaticFunctionDefinitions::GetPedMoveState(*pPed, strMoveState))
        {
            lua_pushstring(luaVM, strMoveState.c_str());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::GetPedMoveAnim(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        unsigned int iMoveAnim;
        if (CStaticFunctionDefinitions::GetPedMoveAnim(*pPed, iMoveAnim))
        {
            lua_pushnumber(luaVM, iMoveAnim);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::IsPedHeadless(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        bool bHeadless;
        if (CStaticFunctionDefinitions::IsPedHeadless(*pPed, bHeadless))
        {
            lua_pushboolean(luaVM, bHeadless);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::IsPedFrozen(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        bool bFrozen;
        if (CStaticFunctionDefinitions::IsPedFrozen(*pPed, bFrozen))
        {
            lua_pushboolean(luaVM, bFrozen);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::IsPedFootBloodEnabled(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        bool bHasFootBlood = false;
        if (CStaticFunctionDefinitions::IsPedFootBloodEnabled(*pPed, bHasFootBlood))
        {
            lua_pushboolean(luaVM, bHasFootBlood);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaPedDefs::IsPedBleeding(CClientPed* pPed)
{
    return pPed->IsBleeding();
}

int CLuaPedDefs::GetPedCameraRotation(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        float fRotation = 0.0f;
        if (CStaticFunctionDefinitions::GetPedCameraRotation(*pPed, fRotation))
        {
            lua_pushnumber(luaVM, fRotation);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::IsPedOnFire(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        bool bOnFire;
        if (CStaticFunctionDefinitions::IsPedOnFire(*pPed, bOnFire))
        {
            lua_pushboolean(luaVM, bOnFire);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::SetPedOnFire(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    bool             bOnFire = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bOnFire);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedOnFire(*pEntity, bOnFire))
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

int CLuaPedDefs::WarpPedIntoVehicle(lua_State* luaVM)
{
    //  warpPedIntoVehicle ( element ped, element vehicle, int seat )
    CClientPed*     pPed;
    CClientVehicle* pVehicle;
    uint            uiSeat;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(uiSeat, 0);

    MinClientReqCheck(argStream, MIN_CLIENT_REQ_REMOVEPEDFROMVEHICLE_CLIENTSIDE, "function is being called client side");

    if (!argStream.HasErrors())
    {
        if (!pPed->IsLocalEntity() || !pVehicle->IsLocalEntity())
            argStream.SetCustomError("This client side function will only work with client created peds and vehicles");
    }

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::WarpPedIntoVehicle(pPed, pVehicle, uiSeat))
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

int CLuaPedDefs::OOP_WarpPedIntoVehicle(lua_State* luaVM)
{
    //  ped.vehicle = element vehicle
    //  ped.vehicle = nil
    CClientPed*     pPed;
    CClientVehicle* pVehicle;
    uint            uiSeat = 0;

    CScriptArgReader argStream(luaVM);

    argStream.ReadUserData(pPed);
    argStream.ReadUserData(pVehicle, NULL);
    if (pVehicle != NULL)
    {
        MinClientReqCheck(argStream, MIN_CLIENT_REQ_WARPPEDINTOVEHICLE_CLIENTSIDE, "function is being called client side");
        if (!argStream.HasErrors())
        {
            if (!pPed->IsLocalEntity() || !pVehicle->IsLocalEntity())
                argStream.SetCustomError("This client side function will only work with client created peds and vehicles");
        }

        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::WarpPedIntoVehicle(pPed, pVehicle, uiSeat))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }
    else
    {
        if (!argStream.HasErrors())
        {
            if (!pPed->IsLocalEntity())
                argStream.SetCustomError("This client side function will only work with client created peds");
        }

        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::RemovePedFromVehicle(pPed))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);

    return 1;
}

int CLuaPedDefs::RemovePedFromVehicle(lua_State* luaVM)
{
    //  removePedFromVehicle ( element ped )
    CClientPed* pPed;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    MinClientReqCheck(argStream, MIN_CLIENT_REQ_WARPPEDINTOVEHICLE_CLIENTSIDE, "function is being called client side");

    if (!argStream.HasErrors())
    {
        if (!pPed->IsLocalEntity())
            argStream.SetCustomError("This client side function will only work with client created peds");
    }

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::RemovePedFromVehicle(pPed))
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

int CLuaPedDefs::GetPedOxygenLevel(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        float fOxygen;
        if (CStaticFunctionDefinitions::GetPedOxygenLevel(*pPed, fOxygen))
        {
            lua_pushnumber(luaVM, fOxygen);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::IsPedDead(lua_State* luaVM)
{
    //  bool isPedDead ( ped thePed )
    CClientPed*      pPed;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPed);

    if (!argStream.HasErrors())
    {
        // Grab his dead state and return it
        bool bDead = pPed->IsDead() || pPed->IsDying();
        lua_pushboolean(luaVM, bDead);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushnil(luaVM);
    return 1;
}

int CLuaPedDefs::AddPedClothes(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    SString          strTexture = "", strModel = "";
    unsigned char    ucType = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadString(strTexture);
    argStream.ReadString(strModel);
    argStream.ReadNumber(ucType);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::AddPedClothes(*pEntity, strTexture, strModel, ucType))
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

int CLuaPedDefs::RemovePedClothes(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    unsigned char    ucType = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(ucType);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::RemovePedClothes(*pEntity, ucType))
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

int CLuaPedDefs::SetPedControlState(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = CStaticFunctionDefinitions::GetLocalPlayer();
    SString          strControl = "";
    bool             bState = false;
    CScriptArgReader argStream(luaVM);

    if (argStream.NextIsUserData())
    {
        argStream.ReadUserData(pEntity);
    }
    argStream.ReadString(strControl);
    argStream.ReadBool(bState);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedControlState(*pEntity, strControl, bState))
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

int CLuaPedDefs::SetPedDoingGangDriveby(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    bool             bDoingGangDriveby = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bDoingGangDriveby);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedDoingGangDriveby(*pEntity, bDoingGangDriveby))
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

bool CLuaPedDefs::SetPedFightingStyle(CClientEntity* const entity, const unsigned int style)
{
    // Is valid style?
    if (style < 4 || style > 16)
        throw std::invalid_argument("Style can only be between 4 and 16");

    return CStaticFunctionDefinitions::SetPedFightingStyle(*entity, style);
}

int CLuaPedDefs::SetPedLookAt(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CVector          vecPosition;
    int              iTime = 3000;
    int              iBlend = 1000;
    CClientEntity*   pTarget = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(iTime, 3000);
    if (argStream.NextIsUserData())
    {
        argStream.ReadUserData(pTarget);
    }
    else
    {
        argStream.ReadNumber(iBlend, 1000);
        argStream.ReadUserData(pTarget, NULL);
    }

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedLookAt(*pEntity, vecPosition, iTime, iBlend, pTarget))
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

int CLuaPedDefs::SetPedHeadless(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    bool             bHeadless = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bHeadless);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedHeadless(*pEntity, bHeadless))
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

int CLuaPedDefs::SetPedFrozen(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    bool             bFrozen = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bFrozen);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedFrozen(*pEntity, bFrozen))
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

int CLuaPedDefs::SetPedFootBloodEnabled(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    bool             bHasFootBlood = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bHasFootBlood);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedFootBloodEnabled(*pEntity, bHasFootBlood))
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

bool CLuaPedDefs::SetPedBleeding(CClientPed* ped, bool bleeding)
{
    ped->SetBleeding(bleeding);
    return true;
}

int CLuaPedDefs::SetPedCameraRotation(lua_State* luaVM)
{
    //  bool setPedCameraRotation ( ped thePed, float cameraRotation )
    CClientEntity* pEntity;
    float          fRotation;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(fRotation);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedCameraRotation(*pEntity, fRotation))
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

int CLuaPedDefs::SetPedAimTarget(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CVector          vecTarget;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadVector3D(vecTarget);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedAimTarget(*pEntity, vecTarget))
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

int CLuaPedDefs::SetPedStat(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    unsigned short   usStat = 0;
    float            fValue = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(usStat);
    argStream.ReadNumber(fValue);

    if (!argStream.HasErrors())
    {
        // Check the stat and value
        if (usStat > NUM_PLAYER_STATS - 1 || fValue < 0.0f || fValue > 1000.0f)
            argStream.SetCustomError("Stat must be 0 to 342 and value must be 0 to 1000.");
        else if (CStaticFunctionDefinitions::SetPedStat(*pEntity, usStat, fValue))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::KillPed(lua_State* luaVM)
{
    CClientEntity* pEntity = NULL;
    CClientEntity* pKiller = NULL;
    unsigned char  ucKillerWeapon;
    unsigned char  ucBodyPart;
    bool           bStealth;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadUserData(pKiller, NULL);
    argStream.ReadNumber(ucKillerWeapon, 0xFF);
    argStream.ReadNumber(ucBodyPart, 0xFF);
    argStream.ReadBool(bStealth, false);

    if (!argStream.HasErrors())
        if (!pEntity->IsLocalEntity())
            argStream.SetCustomError("This client side function will only work with client created peds");

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::KillPed(*pEntity, pKiller, ucKillerWeapon, ucBodyPart, bStealth))
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

int CLuaPedDefs::SetPedRotation(lua_State* luaVM)
{
    //  setPedRotation ( element ped, float rotation [, bool fixPedRotation = false ] )
    CClientEntity* pEntity;
    float          fRotation;
    bool           bNewWay;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(fRotation);
    argStream.ReadBool(bNewWay, false);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedRotation(*pEntity, fRotation, bNewWay))
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

int CLuaPedDefs::SetPedCanBeKnockedOffBike(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    bool             bCanBeKnockedOffBike = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bCanBeKnockedOffBike);

    if (!argStream.HasErrors())
    {
        // Set the new rotation
        if (CStaticFunctionDefinitions::SetPedCanBeKnockedOffBike(*pEntity, bCanBeKnockedOffBike))
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

int CLuaPedDefs::SetPedAnimation(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    bool           bDummy;
    SString        strBlockName = "";
    SString        strAnimName = "";
    int            iTime = -1;
    int            iBlend = 250;
    bool           bLoop = true;
    bool           bUpdatePosition = true;
    bool           bInterruptable = true;
    bool           bFreezeLastFrame = true;
    bool           bTaskToBeRestoredOnAnimEnd = false;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    if (argStream.NextIsBool())
        argStream.ReadBool(bDummy);            // Wiki used setPedAnimation(source,false) as an example
    else if (argStream.NextIsNil())
        argStream.m_iIndex++;            // Wiki docs said blockName could be nil
    else
        argStream.ReadString(strBlockName, "");
    argStream.ReadString(strAnimName, "");
    argStream.ReadNumber(iTime, -1);
    argStream.ReadBool(bLoop, true);
    argStream.ReadBool(bUpdatePosition, true);
    argStream.ReadBool(bInterruptable, true);
    argStream.ReadBool(bFreezeLastFrame, true);
    argStream.ReadNumber(iBlend, 250);
    argStream.ReadBool(bTaskToBeRestoredOnAnimEnd, false);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedAnimation(*pEntity, strBlockName == "" ? NULL : strBlockName.c_str(),
                                                        strAnimName == "" ? NULL : strAnimName.c_str(), iTime, iBlend, bLoop, bUpdatePosition, bInterruptable,
                                                        bFreezeLastFrame))
        {
            CClientPed* pPed = static_cast<CClientPed*>(pEntity);
            if (pPed->IsDucked())
            {
                pPed->SetTaskTypeToBeRestoredOnAnimEnd((eTaskType)TASK_SIMPLE_DUCK);
            }
            else
            {
                bTaskToBeRestoredOnAnimEnd = false;
            }

            pPed->SetTaskToBeRestoredOnAnimEnd(bTaskToBeRestoredOnAnimEnd);
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

int CLuaPedDefs::SetPedAnimationProgress(lua_State* luaVM)
{
    //  bool setPedAnimationProgress ( ped thePed, string animName, float progress )
    CClientEntity* pEntity;
    SString        strAnimName;
    float          fProgress;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadString(strAnimName, "");
    argStream.ReadNumber(fProgress, 0.0f);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedAnimationProgress(*pEntity, strAnimName, fProgress))
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

float CLuaPedDefs::GetPedAnimationProgress(CClientPed* ped)
{
    CTask*       currentTask = ped->GetTaskManager()->GetActiveTask();
    std::int32_t type = currentTask->GetTaskType();

    // check if animation (task type is 401)
    if (type != 401)
        return -1.0f;

    auto* animation = dynamic_cast<CTaskSimpleRunNamedAnim*>(currentTask);
    if (!animation)
        return -1.0f;

    auto animAssociation = g_pGame->GetAnimManager()->RpAnimBlendClumpGetAssociation(ped->GetClump(), animation->GetAnimName());
    if (!animAssociation)
        return -1.0f;

    return animAssociation->GetCurrentProgress() / animAssociation->GetLength();
}

float CLuaPedDefs::GetPedAnimationSpeed(CClientPed* ped)
{
    CTask*       currentTask = ped->GetTaskManager()->GetActiveTask();
    std::int32_t type = currentTask->GetTaskType();

    // check if animation (task type is 401)
    if (type != 401)
        return -1.0f;

    auto* animation = dynamic_cast<CTaskSimpleRunNamedAnim*>(currentTask);
    if (!animation)
        return -1.0f;

    auto animAssociation = g_pGame->GetAnimManager()->RpAnimBlendClumpGetAssociation(ped->GetClump(), animation->GetAnimName());
    if (!animAssociation)
        return -1.0f;

    return animAssociation->GetCurrentSpeed();
}

float CLuaPedDefs::GetPedAnimationLength(CClientPed* ped)
{
    CTask*       currentTask = ped->GetTaskManager()->GetActiveTask();
    std::int32_t type = currentTask->GetTaskType();

    // check if animation (task type is 401)
    if (type != 401)
        return -1.0f;

    auto* animation = dynamic_cast<CTaskSimpleRunNamedAnim*>(currentTask);
    if (!animation)
        return -1.0f;

    auto animAssociation = g_pGame->GetAnimManager()->RpAnimBlendClumpGetAssociation(ped->GetClump(), animation->GetAnimName());
    if (!animAssociation)
        return -1.0f;

    return animAssociation->GetLength();
}

int CLuaPedDefs::SetPedAnimationSpeed(lua_State* luaVM)
{
    CClientEntity* pEntity;
    SString        strAnimName;
    float          fSpeed;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadString(strAnimName, "");
    argStream.ReadNumber(fSpeed, 1.0f);

    if (!argStream.HasErrors())
    {
        if (!strAnimName.empty() && fSpeed >= 0.0f && fSpeed <= 10.0f)
        {
            if (CStaticFunctionDefinitions::SetPedAnimationSpeed(*pEntity, strAnimName, fSpeed))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::SetPedMoveAnim(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    unsigned int     uiMoveAnim = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(uiMoveAnim);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedMoveAnim(*pEntity, uiMoveAnim))
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

bool CLuaPedDefs::SetPedArmor(CClientPed* const ped, const float armor)
{
    ped->SetArmor(armor);
    return true;
}

int CLuaPedDefs::SetPedOxygenLevel(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    float            fOxygen = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(fOxygen);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetPedOxygenLevel(*pEntity, fOxygen))
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

int CLuaPedDefs::CreatePed(lua_State* luaVM)
{
    // Verify the argument
    CClientPed*      pPed = NULL;
    unsigned long    ulModel = 0;
    CVector          vecPosition;
    float            fRotation = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ulModel);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(fRotation, 0.0f);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it
                CClientPed* pPed = CStaticFunctionDefinitions::CreatePed(*pResource, ulModel, vecPosition, fRotation);
                if (pPed)
                {
                    // Return it
                    lua_pushelement(luaVM, pPed);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPedDefs::DetonateSatchels(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::DetonateSatchels())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaPedDefs::SetPedEnterVehicle(CClientPed* pPed, std::optional<CClientVehicle*> pOptVehicle, std::optional<bool> bOptPassenger)
{
    CClientVehicle* pVehicle = pOptVehicle.value_or(nullptr);
    bool            bPassenger = bOptPassenger.value_or(false);
    return pPed->EnterVehicle(pVehicle, bPassenger);
}

bool CLuaPedDefs::SetPedExitVehicle(CClientPed* pPed)
{
    return pPed->ExitVehicle();
}
