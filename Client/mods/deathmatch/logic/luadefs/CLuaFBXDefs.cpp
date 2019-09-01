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
        {"fbxGetAllObjects", FBXGetAllObjects},
        {"fbxGetAllTemplates", FBXGetAllTemplates},
        {"fbxGetAllTemplateModels", FBXGetAllTemplateModels},
        {"fbxGetObjectProperties", FBXGetObjectProperties},
        {"fbxGetTemplateProperties", FBXGetTemplateProperties},
        {"fbxSetTemplateProperties", FBXSetTemplateProperties},
        {"fbxGetTemplateModelProperties", FBXGetTemplateModelProperties},
        {"fbxSetTemplateModelProperties", FBXSetTemplateModelProperties},
        {"fbxAddTemplate", FBXAddTemplate},
        {"fbxRemoveTemplate", FBXRemoveTemplate},
        {"fbxTemplateAddModel", FBXTemplateAddModel},
        {"fbxTemplateRemoveModel", FBXTemplateRemoveModel},
        {"fbxApplyTemplateToModel", FBXApplyTemplateToModel},
        {"fbxRemoveTemplateFromModel", FBXRemoveTemplateFromModel},
        {"fbxApplyTemplateToElement", FBXApplyTemplateToElement},
        {"fbxRemoveTemplateFromElement", FBXRemoveTemplateFromElement},
        {"fbxRenderTemplate", FBXRenderTemplate},

        {"fbxGetAllMeshes", FBXGetAllMeshes},
        {"fbxGetAllTextures", FBXGetAllTextures},
        {"fbxGetAllMaterials", FBXGetAllMaterials},
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

int CLuaFBXDefs::FBXGetAllObjects(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    SString          strFilter;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);

    if (!argStream.HasErrors())
    {
        lua_newtable(luaVM);
        pFBX->LuaGetAllObjectsIds(luaVM);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXGetAllTemplates(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    SString          strFilter;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);

    if (!argStream.HasErrors())
    {
        pFBX->LuaGetAllTemplates(luaVM);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXGetAllTemplateModels(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    SString          strFilter;
    unsigned int     uiTemplateId;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiTemplateId);

    if (!argStream.HasErrors())
    {
        if (pFBX->LuaGetAllTemplateModels(luaVM, uiTemplateId))
            return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
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
        pFBX->LuaGetMeshes(luaVM);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXGetAllTextures(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);

    if (!argStream.HasErrors())
    {
        lua_newtable(luaVM);
        pFBX->LuaGetTextures(luaVM);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXGetAllMaterials(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);

    if (!argStream.HasErrors())
    {
        lua_newtable(luaVM);
        pFBX->LuaGetMaterials(luaVM);
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
    CClientFBX* pFBX;
    SString     strHierarchyMesh;
    CVector     vecPosition;
    SColor      color;
    float       fWidth;
    bool        bPostGUI;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadString(strHierarchyMesh);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadColor(color, 0xFFFFFFFF);
    argStream.ReadNumber(fWidth, 1);
    argStream.ReadBool(bPostGUI, false);

    if (!argStream.HasErrors())
    {
        if (pFBX->IsMeshValid(strHierarchyMesh))
        {
            // pFBX->Draw(pFBX->GetMeshByName(strHierarchyMesh), vecPosition);
            // pFBX->DrawPreview(pFBX->GetMeshByName(strHierarchyMesh), vecPosition, color, fWidth, bPostGUI);
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

int CLuaFBXDefs::FBXGetObjectProperties(lua_State* luaVM)
{
    CClientFBX*        pFBX;
    unsigned long long ulId;
    eFBXObjectProperty eProperty;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(ulId);
    argStream.ReadEnumString(eProperty);

    if (!argStream.HasErrors())
    {
        if (pFBX->IsObjectValid(ulId))
        {
            if (pFBX->LuaGetObjectProperties(luaVM, pFBX->GetObjectById(ulId), eProperty))
            {
                return 1;
            }
        }
        else
        {
            argStream.SetCustomError("Object id not found");
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXGetTemplateProperties(lua_State* luaVM)
{
    CClientFBX*          pFBX;
    unsigned int         uiId;
    eFBXTemplateProperty eProperty;
    CScriptArgReader     argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiId);
    argStream.ReadEnumString(eProperty);

    if (!argStream.HasErrors())
    {
        if (pFBX->LuaGetTemplateProperties(luaVM, uiId, eProperty))
        {
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXSetTemplateProperties(lua_State* luaVM)
{
    CClientFBX*          pFBX;
    unsigned int         uiId;
    eFBXTemplateProperty eProperty;
    CScriptArgReader     argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiId);
    argStream.ReadEnumString(eProperty);

    if (!argStream.HasErrors())
    {
        if (pFBX->LuaSetTemplateProperties(luaVM, argStream, uiId, eProperty))
        {
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXGetTemplateModelProperties(lua_State* luaVM)
{
    CClientFBX*               pFBX;
    unsigned int              uiId;
    unsigned int              uiModelId;
    eFBXTemplateModelProperty eProperty;
    CScriptArgReader          argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiId);
    argStream.ReadNumber(uiModelId);
    argStream.ReadEnumString(eProperty);

    if (!argStream.HasErrors())
    {
        if (pFBX->LuaGetTemplateModelProperties(luaVM, uiId, uiModelId, eProperty))
        {
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXSetTemplateModelProperties(lua_State* luaVM)
{
    CClientFBX*               pFBX;
    unsigned int              uiId;
    unsigned int              uiModelId;
    eFBXTemplateModelProperty eProperty;
    CScriptArgReader          argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiId);
    argStream.ReadNumber(uiModelId);
    argStream.ReadEnumString(eProperty);

    if (!argStream.HasErrors())
    {
        if (pFBX->LuaSetTemplateModelProperties(luaVM, argStream, uiId, uiModelId, eProperty))
        {
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXTemplateAddModel(lua_State* luaVM)
{
    CClientFBX*        pFBX;
    unsigned int       uiId;
    unsigned long long ullMesh;
    unsigned long long ullParentMesh;
    CVector            vecPosition;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiId);
    argStream.ReadNumber(ullMesh);
    argStream.ReadNumber(ullParentMesh, 0);
    argStream.ReadVector3D(vecPosition, CVector(0, 0, 0));

    if (!argStream.HasErrors())
    {
        unsigned int uiObjectId;
        if (pFBX->AddMeshToTemplate(luaVM, uiId, ullMesh, ullParentMesh, vecPosition, uiObjectId))
        {
            lua_pushnumber(luaVM, uiObjectId);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXAddTemplate(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    unsigned int     uiTemplate;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiTemplate, 0);

    if (!argStream.HasErrors())
    {
        unsigned int uiTemplateId;
        if (pFBX->AddTemplate(uiTemplate, uiTemplateId))
        {
            lua_pushnumber(luaVM, uiTemplateId);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXRemoveTemplate(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    unsigned int     uiTemplate;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiTemplate, 0);

    if (!argStream.HasErrors())
    {
        unsigned int uiTemplateId;
        if (pFBX->AddTemplate(uiTemplate, uiTemplateId))
        {
            lua_pushnumber(luaVM, uiTemplateId);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXTemplateRemoveModel(lua_State* luaVM)
{
    CClientFBX*        pFBX;
    unsigned int       uiId;
    unsigned long long ullMesh;
    unsigned long long ullParentMesh;
    CVector            vecPosition;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiId);
    argStream.ReadNumber(ullMesh);
    argStream.ReadNumber(ullParentMesh, 0);
    argStream.ReadVector3D(vecPosition, CVector(0, 0, 0));

    if (!argStream.HasErrors())
    {
        unsigned int uiObjectId;
        if (pFBX->AddMeshToTemplate(luaVM, uiId, ullMesh, ullParentMesh, vecPosition, uiObjectId))
        {
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXApplyTemplateToModel(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    unsigned int     uiTemplateId;
    unsigned long    ulModelID;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiTemplateId);
    argStream.ReadNumber(ulModelID);

    if (!argStream.HasErrors())
    {
        if (CClientObjectManager::IsValidModel(ulModelID))
        {
            if (pFBX->ApplyTemplateToModel(uiTemplateId, ulModelID))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXRemoveTemplateFromModel(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    unsigned int     uiTemplateId;
    unsigned long    ulModelID;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiTemplateId);
    argStream.ReadNumber(ulModelID);

    if (!argStream.HasErrors())
    {
        if (CClientObjectManager::IsValidModel(ulModelID))
        {
            if (pFBX->RemoveTemplateFromModel(uiTemplateId, ulModelID))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXApplyTemplateToElement(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    unsigned int     uiTemplateId;
    CClientEntity*   pElement;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiTemplateId);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        if (IS_OBJECT(pElement))
        {
            CDeathmatchObject* Object = static_cast<CDeathmatchObject*>(pElement);
            if (pFBX->ApplyTemplateToElement(uiTemplateId, Object))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXRemoveTemplateFromElement(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    unsigned int     uiTemplateId;
    CClientEntity*   pElement;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiTemplateId);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        if (IS_OBJECT(pElement))
        {
            CDeathmatchObject* Object = static_cast<CDeathmatchObject*>(pElement);
            if (pFBX->RemoveTemplateFromElement(uiTemplateId, Object))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXRenderTemplate(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    unsigned int     uiTemplateId;
    CVector          vecPosition;
    CVector          vecRotation;
    CVector          vecScale;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiTemplateId);
    argStream.ReadVector3D(vecPosition, CVector(0, 0, 0));
    argStream.ReadVector3D(vecRotation, CVector(0, 0, 0));
    argStream.ReadVector3D(vecScale, CVector(1, 1, 1));

    if (!argStream.HasErrors())
    {
        pFBX->RenderTemplate(uiTemplateId, vecPosition, vecRotation, vecScale);
        lua_pushboolean(luaVM, true);
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
        if (pFBX->IsMeshValid(strHierarchyMesh))
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
                case FBX_DATA_TYPE_MATERIAL:

                    argStream.ReadNumber(iStart, 0);
                    argStream.ReadNumber(iStop, 0);
                    iStart--, iStop--;
                    if (!pFBX->LuaRawGetMaterials(luaVM, pFBX->GetMeshByName(strHierarchyMesh), iStart, iStop))
                    {
                        argStream.SetCustomError("Invalid material range");
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
