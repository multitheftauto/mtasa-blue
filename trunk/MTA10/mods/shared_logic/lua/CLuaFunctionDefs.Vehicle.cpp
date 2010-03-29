/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Vehicle.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::GetVehicleType ( lua_State* luaVM )
{
    unsigned long ucModel = 0;
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
            ucModel = pVehicle -> GetModel ( );
    }
    else if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        ucModel = static_cast < unsigned long > (lua_tonumber ( luaVM, 1 ));
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleType" );

    if ( ucModel >= 400 && ucModel < 610 )
        lua_pushstring ( luaVM, CVehicleNames::GetVehicleTypeName ( ucModel ) );
    else
        lua_pushboolean ( luaVM, false );

    return 1;
}

int CLuaFunctionDefs::IsVehicleTaxiLightOn ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( pVehicle->GetModel() == 438 || pVehicle->GetModel() == 420 )
            {
                bool bLightState = pVehicle->IsTaxiLightOn ();
                if ( bLightState )
                {
                    lua_pushboolean ( luaVM, bLightState );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleTaxiLightOn", "vehicle", 1 );
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleTaxiLightOn ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA && lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( pVehicle->GetModel() == 438 || pVehicle->GetModel() == 420 )
            {
                bool bLightState = ( lua_toboolean ( luaVM, 2 ) ? true : false );
                pVehicle->SetTaxiLightOn ( bLightState );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleTaxiLightOn", "vehicle", 1 );
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle ) 
        {
            unsigned char ucColor1;
            unsigned char ucColor2;
            unsigned char ucColor3;
            unsigned char ucColor4;
            pVehicle->GetColor ( ucColor1, ucColor2, ucColor3, ucColor4 );

            lua_pushnumber ( luaVM, ucColor1 );
            lua_pushnumber ( luaVM, ucColor2 );
            lua_pushnumber ( luaVM, ucColor3 );
            lua_pushnumber ( luaVM, ucColor4 );
            return 4;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleColor", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleModelFromName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szName = lua_tostring ( luaVM, 1 );
        unsigned short usModel;
        if ( CStaticFunctionDefinitions::GetVehicleModelFromName ( szName, usModel ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( usModel ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleModelFromName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleLandingGearDown ( lua_State* luaVM )
{
    // Verify the first argument
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        // Grab the vehicle. Is it valid?
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            // Does the vehicle have landing gears?
            if ( CClientVehicleManager::HasLandingGears ( pVehicle->GetModel () ) )
            {
                // Return whether it has landing gears or not
                bool bLandingGear = pVehicle->IsLandingGearDown ();
                lua_pushboolean ( luaVM, bLandingGear );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleLandingGear", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleLandingGearDown" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::GetVehicleMaxPassengers ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned int uiMaxPassengers = CClientVehicleManager::GetMaxPassengerCount ( pVehicle->GetModel () );
            if ( uiMaxPassengers != 255 )
            {
                lua_pushnumber ( luaVM, uiMaxPassengers );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleMaxPassengers", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleMaxPassengers" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleOccupant ( lua_State* luaVM )
{
    // Grab the seat argument if exists
    unsigned int uiSeat = 0;
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
    {
        uiSeat = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
    }

    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CClientPed* pPed = pVehicle->GetOccupant ( uiSeat );
            if ( pPed )
            {
                lua_pushelement ( luaVM, pPed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleOccupant", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleOccupant" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleOccupants ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            // Create a new table
            lua_newtable ( luaVM );

            // Add All Occupants
            for ( unsigned char ucSeat = 0; ucSeat <= CClientVehicleManager::GetMaxPassengerCount ( pVehicle->GetModel () ); ++ ucSeat )
            {
                CClientPed* pPed = pVehicle->GetOccupant ( ucSeat );
                if ( pPed )
                {
                    lua_pushnumber ( luaVM, ucSeat );
                    lua_pushelement ( luaVM, pPed );
                    lua_settable ( luaVM, -3 );
                }
            }
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleOccupants", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleOccupants" );
            
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleController ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CClientPed* pPed = pVehicle->GetControllingPlayer ();
            if ( pPed )
            {
                lua_pushelement ( luaVM, pPed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleController", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleController" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleSirensOn ( lua_State* luaVM )
{
    // Verify the argument type
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        // Grab the vehicle pointer. Is it valid?
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            // Does the vehicle have Sirens?
            if ( CClientVehicleManager::HasSirens ( pVehicle->GetModel () ) )
            {
                // Return whether it has its Sirens on or not
                bool bSirensOn = pVehicle->IsSirenOrAlarmActive ();
                lua_pushboolean ( luaVM, bSirensOn );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleSirensOn", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleSirensOn" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::GetVehicleTurnVelocity ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CVector vecTurnVelocity;
            pVehicle->GetTurnSpeed ( vecTurnVelocity );

            lua_pushnumber ( luaVM, vecTurnVelocity.fX );
            lua_pushnumber ( luaVM, vecTurnVelocity.fY );
            lua_pushnumber ( luaVM, vecTurnVelocity.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleTurnVelocity", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleTurnVelocity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleTurretPosition ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CVector2D vecPosition;
            pVehicle->GetTurretRotation ( vecPosition.fX, vecPosition.fY );

            lua_pushnumber ( luaVM, vecPosition.fX );
            lua_pushnumber ( luaVM, vecPosition.fY );
            return 2;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleTurretPosition", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleTurretPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsVehicleLocked ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bLocked = pVehicle->AreDoorsLocked ();
            lua_pushboolean ( luaVM, bLocked );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleLocked", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleLocked" );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::GetVehicleUpgradeOnSlot ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {           
            unsigned char ucSlot = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
            if ( pUpgrades )
            {
                unsigned short usUpgrade = pUpgrades->GetSlotState ( ucSlot );
                lua_pushnumber ( luaVM, usUpgrade );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleUpgradeOnSlot", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleUpgradeOnSlot" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleUpgrades ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        // If the vehicle is valid
        if ( pVehicle )
        {           
            CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
            if ( pUpgrades )
            {
                // Create a new table
                lua_newtable ( luaVM );

                // Add all the upgrades to the table
                unsigned short* usSlotStates = pUpgrades->GetSlotStates ();

                unsigned int uiIndex = 0;
                unsigned char ucSlot = 0;
                for ( ; ucSlot < VEHICLE_UPGRADE_SLOTS ; ucSlot++ )
                {
                    if ( usSlotStates [ucSlot] != 0 )
                    {
                        lua_pushnumber ( luaVM, ++uiIndex );
                        lua_pushnumber ( luaVM, usSlotStates [ ucSlot ] );
                        lua_settable ( luaVM, -3 );
                    }
                }

                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleUpgrades", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleUpgrades" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleUpgradeSlotName ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned long ulNumber = static_cast < unsigned long > ( lua_tonumber ( luaVM, 1 ) );

        if ( ulNumber < 17 )
        {
            char szUpgradeName [32];
            if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( static_cast < unsigned char > ( ulNumber ), szUpgradeName, sizeof(szUpgradeName) ) )
            {
                lua_pushstring ( luaVM, szUpgradeName );
                return 1;
            }
        }
        else if ( ulNumber >= 1000 && ulNumber <= 1193 )
        {
            char szUpgradeName [32];
            if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( static_cast < unsigned short > ( ulNumber ), szUpgradeName, sizeof(szUpgradeName) ) )
            {
                lua_pushstring ( luaVM, szUpgradeName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleUpgradeSlotName", "slot/upgrade", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleUpgradeSlotName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleCompatibleUpgrades ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        unsigned char ucSlot = 0xFF;
        int iArgument2 = lua_type ( luaVM, 2 );
        if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
            ucSlot = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        if ( pVehicle )
        {
            CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades ();
            if ( pUpgrades )
            {
                // Create a new table
                lua_newtable ( luaVM );

                unsigned int uiIndex = 0;
                for ( unsigned short usUpgrade = 1000 ; usUpgrade <= 1193 ; usUpgrade++ )
                {
                    if ( pUpgrades->IsUpgradeCompatible ( usUpgrade ) )
                    {
                        if ( ucSlot != 0xFF )
                        {
                            unsigned char ucUpgradeSlot;
                            if ( !pUpgrades->GetSlotFromUpgrade ( usUpgrade, ucUpgradeSlot ) )
                                continue;

                            if ( ucUpgradeSlot != ucSlot )
                                continue;
                        }

                        // Add this one to a list                        
                        lua_pushnumber ( luaVM, ++uiIndex );
                        lua_pushnumber ( luaVM, usUpgrade );
                        lua_settable ( luaVM, -3 );
                    }
                }
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleCompatibleUpgrades", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleCompatibleUpgrades" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleWheelStates ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucFrontLeft = pVehicle->GetWheelStatus ( FRONT_LEFT_WHEEL );
            unsigned char ucRearLeft = pVehicle->GetWheelStatus ( REAR_LEFT_WHEEL );
            unsigned char ucFrontRight = pVehicle->GetWheelStatus ( FRONT_RIGHT_WHEEL );
            unsigned char ucRearRight = pVehicle->GetWheelStatus ( REAR_RIGHT_WHEEL );

            lua_pushnumber ( luaVM, ucFrontLeft );
            lua_pushnumber ( luaVM, ucRearLeft );
            lua_pushnumber ( luaVM, ucFrontRight );
            lua_pushnumber ( luaVM, ucRearRight );
            return 4;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleWheelStates", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleWheelStates" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleDoorState ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucDoor = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            unsigned char ucState = pVehicle->GetDoorStatus ( ucDoor );
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleDoorState", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleDoorState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleLightState ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucLight = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            unsigned char ucState = pVehicle->GetLightStatus ( ucLight );
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleLightState", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleLightState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehiclePanelState ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucPanel = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            unsigned char ucState = pVehicle->GetPanelStatus ( ucPanel );
            lua_pushnumber ( luaVM, ucState );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehiclePanelState", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehiclePanelState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleOverrideLights ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucLights = pVehicle->GetOverrideLights ();
            lua_pushnumber ( luaVM, ucLights );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleOverrideLights", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleOverrideLights" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleTowedByVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CClientVehicle* pTowedVehicle = pVehicle->GetTowedVehicle ();
            if ( pTowedVehicle )
            {
                lua_pushelement ( luaVM, pTowedVehicle );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleTowedByVehicle", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleTowedByVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleTowingVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CClientVehicle* pTowedByVehicle = pVehicle->GetTowedByVehicle ();
            if ( pTowedByVehicle )
            {
                lua_pushelement ( luaVM, pTowedByVehicle );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleTowingVehicle", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleTowingVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehiclePaintjob ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucPaintjob = pVehicle->GetPaintjob ();
            lua_pushnumber ( luaVM, ucPaintjob );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehiclePaintjob", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehiclePaintjob" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehiclePlateText ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            const char* szRegPlate = pVehicle->GetRegPlate ();
            if ( szRegPlate )
            {
                lua_pushstring ( luaVM, szRegPlate );
                return 1;
            }

            return false;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehiclePlateText", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehiclePlateText" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsVehicleFuelTankExplodable ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bExplodable = pVehicle->GetCanShootPetrolTank ();
            lua_pushboolean ( luaVM, bExplodable );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleFuelTankExplodable", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleFuelTankExplodable" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsVehicleFrozen ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bFrozen = pVehicle->IsFrozen ();
            lua_pushboolean ( luaVM, bFrozen );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleFrozen", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleFrozen" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsVehicleOnGround ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );

        if ( pVehicle )
        {
            bool bOnGround = pVehicle->IsOnGround ();
            lua_pushboolean ( luaVM, bOnGround );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleOnGround", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleOnGround" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            const char* szVehicleName = CVehicleNames::GetVehicleName ( pVehicle->GetModel () );
            if ( szVehicleName )
            {
                lua_pushstring ( luaVM, szVehicleName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleName", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleAdjustableProperty ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( pVehicle->HasAdjustableProperty () )
            {
                unsigned short usAdjustableProperty = pVehicle->GetAdjustablePropertyValue ();
                lua_pushnumber ( luaVM, usAdjustableProperty );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleAdjustableProperty", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleAdjustableProperty" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetHelicopterRotorSpeed ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            float fSpeed;
            if ( CStaticFunctionDefinitions::GetHelicopterRotorSpeed ( *pVehicle, fSpeed ) )
            {
                lua_pushnumber ( luaVM, fSpeed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getHelicopterRotorSpeed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getHelicopterRotorSpeed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsTrainDerailed ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bDerailed;
            if ( CStaticFunctionDefinitions::IsTrainDerailed ( *pVehicle, bDerailed ) )
            {
                lua_pushboolean ( luaVM, bDerailed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isTrainDerailed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isTrainDerailed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsTrainDerailable ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bIsDerailable;
            if ( CStaticFunctionDefinitions::IsTrainDerailable ( *pVehicle, bIsDerailable ) )
            {
                lua_pushboolean ( luaVM, bIsDerailable );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isTrainDerailable", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isTrainDerailable" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetTrainDirection ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bDirection;
            if ( CStaticFunctionDefinitions::GetTrainDirection ( *pVehicle, bDirection ) )
            {
                lua_pushboolean ( luaVM, bDirection );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getTrainDirection", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getTrainDirection" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetTrainSpeed ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            float fSpeed;
            if ( CStaticFunctionDefinitions::GetTrainSpeed ( *pVehicle, fSpeed ) )
            {
                lua_pushnumber ( luaVM, fSpeed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getTrainSpeed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getTrainSpeed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleEngineState ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bState;
            if ( CStaticFunctionDefinitions::GetVehicleEngineState ( *pVehicle, bState ) )
            {
                lua_pushboolean ( luaVM, bState );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleEngineState", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleEngineState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleNameFromModel ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned short usModel = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );
        char szVehicleName [32];

        if ( CStaticFunctionDefinitions::GetVehicleNameFromModel ( usModel, szVehicleName, sizeof(szVehicleName) ) )
        {
            lua_pushstring ( luaVM, szVehicleName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleNameFromID" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateVehicle ( lua_State* luaVM )
{
    // Verify the parameters
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the vehicle id parameter
        unsigned short usModel = static_cast < unsigned short > ( atoi ( lua_tostring ( luaVM, 1 ) ) );

        // Grab the position parameters
        CVector vecPosition;
        vecPosition.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 2 ) ) );
        vecPosition.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 3 ) ) );
        vecPosition.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 4 ) ) );

        // Grab the rotation parameters
        CVector vecRotation;
        const char* szRegPlate = NULL;
        int iArgument5 = lua_type ( luaVM, 5 );
        if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
        {
            vecRotation.fX = static_cast < float > ( atof ( lua_tostring ( luaVM, 5 ) ) );

            int iArgument6 = lua_type ( luaVM, 6 );
            if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
            {
                vecRotation.fY = static_cast < float > ( atof ( lua_tostring ( luaVM, 6 ) ) );

                int iArgument7 = lua_type ( luaVM, 7 );
                if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
                {
                    vecRotation.fZ = static_cast < float > ( atof ( lua_tostring ( luaVM, 7 ) ) );

                    if ( lua_type ( luaVM, 8 ) == LUA_TSTRING )
                    {
                        szRegPlate = lua_tostring ( luaVM, 8 );
                    }
                }
            }
        }

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource * pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Create the vehicle and return its handle
                CClientVehicle* pVehicle = CStaticFunctionDefinitions::CreateVehicle ( *pResource, usModel, vecPosition, vecRotation, szRegPlate );
                if ( pVehicle )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CClientEntity* ) pVehicle );
                    }

                    lua_pushelement ( luaVM, pVehicle );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::FixVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::FixVehicle ( *pEntity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fixVehicle", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fixVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BlowVehicle ( lua_State* luaVM )
{
    // Read out whether to explode or not
    bool bExplode = true;
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument2 == LUA_TBOOLEAN )
    {
        bExplode = lua_toboolean ( luaVM, 2 ) ? true:false;
    }
    else if ( iArgument2 != LUA_TNONE )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "blowVehicle" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }

    // Verify the element pointer argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::BlowVehicle ( *pEntity, bExplode ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blowVehicle", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "blowVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}
int CLuaFunctionDefs::IsVehicleBlown ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle ) {
            bool bBlown;
            if ( CStaticFunctionDefinitions::IsVehicleBlown(*pVehicle, bBlown) ) {
                lua_pushboolean ( luaVM, bBlown );
                return 1;
            }
        } 
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isVehicleBlown", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isVehicleBlown" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleHeadLightColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            SColor color;
            if ( CStaticFunctionDefinitions::GetVehicleHeadLightColor ( *pVehicle, color ) )
            {
                lua_pushnumber ( luaVM, color.R );
                lua_pushnumber ( luaVM, color.G );
                lua_pushnumber ( luaVM, color.B );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleHeadLightColor", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleHeadLightColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleTurnVelocity ( lua_State* luaVM )
{
    int iArgumentType1 = lua_type ( luaVM, 1 );
    int iArgumentType2 = lua_type ( luaVM, 2 );
    int iArgumentType3 = lua_type ( luaVM, 3 );
    int iArgumentType4 = lua_type ( luaVM, 4 );
    if ( ( iArgumentType1 == LUA_TLIGHTUSERDATA ) &&
        ( iArgumentType2 == LUA_TNUMBER || iArgumentType2 == LUA_TSTRING ) &&
        ( iArgumentType3 == LUA_TNUMBER || iArgumentType3 == LUA_TSTRING ) &&
        ( iArgumentType4 == LUA_TNUMBER || iArgumentType4 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CVector vecTurnVelocity;
            vecTurnVelocity.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            vecTurnVelocity.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
            vecTurnVelocity.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

            if ( CStaticFunctionDefinitions::SetVehicleTurnVelocity ( *pEntity, vecTurnVelocity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleTurnVelocity", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleTurnVelocity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleColor ( lua_State* luaVM )
{
    int iArgumentType2 = lua_type ( luaVM, 2 );
    int iArgumentType3 = lua_type ( luaVM, 3 );
    int iArgumentType4 = lua_type ( luaVM, 4 );
    int iArgumentType5 = lua_type ( luaVM, 5 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgumentType2 == LUA_TNUMBER || iArgumentType2 == LUA_TSTRING ) &&
        ( iArgumentType3 == LUA_TNUMBER || iArgumentType3 == LUA_TSTRING ) &&
        ( iArgumentType4 == LUA_TNUMBER || iArgumentType4 == LUA_TSTRING ) &&
        ( iArgumentType5 == LUA_TNUMBER || iArgumentType5 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            double dColor1 = lua_tonumber ( luaVM, 2 );
            double dColor2 = lua_tonumber ( luaVM, 3 );
            double dColor3 = lua_tonumber ( luaVM, 4 );
            double dColor4 = lua_tonumber ( luaVM, 5 );

            if ( dColor1 >= 0 && dColor1 <= 255 &&
                dColor2 >= 0 && dColor2 <= 255 &&
                dColor3 >= 0 && dColor3 <= 255 &&
                dColor4 >= 0 && dColor4 <= 255 )
            {
                if ( CStaticFunctionDefinitions::SetVehicleColor ( *pEntity, static_cast < unsigned char > ( dColor1 ), static_cast < unsigned char > ( dColor2 ), static_cast < unsigned char > ( dColor3 ), static_cast < unsigned char > ( dColor4 ) ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleColor", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleLandingGearDown ( lua_State* luaVM )
{
    // Verify the two arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA && iArgument2 == LUA_TBOOLEAN )
    {
        // Grab the element and verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bLandingGearDown = lua_toboolean ( luaVM, 2 ) ? true:false;
        if ( pEntity )
        {
            CClientVehicle* pVehicle = static_cast < CClientVehicle* > ( pEntity );
            if ( CClientVehicleManager::HasLandingGears ( pVehicle->GetModel () ) )
            {
                // Do it
                if ( CStaticFunctionDefinitions::SetVehicleLandingGearDown ( *pEntity, bLandingGearDown ) )
                    lua_pushboolean ( luaVM, true );
                else
                    lua_pushboolean ( luaVM, false );
            }
            else
                lua_pushboolean ( luaVM, false );

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleLandingGearDown", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleLandingGearDown" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleLocked ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
        lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bLocked = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleLocked ( *pEntity, bLocked ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleLocked", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleLocked" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleDoorsUndamageable ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
        lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bDoorsUndamageable = ( lua_toboolean ( luaVM, 2 ) ) ? false:true;

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleDoorsUndamageable ( *pEntity, bDoorsUndamageable ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleDoorsUndamageable", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleDoorsUndamageable" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleSirensOn ( lua_State* luaVM )
{
    // Verify the two arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA && iArgument2 == LUA_TBOOLEAN )
    {
        // Grab the element and verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bSirensOn = lua_toboolean ( luaVM, 2 ) ? true:false;
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehicleSirensOn ( *pEntity, bSirensOn ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleSirensOn", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleSirensOn" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::AddVehicleUpgrade ( lua_State* luaVM )
{
    // Verify the two arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA && 
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {

        // Grab the element and verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );

        unsigned short usUpgrade = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
        if ( pEntity )
        {
            if ( iArgument2 == LUA_TSTRING )
            {
                const char* szUpgrade = lua_tostring ( luaVM, 2 );
                if ( strcmp ( szUpgrade, "all" ) == 0 )
                {
                    if ( CStaticFunctionDefinitions::AddAllVehicleUpgrades ( *pEntity ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
            }

            if ( CStaticFunctionDefinitions::AddVehicleUpgrade ( *pEntity, usUpgrade ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "addVehicleUpgrade", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "addVehicleUpgrade" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveVehicleUpgrade ( lua_State* luaVM )
{
    // Verify the two arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA && 
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {

        // Grab the element and verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned short usUpgrade = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::RemoveVehicleUpgrade ( *pEntity, usUpgrade ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "removeVehicleUpgrade", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "removeVehicleUpgrade" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleDoorState ( lua_State* luaVM )
{
    // Verify the three arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the element and verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned char ucDoor = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        unsigned char ucState = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehicleDoorState ( *pEntity, ucDoor, ucState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleDoorState", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleDoorState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleWheelStates ( lua_State* luaVM )
{
    // Verify the three arguments
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        int iFrontLeft = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
        int iRearLeft = -1, iFrontRight = -1, iRearRight = -1;

        int iArgument3 = lua_type ( luaVM, 3 ), iArgument4 = lua_type ( luaVM, 4 ),
            iArgument5 = lua_type ( luaVM, 5 );

        if ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER )
            iRearLeft = static_cast < int > ( lua_tonumber ( luaVM, 3 ) );

        if ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER )
            iFrontRight = static_cast < int > ( lua_tonumber ( luaVM, 4 ) );

        if ( iArgument5 == LUA_TSTRING || iArgument5 == LUA_TNUMBER )
            iRearRight = static_cast < int > ( lua_tonumber ( luaVM, 5 ) );

        // Grab the element and verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleWheelStates ( *pEntity, iFrontLeft, iRearLeft, iFrontRight, iRearRight ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleWheelStates", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleWheelStates" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleLightState ( lua_State* luaVM )
{
    // Verify the three arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the element and verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned char ucLight = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        unsigned char ucState = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehicleLightState ( *pEntity, ucLight, ucState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleLightState", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleLightState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehiclePanelState ( lua_State* luaVM )
{
    // Verify the three arguments
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the element and verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned char ucPanel = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        unsigned char ucState = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
        if ( pEntity )
        {
            // Do it
            if ( CStaticFunctionDefinitions::SetVehiclePanelState ( *pEntity, ucPanel, ucState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehiclePanelState", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehiclePanelState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleOverrideLights ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        unsigned char ucLights = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        if ( pEntity )
        {
            if ( ucLights <= 2 )
            {
                if ( CStaticFunctionDefinitions::SetVehicleOverrideLights ( *pEntity, ucLights ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleOverrideLights", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleOverrideLights" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::AttachTrailerToVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
        lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        CClientVehicle* pTrailer = lua_tovehicle ( luaVM, 2 );

        if ( pVehicle )
        {
            if ( pTrailer )
            {
                if ( CStaticFunctionDefinitions::AttachTrailerToVehicle ( *pVehicle, *pTrailer ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "attachTrailerToVehicle", "trailer", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "attachTrailerToVehicle", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "attachTrailerToVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::DetachTrailerFromVehicle ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        CClientVehicle* pTrailer = NULL;

        if ( lua_type ( luaVM, 2 )  == LUA_TLIGHTUSERDATA )
            pTrailer = lua_tovehicle ( luaVM, 2 );

        if ( pVehicle )
        {
            if ( CStaticFunctionDefinitions::DetachTrailerFromVehicle ( *pVehicle, pTrailer ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "detachTrailerFromVehicle", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "detachTrailerFromVehicle" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleEngineState ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
        lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bState = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleEngineState ( *pEntity, bState ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleEngineState", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleEngineState" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleDirtLevel ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( /*iArgument2 == LUA_TSTRING ||*/ iArgument2 == LUA_TNUMBER ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        float fDirtLevel = static_cast < float > ( lua_tonumber ( luaVM, 2 ) ); /*( atof ( lua_tostring ( luaVM, 2 ) ) );*/

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleDirtLevel ( *pEntity, fDirtLevel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleDirtLevel", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleDirtLevel" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleDamageProof ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
        lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        bool bDamageProof = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetVehicleDamageProof ( *pEntity, bDamageProof ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleDamageProof", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleDamageProof" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehiclePaintjob ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned char ucPaintjob = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetVehiclePaintjob ( *pEntity, ucPaintjob ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehiclePaintjob", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehiclePaintjob" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleFuelTankExplodable ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
        lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            bool bExplodable = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;
            if ( CStaticFunctionDefinitions::SetVehicleFuelTankExplodable ( *pEntity, bExplodable ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleFuelTankExplodable", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleFuelTankExplodable" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleFrozen ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            {
                if ( CStaticFunctionDefinitions::SetVehicleFrozen ( *pVehicle, lua_toboolean ( luaVM, 2 ) ? true:false ) )
                {
                    lua_pushboolean ( luaVM, true );
                }
                return 1;

            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "setVehicleFrozen" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleFrozen", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleFrozen" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleAdjustableProperty ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned short usAdjustableProperty = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetVehicleAdjustableProperty ( *pEntity, usAdjustableProperty ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleAdjustableProperty", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleAdjustableProperty" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetHelicopterRotorSpeed ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            float fSpeed = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetHelicopterRotorSpeed ( *pVehicle, fSpeed ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setHelicopterRotorSpeed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setHelicopterRotorSpeed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetTrainDerailed ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bDerailed = ( lua_toboolean ( luaVM, 2 ) ? true : false );
            if ( CStaticFunctionDefinitions::SetTrainDerailed ( *pVehicle, bDerailed ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setTrainDerailed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTrainDerailed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetTrainDerailable ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bDerailable = ( lua_toboolean ( luaVM, 2 ) ? true : false );
            if ( CStaticFunctionDefinitions::SetTrainDerailable ( *pVehicle, bDerailable ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setTrainDerailable", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTrainDerailable" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetTrainDirection ( lua_State* luaVM )
{
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bDirection = lua_toboolean ( luaVM, 2 ) ? true : false;
            if ( CStaticFunctionDefinitions::SetTrainDirection ( *pVehicle, bDirection ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setTrainDirection", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTrainDirection" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetTrainSpeed ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            float fSpeed = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetTrainSpeed ( *pVehicle, fSpeed ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setTrainSpeed", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setTrainSpeed" );

    lua_pushboolean ( luaVM, false );
    return 1;
}



// Radio

int CLuaFunctionDefs::SetRadioChannel ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER ||
        lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        unsigned char ucChannel = ( unsigned char ) lua_tonumber ( luaVM, 1 );
        if ( CStaticFunctionDefinitions::SetRadioChannel ( ucChannel ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setRadioChannel" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetRadioChannel ( lua_State* luaVM )
{
    unsigned char ucChannel = 0;
    if ( CStaticFunctionDefinitions::GetRadioChannel ( ucChannel ) )
    {
        lua_pushnumber ( luaVM, ucChannel );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetRadioChannelName ( lua_State* luaVM )
{
    static const char* szRadioStations[] = { "Radio off", "Playback FM", "K-Rose", "K-DST",
        "Bounce FM", "SF-UR", "Radio Los Santos", "Radio X", "CSR 103.9", "K-Jah West",
        "Master Sounds 98.3", "WCTR", "User Track Player" };

    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        int iChannel = static_cast < int > ( lua_tonumber ( luaVM, 1 ) );
        if ( iChannel >= 0 && iChannel < sizeof(szRadioStations)/sizeof(char *) ) {
            lua_pushstring ( luaVM, szRadioStations [ iChannel ] );
            return 1;
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleGravity ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CVector vecGravity;
            pVehicle->GetGravity ( vecGravity );
            lua_pushnumber ( luaVM, vecGravity.fX );
            lua_pushnumber ( luaVM, vecGravity.fY );
            lua_pushnumber ( luaVM, vecGravity.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleGravity", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleGravity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleGravity ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CVector vecGravity ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                                 static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                                 static_cast < float > ( lua_tonumber ( luaVM, 4 ) ) );
            pVehicle->SetGravity ( vecGravity );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleGravity", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleGravity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleHeadLightColor ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            SColor color;
            color.R = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            color.G = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
            color.B = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
            color.A = 255;

            if ( CStaticFunctionDefinitions::SetVehicleHeadLightColor ( *pVehicle, color ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleHeadLightColor", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleHeadLightColor" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleHandlingData ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            const CHandlingEntry* pOriginalEntry = pVehicle->GetOriginalHandlingData ();
            if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
            {
                const char* szHandlingData = lua_tostring ( luaVM, 2 );
                if ( szHandlingData )
                {
                    int iArgument3 = lua_type ( luaVM, 3 );
                    CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
                    if ( pEntry )
                    {
                        bool bSuccess = false;

                        if (strcmp(szHandlingData,"mass")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                float mass = (float)lua_tonumber( luaVM, 3 );

                                if ( mass > 1 )
                                {
                                    pEntry->SetMass ( mass );
                                    bSuccess=true;
                                }
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetMass ( pOriginalEntry->GetMass () );
                                bSuccess=true;
                            }
                        }
                        else if (strcmp(szHandlingData,"turnMass")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetTurnMass ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetTurnMass ( pOriginalEntry->GetTurnMass () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"dragCoeff")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetDragCoeff ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetDragCoeff ( pOriginalEntry->GetDragCoeff () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"centerOfMass")==0)
                        {
                            CVector vecCenter=pEntry->GetCenterOfMass ();
                            if ( iArgument3 == LUA_TTABLE )
                            {
                                lua_getfield ( luaVM, 3, "posX" );
                                lua_getfield ( luaVM, 3, "posY" );
                                lua_getfield ( luaVM, 3, "posZ" );
                                if ( lua_type ( luaVM, -3 ) == LUA_TNUMBER )
                                    vecCenter.fX = (float)lua_tonumber(luaVM, -3);
                                if ( lua_type ( luaVM, -2 ) == LUA_TNUMBER )
                                    vecCenter.fY = (float)lua_tonumber(luaVM, -2);
                                if ( lua_type ( luaVM, -1 ) == LUA_TNUMBER )
                                    vecCenter.fZ = (float)lua_tonumber(luaVM, -1);
                                lua_pop ( luaVM, 3 );
                                pEntry->SetCenterOfMass ( vecCenter );
                                bSuccess=true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetCenterOfMass ( pOriginalEntry->GetCenterOfMass () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"percentSubmerged")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetPercentSubmerged ( static_cast < unsigned int > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetPercentSubmerged ( pOriginalEntry->GetPercentSubmerged () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"tractionMultiplier")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetTractionMultiplier ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetTractionMultiplier ( pOriginalEntry->GetTractionMultiplier () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"driveType")==0)
                        {
                            if ( iArgument3 == LUA_TSTRING )
                            {
                                const char* szType=lua_tostring(luaVM,3);
                                if (strcmp(szType,"fwd")==0)
                                    pEntry->SetCarDriveType(CHandlingEntry::eDriveType::FWD);
                                else if (strcmp(szType,"rwd")==0)
                                    pEntry->SetCarDriveType(CHandlingEntry::eDriveType::RWD);
                                else if (strcmp(szType,"awd")==0)
                                    pEntry->SetCarDriveType(CHandlingEntry::eDriveType::FOURWHEEL);
                                else
                                    _asm jmp _dtf
                                bSuccess=true;
                                __asm
                                {
_dtf:
                                }
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetCarDriveType ( pOriginalEntry->GetCarDriveType () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"engineType")==0)
                        {
                            if ( iArgument3 == LUA_TSTRING )
                            {
                                const char* szType=lua_tostring(luaVM,3);
                                if (strcmp(szType,"petrol")==0)
                                    pEntry->SetCarEngineType(CHandlingEntry::eEngineType::PETROL);
                                else if (strcmp(szType,"diesel")==0)
                                    pEntry->SetCarEngineType(CHandlingEntry::eEngineType::DIESEL);
                                else if (strcmp(szType,"electric")==0)
                                    pEntry->SetCarEngineType(CHandlingEntry::eEngineType::ELECTRIC);
                                else
                                    __asm jmp _etf
                                bSuccess=true;
                                __asm
                                {
_etf:
                                }
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetCarEngineType ( pOriginalEntry->GetCarEngineType () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"numberOfGears")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetNumberOfGears ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetNumberOfGears ( pOriginalEntry->GetNumberOfGears () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"engineAcceleration")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetEngineAccelleration ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetEngineAccelleration ( pOriginalEntry->GetEngineAccelleration () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"engineInertia")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetEngineInertia ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetEngineInertia ( pOriginalEntry->GetEngineInertia () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"maxVelocity")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetMaxVelocity ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetMaxVelocity ( pOriginalEntry->GetMaxVelocity () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"brakeDeceleration")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetBrakeDecelleration ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetBrakeDecelleration ( pOriginalEntry->GetBrakeDecelleration () ); 
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"brakeBias")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetBrakeBias ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetBrakeBias ( pOriginalEntry->GetBrakeBias () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"ABS")==0)
                        {
                            if ( iArgument3 == LUA_TBOOLEAN )
                            {
                                pEntry->SetABS ( lua_toboolean ( luaVM, 3 ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetABS ( pOriginalEntry->GetABS () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"steeringLock")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetSteeringLock ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetSteeringLock ( pOriginalEntry->GetSteeringLock () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"tractionLoss")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetTractionLoss ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetTractionLoss ( pOriginalEntry->GetTractionLoss () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"tractionBias")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetTractionBias ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetTractionBias ( pOriginalEntry->GetTractionBias () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"suspensionForceLevel")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetSuspensionForceLevel ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetSuspensionForceLevel ( pOriginalEntry->GetSuspensionForceLevel () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"suspensionDamping")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetSuspensionDamping ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetSuspensionDamping ( pOriginalEntry->GetSuspensionDamping () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"suspensionHighSpeedDamping")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetSuspensionHighSpeedDamping ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetSuspensionHighSpeedDamping ( pOriginalEntry->GetSuspensionHighSpeedDamping () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"suspensionUpperLimit")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetSuspensionUpperLimit ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetSuspensionUpperLimit ( pOriginalEntry->GetSuspensionUpperLimit () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"suspensionLowerLimit")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetSuspensionLowerLimit ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetSuspensionLowerLimit ( pOriginalEntry->GetSuspensionLowerLimit () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"suspensionFrontRearBias")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetSuspensionFrontRearBias ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetSuspensionFrontRearBias ( pOriginalEntry->GetSuspensionFrontRearBias () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"suspensionAntidiveMultiplier")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetSuspensionAntidiveMultiplier ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetSuspensionAntidiveMultiplier ( pOriginalEntry->GetSuspensionAntidiveMultiplier () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"collisionDamageMultiplier")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetCollisionDamageMultiplier ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetCollisionDamageMultiplier ( pOriginalEntry->GetCollisionDamageMultiplier () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"seatOffsetDistance")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetSeatOffsetDistance ( static_cast < unsigned int > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetSeatOffsetDistance ( pOriginalEntry->GetSeatOffsetDistance () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"monetary")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetMonetary ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetMonetary ( pOriginalEntry->GetMonetary () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"headLight")==0)
                        {
                            if ( iArgument3 == LUA_TSTRING )
                            {
                                const char* szType=lua_tostring(luaVM,3);
                                if (strcmp(szType,"long")==0)
                                    pEntry->SetHeadLight(CHandlingEntry::eLightType::LONG);
                                else if (strcmp(szType,"small")==0)
                                    pEntry->SetHeadLight(CHandlingEntry::eLightType::SMALL);
                                else if (strcmp(szType,"tall")==0)
                                    pEntry->SetHeadLight(CHandlingEntry::eLightType::TALL);
                                else if (strcmp(szType,"big")==0)
                                    pEntry->SetHeadLight(CHandlingEntry::eLightType::BIG);
                                else
                                    __asm jmp _htf
                                bSuccess=true;
                                __asm
                                {
_htf:
                                }
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetHeadLight ( pOriginalEntry->GetHeadLight () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"tailLight")==0)
                        {
                            if ( iArgument3 == LUA_TSTRING )
                            {
                                const char* szType=lua_tostring(luaVM,3);
                                if (strcmp(szType,"long")==0)
                                    pEntry->SetTailLight(CHandlingEntry::eLightType::LONG);
                                else if (strcmp(szType,"small")==0)
                                    pEntry->SetTailLight(CHandlingEntry::eLightType::SMALL);
                                else if (strcmp(szType,"tall")==0)
                                    pEntry->SetTailLight(CHandlingEntry::eLightType::TALL);
                                else if (strcmp(szType,"big")==0)
                                    pEntry->SetTailLight(CHandlingEntry::eLightType::BIG);
                                else
                                    __asm jmp _ttf
                                bSuccess=true;
                                __asm
                                {
_ttf:
                                }
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetTailLight ( pOriginalEntry->GetTailLight () );
                                bSuccess = true;
                            }
                        }
                        else if (strcmp(szHandlingData,"animGroup")==0)
                        {
                            if ( iArgument3 == LUA_TNUMBER )
                            {
                                pEntry->SetAnimGroup ( static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) ) );
                                bSuccess = true;
                            }
                            else if ( iArgument3 == LUA_TNIL )
                            {
                                pEntry->SetAnimGroup ( pOriginalEntry->GetAnimGroup () );
                                bSuccess = true;
                            }
                        }

                        if ( bSuccess )
                        {
                            pVehicle->ApplyHandling();
                            lua_pushboolean ( luaVM, true );
                            return 1;
                        }
                    }
                }
            }
            else if ( lua_type ( luaVM, 2 ) == LUA_TTABLE )
            {
                lua_pushnil ( luaVM );
                while ( lua_next ( luaVM, 2 ) )
                {
                    if ( lua_type ( luaVM, -2 ) == LUA_TSTRING )
                    {
                        const char* szHandlingData = lua_tostring ( luaVM, -2 );
                        if ( szHandlingData )
                        {
                            int iArgument3 = lua_type ( luaVM, -1 );
                            CHandlingEntry* pEntry = pVehicle->GetHandlingData ();
                            if ( pEntry )
                            {
                                if (strcmp(szHandlingData,"mass")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        float mass = (float)lua_tonumber(luaVM,-1);
                                        if (mass > 1)
                                            pEntry->SetMass ( mass );
                                    }
                                }
                                else if (strcmp(szHandlingData,"turnMass")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetTurnMass ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"dragCoeff")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetDragCoeff ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"centerOfMass")==0)
                                {
                                    CVector vecCenter=pEntry->GetCenterOfMass ();
                                    if ( iArgument3 == LUA_TTABLE )
                                    {
                                        lua_getfield ( luaVM, -1, "posX" );
                                        lua_getfield ( luaVM, -2, "posY" );
                                        lua_getfield ( luaVM, -3, "posZ" );
                                        if ( lua_type ( luaVM, -3 ) == LUA_TNUMBER )
                                            vecCenter.fX = (float)lua_tonumber(luaVM, -3);
                                        if ( lua_type ( luaVM, -2 ) == LUA_TNUMBER )
                                            vecCenter.fY = (float)lua_tonumber(luaVM, -2);
                                        if ( lua_type ( luaVM, -1 ) == LUA_TNUMBER )
                                            vecCenter.fZ = (float)lua_tonumber(luaVM, -1);
                                        lua_pop ( luaVM, 3 );
                                        pEntry->SetCenterOfMass ( vecCenter );
                                    }
                                }
                                else if (strcmp(szHandlingData,"percentSubmerged")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetPercentSubmerged ( static_cast < unsigned int > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                    else if ( iArgument3 == LUA_TNIL )
                                    {
                                        pEntry->SetPercentSubmerged ( pOriginalEntry->GetPercentSubmerged () );
                                    }
                                }
                                else if (strcmp(szHandlingData,"tractionMultiplier")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetTractionMultiplier ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"driveType")==0)
                                {
                                    if ( iArgument3 == LUA_TSTRING )
                                    {
                                        const char* szType=lua_tostring(luaVM,-1);
                                        if (strcmp(szType,"fwd")==0)
                                            pEntry->SetCarDriveType(CHandlingEntry::eDriveType::FWD);
                                        else if (strcmp(szType,"rwd")==0)
                                            pEntry->SetCarDriveType(CHandlingEntry::eDriveType::RWD);
                                        else if (strcmp(szType,"awd")==0)
                                            pEntry->SetCarDriveType(CHandlingEntry::eDriveType::FOURWHEEL);
                                    }
                                }
                                else if (strcmp(szHandlingData,"engineType")==0)
                                {
                                    if ( iArgument3 == LUA_TSTRING )
                                    {
                                        const char* szType=lua_tostring(luaVM,-1);
                                        if (strcmp(szType,"petrol")==0)
                                            pEntry->SetCarEngineType(CHandlingEntry::eEngineType::PETROL);
                                        else if (strcmp(szType,"diesel")==0)
                                            pEntry->SetCarEngineType(CHandlingEntry::eEngineType::DIESEL);
                                        else if (strcmp(szType,"electric")==0)
                                            pEntry->SetCarEngineType(CHandlingEntry::eEngineType::ELECTRIC);
                                    }
                                }
                                else if (strcmp(szHandlingData,"numberOfGears")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetNumberOfGears ( static_cast < unsigned char > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"engineAcceleration")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetEngineAccelleration ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"engineInertia")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetEngineInertia ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"maxVelocity")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetMaxVelocity ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"brakeDeceleration")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetBrakeDecelleration ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"brakeBias")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetBrakeBias ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"ABS")==0)
                                {
                                    if ( iArgument3 == LUA_TBOOLEAN )
                                    {
                                        pEntry->SetABS ( lua_toboolean ( luaVM, -1 ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"steeringLock")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetSteeringLock ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"tractionLoss")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetTractionLoss ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"tractionBias")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetTractionBias ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"suspensionForceLevel")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetSuspensionForceLevel ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"suspensionDamping")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetSuspensionDamping ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"suspensionHighSpeedDamping")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetSuspensionHighSpeedDamping ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"suspensionUpperLimit")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetSuspensionUpperLimit ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"suspensionLowerLimit")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetSuspensionLowerLimit ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"suspensionFrontRearBias")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetSuspensionFrontRearBias ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"suspensionAntidiveMultiplier")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetSuspensionAntidiveMultiplier ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"collisionDamageMultiplier")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetCollisionDamageMultiplier ( static_cast < float > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"seatOffsetDistance")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetSeatOffsetDistance ( static_cast < unsigned int > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"monetary")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetMonetary ( static_cast < unsigned char > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                                else if (strcmp(szHandlingData,"headLight")==0)
                                {
                                    if ( iArgument3 == LUA_TSTRING )
                                    {
                                        const char* szType=lua_tostring(luaVM,-1);
                                        if (strcmp(szType,"long")==0)
                                            pEntry->SetHeadLight(CHandlingEntry::eLightType::LONG);
                                        else if (strcmp(szType,"small")==0)
                                            pEntry->SetHeadLight(CHandlingEntry::eLightType::SMALL);
                                        else if (strcmp(szType,"tall")==0)
                                            pEntry->SetHeadLight(CHandlingEntry::eLightType::TALL);
                                        else if (strcmp(szType,"big")==0)
                                            pEntry->SetHeadLight(CHandlingEntry::eLightType::BIG);
                                    }
                                }
                                else if (strcmp(szHandlingData,"tailLight")==0)
                                {
                                    if ( iArgument3 == LUA_TSTRING )
                                    {
                                        const char* szType=lua_tostring(luaVM,-1);
                                        if (strcmp(szType,"long")==0)
                                            pEntry->SetTailLight(CHandlingEntry::eLightType::LONG);
                                        else if (strcmp(szType,"small")==0)
                                            pEntry->SetTailLight(CHandlingEntry::eLightType::SMALL);
                                        else if (strcmp(szType,"tall")==0)
                                            pEntry->SetTailLight(CHandlingEntry::eLightType::TALL);
                                        else if (strcmp(szType,"big")==0)
                                            pEntry->SetTailLight(CHandlingEntry::eLightType::BIG);
                                    }
                                }
                                else if (strcmp(szHandlingData,"animGroup")==0)
                                {
                                    if ( iArgument3 == LUA_TNUMBER )
                                    {
                                        pEntry->SetAnimGroup ( static_cast < unsigned char > ( lua_tonumber ( luaVM, -1 ) ) );
                                    }
                                }
                            }
                        }
                    }
                    lua_pop ( luaVM, 1 );
                }
                pVehicle->ApplyHandling();
                lua_pushboolean ( luaVM, true );
                return 1;
            }
            else if ( lua_type ( luaVM, 2 ) == LUA_TNIL || lua_type ( luaVM, 2 ) == LUA_TNONE )
            {
                CHandlingEntry *pEntry=pVehicle->GetHandlingData ();
                const CHandlingEntry *pOriginalEntry=pVehicle->GetOriginalHandlingData ();
                pEntry->ApplyHandlingData ( (CHandlingEntry*)pOriginalEntry );
                pEntry->Recalculate ();
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleHandlingData" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleHandlingData ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA  )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CHandlingEntry* pEntry = pVehicle->GetHandlingData();
            if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
            {
                const char *szHandlingData = lua_tostring( luaVM, 2 );

                if (!szHandlingData || !pEntry)
                {
                    m_pScriptDebugging->LogBadType ( luaVM, "getVehicleHandlingData" );
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }

                if (strcmp(szHandlingData,"mass")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetMass () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"turnMass")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetTurnMass () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"dragCoeff")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetDragCoeff () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"centerOfMass")==0)
                {
                    CVector vecCenter = pEntry->GetCenterOfMass();
                    lua_newtable ( luaVM );
                    lua_pushnumber ( luaVM, vecCenter.fX );
                    lua_setfield ( luaVM, -2, "posX" );
                    lua_pushnumber ( luaVM, vecCenter.fY );
                    lua_setfield ( luaVM, -2, "posY" );
                    lua_pushnumber ( luaVM, vecCenter.fZ );
                    lua_setfield ( luaVM, -2, "posZ" );
                    return 1;
                }
                else if (strcmp(szHandlingData,"percentSubmerged")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetPercentSubmerged () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"tractionMultiplier")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetTractionMultiplier () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"driveType")==0)
                {
                    CHandlingEntry::eDriveType eType=pEntry->GetCarDriveType();
                    if (eType==CHandlingEntry::eDriveType::FWD)
                    {
                        lua_pushstring(luaVM,"fwd");
                        return 1;
                    }
                    else if (eType==CHandlingEntry::eDriveType::RWD)
                    {
                        lua_pushstring(luaVM,"rwd");
                        return 1;
                    }
                    else if (eType==CHandlingEntry::eDriveType::FOURWHEEL)
                    {
                        lua_pushstring(luaVM,"awd");
                        return 1;
                    }
                }
                else if (strcmp(szHandlingData,"engineType")==0)
                {
                    CHandlingEntry::eEngineType eType=pEntry->GetCarEngineType();
                    if (eType==CHandlingEntry::eEngineType::PETROL)
                    {
                        lua_pushstring(luaVM,"petrol");
                        return 1;
                    }
                    else if (eType==CHandlingEntry::eEngineType::DIESEL)
                    {
                        lua_pushstring(luaVM,"diesel");
                        return 1;
                    }
                    else if (eType==CHandlingEntry::eEngineType::ELECTRIC)
                    {
                        lua_pushstring(luaVM,"electric");
                        return 1;
                    }
                }
                else if (strcmp(szHandlingData,"numberOfGears")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetNumberOfGears() );
                    return 1;
                }
                else if (strcmp(szHandlingData,"engineAcceleration")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetEngineAccelleration () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"engineInertia")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetEngineInertia () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"maxVelocity")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetMaxVelocity () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"brakeDeceleration")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetBrakeDecelleration () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"brakeBias")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetBrakeBias () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"ABS")==0)
                {
                    lua_pushboolean ( luaVM, pEntry->GetABS () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"steeringLock")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetSteeringLock () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"tractionLoss")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetTractionLoss () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"tractionBias")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetTractionBias () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"suspensionForceLevel")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetSuspensionForceLevel () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"suspensionDamping")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetSuspensionDamping () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"suspensionHighSpeedDamping")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetSuspensionHighSpeedDamping () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"suspensionUpperLimit")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetSuspensionUpperLimit () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"suspensionLowerLimit")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetSuspensionLowerLimit () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"suspensionFrontRearBias")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetSuspensionFrontRearBias () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"suspensionAntidiveMultiplier")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetSuspensionAntidiveMultiplier () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"collisionDamageMultiplier")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetCollisionDamageMultiplier () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"seatOffsetDistance")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetSeatOffsetDistance () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"monetary")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetMonetary () );
                    return 1;
                }
                else if (strcmp(szHandlingData,"headLight")==0)
                {
                    CHandlingEntry::eLightType eType=pEntry->GetHeadLight();
                    if (eType==CHandlingEntry::eLightType::LONG)
                    {
                        lua_pushstring(luaVM,"long");
                        return 1;
                    }
                    else if (eType==CHandlingEntry::eLightType::SMALL)
                    {
                        lua_pushstring(luaVM,"small");
                        return 1;
                    }
                    else if (eType==CHandlingEntry::eLightType::BIG)
                    {
                        lua_pushstring(luaVM,"big");
                        return 1;
                    }
                    else if (eType==CHandlingEntry::eLightType::TALL)
                    {
                        lua_pushstring(luaVM,"tall");
                        return 1;
                    }
                }
                else if (strcmp(szHandlingData,"tailLight")==0)
                {
                    CHandlingEntry::eLightType eType=pEntry->GetTailLight();
                    if (eType==CHandlingEntry::eLightType::LONG)
                    {
                        lua_pushstring(luaVM,"long");
                        return 1;
                    }
                    else if (eType==CHandlingEntry::eLightType::SMALL)
                    {
                        lua_pushstring(luaVM,"small");
                        return 1;
                    }
                    else if (eType==CHandlingEntry::eLightType::BIG)
                    {
                        lua_pushstring(luaVM,"big");
                        return 1;
                    }
                    else if (eType==CHandlingEntry::eLightType::TALL)
                    {
                        lua_pushstring(luaVM,"tall");
                        return 1;
                    }
                }
                else if (strcmp(szHandlingData,"animGroup")==0)
                {
                    lua_pushnumber ( luaVM, pEntry->GetAnimGroup () );
                    return 1;
                }
                lua_pushboolean ( luaVM, false );
                return 1;
            }
            else if ( lua_type ( luaVM, 2 ) == LUA_TNIL || lua_type ( luaVM, 2 ) == LUA_TNONE ) 
            {
                lua_newtable ( luaVM );
                lua_pushnumber ( luaVM, pEntry->GetMass() );
                lua_setfield ( luaVM, -2, "mass" );
                lua_pushnumber ( luaVM, pEntry->GetTurnMass() );
                lua_setfield ( luaVM, -2, "turnMass" );
                lua_pushnumber ( luaVM, pEntry->GetDragCoeff() );
                lua_setfield ( luaVM, -2, "dragCoeff" );
                lua_newtable ( luaVM );
                CVector vecCenter = pEntry->GetCenterOfMass ();
                lua_pushnumber ( luaVM, vecCenter.fX );
                lua_setfield ( luaVM, -2, "posX" );
                lua_pushnumber ( luaVM, vecCenter.fY );
                lua_setfield ( luaVM, -2, "posY" );
                lua_pushnumber ( luaVM, vecCenter.fZ );
                lua_setfield ( luaVM, -2, "posZ" );
                lua_setfield ( luaVM, -2, "centerOfMass" );
                lua_pushnumber ( luaVM, pEntry->GetPercentSubmerged() );
                lua_setfield ( luaVM, -2, "percentSubmerged" );
                lua_pushnumber ( luaVM, pEntry->GetTractionMultiplier() );
                lua_setfield ( luaVM, -2, "tractionMultiplier" );
                CHandlingEntry::eDriveType eDriveType=pEntry->GetCarDriveType();
                if (eDriveType==CHandlingEntry::eDriveType::FWD)
                    lua_pushstring(luaVM,"fwd");
                else if (eDriveType==CHandlingEntry::eDriveType::RWD)
                    lua_pushstring(luaVM,"rwd");
                else if (eDriveType==CHandlingEntry::eDriveType::FOURWHEEL)
                    lua_pushstring(luaVM,"awd");
                else // What the ... (yeah, security)
                    lua_pushnil ( luaVM );
                lua_setfield ( luaVM, -2, "driveType" );
                CHandlingEntry::eEngineType eEngineType=pEntry->GetCarEngineType();
                if (eEngineType==CHandlingEntry::eEngineType::PETROL)
                    lua_pushstring(luaVM,"petrol");
                else if (eEngineType==CHandlingEntry::eEngineType::DIESEL)
                    lua_pushstring(luaVM,"diesel");
                else if (eEngineType==CHandlingEntry::eEngineType::ELECTRIC)
                    lua_pushstring(luaVM,"electric");
                else
                    lua_pushnil ( luaVM );
                lua_setfield ( luaVM, -2, "engineType" );
                lua_pushnumber ( luaVM, pEntry->GetNumberOfGears() );
                lua_setfield ( luaVM, -2, "numberOfGears" );
                lua_pushnumber ( luaVM, pEntry->GetEngineAccelleration() );
                lua_setfield ( luaVM, -2, "engineAcceleration" );
                lua_pushnumber ( luaVM, pEntry->GetEngineInertia() );
                lua_setfield ( luaVM, -2, "engineInertia" );
                lua_pushnumber ( luaVM, pEntry->GetMaxVelocity() );
                lua_setfield ( luaVM, -2, "maxVelocity" );
                lua_pushnumber ( luaVM, pEntry->GetBrakeDecelleration() );
                lua_setfield ( luaVM, -2, "brakeDeceleration" );
                lua_pushnumber ( luaVM, pEntry->GetBrakeBias() );
                lua_setfield ( luaVM, -2, "brakeBias" );
                lua_pushboolean ( luaVM, pEntry->GetABS() );
                lua_setfield ( luaVM, -2, "ABS" );
                lua_pushnumber ( luaVM, pEntry->GetSteeringLock() );
                lua_setfield ( luaVM, -2, "steeringLock" );
                lua_pushnumber ( luaVM, pEntry->GetTractionLoss() );
                lua_setfield ( luaVM, -2, "tractionLoss" );
                lua_pushnumber ( luaVM, pEntry->GetTractionBias() );
                lua_setfield ( luaVM, -2, "tractionBias" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionForceLevel() );
                lua_setfield ( luaVM, -2, "suspensionForceLevel" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionDamping() );
                lua_setfield ( luaVM, -2, "suspensionDamping" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionHighSpeedDamping() );
                lua_setfield ( luaVM, -2, "suspensionHighSpeedDamping" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionUpperLimit() );
                lua_setfield ( luaVM, -2, "suspensionUpperLimit" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionLowerLimit() );
                lua_setfield ( luaVM, -2, "suspensionLowerLimit" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionFrontRearBias() );
                lua_setfield ( luaVM, -2, "suspensionFrontRearBias" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionAntidiveMultiplier() );
                lua_setfield ( luaVM, -2, "suspensionAntidiveMultiplier" );
                lua_pushnumber ( luaVM, pEntry->GetCollisionDamageMultiplier() );
                lua_setfield ( luaVM, -2, "collisionDamageMultiplier" );
                lua_pushnumber ( luaVM, pEntry->GetSeatOffsetDistance() );
                lua_setfield ( luaVM, -2, "seatOffsetDistance" );
                lua_pushnumber ( luaVM, pEntry->GetMonetary() );
                lua_setfield ( luaVM, -2, "monetary" );
                CHandlingEntry::eLightType eHeadType=pEntry->GetHeadLight();
                if (eHeadType==CHandlingEntry::eLightType::LONG)
                    lua_pushstring(luaVM,"long");
                else if (eHeadType==CHandlingEntry::eLightType::SMALL)
                    lua_pushstring(luaVM,"small");
                else if (eHeadType==CHandlingEntry::eLightType::BIG)
                    lua_pushstring(luaVM,"big");
                else
                    lua_pushnil( luaVM );
                lua_setfield ( luaVM, -2, "headLight" );
                CHandlingEntry::eLightType eTailType=pEntry->GetHeadLight();
                if (eTailType==CHandlingEntry::eLightType::LONG)
                    lua_pushstring(luaVM,"long");
                else if (eTailType==CHandlingEntry::eLightType::SMALL)
                    lua_pushstring(luaVM,"small");
                else if (eTailType==CHandlingEntry::eLightType::BIG)
                    lua_pushstring(luaVM,"big");
                else
                    lua_pushnil( luaVM );
                lua_setfield ( luaVM, -2, "tailLight" );
                lua_pushnumber ( luaVM, pEntry->GetAnimGroup() );
                lua_setfield ( luaVM, -2, "animGroup" );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleHandlingData", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleHandlingData" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


// Returns the default vehicle handling
int CLuaFunctionDefs::GetDefaultHandling ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER && lua_tonumber ( luaVM, 1 ) >= 400 && lua_tonumber ( luaVM, 1 ) <= 612 )
    {
        const CHandlingEntry* pEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData((eVehicleTypes)(int)lua_tonumber(luaVM,1));
        if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            const char *szHandlingData = lua_tostring( luaVM, 2 );

            if (!szHandlingData || !pEntry)
            {
                m_pScriptDebugging->LogBadType ( luaVM, "getVehicleHandlingData" );
                lua_pushboolean ( luaVM, false );
                return 1;
            }

            if (strcmp(szHandlingData,"mass")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetMass () );
                return 1;
            }
            else if (strcmp(szHandlingData,"turnMass")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetTurnMass () );
                return 1;
            }
            else if (strcmp(szHandlingData,"dragCoeff")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetDragCoeff () );
                return 1;
            }
            else if (strcmp(szHandlingData,"centerOfMass")==0)
            {
                CVector vecCenter = pEntry->GetCenterOfMass();
                lua_newtable ( luaVM );
                lua_pushnumber ( luaVM, vecCenter.fX );
                lua_setfield ( luaVM, -2, "posX" );
                lua_pushnumber ( luaVM, vecCenter.fY );
                lua_setfield ( luaVM, -2, "posY" );
                lua_pushnumber ( luaVM, vecCenter.fZ );
                lua_setfield ( luaVM, -2, "posZ" );
                return 1;
            }
            else if (strcmp(szHandlingData,"percentSubmerged")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetPercentSubmerged () );
                return 1;
            }
            else if (strcmp(szHandlingData,"tractionMultiplier")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetTractionMultiplier () );
                return 1;
            }
            else if (strcmp(szHandlingData,"driveType")==0)
            {
                CHandlingEntry::eDriveType eType=pEntry->GetCarDriveType();
                if (eType==CHandlingEntry::eDriveType::FWD)
                {
                    lua_pushstring(luaVM,"fwd");
                    return 1;
                }
                else if (eType==CHandlingEntry::eDriveType::RWD)
                {
                    lua_pushstring(luaVM,"rwd");
                    return 1;
                }
                else if (eType==CHandlingEntry::eDriveType::FOURWHEEL)
                {
                    lua_pushstring(luaVM,"awd");
                    return 1;
                }
            }
            else if (strcmp(szHandlingData,"engineType")==0)
            {
                CHandlingEntry::eEngineType eType=pEntry->GetCarEngineType();
                if (eType==CHandlingEntry::eEngineType::PETROL)
                {
                    lua_pushstring(luaVM,"petrol");
                    return 1;
                }
                else if (eType==CHandlingEntry::eEngineType::DIESEL)
                {
                    lua_pushstring(luaVM,"diesel");
                    return 1;
                }
                else if (eType==CHandlingEntry::eEngineType::ELECTRIC)
                {
                    lua_pushstring(luaVM,"electric");
                    return 1;
                }
            }
            else if (strcmp(szHandlingData,"numberOfGears")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetNumberOfGears() );
                return 1;
            }
            else if (strcmp(szHandlingData,"engineAcceleration")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetEngineAccelleration () );
                return 1;
            }
            else if (strcmp(szHandlingData,"engineInertia")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetEngineInertia () );
                return 1;
            }
            else if (strcmp(szHandlingData,"maxVelocity")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetMaxVelocity () );
                return 1;
            }
            else if (strcmp(szHandlingData,"brakeDeceleration")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetBrakeDecelleration () );
                return 1;
            }
            else if (strcmp(szHandlingData,"brakeBias")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetBrakeBias () );
                return 1;
            }
            else if (strcmp(szHandlingData,"ABS")==0)
            {
                lua_pushboolean ( luaVM, pEntry->GetABS () );
                return 1;
            }
            else if (strcmp(szHandlingData,"steeringLock")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetSteeringLock () );
                return 1;
            }
            else if (strcmp(szHandlingData,"tractionLoss")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetTractionLoss () );
                return 1;
            }
            else if (strcmp(szHandlingData,"tractionBias")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetTractionBias () );
                return 1;
            }
            else if (strcmp(szHandlingData,"suspensionForceLevel")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetSuspensionForceLevel () );
                return 1;
            }
            else if (strcmp(szHandlingData,"suspensionDamping")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetSuspensionDamping () );
                return 1;
            }
            else if (strcmp(szHandlingData,"suspensionHighSpeedDamping")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetSuspensionHighSpeedDamping () );
                return 1;
            }
            else if (strcmp(szHandlingData,"suspensionUpperLimit")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetSuspensionUpperLimit () );
                return 1;
            }
            else if (strcmp(szHandlingData,"suspensionLowerLimit")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetSuspensionLowerLimit () );
                return 1;
            }
            else if (strcmp(szHandlingData,"suspensionFrontRearBias")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetSuspensionFrontRearBias () );
                return 1;
            }
            else if (strcmp(szHandlingData,"suspensionAntidiveMultiplier")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetSuspensionAntidiveMultiplier () );
                return 1;
            }
            else if (strcmp(szHandlingData,"collisionDamageMultiplier")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetCollisionDamageMultiplier () );
                return 1;
            }
            else if (strcmp(szHandlingData,"seatOffsetDistance")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetSeatOffsetDistance () );
                return 1;
            }
            else if (strcmp(szHandlingData,"monetary")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetMonetary () );
                return 1;
            }
            else if (strcmp(szHandlingData,"headLight")==0)
            {
                CHandlingEntry::eLightType eType=pEntry->GetHeadLight();
                if (eType==CHandlingEntry::eLightType::LONG)
                {
                    lua_pushstring(luaVM,"long");
                    return 1;
                }
                else if (eType==CHandlingEntry::eLightType::SMALL)
                {
                    lua_pushstring(luaVM,"small");
                    return 1;
                }
                else if (eType==CHandlingEntry::eLightType::BIG)
                {
                    lua_pushstring(luaVM,"big");
                    return 1;
                }
                else if (eType==CHandlingEntry::eLightType::TALL)
                {
                    lua_pushstring(luaVM,"tall");
                    return 1;
                }
            }
            else if (strcmp(szHandlingData,"tailLight")==0)
            {
                CHandlingEntry::eLightType eType=pEntry->GetTailLight();
                if (eType==CHandlingEntry::eLightType::LONG)
                {
                    lua_pushstring(luaVM,"long");
                    return 1;
                }
                else if (eType==CHandlingEntry::eLightType::SMALL)
                {
                    lua_pushstring(luaVM,"small");
                    return 1;
                }
                else if (eType==CHandlingEntry::eLightType::BIG)
                {
                    lua_pushstring(luaVM,"big");
                    return 1;
                }
                else if (eType==CHandlingEntry::eLightType::TALL)
                {
                    lua_pushstring(luaVM,"tall");
                    return 1;
                }
            }
            else if (strcmp(szHandlingData,"animGroup")==0)
            {
                lua_pushnumber ( luaVM, pEntry->GetAnimGroup () );
                return 1;
            }
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        else if ( lua_type ( luaVM, 2 ) == LUA_TNIL || lua_type ( luaVM, 2 ) == LUA_TNONE )
        {
            if ( pEntry )
            {
                lua_newtable ( luaVM );
                lua_pushnumber ( luaVM, pEntry->GetMass() );
                lua_setfield ( luaVM, -2, "mass" );
                lua_pushnumber ( luaVM, pEntry->GetTurnMass() );
                lua_setfield ( luaVM, -2, "turnMass" );
                lua_pushnumber ( luaVM, pEntry->GetDragCoeff() );
                lua_setfield ( luaVM, -2, "dragCoeff" );
                lua_newtable ( luaVM );
                CVector vecCenter = pEntry->GetCenterOfMass ();
                lua_pushnumber ( luaVM, vecCenter.fX );
                lua_setfield ( luaVM, -2, "posX" );
                lua_pushnumber ( luaVM, vecCenter.fY );
                lua_setfield ( luaVM, -2, "posY" );
                lua_pushnumber ( luaVM, vecCenter.fZ );
                lua_setfield ( luaVM, -2, "posZ" );
                lua_setfield ( luaVM, -2, "centerOfMass" );
                lua_pushnumber ( luaVM, pEntry->GetPercentSubmerged() );
                lua_setfield ( luaVM, -2, "percentSubmerged" );
                lua_pushnumber ( luaVM, pEntry->GetTractionMultiplier() );
                lua_setfield ( luaVM, -2, "tractionMultiplier" );
                CHandlingEntry::eDriveType eDriveType=pEntry->GetCarDriveType();
                if (eDriveType==CHandlingEntry::eDriveType::FWD)
                    lua_pushstring(luaVM,"fwd");
                else if (eDriveType==CHandlingEntry::eDriveType::RWD)
                    lua_pushstring(luaVM,"rwd");
                else if (eDriveType==CHandlingEntry::eDriveType::FOURWHEEL)
                    lua_pushstring(luaVM,"awd");
                else // What the ... (yeah, security)
                    lua_pushnil ( luaVM );
                lua_setfield ( luaVM, -2, "driveType" );
                CHandlingEntry::eEngineType eEngineType=pEntry->GetCarEngineType();
                if (eEngineType==CHandlingEntry::eEngineType::PETROL)
                    lua_pushstring(luaVM,"petrol");
                else if (eEngineType==CHandlingEntry::eEngineType::DIESEL)
                    lua_pushstring(luaVM,"diesel");
                else if (eEngineType==CHandlingEntry::eEngineType::ELECTRIC)
                    lua_pushstring(luaVM,"electric");
                else
                    lua_pushnil ( luaVM );
                lua_setfield ( luaVM, -2, "engineType" );
                lua_pushnumber ( luaVM, pEntry->GetNumberOfGears() );
                lua_setfield ( luaVM, -2, "numberOfGears" );
                lua_pushnumber ( luaVM, pEntry->GetEngineAccelleration() );
                lua_setfield ( luaVM, -2, "engineAcceleration" );
                lua_pushnumber ( luaVM, pEntry->GetEngineInertia() );
                lua_setfield ( luaVM, -2, "engineInertia" );
                lua_pushnumber ( luaVM, pEntry->GetMaxVelocity() );
                lua_setfield ( luaVM, -2, "maxVelocity" );
                lua_pushnumber ( luaVM, pEntry->GetBrakeDecelleration() );
                lua_setfield ( luaVM, -2, "brakeDeceleration" );
                lua_pushnumber ( luaVM, pEntry->GetBrakeBias() );
                lua_setfield ( luaVM, -2, "brakeBias" );
                lua_pushboolean ( luaVM, pEntry->GetABS() );
                lua_setfield ( luaVM, -2, "ABS" );
                lua_pushnumber ( luaVM, pEntry->GetSteeringLock() );
                lua_setfield ( luaVM, -2, "steeringLock" );
                lua_pushnumber ( luaVM, pEntry->GetTractionLoss() );
                lua_setfield ( luaVM, -2, "tractionLoss" );
                lua_pushnumber ( luaVM, pEntry->GetTractionBias() );
                lua_setfield ( luaVM, -2, "tractionBias" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionForceLevel() );
                lua_setfield ( luaVM, -2, "suspensionForceLevel" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionDamping() );
                lua_setfield ( luaVM, -2, "suspensionDamping" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionHighSpeedDamping() );
                lua_setfield ( luaVM, -2, "suspensionHighSpeedDamping" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionUpperLimit() );
                lua_setfield ( luaVM, -2, "suspensionUpperLimit" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionLowerLimit() );
                lua_setfield ( luaVM, -2, "suspensionLowerLimit" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionFrontRearBias() );
                lua_setfield ( luaVM, -2, "suspensionFrontRearBias" );
                lua_pushnumber ( luaVM, pEntry->GetSuspensionAntidiveMultiplier() );
                lua_setfield ( luaVM, -2, "suspensionAntidiveMultiplier" );
                lua_pushnumber ( luaVM, pEntry->GetCollisionDamageMultiplier() );
                lua_setfield ( luaVM, -2, "collisionDamageMultiplier" );
                lua_pushnumber ( luaVM, pEntry->GetSeatOffsetDistance() );
                lua_setfield ( luaVM, -2, "seatOffsetDistance" );
                lua_pushnumber ( luaVM, pEntry->GetMonetary() );
                lua_setfield ( luaVM, -2, "monetary" );
                CHandlingEntry::eLightType eHeadType=pEntry->GetHeadLight();
                if (eHeadType==CHandlingEntry::eLightType::LONG)
                    lua_pushstring(luaVM,"long");
                else if (eHeadType==CHandlingEntry::eLightType::SMALL)
                    lua_pushstring(luaVM,"small");
                else if (eHeadType==CHandlingEntry::eLightType::BIG)
                    lua_pushstring(luaVM,"big");
                else
                    lua_pushnil( luaVM );
                lua_setfield ( luaVM, -2, "headLight" );
                CHandlingEntry::eLightType eTailType=pEntry->GetHeadLight();
                if (eTailType==CHandlingEntry::eLightType::LONG)
                    lua_pushstring(luaVM,"long");
                else if (eTailType==CHandlingEntry::eLightType::SMALL)
                    lua_pushstring(luaVM,"small");
                else if (eTailType==CHandlingEntry::eLightType::BIG)
                    lua_pushstring(luaVM,"big");
                else
                    lua_pushnil( luaVM );
                lua_setfield ( luaVM, -2, "tailLight" );
                lua_pushnumber ( luaVM, pEntry->GetAnimGroup() );
                lua_setfield ( luaVM, -2, "animGroup" );
                return 1;
            }
        }   
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}
