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
    // bool primitiveBufferDraw( primitive-buffer buffer, vector3 position, vector3 rotation, vector3 scale, bool postGui, primitive-view primitiveView, mixed
    // material)
    CClientPrimitiveBuffer* pPrimitiveBuffer;
    CVector                 vecPosition;
    CVector                 vecRotation;
    CVector                 vecScale;
    bool                    bPostGui;
    ePrimitiveView          primitiveView;
    CClientMaterial*        pMaterialElement = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPrimitiveBuffer);
    if (!argStream.HasErrors())
    {
        if (pPrimitiveBuffer->IsRequireMaterial())
            MixedReadMaterialString(argStream, pMaterialElement);
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
            if (pMaterialElement)
                bufferSettings.pMaterial = pMaterialElement->GetMaterialItem();
            else
                bufferSettings.pMaterial = nullptr;

            g_pCore->GetGraphics()->DrawPrimitiveBufferQueued(reinterpret_cast<CClientPrimitiveBufferInterface*>(pPrimitiveBuffer), bufferSettings, bPostGui);

            lua_pushboolean(luaVM, true);
            return 1;
        }
        if (argStream.HasErrors())
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPrimitiveBufferDefs::PrimitiveBufferDraw3D(lua_State* luaVM)
{
    // bool primitiveBufferDraw3D( primitive-buffer buffer, vector3 position, vector3 rotation, vector3 scale, bool postGui, primitive-view primitiveView, mixed
    // material)

    CClientPrimitiveBuffer* pPrimitiveBuffer;
    CVector                 vecPosition;
    CVector                 vecRotation;
    CVector                 vecScale;
    bool                    bPostGui;
    ePrimitiveView          primitiveView;
    CClientMaterial*        pMaterialElement = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPrimitiveBuffer);
    if (!argStream.HasErrors())
    {
        if (pPrimitiveBuffer->IsRequireMaterial())
            MixedReadMaterialString(argStream, pMaterialElement);
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
            if (pMaterialElement)
                bufferSettings.pMaterial = pMaterialElement->GetMaterialItem();
            else
                bufferSettings.pMaterial = nullptr;

            g_pCore->GetGraphics()->DrawPrimitiveBuffer3DQueued(reinterpret_cast<CClientPrimitiveBufferInterface*>(pPrimitiveBuffer), bufferSettings, bPostGui);

            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPrimitiveBufferDefs::PrimitiveBufferCreate(lua_State* luaVM)
{
    // primitive-buffer primitiveBufferCreate( ePrimitiveFormat primitiveFormat, ePrimitiveType primitiveType, [table indices, ] table vertex1, table vertex2,
    // table vertex3
    std::vector<float>          vecTableContent;
    std::vector<ePrimitiveData> primitiveDataList;
    D3DPRIMITIVETYPE            ePrimitiveType;
    CScriptArgReader            argStream(luaVM);
    argStream.ReadEnumStringList(primitiveDataList, 0);
    argStream.ReadEnumString(ePrimitiveType);

    int iPrimitiveDataMask = 0;

    std::sort(primitiveDataList.begin(), primitiveDataList.end());
    primitiveDataList.erase(unique(primitiveDataList.begin(), primitiveDataList.end()), primitiveDataList.end());

    for (ePrimitiveData primitiveData : primitiveDataList)
        iPrimitiveDataMask |= primitiveData;

    if (iPrimitiveDataMask == 0)
    {
        m_pScriptDebugging->LogCustom(luaVM, "Primitive data declaration is empty or invalid.");
        lua_pushboolean(luaVM, false);
        return 1;
    }

    bool containXY = (iPrimitiveDataMask & PRIMITIVE_DATA_XY) == PRIMITIVE_DATA_XY;
    bool containXYZ = (iPrimitiveDataMask & PRIMITIVE_DATA_XYZ) == PRIMITIVE_DATA_XYZ;
    if (!containXY && !containXYZ)
    {
        m_pScriptDebugging->LogCustom(luaVM, "Primitive data must contain `xy` or `xyz` data declaration.");
        lua_pushboolean(luaVM, false);
        return 1;
    }

    if (containXY && containXYZ)
    {
        m_pScriptDebugging->LogCustom(luaVM, "You can not use `xy` and `xyz` data declaration at once.");
        lua_pushboolean(luaVM, false);
        return 1;
    }

    bool contain16BitIndices = (iPrimitiveDataMask & PRIMITIVE_DATA_INDICES16) == PRIMITIVE_DATA_INDICES16;
    bool contain32BitIndices = (iPrimitiveDataMask & PRIMITIVE_DATA_INDICES32) == PRIMITIVE_DATA_INDICES32;
    if (contain16BitIndices && contain32BitIndices)
    {
        m_pScriptDebugging->LogCustom(luaVM, "You can not use `16bitindices` and `32bitindices` data declaration at once.");
        lua_pushboolean(luaVM, false);
        return 1;
    }

    std::vector<unsigned short> vec16BitIndexList;
    std::vector<int>            vec32BitIndexList;

    if ((iPrimitiveDataMask & PRIMITIVE_DATA_INDICES16) == PRIMITIVE_DATA_INDICES16)
    {
        if (argStream.NextIsTable())
        {
            argStream.ReadNumberTable(vec16BitIndexList);

            if (vec16BitIndexList.size() > 0xffff)
            {
                m_pScriptDebugging->LogCustom(luaVM, "You can not pass more than 65536 indices in 16 bit version, use 32 bit instead.");
                lua_pushboolean(luaVM, false);
                return 1;
            }
            if (vec16BitIndexList.size() > 0 && vec16BitIndexList.size() % 3 == 0)
            {
                for (unsigned short index : vec16BitIndexList)
                {
                    if (index <= 0)
                    {
                        m_pScriptDebugging->LogCustom(luaVM, "Indices table can not contain negative numbers.");
                        lua_pushboolean(luaVM, false);
                        return 1;
                    }
                }
                std::transform(std::begin(vec16BitIndexList), std::end(vec16BitIndexList), std::begin(vec16BitIndexList),
                               [](unsigned short x) { return x - 1; });
            }
            else
            {
                m_pScriptDebugging->LogCustom(luaVM, "Indices table must be divisible by 3 and contain at least 3 non negative numbers.");
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }
    }
    if ((iPrimitiveDataMask & PRIMITIVE_DATA_INDICES32) == PRIMITIVE_DATA_INDICES32)
    {
        if (argStream.NextIsTable())
        {
            argStream.ReadNumberTable(vec32BitIndexList);

            if (vec32BitIndexList.size() > 0 && vec32BitIndexList.size() % 3 == 0)
            {
                for (int index : vec32BitIndexList)
                {
                    if (index <= 0)
                    {
                        m_pScriptDebugging->LogCustom(luaVM, "Indices table can not contain negative numbers.");
                        lua_pushboolean(luaVM, false);
                        return 1;
                    }
                }
                std::transform(std::begin(vec32BitIndexList), std::end(vec32BitIndexList), std::begin(vec32BitIndexList), [](int x) { return x - 1; });
            }
            else
            {
                m_pScriptDebugging->LogCustom(luaVM, "Indices table must be divisible by 3 and contain at least 3 non negative numbers.");
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }
    }

    std::vector<CVector>   vecXYZ;
    std::vector<CVector2D> vecUV;
    std::vector<D3DCOLOR>  vecDiffuse;

    int iTableSize = 0;
    for (ePrimitiveData primitiveData : primitiveDataList)
        if (primitiveDataSizeMap.find(primitiveData) != primitiveDataSizeMap.end())
            iTableSize += primitiveDataSizeMap[primitiveData];

    int iTableOffset = 0;
    int iVertexIndex = 0;

    while (argStream.NextIsTable())
    {
        iVertexIndex++;
        iTableOffset = 0;
        vecTableContent.clear();
        argStream.ReadNumberTable(vecTableContent);
        if (vecTableContent.size() != iTableSize)
        {
            m_pScriptDebugging->LogCustom(
                luaVM,
                SString("Primitive vertex require %i number, got %i numbers, vertex index %i.", iTableSize, vecTableContent.size(), iVertexIndex).c_str());
            lua_pushboolean(luaVM, false);
            return 1;
        }

        if ((iPrimitiveDataMask & PRIMITIVE_DATA_XY) == PRIMITIVE_DATA_XY)
        {
            vecXYZ.emplace_back(vecTableContent[iTableOffset], vecTableContent[iTableOffset + 1], 0);
            iTableOffset += primitiveDataSizeMap[PRIMITIVE_DATA_XY];
        }

        if ((iPrimitiveDataMask & PRIMITIVE_DATA_XYZ) == PRIMITIVE_DATA_XYZ)
        {
            vecXYZ.emplace_back(vecTableContent[iTableOffset], vecTableContent[iTableOffset + 1], vecTableContent[iTableOffset + 2]);
            iTableOffset += primitiveDataSizeMap[PRIMITIVE_DATA_XYZ];
        }

        if ((iPrimitiveDataMask & PRIMITIVE_DATA_DIFFUSE) == PRIMITIVE_DATA_DIFFUSE)
        {
            vecDiffuse.emplace_back(static_cast<unsigned long>(static_cast<int64_t>(vecTableContent[iTableOffset])));
            iTableOffset += primitiveDataSizeMap[PRIMITIVE_DATA_DIFFUSE];
        }

        if ((iPrimitiveDataMask & PRIMITIVE_DATA_UV) == PRIMITIVE_DATA_UV)
        {
            vecUV.emplace_back(vecTableContent[iTableOffset], vecTableContent[iTableOffset + 1]);
            iTableOffset += primitiveDataSizeMap[PRIMITIVE_DATA_UV];
        }
    }

    if (vec32BitIndexList.size() > 0)
    {
        int i = 0;
        for (auto& index : vec32BitIndexList)
        {
            i++;
            if (index > vecXYZ.size())
            {
                m_pScriptDebugging->LogCustom(luaVM, SString("Indices table contains index %i out of range at %i table index.", index, i).c_str());
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }
    }
    if (vec16BitIndexList.size() > 0)
    {
        int i = 0;
        for (auto& index : vec16BitIndexList)
        {
            i++;
            if (index > vecXYZ.size())
            {
                m_pScriptDebugging->LogCustom(luaVM, SString("Indices table contains index %i out of range at %i table index.", index, i).c_str());
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }
    }

    if (contain16BitIndices || contain32BitIndices)
    {
        if (vecXYZ.size() < 1)
        {
            m_pScriptDebugging->LogCustom(luaVM, SString("You must add at least 1 vertex while using indices.", vecXYZ.size()).c_str());
            lua_pushboolean(luaVM, false);
            return 1;
        }
    }
    else if (!g_pCore->GetGraphics()->IsValidPrimitiveSize(vecXYZ.size(), ePrimitiveType))
    {
        m_pScriptDebugging->LogCustom(luaVM, SString("Vertices amount %i is invalid selected vertices type", vecXYZ.size()).c_str());
        lua_pushboolean(luaVM, false);
        return 1;
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

                    pBuffer->SetPrimitiveType(ePrimitiveType);

                    int FVF = D3DFVF_XYZ;
                    pBuffer->AddVertexBuffer(vecXYZ, PRIMITIVE_DATA_XYZ);

                    if (vecUV.size() > 0)
                    {
                        FVF |= D3DFVF_TEX1;
                        pBuffer->SetRequireMaterial(true);
                        pBuffer->AddVertexBuffer(vecUV, PRIMITIVE_DATA_UV);
                    }
                    if (vecDiffuse.size() > 0)
                    {
                        FVF |= D3DFVF_DIFFUSE;
                        pBuffer->AddVertexBuffer(vecDiffuse, PRIMITIVE_DATA_DIFFUSE);
                    }

                    if (vec16BitIndexList.size() > 0)
                    {
                        pBuffer->CreateIndexBuffer(vec16BitIndexList);
                    }
                    else if (vec32BitIndexList.size() > 0)
                    {
                        pBuffer->CreateIndexBuffer(vec32BitIndexList);
                    }

                    pBuffer->SetFVF(FVF);
                    pBuffer->Finalize();

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
