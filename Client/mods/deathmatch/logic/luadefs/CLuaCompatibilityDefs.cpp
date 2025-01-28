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
        {"getPlayerRotation", CLuaPedDefs::GetPedRotation},
        {"canPlayerBeKnockedOffBike", CLuaPedDefs::CanPedBeKnockedOffBike},
        {"getPlayerContactElement", CLuaPedDefs::GetPedContactElement},
        {"isPlayerInVehicle", CLuaPedDefs::IsPedInVehicle},
        {"doesPlayerHaveJetPack", CLuaPedDefs::DoesPedHaveJetPack},
        {"isPlayerInWater", CLuaElementDefs::IsElementInWater},
        {"isPedInWater", CLuaElementDefs::IsElementInWater},
        {"isPedOnFire", CLuaPedDefs::IsPedOnFire},
        {"setPedOnFire", CLuaPedDefs::SetPedOnFire},
        {"isPlayerOnGround", CLuaPedDefs::IsPedOnGround},
        {"getPlayerTask", CLuaPedDefs::GetPedTask},
        {"getPlayerSimplestTask", CLuaPedDefs::GetPedSimplestTask},
        {"isPlayerDoingTask", CLuaPedDefs::IsPedDoingTask},
        {"getPlayerTarget", CLuaPedDefs::GetPedTarget},
        {"getPlayerTargetStart", CLuaPedDefs::GetPedTargetStart},
        {"getPlayerTargetEnd", CLuaPedDefs::GetPedTargetEnd},
        {"getPlayerTargetCollision", CLuaPedDefs::GetPedTargetCollision},
        {"getPlayerWeaponSlot", CLuaPedDefs::GetPedWeaponSlot},
        {"getPlayerWeapon", CLuaPedDefs::GetPedWeapon},
        {"getPlayerAmmoInClip", CLuaPedDefs::GetPedAmmoInClip},
        {"getPlayerTotalAmmo", CLuaPedDefs::GetPedTotalAmmo},
        {"getPedWeaponMuzzlePosition", CLuaPedDefs::GetPedWeaponMuzzlePosition},
        {"getPlayerOccupiedVehicle", CLuaPedDefs::GetPedOccupiedVehicle},
        {"getPlayerArmor", ArgumentParserWarn<false, CLuaPedDefs::GetPedArmor>},
        {"getPlayerSkin", CLuaElementDefs::GetElementModel},
        {"isPlayerChoking", CLuaPedDefs::IsPedChoking},
        {"isPlayerDucked", CLuaPedDefs::IsPedDucked},
        {"getPlayerStat", CLuaPedDefs::GetPedStat},
        {"setPlayerWeaponSlot", CLuaPedDefs::SetPedWeaponSlot},
        {"setPlayerSkin", CLuaElementDefs::SetElementModel},
        {"setPlayerRotation", CLuaPedDefs::SetPedRotation},
        {"setPlayerCanBeKnockedOffBike", CLuaPedDefs::SetPedCanBeKnockedOffBike},
        {"setVehicleModel", CLuaElementDefs::SetElementModel},
        {"getVehicleModel", CLuaElementDefs::GetElementModel},
        {"getPedSkin", CLuaElementDefs::GetElementModel},
        {"setPedSkin", CLuaElementDefs::SetElementModel},
        {"getObjectRotation", CLuaElementDefs::GetElementRotation},
        {"setObjectRotation", CLuaElementDefs::SetElementRotation},
        {"getVehicleIDFromName", CLuaVehicleDefs::GetVehicleModelFromName},
        {"getVehicleID", CLuaElementDefs::GetElementModel},
        {"getVehicleRotation", CLuaElementDefs::GetElementRotation},
        {"getVehicleNameFromID", CLuaVehicleDefs::GetVehicleNameFromModel},
        {"setVehicleRotation", CLuaElementDefs::SetElementRotation},
        {"attachElementToElement", CLuaElementDefs::AttachElements},
        {"detachElementFromElement", CLuaElementDefs::DetachElements},
        {"xmlFindSubNode", CLuaXMLDefs::xmlNodeFindChild},
        {"xmlNodeGetSubNodes", CLuaXMLDefs::xmlNodeGetChildren},
        {"xmlNodeFindSubNode", CLuaXMLDefs::xmlNodeFindChild},
        {"xmlCreateSubNode", CLuaXMLDefs::xmlCreateChild},
        {"xmlNodeFindChild", CLuaXMLDefs::xmlNodeFindChild},
        {"isPlayerDead", CLuaPedDefs::IsPedDead},
        {"guiEditSetCaratIndex", CLuaGUIDefs::GUIEditSetCaretIndex},
        {"guiMemoSetCaratIndex", CLuaGUIDefs::GUIMemoSetCaretIndex},
        {"setControlState", ArgumentParserWarn<false, CLuaPedDefs::SetPedControlState>},
        {"getControlState", ArgumentParserWarn<false, CLuaPedDefs::GetPedControlState>},
        {"setCameraShakeLevel", ArgumentParserWarn<false, CLuaCameraDefs::SetCameraDrunkLevel>},
        {"getCameraShakeLevel", ArgumentParserWarn<false, CLuaCameraDefs::GetCameraDrunkLevel>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}
