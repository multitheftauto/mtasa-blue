/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaCompatibilityDefs.h"
#include "CLuaElementDefs.h"
#include "CLuaVehicleDefs.h"
#include "CLuaPedDefs.h"
#include "CLuaPlayerDefs.h"
#include "luadefs/CLuaXMLDefs.h"
#include <lua/CLuaFunctionParser.h>

void CLuaCompatibilityDefs::LoadFunctions()
{
    // Backwards compatibility functions
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Ped/player set funcs
        {"killPlayer", CLuaPedDefs::KillPed},
        {"addPlayerClothes", CLuaPedDefs::AddPedClothes},
        {"removePlayerClothes", CLuaPedDefs::RemovePedClothes},
        {"setPlayerSkin", CLuaElementDefs::setElementModel},
        {"setPlayerArmor", CLuaPedDefs::SetPedArmor},
        {"setPlayerWeaponSlot", CLuaPedDefs::SetPedWeaponSlot},
        {"setPlayerRotation", CLuaPedDefs::SetPedRotation},
        {"setPlayerStat", CLuaPedDefs::SetPedStat},
        {"setPlayerFightingStyle", CLuaPedDefs::SetPedFightingStyle},
        {"setPlayerGravity", CLuaPedDefs::SetPedGravity},
        {"setPlayerChoking", CLuaPedDefs::SetPedChoking},
        {"setClientName", CLuaPlayerDefs::SetPlayerName},
        {"setPedSkin", CLuaElementDefs::setElementModel},
        {"warpPlayerIntoVehicle", CLuaPedDefs::WarpPedIntoVehicle},
        {"givePlayerJetPack", CLuaPedDefs::GivePedJetPack},
        {"removePlayerJetPack", CLuaPedDefs::RemovePedJetPack},
        {"removePlayerFromVehicle", CLuaPedDefs::RemovePedFromVehicle},

        // Ped/player get funcs
        {"doesPlayerHaveJetPack", CLuaPedDefs::DoesPedHaveJetPack},
        {"isPlayerChoking", CLuaPedDefs::IsPedChoking},
        {"isPlayerDead", CLuaPedDefs::IsPedDead},
        {"isPlayerDucked", CLuaPedDefs::IsPedDucked},
        {"isPlayerInWater", CLuaElementDefs::isElementInWater},
        {"isPedInWater", CLuaElementDefs::isElementInWater},
        {"isPlayerOnGround", CLuaPedDefs::IsPedOnGround},
        {"isPlayerInVehicle", CLuaPedDefs::IsPedInVehicle},
        {"getPlayerSkin", CLuaElementDefs::getElementModel},
        {"getPlayerWeaponSlot", CLuaPedDefs::GetPedWeaponSlot},
        {"getPlayerArmor", CLuaPedDefs::GetPedArmor},
        {"getPlayerRotation", CLuaPedDefs::GetPedRotation},
        {"getPlayerStat", CLuaPedDefs::GetPedStat},
        {"getPlayerTarget", CLuaPedDefs::GetPedTarget},
        {"getPlayerClothes", CLuaPedDefs::GetPedClothes},
        {"getPlayerFightingStyle", CLuaPedDefs::GetPedFightingStyle},
        {"getPlayerGravity", CLuaPedDefs::GetPedGravity},
        {"getPlayerContactElement", CLuaPedDefs::GetPedContactElement},
        {"getPlayerOccupiedVehicle", CLuaPedDefs::GetPedOccupiedVehicle},
        {"getPlayerOccupiedVehicleSeat", CLuaPedDefs::GetPedOccupiedVehicleSeat},
        {"getClientName", CLuaPlayerDefs::GetPlayerName},
        {"getClientIP", CLuaPlayerDefs::GetPlayerIP},
        {"getClientAccount", CLuaPlayerDefs::GetPlayerAccount},
        {"getPlayerWeapon", CLuaPedDefs::GetPedWeapon},
        {"getPlayerTotalAmmo", CLuaPedDefs::GetPedTotalAmmo},
        {"getPlayerAmmoInClip", CLuaPedDefs::GetPedAmmoInClip},
        {"getPedSkin", CLuaElementDefs::getElementModel},

        // Vehicle set funcs
        {"setVehicleModel", CLuaElementDefs::setElementModel},

        // Vehicle get funcs
        {"getVehicleID", CLuaElementDefs::getElementModel},
        {"getVehicleModel", CLuaElementDefs::getElementModel},
        {"getVehicleIDFromName", CLuaVehicleDefs::GetVehicleModelFromName},
        {"getVehicleNameFromID", CLuaVehicleDefs::GetVehicleNameFromModel},

        // Object set funcs
        {"setObjectModel", CLuaElementDefs::setElementModel},

        // Object get funcs
        {"getObjectModel", CLuaElementDefs::getElementModel},

        // XML funcs
        {"xmlNodeGetSubNodes", CLuaXMLDefs::xmlNodeGetChildren},
        {"xmlCreateSubNode", CLuaXMLDefs::xmlCreateChild},
        {"xmlFindSubNode", CLuaXMLDefs::xmlNodeFindChild},

        // Attach funs
        {"attachElementToElement", CLuaElementDefs::attachElements},
        {"detachElementFromElement", CLuaElementDefs::detachElements},

        // Deprecated since 1.5.5-9.13846
        {"doesPedHaveJetPack", CLuaPedDefs::DoesPedHaveJetPack},
        {"givePedJetPack", CLuaPedDefs::GivePedJetPack},
        {"removePedJetPack", CLuaPedDefs::RemovePedJetPack},

        // Old Discord implementation (see #2499)
        {"setPlayerDiscordJoinParams",
         [](lua_State* luaVM) -> int {
             lua_pushboolean(luaVM, false);
             return 1;
         }},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}
