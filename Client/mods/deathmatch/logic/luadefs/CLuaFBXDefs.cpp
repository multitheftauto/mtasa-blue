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

#include "StdInc.h"

void CLuaFBXDefs::LoadFunctions(void)
{
    std::map<const char*, lua_CFunction> functions{
        {"fbxLoadFile", FBXLoadFile},
        {"fbxGetAllMeshes", FBXGetAllMeshes},
        {"fbxGetMeshProperties", FBXGetMeshProperties},
        {"fbxGetMeshRawData", FBXGetMeshRawData},
        {"fbxDrawPreview", FBXDrawPreview},

    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

void CLuaFBXDefs::AddClass(lua_State* luaVM)
{
    /*
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "set", "FBXTestSet");
    lua_classfunction(luaVM, "get", "FBXTestGet");


    lua_registerstaticclass(luaVM, "FBX");*/
}

int CLuaFBXDefs::FBXLoadFile(lua_State* luaVM)
{
    SString          strFile = "";
    CScriptArgReader argStream(luaVM);

    argStream.ReadString(strFile);

    if (!argStream.HasErrors())
    {
        // Grab our virtual machine and grab our resource from that.
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Get this resource
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                bool    bIsRawData = CClientFBX::IsFBXData(strFile);
                SString strPath;
                // Is this a legal filepath?
                if (bIsRawData || CResourceManager::ParseResourcePathInput(strFile, pResource, &strPath))
                {
                    // Grab the resource root entity
                    CClientEntity* pRoot = pResource->GetResourceDFFRoot();

                    // Create a DFF element
                    CClientFBX* pFBX = new CClientFBX(m_pManager, INVALID_ELEMENT_ID);

                    // Try to load the DFF file
                    if (pFBX->LoadFBX(bIsRawData ? strFile : strPath, bIsRawData))
                    {
                        // Success loading the file. Set parent to DFF root
                        pFBX->SetParent(pRoot);

                        // Return the DFF
                        lua_pushelement(luaVM, pFBX);
                        return 1;
                    }
                    else
                    {
                        // Delete it again
                        delete pFBX;
                        argStream.SetCustomError(bIsRawData ? "raw data" : strFile, "Error loading FBX");
                    }
                }
                else
                    argStream.SetCustomError(bIsRawData ? "raw data" : strFile, "Bad file path");
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // We failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXGetAllMeshes(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    SString          strFilter;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadString(strFilter, "");

    if (!argStream.HasErrors())
    {
        lua_newtable(luaVM);
        pFBX->LuaGetMeshes(luaVM, strFilter);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXDrawPreview(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    SString          strHierarchyMesh;
    CVector          vecPosition;
    SColor           color;
    float            fWidth;
    bool             bPostGUI;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadString(strHierarchyMesh);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadColor(color, 0xFFFFFFFF);
    argStream.ReadNumber(fWidth, 1);
    argStream.ReadBool(bPostGUI, false);

    if (!argStream.HasErrors())
    {
        if (pFBX->IsMeshExists(strHierarchyMesh))
        {
            pFBX->DrawPreview(pFBX->GetMeshByName(strHierarchyMesh), vecPosition, color, fWidth, bPostGUI);
            lua_pushboolean(luaVM, true);
            return 1;
        }
        else
        {
            argStream.SetCustomError("Mesh in hierarchy not found");
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXGetMeshProperties(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    SString          strHierarchyMesh;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadString(strHierarchyMesh);

    if (!argStream.HasErrors())
    {
        if (pFBX->IsMeshExists(strHierarchyMesh))
        {
            if (pFBX->LuaGetMeshProperties(luaVM, pFBX->GetMeshByName(strHierarchyMesh)))
            {
                return 1;
            }
        }
        else
        {
            argStream.SetCustomError("Mesh in hierarchy not found");
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXGetMeshRawData(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    SString          strHierarchyMesh;
    eFBXDataType     eDataType;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadString(strHierarchyMesh);
    argStream.ReadEnumString(eDataType);

    if (!argStream.HasErrors())
    {
        if (pFBX->IsMeshExists(strHierarchyMesh))
        {
            int iStart, iStop;
            switch (eDataType)
            {
                case FBX_DATA_TYPE_VERTEX:

                    argStream.ReadNumber(iStart, 0);
                    argStream.ReadNumber(iStop, 0);
                    iStart--, iStop--;
                    if (!pFBX->LuaRawGetVertices(luaVM, pFBX->GetMeshByName(strHierarchyMesh), iStart, iStop))
                    {
                        argStream.SetCustomError("Invalid vertex range");
                    }
                    break;
                case FBX_DATA_TYPE_INDICATOR:

                    argStream.ReadNumber(iStart, 0);
                    argStream.ReadNumber(iStop, 0);
                    iStart--, iStop--;
                    if (!pFBX->LuaRawGetIndices(luaVM, pFBX->GetMeshByName(strHierarchyMesh), iStart, iStop))
                    {
                        argStream.SetCustomError("Invalid vertex range");
                    }
                    break;
                default:
                    argStream.SetCustomError("Not supported data type");
                    break;
            }
            return 1;
        }
        else
        {
            argStream.SetCustomError("Mesh in hierarchy not found");
        }
    }

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }
    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}
