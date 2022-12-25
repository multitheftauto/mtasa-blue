/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaCompatibilityDefs::LoadFunctions()
{
    // Backwards compatibility functions
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"getPlayerSkin", CLuaElementDefs::getElementModel},
        {"setPlayerSkin", CLuaElementDefs::setElementModel},
        {"getVehicleModel", CLuaElementDefs::getElementModel},
        {"setVehicleModel", CLuaElementDefs::setElementModel},
        {"getObjectModel", CLuaElementDefs::getElementModel},
        {"setObjectModel", CLuaElementDefs::setElementModel},
        {"getVehicleID", CLuaElementDefs::getElementModel},
        {"getVehicleIDFromName", CLuaVehicleDefs::GetVehicleModelFromName},
        {"getVehicleNameFromID", CLuaVehicleDefs::GetVehicleNameFromModel},
        {"getPlayerWeaponSlot", CLuaPedDefs::GetPedWeaponSlot},
        {"getPlayerArmor", CLuaPedDefs::GetPedArmor},
        {"getPlayerRotation", CLuaPedDefs::GetPedRotation},
        {"isPlayerChoking", CLuaPedDefs::IsPedChoking},
        {"isPlayerDead", CLuaPedDefs::IsPedDead},
        {"isPlayerDucked", CLuaPedDefs::IsPedDucked},
        {"getPlayerStat", CLuaPedDefs::GetPedStat},
        {"getPlayerTarget", CLuaPedDefs::GetPedTarget},
        {"getPlayerClothes", CLuaPedDefs::GetPedClothes},
        {"doesPlayerHaveJetPack", CLuaPedDefs::DoesPedHaveJetPack},
        {"isPlayerInWater", CLuaElementDefs::isElementInWater},
        {"isPedInWater", CLuaElementDefs::isElementInWater},
        {"isPlayerOnGround", CLuaPedDefs::IsPedOnGround},
        {"getPlayerFightingStyle", CLuaPedDefs::GetPedFightingStyle},
        {"getPlayerGravity", CLuaPedDefs::GetPedGravity},
        {"getPlayerContactElement", CLuaPedDefs::GetPedContactElement},
        {"setPlayerArmor", CLuaPedDefs::SetPedArmor},
        {"setPlayerWeaponSlot", CLuaPedDefs::SetPedWeaponSlot},
        {"killPlayer", CLuaPedDefs::KillPed},
        {"setPlayerRotation", CLuaPedDefs::SetPedRotation},
        {"setPlayerStat", CLuaPedDefs::SetPedStat},
        {"addPlayerClothes", CLuaPedDefs::AddPedClothes},
        {"removePlayerClothes", CLuaPedDefs::RemovePedClothes},
        {"givePlayerJetPack", CLuaPedDefs::GivePedJetPack},
        {"removePlayerJetPack", CLuaPedDefs::RemovePedJetPack},
        {"setPlayerFightingStyle", CLuaPedDefs::SetPedFightingStyle},
        {"setPlayerGravity", CLuaPedDefs::SetPedGravity},
        {"setPlayerChoking", CLuaPedDefs::SetPedChoking},
        {"warpPlayerIntoVehicle", CLuaPedDefs::WarpPedIntoVehicle},
        {"removePlayerFromVehicle", CLuaPedDefs::RemovePedFromVehicle},
        {"getPlayerOccupiedVehicle", CLuaPedDefs::GetPedOccupiedVehicle},
        {"getPlayerOccupiedVehicleSeat", CLuaPedDefs::GetPedOccupiedVehicleSeat},
        {"isPlayerInVehicle", CLuaPedDefs::IsPedInVehicle},
        {"getClientName", CLuaPlayerDefs::GetPlayerName},
        {"getClientIP", CLuaPlayerDefs::GetPlayerIP},
        {"getClientAccount", CLuaPlayerDefs::GetPlayerAccount},
        {"setClientName", CLuaPlayerDefs::SetPlayerName},
        {"getPlayerWeapon", CLuaPedDefs::GetPedWeapon},
        {"getPlayerTotalAmmo", CLuaPedDefs::GetPedTotalAmmo},
        {"getPlayerAmmoInClip", CLuaPedDefs::GetPedAmmoInClip},
        {"getPedSkin", CLuaElementDefs::getElementModel},
        {"setPedSkin", CLuaElementDefs::setElementModel},
        {"xmlNodeGetSubNodes", CLuaXMLDefs::xmlNodeGetChildren},
        {"xmlCreateSubNode", CLuaXMLDefs::xmlCreateChild},
        {"xmlFindSubNode", CLuaXMLDefs::xmlNodeFindChild},
        {"attachElementToElement", CLuaElementDefs::attachElements},
        {"detachElementFromElement", CLuaElementDefs::detachElements},

        // Deprecated since 1.5.5-9.13846
        {"doesPedHaveJetPack", CLuaPedDefs::DoesPedHaveJetPack},
        {"givePedJetPack", CLuaPedDefs::GivePedJetPack},
        {"removePedJetPack", CLuaPedDefs::RemovePedJetPack}
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}
