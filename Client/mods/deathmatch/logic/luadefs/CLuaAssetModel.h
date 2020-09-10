/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaAssetModelDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaAssetModelDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(LoadAssetModel);

    static unsigned int GetAssetModelProperty(CClientAssetModel* pModel, eAssetProperty eProperty);
    static std::variant<unsigned int, float, std::tuple<CVector, CVector>, const char*, CVector, CLuaAssetNode*> GetAssetNodeProperty(CLuaAssetNode* pNode,
                                                                                                                                      eAssetProperty eProperty);
    static std::variant<unsigned int, std::tuple<CVector, CVector>,
                        std::tuple<CVector4D, CVector4D>> GetAssetMeshProperty(CLuaAssetMesh* pMesh, eAssetProperty eProperty);

    static std::variant<bool, std::vector<CLuaAssetMesh*>> AssetGetModelMeshes(CClientAssetModel* pAssetModel);
    static std::vector<CLuaAssetMesh*>                     AssetGetNodeMeshes(CLuaAssetNode* pAssetNode);


    LUA_DECLARE(AssetGetNodes);
    LUA_DECLARE(AssetGetTextures);
    LUA_DECLARE(AssetRender);
    LUA_DECLARE(AssetSetTexture);
    LUA_DECLARE(AssetGetMaterialProperties);
    LUA_DECLARE(AssetGetMetaData);
    LUA_DECLARE(AssetCreateInstance);
    LUA_DECLARE(AssetGetRenderGroupProperties);
    LUA_DECLARE(AssetSetRenderGroupProperties);
};
