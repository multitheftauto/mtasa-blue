/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaEffekseerDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaEffekseerDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"loadEffekseerFx", ArgumentParser<loadEffekseerFx>},
        {"playEffekseerFx", ArgumentParser<playEffekseerFx>},
        {"setEffekseerFxSpeed", ArgumentParser<setEffekseerFxSpeed>},
        {"getEffekseerFxSpeed", ArgumentParser<getEffekseerFxSpeed>},
        {"setEffekseerFxScale", ArgumentParser<setEffekseerFxScale>},
        {"setEffekseedFxInput", ArgumentParser<setEffekseedFxInput>},
        {"getEffekseedFxInput", ArgumentParser<getEffekseedFxInput>},
    };
    
    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

CClientEffekseerEffect* CLuaEffekseerDefs::loadEffekseerFx(lua_State* const luaVM, std::string strRelativeFilePath)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CResource* pResource = pLuaMain->GetResource();
        if (!pResource)
            return false;

        std::string strFullPath;

        if (!CResourceManager::ParseResourcePathInput(strRelativeFilePath, pResource, &strFullPath))
            return false;

        // Grab the resource root entity
        CClientEntity* pRoot = pResource->GetResourceEffekseerRoot();

        CClientEffekseerEffect* pEffect = new CClientEffekseerEffect(m_pManager, INVALID_ELEMENT_ID);

        if (pEffect->Load(strFullPath))
        {
            // Success loading the file. Set parent to effekseer root
            pEffect->SetParent(pRoot);

            return pEffect;
        }

        delete pEffect;
    }
    throw std::invalid_argument("Error loading FX");
}

CClientEffekseerEffectHandler* CLuaEffekseerDefs::playEffekseerFx(CClientEffekseerEffect* pEffect, CVector vecPos)
{
    return pEffect->Play(vecPos);
}

bool CLuaEffekseerDefs::setEffekseerFxSpeed(CClientEffekseerEffectHandler* pHandler, float fSpeed)
{
    pHandler->SetSpeed(fSpeed);
    return true;
}

float CLuaEffekseerDefs::getEffekseerFxSpeed(CClientEffekseerEffectHandler* pHandler)
{
    return pHandler->GetSpeed();
}

bool CLuaEffekseerDefs::setEffekseerFxScale(CClientEffekseerEffectHandler* pHandler, CVector vecScale)
{
    pHandler->SetScale(vecScale);
    return true;
}

bool CLuaEffekseerDefs::setEffekseedFxInput(CClientEffekseerEffectHandler* pHandler, int32_t index, float fValue)
{
    pHandler->SetDynamicInput(index, fValue);
    return true;
}

float CLuaEffekseerDefs::getEffekseedFxInput(CClientEffekseerEffectHandler* pHandler, int32_t index)
{
    return pHandler->GetDynamicInput(index);
}
