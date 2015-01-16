/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/oopdefs/CLuaOOPFunctionDefs.Ped.cpp
*  PURPOSE:     Lua OOP specific function definitions class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

#define MIN_CLIENT_REQ_REMOVEPEDFROMVEHICLE_CLIENTSIDE  "1.3.0-9.04482"
#define MIN_CLIENT_REQ_WARPPEDINTOVEHICLE_CLIENTSIDE    "1.3.0-9.04482"

int CLuaOOPDefs::SetPedOccupiedVehicle ( lua_State* luaVM )
{
    //  ped.vehicle = element vehicle
    //  ped.vehicle = nil
    CPed* pPed; CVehicle* pVehicle; uint uiSeat = 0;

    CScriptArgReader argStream ( luaVM );

    argStream.ReadUserData ( pPed );
    argStream.ReadUserData ( pVehicle, NULL );

    if ( pVehicle != NULL )
    {
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