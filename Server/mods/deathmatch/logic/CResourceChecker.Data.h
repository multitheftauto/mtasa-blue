/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CResourceChecker.Data.h
 *  PURPOSE:     Resource file content checker/validator/upgrader
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

namespace
{
    //
    // Minimum version requirements for functions/events
    //

    struct SVersionItem
    {
        SString functionName;
        SString minMtaVersion;
    };

    SVersionItem clientFunctionInitList[] = {
        // Features added in 1.5.9
        {"getRoofPosition", "1.5.8-9.20675"},
        {"setVehicleVariant", "1.5.8-9.20693"},
        {"engineRestreamWorld", "1.5.8-9.20704"},
        {"getElementBonePosition", "1.5.8-9.20704"},
        {"getElementBoneRotation", "1.5.8-9.20704"},
        {"getElementBoneMatrix", "1.5.8-9.20704"},
        {"setElementBonePosition", "1.5.8-9.20704"},
        {"setElementBoneRotation", "1.5.8-9.20704"},
        {"setElementBoneMatrix", "1.5.8-9.20704"},
        {"onClientPedsProcessed", "1.5.8-9.20704"},
        {"updateElementRpHAnim", "1.5.8-9.20704"},
        {"engineGetModelVisibleTime", "1.5.8-9.20704"},
        {"engineSetModelVisibleTime", "1.5.8-9.20704"},
        {"resetColorFilter", "1.5.8-9.20718"},
        {"setColorFilter", "1.5.8-9.20718"},
        {"onClientElementDimensionChange", "1.5.8-9.20724"},
        {"getVehicleWheelFrictionState", "1.5.8-9.20726"},
        {"onClientPedVehicleEnter", "1.5.8-9.20740"},
        {"onClientPedVehicleExit", "1.5.8-9.20740"},
        {"setPedEnterVehicle", "1.5.8-9.20740"},
        {"setPedExitVehicle", "1.5.8-9.20740"},
        {"isSoundLooped", "1.5.8-9.20785"},
        {"setSoundLooped", "1.5.8-9.20785"},
        {"isTransferBoxAlwaysVisible", "1.5.8-9.20788"},
        {"isTransferBoxVisible", "1.5.8-9.20788"},
        {"onClientResourceFileDownload", "1.5.8-9.20788"},
        {"onClientTransferBoxProgressChange", "1.5.8-9.20788"},
        {"onClientTransferBoxVisibilityChange", "1.5.8-9.20788"},
        {"setTransferBoxVisible", "1.5.8-9.20788"},
        {"getVehicleDummyPosition", "1.5.8-9.20797"},
        {"getVehicleModelDummyDefaultPosition", "1.5.8-9.20797"},
        {"resetVehicleDummyPositions", "1.5.8-9.20797"},
        {"setVehicleDummyPosition", "1.5.8-9.20797"},
        {"isBrowserRenderingPaused", "1.5.8-9.20806"},
        {"getColPolygonHeight", "1.5.8-9.20807"},
        {"setColPolygonHeight", "1.5.8-9.20807"},
        {"isObjectMoving", "1.5.8-9.20811"},
        {"onClientObjectMoveStart", "1.5.8-9.20811"},
        {"onClientObjectMoveStop", "1.5.8-9.20811"},
        {"onClientElementInteriorChange", "1.5.8-9.20837"},
        {"isShowCollisionsEnabled", "1.5.8-9.20844"},
        {"isShowSoundEnabled", "1.5.8-9.20844"},
        {"showCol", "1.5.8-9.20844"},
        {"showSound", "1.5.8-9.20844"},
        {"clearDebugBox", "1.5.8-9.20854"},
        {"isChatInputBlocked", "1.5.8-9.20898"},
        {"engineStreamingGetUsedMemory", "1.5.8-9.20901"},
        {"engineStreamingFreeUpMemory", "1.5.8-9.20901"},
        {"getSoundEffectParameters", "1.5.8-9.20914"},
        {"setSoundEffectParameter", "1.5.8-9.20914"},
        {"isPedBleeding", "1.5.8-9.20935"},
        {"setPedBleeding", "1.5.8-9.20935"},
        {"getPlayerMapOpacity", "1.5.8-9.20945"},
        {"svgCreate", "1.5.8-9.20979"},
        {"svgGetDocumentXML", "1.5.8-9.20979"},
        {"svgGetSize", "1.5.8-9.20979"},
        {"svgSetDocumentXML", "1.5.8-9.20979"},
        {"svgSetSize", "1.5.8-9.20979"},
        {"generateKeyPair", "1.5.8-9.21055"},
        {"getProcessMemoryStats", "1.5.8-9.21389"},
        {"isCapsLockEnabled", "1.5.8-9.21313"},
        {"isMTAWindowFocused", "1.5.8-9.21313"},
        {"onClientMTAFocusChange", "1.5.8-9.21313"},
        {"getElementLighting", "1.5.8-9.21367"},
        {"setChatboxCharacterLimit", "1.5.8-9.21476"},
        {"getChatboxCharacterLimit", "1.5.8-9.21476"},

        // Features added in 1.6.0
        {"getAllElementData", "1.6.0-9.21695"},
        {"engineLoadIMG", "1.6.0-9.21695"},
        {"engineImageLinkDFF", "1.6.0-9.21695"},
        {"engineImageLinkTXD", "1.6.0-9.21695"},
        {"engineRestoreDFFImage", "1.6.0-9.21695"},
        {"engineRestoreTXDImage", "1.6.0-9.21695"},
        {"engineAddImage", "1.6.0-9.21695"},
        {"engineRemoveImage", "1.6.0-9.21695"},
        {"engineImageGetFilesCount", "1.6.0-9.21695"},
        {"engineImageGetFiles", "1.6.0-9.21695"},
        {"engineImageGetFile", "1.6.0-9.21695"},
        {"engineGetModelTXDID", "1.6.0-9.21695"},
        {"engineSetModelFlags", "1.6.0-9.21695"},
        {"engineGetModelFlags", "1.6.0-9.21695"},
        {"engineResetModelFlags", "1.6.0-9.21695"},
        {"engineGetModelFlag", "1.6.0-9.21695"},
        {"engineSetModelFlag", "1.6.0-9.21695"},
    };

    SVersionItem serverFunctionInitList[] = {
        // Features added in 1.5.9
        {"onElementDimensionChange", "1.5.8-9.20724"},
        {"onPedVehicleEnter", "1.5.8-9.20740"},
        {"onPedVehicleExit", "1.5.8-9.20740"},
        {"isTransferBoxVisible", "1.5.8-9.20788"},
        {"setTransferBoxVisible", "1.5.8-9.20788"},
        {"getColPolygonHeight", "1.5.8-9.20807"},
        {"setColPolygonHeight", "1.5.8-9.20807"},
        {"onResourceLoadStateChange", "1.5.8-9.20809"},
        {"onElementInteriorChange", "1.5.8-9.20837"},
        {"onPlayerResourceStart", "1.5.8-9.20957"},
        {"generateKeyPair", "1.5.8-9.21055"},
        {"getProcessMemoryStats", "1.5.8-9.21389"},

        // Features added in 1.6.0
        {"isObjectBreakable", "1.6.0-9.21765"},
        {"setObjectBreakable", "1.6.0-9.21765"},
    };

    //
    // Deprecated function/events
    //

    struct SDeprecatedItem
    {
        // bRemoved does not mean:
        //     "has this function been removed yet?"
        // bRemoved actually means:
        //     "is not rename?" (you can't rename removed functions)
        bool bRemoved;

        SString strOldName;
        SString strNewName;
        SString strVersion;
    };

    SDeprecatedItem clientDeprecatedList[] = {
        // Client functions
        {false, "getPlayerRotation", "getPedRotation"},
        {false, "canPlayerBeKnockedOffBike", "canPedBeKnockedOffBike"},
        {false, "getPlayerContactElement", "getPedContactElement"},
        {false, "isPlayerInVehicle", "isPedInVehicle"},
        {false, "doesPlayerHaveJetPack", "doesPedHaveJetPack"},
        {false, "isPlayerInWater", "isElementInWater"},
        {false, "isPedInWater", "isElementInWater"},
        {false, "isPlayerOnGround", "isPedOnGround"},
        {false, "getPlayerTask", "getPedTask"},
        {false, "getPlayerSimplestTask", "getPedSimplestTask"},
        {false, "isPlayerDoingTask", "isPedDoingTask"},
        {false, "getPlayerTarget", "getPedTarget"},
        {false, "getPlayerTargetStart", "getPedTargetStart"},
        {false, "getPlayerTargetEnd", "getPedTargetEnd"},
        {false, "getPlayerTargetCollision", "getPedTargetCollision"},
        {false, "getPlayerWeaponSlot", "getPedWeaponSlot"},
        {false, "getPlayerWeapon", "getPedWeapon"},
        {false, "getPlayerAmmoInClip", "getPedAmmoInClip"},
        {false, "getPlayerTotalAmmo", "getPedTotalAmmo"},
        {false, "getPlayerOccupiedVehicle", "getPedOccupiedVehicle"},
        {false, "getPlayerArmor", "getPedArmor"},
        {false, "getPlayerSkin", "getElementModel"},
        {false, "isPlayerChoking", "isPedChoking"},
        {false, "isPlayerDucked", "isPedDucked"},
        {false, "getPlayerStat", "getPedStat"},
        {false, "setPlayerWeaponSlot", "setPedWeaponSlot"},
        {false, "setPlayerSkin", "setElementModel"},
        {false, "setPlayerRotation", "setPedRotation"},
        {false, "setPlayerCanBeKnockedOffBike", "setPedCanBeKnockedOffBike"},
        {false, "setVehicleModel", "setElementModel"},
        {false, "getVehicleModel", "getElementModel"},
        {false, "getPedSkin", "getElementModel"},
        {false, "setPedSkin", "setElementModel"},
        {false, "getObjectRotation", "getElementRotation"},
        {false, "setObjectRotation", "setElementRotation"},
        {false, "getVehicleIDFromName", "getVehicleModelFromName"},
        {false, "getVehicleID", "getElementModel"},
        {false, "getVehicleRotation", "getElementRotation"},
        {false, "getVehicleNameFromID", "getVehicleNameFromModel"},
        {false, "setVehicleRotation", "setElementRotation"},
        {false, "attachElementToElement", "attachElements"},
        {false, "detachElementFromElement", "detachElements"},
        {false, "xmlFindSubNode", "xmlFindChild"},
        {false, "xmlNodeGetSubNodes", "xmlNodeGetChildren"},
        {false, "xmlNodeFindSubNode", "xmlFindChild"},
        {false, "xmlCreateSubNode", "xmlCreateChild"},
        {false, "isPedFrozen", "isElementFrozen"},
        {false, "isVehicleFrozen", "isElementFrozen"},
        {false, "isObjectStatic", "isElementFrozen"},
        {false, "setPedFrozen", "setElementFrozen"},
        {false, "setVehicleFrozen", "setElementFrozen"},
        {false, "setObjectStatic", "setElementFrozen"},
        {false, "isPlayerDead", "isPedDead"},
        {false, "showPlayerHudComponent", "setPlayerHudComponentVisible"},
        {false, "setControlState", "setPedControlState"},
        {false, "getControlState", "getPedControlState"},
        {false, "getVehicleTurnVelocity", "getElementAngularVelocity"},
        {false, "setVehicleTurnVelocity", "setElementAngularVelocity"},
        {false, "getCameraShakeLevel", "getCameraDrunkLevel"},
        {false, "setCameraShakeLevel", "setCameraDrunkLevel"},
        // Edit
        {false, "guiEditSetCaratIndex", "guiEditSetCaretIndex"},
        {false, "guiMemoSetCaratIndex", "guiMemoSetCaretIndex"},
        // XML
        {false, "xmlNodeFindChild", "xmlFindChild"},

        {false, "getComponentPosition", "will return 3 floats instead of a Vector3", "1.5.5-9.11710"},
        {false, "getComponentRotation", "will return 3 floats instead of a Vector3", "1.5.5-9.11710"},

        {false, "getBoundingBox", "will return 6 floats instead of 2 Vector3", "1.5.5-9.13999"},

        // Ped jetpacks
        //{false, "doesPedHaveJetPack", "isPedWearingJetpack"},
    };

    SDeprecatedItem serverDeprecatedList[] = {
        // Server events
        {false, "onClientLogin", "onPlayerLogin"},
        {false, "onClientLogout", "onPlayerLogout"},
        {false, "onClientChangeNick", "onPlayerChangeNick"},
        // Server functions
        {false, "getPlayerSkin", "getElementModel"},
        {false, "setPlayerSkin", "setElementModel"},
        {false, "getVehicleModel", "getElementModel"},
        {false, "setVehicleModel", "setElementModel"},
        {false, "getObjectModel", "getElementModel"},
        {false, "setObjectModel", "setElementModel"},
        {false, "getVehicleID", "getElementModel"},
        {false, "getVehicleIDFromName", "getVehicleModelFromName"},
        {false, "getVehicleNameFromID", "getVehicleNameFromModel"},
        {false, "getPlayerWeaponSlot", "getPedWeaponSlot"},
        {false, "getPlayerWeapon", "getPedWeapon"},
        {false, "getPlayerTotalAmmo", "getPedTotalAmmo"},
        {false, "getPlayerAmmoInClip", "getPedAmmoInClip"},
        {false, "getPlayerArmor", "getPedArmor"},
        {false, "getPlayerRotation", "getPedRotation"},
        {false, "isPlayerChoking", "isPedChoking"},
        {false, "isPlayerDead", "isPedDead"},
        {false, "isPlayerDucked", "isPedDucked"},
        {false, "getPlayerStat", "getPedStat"},
        {false, "getPlayerTarget", "getPedTarget"},
        {false, "getPlayerClothes", "getPedClothes"},
        {false, "doesPlayerHaveJetPack", "doesPedHaveJetPack"},
        {false, "isPlayerInWater", "isElementInWater"},
        {false, "isPedInWater", "isElementInWater"},
        {false, "isPlayerOnGround", "isPedOnGround"},
        {false, "getPlayerFightingStyle", "getPedFightingStyle"},
        {false, "getPlayerGravity", "getPedGravity"},
        {false, "getPlayerContactElement", "getPedContactElement"},
        {false, "setPlayerArmor", "setPedArmor"},
        {false, "setPlayerWeaponSlot", "setPedWeaponSlot"},
        {false, "killPlayer", "killPed"},
        {false, "setPlayerRotation", "setPedRotation"},
        {false, "setPlayerStat", "setPedStat"},
        {false, "addPlayerClothes", "addPedClothes"},
        {false, "removePlayerClothes", "removePedClothes"},
        {false, "setPlayerFightingStyle", "setPedFightingStyle"},
        {false, "setPlayerGravity", "setPedGravity"},
        {false, "setPlayerChoking", "setPedChoking"},
        {false, "warpPlayerIntoVehicle", "warpPedIntoVehicle"},
        {false, "removePlayerFromVehicle", "removePedFromVehicle"},
        {false, "attachElementToElement", "attachElements"},
        {false, "detachElementFromElement", "detachElements"},
        {false, "showPlayerHudComponent", "setPlayerHudComponentVisible"},
        {false, "getVehicleTurnVelocity", "getElementAngularVelocity"},
        {false, "setVehicleTurnVelocity", "setElementAngularVelocity"},

        // Server ped jetpack
        {true, "givePlayerJetPack", "Replaced with setPedWearingJetpack. Refer to the wiki for details"},
        {true, "removePlayerJetPack", "Replaced with setPedWearingJetpack. Refer to the wiki for details"},
        {true, "givePedJetPack", "Replaced with setPedWearingJetpack. Refer to the wiki for details"},
        {true, "removePedJetPack", "Replaced with setPedWearingJetpack. Refer to the wiki for details"},
        {false, "doesPedHaveJetPack", "isPedWearingJetpack"},

        // XML
        {false, "xmlNodeGetSubNodes", "xmlNodeGetChildren"},
        {false, "xmlCreateSubNode", "xmlCreateChild"},
        {false, "xmlFindSubNode", "xmlFindChild"},
        // Frozen
        {false, "isPedFrozen", "isElementFrozen"},
        {false, "isVehicleFrozen", "isElementFrozen"},
        {false, "setPedFrozen", "setElementFrozen"},
        {false, "setVehicleFrozen", "setElementFrozen"},
        // Player
        {false, "getPlayerOccupiedVehicle", "getPedOccupiedVehicle"},
        {false, "getPlayerOccupiedVehicleSeat", "getPedOccupiedVehicleSeat"},
        {false, "isPlayerInVehicle", "isPedInVehicle"},
        {false, "getPlayerFromNick", "getPlayerFromName"},
        // Client
        {false, "getClientName", "getPlayerName"},
        {false, "getClientIP", "getPlayerIP"},
        {false, "getClientAccount", "getPlayerAccount"},
        {true, "getAccountClient", "getAccountPlayer"},
        {false, "setClientName", "setPlayerName"},
        // Utility
        {true, "randFloat", "math.random"},
        {true, "randInt", "math.random"},
        // Weapon
        {false, "giveWeaponAmmo", "giveWeapon"},
        {false, "takeWeaponAmmo", "takeWeapon"},

        // Admin
        {true, "banIP", "Please manually update this.  Refer to the wiki for details"},
        {true, "banSerial", "Please manually update this.  Refer to the wiki for details"},
        {true, "unbanIP", "Please manually update this.  Refer to the wiki for details"},
        {true, "unbanSerial", "Please manually update this.  Refer to the wiki for details"},
        {true, "getBansXML", "Please manually update this.  Refer to the wiki for details"},
        {true, "canPlayerUseFunction", "Please manually update this.  Refer to the wiki for details"},

        // Old Discord implementation (see #2499)
        {true, "setPlayerDiscordJoinParams", "See GitHub PR #2499 for more details"},
    };
}            // namespace
