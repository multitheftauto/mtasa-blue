/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaFunctionDefs.h"

CTimeUsMarker<20> markerLatentEvent;            // For timing triggerLatentClientEvent

void CLuaFunctionDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]
    {
        {"addEvent", CLuaFunctionDefs::AddEvent}, {"addEventHandler", CLuaFunctionDefs::AddEventHandler},
            {"removeEventHandler", CLuaFunctionDefs::RemoveEventHandler}, {"getEventHandlers", CLuaFunctionDefs::GetEventHandlers},
            {"triggerEvent", CLuaFunctionDefs::TriggerEvent}, {"triggerClientEvent", CLuaFunctionDefs::TriggerClientEvent},
            {"cancelEvent", CLuaFunctionDefs::CancelEvent}, {"wasEventCancelled", CLuaFunctionDefs::WasEventCancelled},
            {"getCancelReason", CLuaFunctionDefs::GetCancelReason}, {"triggerLatentClientEvent", CLuaFunctionDefs::TriggerLatentClientEvent},
            {"getLatentEventHandles", CLuaFunctionDefs::GetLatentEventHandles}, {"getLatentEventStatus", CLuaFunctionDefs::GetLatentEventStatus},
            {"cancelLatentEvent", CLuaFunctionDefs::CancelLatentEvent}, {"addDebugHook", CLuaFunctionDefs::AddDebugHook},
            {"removeDebugHook", CLuaFunctionDefs::RemoveDebugHook},

            // Explosion create funcs
            {"createExplosion", CLuaFunctionDefs::CreateExplosion},

            // Ped body funcs?
            {"getBodyPartName", CLuaFunctionDefs::GetBodyPartName}, {"getClothesByTypeIndex", CLuaFunctionDefs::GetClothesByTypeIndex},
            {"getTypeIndexFromClothes", CLuaFunctionDefs::GetTypeIndexFromClothes}, {"getClothesTypeName", CLuaFunctionDefs::GetClothesTypeName},

            // Weapon funcs
            {"getWeaponNameFromID", CLuaFunctionDefs::GetWeaponNameFromID}, {"getWeaponIDFromName", CLuaFunctionDefs::GetWeaponIDFromName},
            {"getWeaponProperty", CLuaFunctionDefs::GetWeaponProperty}, {"getOriginalWeaponProperty", CLuaFunctionDefs::GetOriginalWeaponProperty},
            {"setWeaponProperty", CLuaFunctionDefs::SetWeaponProperty}, {"setWeaponAmmo", CLuaFunctionDefs::SetWeaponAmmo},
            {"getSlotFromWeapon", CLuaFunctionDefs::GetSlotFromWeapon},

    #if MTASA_VERSION_TYPE < VERSION_TYPE_RELEASE
            {"createWeapon", CLuaFunctionDefs::CreateWeapon}, {"fireWeapon", CLuaFunctionDefs::FireWeapon},
            {"setWeaponState", CLuaFunctionDefs::SetWeaponState}, {"getWeaponState", CLuaFunctionDefs::GetWeaponState},
            {"setWeaponTarget", CLuaFunctionDefs::SetWeaponTarget}, {"getWeaponTarget", CLuaFunctionDefs::GetWeaponTarget},
            {"setWeaponOwner", CLuaFunctionDefs::SetWeaponOwner}, {"getWeaponOwner", CLuaFunctionDefs::GetWeaponOwner},
            {"setWeaponFlags", CLuaFunctionDefs::SetWeaponFlags}, {"getWeaponFlags", CLuaFunctionDefs::GetWeaponFlags},
            {"setWeaponFiringRate", CLuaFunctionDefs::SetWeaponFiringRate}, {"getWeaponFiringRate", CLuaFunctionDefs::GetWeaponFiringRate},
            {"resetWeaponFiringRate", CLuaFunctionDefs::ResetWeaponFiringRate}, {"getWeaponAmmo", CLuaFunctionDefs::GetWeaponAmmo},
            {"getWeaponClipAmmo", CLuaFunctionDefs::GetWeaponClipAmmo}, {"setWeaponClipAmmo", CLuaFunctionDefs::SetWeaponClipAmmo},
    #endif

            // Console funcs
            {"addCommandHandler", CLuaFunctionDefs::AddCommandHandler}, {"removeCommandHandler", CLuaFunctionDefs::RemoveCommandHandler},
            {"executeCommandHandler", CLuaFunctionDefs::ExecuteCommandHandler}, {"getCommandHandlers", CLuaFunctionDefs::GetCommandHandlers},

            // Loaded map funcs
            {"getRootElement", CLuaFunctionDefs::GetRootElement}, {"loadMapData", CLuaFunctionDefs::LoadMapData},
            {"saveMapData", CLuaFunctionDefs::SaveMapData},

            // All-Seeing Eye Functions
            {"getGameType", CLuaFunctionDefs::GetGameType}, {"getMapName", CLuaFunctionDefs::GetMapName}, {"setGameType", CLuaFunctionDefs::SetGameType},
            {"setMapName", CLuaFunctionDefs::SetMapName}, {"getRuleValue", CLuaFunctionDefs::GetRuleValue}, {"setRuleValue", CLuaFunctionDefs::SetRuleValue},
            {"removeRuleValue", CLuaFunctionDefs::RemoveRuleValue},

            // Registry functions
            {"getPerformanceStats", CLuaFunctionDefs::GetPerformanceStats},

            // Misc funcs
            {"resetMapInfo", CLuaFunctionDefs::ResetMapInfo}, {"getServerPort", CLuaFunctionDefs::GetServerPort},

            // Settings registry funcs
            {"get", CLuaFunctionDefs::Get}, {"set", CLuaFunctionDefs::Set},

            // Utility
            {"getVersion", CLuaFunctionDefs::GetVersion}, {"getNetworkUsageData", CLuaFunctionDefs::GetNetworkUsageData},
            {"getNetworkStats", CLuaFunctionDefs::GetNetworkStats}, {"getLoadedModules", CLuaFunctionDefs::GetModules},
            {"getModuleInfo", CLuaFunctionDefs::GetModuleInfo},

            {"setDevelopmentMode", CLuaFunctionDefs::SetDevelopmentMode}, {"getDevelopmentMode", CLuaFunctionDefs::GetDevelopmentMode},
    };

    // Add all functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);

    // Restricted functions
    CLuaCFunctions::AddFunction("setServerConfigSetting", CLuaFunctionDefs::SetServerConfigSetting, true);
    CLuaCFunctions::AddFunction("shutdown", ArgumentParserWarn<false, CLuaFunctionDefs::Shutdown>, true);
}
