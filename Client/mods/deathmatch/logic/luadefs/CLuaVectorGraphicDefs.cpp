/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaVectorGraphicDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"

void CLuaVectorGraphicDefs::LoadFunctions()
{
   constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"svgCreate", SVGCreate},
        {"svgAddRect", ArgumentParser<SVGAddRect>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaVectorGraphicDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "svgCreate");

    lua_registerclass(luaVM, "SVG");
}

int CLuaVectorGraphicDefs::SVGCreate(lua_State* luaVM)
{
    //  texture svgCreate ( int width, int height )
    CVector2D vecSize;
    SString   path;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecSize);
    argStream.ReadString(path, "");

    if (!argStream.HasErrors())
    {
        if (vecSize.fX < 0)
        {
            argStream.SetCustomError("Width is smaller than 0", "Invalid parameter");
        }
        else if (vecSize.fY < 0)
        {
            argStream.SetCustomError("Height is smaller than 0", "Invalid parameter");
        }
        else if (vecSize.fX == 0 || vecSize.fY == 0)
        {
            m_pScriptDebugging->LogWarning(luaVM, "A vector graphic must be at least 1x1 in size. This warning may be an error in future versions.");
        }
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pParentResource = pLuaMain->GetResource();

            CClientVectorGraphic* pVectorGraphic =
                g_pClientGame->GetManager()->GetRenderElementManager()->CreateVectorGraphic((int)vecSize.fX, (int)vecSize.fY);

            if (pVectorGraphic)
            {
                // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                pVectorGraphic->SetParent(pParentResource->GetResourceDynamicEntity());

                // Set our owner resource
                pVectorGraphic->SetResource(pParentResource);

                // Try and load from the filepath or svg data (if provided)
                std::string strPath;
                if (CResourceManager::ParseResourcePathInput(path, pParentResource, &strPath))
                {
                    if (FileExists(strPath))
                        pVectorGraphic->LoadFromFile(strPath);
                }
            }
            lua_pushelement(luaVM, pVectorGraphic);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

std::variant<bool, int, std::string> CLuaVectorGraphicDefs::SVGAddRect(CClientVectorGraphic* pVectorGraphic, std::variant<float, std::string> x, std::variant<float, std::string> y,
                                                        std::variant<float, std::string> width, std::variant<float, std::string> height, std::variant<float, std::string> rx,
                                                        std::variant<float, std::string> ry, float pathLength, std::string fill)
{
       return pVectorGraphic->AddRect(x, y, width, height, rx, ry, pathLength, fill);
}
