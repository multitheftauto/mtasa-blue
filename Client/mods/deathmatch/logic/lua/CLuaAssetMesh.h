/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaAssetMesh.h
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaAssetMesh;

#pragma once
class aiMesh;
class CClientAssetModel;
// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaAssetMesh
{
public:
    CLuaAssetMesh(CClientAssetModel* pAssetModel, const aiMesh* pMesh);
    ~CLuaAssetMesh();

    void RemoveScriptID();

    uint GetScriptID() const { return m_uiScriptID; }
    static CLuaAssetMesh* GetFromScriptID(unsigned int uiScriptID);
    void GetProperties(lua_State* luaVM, eAssetProperty assetProperty);

private:
    CClientAssetModel* m_pAssetModel;
    const aiMesh* m_pMesh;
    uint m_uiScriptID;
};
