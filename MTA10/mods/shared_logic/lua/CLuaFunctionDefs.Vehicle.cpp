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
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleVariant ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucVariant = -1;
            unsigned char ucVariant2 = -1;
            if ( CStaticFunctionDefinitions::GetVehicleVariant ( pVehicle, ucVariant, ucVariant2 ) )
            {
                lua_pushnumber ( luaVM, ucVariant );
                lua_pushnumber ( luaVM, ucVariant2 );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        bool bRGB = false;
        if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            bRGB = lua_toboolean ( luaVM, 2 ) ? true : false;

        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle ) 
        {
            CVehicleColor& color = pVehicle->GetColor ();

            if ( bRGB )
            {
                for ( uint i = 0 ; i < 4 ; i++ )
                {
                    SColor RGBColor = color.GetRGBColor ( i );
                    lua_pushnumber ( luaVM, RGBColor.R );
                    lua_pushnumber ( luaVM, RGBColor.G );
                    lua_pushnumber ( luaVM, RGBColor.B );
                }
                return 12;
            }
            else
            {
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 0 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 1 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 2 ) );
                lua_pushnumber ( luaVM, color.GetPaletteColor ( 3 ) );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::GetVehicleMaxPassengers ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA || lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        unsigned short model = 0;

        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA)
        {
            CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
            if ( pVehicle )
                model = pVehicle->GetModel();
            else
            {
                m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }
        else
        {
            model = (unsigned short) lua_tonumber ( luaVM, 1 );

            if (!CClientVehicleManager::IsValidModel(model))
            {
                m_pScriptDebugging->LogBadType ( luaVM );
                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }
        
        unsigned int uiMaxPassengers = CClientVehicleManager::GetMaxPassengerCount ( model );
        if ( uiMaxPassengers != 0xFF )
        {
            lua_pushnumber ( luaVM, uiMaxPassengers );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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

            // Get the maximum amount of passengers
            unsigned char ucMaxPassengers = CClientVehicleManager::GetMaxPassengerCount ( pVehicle->GetModel ( ) );

            // Make sure that if the vehicle doesn't have any seats, the function returns false
            if ( ucMaxPassengers == 255 )
            {
                lua_pushboolean ( luaVM, false );
                return 1;
            }

            // Add All Occupants
            for ( unsigned char ucSeat = 0; ucSeat <= ucMaxPassengers; ++ ucSeat )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );
            
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            if ( CClientVehicleManager::HasSirens ( pVehicle->GetModel () ) || pVehicle->DoesVehicleHaveSirens ( ) )
            {
                // Return whether it has its Sirens on or not
                bool bSirensOn = pVehicle->IsSirenOrAlarmActive ();
                lua_pushboolean ( luaVM, bSirensOn );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
                const SSlotStates& usSlotStates = pUpgrades->GetSlotStates ();

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            SString strUpgradeName;
            if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( static_cast < unsigned char > ( ulNumber ), strUpgradeName ) )
            {
                lua_pushstring ( luaVM, strUpgradeName );
                return 1;
            }
        }
        else if ( ulNumber >= 1000 && ulNumber <= 1193 )
        {
            SString strUpgradeName;
            if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( static_cast < unsigned short > ( ulNumber ), strUpgradeName ) )
            {
                lua_pushstring ( luaVM, strUpgradeName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "slot/upgrade", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsVehicleDamageProof ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            bool bDamageProof;
            if ( CStaticFunctionDefinitions::IsVehicleDamageProof ( *pVehicle, bDamageProof ) )
            {
                lua_pushboolean ( luaVM, bDamageProof );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleNameFromModel ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned short usModel = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );
        SString strVehicleName;

        if ( CStaticFunctionDefinitions::GetVehicleNameFromModel ( usModel, strVehicleName ) )
        {
            lua_pushstring ( luaVM, strVehicleName );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
        unsigned char ucVariant = 255;
        unsigned char ucVariant2 = 255;
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

                    int iArgument8 = lua_type ( luaVM, 8 );
                    if ( iArgument8 == LUA_TSTRING || iArgument8 == LUA_TBOOLEAN )
                    {
                        if ( iArgument8 == LUA_TSTRING )
                        {
                            szRegPlate = lua_tostring ( luaVM, 8 );
                        }
                        int iArgument9 = lua_type ( luaVM, 9 );
                        if ( iArgument9 == LUA_TNUMBER || iArgument9 == LUA_TSTRING )
                        {
                            ucVariant = static_cast < unsigned char > ( atoi ( lua_tostring ( luaVM, 9 ) ) );
                            int iArgument10 = lua_type ( luaVM, 10 );
                            if ( iArgument10 == LUA_TNUMBER || iArgument10 == LUA_TSTRING )
                            {
                                ucVariant2 = static_cast < unsigned char > ( atoi ( lua_tostring ( luaVM, 10 ) ) );
                            }
                        }
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
                CClientVehicle* pVehicle = CStaticFunctionDefinitions::CreateVehicle ( *pResource, usModel, vecPosition, vecRotation, szRegPlate, ucVariant, ucVariant2 );
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
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::BlowVehicle ( lua_State* luaVM )
{
    // Verify the element pointer argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::BlowVehicle ( *pEntity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleCurrentGear ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned short currentGear;
            if ( CStaticFunctionDefinitions::GetVehicleCurrentGear ( *pVehicle, currentGear ) )
            {
                lua_pushnumber ( luaVM, currentGear );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
	else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleColor ( lua_State* luaVM )
{
    // Count up number of args after the first one
    uchar ucParams[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int i;
    for ( i = 0 ; i < 12 ; i++ )
    {
        int iArgumentType = lua_type ( luaVM, i + 2 );
        if ( iArgumentType == LUA_TNUMBER || iArgumentType == LUA_TSTRING )
        {
            ucParams[i] = static_cast < unsigned char > ( Clamp ( 0.0, lua_tonumber ( luaVM, i + 2 ), 255.0 ) );
        }
        else
            break;
    }

    if  ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
          ( i == 3 || i == 4 || i == 6 || i == 9 || i == 12 ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            CVehicleColor color;

            if ( i == 4 )
            {
                // 4 args mean palette colours
                color.SetPaletteColors ( ucParams[0], ucParams[1], ucParams[2], ucParams[3] );
            }
            else
            {
                // 3,6,9 or 12 args mean rgb colours
                color.SetRGBColors ( SColorRGBA ( ucParams[0], ucParams[1], ucParams[2], 0 ),
                                     SColorRGBA ( ucParams[3], ucParams[4], ucParams[5], 0 ),
                                     SColorRGBA ( ucParams[6], ucParams[7], ucParams[8], 0 ),
                                     SColorRGBA ( ucParams[9], ucParams[10], ucParams[11], 0 ) );
            }

            if ( CStaticFunctionDefinitions::SetVehicleColor ( *pEntity, color ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
                m_pScriptDebugging->LogBadPointer ( luaVM, "trailer", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
                m_pScriptDebugging->LogBadType ( luaVM );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
    static const SFixedArray < const char*, 13 > szRadioStations = { {
        "Radio off", "Playback FM", "K-Rose", "K-DST",
        "Bounce FM", "SF-UR", "Radio Los Santos", "Radio X", "CSR 103.9", "K-Jah West",
        "Master Sounds 98.3", "WCTR", "User Track Player" } };

    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        int iChannel = static_cast < int > ( lua_tonumber ( luaVM, 1 ) );
        if ( iChannel >= 0 && iChannel < NUMELMS( szRadioStations ) ) {
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            SColor color;
            color.R = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            color.G = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
            color.B = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
            color.A = 255;

            if ( CStaticFunctionDefinitions::SetVehicleHeadLightColor ( *pEntity, color ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleTurretPosition ( lua_State *luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA
        && lua_type( luaVM, 2 ) == LUA_TNUMBER
        && lua_type( luaVM, 3 ) == LUA_TNUMBER )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            float fHorizontal = ( float ) lua_tonumber ( luaVM, 2 );
            float fVertical   = ( float ) lua_tonumber ( luaVM, 3 );

            pVehicle->SetTurretRotation ( fHorizontal, fVertical );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleHandling ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CHandlingEntry* pEntry = pVehicle->GetHandlingData ();

            lua_newtable ( luaVM );

            lua_pushnumber ( luaVM, pEntry->GetMass() );
            lua_setfield ( luaVM, -2, "mass" );

            lua_pushnumber ( luaVM, pEntry->GetTurnMass() );
            lua_setfield ( luaVM, -2, "turnMass" );

            lua_pushnumber ( luaVM, pEntry->GetDragCoeff() );
            lua_setfield ( luaVM, -2, "dragCoeff" );

            lua_createtable ( luaVM, 3, 0 );
            CVector vecCenter = pEntry->GetCenterOfMass ();
            lua_pushnumber ( luaVM, 1 );
            lua_pushnumber ( luaVM, vecCenter.fX );
            lua_settable ( luaVM, -3 );
            lua_pushnumber ( luaVM, 2 );
            lua_pushnumber ( luaVM, vecCenter.fY );
            lua_settable ( luaVM, -3 );
            lua_pushnumber ( luaVM, 3 );
            lua_pushnumber ( luaVM, vecCenter.fZ );
            lua_settable ( luaVM, -3 );
            lua_setfield ( luaVM, -2, "centerOfMass" );

            lua_pushnumber ( luaVM, pEntry->GetPercentSubmerged() );
            lua_setfield ( luaVM, -2, "percentSubmerged" );

            lua_pushnumber ( luaVM, pEntry->GetTractionMultiplier() );
            lua_setfield ( luaVM, -2, "tractionMultiplier" );

            CHandlingEntry::eDriveType eDriveType = pEntry->GetCarDriveType ();
            if ( eDriveType == CHandlingEntry::FWD )
                lua_pushstring ( luaVM, "fwd" );
            else if ( eDriveType==CHandlingEntry::RWD )
                lua_pushstring ( luaVM, "rwd" );
            else if ( eDriveType==CHandlingEntry::FOURWHEEL )
                lua_pushstring ( luaVM, "awd" );
            else // What the ... (yeah, security)
                lua_pushnil ( luaVM );
            lua_setfield ( luaVM, -2, "driveType" );
            CHandlingEntry::eEngineType eEngineType = pEntry->GetCarEngineType ();
            if ( eEngineType == CHandlingEntry::PETROL )
                lua_pushstring ( luaVM, "petrol" );
            else if ( eEngineType == CHandlingEntry::DIESEL )
                lua_pushstring ( luaVM, "diesel" );
            else if ( eEngineType == CHandlingEntry::ELECTRIC )
                lua_pushstring ( luaVM, "electric" );
            else
                lua_pushnil ( luaVM );
            lua_setfield ( luaVM, -2, "engineType" );

            lua_pushnumber ( luaVM, pEntry->GetNumberOfGears () );
            lua_setfield ( luaVM, -2, "numberOfGears" );

            lua_pushnumber ( luaVM, pEntry->GetEngineAcceleration () );
            lua_setfield ( luaVM, -2, "engineAcceleration" );

            lua_pushnumber ( luaVM, pEntry->GetEngineInertia () );
            lua_setfield ( luaVM, -2, "engineInertia" );

            lua_pushnumber ( luaVM, pEntry->GetMaxVelocity () );
            lua_setfield ( luaVM, -2, "maxVelocity" );

            lua_pushnumber ( luaVM, pEntry->GetBrakeDeceleration () );
            lua_setfield ( luaVM, -2, "brakeDeceleration" );

            lua_pushnumber ( luaVM, pEntry->GetBrakeBias () );
            lua_setfield ( luaVM, -2, "brakeBias" );

            lua_pushboolean ( luaVM, pEntry->GetABS () );
            lua_setfield ( luaVM, -2, "ABS" );

            lua_pushnumber ( luaVM, pEntry->GetSteeringLock () );
            lua_setfield ( luaVM, -2, "steeringLock" );

            lua_pushnumber ( luaVM, pEntry->GetTractionLoss () );
            lua_setfield ( luaVM, -2, "tractionLoss" );

            lua_pushnumber ( luaVM, pEntry->GetTractionBias () );
            lua_setfield ( luaVM, -2, "tractionBias" );

            lua_pushnumber ( luaVM, pEntry->GetSuspensionForceLevel () );
            lua_setfield ( luaVM, -2, "suspensionForceLevel" );

            lua_pushnumber ( luaVM, pEntry->GetSuspensionDamping () );
            lua_setfield ( luaVM, -2, "suspensionDamping" );

            lua_pushnumber ( luaVM, pEntry->GetSuspensionHighSpeedDamping () );
            lua_setfield ( luaVM, -2, "suspensionHighSpeedDamping" );

            lua_pushnumber ( luaVM, pEntry->GetSuspensionUpperLimit () );
            lua_setfield ( luaVM, -2, "suspensionUpperLimit" );

            lua_pushnumber ( luaVM, pEntry->GetSuspensionLowerLimit () );
            lua_setfield ( luaVM, -2, "suspensionLowerLimit" );

            lua_pushnumber ( luaVM, pEntry->GetSuspensionFrontRearBias () );
            lua_setfield ( luaVM, -2, "suspensionFrontRearBias" );

            lua_pushnumber ( luaVM, pEntry->GetSuspensionAntiDiveMultiplier () );
            lua_setfield ( luaVM, -2, "suspensionAntiDiveMultiplier" );

            lua_pushnumber ( luaVM, pEntry->GetCollisionDamageMultiplier () );
            lua_setfield ( luaVM, -2, "collisionDamageMultiplier" );

            lua_pushnumber ( luaVM, pEntry->GetSeatOffsetDistance () );
            lua_setfield ( luaVM, -2, "seatOffsetDistance" );

            lua_pushnumber ( luaVM, pEntry->GetHandlingFlags () );
            lua_setfield ( luaVM, -2, "handlingFlags" );

            lua_pushnumber ( luaVM, pEntry->GetModelFlags () );
            lua_setfield ( luaVM, -2, "modelFlags" );

            lua_pushnumber ( luaVM, pEntry->GetMonetary () );
            lua_setfield ( luaVM, -2, "monetary" );

            CHandlingEntry::eLightType eHeadType = pEntry->GetHeadLight ();
            if ( eHeadType == CHandlingEntry::LONG )
                lua_pushstring ( luaVM, "long" );
            else if ( eHeadType == CHandlingEntry::SMALL )
                lua_pushstring ( luaVM, "small" );
            else if ( eHeadType == CHandlingEntry::BIG )
                lua_pushstring ( luaVM, "big" );
            else
                lua_pushnil( luaVM );
            lua_setfield ( luaVM, -2, "headLight" );

            CHandlingEntry::eLightType eTailType = pEntry->GetTailLight ();
            if ( eTailType == CHandlingEntry::LONG )
                lua_pushstring ( luaVM, "long" );
            else if ( eTailType == CHandlingEntry::SMALL )
                lua_pushstring ( luaVM, "small" );
            else if ( eTailType == CHandlingEntry::BIG )
                lua_pushstring ( luaVM, "big" );
            else
                lua_pushnil( luaVM );
            lua_setfield ( luaVM, -2, "tailLight" );

            lua_pushnumber ( luaVM, pEntry->GetAnimGroup () );
            lua_setfield ( luaVM, -2, "animGroup" );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetOriginalHandling ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        eVehicleTypes eModel = static_cast < eVehicleTypes > ( (int)lua_tonumber ( luaVM, 1 ) );
        if ( eModel )
        {
            const CHandlingEntry* pEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData( eModel );
            if ( pEntry )
            {
                lua_newtable ( luaVM );
                lua_pushnumber ( luaVM, pEntry->GetMass() );
                lua_setfield ( luaVM, -2, "mass" );
                lua_pushnumber ( luaVM, pEntry->GetTurnMass() );
                lua_setfield ( luaVM, -2, "turnMass" );
                lua_pushnumber ( luaVM, pEntry->GetDragCoeff() );
                lua_setfield ( luaVM, -2, "dragCoeff" );
                lua_createtable ( luaVM, 3, 0 );
                CVector vecCenter = pEntry->GetCenterOfMass ();
                lua_pushnumber ( luaVM, 1 );
                lua_pushnumber ( luaVM, vecCenter.fX );
                lua_settable ( luaVM, -3 );
                lua_pushnumber ( luaVM, 2 );
                lua_pushnumber ( luaVM, vecCenter.fY );
                lua_settable ( luaVM, -3 );
                lua_pushnumber ( luaVM, 3 );
                lua_pushnumber ( luaVM, vecCenter.fZ );
                lua_settable ( luaVM, -3 );
                lua_setfield ( luaVM, -2, "centerOfMass" );
                lua_pushnumber ( luaVM, pEntry->GetPercentSubmerged() );
                lua_setfield ( luaVM, -2, "percentSubmerged" );
                lua_pushnumber ( luaVM, pEntry->GetTractionMultiplier() );
                lua_setfield ( luaVM, -2, "tractionMultiplier" );
                CHandlingEntry::eDriveType eDriveType=pEntry->GetCarDriveType();
                if (eDriveType==CHandlingEntry::FWD)
                    lua_pushstring(luaVM,"fwd");
                else if (eDriveType==CHandlingEntry::RWD)
                    lua_pushstring(luaVM,"rwd");
                else if (eDriveType==CHandlingEntry::FOURWHEEL)
                    lua_pushstring(luaVM,"awd");
                else // What the ... (yeah, security)
                    lua_pushnil ( luaVM );
                lua_setfield ( luaVM, -2, "driveType" );
                CHandlingEntry::eEngineType eEngineType=pEntry->GetCarEngineType();
                if (eEngineType==CHandlingEntry::PETROL)
                    lua_pushstring(luaVM,"petrol");
                else if (eEngineType==CHandlingEntry::DIESEL)
                    lua_pushstring(luaVM,"diesel");
                else if (eEngineType==CHandlingEntry::ELECTRIC)
                    lua_pushstring(luaVM,"electric");
                else
                    lua_pushnil ( luaVM );
                lua_setfield ( luaVM, -2, "engineType" );
                lua_pushnumber ( luaVM, pEntry->GetNumberOfGears() );
                lua_setfield ( luaVM, -2, "numberOfGears" );
                lua_pushnumber ( luaVM, pEntry->GetEngineAcceleration() );
                lua_setfield ( luaVM, -2, "engineAcceleration" );
                lua_pushnumber ( luaVM, pEntry->GetEngineInertia() );
                lua_setfield ( luaVM, -2, "engineInertia" );
                lua_pushnumber ( luaVM, pEntry->GetMaxVelocity() );
                lua_setfield ( luaVM, -2, "maxVelocity" );
                lua_pushnumber ( luaVM, pEntry->GetBrakeDeceleration() );
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
                lua_pushnumber ( luaVM, pEntry->GetSuspensionAntiDiveMultiplier() );
                lua_setfield ( luaVM, -2, "suspensionAntiDiveMultiplier" );
                lua_pushnumber ( luaVM, pEntry->GetCollisionDamageMultiplier() );
                lua_setfield ( luaVM, -2, "collisionDamageMultiplier" );
                lua_pushnumber ( luaVM, pEntry->GetSeatOffsetDistance() );
                lua_setfield ( luaVM, -2, "seatOffsetDistance" );
                lua_pushnumber ( luaVM, pEntry->GetHandlingFlags() );
                lua_setfield ( luaVM, -2, "handlingFlags" );
                lua_pushnumber ( luaVM, pEntry->GetModelFlags() );
                lua_setfield ( luaVM, -2, "modelFlags" );
                lua_pushnumber ( luaVM, pEntry->GetMonetary() );
                lua_setfield ( luaVM, -2, "monetary" );
                CHandlingEntry::eLightType eHeadType=pEntry->GetHeadLight();
                if (eHeadType==CHandlingEntry::LONG)
                    lua_pushstring(luaVM,"long");
                else if (eHeadType==CHandlingEntry::SMALL)
                    lua_pushstring(luaVM,"small");
                else if (eHeadType==CHandlingEntry::BIG)
                    lua_pushstring(luaVM,"big");
                else
                    lua_pushnil( luaVM );
                lua_setfield ( luaVM, -2, "headLight" );
                CHandlingEntry::eLightType eTailType=pEntry->GetTailLight();
                if (eTailType==CHandlingEntry::LONG)
                    lua_pushstring(luaVM,"long");
                else if (eTailType==CHandlingEntry::SMALL)
                    lua_pushstring(luaVM,"small");
                else if (eTailType==CHandlingEntry::BIG)
                    lua_pushstring(luaVM,"big");
                else
                    lua_pushnil( luaVM );
                lua_setfield ( luaVM, -2, "tailLight" );
                lua_pushnumber ( luaVM, pEntry->GetAnimGroup() );
                lua_setfield ( luaVM, -2, "animGroup" );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "model", 1 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "model", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleDoorOpenRatio ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TNUMBER &&
         lua_type ( luaVM, 3 ) == LUA_TNUMBER )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned char ucDoor = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            float fRatio = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
            unsigned long ulTime = 0UL;

            if ( lua_type ( luaVM, 4 ) == LUA_TNUMBER )
                ulTime = static_cast < unsigned long > ( lua_tonumber ( luaVM, 4 ) );

            if ( CStaticFunctionDefinitions::SetVehicleDoorOpenRatio ( *pEntity, ucDoor, fRatio, ulTime ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleSirenParams( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CClientVehicle* pVehicle = NULL;
    unsigned char ucSirenID = 0;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    if ( argStream.HasErrors ( ) == false )
    {
        if ( pVehicle )
        {
            tSirenInfo = pVehicle->m_tSirenBeaconInfo;// Grab the siren structure data
            lua_newtable ( luaVM );

            lua_pushstring( luaVM, "SirenCount" );
            lua_pushnumber ( luaVM, tSirenInfo.m_ucSirenCount );
            lua_settable ( luaVM, -3 ); // End of SirenCount Property

            lua_pushstring( luaVM, "SirenType" );
            lua_pushnumber ( luaVM, tSirenInfo.m_ucSirenType );
            lua_settable ( luaVM, -3 ); // End of SirenType Property

            lua_pushstring( luaVM, "Flags" );
            lua_newtable ( luaVM );

            lua_pushstring ( luaVM, "360" );
            lua_pushboolean ( luaVM, tSirenInfo.m_b360Flag );
            lua_settable ( luaVM, -3 ); // End of 360 Property

            lua_pushstring ( luaVM, "DoLOSCheck" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bDoLOSCheck );
            lua_settable ( luaVM, -3 ); // End of DoLOSCheck Property

            lua_pushstring ( luaVM, "UseRandomiser" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bUseRandomiser );
            lua_settable ( luaVM, -3 ); // End of UseRandomiser Property

            lua_pushstring ( luaVM, "Silent" );
            lua_pushboolean ( luaVM, tSirenInfo.m_bSirenSilent );
            lua_settable ( luaVM, -3 ); // End of Silent Property

            lua_settable ( luaVM, -3 ); // End of table

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleSirens( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CClientVehicle* pVehicle = NULL;
    unsigned char ucSirenID = 0;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    if ( argStream.HasErrors ( ) == false )
    {
        if ( pVehicle )
        {
            tSirenInfo = pVehicle->m_tSirenBeaconInfo;// Grab the siren structure data
            lua_newtable ( luaVM );

            for ( int i = 0; i <= tSirenInfo.m_ucSirenCount;i++ )
            {
                lua_pushnumber ( luaVM, i+1 );
                lua_newtable ( luaVM );

                lua_pushstring( luaVM, "Min_Alpha" );
                lua_pushnumber ( luaVM, tSirenInfo.m_tSirenInfo[i].m_dwMinSirenAlpha );
                lua_settable ( luaVM, -3 ); // End of Min_Alpha property

                lua_pushstring( luaVM, "Red" );
                lua_pushnumber ( luaVM, tSirenInfo.m_tSirenInfo[i].m_RGBBeaconColour.R );
                lua_settable ( luaVM, -3 ); // End of Red property

                lua_pushstring( luaVM, "Green" );
                lua_pushnumber ( luaVM, tSirenInfo.m_tSirenInfo[i].m_RGBBeaconColour.G );
                lua_settable ( luaVM, -3 ); // End of Green property

                lua_pushstring( luaVM, "Blue" );
                lua_pushnumber ( luaVM, tSirenInfo.m_tSirenInfo[i].m_RGBBeaconColour.B );
                lua_settable ( luaVM, -3 ); // End of Blue property

                lua_pushstring( luaVM, "Alpha" );
                lua_pushnumber ( luaVM, tSirenInfo.m_tSirenInfo[i].m_RGBBeaconColour.A );
                lua_settable ( luaVM, -3 ); // End of Alpha property

                lua_pushstring( luaVM, "x" );
                lua_pushnumber ( luaVM, tSirenInfo.m_tSirenInfo[i].m_vecSirenPositions.fX );
                lua_settable ( luaVM, -3 ); // End of X property

                lua_pushstring( luaVM, "y" );
                lua_pushnumber ( luaVM, tSirenInfo.m_tSirenInfo[i].m_vecSirenPositions.fY );
                lua_settable ( luaVM, -3 ); // End of Y property

                lua_pushstring( luaVM, "z" );
                lua_pushnumber ( luaVM, tSirenInfo.m_tSirenInfo[i].m_vecSirenPositions.fZ );
                lua_settable ( luaVM, -3 ); // End of Z property

                lua_settable ( luaVM, -3 ); // End of Table
            }

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleSirens ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    CClientVehicle* pVehicle = NULL;
    unsigned char ucSirenID = 0;
    SSirenInfo tSirenInfo;

    argStream.ReadUserData ( pVehicle );
    argStream.ReadNumber ( ucSirenID );
    if ( ucSirenID > 0 && ucSirenID < 9 )
    {
        // Array indicies start at 0 so compensate here. This way all code works properly and we get nice 1-8 numbers for API
        ucSirenID--;
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_vecSirenPositions.fX );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_vecSirenPositions.fY );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_vecSirenPositions.fZ );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_RGBBeaconColour.R );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_RGBBeaconColour.G );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_RGBBeaconColour.B );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_RGBBeaconColour.A, 255 );
        argStream.ReadNumber( tSirenInfo.m_tSirenInfo[ ucSirenID ].m_dwMinSirenAlpha, 0 );
        if ( argStream.HasErrors ( ) == false )
        {
            if ( pVehicle )
            {
                if ( CStaticFunctionDefinitions::SetVehicleSirens ( *pVehicle, ucSirenID, tSirenInfo ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleDoorOpenRatio ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TNUMBER )
    {
        CClientVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            unsigned char ucDoor = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( ucDoor <= 5 )
            {
                lua_pushnumber ( luaVM, pVehicle->GetDoorOpenRatio ( ucDoor ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetVehicleComponentPosition ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );

    SString strComponent;
    CClientVehicle * pVehicle = NULL;
    CVector vecPosition;
    argStream.ReadUserData ( pVehicle );
    argStream.ReadString ( strComponent );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    if ( !argStream.HasErrors() )
    {
        if ( pVehicle )
        { 
            if ( pVehicle->SetComponentPosition ( strComponent, vecPosition ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleComponentPosition ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    SString strComponent;
    CClientVehicle * pVehicle = NULL;
    CVector vecPosition;
    argStream.ReadUserData ( pVehicle );
    argStream.ReadString ( strComponent );
    if ( !argStream.HasErrors() )
    {
        if ( pVehicle )
        {
            if ( pVehicle->GetComponentPosition ( strComponent, vecPosition ) )
            {
                lua_pushnumber ( luaVM, vecPosition.fX );
                lua_pushnumber ( luaVM, vecPosition.fY );
                lua_pushnumber ( luaVM, vecPosition.fZ );
                return 3;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleComponentRotation ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    SString strComponent;
    CClientVehicle * pVehicle = NULL;
    CVector vecRotation;
    argStream.ReadUserData ( pVehicle );
    argStream.ReadString ( strComponent );
    argStream.ReadNumber ( vecRotation.fX );
    argStream.ReadNumber ( vecRotation.fY );
    argStream.ReadNumber ( vecRotation.fZ );
    if ( !argStream.HasErrors() )
    {
        if ( pVehicle )
        {
            pVehicle->SetComponentRotation ( strComponent, vecRotation );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleComponentRotation ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    SString strComponent;
    CClientVehicle * pVehicle = NULL;
    CVector vecRotation;
    argStream.ReadUserData ( pVehicle );
    argStream.ReadString ( strComponent );
    if ( !argStream.HasErrors() )
    {
        if ( pVehicle )
        {
            if ( pVehicle->GetComponentRotation ( strComponent, vecRotation ) )
            {
                lua_pushnumber ( luaVM, vecRotation.fX );
                lua_pushnumber ( luaVM, vecRotation.fY );
                lua_pushnumber ( luaVM, vecRotation.fZ );
                return 3;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::ResetVehicleComponentPosition ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    SString strComponent;
    CClientVehicle * pVehicle = NULL;
    argStream.ReadUserData ( pVehicle );
    argStream.ReadString ( strComponent );
    if ( !argStream.HasErrors() )
    {
        if ( pVehicle )
        {
            if ( pVehicle->ResetComponentPosition ( strComponent ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::ResetVehicleComponentRotation ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    SString strComponent;
    CClientVehicle * pVehicle = NULL;
    argStream.ReadUserData ( pVehicle );
    argStream.ReadString ( strComponent );
    if ( !argStream.HasErrors() )
    {
        if ( pVehicle )
        {
            if ( pVehicle->ResetComponentRotation ( strComponent ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetVehicleComponentVisible ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    SString strComponent;
    CClientVehicle * pVehicle = NULL;
    bool bVisible = false;
    argStream.ReadUserData ( pVehicle );
    argStream.ReadString ( strComponent );
    argStream.ReadBool ( bVisible );
    if ( !argStream.HasErrors() )
    {
        if ( pVehicle )
        {
            if ( pVehicle->SetComponentVisible ( strComponent, bVisible ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetVehicleComponentVisible ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    SString strComponent;
    CClientVehicle * pVehicle = NULL;
    bool bVisible = false;
    argStream.ReadUserData ( pVehicle );
    argStream.ReadString ( strComponent );
    if ( !argStream.HasErrors() )
    {
        if ( pVehicle )
        {
            if ( pVehicle->GetComponentVisible ( strComponent, bVisible ) )
            {
                lua_pushboolean ( luaVM, bVisible );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetVehicleComponents ( lua_State* luaVM )
{
    CScriptArgReader argStream ( luaVM );
    SString strComponent;
    CClientVehicle * pVehicle = NULL;

    argStream.ReadUserData ( pVehicle );

    if ( !argStream.HasErrors() )
    {
        if ( pVehicle )
        {
            std::map < SString, SVehicleComponentData > ::iterator iter = pVehicle->ComponentsBegin ( );
            lua_newtable ( luaVM );
            for ( ;iter != pVehicle->ComponentsEnd ( ); iter++ )
            {
                lua_pushstring( luaVM, (*iter).first );
                lua_pushboolean( luaVM, (*iter).second.m_bVisible );
                lua_settable ( luaVM, -3 ); // End of Table
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}