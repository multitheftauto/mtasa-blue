// How to add a new event:
// Just add it to BUILTINEVENT_LIST_APPLY_MACRO

// Sorry for the macro of doom stuff, but than copy pasting it 5 times

// Macro of doom: Calls macro with arguments as seen below.
// # can be used on the first argument to form a string. See code for examples in BuiltInEvents.h/cpp

// Give special attention to the name, it must start with a lowercase letter!
#define BUILTINEVENT_ID_EXPAND(id) BuiltInEvent::ID::id
#ifdef MTA_CLIENT
#define BUILTINEVENT_LIST_APPLY_MACRO(macro) \
    /* Resource events */ \
    macro(onClientResourceStart, ON_CLIENT_RESOURCE_START) \
    macro(onClientResourceStop, ON_CLIENT_RESOURCE_STOP) \
    \
    /* Element events */ \
    macro(onClientElementDataChange, ON_CLIENT_ELEMENT_DATA_CHANGE) \
    macro(onClientElementStreamIn, ON_CLIENT_ELEMENT_STREAM_IN) \
    macro(onClientElementStreamOut, ON_CLIENT_ELEMENT_STREAM_OUT) \
    macro(onClientElementDestroy, ON_CLIENT_ELEMENT_DESTROY) \
    macro(onClientElementModelChange, ON_CLIENT_ELEMENT_MODEL_CHANGE) \
    macro(onClientElementDimensionChange, ON_CLIENT_ELEMENT_DIMENSION_CHANGE) \
    macro(onClientElementInteriorChange, ON_CLIENT_ELEMENT_INTERIOR_CHANGE) \
    \
    /* Player events */ \
    macro(onClientPlayerJoin, ON_CLIENT_PLAYER_JOIN) \
    macro(onClientPlayerQuit, ON_CLIENT_PLAYER_QUIT) \
    macro(onClientPlayerTarget, ON_CLIENT_PLAYER_TARGET) \
    macro(onClientPlayerSpawn, ON_CLIENT_PLAYER_SPAWN) \
    macro(onClientPlayerChangeNick, ON_CLIENT_PLAYER_CHANGE_NICK) \
    macro(onClientPlayerVehicleEnter, ON_CLIENT_PLAYER_VEHICLE_ENTER) \
    macro(onClientPlayerVehicleExit, ON_CLIENT_PLAYER_VEHICLE_EXIT) \
    macro(onClientPlayerTask, ON_CLIENT_PLAYER_TASK) \
    macro(onClientPlayerWeaponSwitch, ON_CLIENT_PLAYER_WEAPON_SWITCH) \
    macro(onClientPlayerStuntStart, ON_CLIENT_PLAYER_STUNT_START) \
    macro(onClientPlayerStuntFinish, ON_CLIENT_PLAYER_STUNT_FINISH) \
    macro(onClientPlayerRadioSwitch, ON_CLIENT_PLAYER_RADIO_SWITCH) \
    macro(onClientPlayerDamage, ON_CLIENT_PLAYER_DAMAGE) \
    macro(onClientPlayerWeaponFire, ON_CLIENT_PLAYER_WEAPON_FIRE) \
    macro(onClientPlayerWasted, ON_CLIENT_PLAYER_WASTED) \
    macro(onClientPlayerChoke, ON_CLIENT_PLAYER_CHOKE) \
    macro(onClientPlayerVoiceStart, ON_CLIENT_PLAYER_VOICE_START) \
    macro(onClientPlayerVoiceStop, ON_CLIENT_PLAYER_VOICE_STOP) \
    macro(onClientPlayerVoicePause, ON_CLIENT_PLAYER_VOICE_PAUSE) \
    macro(onClientPlayerVoiceResumed, ON_CLIENT_PLAYER_VOICE_RESUMED) \
    macro(onClientPlayerStealthKill, ON_CLIENT_PLAYER_STEALTH_KILL) \
    macro(onClientPlayerHitByWaterCannon, ON_CLIENT_PLAYER_HIT_BY_WATER_CANNON) \
    macro(onClientPlayerHeliKilled, ON_CLIENT_PLAYER_HELI_KILLED) \
    macro(onClientPlayerPickupHit, ON_CLIENT_PLAYER_PICKUP_HIT) \
    macro(onClientPlayerPickupLeave, ON_CLIENT_PLAYER_PICKUP_LEAVE) \
    macro(onClientPlayerNetworkStatus, ON_CLIENT_PLAYER_NETWORK_STATUS) \
    \
    /* Ped events */ \
    macro(onClientPedDamage, ON_CLIENT_PED_DAMAGE) \
    macro(onClientPedVehicleEnter, ON_CLIENT_PED_VEHICLE_ENTER) \
    macro(onClientPedVehicleExit, ON_CLIENT_PED_VEHICLE_EXIT) \
    macro(onClientPedWeaponFire, ON_CLIENT_PED_WEAPON_FIRE) \
    macro(onClientPedWasted, ON_CLIENT_PED_WASTED) \
    macro(onClientPedChoke, ON_CLIENT_PED_CHOKE) \
    macro(onClientPedHeliKilled, ON_CLIENT_PED_HELI_KILLED) \
    macro(onClientPedHitByWaterCannon, ON_CLIENT_PED_HIT_BY_WATER_CANNON) \
    macro(onClientPedStep, ON_CLIENT_PED_STEP) \
    \
    /* Vehicle events */ \
    macro(onClientVehicleRespawn, ON_CLIENT_VEHICLE_RESPAWN) \
    macro(onClientVehicleEnter, ON_CLIENT_VEHICLE_ENTER) \
    macro(onClientVehicleExit, ON_CLIENT_VEHICLE_EXIT) \
    macro(onClientVehicleStartEnter, ON_CLIENT_VEHICLE_START_ENTER) \
    macro(onClientVehicleStartExit, ON_CLIENT_VEHICLE_START_EXIT) \
    macro(onClientTrailerAttach, ON_CLIENT_TRAILER_ATTACH) \
    macro(onClientTrailerDetach, ON_CLIENT_TRAILER_DETACH) \
    macro(onClientVehicleExplode, ON_CLIENT_VEHICLE_EXPLODE) \
    macro(onClientVehicleCollision, ON_CLIENT_VEHICLE_COLLISION) \
    macro(onClientVehicleDamage, ON_CLIENT_VEHICLE_DAMAGE) \
    macro(onClientVehicleNitroStateChange, ON_CLIENT_VEHICLE_NITRO_STATE_CHANGE) \
    macro(onClientVehicleWeaponHit, ON_CLIENT_VEHICLE_WEAPON_HIT) \
    \
    /* GUI events */ \
    macro(onClientGUIClick, ON_CLIENT_GUI_CLICK) \
    macro(onClientGUIDoubleClick, ON_CLIENT_GUI_DOUBLE_CLICK) \
    macro(onClientGUIMouseDown, ON_CLIENT_GUI_MOUSE_DOWN) \
    macro(onClientGUIMouseUp, ON_CLIENT_GUI_MOUSE_UP) \
    macro(onClientGUIScroll, ON_CLIENT_GUI_SCROLL) \
    macro(onClientGUIChanged, ON_CLIENT_GUI_CHANGED) \
    macro(onClientGUIAccepted, ON_CLIENT_GUI_ACCEPTED) \
    macro(onClientGUITabSwitched, ON_CLIENT_GUI_TAB_SWITCHED) \
    macro(onClientGUIComboBoxAccepted, ON_CLIENT_GUI_COMBO_BOX_ACCEPTED) \
    \
    /* Input events */ \
    macro(onClientDoubleClick, ON_CLIENT_DOUBLE_CLICK) \
    macro(onClientMouseMove, ON_CLIENT_MOUSE_MOVE) \
    macro(onClientMouseEnter, ON_CLIENT_MOUSE_ENTER) \
    macro(onClientMouseLeave, ON_CLIENT_MOUSE_LEAVE) \
    macro(onClientMouseWheel, ON_CLIENT_MOUSE_WHEEL) \
    macro(onClientGUIMove, ON_CLIENT_GUI_MOVE) \
    macro(onClientGUISize, ON_CLIENT_GUI_SIZE) \
    macro(onClientGUIFocus, ON_CLIENT_GUI_FOCUS) \
    macro(onClientGUIBlur, ON_CLIENT_GUI_BLUR) \
    macro(onClientKey, ON_CLIENT_KEY) \
    macro(onClientCharacter, ON_CLIENT_CHARACTER) \
    macro(onClientPaste, ON_CLIENT_PASTE) \
    \
    /* Console events */ \
    macro(onClientConsole, ON_CLIENT_CONSOLE) \
    \
    /* Chat events */ \
    macro(onClientChatMessage, ON_CLIENT_CHAT_MESSAGE) \
    \
    /* Debug events */ \
    macro(onClientDebugMessage, ON_CLIENT_DEBUG_MESSAGE) \
    \
    /* Game events */ \
    macro(onClientPreRender, ON_CLIENT_PRE_RENDER) \
    macro(onClientPedsProcessed, ON_CLIENT_PEDS_PROCESSED) \
    macro(onClientHUDRender, ON_CLIENT_H_U_D_RENDER) \
    macro(onClientRender, ON_CLIENT_RENDER) \
    macro(onClientMinimize, ON_CLIENT_MINIMIZE) \
    macro(onClientRestore, ON_CLIENT_RESTORE) \
    \
    /* Cursor events */ \
    macro(onClientClick, ON_CLIENT_CLICK) \
    macro(onClientCursorMove, ON_CLIENT_CURSOR_MOVE) \
    \
    /* Marker events */ \
    macro(onClientMarkerHit, ON_CLIENT_MARKER_HIT) \
    macro(onClientMarkerLeave, ON_CLIENT_MARKER_LEAVE) \
    \
    /* Marker events */ \
    macro(onClientPickupHit, ON_CLIENT_PICKUP_HIT) \
    macro(onClientPickupLeave, ON_CLIENT_PICKUP_LEAVE) \
    \
    /* Col-shape events */ \
    macro(onClientColShapeHit, ON_CLIENT_COL_SHAPE_HIT) \
    macro(onClientColShapeLeave, ON_CLIENT_COL_SHAPE_LEAVE) \
    macro(onClientElementColShapeHit, ON_CLIENT_ELEMENT_COL_SHAPE_HIT) \
    macro(onClientElementColShapeLeave, ON_CLIENT_ELEMENT_COL_SHAPE_LEAVE) \
    \
    /* Explosion events */ \
    macro(onClientExplosion, ON_CLIENT_EXPLOSION) \
    \
    /* Projectile events */ \
    macro(onClientProjectileCreation, ON_CLIENT_PROJECTILE_CREATION) \
    \
    /* Sound events */ \
    macro(onClientSoundStream, ON_CLIENT_SOUND_STREAM) \
    macro(onClientSoundFinishedDownload, ON_CLIENT_SOUND_FINISHED_DOWNLOAD) \
    macro(onClientSoundChangedMeta, ON_CLIENT_SOUND_CHANGED_META) \
    macro(onClientSoundStarted, ON_CLIENT_SOUND_STARTED) \
    macro(onClientSoundStopped, ON_CLIENT_SOUND_STOPPED) \
    macro(onClientSoundBeat, ON_CLIENT_SOUND_BEAT) \
    \
    /* Object events */ \
    macro(onClientObjectDamage, ON_CLIENT_OBJECT_DAMAGE) \
    macro(onClientObjectBreak, ON_CLIENT_OBJECT_BREAK) \
    macro(onClientObjectMoveStart, ON_CLIENT_OBJECT_MOVE_START) \
    macro(onClientObjectMoveStop, ON_CLIENT_OBJECT_MOVE_STOP) \
    \
    /* Web events */ \
    macro(onClientBrowserWhitelistChange, ON_CLIENT_BROWSER_WHITELIST_CHANGE) \
    macro(onClientBrowserCreated, ON_CLIENT_BROWSER_CREATED) \
    macro(onClientBrowserLoadingStart, ON_CLIENT_BROWSER_LOADING_START) \
    macro(onClientBrowserDocumentReady, ON_CLIENT_BROWSER_DOCUMENT_READY) \
    macro(onClientBrowserLoadingFailed, ON_CLIENT_BROWSER_LOADING_FAILED) \
    macro(onClientBrowserNavigate, ON_CLIENT_BROWSER_NAVIGATE) \
    macro(onClientBrowserPopup, ON_CLIENT_BROWSER_POPUP) \
    macro(onClientBrowserCursorChange, ON_CLIENT_BROWSER_CURSOR_CHANGE) \
    macro(onClientBrowserTooltip, ON_CLIENT_BROWSER_TOOLTIP) \
    macro(onClientBrowserInputFocusChanged, ON_CLIENT_BROWSER_INPUT_FOCUS_CHANGED) \
    macro(onClientBrowserResourceBlocked, ON_CLIENT_BROWSER_RESOURCE_BLOCKED) \
    \
    /* Misc events */ \
    macro(onClientFileDownloadComplete, ON_CLIENT_FILE_DOWNLOAD_COMPLETE) \
    \
    macro(onClientResourceFileDownload, ON_CLIENT_RESOURCE_FILE_DOWNLOAD) \
    macro(onClientTransferBoxProgressChange, ON_CLIENT_TRANSFER_BOX_PROGRESS_CHANGE) \
    macro(onClientTransferBoxVisibilityChange, ON_CLIENT_TRANSFER_BOX_VISIBILITY_CHANGE) \
    \
    macro(onClientWeaponFire, ON_CLIENT_WEAPON_FIRE) \
    \
    macro(onClientWorldSound, ON_CLIENT_WORLD_SOUND)
#else
#define BUILTINEVENT_LIST_APPLY_MACRO(macro) \
    /* Resource events */ \
    macro(onResourcePreStart, ON_RESOURCE_PRE_START) \
    macro(onResourceStart, ON_RESOURCE_START) \
    macro(onResourceStop, ON_RESOURCE_STOP) \
    macro(onResourceLoadStateChange, ON_RESOURCE_LOAD_STATE_CHANGE) \
    \
    /* Blip events */ \
    \
    /* Marker events */ \
    macro(onMarkerHit, ON_MARKER_HIT) \
    macro(onMarkerLeave, ON_MARKER_LEAVE) \
    \
    /* Voice events */ \
    macro(onPlayerVoiceStart, ON_PLAYER_VOICE_START) \
    macro(onPlayerVoiceStop, ON_PLAYER_VOICE_STOP) \
    \
    /* Object events */ \
    \
    /* Pickup events */ \
    macro(onPickupHit, ON_PICKUP_HIT) \
    macro(onPickupLeave, ON_PICKUP_LEAVE) \
    macro(onPickupUse, ON_PICKUP_USE) \
    macro(onPickupSpawn, ON_PICKUP_SPAWN) \
    \
    /* Player events */ \
    macro(onPlayerConnect, ON_PLAYER_CONNECT) \
    macro(onPlayerChat, ON_PLAYER_CHAT) \
    macro(onPlayerDamage, ON_PLAYER_DAMAGE) \
    macro(onPlayerVehicleEnter, ON_PLAYER_VEHICLE_ENTER) \
    macro(onPlayerVehicleExit, ON_PLAYER_VEHICLE_EXIT) \
    macro(onPlayerJoin, ON_PLAYER_JOIN) \
    macro(onPlayerQuit, ON_PLAYER_QUIT) \
    macro(onPlayerSpawn, ON_PLAYER_SPAWN) \
    macro(onPlayerTarget, ON_PLAYER_TARGET) \
    macro(onPlayerWasted, ON_PLAYER_WASTED) \
    macro(onPlayerWeaponSwitch, ON_PLAYER_WEAPON_SWITCH) \
    macro(onPlayerMarkerHit, ON_PLAYER_MARKER_HIT) \
    macro(onPlayerMarkerLeave, ON_PLAYER_MARKER_LEAVE) \
    macro(onPlayerPickupHit, ON_PLAYER_PICKUP_HIT) \
    macro(onPlayerPickupLeave, ON_PLAYER_PICKUP_LEAVE) \
    macro(onPlayerPickupUse, ON_PLAYER_PICKUP_USE) \
    macro(onPlayerClick, ON_PLAYER_CLICK) \
    macro(onPlayerContact, ON_PLAYER_CONTACT) \
    macro(onPlayerBan, ON_PLAYER_BAN) \
    macro(onPlayerLogin, ON_PLAYER_LOGIN) \
    macro(onPlayerLogout, ON_PLAYER_LOGOUT) \
    macro(onPlayerChangeNick, ON_PLAYER_CHANGE_NICK) \
    macro(onPlayerPrivateMessage, ON_PLAYER_PRIVATE_MESSAGE) \
    macro(onPlayerStealthKill, ON_PLAYER_STEALTH_KILL) \
    macro(onPlayerMute, ON_PLAYER_MUTE) \
    macro(onPlayerUnmute, ON_PLAYER_UNMUTE) \
    macro(onPlayerCommand, ON_PLAYER_COMMAND) \
    macro(onPlayerModInfo, ON_PLAYER_MOD_INFO) \
    macro(onPlayerACInfo, ON_PLAYER_A_C_INFO) \
    macro(onPlayerNetworkStatus, ON_PLAYER_NETWORK_STATUS) \
    macro(onPlayerScreenShot, ON_PLAYER_SCREEN_SHOT) \
    macro(onPlayerDiscordJoin, ON_PLAYER_DISCORD_JOIN) \
    \
    /* Ped events */ \
    macro(onPedVehicleEnter, ON_PED_VEHICLE_ENTER) \
    macro(onPedVehicleExit, ON_PED_VEHICLE_EXIT) \
    macro(onPedWasted, ON_PED_WASTED) \
    macro(onPedWeaponSwitch, ON_PED_WEAPON_SWITCH) \
    \
    /* Element events */ \
    macro(onElementColShapeHit, ON_ELEMENT_COL_SHAPE_HIT) \
    macro(onElementColShapeLeave, ON_ELEMENT_COL_SHAPE_LEAVE) \
    macro(onElementClicked, ON_ELEMENT_CLICKED) \
    macro(onElementDataChange, ON_ELEMENT_DATA_CHANGE) \
    macro(onElementDestroy, ON_ELEMENT_DESTROY) \
    macro(onElementStartSync, ON_ELEMENT_START_SYNC) \
    macro(onElementStopSync, ON_ELEMENT_STOP_SYNC) \
    macro(onElementModelChange, ON_ELEMENT_MODEL_CHANGE) \
    macro(onElementDimensionChange, ON_ELEMENT_DIMENSION_CHANGE) \
    macro(onElementInteriorChange, ON_ELEMENT_INTERIOR_CHANGE) \
    \
    /* Radar area events */ \
    \
    /* Shape events */ \
    macro(onColShapeHit, ON_COL_SHAPE_HIT) \
    macro(onColShapeLeave, ON_COL_SHAPE_LEAVE) \
    \
    /* Vehicle events */ \
    macro(onVehicleDamage, ON_VEHICLE_DAMAGE) \
    macro(onVehicleRespawn, ON_VEHICLE_RESPAWN) \
    macro(onTrailerAttach, ON_TRAILER_ATTACH) \
    macro(onTrailerDetach, ON_TRAILER_DETACH) \
    macro(onVehicleStartEnter, ON_VEHICLE_START_ENTER) \
    macro(onVehicleStartExit, ON_VEHICLE_START_EXIT) \
    macro(onVehicleEnter, ON_VEHICLE_ENTER) \
    macro(onVehicleExit, ON_VEHICLE_EXIT) \
    macro(onVehicleExplode, ON_VEHICLE_EXPLODE) \
    \
    /* Console events */ \
    macro(onConsole, ON_CONSOLE) \
    \
    /* Debug events */ \
    macro(onDebugMessage, ON_DEBUG_MESSAGE) \
    \
    /* Ban events */ \
    macro(onBan, ON_BAN) \
    macro(onUnban, ON_UNBAN) \
    \
    /* Account events */ \
    macro(onAccountDataChange, ON_ACCOUNT_DATA_CHANGE) \
    \
    /* Other events */ \
    macro(onSettingChange, ON_SETTING_CHANGE) \
    macro(onChatMessage, ON_CHAT_MESSAGE) \
    \
    /* Weapon events */ \
    macro(onWeaponFire, ON_WEAPON_FIRE) \
    macro(onPlayerWeaponFire, ON_PLAYER_WEAPON_FIRE)
#endif
// ACHTUNG: DONT FORGET TO UNDEF THE MACROS IF YOU INCLUDE THIS FILE IN A HEADER


