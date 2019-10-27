/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPrimitiveBufferDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaPrimitiveBufferDefs::LoadFunctions()
{
    std::map<const char*, lua_CFunction> functions{
        {"primitiveBufferCreate", PrimitiveBufferCreate},
        {"primitiveBufferDraw", PrimitiveBufferDraw},
        {"primitiveBufferDraw3D", PrimitiveBufferDraw3D},
    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

void CLuaPrimitiveBufferDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "Create", "primitiveBufferCreate");
    lua_classfunction(luaVM, "Draw", "primitiveBufferDraw");
    lua_classfunction(luaVM, "Draw3D", "primitiveBufferDraw");
    // lua_classvariable(luaVM, "someVariable", nullptr, "functionName");

    lua_registerclass(luaVM, "PrimitiveBuffer", "Element");
}

int CLuaPrimitiveBufferDefs::PrimitiveBufferDraw(lua_State* luaVM)
{
    CClientPrimitiveBuffer* pPrimitiveBuffer;
    CVector                 vecPosition;
    CVector                 vecRotation;
    CVector                 vecScale;
    bool                    bPostGui;
    ePrimitiveView          primitiveView;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPrimitiveBuffer);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecRotation, CVector(0, 0, 0));
    argStream.ReadVector3D(vecScale, CVector(1, 1, 1));
    argStream.ReadBool(bPostGui, false);
    argStream.ReadEnumString(primitiveView, (ePrimitiveView)0);

    if (!argStream.HasErrors())
    {
        ConvertDegreesToRadians(vecRotation);

        PrimitiveBufferSettings bufferSettings;
        bufferSettings.matrix.SetPosition(vecPosition);
        bufferSettings.matrix.SetRotation(vecRotation);
        bufferSettings.matrix.SetScale(vecScale);
        bufferSettings.eView = primitiveView;

        g_pCore->GetGraphics()->DrawPrimitiveBufferQueued(reinterpret_cast<CClientPrimitiveBufferInterface*>(pPrimitiveBuffer), bufferSettings, bPostGui);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPrimitiveBufferDefs::PrimitiveBufferDraw3D(lua_State* luaVM)
{
    CClientPrimitiveBuffer* pPrimitiveBuffer;
    CVector                 vecPosition;
    CVector                 vecRotation;
    CVector                 vecScale;
    bool                    bPostGui;
    ePrimitiveView          primitiveView;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPrimitiveBuffer);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecRotation, CVector(0, 0, 0));
    argStream.ReadVector3D(vecScale, CVector(1, 1, 1));
    argStream.ReadBool(bPostGui, false);
    argStream.ReadEnumString(primitiveView, (ePrimitiveView)0);

    if (!argStream.HasErrors())
    {
        ConvertDegreesToRadians(vecRotation);

        PrimitiveBufferSettings bufferSettings;
        bufferSettings.matrix.SetPosition(vecPosition);
        bufferSettings.matrix.SetRotation(vecRotation);
        bufferSettings.matrix.SetScale(vecScale);
        bufferSettings.eView = primitiveView;

        g_pCore->GetGraphics()->DrawPrimitiveBuffer3DQueued(reinterpret_cast<CClientPrimitiveBufferInterface*>(pPrimitiveBuffer), bufferSettings, bPostGui);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPrimitiveBufferDefs::PrimitiveBufferCreate(lua_State* luaVM)
{
    CVector2D                       vecPosition;
    ePrimitiveFormat                primitiveFormat;
    std::vector<VertexXYZUVDiffuse> vecXYZUVDiffuse;
    std::vector<VertexXYZDiffuse>   vecXYZDiffuse;
    std::vector<VertexXYZUV>        vecXYZUV;
    std::vector<VertexXYZ>          vecXYZ;
    std::vector<float>              vecTableContent;
    std::vector<int>                vecIndexList;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(primitiveFormat);

    bool bBreak = false;
    while (argStream.NextIsTable())
    {
        vecIndexList.emplace_back(vecIndexList.size());            // replacment of i++
        bBreak = false;
        vecTableContent.clear();
        argStream.ReadNumberTable(vecTableContent);

        // table size due future compatibility such a xyz specular what is size == 4 as well
        switch (primitiveFormat)
        {
            case PRIMITIVE_FORMAT_XYZ:
                if (vecTableContent.size() == 3)
                    vecXYZ.push_back(VertexXYZ{CVector(vecTableContent[0], vecTableContent[1], vecTableContent[2])});
                else
                {
                    bBreak = true;
                    argStream.SetCustomError(SString("Primitive format xyz require 3 numbers, got %i numbers.", vecTableContent.size()).c_str());
                }
                break;
            case PRIMITIVE_FORMAT_XYZ_UV:
                if (vecTableContent.size() == 5)
                    vecXYZUV.push_back(
                        VertexXYZUV{CVector(vecTableContent[0], vecTableContent[1], vecTableContent[2]), vecTableContent[3], vecTableContent[4]});
                else
                {
                    bBreak = true;
                    argStream.SetCustomError(SString("Primitive format xyz, uv, diffuse require 5 numbers, got %i numbers.", vecTableContent.size()).c_str());
                }
                break;
            case PRIMITIVE_FORMAT_XYZ_DIFFUSE:
                if (vecTableContent.size() == 4)
                    vecXYZDiffuse.push_back(VertexXYZDiffuse{CVector(vecTableContent[0], vecTableContent[1], vecTableContent[2]),
                                                             static_cast<unsigned long>(static_cast<int64_t>(vecTableContent[3]))});
                else
                {
                    bBreak = true;
                    argStream.SetCustomError(SString("Primitive format xyz, uv, diffuse require 6 numbers, got %i numbers.", vecTableContent.size()).c_str());
                }
                break;
            case PRIMITIVE_FORMAT_XYZ_DIFFUSE_UV:
                if (vecTableContent.size() == 6)
                    vecXYZUVDiffuse.push_back(VertexXYZUVDiffuse{CVector(vecTableContent[0], vecTableContent[1], vecTableContent[2]), vecTableContent[3],
                                                                 vecTableContent[4], static_cast<unsigned long>(static_cast<int64_t>(vecTableContent[5]))});
                else
                {
                    bBreak = true;
                    argStream.SetCustomError(SString("Primitive format xyz, uv, diffuse require 6 numbers, got %i numbers.", vecTableContent.size()).c_str());
                }
                break;
        }
        if (bBreak)
            break;
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CClientPrimitiveBuffer* pBuffer = CStaticFunctionDefinitions::CreatePrimitiveBuffer(*pResource);
                if (pBuffer)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pBuffer);
                    }

                    switch (primitiveFormat)
                    {
                        case PRIMITIVE_FORMAT_XYZ:
                            pBuffer->CreateBuffer(vecXYZ, vecIndexList);
                            break;
                        case PRIMITIVE_FORMAT_XYZ_UV:
                            pBuffer->CreateBuffer(vecXYZUV, vecIndexList);
                            break;
                        case PRIMITIVE_FORMAT_XYZ_DIFFUSE:
                            pBuffer->CreateBuffer(vecXYZDiffuse, vecIndexList);
                            break;
                        case PRIMITIVE_FORMAT_XYZ_DIFFUSE_UV:
                            pBuffer->CreateBuffer(vecXYZUVDiffuse, vecIndexList);
                            break;
                    }
                    lua_pushelement(luaVM, pBuffer);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
