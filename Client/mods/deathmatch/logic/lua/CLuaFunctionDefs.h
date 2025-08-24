/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.h
 *  PURPOSE:     Lua function definitions class header
 *
 *****************************************************************************/

class CLuaFunctionDefinitions;

#pragma once

#include "LuaCommon.h"
#include "CLuaMain.h"
#include "CLuaTimerManager.h"

class CRegisteredCommands;

#define LUA_DECLARE(x) static int x ( lua_State * luaVM );

class CLuaFunctionDefs
{
    friend class CScriptArgReader;

public:
    static void Initialize(class CLuaManager* pLuaManager, class CScriptDebugging* pScriptDebugging, class CClientGame* pClientGame);

    // Event functions
    LUA_DECLARE(AddEvent);
    LUA_DECLARE(AddEventHandler);
    LUA_DECLARE(RemoveEventHandler);
    LUA_DECLARE(GetEventHandlers);
    LUA_DECLARE(TriggerEvent);
    LUA_DECLARE(TriggerServerEvent);
    LUA_DECLARE(CancelEvent);
    LUA_DECLARE(WasEventCancelled);
    LUA_DECLARE(TriggerLatentServerEvent);
    LUA_DECLARE(GetLatentEventHandles);
    LUA_DECLARE(GetLatentEventStatus);
    LUA_DECLARE(CancelLatentEvent);

    // Misc functions
    LUA_DECLARE(DownloadFile);

    // Output functions
    LUA_DECLARE(OutputConsole);
    LUA_DECLARE(OutputChatBox);
    LUA_DECLARE(OutputClientDebugString);
    LUA_DECLARE(SetClipboard);
    LUA_DECLARE(SetWindowFlashing);
    LUA_DECLARE(ClearChatBox);

    // Notification functions
    LUA_DECLARE(CreateTrayNotification);
    LUA_DECLARE(IsTrayNotificationEnabled);

    // Clothes and body functions
    LUA_DECLARE(GetBodyPartName);
    LUA_DECLARE(GetClothesByTypeIndex);
    LUA_DECLARE(GetTypeIndexFromClothes);
    LUA_DECLARE(GetClothesTypeName);
    LUA_DECLARE(AddClothingModel);
    LUA_DECLARE(RemoveClothingModel);

    // Cursor funcs
    LUA_DECLARE(GetCursorPosition);
    LUA_DECLARE(SetCursorPosition);
    LUA_DECLARE(IsCursorShowing);
    LUA_DECLARE(GetCursorAlpha);
    LUA_DECLARE(SetCursorAlpha);

    LUA_DECLARE(ShowCursor);

    // Util functions to make scripting easier for the end user
    // Some of these are based on standard mIRC script funcs as a lot of people will be used to them
    LUA_DECLARE(GetValidPedModels);
    LUA_DECLARE(SetDevelopmentMode);
    LUA_DECLARE(GetDevelopmentMode);

    // Input functions
    LUA_DECLARE(BindKey);
    LUA_DECLARE(UnbindKey);
    LUA_DECLARE(GetKeyState);
    LUA_DECLARE(GetControlState);
    LUA_DECLARE(GetAnalogControlState);
    LUA_DECLARE(SetAnalogControlState);
    LUA_DECLARE(IsControlEnabled);
    LUA_DECLARE(GetBoundKeys);
    LUA_DECLARE(GetFunctionsBoundToKey);
    LUA_DECLARE(GetKeyBoundToFunction);
    LUA_DECLARE(GetCommandsBoundToKey);
    LUA_DECLARE(GetKeyBoundToCommand);

    LUA_DECLARE(SetControlState);
    LUA_DECLARE(ToggleControl);
    LUA_DECLARE(ToggleAllControls);

    // Command funcs
    LUA_DECLARE(AddCommandHandler);
    LUA_DECLARE(RemoveCommandHandler);
    LUA_DECLARE(ExecuteCommandHandler);
    LUA_DECLARE(GetCommandHandlers);

    // Utility
    LUA_DECLARE(GetNetworkUsageData);
    LUA_DECLARE(GetNetworkStats);
    LUA_DECLARE(GetPerformanceStats);
    LUA_DECLARE(AddDebugHook);
    LUA_DECLARE(RemoveDebugHook);
    LUA_DECLARE(GetVersion);

    // L10n
    LUA_DECLARE(GetLocalization);
    LUA_DECLARE(GetKeyboardLayout);

    // Voice functions
    LUA_DECLARE(IsVoiceEnabled);

private:
    // Static references to objects
    static CLuaManager*               m_pLuaManager;
    static CScriptDebugging*          m_pScriptDebugging;
    static CClientGame*               m_pClientGame;
    static CClientManager*            m_pManager;
    static CClientEntity*             m_pRootEntity;
    static CClientGUIManager*         m_pGUIManager;
    static CClientPedManager*         m_pPedManager;
    static CClientPlayerManager*      m_pPlayerManager;
    static CClientRadarMarkerManager* m_pRadarMarkerManager;
    static CResourceManager*          m_pResourceManager;
    static CClientVehicleManager*     m_pVehicleManager;
    static CClientColManager*         m_pColManager;
    static CClientObjectManager*      m_pObjectManager;
    static CClientTeamManager*        m_pTeamManager;
    static CRenderWare*               m_pRenderWare;
    static CClientMarkerManager*      m_pMarkerManager;
    static CClientPickupManager*      m_pPickupManager;
    static CClientDFFManager*         m_pDFFManager;
    static CClientColModelManager*    m_pColModelManager;
    static CRegisteredCommands*       m_pRegisteredCommands;
    static CClientIMGManager*         m_pImgManager;
    static CClientBuildingManager*    m_pBuildingManager;
};
