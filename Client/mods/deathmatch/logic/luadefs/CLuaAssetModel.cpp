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

#include "StdInc.h"

void CLuaAssetModelDefs::LoadFunctions()
{
    std::map<const char*, lua_CFunction> functions{
        {"loadAssetModel", LoadAssetModel},
    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

void CLuaAssetModelDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    //lua_classfunction(luaVM, "create", "setTimer");
    //lua_classfunction(luaVM, "destroy", "killTimer");
    //lua_classfunction(luaVM, "reset", "resetTimer");
    //lua_classfunction(luaVM, "isValid", "isTimer");

    //lua_classfunction(luaVM, "getDetails", "getTimerDetails");

    //lua_classvariable(luaVM, "valid", NULL, "isTimer");

    lua_registerclass(luaVM, "AssetModel");
}

const aiScene* loadModel(string path)
{
    Assimp::Importer import;

    const aiScene*   scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ValidateDataStructure);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        return nullptr;
    }
    return scene;
}

int CLuaAssetModelDefs::LoadAssetModel(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        SString strFileInput;
        bool    bReadOnly;

        CScriptArgReader argStream(luaVM);
        argStream.ReadString(strFileInput);

        if (!argStream.HasErrors())
        {
            SString    strPath;
            CResource* pThisResource = pLuaMain->GetResource();
            CResource* pOtherResource = pThisResource;

            // Resolve other resource from name
            if (CResourceManager::ParseResourcePathInput(strFileInput, pOtherResource, &strPath))
            {
                CResource* pResource = pLuaMain ? pLuaMain->GetResource() : nullptr;

                if (pResource)
                {
                    // const aiScene* scene = loadModel(strPath);
                    auto pAssetModel = CStaticFunctionDefinitions::CreateAssetModel(*pResource);
                    if (pAssetModel)
                    {
                        CElementGroup* pGroup = pResource->GetElementGroup();
                        if (pGroup)
                            pGroup->Add(pAssetModel);

                        lua_pushelement(luaVM, pAssetModel);
                        return 1;
                    }
                }
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
//
//int CLuaAssetModelDefs::LoadAssetModel(lua_State* luaVM)
//{
//    //  timer setTimer ( function theFunction, int timeInterval, int timesToExecute, [ var arguments... ] )
//    CLuaFunctionRef iLuaFunction;
//    double          dTimeInterval;
//    uint            uiTimesToExecute;
//    CLuaArguments   Arguments;
//
//    CScriptArgReader argStream(luaVM);
//    argStream.ReadFunction(iLuaFunction);
//    argStream.ReadNumber(dTimeInterval);
//    argStream.ReadNumber(uiTimesToExecute);
//    argStream.ReadLuaArguments(Arguments);
//    argStream.ReadFunctionComplete();
//
//    if (!argStream.HasErrors())
//    {
//        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
//        if (luaMain)
//        {
//            // Check for the minimum interval
//            if (dTimeInterval < LUA_TIMER_MIN_INTERVAL)
//            {
//                argStream.SetCustomError("Interval is below " MTA_STR(LUA_TIMER_MIN_INTERVAL));
//            }
//            else
//            {
//                CLuaTimer* pLuaTimer = luaMain->GetTimerManager()->AddTimer(iLuaFunction, CTickCount(dTimeInterval), uiTimesToExecute, Arguments);
//                if (pLuaTimer)
//                {
//                    // Set our timer debug info (in case we don't have any debug info which is usually when you do setTimer(destroyElement, 50, 1) or such)
//                    pLuaTimer->SetLuaDebugInfo(g_pClientGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
//
//                    lua_pushtimer(luaVM, pLuaTimer);
//                    return 1;
//                }
//            }
//        }
//    }
//    if (argStream.HasErrors())
//        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
//
//    lua_pushboolean(luaVM, false);
//    return 1;
//}
