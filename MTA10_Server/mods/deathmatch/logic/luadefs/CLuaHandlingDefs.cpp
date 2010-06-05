/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaHandlingDefs.cpp
*  PURPOSE:     Lua vehicle handling definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define lua_istype(luavm, number,type) (lua_type(luavm,number) == type)

void CLuaHandlingDefs::LoadFunctions ( void )
{
    // Create
    CLuaCFunctions::AddFunction ( "createHandling", CLuaHandlingDefs::createHandling );
    CLuaCFunctions::AddFunction ( "addDefaultHandling", CLuaHandlingDefs::addDefaultHandling );

    // Get
    CLuaCFunctions::AddFunction ( "handlingGetMass", CLuaHandlingDefs::handlingGetMass );
    CLuaCFunctions::AddFunction ( "handlingGetTurnMass", CLuaHandlingDefs::handlingGetTurnMass );
    CLuaCFunctions::AddFunction ( "handlingGetDragCoefficiency", CLuaHandlingDefs::handlingGetDragCoefficiency );
    CLuaCFunctions::AddFunction ( "handlingGetCenterOfMass", CLuaHandlingDefs::handlingGetCenterOfMass );

    CLuaCFunctions::AddFunction ( "handlingGetPercentSubmerged", CLuaHandlingDefs::handlingGetPercentSubmerged );
    CLuaCFunctions::AddFunction ( "handlingGetTractionMultiplier", CLuaHandlingDefs::handlingGetTractionMultiplier );

    CLuaCFunctions::AddFunction ( "handlingGetDriveType", CLuaHandlingDefs::handlingGetDriveType );
    CLuaCFunctions::AddFunction ( "handlingGetEngineType", CLuaHandlingDefs::handlingGetEngineType );
    CLuaCFunctions::AddFunction ( "handlingGetNumberOfGears", CLuaHandlingDefs::handlingGetNumberOfGears );

    CLuaCFunctions::AddFunction ( "handlingGetEngineAcceleration", CLuaHandlingDefs::handlingGetEngineAccelleration );
    CLuaCFunctions::AddFunction ( "handlingGetEngineInertia", CLuaHandlingDefs::handlingGetEngineInertia );
    CLuaCFunctions::AddFunction ( "handlingGetMaxVelocity", CLuaHandlingDefs::handlingGetMaxVelocity );

    CLuaCFunctions::AddFunction ( "handlingGetBrakeDeceleration", CLuaHandlingDefs::handlingGetBrakeDecelleration );
    CLuaCFunctions::AddFunction ( "handlingGetBrakeBias", CLuaHandlingDefs::handlingGetBrakeBias );
    CLuaCFunctions::AddFunction ( "handlingGetABS", CLuaHandlingDefs::handlingGetABS );

    CLuaCFunctions::AddFunction ( "handlingGetSteeringLock", CLuaHandlingDefs::handlingGetSteeringLock );
    CLuaCFunctions::AddFunction ( "handlingGetTractionLoss", CLuaHandlingDefs::handlingGetTractionLoss );
    CLuaCFunctions::AddFunction ( "handlingGetTractionBias", CLuaHandlingDefs::handlingGetTractionBias );

    CLuaCFunctions::AddFunction ( "handlingGetSuspensionForceLevel", CLuaHandlingDefs::handlingGetSuspensionForceLevel );
    CLuaCFunctions::AddFunction ( "handlingGetSuspensionDamping", CLuaHandlingDefs::handlingGetSuspensionDamping );
    CLuaCFunctions::AddFunction ( "handlingGetSuspensionHighSpeedDamping", CLuaHandlingDefs::handlingGetSuspensionHighSpeedDamping );
    CLuaCFunctions::AddFunction ( "handlingGetSuspensionUpperLimit", CLuaHandlingDefs::handlingGetSuspensionUpperLimit );
    CLuaCFunctions::AddFunction ( "handlingGetSuspensionLowerLimit", CLuaHandlingDefs::handlingGetSuspensionLowerLimit );
    CLuaCFunctions::AddFunction ( "handlingGetSuspensionFrontRearBias", CLuaHandlingDefs::handlingGetSuspensionFrontRearBias );
    CLuaCFunctions::AddFunction ( "handlingGetSuspensionAntidiveMultiplier", CLuaHandlingDefs::handlingGetSuspensionAntidiveMultiplier );

    CLuaCFunctions::AddFunction ( "handlingGetCollisionDamageMultiplier", CLuaHandlingDefs::handlingGetCollisionDamageMultiplier );

    CLuaCFunctions::AddFunction ( "handlingGetFlags", CLuaHandlingDefs::handlingGetFlags );
    CLuaCFunctions::AddFunction ( "handlingGetModelFlags", CLuaHandlingDefs::handlingGetModelFlags );
    CLuaCFunctions::AddFunction ( "handlingGetSeatOffsetDistance", CLuaHandlingDefs::handlingGetSeatOffsetDistance );

    CLuaCFunctions::AddFunction ( "handlingGetHeadLightType", CLuaHandlingDefs::handlingGetHeadLightType );
    CLuaCFunctions::AddFunction ( "handlingGetTailLightType", CLuaHandlingDefs::handlingGetTailLightType );
    CLuaCFunctions::AddFunction ( "handlingGetAnimGroup", CLuaHandlingDefs::handlingGetAnimGroup );

    // Set
    CLuaCFunctions::AddFunction ( "handlingSetMass", CLuaHandlingDefs::handlingSetMass );
    CLuaCFunctions::AddFunction ( "handlingSetTurnMass", CLuaHandlingDefs::handlingSetTurnMass );
    CLuaCFunctions::AddFunction ( "handlingSetDragCoefficiency", CLuaHandlingDefs::handlingSetDragCoefficiency );
    CLuaCFunctions::AddFunction ( "handlingSetCenterOfMass", CLuaHandlingDefs::handlingSetCenterOfMass );

    CLuaCFunctions::AddFunction ( "handlingSetPercentSubmerged", CLuaHandlingDefs::handlingSetPercentSubmerged );
    CLuaCFunctions::AddFunction ( "handlingSetTractionMultiplier", CLuaHandlingDefs::handlingSetTractionMultiplier );

    CLuaCFunctions::AddFunction ( "handlingSetDriveType", CLuaHandlingDefs::handlingSetDriveType );
    CLuaCFunctions::AddFunction ( "handlingSetEngineType", CLuaHandlingDefs::handlingSetEngineType );
    CLuaCFunctions::AddFunction ( "handlingSetNumberOfGears", CLuaHandlingDefs::handlingSetNumberOfGears );

    CLuaCFunctions::AddFunction ( "handlingSetEngineAcceleration", CLuaHandlingDefs::handlingSetEngineAccelleration );
    CLuaCFunctions::AddFunction ( "handlingSetEngineInertia", CLuaHandlingDefs::handlingSetEngineInertia );
    CLuaCFunctions::AddFunction ( "handlingSetMaxVelocity", CLuaHandlingDefs::handlingSetMaxVelocity );

    CLuaCFunctions::AddFunction ( "handlingSetBrakeDeceleration", CLuaHandlingDefs::handlingSetBrakeDecelleration );
    CLuaCFunctions::AddFunction ( "handlingSetBrakeBias", CLuaHandlingDefs::handlingSetBrakeBias );
    CLuaCFunctions::AddFunction ( "handlingSetABS", CLuaHandlingDefs::handlingSetABS );

    CLuaCFunctions::AddFunction ( "handlingSetSteeringLock", CLuaHandlingDefs::handlingSetSteeringLock );
    CLuaCFunctions::AddFunction ( "handlingSetTractionLoss", CLuaHandlingDefs::handlingSetTractionLoss );
    CLuaCFunctions::AddFunction ( "handlingSetTractionBias", CLuaHandlingDefs::handlingSetTractionBias );

    CLuaCFunctions::AddFunction ( "handlingSetSuspensionForceLevel", CLuaHandlingDefs::handlingSetSuspensionForceLevel );
    CLuaCFunctions::AddFunction ( "handlingSetSuspensionDamping", CLuaHandlingDefs::handlingSetSuspensionDamping );
    CLuaCFunctions::AddFunction ( "handlingSetSuspensionHighSpeedDamping", CLuaHandlingDefs::handlingSetSuspensionHighSpeedDamping );
    CLuaCFunctions::AddFunction ( "handlingSetSuspensionUpperLimit", CLuaHandlingDefs::handlingSetSuspensionUpperLimit );
    CLuaCFunctions::AddFunction ( "handlingSetSuspensionLowerLimit", CLuaHandlingDefs::handlingSetSuspensionLowerLimit );
    CLuaCFunctions::AddFunction ( "handlingSetSuspensionFrontRearBias", CLuaHandlingDefs::handlingSetSuspensionFrontRearBias );
    CLuaCFunctions::AddFunction ( "handlingSetSuspensionAntidiveMultiplier", CLuaHandlingDefs::handlingSetSuspensionAntidiveMultiplier );

    CLuaCFunctions::AddFunction ( "handlingSetCollisionDamageMultiplier", CLuaHandlingDefs::handlingSetCollisionDamageMultiplier );

    CLuaCFunctions::AddFunction ( "handlingSetFlags", CLuaHandlingDefs::handlingSetFlags );
    CLuaCFunctions::AddFunction ( "handlingSetModelFlags", CLuaHandlingDefs::handlingSetModelFlags );
    CLuaCFunctions::AddFunction ( "handlingSetSeatOffsetDistance", CLuaHandlingDefs::handlingSetSeatOffsetDistance );

    CLuaCFunctions::AddFunction ( "handlingSetHeadLightType", CLuaHandlingDefs::handlingSetHeadLightType );
    CLuaCFunctions::AddFunction ( "handlingSetTailLightType", CLuaHandlingDefs::handlingSetTailLightType );
    CLuaCFunctions::AddFunction ( "handlingSetAnimGroup", CLuaHandlingDefs::handlingSetAnimGroup );
}


int CLuaHandlingDefs::createHandling ( lua_State* luaVM )
{
    // handling createHandling ()

    // Get the VM
    CLuaMain* pLuaVM = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaVM )
    {
        // Grab the resource
        CResource* pResource = pLuaVM->GetResource ();
        if ( pResource )
        {
            // Create the entry
            CHandling* pHandling = m_pHandlingManager->Create ( pResource->GetDynamicElementRoot () );
            if ( pHandling )
            {
                // Add it to the element group
                CElementGroup * pGroup = pResource->GetElementGroup ();
                if ( pGroup )
                {
                    pGroup->Add ( pHandling );
                }

                // Tell players about it
                CEntityAddPacket Packet;
                Packet.Add ( pHandling );
                m_pPlayerManager->BroadcastOnlyJoined ( Packet );

                // Success
                lua_pushelement ( luaVM, pHandling );
                return 1;
            }
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::addDefaultHandling ( lua_State* luaVM )
{
    // bool addDefaultHandling ( number Model, handling theHandling )

    // Got first and second argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 2 );
    if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) &&
         pHandling )
    {
        // Grab it. Valid id?
        unsigned short usModel = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usModel ) )
        {
            // Make this handling default to that vehicle ID
            pHandling->AddDefaultTo ( usModel );

            // Vehicle ID as a byte
            unsigned char ucModel = static_cast < unsigned char > ( usModel - 400 );

            // Tell clients about it
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( ucModel );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_DEFAULT, *BitStream.pBitStream ) );

            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetMass ( lua_State* luaVM )
{
    // Got handling argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetMass ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetMass ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetTurnMass ( lua_State* luaVM )
{
    // Got handling argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetTurnMass ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetTurnMass ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetDragCoefficiency ( lua_State* luaVM )
{
    // Got handling argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetDragCoeff ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetDragCoeff ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetCenterOfMass ( lua_State* luaVM )
{
    // Got handling argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        const CVector& vecCenterOfMass = pHandling->GetCenterOfMass ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, vecCenterOfMass.fX );
            lua_pushnumber ( luaVM, vecCenterOfMass.fY );
            lua_pushnumber ( luaVM, vecCenterOfMass.fZ );
            return 3;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            const CVector& vecCenterOfMass = m_pHandlingManager->GetCenterOfMass ( List );
            lua_pushnumber ( luaVM, vecCenterOfMass.fX );
            lua_pushnumber ( luaVM, vecCenterOfMass.fY );
            lua_pushnumber ( luaVM, vecCenterOfMass.fZ );
            return 3;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetPercentSubmerged ( lua_State* luaVM )
{
    // Got handling argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        unsigned int uiPercentSubmerged = pHandling->GetPercentSubmerged ( bChanged );
        if ( bChanged )
        {
            lua_pushinteger ( luaVM, uiPercentSubmerged );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushinteger ( luaVM, m_pHandlingManager->GetPercentSubmerged ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetTractionMultiplier ( lua_State* luaVM )
{
    // Got handling argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetTractionMultiplier ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetTractionMultiplier ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetDriveType ( lua_State* luaVM )
{
    // Drive type
    CHandling::eDriveType Drive;

    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Grab the value, but only if it's changed
        bool bChanged;
        Drive = pHandling->GetDriveType ( bChanged );
        if ( !bChanged )
        {
            // No set value, return nil
            lua_pushnil ( luaVM );
            return 1;
        }
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Grab the one that actually set that property. Eventually this is the original.
            Drive = m_pHandlingManager->GetDriveType ( List );
        }
        else
        {
            // Failed
            lua_pushboolean ( luaVM, false );
            return 1;
        }
    }
    else
    {
        // Failed
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    // Return a string depending on what it is
    switch ( Drive )
    {
        case CHandling::RWD:
            lua_pushstring ( luaVM, "rwd" );
            return 1;

        case CHandling::FWD:
            lua_pushstring ( luaVM, "fwd" );
            return 1;

        case CHandling::FOURWHEEL:
            lua_pushstring ( luaVM, "awd" );
            return 1;

        default:
            lua_pushboolean ( luaVM, false );
            return 1;
    }
}


int CLuaHandlingDefs::handlingGetEngineType ( lua_State* luaVM )
{
    // Engine type
    CHandling::eEngineType Engine;

    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Grab the value, but only if it's changed
        bool bChanged;
        Engine = pHandling->GetEngineType ( bChanged );
        if ( !bChanged )
        {
            // No set value, return nil
            lua_pushnil ( luaVM );
            return 1;
        }
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Grab the one that actually set that property. Eventually this is the original.
            Engine = m_pHandlingManager->GetEngineType ( List );
        }
        else
        {
            // Failed
            lua_pushboolean ( luaVM, false );
            return 1;
        }
    }
    else
    {
        // Failed
        lua_pushboolean ( luaVM, false );
        return 1;
    }


    // Return a string depending on what it is
    switch ( Engine )
    {
        case CHandling::PETROL:
            lua_pushstring ( luaVM, "petrol" );
            return 1;

        case CHandling::DIESEL:
            lua_pushstring ( luaVM, "diesel" );
            return 1;

        case CHandling::ELECTRIC:
            lua_pushstring ( luaVM, "electric" );
            return 1;
        
        default:
            lua_pushboolean ( luaVM, false );
            return 1;
    }
}


int CLuaHandlingDefs::handlingGetNumberOfGears ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        unsigned char ucNumberOfGears = pHandling->GetNumberOfGears ( bChanged );
        if ( bChanged )
        {
            lua_pushinteger ( luaVM, ucNumberOfGears );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushinteger ( luaVM, m_pHandlingManager->GetNumberOfGears ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetEngineAccelleration ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetEngineAccelleration ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetEngineAccelleration ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetEngineInertia ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetEngineInertia ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetEngineInertia ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetMaxVelocity ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetMaxVelocity ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetMaxVelocity ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetBrakeDecelleration ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetBrakeDecelleration ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetBrakeDecelleration ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetBrakeBias ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetBrakeBias ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetBrakeBias ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetABS ( lua_State* luaVM )
{
    // Got handling argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's not default
        bool bChanged;
        bool bABS = pHandling->GetABS ( bChanged );
        if ( bChanged )
        {
            lua_pushboolean ( luaVM, bABS );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushboolean ( luaVM, m_pHandlingManager->GetABS ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaHandlingDefs::handlingGetSteeringLock ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetSteeringLock ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetSteeringLock ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetTractionLoss ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetTractionLoss ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetTractionLoss ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetTractionBias ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetTractionBias ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetTractionBias ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetSuspensionForceLevel ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetSuspensionForceLevel ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetSuspensionForceLevel ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetSuspensionDamping ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetSuspensionDamping ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetSuspensionDamping ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetSuspensionHighSpeedDamping ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetSuspensionHighSpeedDamping ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetSuspensionHighSpeedDamping ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetSuspensionUpperLimit ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetSuspensionUpperLimit ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetSuspensionUpperLimit ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetSuspensionLowerLimit ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetSuspensionLowerLimit ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetSuspensionLowerLimit ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetSuspensionFrontRearBias ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetSuspensionFrontRearBias ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetSuspensionFrontRearBias ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetSuspensionAntidiveMultiplier ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetSuspensionAntidiveMultiplier ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetSuspensionAntidiveMultiplier ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetCollisionDamageMultiplier ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Return the value if it's changed
        bool bChanged;
        float fFloat = pHandling->GetCollisionDamageMultiplier ( bChanged );
        if ( bChanged )
        {
            lua_pushnumber ( luaVM, fFloat );
            return 1;
        }

        // No set value, return nil
        lua_pushnil ( luaVM );
        return 1;
    }

    // Got number argument?
    else if ( lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the number and check its validity
        unsigned short usID = static_cast < unsigned short > ( lua_tointeger ( luaVM, 1 ) );
        if ( m_pVehicleManager->IsValidModel ( usID ) )
        {
            // Grab a list over handling items that are applied to this ID
            std::vector < CHandling* > List;
            m_pHandlingManager->GetDefaultHandlings ( usID, List );

            // Return the one that actually set that property. Eventually this is the original.
            lua_pushnumber ( luaVM, m_pHandlingManager->GetCollisionDamageMultiplier ( List ) );
            return 1;
        }
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetFlags ( lua_State* luaVM )
{
    // TODO: handlingGetFlags

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetModelFlags ( lua_State* luaVM )
{
    // TODO: handlingGetModelFlags

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetSeatOffsetDistance ( lua_State* luaVM )
{
    // TODO: handlingGetSeatOffsetDistance

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetHeadLightType ( lua_State* luaVM )
{
    // TODO: handlingGetHeadLightType

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetTailLightType ( lua_State* luaVM )
{
    // TODO: handlingGetTailLightType

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingGetAnimGroup ( lua_State* luaVM )
{
    // TODO: handlingGetAnimGroup

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


// Enum with property id's for handling
enum eHandlingProperty
{
    PROPERTY_MASS,
    PROPERTY_TURNMASS,
    PROPERTY_DRAGCOEFF,
    PROPERTY_CENTEROFMASS,
    PROPERTY_PERCENTSUBMERGED,
    PROPERTY_TRACTIONMULTIPLIER,
    PROPERTY_DRIVETYPE,
    PROPERTY_ENGINETYPE,
    PROPERTY_NUMOFGEARS,
    PROPERTY_ENGINEACCELLERATION,
    PROPERTY_ENGINEINERTIA,
    PROPERTY_MAXVELOCITY,
    PROPERTY_BRAKEDECELLERATION,
    PROPERTY_BRAKEBIAS,
    PROPERTY_ABS,
    PROPERTY_STEERINGLOCK,
    PROPERTY_TRACTIONLOSS,
    PROPERTY_TRACTIONBIAS,
    PROPERTY_SUSPENSION_FORCELEVEL,
    PROPERTY_SUSPENSION_DAMPING,
    PROPERTY_SUSPENSION_HIGHSPEEDDAMPING,
    PROPERTY_SUSPENSION_UPPER_LIMIT,
    PROPERTY_SUSPENSION_LOWER_LIMIT,
    PROPERTY_SUSPENSION_FRONTREARBIAS,
    PROPERTY_SUSPENSION_ANTIDIVEMULTIPLIER,
    PROPERTY_COLLISIONDAMAGEMULTIPLIER,
    PROPERTY_SEATOFFSETDISTANCE,
    PROPERTY_HANDLINGFLAGS,
    PROPERTY_MODELFLAGS,
    PROPERTY_HEADLIGHT,
    PROPERTY_TAILLIGHT,
    PROPERTY_ANIMGROUP,

    PROPERTY_MASS_RESTORE,
    PROPERTY_TURNMASS_RESTORE,
    PROPERTY_DRAGCOEFF_RESTORE,
    PROPERTY_CENTEROFMASS_RESTORE,
    PROPERTY_PERCENTSUBMERGED_RESTORE,
    PROPERTY_TRACTIONMULTIPLIER_RESTORE,
    PROPERTY_DRIVETYPE_RESTORE,
    PROPERTY_ENGINETYPE_RESTORE,
    PROPERTY_NUMOFGEARS_RESTORE,
    PROPERTY_ENGINEACCELLERATION_RESTORE,
    PROPERTY_ENGINEINERTIA_RESTORE,
    PROPERTY_MAXVELOCITY_RESTORE,
    PROPERTY_BRAKEDECELLERATION_RESTORE,
    PROPERTY_BRAKEBIAS_RESTORE,
    PROPERTY_ABS_RESTORE,
    PROPERTY_STEERINGLOCK_RESTORE,
    PROPERTY_TRACTIONLOSS_RESTORE,
    PROPERTY_TRACTIONBIAS_RESTORE,
    PROPERTY_SUSPENSION_FORCELEVEL_RESTORE,
    PROPERTY_SUSPENSION_DAMPING_RESTORE,
    PROPERTY_SUSPENSION_HIGHSPEEDDAMPING_RESTORE,
    PROPERTY_SUSPENSION_UPPER_LIMIT_RESTORE,
    PROPERTY_SUSPENSION_LOWER_LIMIT_RESTORE,
    PROPERTY_SUSPENSION_FRONTREARBIAS_RESTORE,
    PROPERTY_SUSPENSION_ANTIDIVEMULTIPLIER_RESTORE,
    PROPERTY_COLLISIONDAMAGEMULTIPLIER_RESTORE,
    PROPERTY_SEATOFFSETDISTANCE_RESTORE,
    PROPERTY_HANDLINGFLAGS_RESTORE,
    PROPERTY_MODELFLAGS_RESTORE,
    PROPERTY_HEADLIGHT_RESTORE,
    PROPERTY_TAILLIGHT_RESTORE,
    PROPERTY_ANIMGROUP_RESTORE,
};


int CLuaHandlingDefs::handlingSetMass ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetMass ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_MASS ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreMass ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_MASS_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetTurnMass ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetTurnMass ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_TURNMASS ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreTurnMass ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_TURNMASS_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetDragCoefficiency ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetDragCoeff ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_DRAGCOEFF ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreDragCoeff ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_DRAGCOEFF_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetCenterOfMass ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got concrete values specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Got last 2 arguments? y,z
            if ( lua_istype ( luaVM, 3, LUA_TNUMBER ) &&
                 lua_istype ( luaVM, 4, LUA_TNUMBER ) )
            {
                // Grab the value from parameters and set it
                CVector vecCenterOfMass;
                vecCenterOfMass.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
                vecCenterOfMass.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
                vecCenterOfMass.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
                pHandling->SetCenterOfMass ( vecCenterOfMass );

                // Tell the players
                CBitStream BitStream;
                BitStream.pBitStream->Write ( pHandling->GetID () );
                BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_CENTEROFMASS ) );
                BitStream.pBitStream->Write ( vecCenterOfMass.fX );
                BitStream.pBitStream->Write ( vecCenterOfMass.fY );
                BitStream.pBitStream->Write ( vecCenterOfMass.fZ );
                m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
            }
            else
            {
                // Return failure
                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }
        else
        {
            // Restore it
            pHandling->RestoreCenterOfMass ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_CENTEROFMASS_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetPercentSubmerged ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            unsigned int uiPercentSubmerged = static_cast < unsigned int > ( lua_tointeger ( luaVM, 2 ) );
            pHandling->SetPercentSubmerged ( uiPercentSubmerged );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_PERCENTSUBMERGED ) );
            BitStream.pBitStream->Write ( uiPercentSubmerged );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestorePercentSubmerged ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_PERCENTSUBMERGED_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetTractionMultiplier ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetTractionMultiplier ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_TRACTIONMULTIPLIER ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreTractionMultiplier ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_TRACTIONMULTIPLIER_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetDriveType ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TSTRING ) )
        {
            // Grab the drive string, and try to recognize it
            CHandling::eDriveType Drive;
            const char* szDrive = lua_tostring ( luaVM, 2 );
            if ( stricmp ( szDrive, "rwd" ) == 0 )
                Drive = CHandling::RWD;
            else if ( stricmp ( szDrive, "fwd" ) == 0 )
                Drive = CHandling::FWD;
            else if ( stricmp ( szDrive, "awd" ) == 0 )
                Drive = CHandling::FOURWHEEL;
            else
            {
                // Bad type
                lua_pushboolean ( luaVM, false );
                return 1;
            }

            // Set it
            pHandling->SetDriveType ( Drive );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_DRIVETYPE ) );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( Drive ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreDriveType ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_DRIVETYPE_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetEngineType ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TSTRING ) )
        {
            // Grab the engine string, and try to recognize it
            CHandling::eEngineType Engine;
            const char* szDrive = lua_tostring ( luaVM, 2 );
            if ( stricmp ( szDrive, "petrol" ) == 0 )
                Engine = CHandling::PETROL;
            else if ( stricmp ( szDrive, "diesel" ) == 0 )
                Engine = CHandling::DIESEL;
            else if ( stricmp ( szDrive, "electric" ) == 0 )
                Engine = CHandling::ELECTRIC;
            else
            {
                // Bad type
                lua_pushboolean ( luaVM, false );
                return 1;
            }

            // Set it
            pHandling->SetEngineType ( Engine );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_ENGINETYPE ) );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( Engine ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreEngineType ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_ENGINETYPE_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetNumberOfGears ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            unsigned char ucNumberOfGears = static_cast < unsigned char > ( lua_tointeger ( luaVM, 2 ) );
            pHandling->SetNumberOfGears ( ucNumberOfGears );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_NUMOFGEARS ) );
            BitStream.pBitStream->Write ( ucNumberOfGears );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreNumberOfGears ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_NUMOFGEARS_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetEngineAccelleration ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetEngineAccelleration ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_ENGINEACCELLERATION ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreEngineAccelleration ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_ENGINEACCELLERATION_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetEngineInertia ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetEngineInertia ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_ENGINEINERTIA ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreEngineInertia ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_ENGINEINERTIA_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetMaxVelocity ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetMaxVelocity ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_MAXVELOCITY ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreMaxVelocity ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_MAXVELOCITY_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetBrakeDecelleration ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetBrakeDecelleration ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_BRAKEDECELLERATION ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreBrakeDecelleration ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_BRAKEDECELLERATION_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetBrakeBias ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetBrakeBias ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_BRAKEBIAS ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreBrakeBias ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_BRAKEBIAS_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetABS ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TBOOLEAN ) )
        {
            // Grab the value from parameters and set it
            bool bABS = lua_toboolean ( luaVM, 2 ) ? true:false;
            pHandling->SetABS ( bABS );

            // Put it in a byte
            unsigned char ucABS = 0;
            if ( bABS ) ucABS = 1;

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_ABS ) );
            BitStream.pBitStream->Write ( ucABS );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreABS ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_ABS_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetSteeringLock ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetSteeringLock ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_STEERINGLOCK ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreSteeringLock ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_STEERINGLOCK_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetTractionLoss ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetTractionLoss ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_TRACTIONLOSS ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreTractionLoss ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_TRACTIONLOSS_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetTractionBias ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetTractionBias ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_TRACTIONBIAS ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreTractionBias ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_TRACTIONBIAS_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetSuspensionForceLevel ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetSuspensionForceLevel ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_FORCELEVEL ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreSuspensionForceLevel ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_FORCELEVEL_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetSuspensionDamping ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetSuspensionDamping ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_DAMPING ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreSuspensionDamping ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_DAMPING_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetSuspensionHighSpeedDamping ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetSuspensionHighSpeedDamping ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_HIGHSPEEDDAMPING ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreSuspensionHighSpeedDamping ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_HIGHSPEEDDAMPING_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetSuspensionUpperLimit ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetSuspensionUpperLimit ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_UPPER_LIMIT ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreSuspensionUpperLimit ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_UPPER_LIMIT_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetSuspensionLowerLimit ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetSuspensionLowerLimit ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_LOWER_LIMIT ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreSuspensionLowerLimit();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_LOWER_LIMIT_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetSuspensionFrontRearBias ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetSuspensionFrontRearBias ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_FRONTREARBIAS ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreSuspensionFrontRearBias ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_FRONTREARBIAS_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetSuspensionAntidiveMultiplier ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetSuspensionAntidiveMultiplier ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_ANTIDIVEMULTIPLIER ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreSuspensionAntidiveMultiplier ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_SUSPENSION_ANTIDIVEMULTIPLIER_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetCollisionDamageMultiplier ( lua_State* luaVM )
{
    // Got argument?
    CHandling* pHandling = lua_tohandling ( luaVM, 1 );
    if ( pHandling )
    {
        // Got a concrete value specified?
        if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) )
        {
            // Grab the value from parameters and set it
            float fValue = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            pHandling->SetCollisionDamageMultiplier ( fValue );

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_COLLISIONDAMAGEMULTIPLIER ) );
            BitStream.pBitStream->Write ( fValue );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }
        else
        {
            // Restore it
            pHandling->RestoreCollisionDamageMultiplier ();

            // Tell the players
            CBitStream BitStream;
            BitStream.pBitStream->Write ( pHandling->GetID () );
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( PROPERTY_COLLISIONDAMAGEMULTIPLIER_RESTORE ) );
            m_pPlayerManager->BroadcastOnlyJoined ( CLuaPacket ( HANDLING_SET_PROPERTY, *BitStream.pBitStream ) );
        }

        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetFlags ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetModelFlags ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetSeatOffsetDistance ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetHeadLightType ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetTailLightType ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHandlingDefs::handlingSetAnimGroup ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
    return 1;
}
