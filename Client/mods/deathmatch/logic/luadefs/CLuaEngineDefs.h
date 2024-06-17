/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaEngineDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <lua/CLuaMultiReturn.h>

class CLuaEngineDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(EngineLoadDFF);
    LUA_DECLARE(EngineLoadTXD);
    LUA_DECLARE(EngineLoadCOL);
    LUA_DECLARE(EngineLoadIFP);
    LUA_DECLARE(EngineImportTXD);
    LUA_DECLARE(EngineReplaceCOL);
    LUA_DECLARE(EngineRestoreCOL);
    LUA_DECLARE(EngineReplaceModel);
    LUA_DECLARE(EngineRestoreModel);
    LUA_DECLARE(EngineRequestModel);
    LUA_DECLARE(EngineFreeModel);
    LUA_DECLARE(EngineReplaceAnimation);
    LUA_DECLARE(EngineRestoreAnimation);
    LUA_DECLARE(EngineReplaceMatchingAtomics);
    LUA_DECLARE(EngineReplaceWheelAtomics);
    LUA_DECLARE(EnginePositionAtomic);
    LUA_DECLARE(EnginePositionSeats);
    LUA_DECLARE(EngineAddAllAtomics);
    LUA_DECLARE(EngineReplaceVehiclePart);
    LUA_DECLARE(EngineGetModelLODDistance);
    LUA_DECLARE(EngineSetModelLODDistance);
    LUA_DECLARE(EngineResetModelLODDistance);
    LUA_DECLARE(EngineSetAsynchronousLoading);
    LUA_DECLARE(EngineApplyShaderToWorldTexture);
    LUA_DECLARE(EngineRemoveShaderFromWorldTexture);
    LUA_DECLARE(EngineGetModelNameFromID);
    LUA_DECLARE(EngineGetModelIDFromName);
    LUA_DECLARE(EngineGetModelTextureNames);
    LUA_DECLARE(EngineGetVisibleTextureNames);
    LUA_DECLARE(EngineGetModelTextures);
    LUA_DECLARE(EngineSetSurfaceProperties);
    LUA_DECLARE(EngineGetSurfaceProperties);
    LUA_DECLARE(EngineResetSurfaceProperties);
    LUA_DECLARE(EngineGetModelPhysicalPropertiesGroup)
    LUA_DECLARE(EngineSetModelPhysicalPropertiesGroup)
    LUA_DECLARE(EngineRestoreModelPhysicalPropertiesGroup)
    LUA_DECLARE(EngineSetObjectGroupPhysicalProperty)
    LUA_DECLARE(EngineGetObjectGroupPhysicalProperty)
    LUA_DECLARE(EngineRestoreObjectGroupPhysicalProperties)
    static uint                                            EngineGetModelFlags(uint uiModelID);
    static bool                                            EngineSetModelFlags(uint uiModelID, uint uiFlags, std::optional<bool> bIdeFlags);
    static bool                                            EngineGetModelFlag(uint uiModelID, eModelIdeFlag eFlag);
    static bool                                            EngineSetModelFlag(uint uiModelID, eModelIdeFlag eFlag, bool state);
    static bool                                            EngineResetModelFlags(uint uiModelID);
    static uint                                            EngineGetModelTXDID(uint uiDffModelID);
    static bool                                            EngineSetModelTXDID(uint uiDffModelID, unsigned short uiTxdId);
    static bool                                            EngineResetModelTXDID(uint uiDffModelID);
    static CClientIMG*                                     EngineLoadIMG(lua_State* const luaVM, std::string strFilePath);
    static bool                                            EngineAddImage(CClientIMG* pImg);
    static bool                                            EngineRemoveImage(CClientIMG* pImg);
    static uint                                            EngineImageGetFilesCount(CClientIMG* pImg);
    static bool                                            EngineImageLinkDFF(CClientIMG* pImg, std::variant<size_t, std::string_view> file, uint uiModelID);
    static bool                                            EngineImageLinkTXD(CClientIMG* pImg, std::variant<size_t, std::string_view> file, uint uiModelID);
    static bool                                            EngineRestoreDFFImage(uint uiModelID);
    static bool                                            EngineRestoreTXDImage(uint uiModelID);
    static std::vector<std::string_view>                   EngineImageGetFileList(CClientIMG* pImg);
    static std::string                                     EngineImageGetFile(CClientIMG* pImg, std::variant<size_t, std::string_view> file);
    static bool                                            EngineRestreamWorld(lua_State* const luaVM);
    static bool                                            EngineSetModelVisibleTime(std::string strModelId, char cHourOn, char cHourOff);
    static std::variant<bool, CLuaMultiReturn<char, char>> EngineGetModelVisibleTime(std::string strModelId);

    static size_t EngineGetPoolCapacity(ePools pool);
    static size_t EngineGetPoolDefaultCapacity(ePools pool);
    static size_t EngineGetPoolUsedCapacity(ePools pool);
    static bool   EngineSetPoolCapacity(lua_State* luaVM, ePools pool, size_t newSize);

    static uint EngineRequestTXD(lua_State* const luaVM, std::string strTxdName);
    static bool EngineFreeTXD(uint txdID);

private:
    static void AddEngineColClass(lua_State* luaVM);
    static void AddEngineTxdClass(lua_State* luaVM);
    static void AddEngineDffClass(lua_State* luaVM);
    static void AddEngineImgClass(lua_State* luaVM);
};
