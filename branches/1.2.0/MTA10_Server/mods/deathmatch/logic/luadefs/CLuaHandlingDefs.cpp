/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaHandlingDefs.cpp
*  PURPOSE:     Lua vehicle handling definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Florian Busse <flobu@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define lua_istype(luavm, number,type) (lua_type(luavm,number) == type)

void CLuaHandlingDefs::LoadFunctions ( void )
{
    // Set
    CLuaCFunctions::AddFunction ( "setVehicleHandling", CLuaHandlingDefs::SetVehicleHandling );
    CLuaCFunctions::AddFunction ( "setModelHandling", CLuaHandlingDefs::SetModelHandling );

    // Get
    CLuaCFunctions::AddFunction ( "getVehicleHandling", CLuaHandlingDefs::GetVehicleHandling );
    CLuaCFunctions::AddFunction ( "getModelHandling", CLuaHandlingDefs::GetModelHandling );
    CLuaCFunctions::AddFunction ( "getOriginalHandling", CLuaHandlingDefs::GetOriginalHandling );
}

int CLuaHandlingDefs::SetVehicleHandling ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
            {
                eHandlingProperty eProperty = m_pHandlingManager->GetPropertyEnumFromName ( lua_tostring ( luaVM, 2 ) );
                if ( eProperty )
                {
                    if ( lua_type ( luaVM, 3 ) == LUA_TNIL )
                    {
                        bool bUseOriginal = false;
                        if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
                            bUseOriginal = lua_toboolean ( luaVM, 4 ) ? true : false;

                        if ( CStaticFunctionDefinitions::ResetVehicleHandlingProperty ( pVehicle, eProperty, bUseOriginal ) )
                        {
                            lua_pushboolean ( luaVM, true );
                            return 1;
                        }
                    }
                    else
                    {
                        switch ( eProperty )
                        {                
                            case HANDLING_MASS:
                            case HANDLING_TURNMASS:
                            case HANDLING_DRAGCOEFF:
                            case HANDLING_TRACTIONMULTIPLIER:
                            case HANDLING_ENGINEACCELERATION:
                            case HANDLING_ENGINEINERTIA:
                            case HANDLING_MAXVELOCITY:
                            case HANDLING_BRAKEDECELERATION:
                            case HANDLING_BRAKEBIAS:
                            case HANDLING_STEERINGLOCK:
                            case HANDLING_TRACTIONLOSS:
                            case HANDLING_TRACTIONBIAS:
                            case HANDLING_SUSPENSION_FORCELEVEL:
                            case HANDLING_SUSPENSION_DAMPING:
                            case HANDLING_SUSPENSION_HIGHSPEEDDAMPING:
                            case HANDLING_SUSPENSION_UPPER_LIMIT:
                            case HANDLING_SUSPENSION_LOWER_LIMIT:
                            case HANDLING_SUSPENSION_FRONTREARBIAS:
                            case HANDLING_SUSPENSION_ANTIDIVEMULTIPLIER:
                            case HANDLING_COLLISIONDAMAGEMULTIPLIER:
                            case HANDLING_SEATOFFSETDISTANCE:
                                {
                                   if ( lua_type ( luaVM, 3 ) == LUA_TNUMBER )
                                    {
                                        float fValue = (float)lua_tonumber ( luaVM, 3 );
                                        if ( CStaticFunctionDefinitions::SetVehicleHandling ( pVehicle, eProperty, fValue ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleHandling", "value", 3 );
                                    break;
                                }
                            case HANDLING_PERCENTSUBMERGED: // unsigned int
                            //case HANDLING_MONETARY:
                            case HANDLING_HANDLINGFLAGS:
                            case HANDLING_MODELFLAGS:
                                {
                                    if ( lua_type ( luaVM, 3 ) == LUA_TNUMBER )
                                    {
                                        unsigned int uiValue = (unsigned int)lua_tointeger ( luaVM, 3 );
                                        if ( CStaticFunctionDefinitions::SetVehicleHandling ( pVehicle, eProperty, uiValue ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleHandling", "value", 3 );
                                    break;
                                }
                            case HANDLING_NUMOFGEARS:
                            case HANDLING_ANIMGROUP:
                                {
                                    if ( lua_type ( luaVM, 3 ) == LUA_TNUMBER )
                                    {
                                        unsigned char ucValue = (unsigned char)lua_tonumber ( luaVM, 3 );
                                        if ( CStaticFunctionDefinitions::SetVehicleHandling ( pVehicle, eProperty, ucValue ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleHandling", "value", 3 );
                                    break;
                                }
                            case HANDLING_CENTEROFMASS:
                                {
                                    if ( lua_type ( luaVM, 3 ) == LUA_TTABLE )
                                    {
                                        lua_pushnumber ( luaVM, 1 );
                                        lua_gettable ( luaVM, 3 );
                                        float fX = static_cast < float > ( lua_tonumber ( luaVM, -1 ) );
                                        lua_pop ( luaVM, 1 );

                                        lua_pushnumber ( luaVM, 2 );
                                        lua_gettable ( luaVM, 3 );
                                        float fY = static_cast < float > ( lua_tonumber ( luaVM, -1 ) );
                                        lua_pop ( luaVM, 1 );

                                        lua_pushnumber ( luaVM, 3 );
                                        lua_gettable ( luaVM, 3 );
                                        float fZ = static_cast < float > ( lua_tonumber ( luaVM, -1 ) );
                                        lua_pop ( luaVM, 1 );

                                        CVector vecCenterOfMass ( fX, fY, fZ );

                                        if ( CStaticFunctionDefinitions::SetVehicleHandling ( pVehicle, eProperty, vecCenterOfMass ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleHandling", "value", 3 );
                                    break;
                                }
                            case HANDLING_DRIVETYPE:
                            case HANDLING_ENGINETYPE:
                            //case HANDLING_HEADLIGHT:
                            //case HANDLING_TAILLIGHT:
                                {
                                    if ( lua_type ( luaVM, 3 ) == LUA_TSTRING )
                                    {
                                        if ( CStaticFunctionDefinitions::SetVehicleHandling ( pVehicle, eProperty, std::string ( lua_tostring ( luaVM, 3 ) ) ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleHandling", "value", 3 );
                                    break;
                                }
                            case HANDLING_ABS:
                                {
                                    if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
                                    {
                                        if ( CStaticFunctionDefinitions::SetVehicleHandling ( pVehicle, eProperty, lua_toboolean ( luaVM, 3 ) ? 1.0f : 0.0f ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleHandling", "value", 3 );
                                    break;
                                }
                            case HANDLING_MAX:
                                {
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleHandling", "property", 2 );
                                    break;
                                }
                            default:
                                break;
                        }
                    }
                }
            }
            else if ( lua_type ( luaVM, 2 ) == LUA_TNIL || lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            {
                bool bUseOriginal = false;
                if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
                    bUseOriginal = lua_toboolean ( luaVM, 2 ) ? true : false;

                if ( CStaticFunctionDefinitions::ResetVehicleHandling ( pVehicle, bUseOriginal ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setVehicleHandling", "property", 2 );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setVehicleHandling" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaHandlingDefs::SetModelHandling ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        eVehicleTypes eModel = static_cast < eVehicleTypes > ( (int)lua_tonumber ( luaVM, 1 ) );
        if ( eModel )
        {
            if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
            {
                eHandlingProperty eProperty = m_pHandlingManager->GetPropertyEnumFromName ( lua_tostring ( luaVM, 2 ) );
                
                if ( eProperty )
                {
                    if ( lua_type ( luaVM, 3 ) == LUA_TNIL )
                    {
                        if ( CStaticFunctionDefinitions::ResetModelHandlingProperty ( eModel, eProperty ) )
                        {
                            lua_pushboolean ( luaVM, true );
                            return 1;
                        }
                    }
                    else
                    {
                        switch ( eProperty )
                        {                
                            case HANDLING_MASS:
                            case HANDLING_TURNMASS:
                            case HANDLING_DRAGCOEFF:
                            case HANDLING_TRACTIONMULTIPLIER:
                            case HANDLING_ENGINEACCELERATION:
                            case HANDLING_ENGINEINERTIA:
                            case HANDLING_MAXVELOCITY:
                            case HANDLING_BRAKEDECELERATION:
                            case HANDLING_BRAKEBIAS:
                            case HANDLING_STEERINGLOCK:
                            case HANDLING_TRACTIONLOSS:
                            case HANDLING_TRACTIONBIAS:
                            case HANDLING_SUSPENSION_FORCELEVEL:
                            case HANDLING_SUSPENSION_DAMPING:
                            case HANDLING_SUSPENSION_HIGHSPEEDDAMPING:
                            case HANDLING_SUSPENSION_UPPER_LIMIT:
                            case HANDLING_SUSPENSION_LOWER_LIMIT:
                            case HANDLING_SUSPENSION_FRONTREARBIAS:
                            case HANDLING_SUSPENSION_ANTIDIVEMULTIPLIER:
                            case HANDLING_COLLISIONDAMAGEMULTIPLIER:
                            case HANDLING_SEATOFFSETDISTANCE:
                                {
                                    if ( lua_type ( luaVM, 3 ) == LUA_TNUMBER )
                                    {
                                        float fValue = (float)lua_tonumber ( luaVM, 3 );
                                        if ( CStaticFunctionDefinitions::SetModelHandling ( eModel, eProperty, fValue ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setModelHandling", "value", 3 );
                                    break;
                                }
                            case HANDLING_PERCENTSUBMERGED: // unsigned int
                            //case HANDLING_MONETARY:
                            case HANDLING_HANDLINGFLAGS:
                            case HANDLING_MODELFLAGS:
                                {
                                    if ( lua_type ( luaVM, 3 ) == LUA_TNUMBER )
                                    {
                                        unsigned int uiValue = (unsigned int)lua_tonumber ( luaVM, 3 );
                                        if ( uiValue == 0xffffffff && eProperty == HANDLING_MODELFLAGS )
                                        {
                                            lua_pushboolean ( luaVM, false );
                                            m_pScriptDebugging->LogBadPointer ( luaVM, "setModelHandling", "value", 3 );
                                            return 1;
                                        }
                                        if ( CStaticFunctionDefinitions::SetModelHandling ( eModel, eProperty, uiValue ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setModelHandling", "value", 3 );
                                    break;
                                }
                            case HANDLING_NUMOFGEARS:
                            case HANDLING_ANIMGROUP:
                                {
                                    if ( lua_type ( luaVM, 3 ) == LUA_TNUMBER )
                                    {
                                        unsigned char ucValue = (unsigned char)lua_tonumber ( luaVM, 3 );
                                        if ( CStaticFunctionDefinitions::SetModelHandling ( eModel, eProperty, ucValue ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setModelHandling", "value", 3 );
                                    break;
                                }
                            case HANDLING_CENTEROFMASS:
                                {
                                    if ( lua_type ( luaVM, 3 ) == LUA_TTABLE )
                                    {
                                        lua_pushnumber ( luaVM, 1 );
                                        lua_gettable ( luaVM, 3 );
                                        float fX = static_cast < float > ( lua_tonumber ( luaVM, -1 ) );
                                        lua_pop ( luaVM, 1 );

                                        lua_pushnumber ( luaVM, 2 );
                                        lua_gettable ( luaVM, 3 );
                                        float fY = static_cast < float > ( lua_tonumber ( luaVM, -1 ) );
                                        lua_pop ( luaVM, 1 );

                                        lua_pushnumber ( luaVM, 3 );
                                        lua_gettable ( luaVM, 3 );
                                        float fZ = static_cast < float > ( lua_tonumber ( luaVM, -1 ) );
                                        lua_pop ( luaVM, 1 );

                                        CVector vecCenterOfMass ( fX, fY, fZ );

                                        if ( CStaticFunctionDefinitions::SetModelHandling ( eModel, eProperty, vecCenterOfMass ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setModelHandling", "value", 3 );
                                    break;
                                }
                            case HANDLING_DRIVETYPE:
                            case HANDLING_ENGINETYPE:
                            //case HANDLING_HEADLIGHT:
                            //case HANDLING_TAILLIGHT:
                                {
                                    if ( lua_type ( luaVM, 3 ) == LUA_TSTRING )
                                    {
                                        if ( CStaticFunctionDefinitions::SetModelHandling ( eModel, eProperty, std::string ( lua_tostring ( luaVM, 3 ) ) ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                }
                            case HANDLING_ABS:
                                {
                                    if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
                                    {
                                        if ( CStaticFunctionDefinitions::SetModelHandling ( eModel, eProperty, lua_toboolean ( luaVM, 3 ) ? 1.0f : 0.0f ) )
                                        {
                                            lua_pushboolean ( luaVM, true );
                                            return 1;
                                        }
                                    }
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setModelHandling", "value", 3 );
                                    break;
                                }
                            case HANDLING_MAX:
                                {
                                    m_pScriptDebugging->LogBadPointer ( luaVM, "setModelHandling", "property", 2 );
                                }
                            default:
                                break;
                        }
                    }
                }
            }
            else if ( lua_type ( luaVM, 2 ) == LUA_TNIL )
            {
                if ( CStaticFunctionDefinitions::ResetModelHandling ( eModel ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setModelHandling", "property", 2 );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setModelHandling" );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaHandlingDefs::GetVehicleHandling ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CVehicle* pVehicle = lua_tovehicle ( luaVM, 1 );
        if ( pVehicle )
        {
            CHandlingEntry* pEntry = pVehicle->GetHandlingData ();

            lua_newtable ( luaVM );

            lua_pushnumber ( luaVM, pEntry->GetMass () );
            lua_setfield ( luaVM, -2, "mass" );

            lua_pushnumber ( luaVM, pEntry->GetTurnMass () );
            lua_setfield ( luaVM, -2, "turnMass" );

            lua_pushnumber ( luaVM, pEntry->GetDragCoeff () );
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

            lua_pushnumber ( luaVM, pEntry->GetPercentSubmerged () );
            lua_setfield ( luaVM, -2, "percentSubmerged" );

            lua_pushnumber ( luaVM, pEntry->GetTractionMultiplier () );
            lua_setfield ( luaVM, -2, "tractionMultiplier" );

            CHandlingEntry::eDriveType eDriveType = pEntry->GetCarDriveType ();
            if ( eDriveType == CHandlingEntry::FWD )
                lua_pushstring( luaVM, "fwd" );
            else if ( eDriveType == CHandlingEntry::RWD )
                lua_pushstring ( luaVM, "rwd" );
            else if ( eDriveType == CHandlingEntry::FOURWHEEL )
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
                lua_pushnil ( luaVM );
            lua_setfield ( luaVM, -2, "headLight" );

            CHandlingEntry::eLightType eTailType = pEntry->GetTailLight ();
            if ( eTailType == CHandlingEntry::LONG )
                lua_pushstring ( luaVM, "long" );
            else if ( eTailType == CHandlingEntry::SMALL )
                lua_pushstring ( luaVM, "small" );
            else if ( eTailType == CHandlingEntry::BIG )
                lua_pushstring ( luaVM, "big" );
            else
                lua_pushnil ( luaVM );
            lua_setfield ( luaVM, -2, "tailLight" );

            lua_pushnumber ( luaVM, pEntry->GetAnimGroup () );
            lua_setfield ( luaVM, -2, "animGroup" );

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getVehicleHandling", "vehicle", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getVehicleHandling" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::GetModelHandling ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        eVehicleTypes eModel = static_cast < eVehicleTypes > ( (int)lua_tonumber ( luaVM, 1 ) );
        if ( eModel )
        {
            const CHandlingEntry* pEntry = g_pGame->GetHandlingManager ()->GetModelHandlingData ( eModel );

            lua_newtable ( luaVM );

            lua_pushnumber ( luaVM, pEntry->GetMass () );
            lua_setfield ( luaVM, -2, "mass" );

            lua_pushnumber ( luaVM, pEntry->GetTurnMass () );
            lua_setfield ( luaVM, -2, "turnMass" );

            lua_pushnumber ( luaVM, pEntry->GetDragCoeff () );
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

            lua_pushnumber ( luaVM, pEntry->GetPercentSubmerged () );
            lua_setfield ( luaVM, -2, "percentSubmerged" );

            lua_pushnumber ( luaVM, pEntry->GetTractionMultiplier () );
            lua_setfield ( luaVM, -2, "tractionMultiplier" );

            CHandlingEntry::eDriveType eDriveType = pEntry->GetCarDriveType ();
            if ( eDriveType == CHandlingEntry::FWD )
                lua_pushstring ( luaVM, "fwd" );
            else if ( eDriveType == CHandlingEntry::RWD )
                lua_pushstring ( luaVM, "rwd" );
            else if ( eDriveType == CHandlingEntry::FOURWHEEL )
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
            if (eTailType == CHandlingEntry::LONG )
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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getModelHandling", "model", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getModelHandling" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::GetOriginalHandling ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        eVehicleTypes eModel = static_cast < eVehicleTypes > ( (int)lua_tonumber ( luaVM, 1 ) );
        if ( eModel )
        {
            const CHandlingEntry* pEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData( eModel );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "getOriginalHandling", "model", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getOriginalHandling" );

    lua_pushboolean ( luaVM, false );
    return 1;
}
