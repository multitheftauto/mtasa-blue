/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaFBXDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaFBXDefs : public CLuaDefs
{
public:
    static void LoadFunctions(void);
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(FBXLoadFile);
    LUA_DECLARE(FBXGetLoadingStatus);
    LUA_DECLARE(FBXGetAllObjects);
    LUA_DECLARE(FBXGetObjectProperties);
    LUA_DECLARE(FBXGetTemplateProperties);
    LUA_DECLARE(FBXSetTemplateProperties);
    LUA_DECLARE(FBXSetTemplateModelProperties);
    LUA_DECLARE(FBXGetTemplateModelProperties);
    LUA_DECLARE(FBXTemplateAddModel);
    LUA_DECLARE(FBXTemplateRemoveModel);
    LUA_DECLARE(FBXGetAllTemplates);
    LUA_DECLARE(FBXGetAllTemplateModels);
    LUA_DECLARE(FBXGetAllTextures);
    LUA_DECLARE(FBXGetObjectRawData);
    LUA_DECLARE(FBXTemplateExists);
    LUA_DECLARE(FBXAddTemplate);
    LUA_DECLARE(FBXRemoveTemplate);
    LUA_DECLARE(FBXApplyTemplateToModel);
    LUA_DECLARE(FBXRemoveTemplateFromModel);
    LUA_DECLARE(FBXApplyTemplateToElement);
    LUA_DECLARE(FBXRemoveTemplateFromElement);
    LUA_DECLARE(FBXRenderTemplate);
};
