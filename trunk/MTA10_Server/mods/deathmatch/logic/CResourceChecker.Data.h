/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceChecker.Data.h
*  PURPOSE:     Resource file content checker/validator/upgrader
*  DEVELOPERS:  Developer wishes to remain anonymous
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace
{

    //
    // Minimum version requirments for functions/events
    //

    struct SVersionItem
    {
        SString functionName;
        SString minMtaVersion;      
    };

    SVersionItem clientFunctionInitList[] = {
                                         { "createSWATRope",            "1.1.1-9.03250" },
                                         { "toJSON",                    "1.1.1-9.03316" },
                                         { "fromJSON",                  "1.1.1-9.03316" },
                                         { "dxSetShaderTessellation",   "1.1.1-9.03316" },
                                         { "setDevelopmentMode",        "1.1.1-9.03355" },
                                         { "getDevelopmentMode",        "1.1.1-9.03355" },
                                         { "getOriginalWeaponProperty", "1.2" },
                                         { "getWeaponProperty",         "1.2" },
                                         { "getVehicleVariant",         "1.2" },
                                         { "setLowLODElement",          "1.2" },
                                         { "getLowLODElement",          "1.2" },
                                         { "isElementLowLOD",           "1.2" },
                                         { "resetWaterLevel",           "1.2" },
                                         { "removeWorldModel",          "1.2.0-9.03618" },
                                         { "restoreWorldModel",         "1.2.0-9.03618" },
                                         { "restoreAllWorldModels",     "1.2.0-9.03618" },
                                         { "dxSetShaderTransform",      "1.2.0-9.03618" },
                                         { "onClientVehicleCollision",  "1.3" },
                                         { "setBirdsEnabled",           "1.3" },
                                         { "setOcclusionsEnabled",      "1.3" },
                                         { "dxGetTexturePixels",        "1.3" },
                                         { "dxSetTexturePixels",        "1.3" },
                                         { "dxGetPixelsSize",           "1.3" },
                                         { "dxGetPixelsFormat",         "1.3" },
                                         { "dxConvertPixels",           "1.3" },
                                         { "dxGetPixelColor",           "1.3" },
                                         { "dxSetPixelColor",           "1.3" },
                                        };

    SVersionItem serverFunctionInitList[] = {
                                         { "onChatMessage",             "1.1.1-9.03316" },
                                         { "getPlayerIdleTime",         "1.1.1-9.03316" },
                                         { "copyResource",              "1.1.1-9.03316" },
                                         { "renameResource",            "1.1.1-9.03316" },
                                         { "deleteResource",            "1.1.1-9.03316" },
                                         { "dbConnect",                 "1.1.1-9.03328" },
                                         { "dbQuery",                   "1.1.1-9.03328" },
                                         { "dbPoll",                    "1.1.1-9.03328" },
                                         { "dbFree",                    "1.1.1-9.03328" },
                                         { "dbExec",                    "1.1.1-9.03341" },
                                         { "resendPlayerModInfo",       "1.1.1-9.03451" },
                                         { "getResourceACLRequests",    "1.1.1-9.03503" },
                                         { "updateResourceACLRequest",  "1.1.1-9.03503" },
                                         { "getOriginalWeaponProperty", "1.2" },
                                         { "getWeaponProperty",         "1.2" },
                                         { "setWeaponProperty",         "1.2" },
                                         { "getVehicleVariant",         "1.2" },
                                         { "setVehicleVariant",         "1.2" },
                                         { "setLowLODElement",          "1.2" },
                                         { "getLowLODElement",          "1.2" },
                                         { "isElementLowLOD",           "1.2" },
                                         { "resetWaterLevel",           "1.2" },
                                         { "removeWorldModel",          "1.2.0-9.03618" },
                                         { "restoreWorldModel",         "1.2.0-9.03618" },
                                         { "restoreAllWorldModels",     "1.2.0-9.03618" },
                                         { "takePlayerScreenShot",      "1.3" },
                                         { "onPlayerScreenShot",        "1.3" },
                                         { "setOcclusionsEnabled",      "1.3" },
                                         { "getOcclusionsEnabled",      "1.3" },
                                         { "fetchRemote",               "1.3.0-9.03736" },
                                        };


    //
    // Deprecated function/events
    //

    struct SDeprecatedItem
    {
        bool bRemoved;
        SString strOldName;
        SString strNewName;      
    };

    SDeprecatedItem clientDeprecatedList [] = {
                // Client functions
                { false, "getPlayerRotation",              "getPedRotation" },
                { false, "canPlayerBeKnockedOffBike",      "canPedBeKnockedOffBike" },
                { false, "getPlayerContactElement",        "getPedContactElement" },
                { false, "isPlayerInVehicle",              "isPedInVehicle" },
                { false, "doesPlayerHaveJetPack",          "doesPedHaveJetPack" },
                { false, "isPlayerInWater",                "isElementInWater" },
                { false, "isPedInWater",                   "isElementInWater" },
                { false, "isPlayerOnGround",               "isPedOnGround" },
                { false, "getPlayerTask",                  "getPedTask" },
                { false, "getPlayerSimplestTask",          "getPedSimplestTask" },
                { false, "isPlayerDoingTask",              "isPedDoingTask" },
                { false, "getPlayerTarget",                "getPedTarget" },
                { false, "getPlayerTargetStart",           "getPedTargetStart" },
                { false, "getPlayerTargetEnd",             "getPedTargetEnd" },
                { false, "getPlayerTargetRange",           "getPedTargetRange" },
                { false, "getPlayerTargetCollision",       "getPedTargetCollision" },
                { false, "getPlayerWeaponSlot",            "getPedWeaponSlot" },
                { false, "getPlayerWeapon",                "getPedWeapon" },
                { false, "getPlayerAmmoInClip",            "getPedAmmoInClip" },
                { false, "getPlayerTotalAmmo",             "getPedTotalAmmo" },
                { false, "getPlayerOccupiedVehicle",       "getPedOccupiedVehicle" },
                { false, "getPlayerArmor",                 "getPedArmor" },
                { false, "getPlayerSkin",                  "getElementModel" },
                { false, "isPlayerChoking",                "isPedChoking" },
                { false, "isPlayerDucked",                 "isPedDucked" },
                { false, "getPlayerStat",                  "getPedStat" },
                { false, "setPlayerWeaponSlot",            "setPedWeaponSlot" },
                { false, "setPlayerSkin",                  "setElementModel" },
                { false, "setPlayerRotation",              "setPedRotation" },
                { false, "setPlayerCanBeKnockedOffBike",   "setPedCanBeKnockedOffBike" },
                { false, "setVehicleModel",                "setElementModel" },
                { false, "getVehicleModel",                "getElementModel" },
                { false, "getPedSkin",                     "getElementModel" },
                { false, "setPedSkin",                     "setElementModel" },
                { false, "getObjectRotation",              "getElementRotation" },
                { false, "setObjectRotation",              "setElementRotation" },
                { false, "getModel",                       "getElementModel" },
                { false, "getVehicleIDFromName",           "getVehicleModelFromName" },
                { false, "getVehicleID",                   "getElementModel" },
                { false, "getVehicleRotation",             "getElementRotation" },
                { false, "getVehicleNameFromID",           "getVehicleNameFromModel" },
                { false, "setVehicleRotation",             "setElementRotation" },
                { false, "attachElementToElement",         "attachElements" },
                { false, "detachElementFromElement",       "detachElements" },
                { false, "xmlFindSubNode",                 "xmlFindChild" },
                { false, "xmlNodeGetSubNodes",             "xmlNodeGetChildren" },
                { false, "xmlNodeFindSubNode",             "xmlFindChild" },
                { false, "xmlCreateSubNode",               "xmlCreateChild" },
                { false, "isPedFrozen",                    "isElementFrozen" },
                { false, "isVehicleFrozen",                "isElementFrozen" },
                { false, "isObjectStatic",                 "isElementFrozen" },
                { false, "setPedFrozen",                   "setElementFrozen" },
                { false, "setVehicleFrozen",               "setElementFrozen" },
                { false, "setObjectStatic",                "setElementFrozen" },
                // Edit
                { false, "guiEditSetCaratIndex",           "guiEditSetCaretIndex" },
                // XML
                { false, "xmlNodeFindChild",               "xmlFindChild" },
            };


    SDeprecatedItem serverDeprecatedList [] = {
                // Server events
                { false, "onClientLogin",                  "onPlayerLogin" },
                { false, "onClientLogout",                 "onPlayerLogout" },
                { false, "onClientChangeNick",             "onPlayerChangeNick" },
                // Server functions
                { false, "getPlayerSkin",                  "getElementModel" },
                { false, "setPlayerSkin",                  "setElementModel" },
                { false, "getVehicleModel",                "getElementModel" },
                { false, "setVehicleModel",                "setElementModel" },
                { false, "getObjectModel",                 "getElementModel" },
                { false, "setObjectModel",                 "setElementModel" },
                { false, "getVehicleID",                   "getElementModel" },
                { false, "getVehicleIDFromName",           "getVehicleModelFromName" },
                { false, "getVehicleNameFromID",           "getVehicleNameFromModel" },
                { false, "getPlayerWeaponSlot",            "getPedWeaponSlot" },
                { false, "getPlayerWeapon",                "getPedWeapon" },
                { false, "getPlayerTotalAmmo",             "getPedTotalAmmo" },
                { false, "getPlayerAmmoInClip",            "getPedAmmoInClip" },
                { false, "getPlayerArmor",                 "getPedArmor" },
                { false, "getPlayerRotation",              "getPedRotation" },
                { false, "isPlayerChoking",                "isPedChoking" },
                { false, "isPlayerDead",                   "isPedDead" },
                { false, "isPlayerDucked",                 "isPedDucked" },
                { false, "getPlayerStat",                  "getPedStat" },
                { false, "getPlayerTarget",                "getPedTarget" },
                { false, "getPlayerClothes",               "getPedClothes" },
                { false, "doesPlayerHaveJetPack",          "doesPedHaveJetPack" },
                { false, "isPlayerInWater",                "isElementInWater" },
                { false, "isPedInWater",                   "isElementInWater" },
                { false, "isPlayerOnGround",               "isPedOnGround" },
                { false, "getPlayerFightingStyle",         "getPedFightingStyle" },
                { false, "getPlayerGravity",               "getPedGravity" },
                { false, "getPlayerContactElement",        "getPedContactElement" },
                { false, "setPlayerArmor",                 "setPedArmor" },
                { false, "setPlayerWeaponSlot",            "setPedWeaponSlot" },
                { false, "killPlayer",                     "killPed" },
                { false, "setPlayerRotation",              "setPedRotation" },
                { false, "setPlayerStat",                  "setPedStat" },
                { false, "addPlayerClothes",               "addPedClothes" },
                { false, "removePlayerClothes",            "removePedClothes" },
                { false, "givePlayerJetPack",              "givePedJetPack" },
                { false, "removePlayerJetPack",            "removePedJetPack" },
                { false, "setPlayerFightingStyle",         "setPedFightingStyle" },
                { false, "setPlayerGravity",               "setPedGravity" },
                { false, "setPlayerChoking",               "setPedChoking" },
                { false, "warpPlayerIntoVehicle",          "warpPedIntoVehicle" },
                { false, "removePlayerFromVehicle",        "removePedFromVehicle" },
                { false, "attachElementToElement",         "attachElements" },
                { false, "detachElementFromElement",       "detachElements" },
                // XML
                { false, "xmlNodeGetSubNodes",             "xmlNodeGetChildren" },
                { false, "xmlCreateSubNode",               "xmlCreateChild" },
                { false, "xmlFindSubNode",                 "xmlFindChild" },
                // Frozen
                { false, "isPedFrozen",                    "isElementFrozen" },
                { false, "isVehicleFrozen",                "isElementFrozen" },
                { false, "setPedFrozen",                   "setElementFrozen" },
                { false, "setVehicleFrozen",               "setElementFrozen" },
                // Player
                { false, "getPlayerOccupiedVehicle",       "getPedOccupiedVehicle" },
                { false, "getPlayerOccupiedVehicleSeat",   "getPedOccupiedVehicleSeat" },
                { false, "isPlayerInVehicle",              "isPedInVehicle" },
                { false, "getPlayerFromNick",              "getPlayerFromName" },
                // Client
                { false, "getClientName",                  "getPlayerName" },
                { false, "getClientIP",                    "getPlayerIP" },
                { false, "getClientAccount",               "getPlayerAccount" },
                { false, "getAccountClient",               "getAccountPlayer" },
                { false, "setClientName",                  "setPlayerName" },
                // Utility
                { false, "randFloat",                      "math.random" },
                { false, "randInt",                        "math.random" },
                // Weapon
                { false, "giveWeaponAmmo",                 "giveWeapon" },
                { false, "takeWeaponAmmo",                 "takeWeapon" },

                // Admin
                { true, "banIP",                           "Please manually update this.  Refer to the wiki for details" },
                { true, "banSerial",                       "Please manually update this.  Refer to the wiki for details" },
                { true, "unbanIP",                         "Please manually update this.  Refer to the wiki for details" },
                { true, "unbanSerial",                     "Please manually update this.  Refer to the wiki for details" },
                { true, "getBansXML",                      "Please manually update this.  Refer to the wiki for details" },
                { true, "canPlayerUseFunction",            "Please manually update this.  Refer to the wiki for details" },
            };
}
