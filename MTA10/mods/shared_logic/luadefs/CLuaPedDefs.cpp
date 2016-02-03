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
#define MIN_CLIENT_REQ_REMOVEPEDFROMVEHICLE_CLIENTSIDE  "1.3.0-9.04482"
#define MIN_CLIENT_REQ_WARPPEDINTOVEHICLE_CLIENTSIDE    "1.3.0-9.04482"
#define MIN_CLIENT_REQ_WEAPON_PROPERTY_FLAG             "1.3.5-9.06139"

void CLuaPedDefs::LoadFunctions ( void ) {
    CLuaCFunctions::AddFunction ( "createPed", CreatePed );
    CLuaCFunctions::AddFunction ( "detonateSatchels", DetonateSatchels );
    CLuaCFunctions::AddFunction ( "killPed", KillPed );

    CLuaCFunctions::AddFunction ( "getPedVoice", GetPedVoice );
    CLuaCFunctions::AddFunction ( "setPedVoice", SetPedVoice );
    CLuaCFunctions::AddFunction ( "getPedRotation", GetPedRotation );
    CLuaCFunctions::AddFunction ( "canPedBeKnockedOffBike", CanPedBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "getPedContactElement", GetPedContactElement );
    CLuaCFunctions::AddFunction ( "isPedInVehicle", IsPedInVehicle );
    CLuaCFunctions::AddFunction ( "doesPedHaveJetPack", DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPedOnGround", IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPedTask", GetPedTask );
    CLuaCFunctions::AddFunction ( "getPedSimplestTask", GetPedSimplestTask );
    CLuaCFunctions::AddFunction ( "isPedDoingTask", IsPedDoingTask );
    CLuaCFunctions::AddFunction ( "getPedTarget", GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPedTargetStart", GetPedTargetStart );
    CLuaCFunctions::AddFunction ( "getPedTargetEnd", GetPedTargetEnd );
    CLuaCFunctions::AddFunction ( "getPedTargetRange", GetPedTargetRange );
    CLuaCFunctions::AddFunction ( "getPedTargetCollision", GetPedTargetCollision );
    CLuaCFunctions::AddFunction ( "getPedWeaponSlot", GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPedWeapon", GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPedAmmoInClip", GetPedAmmoInClip );
    CLuaCFunctions::AddFunction ( "getPedTotalAmmo", GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPedOccupiedVehicle", GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPedOccupiedVehicleSeat", GetPedOccupiedVehicleSeat );
    CLuaCFunctions::AddFunction ( "getPedArmor", GetPedArmor );
    CLuaCFunctions::AddFunction ( "isPedChoking", IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPedDucked", IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPedStat", GetPedStat );
    CLuaCFunctions::AddFunction ( "getPedBonePosition", GetPedBonePosition );
    CLuaCFunctions::AddFunction ( "getPedClothes", GetPedClothes );
    CLuaCFunctions::AddFunction ( "getPedControlState", GetPedControlState );
    CLuaCFunctions::AddFunction ( "getPedAnalogControlState", GetPedAnalogControlState );
    CLuaCFunctions::AddFunction ( "isPedDead", IsPedDead );

    CLuaCFunctions::AddFunction ( "isPedDoingGangDriveby", IsPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "getPedAnimation", GetPedAnimation );
    CLuaCFunctions::AddFunction ( "getPedMoveState", GetPedMoveState );
    CLuaCFunctions::AddFunction ( "getPedWalkingStyle", GetPedMoveAnim );
    CLuaCFunctions::AddFunction ( "isPedHeadless", IsPedHeadless );
    CLuaCFunctions::AddFunction ( "isPedFrozen", IsPedFrozen );
    CLuaCFunctions::AddFunction ( "isPedFootBloodEnabled", IsPedFootBloodEnabled );
    CLuaCFunctions::AddFunction ( "getPedCameraRotation", GetPedCameraRotation );
    CLuaCFunctions::AddFunction ( "getPedOxygenLevel", GetPedOxygenLevel );

    CLuaCFunctions::AddFunction ( "setPedWeaponSlot", SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "setPedRotation", SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPedCanBeKnockedOffBike", SetPedCanBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "setPedAnimation", SetPedAnimation );
    CLuaCFunctions::AddFunction ( "setPedAnimationProgress", SetPedAnimationProgress );
    CLuaCFunctions::AddFunction ( "setPedWalkingStyle", SetPedMoveAnim );
    CLuaCFunctions::AddFunction ( "addPedClothes", AddPedClothes );
    CLuaCFunctions::AddFunction ( "removePedClothes", RemovePedClothes );
    CLuaCFunctions::AddFunction ( "setPedControlState", SetPedControlState );
    CLuaCFunctions::AddFunction ( "setPedAnalogControlState", SetPedAnalogControlState );
    CLuaCFunctions::AddFunction ( "setPedDoingGangDriveby", SetPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "setPedLookAt", SetPedLookAt );
    CLuaCFunctions::AddFunction ( "setPedHeadless", SetPedHeadless );
    CLuaCFunctions::AddFunction ( "setPedFrozen", SetPedFrozen );
    CLuaCFunctions::AddFunction ( "setPedFootBloodEnabled", SetPedFootBloodEnabled );
    CLuaCFunctions::AddFunction ( "setPedCameraRotation", SetPedCameraRotation );
    CLuaCFunctions::AddFunction ( "setPedAimTarget", SetPedAimTarget );
    CLuaCFunctions::AddFunction ( "getWeaponProperty", GetWeaponProperty );
    CLuaCFunctions::AddFunction ( "getOriginalWeaponProperty", GetOriginalWeaponProperty );
    CLuaCFunctions::AddFunction ( "warpPedIntoVehicle", WarpPedIntoVehicle );
    CLuaCFunctions::AddFunction ( "removePedFromVehicle", RemovePedFromVehicle );
    CLuaCFunctions::AddFunction ( "setPedOxygenLevel", SetPedOxygenLevel );
    CLuaCFunctions::AddFunction ( "givePedWeapon", GivePedWeapon );
}

void CLuaPedDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createPed" );
    lua_classfunction ( luaVM, "kill", "killPed" );

    lua_classfunction ( luaVM, "getBodyPartName", "getBodyPartName" );
    lua_classfunction ( luaVM, "getClothesTypeName", "getClothesTypeName" );
    lua_classfunction ( luaVM, "getValidModels", "getValidPedModels" );
    lua_classfunction ( luaVM, "getTypeIndexFromClothes", "getTypeIndexFromClothes" );
    lua_classfunction ( luaVM, "getClothesByTypeIndex", "getClothesByTypeIndex" );
    lua_classvariable ( luaVM, "validModels", NULL, "getValidPedModels" );
    //lua_classvariable ( luaVM, "clothesTypeName", NULL, "getClothesTypeName" ); table
    //lua_classvariable ( luaVM, "bodyPartName", NULL, "getBodyPartName" ); table

    lua_classfunction ( luaVM, "canBeKnockedOffBike", "canPedBeKnockedOffBike" );
    lua_classfunction ( luaVM, "doesHaveJetPack", "doesPedHaveJetPack" );
    lua_classfunction ( luaVM, "getAmmoInClip", "getPedAmmoInClip" );
    lua_classfunction ( luaVM, "getAnalogControlState", "getPedAnalogControlState" );
    lua_classfunction ( luaVM, "getAnimation", "getPedAnimation" );
    lua_classfunction ( luaVM, "getArmor", "getPedArmor" );
    lua_classfunction ( luaVM, "getClothes", "getPedClothes" );
    lua_classfunction ( luaVM, "addClothes", "addPedClothes" );
    lua_classfunction ( luaVM, "removeClothes", "removePedClothes" );
    lua_classfunction ( luaVM, "getContactElement", "getPedContactElement" );
    lua_classfunction ( luaVM, "getControlState", "getPedControlState" );
    lua_classfunction ( luaVM, "getMoveState", "getPedMoveState" );
    lua_classfunction ( luaVM, "getOccupiedVehicle", "getPedOccupiedVehicle" );
    lua_classfunction ( luaVM, "getOccupiedVehicleSeat", "getPedOccupiedVehicleSeat" );
    lua_classfunction ( luaVM, "getOxygenLevel", "getPedOxygenLevel" );
    lua_classfunction ( luaVM, "getStat", "getPedStat" );
    lua_classfunction ( luaVM, "getTarget", "getPedTarget" );
    lua_classfunction ( luaVM, "getTargetCollision", "getPedTargetCollision" );
    lua_classfunction ( luaVM, "getSimplestTask", "getPedSimplestTask" );
    lua_classfunction ( luaVM, "getTask", "getPedTask" );
    lua_classfunction ( luaVM, "getTotalAmmo", "getPedTotalAmmo" );
    lua_classfunction ( luaVM, "getVoice", "getPedVoice" );
    lua_classfunction ( luaVM, "getWeapon", "getPedWeapon" );
    lua_classfunction ( luaVM, "isChocking", "isPedChoking" );
    lua_classfunction ( luaVM, "isDoingGangDriveby", "isPedDoingGangDriveby" );
    lua_classfunction ( luaVM, "isDoingTask", "isPedDoingTask" );
    lua_classfunction ( luaVM, "isDucked", "isPedDucked" );
    lua_classfunction ( luaVM, "isHeadless", "isPedHeadless" );
    lua_classfunction ( luaVM, "isInVehicle", "isPedInVehicle" );
    lua_classfunction ( luaVM, "isOnFire", "isPedOnFire" );
    lua_classfunction ( luaVM, "isOnGround", "isPedOnGround" );
    lua_classfunction ( luaVM, "isTargetingMarkerEnabled", "isPedTargetingMarkerEnabled" );
    lua_classfunction ( luaVM, "isDead", "isPedDead" );
    lua_classfunction ( luaVM, "setFootBloodEnabled", "setPedFootBloodEnabled" );
    lua_classfunction ( luaVM, "getTargetEnd", "getPedTargetEnd" );
    lua_classfunction ( luaVM, "getTargetStart", "getPedTargetStart" );
    lua_classfunction ( luaVM, "getWeaponMuzzlePosition", "getPedWeaponMuzzlePosition" );
    lua_classfunction ( luaVM, "getBonePosition", "getPedBonePosition" );
    lua_classfunction ( luaVM, "getCameraRotation", "getPedCameraRotation" );
    lua_classfunction ( luaVM, "getWeaponSlot", "getPedWeaponSlot" );
    lua_classfunction ( luaVM, "getWalkingStyle", "getPedWalkingStyle" );

    lua_classfunction ( luaVM, "setCanBeKnockedOffBike", "setPedCanBeKnockedOffBike" );
    lua_classfunction ( luaVM, "setAnalogControlState", "setPedAnalogControlState" );
    lua_classfunction ( luaVM, "setAnimation", "setPedAnimation" );
    lua_classfunction ( luaVM, "setAnimationProgress", "setPedAnimationProgress" );
    lua_classfunction ( luaVM, "setCameraRotation", "setPedCameraRotation" );
    lua_classfunction ( luaVM, "setControlState", "setPedControlState" );
    lua_classfunction ( luaVM, "warpIntoVehicle", "warpPedIntoVehicle" );
    lua_classfunction ( luaVM, "setOxygenLevel", "setPedOxygenLevel" );
    lua_classfunction ( luaVM, "setWeaponSlot", "setPedWeaponSlot" );
    lua_classfunction ( luaVM, "setDoingGangDriveby", "setPedDoingGangDriveby" );
    lua_classfunction ( luaVM, "setHeadless", "setPedHeadless" );
    lua_classfunction ( luaVM, "setOnFire", "setPedOnFire" );
    lua_classfunction ( luaVM, "setTargetingMarkerEnabled", "setPedTargetingMarkerEnabled" );
    lua_classfunction ( luaVM, "setVoice", "setPedVoice" );
    lua_classfunction ( luaVM, "removeFromVehicle", "removePedFromVehicle" );
    lua_classfunction ( luaVM, "setAimTarget", "setPedAimTarget" );
    lua_classfunction ( luaVM, "setLookAt", "setPedLookAt" );
    lua_classfunction ( luaVM, "setWalkingStyle", "setPedWalkingStyle" );
    lua_classfunction ( luaVM, "giveWeapon", "givePedWeapon" );

    lua_classvariable ( luaVM, "vehicle", OOP_WarpPedIntoVehicle, GetPedOccupiedVehicle );
    lua_classvariable ( luaVM, "vehicleSeat", NULL, "getPedOccupiedVehicleSeat" );
    lua_classvariable ( luaVM, "canBeKnockedOffBike", "setPedCanBeKnockedOffBike", "canPedBeKnockedOffBike" );
    lua_classvariable ( luaVM, "hasJetPack", NULL, "doesPedHaveJetPack" );
    lua_classvariable ( luaVM, "armor", NULL, "getPedArmor" );
    lua_classvariable ( luaVM, "cameraRotation", "setPedCameraRotation", "getPedCameraRotation" );
    lua_classvariable ( luaVM, "contactElement", NULL, "getPedContactElement" );
    lua_classvariable ( luaVM, "moveState", NULL, "getPedMoveState" );
    lua_classvariable ( luaVM, "oxygenLevel", "setPedOxygenLevel", "getPedOxygenLevel" );
    lua_classvariable ( luaVM, "target", NULL, "getPedTarget" );
    lua_classvariable ( luaVM, "simplestTask", NULL, "getPedSimplestTask" );
    lua_classvariable ( luaVM, "choking", NULL, "isPedChoking" );
    lua_classvariable ( luaVM, "doingGangDriveby", "setPedDoingGangDriveby", "isPedDoingGangDriveby" );
    lua_classvariable ( luaVM, "ducked", NULL, "isPedDucked" );
    lua_classvariable ( luaVM, "headless", "setPedHeadless", "isPedHeadless" );
    lua_classvariable ( luaVM, "inVehicle", NULL, "isPedInVehicle" );
    lua_classvariable ( luaVM, "onFire", "setPedOnFire", "isPedOnFire" );
    lua_classvariable ( luaVM, "onGround", NULL, "isPedOnGround" );
    lua_classvariable ( luaVM, "dead", NULL, "isPedDead" );
    lua_classvariable ( luaVM, "targetingMarker", "setPedTargetingMarkerEnabled", "isPedTargetingMarkerEnabled" );
    lua_classvariable ( luaVM, "footBlood", "setPedFootBloodEnabled", NULL );
    lua_classvariable ( luaVM, "targetCollision", NULL, "getPedTargetCollision" );
    lua_classvariable ( luaVM, "targetEnd", NULL, "getPedTargetEnd" );
    lua_classvariable ( luaVM, "targetStart", NULL, "getPedTargetStart" );
    // lua_classvariable ( luaVM, "muzzlePosition", NULL, "getPedWeaponMuzzlePosition" ); // TODO: needs to return a vector3 for oop
    lua_classvariable ( luaVM, "weaponSlot", "setPedWeaponSlot", "getPedWeaponSlot" );
    lua_classvariable ( luaVM, "walkingStyle", "setPedWalkingStyle", "getPedWalkingStyle" );

    //lua_classvariable ( luaVM, "ammoInClip", NULL, CLuaOOPDefs::GetPedAmmoInClip ); // .ammoInClip["slot"] (readonly)
    //lua_classvariable ( luaVM, "analogControlState", CLuaOOPDefs::SetPedAnalogControlState, CLuaOOPDefs::GetPedAnalogControlState ); //TODO: .analogControlState["control"] = value
    //lua_classvariable ( luaVM, "controlState", CLuaOOPDefs::SetPedControlState, CLuaOOPDefs::GetPedControlState ); // TODO: .controlState["control"] = value
    //lua_classvariable ( luaVM, "stats", NULL, CLuaOOPDefs::GetPedStat ); // table (readonly)
    //lua_classvariable ( luaVM, "doingTask", NULL, CLuaOOPDefs::IsPedDoingTask ); // table (readonly)
    //lua_classvariable ( luaVM, "totalAmmo", NULL, CLuaDefs::GetPedTotalAmmo ); // table readonly    
    lua_registerclass ( luaVM, "Ped", "Element" );
}


int CLuaPedDefs::GetPedVoice ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            if ( !pPed->IsSpeechEnabled () )
            {
                lua_pushstring ( luaVM, "PED_TYPE_DISABLED" );
                return 1;
            }
            else
            {
                const char* szVoiceType = 0;
                const char* szVoiceBank = 0;
                pPed->GetVoice ( &szVoiceType, &szVoiceBank );
                if ( szVoiceType && szVoiceBank )
                {
                    lua_pushstring ( luaVM, szVoiceType );
                    lua_pushstring ( luaVM, szVoiceBank );
                    return 2;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPedDefs::SetPedVoice ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strVoiceType = "", strVoiceBank = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strVoiceType );
    argStream.ReadString ( strVoiceBank, "" );

    if ( !argStream.HasErrors () )
    {
        const char* szVoiceType = strVoiceType.c_str ();
        const char* szVoiceBank = strVoiceBank == "" ? NULL : strVoiceBank.c_str ();

        if ( pPed && szVoiceType )
        {
            if ( !stricmp ( szVoiceType, "PED_TYPE_DISABLED" ) )
            {
                pPed->SetSpeechEnabled ( false );
            }

            else if ( szVoiceBank )
            {
                pPed->SetSpeechEnabled ( true );
                pPed->SetVoice ( szVoiceType, szVoiceBank );
            }

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedWeapon ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned char ucSlot = 0xFF;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucSlot, 0xFF );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            if ( ucSlot == 0xFF )
                ucSlot = pPed->GetCurrentWeaponSlot ();

            CWeapon* pWeapon = pPed->GetWeapon ( (eWeaponSlot) ucSlot );
            if ( pWeapon )
            {
                unsigned char ucWeapon = pWeapon->GetType ();
                lua_pushnumber ( luaVM, ucWeapon );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedWeaponSlot ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Grab his current slot
            int iSlot = pPed->GetCurrentWeaponSlot ();
            lua_pushnumber ( luaVM, iSlot );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedAmmoInClip ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned char ucSlot = 0xFF;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucSlot, 0xFF );

    if ( !argStream.HasErrors () )
    {
        // Got a ped
        if ( pPed )
        {
            // Got a second argument too (slot)?
            ucSlot = ucSlot == 0xFF ? pPed->GetCurrentWeaponSlot () : ucSlot;

            CWeapon* pWeapon = pPed->GetWeapon ( (eWeaponSlot) ucSlot );
            if ( pWeapon )
            {
                unsigned short usAmmo = static_cast < unsigned short > ( pWeapon->GetAmmoInClip () );
                lua_pushnumber ( luaVM, usAmmo );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedTotalAmmo ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned char ucSlot = 0xFF;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucSlot, 0xFF );

    if ( !argStream.HasErrors () )
    {
        // Got the ped?
        if ( pPed )
        {
            // Got a second argument too (slot)?
            ucSlot = ucSlot == 0xFF ? pPed->GetCurrentWeaponSlot () : ucSlot;

            // Grab the ammo and return
            CWeapon* pWeapon = pPed->GetWeapon ( (eWeaponSlot) ucSlot );
            if ( pWeapon )
            {
                // Keep server and client synced
                unsigned short usAmmo = 1;
                if ( CWeaponNames::DoesSlotHaveAmmo ( ucSlot ) )
                    usAmmo = static_cast < unsigned short > ( pWeapon->GetAmmoTotal () );

                lua_pushnumber ( luaVM, usAmmo );
                return 1;
            }
            else if ( ucSlot < WEAPONSLOT_MAX && pPed->m_usWeaponAmmo[ucSlot] )
            {
                // The ped musn't be streamed in, so we can get the stored value instead
                ushort usAmmo = 1;

                if ( CWeaponNames::DoesSlotHaveAmmo ( ucSlot ) )
                    usAmmo = pPed->m_usWeaponAmmo[ucSlot];

                lua_pushnumber ( luaVM, usAmmo );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedWeaponMuzzlePosition ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            CVector vecMuzzlePos;
            if ( CStaticFunctionDefinitions::GetPedWeaponMuzzlePosition ( *pPed, vecMuzzlePos ) )
            {
                lua_pushnumber ( luaVM, vecMuzzlePos.fX );
                lua_pushnumber ( luaVM, vecMuzzlePos.fY );
                lua_pushnumber ( luaVM, vecMuzzlePos.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedOccupiedVehicle ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Grab his occupied vehicle
            CClientVehicle* pVehicle = pPed->GetOccupiedVehicle ();
            if ( pVehicle )
            {
                lua_pushelement ( luaVM, pVehicle );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaPedDefs::GetPedOccupiedVehicleSeat ( lua_State* luaVM )
{
    CClientPed* pPed = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        unsigned int uiVehicleSeat;
        if ( CStaticFunctionDefinitions::GetPedOccupiedVehicleSeat ( *pPed, uiVehicleSeat ) )
        {
            lua_pushnumber ( luaVM, uiVehicleSeat );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPedDefs::GetPedTask ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strPriority = "";
    unsigned int uiTaskType = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strPriority );
    argStream.ReadNumber ( uiTaskType );

    if ( !argStream.HasErrors () )
    {
        // Valid ped?
        if ( pPed )
        {
            // Any priority specified?
            if ( strPriority != "" )
            {
                // Primary or secondary task grabbed?
                bool bPrimary = false;
                if ( ( bPrimary = !stricmp ( strPriority.c_str (), "primary" ) ) ||
                    ( !stricmp ( strPriority.c_str (), "secondary" ) ) )
                {
                    // Grab the taskname list and return it
                    std::vector < SString > taskHierarchy;
                    if ( CStaticFunctionDefinitions::GetPedTask ( *pPed, bPrimary, uiTaskType, taskHierarchy ) )
                    {
                        for ( uint i = 0; i < taskHierarchy.size (); i++ )
                            lua_pushstring ( luaVM, taskHierarchy[i] );
                        return taskHierarchy.size ();
                    }
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedSimplestTask ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Grab his simplest task and return it
            const char* szTaskName = CStaticFunctionDefinitions::GetPedSimplestTask ( *pPed );
            if ( szTaskName )
            {
                lua_pushstring ( luaVM, szTaskName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::IsPedDoingTask ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strTaskName = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strTaskName );

    if ( !argStream.HasErrors () )
    {
        // Check ped
        if ( pPed )
        {
            // Check whether he's doing that task or not
            bool bIsDoingTask;
            if ( CStaticFunctionDefinitions::IsPedDoingTask ( *pPed, strTaskName.c_str (), bIsDoingTask ) )
            {
                lua_pushboolean ( luaVM, bIsDoingTask );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedTarget ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Grab his target element
            CClientEntity* pEntity = CStaticFunctionDefinitions::GetPedTarget ( *pPed );
            if ( pEntity )
            {
                lua_pushelement ( luaVM, pEntity );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedTargetStart ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Grab his start aim position and return it
            CVector vecStart;
            pPed->GetShotData ( &vecStart );

            lua_pushnumber ( luaVM, vecStart.fX );
            lua_pushnumber ( luaVM, vecStart.fY );
            lua_pushnumber ( luaVM, vecStart.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedTargetEnd ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Grab the ped end target position and return it
            CVector vecEnd;
            pPed->GetShotData ( NULL, &vecEnd );

            lua_pushnumber ( luaVM, vecEnd.fX );
            lua_pushnumber ( luaVM, vecEnd.fY );
            lua_pushnumber ( luaVM, vecEnd.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedTargetRange ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // TODO: getPedTargetRange
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedTargetCollision ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Grab his target collision and return it
            CVector vecCollision;
            if ( CStaticFunctionDefinitions::GetPedTargetCollision ( *pPed, vecCollision ) )
            {
                lua_pushnumber ( luaVM, vecCollision.fX );
                lua_pushnumber ( luaVM, vecCollision.fY );
                lua_pushnumber ( luaVM, vecCollision.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedArmor ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Grab the armor and return it
            float fArmor = pPed->GetArmor ();
            lua_pushnumber ( luaVM, fArmor );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedStat ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned short usStat = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( usStat );

    if ( !argStream.HasErrors () )
    {
        // Valid ped?
        if ( pPed )
        {
            // Check the stat
            if ( usStat < NUM_PLAYER_STATS )
            {
                float fValue = pPed->GetStat ( usStat );
                lua_pushnumber ( luaVM, fValue );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::IsPedChoking ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Return whether he's choking or not
            lua_pushboolean ( luaVM, pPed->IsChoking () );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaPedDefs::IsPedDucked ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Grab his ducked state
            bool bDucked = pPed->IsDucked ();
            lua_pushboolean ( luaVM, bDucked );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaPedDefs::IsPedInVehicle ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Find out whether he's in a vehicle or not
            bool bInVehicle;
            if ( CStaticFunctionDefinitions::IsPedInVehicle ( *pPed, bInVehicle ) )
            {
                // Return that state
                lua_pushboolean ( luaVM, bInVehicle );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaPedDefs::DoesPedHaveJetPack ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Find out whether he has a jetpack or not and return it
            bool bHasJetPack = pPed->HasJetPack ();
            lua_pushboolean ( luaVM, bHasJetPack );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::IsPedOnGround ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            // Find out whether he's on the ground or not and return it
            bool bOnGround = pPed->IsOnGround ();
            lua_pushboolean ( luaVM, bOnGround );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedContactElement ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            CClientEntity* pEntity = pPed->GetContactEntity ();
            if ( pEntity )
            {
                lua_pushelement ( luaVM, pEntity );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedRotation ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            float fRotation = ConvertRadiansToDegrees ( pPed->GetCurrentRotation () );
            lua_pushnumber ( luaVM, fRotation );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::CanPedBeKnockedOffBike ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            bool bCanBeKnockedOffBike = pPed->GetCanBeKnockedOffBike ();
            lua_pushboolean ( luaVM, bCanBeKnockedOffBike );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedBonePosition ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned char ucBone = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucBone );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            if ( ucBone <= BONE_RIGHTFOOT )
            {
                eBone bone = (eBone) ucBone;
                CVector vecPosition;
                if ( CStaticFunctionDefinitions::GetPedBonePosition ( *pPed, bone, vecPosition ) )
                {
                    lua_pushnumber ( luaVM, vecPosition.fX );
                    lua_pushnumber ( luaVM, vecPosition.fY );
                    lua_pushnumber ( luaVM, vecPosition.fZ );
                    return 3;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedWeaponSlot ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pElement = NULL;
    int iSlot = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( iSlot );

    if ( !argStream.HasErrors () )
    {
        // Valid entity?
        if ( pElement )
        {
            // Valid slot?
            if ( iSlot >= 0 )
            {
                // Set his slot
                if ( CStaticFunctionDefinitions::SetPedWeaponSlot ( *pElement, iSlot ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GivePedWeapon ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    eWeaponType weaponType;
    ushort usAmmo = 0;
    bool bSetAsCurrent = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadEnumStringOrNumber ( weaponType );
    argStream.ReadNumber ( usAmmo, 30 );
    argStream.ReadBool ( bSetAsCurrent, false );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::GivePedWeapon ( *pEntity, weaponType, usAmmo, bSetAsCurrent ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedClothes ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned char ucType = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadNumber ( ucType );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            SString strTexture, strModel;
            if ( CStaticFunctionDefinitions::GetPedClothes ( *pPed, ucType, strTexture, strModel ) )
            {
                lua_pushstring ( luaVM, strTexture );
                lua_pushstring ( luaVM, strModel );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "player", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedControlState ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    SString strControl = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strControl );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            bool bState;
            if ( CStaticFunctionDefinitions::GetPedControlState ( *pPed, strControl, bState ) )
            {
                lua_pushboolean ( luaVM, bState );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPedDefs::GetPedAnalogControlState ( lua_State* luaVM )
{
    SString strControlState = "";
    float fState = 0.0f;
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadString ( strControlState );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            float fState;
            if ( CStaticFunctionDefinitions::GetPedAnalogControlState ( *pPed, strControlState, fState ) )
            {
                lua_pushnumber ( luaVM, fState );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPedDefs::IsPedDoingGangDriveby ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            bool bDoingGangDriveby;
            if ( CStaticFunctionDefinitions::IsPedDoingGangDriveby ( *pPed, bDoingGangDriveby ) )
            {
                lua_pushboolean ( luaVM, bDoingGangDriveby );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedAnalogControlState ( lua_State* luaVM )
{
    SString strControlState = "";
    float fState = 0.0f;
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strControlState );
    argStream.ReadNumber ( fState );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedAnalogControlState ( *pEntity, strControlState, fState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPedDefs::GetPedAnimation ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            SString strBlockName, strAnimName;
            if ( CStaticFunctionDefinitions::GetPedAnimation ( *pPed, strBlockName, strAnimName ) )
            {
                lua_pushstring ( luaVM, strBlockName );
                lua_pushstring ( luaVM, strAnimName );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPedDefs::GetPedMoveState ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            std::string strMoveState;
            if ( CStaticFunctionDefinitions::GetPedMoveState ( *pPed, strMoveState ) )
            {
                lua_pushstring ( luaVM, strMoveState.c_str () );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPedDefs::GetPedMoveAnim ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            unsigned int iMoveAnim;
            if ( CStaticFunctionDefinitions::GetPedMoveAnim ( *pPed, iMoveAnim ) )
            {
                lua_pushnumber ( luaVM, iMoveAnim );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::IsPedHeadless ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            bool bHeadless;
            if ( CStaticFunctionDefinitions::IsPedHeadless ( *pPed, bHeadless ) )
            {
                lua_pushboolean ( luaVM, bHeadless );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::IsPedFrozen ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            bool bFrozen;
            if ( CStaticFunctionDefinitions::IsPedFrozen ( *pPed, bFrozen ) )
            {
                lua_pushboolean ( luaVM, bFrozen );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::IsPedFootBloodEnabled ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            bool bHasFootBlood = false;
            if ( CStaticFunctionDefinitions::IsPedFootBloodEnabled ( *pPed, bHasFootBlood ) )
            {
                lua_pushboolean ( luaVM, bHasFootBlood );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedCameraRotation ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            float fRotation = 0.0f;
            if ( CStaticFunctionDefinitions::GetPedCameraRotation ( *pPed, fRotation ) )
            {
                lua_pushnumber ( luaVM, fRotation );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::IsPedOnFire ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            bool bOnFire;
            if ( CStaticFunctionDefinitions::IsPedOnFire ( *pPed, bOnFire ) )
            {
                lua_pushboolean ( luaVM, bOnFire );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPedDefs::SetPedOnFire ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    bool bOnFire = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bOnFire );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedOnFire ( *pEntity, bOnFire ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::WarpPedIntoVehicle ( lua_State* luaVM )
{
    //  warpPedIntoVehicle ( element ped, element vehicle, int seat )
    CClientPed* pPed; CClientVehicle* pVehicle; uint uiSeat;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );
    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( uiSeat, 0 );

    MinClientReqCheck ( argStream, MIN_CLIENT_REQ_REMOVEPEDFROMVEHICLE_CLIENTSIDE, "function is being called client side" );

    if ( !argStream.HasErrors () )
    {
        if ( !pPed->IsLocalEntity () || !pVehicle->IsLocalEntity () )
            argStream.SetCustomError ( "This client side function will only work with client created peds and vehicles" );
    }

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::WarpPedIntoVehicle ( pPed, pVehicle, uiSeat ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::OOP_WarpPedIntoVehicle ( lua_State* luaVM )
{
    //  ped.vehicle = element vehicle
    //  ped.vehicle = nil
    CClientPed* pPed; CClientVehicle* pVehicle; uint uiSeat = 0;

    CScriptArgReader argStream ( luaVM );

    argStream.ReadUserData ( pPed );
    argStream.ReadUserData ( pVehicle, NULL );
    if ( pVehicle != NULL )
    {
        MinClientReqCheck ( argStream, MIN_CLIENT_REQ_WARPPEDINTOVEHICLE_CLIENTSIDE, "function is being called client side" );
        if ( !argStream.HasErrors () )
        {
            if ( !pPed->IsLocalEntity () || !pVehicle->IsLocalEntity () )
                argStream.SetCustomError ( "This client side function will only work with client created peds and vehicles" );
        }

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::WarpPedIntoVehicle ( pPed, pVehicle, uiSeat ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }
    else
    {
        if ( !argStream.HasErrors () )
        {
            if ( !pPed->IsLocalEntity () )
                argStream.SetCustomError ( "This client side function will only work with client created peds" );
        }

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::RemovePedFromVehicle ( pPed ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }

    lua_pushboolean ( luaVM, false );

    return 1;
}


int CLuaPedDefs::RemovePedFromVehicle ( lua_State* luaVM )
{
    //  removePedFromVehicle ( element ped )
    CClientPed* pPed;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    MinClientReqCheck ( argStream, MIN_CLIENT_REQ_WARPPEDINTOVEHICLE_CLIENTSIDE, "function is being called client side" );

    if ( !argStream.HasErrors () )
    {
        if ( !pPed->IsLocalEntity () )
            argStream.SetCustomError ( "This client side function will only work with client created peds" );
    }

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::RemovePedFromVehicle ( pPed ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::GetPedOxygenLevel ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        if ( pPed )
        {
            float fOxygen;
            if ( CStaticFunctionDefinitions::GetPedOxygenLevel ( *pPed, fOxygen ) )
            {
                lua_pushnumber ( luaVM, fOxygen );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::IsPedDead ( lua_State* luaVM )
{
    //  bool isPedDead ( ped thePed )
    CClientPed* pPed;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPed );

    if ( !argStream.HasErrors () )
    {
        // Grab his dead state and return it
        bool bDead = pPed->IsDead () || pPed->IsDying ();
        lua_pushboolean ( luaVM, bDead );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaPedDefs::AddPedClothes ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    SString strTexture = "", strModel = "";
    unsigned char ucType = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strTexture );
    argStream.ReadString ( strModel );
    argStream.ReadNumber ( ucType );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::AddPedClothes ( *pEntity, strTexture, strModel, ucType ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::RemovePedClothes ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    unsigned char ucType = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( ucType );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::RemovePedClothes ( *pEntity, ucType ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedControlState ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    SString strControl = "";
    bool bState = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strControl );
    argStream.ReadBool ( bState );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedControlState ( *pEntity, strControl, bState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedDoingGangDriveby ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    bool bDoingGangDriveby = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bDoingGangDriveby );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedDoingGangDriveby ( *pEntity, bDoingGangDriveby ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedLookAt ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CVector vecPosition;
    int iTime = 3000;
    int iBlend = 1000;
    CClientEntity * pTarget = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( iTime, 3000 );
    if ( argStream.NextIsUserData () )
    {
        argStream.ReadUserData ( pTarget );
    }
    else
    {
        argStream.ReadNumber ( iBlend, 1000 );
        argStream.ReadUserData ( pTarget, NULL );
    }

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedLookAt ( *pEntity, vecPosition, iTime, iBlend, pTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedHeadless ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    bool bHeadless = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bHeadless );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedHeadless ( *pEntity, bHeadless ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedFrozen ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    bool bFrozen = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bFrozen );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedFrozen ( *pEntity, bFrozen ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedFootBloodEnabled ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    bool bHasFootBlood = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bHasFootBlood );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedFootBloodEnabled ( *pEntity, bHasFootBlood ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "ped", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedCameraRotation ( lua_State* luaVM )
{
    //  bool setPedCameraRotation ( ped thePed, float cameraRotation )
    CClientEntity* pEntity; float fRotation;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( fRotation );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedCameraRotation ( *pEntity, fRotation ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedAimTarget ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CVector vecTarget;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadVector3D ( vecTarget );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedAimTarget ( *pEntity, vecTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::KillPed ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    CClientEntity *pKiller = NULL;
    unsigned char ucKillerWeapon;
    unsigned char ucBodyPart;
    bool bStealth;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadUserData ( pKiller, NULL );
    argStream.ReadNumber ( ucKillerWeapon, 0xFF );
    argStream.ReadNumber ( ucBodyPart, 0xFF );
    argStream.ReadBool ( bStealth, false );

    if ( !argStream.HasErrors () )
        if ( !pEntity->IsLocalEntity () )
            argStream.SetCustomError ( "This client side function will only work with client created peds" );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::KillPed ( *pEntity, pKiller, ucKillerWeapon, ucBodyPart, bStealth ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedRotation ( lua_State* luaVM )
{
    //  setPedRotation ( element ped, float rotation [, bool fixPedRotation = false ] )
    CClientEntity* pEntity; float fRotation; bool bNewWay;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( fRotation );
    argStream.ReadBool ( bNewWay, false );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedRotation ( *pEntity, fRotation, bNewWay ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedCanBeKnockedOffBike ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    bool bCanBeKnockedOffBike = false;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bCanBeKnockedOffBike );

    if ( !argStream.HasErrors () )
    {
        // Valid element?
        if ( pEntity )
        {
            // Set the new rotation
            if ( CStaticFunctionDefinitions::SetPedCanBeKnockedOffBike ( *pEntity, bCanBeKnockedOffBike ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedAnimation ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    bool bDummy;
    SString strBlockName = "";
    SString strAnimName = "";
    int iTime = -1;
    bool bLoop = true;
    bool bUpdatePosition = true;
    bool bInterruptable = true;
    bool bFreezeLastFrame = true;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    if ( argStream.NextIsBool () )
        argStream.ReadBool ( bDummy );      // Wiki used setPedAnimation(source,false) as an example
    else
        if ( argStream.NextIsNil () )
            argStream.m_iIndex++;               // Wiki docs said blockName could be nil
        else
            argStream.ReadString ( strBlockName, "" );
    argStream.ReadString ( strAnimName, "" );
    argStream.ReadNumber ( iTime, -1 );
    argStream.ReadBool ( bLoop, true );
    argStream.ReadBool ( bUpdatePosition, true );
    argStream.ReadBool ( bInterruptable, true );
    argStream.ReadBool ( bFreezeLastFrame, true );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedAnimation ( *pEntity, strBlockName == "" ? NULL : strBlockName.c_str (), strAnimName == "" ? NULL : strAnimName.c_str (), iTime, bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPedDefs::SetPedAnimationProgress ( lua_State* luaVM )
{
    //  bool setPedAnimationProgress ( ped thePed, string animName, float progress )
    CClientEntity* pEntity; SString strAnimName; float fProgress;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strAnimName, "" );
    argStream.ReadNumber ( fProgress, 0.0f );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetPedAnimationProgress ( *pEntity, strAnimName, fProgress ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedMoveAnim ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    unsigned int uiMoveAnim = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( uiMoveAnim );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedMoveAnim ( *pEntity, uiMoveAnim ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::SetPedOxygenLevel ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    float fOxygen = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( fOxygen );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetPedOxygenLevel ( *pEntity, fOxygen ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::CreatePed ( lua_State* luaVM )
{
    // Verify the argument
    CClientPed* pPed = NULL;
    unsigned long ulModel = 0;
    CVector vecPosition;
    float fRotation = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ulModel );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( fRotation, 0.0f );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource * pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create it
                CClientPed* pPed = CStaticFunctionDefinitions::CreatePed ( *pResource, ulModel, vecPosition, fRotation );
                if ( pPed )
                {
                    // Return it
                    lua_pushelement ( luaVM, pPed );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaPedDefs::DetonateSatchels ( lua_State* luaVM )
{
    if ( CStaticFunctionDefinitions::DetonateSatchels () )
    {
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPedDefs::GetWeaponProperty ( lua_State* luaVM )
{
    eWeaponSkill eWepSkill = eWeaponSkill::WEAPONSKILL_STD;
    eWeaponType eWep = eWeaponType::WEAPONTYPE_UNARMED;
    eWeaponProperty eProp = eWeaponProperty::WEAPON_INVALID_PROPERTY;
    CClientWeapon * pWeapon;
    CScriptArgReader argStream ( luaVM );

    if ( argStream.NextIsUserData () )
    {
        argStream.ReadUserData ( pWeapon );
        argStream.ReadEnumString ( eProp );

        if ( !argStream.HasErrors () )
        {
            if ( eProp == WEAPON_DAMAGE )
            {
                short sData = 0;
                if ( CStaticFunctionDefinitions::GetWeaponProperty ( pWeapon, eProp, sData ) )
                {
                    lua_pushnumber ( luaVM, sData );
                    return 1;
                }
            }
            else
                if ( eProp == WEAPON_FIRE_ROTATION )
                {
                    CVector vecWeaponInfo;
                    if ( CStaticFunctionDefinitions::GetWeaponProperty ( pWeapon, eProp, vecWeaponInfo ) )
                    {
                        lua_pushnumber ( luaVM, vecWeaponInfo.fX );
                        lua_pushnumber ( luaVM, vecWeaponInfo.fY );
                        lua_pushnumber ( luaVM, vecWeaponInfo.fZ );
                        return 1;
                    }
                }
                else
                {
                    float fData = 0;
                    if ( CStaticFunctionDefinitions::GetWeaponProperty ( pWeapon, eProp, fData ) )
                    {
                        lua_pushnumber ( luaVM, fData );
                        return 1;
                    }
                }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

        lua_pushboolean ( luaVM, false );
        return 1;
    }

    argStream.ReadEnumStringOrNumber ( eWep );
    argStream.ReadEnumStringOrNumber ( eWepSkill );
    argStream.ReadEnumString ( eProp );
    if ( !argStream.HasErrors () )
    {
        switch ( eProp )
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

            if ( CStaticFunctionDefinitions::GetWeaponProperty ( eProp, eWep, eWepSkill, fWeaponInfo ) )
            {
                lua_pushnumber ( luaVM, fWeaponInfo );
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

            if ( CStaticFunctionDefinitions::GetWeaponProperty ( eProp, eWep, eWepSkill, sWeaponInfo ) )
            {
                lua_pushinteger ( luaVM, sWeaponInfo );
                return 1;
            }
            break;
        }
        case WEAPON_FIRE_OFFSET:
        {
            CVector vecWeaponInfo;

            if ( CStaticFunctionDefinitions::GetWeaponProperty ( eProp, eWep, eWepSkill, vecWeaponInfo ) )
            {
                lua_pushnumber ( luaVM, vecWeaponInfo.fX );
                lua_pushnumber ( luaVM, vecWeaponInfo.fY );
                lua_pushnumber ( luaVM, vecWeaponInfo.fZ );
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
            MinClientReqCheck ( argStream, MIN_CLIENT_REQ_WEAPON_PROPERTY_FLAG, "flag name is being used" );
            if ( !argStream.HasErrors () )
            {
                bool bEnable;
                if ( CStaticFunctionDefinitions::GetWeaponPropertyFlag ( eProp, eWep, eWepSkill, bEnable ) )
                {
                    lua_pushboolean ( luaVM, bEnable );
                    return 1;
                }
            }
            break;
        }
        default:
        {
            argStream.SetCustomError ( "unsupported weapon property at argument 3" );
            break;
        }
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );


    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaPedDefs::GetOriginalWeaponProperty ( lua_State* luaVM )
{
    eWeaponSkill eWepSkill = eWeaponSkill::WEAPONSKILL_STD;
    eWeaponType eWep = eWeaponType::WEAPONTYPE_UNARMED;
    eWeaponProperty eProp = eWeaponProperty::WEAPON_INVALID_PROPERTY;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumStringOrNumber ( eWep );
    argStream.ReadEnumStringOrNumber ( eWepSkill );
    argStream.ReadEnumString ( eProp );
    if ( !argStream.HasErrors () )
    {
        switch ( eProp )
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

            if ( CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eProp, eWep, eWepSkill, fWeaponInfo ) )
            {
                lua_pushnumber ( luaVM, fWeaponInfo );
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

            if ( CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eProp, eWep, eWepSkill, sWeaponInfo ) )
            {
                lua_pushinteger ( luaVM, sWeaponInfo );
                return 1;
            }
            break;
        }
        case WEAPON_FIRE_OFFSET:
        {
            CVector vecWeaponInfo;

            if ( CStaticFunctionDefinitions::GetOriginalWeaponProperty ( eProp, eWep, eWepSkill, vecWeaponInfo ) )
            {
                lua_pushnumber ( luaVM, vecWeaponInfo.fX );
                lua_pushnumber ( luaVM, vecWeaponInfo.fY );
                lua_pushnumber ( luaVM, vecWeaponInfo.fZ );
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
            MinClientReqCheck ( argStream, MIN_CLIENT_REQ_WEAPON_PROPERTY_FLAG, "flag name is being used" );
            if ( !argStream.HasErrors () )
            {
                bool bEnable;
                if ( CStaticFunctionDefinitions::GetOriginalWeaponPropertyFlag ( eProp, eWep, eWepSkill, bEnable ) )
                {
                    lua_pushboolean ( luaVM, bEnable );
                    return 1;
                }
            }
            break;
        }
        default:
        {
            argStream.SetCustomError ( "unsupported weapon property at argument 3" );
            break;
        }
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );


    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}