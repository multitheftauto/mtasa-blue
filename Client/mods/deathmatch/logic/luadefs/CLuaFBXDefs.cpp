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
        {"fbxGetLoadingStatus", FBXGetLoadingStatus},
        {"fbxGetAllObjects", FBXGetAllObjects},
        {"fbxGetAllTemplates", FBXGetAllTemplates},
        {"fbxGetAllTemplateModels", FBXGetAllTemplateModels},
        {"fbxGetObjectProperties", FBXGetObjectProperties},
        {"fbxGetTemplateProperties", FBXGetTemplateProperties},
        {"fbxSetTemplateProperties", FBXSetTemplateProperties},
        {"fbxGetTemplateModelProperties", FBXGetTemplateModelProperties},
        {"fbxSetTemplateModelProperties", FBXSetTemplateModelProperties},
        {"fbxTemplateExists", FBXTemplateExists},
        {"fbxAddTemplate", FBXAddTemplate},
        {"fbxRemoveTemplate", FBXRemoveTemplate},
        {"fbxTemplateAddModel", FBXTemplateAddModel},
        {"fbxTemplateRemoveModel", FBXTemplateRemoveModel},
        {"fbxApplyTemplateToModel", FBXApplyTemplateToModel},
        {"fbxRemoveTemplateFromModel", FBXRemoveTemplateFromModel},
        {"fbxApplyTemplateToElement", FBXApplyTemplateToElement},
        {"fbxRemoveTemplateFromElement", FBXRemoveTemplateFromElement},
        {"fbxRenderTemplate", FBXRenderTemplate},
        {"fbxGetAllTextures", FBXGetAllTextures},
        {"fbxGetObjectRawData", FBXGetObjectRawData},

        
        {"fbxGetGlobalProperties", FBXGetGlobalProperties},
        {"fbxSetGlobalProperties", FBXSetGlobalProperties},

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

                    // prepare error message for future
                    argStream.SetCustomError(bIsRawData ? "raw data" : strFile, "%s");

                    // Try to load the DFF file
                    if (pFBX->LoadFBX(bIsRawData ? strFile : strPath, bIsRawData, luaVM, argStream.GetFullErrorMessage()))
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

int CLuaFBXDefs::FBXGetLoadingStatus(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);

    if (!argStream.HasErrors())
    {
        pFBX->LuaGetLoadingStatus(luaVM);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}
int CLuaFBXDefs::FBXGetAllObjects(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    eFBXObjectType   eObjectType;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadEnumString(eObjectType, FBX_OBJECT_ALL);

    if (!argStream.HasErrors())
    {
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
        lua_newtable(luaVM);
        pFBX->LuaGetAllObjectsIds(luaVM, eObjectType);
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
        if (pFBX->LuaGetAllTemplateModels(luaVM, uiTemplateId))
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
        const ofbx::Object* const* pObject = pFBX->GetObjectById(ulId);
        if (pObject != nullptr)
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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

int CLuaFBXDefs::FBXSetGlobalProperties(lua_State* luaVM)
{
    eFBXTemplateProperty eProperty;
    CScriptArgReader     argStream(luaVM);
    argStream.ReadEnumString(eProperty);

    if (!argStream.HasErrors())
    {
        if (pFBX(luaVM, uiId, eProperty))
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

int CLuaFBXDefs::FBXGetGlobalProperties(lua_State* luaVM)
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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

int CLuaFBXDefs::FBXTemplateExists(lua_State* luaVM)
{
    CClientFBX*      pFBX;
    unsigned int     uiTemplate;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiTemplate);

    if (!argStream.HasErrors())
    {
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
        lua_pushboolean(luaVM, pFBX->IsTemplateValid(uiTemplate));
        return 1;
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }

        if (pFBX->RemoveTemplate(uiTemplate))
        {
            lua_pushboolean(luaVM, true);
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
    CClientFBX*      pFBX;
    unsigned int     uiId;
    unsigned int     uiObjectId;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadNumber(uiId);
    argStream.ReadNumber(uiObjectId);

    if (!argStream.HasErrors())
    {
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
        if (pFBX->RemoveObjectToTemplate(uiId, uiObjectId))
        {
            lua_pushboolean(luaVM, true);
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }
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
        if (pFBX->IsLoaded())
        {
            if (pFBX->IsTemplateValid(uiTemplateId))
            {
                pFBX->RenderTemplate(uiTemplateId, vecPosition, vecRotation, vecScale);
                lua_pushboolean(luaVM, true);
            }
            else
                m_pScriptDebugging->LogCustom(luaVM, SString("Template id %i doesn't exists", uiTemplateId).c_str());
        }
        else
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");

    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFBXDefs::FBXGetObjectRawData(lua_State* luaVM)
{
    CClientFBX*        pFBX;
    unsigned long long ullObjectId;
    eFBXDataType       eDataType;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadUserData(pFBX);
    argStream.ReadEnumString(eDataType);
    argStream.ReadNumber(ullObjectId);

    if (!argStream.HasErrors())
    {
        if (!pFBX->IsLoaded())
        {
            m_pScriptDebugging->LogCustom(luaVM, "FBX isn't loaded yet");
            lua_pushboolean(luaVM, false);
            return 1;
        }

        if (pFBX->IsObjectValid(ullObjectId))
        {
            const ofbx::Object* const* pObject = pFBX->GetObjectById(ullObjectId);
            switch (eDataType)
            {
                case FBX_DATA_TYPE_VERTEX:
                    if ((*pObject)->getType() == ofbx::Object::Type::GEOMETRY)
                        pFBX->LuaRawGetVertices(luaVM, pObject);
                    else
                        argStream.SetCustomError("Not supported object type");
                    break;
                case FBX_DATA_TYPE_INDEX:
                    if ((*pObject)->getType() == ofbx::Object::Type::GEOMETRY)
                        pFBX->LuaRawGetIndices(luaVM, pObject);
                    else
                        argStream.SetCustomError("Not supported object type");
                    break;
            }
            return 1;
        }
        else
        {
            argStream.SetCustomError("Object not found");
        }
    }

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }
    lua_pushboolean(luaVM, false);
    return 1;
}
