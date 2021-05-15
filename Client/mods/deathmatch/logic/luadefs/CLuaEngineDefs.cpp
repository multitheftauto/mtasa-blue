/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaEngineDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

void CLuaEngineDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"engineFreeModel", EngineFreeModel},
        {"engineLoadTXD", EngineLoadTXD},
        {"engineLoadCOL", EngineLoadCOL},
        {"engineLoadDFF", EngineLoadDFF},
        {"engineLoadIFP", EngineLoadIFP},
        {"engineImportTXD", EngineImportTXD},
        {"engineReplaceCOL", EngineReplaceCOL},
        {"engineRestoreCOL", EngineRestoreCOL},
        {"engineReplaceModel", EngineReplaceModel},
        {"engineRestoreModel", EngineRestoreModel},
        {"engineReplaceAnimation", EngineReplaceAnimation},
        {"engineRestoreAnimation", EngineRestoreAnimation},
        {"engineRequestModel", EngineRequestModel},
        {"engineGetModelLODDistance", EngineGetModelLODDistance},
        {"engineSetModelLODDistance", EngineSetModelLODDistance},
        {"engineResetModelLODDistance", EngineResetModelLODDistance},
        {"engineSetAsynchronousLoading", EngineSetAsynchronousLoading},
        {"engineApplyShaderToWorldTexture", EngineApplyShaderToWorldTexture},
        {"engineRemoveShaderFromWorldTexture", EngineRemoveShaderFromWorldTexture},
        {"engineGetModelNameFromID", EngineGetModelNameFromID},
        {"engineGetModelIDFromName", EngineGetModelIDFromName},
        {"engineGetModelTextureNames", EngineGetModelTextureNames},
        {"engineGetVisibleTextureNames", EngineGetVisibleTextureNames},
        {"engineSetModelVisibleTime", ArgumentParser<EngineSetModelVisibleTime>},
        {"engineGetModelVisibleTime", ArgumentParser<EngineGetModelVisibleTime>},
        {"engineGetModelTextures", EngineGetModelTextures},
        {"engineGetSurfaceProperties", EngineGetSurfaceProperties},
        {"engineSetSurfaceProperties", EngineSetSurfaceProperties},
        {"engineResetSurfaceProperties", EngineResetSurfaceProperties},
        {"engineGetModelPhysicalPropertiesGroup", EngineGetModelPhysicalPropertiesGroup},
        {"engineSetModelPhysicalPropertiesGroup", EngineSetModelPhysicalPropertiesGroup},
        {"engineRestoreModelPhysicalPropertiesGroup", EngineRestoreModelPhysicalPropertiesGroup},
        {"engineSetObjectGroupPhysicalProperty", EngineSetObjectGroupPhysicalProperty},
        {"engineGetObjectGroupPhysicalProperty", EngineGetObjectGroupPhysicalProperty},
        {"engineRestoreObjectGroupPhysicalProperties", EngineRestoreObjectGroupPhysicalProperties},
        {"engineRestreamWorld", ArgumentParser<EngineRestreamWorld>},

        // CLuaCFunctions::AddFunction ( "engineReplaceMatchingAtomics", EngineReplaceMatchingAtomics );
        // CLuaCFunctions::AddFunction ( "engineReplaceWheelAtomics", EngineReplaceWheelAtomics );
        // CLuaCFunctions::AddFunction ( "enginePositionAtomic", EnginePositionAtomic );
        // CLuaCFunctions::AddFunction ( "enginePositionSeats", EnginePositionSeats );
        // CLuaCFunctions::AddFunction ( "engineAddAllAtomics", EngineAddAllAtomics );
        // CLuaCFunctions::AddFunction ( "engineReplaceVehiclePart", EngineReplaceVehiclePart );
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaEngineDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "restoreCOL", "engineRestoreCOL");
    lua_classfunction(luaVM, "restoreModel", "engineRestoreModel");
    lua_classfunction(luaVM, "setAsynchronousLoading", "engineSetAsynchronousLoading");
    lua_classfunction(luaVM, "setModelLODDistance", "engineSetModelLODDistance");
    lua_classfunction(luaVM, "resetModelLODDistance", "engineResetModelLODDistance");
    lua_classfunction(luaVM, "setModelVisibleTime", "engineSetModelVisibleTime");

    lua_classfunction(luaVM, "getVisibleTextureNames", "engineGetVisibleTextureNames");
    lua_classfunction(luaVM, "getModelLODDistance", "engineGetModelLODDistance");
    lua_classfunction(luaVM, "getModelTextureNames", "engineGetModelTextureNames");
    lua_classfunction(luaVM, "getModelTextures", "engineGetModelTextures");
    lua_classfunction(luaVM, "getModelIDFromName", "engineGetModelIDFromName");
    lua_classfunction(luaVM, "getModelNameFromID", "engineGetModelNameFromID");
    lua_classfunction(luaVM, "getModelVisibleTime", "engineGetModelVisibleTime");
    lua_classfunction(luaVM, "getModelPhysicalPropertiesGroup", "engineGetModelPhysicalPropertiesGroup");
    lua_classfunction(luaVM, "setModelPhysicalPropertiesGroup", "engineSetModelPhysicalPropertiesGroup");
    lua_classfunction(luaVM, "restoreModelPhysicalPropertiesGroup", "engineRestoreModelPhysicalPropertiesGroup");
    lua_classfunction(luaVM, "setObjectGroupPhysicalProperty", "engineSetObjectGroupPhysicalProperty");
    lua_classfunction(luaVM, "getObjectGroupPhysicalProperty", "engineGetObjectGroupPhysicalProperty");
    lua_classfunction(luaVM, "restoreObjectGroupPhysicalProperties", "engineRestoreObjectGroupPhysicalProperties");

    lua_registerstaticclass(luaVM, "Engine");

    AddEngineColClass(luaVM);
    AddEngineTxdClass(luaVM);
    AddEngineDffClass(luaVM);
}

void CLuaEngineDefs::AddEngineColClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "engineLoadCOL");
    lua_classfunction(luaVM, "replace", "engineReplaceCOL");

    lua_registerclass(luaVM, "EngineCOL", "Element");
}

void CLuaEngineDefs::AddEngineTxdClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "engineLoadTXD");

    lua_classfunction(luaVM, "import", "engineImportTXD");

    lua_registerclass(luaVM, "EngineTXD", "Element");
}

void CLuaEngineDefs::AddEngineDffClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "engineLoadDFF");
    lua_classfunction(luaVM, "replace", "engineReplaceModel");

    lua_registerclass(luaVM, "EngineDFF", "Element");
}

int CLuaEngineDefs::EngineLoadCOL(lua_State* luaVM)
{
    SString          input;
    CScriptArgReader argStream(luaVM);
    // Grab the COL filename or data
    argStream.ReadString(input);

    if (!argStream.HasErrors())
    {
        // Grab the lua main and the resource belonging to this script
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Get the resource we belong to
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                bool bIsRawData = CClientColModel::IsCOLData(input);

                // Do not proceed if the file path length appears too long to be real
                if (!bIsRawData && input.size() > 32767)
                {
                    argStream.SetCustomError("Corrupt COL file data or file path too long", "Error loading COL");
                    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                    lua_pushboolean(luaVM, false);
                    return 1;
                }

                SString filePath;

                // Is this a legal filepath?
                if (bIsRawData || CResourceManager::ParseResourcePathInput(input, pResource, &filePath))
                {
                    // Grab the resource root entity
                    CClientEntity* pRoot = pResource->GetResourceCOLModelRoot();

                    // Create the col model
                    CClientColModel* pCol = new CClientColModel(m_pManager, INVALID_ELEMENT_ID);

                    // Attempt loading the file
                    if (pCol->Load(bIsRawData, bIsRawData ? std::move(input) : std::move(filePath)))
                    {
                        // Success. Make it a child of the resource collision root
                        pCol->SetParent(pRoot);

                        // Return the created col model
                        lua_pushelement(luaVM, pCol);
                        return 1;
                    }
                    else
                    {
                        // Delete it again. We failed
                        delete pCol;
                        argStream.SetCustomError(bIsRawData ? "raw data" : input, "Error loading COL");
                    }
                }
                else
                {
                    argStream.SetCustomError(bIsRawData ? "raw data" : input, "Bad file path");
                }
            }
        }
    }

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineLoadDFF(lua_State* luaVM)
{
    SString          input = "";
    CScriptArgReader argStream(luaVM);
    // Grab the DFF filename or data (model ID ignored after 1.3.1)
    argStream.ReadString(input);

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
                bool bIsRawData = CClientDFF::IsDFFData(input);

                // Do not proceed if the file path length appears too long to be real
                if (!bIsRawData && input.size() > 32767)
                {
                    argStream.SetCustomError("Corrupt DFF file data or file path too long", "Error loading DFF");
                    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                    lua_pushboolean(luaVM, false);
                    return 1;
                }

                SString filePath;

                if (bIsRawData || CResourceManager::ParseResourcePathInput(input, pResource, &filePath))
                {
                    // Grab the resource root entity
                    CClientEntity* pRoot = pResource->GetResourceDFFRoot();

                    // Create a DFF element
                    CClientDFF* pDFF = new CClientDFF(m_pManager, INVALID_ELEMENT_ID);

                    // Try to load the DFF file
                    if (pDFF->Load(bIsRawData, bIsRawData ? std::move(input) : std::move(filePath)))
                    {
                        // Success loading the file. Set parent to DFF root
                        pDFF->SetParent(pRoot);

                        // Return the DFF
                        lua_pushelement(luaVM, pDFF);
                        return 1;
                    }
                    else
                    {
                        // Delete it again
                        delete pDFF;
                        argStream.SetCustomError(bIsRawData ? "raw data" : input, "Error loading DFF");
                    }
                }
                else
                {
                    argStream.SetCustomError(bIsRawData ? "raw data" : input, "Bad file path");
                }
            }
        }
    }

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineLoadTXD(lua_State* luaVM)
{
    SString          input;
    bool             bFilteringEnabled = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(input);
    if (argStream.NextIsBool())            // Some scripts have a number here (in error)
        argStream.ReadBool(bFilteringEnabled, true);

    if (!argStream.HasErrors())
    {
        // Grab our virtual machine and grab our resource from that.
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Grab this resource
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                bool bIsRawData = CClientTXD::IsTXDData(input);

                // Do not proceed if the file path length appears too long to be real
                if (!bIsRawData && input.size() > 32767)
                {
                    argStream.SetCustomError("Corrupt TXD file data or file path too long", "Error loading TXD");
                    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                    lua_pushboolean(luaVM, false);
                    return 1;
                }

                SString filePath;

                if (bIsRawData || CResourceManager::ParseResourcePathInput(input, pResource, &filePath))
                {
                    // Grab the resource root entity
                    CClientEntity* pRoot = pResource->GetResourceTXDRoot();

                    // Create a TXD element
                    CClientTXD* pTXD = new CClientTXD(m_pManager, INVALID_ELEMENT_ID);

                    // Try to load the TXD file
                    if (pTXD->Load(bIsRawData, bIsRawData ? std::move(input) : std::move(filePath), bFilteringEnabled))
                    {
                        // Success loading the file. Set parent to TXD root
                        pTXD->SetParent(pRoot);

                        // Return the TXD
                        lua_pushelement(luaVM, pTXD);
                        return 1;
                    }
                    else
                    {
                        // Delete it again
                        delete pTXD;
                        argStream.SetCustomError(bIsRawData ? "raw data" : input, "Error loading TXD");
                    }
                }
                else
                    argStream.SetCustomError(bIsRawData ? "raw data" : input, "Bad file path");
            }
        }
    }

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineLoadIFP(lua_State* luaVM)
{
    SString input;
    SString blockName;

    CScriptArgReader argStream(luaVM);
    // Grab the IFP filename or data
    argStream.ReadString(input);
    argStream.ReadString(blockName);

    if (!argStream.HasErrors())
    {
        // Grab our virtual machine and grab our resource from that.
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Grab this resource
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                bool bIsRawData = CIFPEngine::IsIFPData(input);

                // Do not proceed if the file path length appears too long to be real
                if (!bIsRawData && input.size() > 32767)
                {
                    argStream.SetCustomError("Corrupt IFP file data or file path too long", "Error loading IFP");
                    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                    lua_pushboolean(luaVM, false);
                    return 1;
                }

                SString filePath;

                // Is this a legal filepath?
                if (bIsRawData || CResourceManager::ParseResourcePathInput(input, pResource, &filePath))
                {
                    std::shared_ptr<CClientIFP> pIFP = CIFPEngine::LoadIFP(pResource, m_pManager, std::move(blockName), bIsRawData, bIsRawData ? std::move(input) : std::move(filePath));

                    if (pIFP)
                    {
                        // Return the IFP element
                        lua_pushelement(luaVM, pIFP.get());
                        return 1;
                    }
                    else
                    {
                        argStream.SetCustomError(bIsRawData ? "raw data" : input, "Error loading IFP");
                    }
                }
                else
                {
                    argStream.SetCustomError(bIsRawData ? "raw data" : input, "Bad file path");
                }
            }
        }
    }

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineReplaceCOL(lua_State* luaVM)
{
    CClientColModel* pCol = NULL;
    unsigned short   usModel = 0;
    CScriptArgReader argStream(luaVM);
    // Grab the COL and model ID
    argStream.ReadUserData(pCol);
    argStream.ReadNumber(usModel);

    if (!argStream.HasErrors())
    {
        // Valid client DFF and model?
        if (CClientColModelManager::IsReplacableModel(usModel))
        {
            // Replace the colmodel
            if (pCol->Replace(usModel))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer(luaVM, "number", 2);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineRestoreCOL(lua_State* luaVM)
{
    SString          strModelName = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strModelName);

    if (!argStream.HasErrors())
    {
        unsigned short usModelID = CModelNames::ResolveModelID(strModelName);

        if (m_pColModelManager->RestoreModel(usModelID))
        {
            // Success
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed.
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineImportTXD(lua_State* luaVM)
{
    CClientTXD*      pTXD = NULL;
    SString          strModelName;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pTXD);
    argStream.ReadString(strModelName);

    if (!argStream.HasErrors())
    {
        // Valid importable model?
        ushort usModelID = CModelNames::ResolveModelID(strModelName);
        if (usModelID == INVALID_MODEL_ID)
            usModelID = CModelNames::ResolveClothesTexID(strModelName);
        if (CClientTXD::IsImportableModel(usModelID))
        {
            // Try to import
            if (pTXD->Import(usModelID))
            {
                // Success
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer(luaVM, "number", 2);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineReplaceModel(lua_State* luaVM)
{
    CClientDFF* pDFF;
    SString     strModelName;
    bool        bAlphaTransparency;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pDFF);
    argStream.ReadString(strModelName);
    argStream.ReadBool(bAlphaTransparency, false);

    if (!argStream.HasErrors())
    {
        ushort usModelID = CModelNames::ResolveModelID(strModelName);
        if (usModelID != INVALID_MODEL_ID)
        {
            // Fixes vehicle dff leak problem with engineReplaceModel
            m_pDFFManager->RestoreModel(usModelID);
            if (pDFF->ReplaceModel(usModelID, bAlphaTransparency))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
            else
                argStream.SetCustomError(SString("Model ID %d replace failed", usModelID));
        }
        else
            argStream.SetCustomError("Expected valid model ID or name at argument 2");
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineRestoreModel(lua_State* luaVM)
{
    // Grab the model ID
    unsigned short usModelID = CModelNames::ResolveModelID(lua_tostring(luaVM, 1));

    // Valid client DFF and model?
    if (CClientDFFManager::IsReplacableModel(usModelID))
    {
        // Restore the model
        if (m_pDFFManager->RestoreModel(usModelID))
        {
            // Success
            lua_pushboolean(luaVM, true);
            return true;
        }
    }
    else
    {
        m_pScriptDebugging->LogBadType(luaVM);
    }

    // Failure
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineRequestModel(lua_State* luaVM)
{
    eClientModelType eModelType;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(eModelType);

    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CResource* pResource = pLuaMain->GetResource();
        if (pResource)
        {
            if (!argStream.HasErrors())
            {
                int iModelID = m_pManager->GetModelManager()->GetFirstFreeModelID();
                if (iModelID != INVALID_MODEL_ID) {
                    std::shared_ptr<CClientModel> pModel = m_pManager->GetModelManager()->FindModelByID(iModelID);
                    if (pModel == nullptr)
                        pModel = std::make_shared<CClientModel>(m_pManager, iModelID, eModelType);
                    m_pManager->GetModelManager()->Add(pModel);
                    ushort usParentID = -1;

                    if (argStream.NextIsNumber())
                        argStream.ReadNumber(usParentID);
                    else
                    {
                        switch (eModelType)
                        {
                            case eClientModelType::PED:
                                usParentID = 7; // male01
                                break;
                            case eClientModelType::OBJECT:
                                usParentID = 1337; // BinNt07_LA (trash can)
                                break;
                            case eClientModelType::VEHICLE:
                                usParentID = VT_LANDSTAL;
                                break;
                            default:
                                break;
                        }
                    }
                    pModel->Allocate(usParentID);
                    pModel->SetParentResource(pResource);

                    lua_pushinteger(luaVM, iModelID);
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        }
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineFreeModel(lua_State* luaVM)
{
    int iModelID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iModelID);

    if (!argStream.HasErrors())
    {
        auto modelManager = m_pManager->GetModelManager();
        std::shared_ptr<CClientModel> pModel = modelManager->FindModelByID(iModelID);
        if (pModel && modelManager->Remove(pModel))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineReplaceAnimation(lua_State* luaVM)
{
    CClientEntity* pEntity = nullptr;
    SString        strInternalBlockName = "";
    SString        strInternalAnimName = "";
    SString        strCustomBlockName = "";
    SString        strCustomAnimName = "";

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadString(strInternalBlockName);
    argStream.ReadString(strInternalAnimName);
    argStream.ReadString(strCustomBlockName);
    argStream.ReadString(strCustomAnimName);

    if (!argStream.HasErrors())
    {
        if (CIFPEngine::EngineReplaceAnimation(pEntity, strInternalBlockName, strInternalAnimName, strCustomBlockName, strCustomAnimName))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineRestoreAnimation(lua_State* luaVM)
{
    CClientEntity*                pEntity = nullptr;
    CIFPEngine::eRestoreAnimation eRestoreType = CIFPEngine::eRestoreAnimation::SINGLE;
    SString                       strInternalBlockName = "";
    SString                       strInternalAnimName = "";

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    if (argStream.NextIsNil() || argStream.NextIsNone())
    {
        eRestoreType = CIFPEngine::eRestoreAnimation::ALL;
    }
    else
    {
        argStream.ReadString(strInternalBlockName);
        if (argStream.NextIsNil() || argStream.NextIsNone())
        {
            eRestoreType = CIFPEngine::eRestoreAnimation::BLOCK;
        }
        else
        {
            argStream.ReadString(strInternalAnimName);
        }
    }

    if (!argStream.HasErrors())
    {
        if (CIFPEngine::EngineRestoreAnimation(pEntity, strInternalBlockName, strInternalAnimName, eRestoreType))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineGetModelLODDistance(lua_State* luaVM)
{
    // float engineGetModelLODDistance ( int/string modelID )
    SString          strModelId;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strModelId);

    if (!argStream.HasErrors())
    {
        ushort usModelID = CModelNames::ResolveModelID(strModelId);
        // Ensure we have a good model (GitHub #446)
        if (usModelID < g_pGame->GetBaseIDforTXD())
        {
            CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModelID);
            if (pModelInfo)
            {
                lua_pushnumber(luaVM, pModelInfo->GetLODDistance());
                return 1;
            }
        }
        else
            argStream.SetCustomError(SString("Expected a valid model name or ID in range [0-%d] at argument 1, got \"%s\"", g_pGame->GetBaseIDforTXD(), *strModelId));
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineSetModelLODDistance(lua_State* luaVM)
{
    // bool engineSetModelLODDistance ( int/string modelID, float distance )
    SString          strModelId;
    float            fDistance;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strModelId);
    argStream.ReadNumber(fDistance);

    if (!argStream.HasErrors())
    {
        ushort usModelID = CModelNames::ResolveModelID(strModelId);
        // Ensure we have a good model (GitHub #446)
        if (usModelID < g_pGame->GetBaseIDforTXD())
        {
            CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModelID);
            if (pModelInfo && fDistance > 0.0f)
            {
                pModelInfo->SetLODDistance(fDistance);
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
            argStream.SetCustomError(SString("Expected a valid model name or ID in range [0-%d] at argument 1, got \"%s\"", g_pGame->GetBaseIDforTXD() - 1,* strModelId));
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineResetModelLODDistance(lua_State* luaVM)
{
    SString          strModel = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strModel);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    unsigned short usModelID = CModelNames::ResolveModelID(strModel);
    CModelInfo*    pModelInfo = g_pGame->GetModelInfo(usModelID);
    if (pModelInfo)
    {
        float fCurrentDistance = pModelInfo->GetLODDistance();
        float fOriginalDistance = pModelInfo->GetOriginalLODDistance();
        //Make sure we're dealing with a valid LOD distance, and not setting the same LOD distance
        if (fOriginalDistance > 0.0f && fOriginalDistance != fCurrentDistance) {
            pModelInfo->SetLODDistance(fOriginalDistance, true);
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineSetAsynchronousLoading(lua_State* luaVM)
{
    bool             bEnabled = false;
    bool             bForced = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bEnabled);
    argStream.ReadBool(bForced, false);

    if (!argStream.HasErrors())
    {
        g_pGame->SetAsyncLoadingFromScript(bEnabled, bForced);
        lua_pushboolean(luaVM, true);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

// TODO: int CLuaEngineDefs::EngineReplaceMatchingAtomics ( lua_State* luaVM )
int CLuaEngineDefs::EngineReplaceMatchingAtomics(lua_State* luaVM)
{
    /*
    RpClump * pClump = ( lua_istype ( 1, LUA_TLIGHTUSERDATA ) ? reinterpret_cast < RpClump* > ( lua_touserdata ( luaVM, 1 ) ) : NULL );
    CClientEntity* pEntity = ( lua_istype ( 2, LUA_TLIGHTUSERDATA ) ? reinterpret_cast < CClientEntity* > ( lua_touserdata ( luaVM, 2 ) ) : NULL );
    RpAtomicContainer Atomics[MAX_ATOMICS_PER_CLUMP];
    RpClump * pEntityClump = NULL;
    unsigned int uiAtomics = 0;

    if ( pEntity ) {
    if ( IS_VEHICLE ( pEntity ) )
    pEntityClump = static_cast < CClientVehicle* > ( pEntity ) -> GetGameVehicle () -> GetRpClump ();
    else if ( IS_OBJECT ( pEntity ) )
    pEntityClump = static_cast < CClientObject* > ( pEntity ) -> GetGameObject () -> GetRpClump ();
    else {
    m_pScriptDebugging->LogWarning ( luaVM, "engineReplaceMatchingAtomics only supports vehicles and objects." );
    m_pScriptDebugging->LogBadType ( luaVM );
    }
    }

    if ( pEntityClump && pClump ) {
    uiAtomics = m_pRenderWare->LoadAtomics ( pClump, &Atomics[0] );
    m_pRenderWare->ReplaceAllAtomicsInClump ( pEntityClump, &Atomics[0], uiAtomics );

    lua_pushboolean ( luaVM, true );
    } else {
    m_pScriptDebugging->LogBadType ( luaVM );
    lua_pushboolean ( luaVM, false );
    }
    */

    lua_pushboolean(luaVM, false);
    return 1;
}

// TODO: int CLuaEngineDefs::EngineReplaceWheelAtomics ( lua_State* luaVM )
int CLuaEngineDefs::EngineReplaceWheelAtomics(lua_State* luaVM)
{
    /*
    RpClump * pClump = ( lua_istype ( 1, LUA_TLIGHTUSERDATA ) ? reinterpret_cast < RpClump* > ( lua_touserdata ( luaVM, 1 ) ) : NULL );
    CClientEntity* pEntity = ( lua_istype ( 2, LUA_TLIGHTUSERDATA ) ? reinterpret_cast < CClientEntity* > ( lua_touserdata ( luaVM, 2 ) ) : NULL );
    const char * szWheel = ( lua_istype ( 3, LUA_TSTRING ) ? lua_tostring ( luaVM, 3 ) : NULL );

    RpAtomicContainer Atomics[MAX_ATOMICS_PER_CLUMP];
    RpClump * pEntityClump = NULL;
    unsigned int uiAtomics = 0;

    if ( pEntity ) {
    if ( IS_VEHICLE ( pEntity ) )
    pEntityClump = static_cast < CClientVehicle* > ( pEntity ) -> GetGameVehicle () -> GetRpClump ();
    else if ( IS_OBJECT ( pEntity ) )
    pEntityClump = static_cast < CClientObject* > ( pEntity ) -> GetGameObject () -> GetRpClump ();
    else {
    m_pScriptDebugging->LogWarning ( luaVM, "engineReplaceWheelAtomics only supports vehicles and objects." );
    }
    }

    if ( pClump ) {
    uiAtomics = m_pRenderWare->LoadAtomics ( pClump, &Atomics[0] );
    m_pScriptDebugging->LogWarning ( luaVM, "engineReplaceWheelAtomics DFF argument was not valid." );
    }

    if ( pEntityClump && uiAtomics > 0 && szWheel ) {
    m_pRenderWare->ReplaceWheels ( pEntityClump, &Atomics[0], uiAtomics, szWheel );

    lua_pushboolean ( luaVM, true );
    } else {
    m_pScriptDebugging->LogBadType ( luaVM );
    lua_pushboolean ( luaVM, false );
    }
    */

    lua_pushboolean(luaVM, false);
    return 1;
}

// TODO: int CLuaEngineDefs::EnginePositionAtomic ( lua_State* luaVM )
int CLuaEngineDefs::EnginePositionAtomic(lua_State* luaVM)
{
    lua_pushboolean(luaVM, false);
    return 1;
}

// TODO: int CLuaEngineDefs::EnginePositionSeats ( lua_State* luaVM )
int CLuaEngineDefs::EnginePositionSeats(lua_State* luaVM)
{
    lua_pushboolean(luaVM, false);
    return 1;
}

// TODO: int CLuaEngineDefs::EngineAddAllAtomics ( lua_State* luaVM )
int CLuaEngineDefs::EngineAddAllAtomics(lua_State* luaVM)
{
    lua_pushboolean(luaVM, false);
    return 1;
}

// TODO: int CLuaEngineDefs::EngineReplaceVehiclePart ( lua_State* luaVM )
int CLuaEngineDefs::EngineReplaceVehiclePart(lua_State* luaVM)
{
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineApplyShaderToWorldTexture(lua_State* luaVM)
{
    //  bool engineApplyShaderToWorldTexture ( element shader, string textureName, [ element targetElement, bool appendLayers ] )
    CClientShader* pShader;
    SString        strTextureNameMatch;
    CClientEntity* pElement;
    bool           bAppendLayers;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pShader);
    argStream.ReadString(strTextureNameMatch);
    argStream.ReadUserData(pElement, NULL);
    argStream.ReadBool(bAppendLayers, true);

    if (!argStream.HasErrors())
    {
        bool bResult = g_pCore->GetGraphics()->GetRenderItemManager()->ApplyShaderItemToWorldTexture(pShader->GetShaderItem(), strTextureNameMatch, pElement,
                                                                                                     bAppendLayers);
        lua_pushboolean(luaVM, bResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // We failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineRemoveShaderFromWorldTexture(lua_State* luaVM)
{
    //  bool engineRemoveShaderFromWorldTexture ( element shader, string textureName, [ element targetElement ] )
    CClientShader* pShader;
    SString        strTextureNameMatch;
    CClientEntity* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pShader);
    argStream.ReadString(strTextureNameMatch);
    argStream.ReadUserData(pElement, NULL);

    if (!argStream.HasErrors())
    {
        bool bResult =
            g_pCore->GetGraphics()->GetRenderItemManager()->RemoveShaderItemFromWorldTexture(pShader->GetShaderItem(), strTextureNameMatch, pElement);
        lua_pushboolean(luaVM, bResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // We failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineGetModelNameFromID(lua_State* luaVM)
{
    //  string engineGetModelNameFromID ( int modelID )
    int iModelID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iModelID);

    if (!argStream.HasErrors())
    {
        SString strModelName = CModelNames::GetModelName(iModelID);
        if (!strModelName.empty())
        {
            lua_pushstring(luaVM, strModelName);
            return 1;
        }
        argStream.SetCustomError("Expected valid model ID at argument 1");
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // We failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineGetModelIDFromName(lua_State* luaVM)
{
    //  int engineGetModelIDFromName ( string modelName )
    SString strModelName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strModelName);

    if (!argStream.HasErrors())
    {
        int iModelID = CModelNames::GetModelID(strModelName);
        if (iModelID != INVALID_MODEL_ID)
        {
            lua_pushnumber(luaVM, iModelID);
            return 1;
        }
        argStream.SetCustomError("Expected valid model name at argument 1");
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // We failed
    lua_pushboolean(luaVM, false);
    return 1;
}

// Only works if the model is loaded. EngineGetVisibleTextureNames should be used instead
int CLuaEngineDefs::EngineGetModelTextureNames(lua_State* luaVM)
{
    //  table engineGetModelTextureNames ( string modelName )
    SString strModelName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strModelName, "");

    if (!argStream.HasErrors())
    {
        ushort usModelID = CModelNames::ResolveModelID(strModelName);
        if (usModelID != INVALID_MODEL_ID)
        {
            std::vector<SString> nameList;
            g_pGame->GetRenderWare()->GetModelTextureNames(nameList, usModelID);

            lua_newtable(luaVM);
            for (uint i = 0; i < nameList.size(); i++)
            {
                lua_pushnumber(luaVM, i + 1);
                lua_pushstring(luaVM, nameList[i]);
                lua_settable(luaVM, -3);
            }
            return 1;
        }
        argStream.SetCustomError("Expected valid model ID or name at argument 1");
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // We failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineGetVisibleTextureNames(lua_State* luaVM)
{
    //  table engineGetVisibleTextureNames ( string wildcardMatch = "*" [, string modelName )
    SString strTextureNameMatch;
    SString strModelName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strTextureNameMatch, "*");
    argStream.ReadString(strModelName, "");

    if (!argStream.HasErrors())
    {
        ushort usModelID = CModelNames::ResolveModelID(strModelName);
        if (usModelID != INVALID_MODEL_ID || strModelName == "")
        {
            std::vector<SString> nameList;
            g_pCore->GetGraphics()->GetRenderItemManager()->GetVisibleTextureNames(nameList, strTextureNameMatch, usModelID);

            lua_newtable(luaVM);
            for (uint i = 0; i < nameList.size(); i++)
            {
                lua_pushnumber(luaVM, i + 1);
                lua_pushstring(luaVM, nameList[i]);
                lua_settable(luaVM, -3);
            }
            return 1;
        }
        argStream.SetCustomError("Expected valid model ID or name at argument 1");
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // We failed
    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaEngineDefs::EngineSetModelVisibleTime(std::string strModelId, char cHourOn, char cHourOff)
{
    ushort      usModelID = CModelNames::ResolveModelID(strModelId);
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModelID);
    if (pModelInfo)
    {
        if (cHourOn >= 0 && cHourOn <= 24 && cHourOff >= 0 && cHourOff <= 24)
        {
            return pModelInfo->SetTime(cHourOn, cHourOff);
        }
    }
    return false;
}

std::variant<bool, CLuaMultiReturn<char, char>> CLuaEngineDefs::EngineGetModelVisibleTime(std::string strModelId)
{
    ushort      usModelID = CModelNames::ResolveModelID(strModelId);
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModelID);
    if (pModelInfo)
    {
        char cHourOn, cHourOff;
        if (pModelInfo->GetTime(cHourOn, cHourOff))
        {
            return std::tuple(cHourOn, cHourOff);
        }
        else // Model is incompatible, don't let confuse user.
        {
            return std::tuple(0, 24);
        }
    }

    return false;
}

int CLuaEngineDefs::EngineGetModelTextures(lua_State* luaVM)
{
    //  table engineGetModelTextures ( string/int modelName/modelID, string/table textureNames )
    SString                                       strModelName;
    std::vector<SString>                          vTextureNames;
    std::vector<std::tuple<std::string, CPixels>> textureList;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strModelName);

    if (argStream.NextIsString())
    {
        SString strTextureName;
        argStream.ReadString(strTextureName, "");
        vTextureNames.push_back(strTextureName);
    }
    else if (argStream.NextIsTable())
        argStream.ReadStringTable(vTextureNames);

    ushort usModelID = CModelNames::ResolveModelID(strModelName);

    if (usModelID == INVALID_MODEL_ID || !g_pGame->GetRenderWare()->GetModelTextures(textureList, usModelID, vTextureNames))
    {
        argStream.SetCustomError("Invalid model ID");
        lua_pushboolean(luaVM, false);
    }

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    CLuaMain*  pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    CResource* pParentResource = pLuaMain->GetResource();

    lua_newtable(luaVM);
    for (const auto& pair : textureList)
    {
        CClientTexture* pTexture = g_pClientGame->GetManager()->GetRenderElementManager()->CreateTexture("", &std::get<1>(pair));

        if (pTexture)
        {
            pTexture->SetParent(pParentResource->GetResourceDynamicEntity());
        }
        lua_pushstring(luaVM, std::get<0>(pair).c_str());
        lua_pushelement(luaVM, pTexture);
        lua_settable(luaVM, -3);
    }

    return 1;
}

int CLuaEngineDefs::EngineSetSurfaceProperties(lua_State* luaVM)
{
    int                iSurfaceID;
    eSurfaceProperties eType;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadNumber(iSurfaceID);
    argStream.ReadEnumString(eType);
    if (!argStream.HasErrors())
    {
        if (iSurfaceID >= EColSurfaceValue::DEFAULT && iSurfaceID <= EColSurfaceValue::SIZE)
        {
            CSurfaceType*  pSurfaceInfo = g_pGame->GetWorld()->GetSurfaceInfo();
            SurfaceInfo_c* pSurface = &pSurfaceInfo->surfType[iSurfaceID];

            bool bEnabled;
            switch (eType)
            {
                case SURFACE_PROPERTY_AUDIO:
                    eSurfaceAudio eAudio;
                    argStream.ReadEnumString(eAudio);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(1, SURFACE_AUDIO_CONCRETE, false, 6);
                        pSurface->setFlagEnabled(1, eAudio, true);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_STEPWATERSPLASH:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(0, 16, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_FOOTEFFECT:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(0, 29, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_CREATEOBJECTS:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(1, 7, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_CREATEPLANTS:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(1, 8, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_BULLETEFFECT:
                    eSurfaceBulletEffect eBulletEffect;
                    argStream.ReadEnumString(eBulletEffect);
                    if (!argStream.HasErrors())
                    {
                        pSurface->m_bulletFx = eBulletEffect;
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_SHOOTTHROUGH:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(0, 14, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_SEETHROUGH:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(0, 13, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_STEEPSLOPE:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(0, 18, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_GLASS:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(0, 19, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_PAVEMENT:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(0, 22, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_SOFTLANDING:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(0, 11, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_SKIDMARKTYPE:
                    eSurfaceSkidMarkType eSkidMarkType;
                    argStream.ReadEnumString(eSkidMarkType);
                    if (!argStream.HasErrors())
                    {
                        pSurface->m_skidmarkType = eSkidMarkType;
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_FRACTIONEFFECT:
                    short sFractionEffect;
                    argStream.ReadNumber(sFractionEffect);
                    if (!argStream.HasErrors() && sFractionEffect >= 0 && sFractionEffect < 8)
                    {
                        pSurface->m_frictionEffect = sFractionEffect;
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    else
                        argStream.SetCustomError("Expected valid fraction effect ID ( 0 - 7 ) at argument 3");
                    break;
                case SURFACE_PROPERTY_TYREGRIP:
                    uint uiTyreGrip;
                    argStream.ReadNumber(uiTyreGrip);
                    if (!argStream.HasErrors() && uiTyreGrip >= 0 && uiTyreGrip <= 255)
                    {
                        pSurface->m_tyreGrip = uiTyreGrip;
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    else
                        argStream.SetCustomError("Expected valid tyre grip ( 0 - 255 ) at argument 3");
                    break;
                case SURFACE_PROPERTY_WETGRIP:
                    uint uiWetGrip;
                    argStream.ReadNumber(uiWetGrip);
                    if (!argStream.HasErrors() && uiWetGrip >= 0 && uiWetGrip <= 255)
                    {
                        pSurface->m_wetGrip = uiWetGrip;
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    else
                        argStream.SetCustomError("Expected valid wet grip ( 0 - 255 ) at argument 3");
                    break;
                case SURFACE_PROPERTY_ADHESIONGROUP:
                    eSurfaceAdhesionGroup eAdhesionGroup;
                    argStream.ReadEnumString(eAdhesionGroup);
                    if (!argStream.HasErrors())
                    {
                        pSurface->m_adhesionGroup = eAdhesionGroup;
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_CLIMBING:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(1, 9, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_WHEELEFFECT:
                    eSurfaceWheelEffect eWheelEffect;
                    argStream.ReadEnumString(eWheelEffect);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(1, SURFACE_WHEEL_EFFECT_DISABLED, false, 6);
                        if (eWheelEffect != SURFACE_WHEEL_EFFECT_DISABLED)
                            pSurface->setFlagEnabled(1, eWheelEffect, true);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_STAIRS:
                    argStream.ReadBool(bEnabled);
                    if (!argStream.HasErrors())
                    {
                        pSurface->setFlagEnabled(0, 20, bEnabled);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case SURFACE_PROPERTY_ROUGHNESS:
                    char cRoughness;
                    argStream.ReadNumber(cRoughness);
                    if (!argStream.HasErrors() && cRoughness >= 0 && cRoughness <= 3)
                    {
                        pSurface->m_roughness = cRoughness;
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    else
                        argStream.SetCustomError("Expected valid roughness ( 0 - 3 ) at argument 3");
                    break;
            }
        }
        else
            argStream.SetCustomError("Expected valid surface ID ( 0 - 179 ) at argument 1");
    }
    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
    }
    else
        lua_pushboolean(luaVM, false);
    return 1;
}

const char* cSurfaceAudio[8] = {"concrete", "grass", "sand", "gravel", "wood", "water", "metal", "longGrass"};
const char* cSurfaceStepEffect[2] = {"sand", "water"};
const char* cSurfaceBulletEffect[4] = {"metal", "sand", "wood", "concrete"};
const char* cSurfaceWheelEffect[5] = {"grass", "gravel", "mud", "sand", "dust"};
const char* cSurfaceSkidMark[3] = {"asphalt", "dirt", "dust"};
const char* cSurfaceAdhesionGroup[6] = {"rubber", "hard", "road", "loose", "sand", "wet"};

int CLuaEngineDefs::EngineGetSurfaceProperties(lua_State* luaVM)
{
    int                iSurfaceID;
    eSurfaceProperties eType;
    CScriptArgReader   argStream(luaVM);
    argStream.ReadNumber(iSurfaceID);
    argStream.ReadEnumString(eType);
    if (!argStream.HasErrors())
    {
        if (iSurfaceID >= EColSurfaceValue::DEFAULT && iSurfaceID <= EColSurfaceValue::SIZE)
        {
            CSurfaceType*  pSurfaceInfo = g_pGame->GetWorld()->GetSurfaceInfo();
            SurfaceInfo_c* pSurface = &pSurfaceInfo->surfType[iSurfaceID];
            switch (eType)
            {
                case SURFACE_PROPERTY_AUDIO:
                    for (char cFlag = SURFACE_AUDIO_CONCRETE; cFlag <= SURFACE_AUDIO_METAL; cFlag++)
                    {
                        if (pSurface->getFlagEnabled(1, cFlag))
                        {
                            lua_pushstring(luaVM, cSurfaceAudio[cFlag - SURFACE_AUDIO_CONCRETE]);
                            return 1;
                        }
                    }
                    lua_pushstring(luaVM, "disabled");
                    return 1;
                case SURFACE_PROPERTY_STEPWATERSPLASH:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(0, 16));
                    return 1;
                case SURFACE_PROPERTY_FOOTEFFECT:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(0, 29));
                    return 1;
                case SURFACE_PROPERTY_CREATEOBJECTS:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(1, 7));
                    return 1;
                case SURFACE_PROPERTY_CREATEPLANTS:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(1, 8));
                    return 1;
                case SURFACE_PROPERTY_BULLETEFFECT:
                    if (pSurface->m_bulletFx == 0)
                        lua_pushstring(luaVM, "disabled");
                    else
                        lua_pushstring(luaVM, cSurfaceBulletEffect[pSurface->m_bulletFx - 1]);

                    return 1;
                case SURFACE_PROPERTY_SHOOTTHROUGH:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(0, 14));
                    return 1;
                case SURFACE_PROPERTY_STEEPSLOPE:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(0, 18));
                    return 1;
                case SURFACE_PROPERTY_SEETHROUGH:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(0, 13));
                    return 1;
                case SURFACE_PROPERTY_GLASS:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(0, 19));
                    return 1;
                case SURFACE_PROPERTY_PAVEMENT:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(0, 22));
                    return 1;
                case SURFACE_PROPERTY_SOFTLANDING:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(0, 11));
                    return 1;
                case SURFACE_PROPERTY_SKIDMARKTYPE:
                    if (pSurface->m_skidmarkType == SURFACE_SKID_MARK_DISABLED)
                        lua_pushstring(luaVM, "disabled");
                    else
                        lua_pushstring(luaVM, cSurfaceSkidMark[pSurface->m_skidmarkType]);

                    return 1;
                case SURFACE_PROPERTY_FRACTIONEFFECT:
                    lua_pushnumber(luaVM, pSurface->m_frictionEffect);
                    return 1;
                case SURFACE_PROPERTY_TYREGRIP:
                    lua_pushnumber(luaVM, pSurface->m_tyreGrip);
                    return 1;
                case SURFACE_PROPERTY_WETGRIP:
                    lua_pushnumber(luaVM, pSurface->m_wetGrip);
                    return 1;
                case SURFACE_PROPERTY_ADHESIONGROUP:
                    lua_pushstring(luaVM, cSurfaceAdhesionGroup[pSurface->m_adhesionGroup]);
                    return 1;
                case SURFACE_PROPERTY_CLIMBING:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(1, 9));
                    return 1;
                case SURFACE_PROPERTY_WHEELEFFECT:
                    for (char cFlag = SURFACE_WHEEL_EFFECT_DISABLED; cFlag <= SURFACE_WHEEL_EFFECT_DUST; cFlag++)
                    {
                        if (pSurface->getFlagEnabled(1, cFlag))
                        {
                            lua_pushstring(luaVM, cSurfaceWheelEffect[cFlag - 1]);
                            return 1;
                        }
                    }

                    lua_pushstring(luaVM, "disabled");
                    return 1;
                case SURFACE_PROPERTY_STAIRS:
                    lua_pushboolean(luaVM, pSurface->getFlagEnabled(0, 20));
                    return 1;
                case SURFACE_PROPERTY_ROUGHNESS:
                    lua_pushnumber(luaVM, pSurface->m_roughness);
                    return 1;
            }
        }
        else
            argStream.SetCustomError("Expected valid surface ID ( 0 - 179 ) at argument 1");
    }
    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
    }
    else
        lua_pushboolean(luaVM, false);

    return 1;
}

int CLuaEngineDefs::EngineResetSurfaceProperties(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsNumber())
    {
        short sSurfaceID;
        argStream.ReadNumber(sSurfaceID);
        if (!argStream.HasErrors())
        {
            if (sSurfaceID >= EColSurfaceValue::DEFAULT && sSurfaceID <= EColSurfaceValue::SIZE)
            {
                lua_pushboolean(luaVM, CStaticFunctionDefinitions::ResetSurfaceInfo(sSurfaceID));
                return 1;
            }
            else
                argStream.SetCustomError("Expected valid surface ID ( 0 - 179 ) at argument 1");
        }
    }
    else
    {
        lua_pushboolean(luaVM, CStaticFunctionDefinitions::ResetAllSurfaceInfo());
        return 1;
    }
    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
    }
    else
        lua_pushboolean(luaVM, false);

    return 1;
}

int CLuaEngineDefs::EngineGetModelPhysicalPropertiesGroup(lua_State* luaVM)
{
    //  int engineGetModelPhysicalPropertiesGroup ( int modelID )
    int iModelID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iModelID);

    if (!argStream.HasErrors())
    {
        if (iModelID < 0 || iModelID >= g_pGame->GetBaseIDforTXD())
        {
            argStream.SetCustomError(SString("Expected model ID in range [0-%d] at argument 1", g_pGame->GetBaseIDforTXD() - 1));
            return luaL_error(luaVM, argStream.GetFullErrorMessage());
        }

        auto pModelInfo = g_pGame->GetModelInfo(iModelID);
        if (pModelInfo)
        {
            lua_pushnumber(luaVM, pModelInfo->GetObjectPropertiesGroup());
            return 1;
        }
        argStream.SetCustomError("Expected valid model ID at argument 1");
    }

    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaEngineDefs::EngineSetModelPhysicalPropertiesGroup(lua_State* luaVM)
{
    //  bool engineSetModelPhysicalPropertiesGroup ( int modelID, int newGroup )
    int          iModelID;
    unsigned int iNewGroup;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iModelID);
    argStream.ReadNumber(iNewGroup);

    if (!argStream.HasErrors())
    {
        if (iModelID < 0 || iModelID > g_pGame->GetBaseIDforTXD() - 1)
        {
            argStream.SetCustomError(SString("Expected model ID in range [0-%d] at argument 1", g_pGame->GetBaseIDforTXD() - 1));
            return luaL_error(luaVM, argStream.GetFullErrorMessage());
        }

        if (iNewGroup < 0 || iNewGroup > 159)
        {
            argStream.SetCustomError("Expected group ID in range [0-159] at argument 1");
            return luaL_error(luaVM, argStream.GetFullErrorMessage());
        }

        auto pModelInfo = g_pGame->GetModelInfo(iModelID);
        if (pModelInfo)
        {
            pModelInfo->SetObjectPropertiesGroup(iNewGroup);
            lua_pushboolean(luaVM, true);
            return 1;
        }
        argStream.SetCustomError("Expected valid model ID at argument 1");
    }

    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaEngineDefs::EngineRestoreModelPhysicalPropertiesGroup(lua_State* luaVM)
{
    //  bool engineRestoreModelPhysicalPropertiesGroup ( int modelID )
    int iModelID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iModelID);

    if (!argStream.HasErrors())
    {
        if (iModelID < 0 || iModelID > g_pGame->GetBaseIDforTXD() - 1)
        {
            argStream.SetCustomError(SString("Expected model ID in range [0-%d] at argument 1", g_pGame->GetBaseIDforTXD() - 1));
            return luaL_error(luaVM, argStream.GetFullErrorMessage());
        }

        auto pModelInfo = g_pGame->GetModelInfo(iModelID);
        if (pModelInfo)
        {
            pModelInfo->RestoreObjectPropertiesGroup();
            lua_pushboolean(luaVM, true);
            return 1;
        }
        argStream.SetCustomError("Expected valid model ID at argument 1");
    }

    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

std::unordered_map<eObjectGroup::Modifiable, std::function<void(CObjectGroupPhysicalProperties*, float)>> g_GroupPropertiesSettersFloat{
    {eObjectGroup::Modifiable::MASS,                [](CObjectGroupPhysicalProperties* pGroup, float fValue) { pGroup->SetMass(fValue); }},
    {eObjectGroup::Modifiable::TURNMASS,            [](CObjectGroupPhysicalProperties* pGroup, float fValue) { pGroup->SetTurnMass(fValue); }},
    {eObjectGroup::Modifiable::AIRRESISTANCE,       [](CObjectGroupPhysicalProperties* pGroup, float fValue) { pGroup->SetAirResistance(fValue); }},
    {eObjectGroup::Modifiable::ELASTICITY,          [](CObjectGroupPhysicalProperties* pGroup, float fValue) { pGroup->SetElasticity(fValue); }},
    {eObjectGroup::Modifiable::BUOYANCY,            [](CObjectGroupPhysicalProperties* pGroup, float fValue) { pGroup->SetBuoyancy(fValue); }},
    {eObjectGroup::Modifiable::UPROOTLIMIT,         [](CObjectGroupPhysicalProperties* pGroup, float fValue) { pGroup->SetUprootLimit(fValue); }},
    {eObjectGroup::Modifiable::COLDAMAGEMULTIPLIER, [](CObjectGroupPhysicalProperties* pGroup, float fValue) { pGroup->SetCollisionDamageMultiplier(fValue); }},
    {eObjectGroup::Modifiable::SMASHMULTIPLIER,     [](CObjectGroupPhysicalProperties* pGroup, float fValue) { pGroup->SetSmashMultiplier(fValue); }},
    {eObjectGroup::Modifiable::BREAKVELOCITYRAND,   [](CObjectGroupPhysicalProperties* pGroup, float fValue) { pGroup->SetBreakVelocityRandomness(fValue); }},
};
std::unordered_map<eObjectGroup::Modifiable, std::function<void(CObjectGroupPhysicalProperties*, bool)>> g_GroupPropertiesSettersBool{
    {eObjectGroup::Modifiable::CAMERAAVOID,    [](CObjectGroupPhysicalProperties* pGroup, bool bValue) { pGroup->SetCameraAvoidObject(bValue); }},
    {eObjectGroup::Modifiable::EXPLOSION,      [](CObjectGroupPhysicalProperties* pGroup, bool bValue) { pGroup->SetCausesExplosion(bValue); }},
    {eObjectGroup::Modifiable::SPARKSONIMPACT, [](CObjectGroupPhysicalProperties* pGroup, bool bValue) { pGroup->SetSparksOnImpact(bValue); }},
};
std::unordered_map<eObjectGroup::Modifiable, std::function<void(CObjectGroupPhysicalProperties*, CVector)>> g_GroupPropertiesSettersVector{
    {eObjectGroup::Modifiable::FXOFFSET,      [](CObjectGroupPhysicalProperties* pGroup, CVector vecValue) { pGroup->SetFxOffset(vecValue); }},
    {eObjectGroup::Modifiable::BREAKVELOCITY, [](CObjectGroupPhysicalProperties* pGroup, CVector vecValue) { pGroup->SetBreakVelocity(vecValue); }},
};

int CLuaEngineDefs::EngineSetObjectGroupPhysicalProperty(lua_State* luaVM)
{
    //  bool engineSetObjectGroupPhysicalProperty ( int groupID, string property, ...)
    int                      iGivenGroup;
    eObjectGroup::Modifiable eProperty;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iGivenGroup);
    argStream.ReadEnumString(eProperty);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (iGivenGroup < 0 || iGivenGroup > 159)
    {
        argStream.SetCustomError("Expected group ID in range [0-159] at argument 1");
        return luaL_error(luaVM, argStream.GetFullErrorMessage());
    }

    auto pGroup = g_pGame->GetObjectGroupPhysicalProperties(iGivenGroup);
    if (!pGroup)
    {
        argStream.SetCustomError("Expected valid group ID at argument 1");
        return luaL_error(luaVM, argStream.GetFullErrorMessage());
    }

    switch (eProperty)
    {
        case eObjectGroup::Modifiable::MASS:
        case eObjectGroup::Modifiable::TURNMASS:
        case eObjectGroup::Modifiable::AIRRESISTANCE:
        case eObjectGroup::Modifiable::ELASTICITY:
        case eObjectGroup::Modifiable::BUOYANCY:
        case eObjectGroup::Modifiable::UPROOTLIMIT:
        case eObjectGroup::Modifiable::COLDAMAGEMULTIPLIER:
        case eObjectGroup::Modifiable::SMASHMULTIPLIER:
        case eObjectGroup::Modifiable::BREAKVELOCITYRAND:
        {
            float fValue;
            argStream.ReadNumber(fValue);
            if (argStream.HasErrors())
                break;

            g_GroupPropertiesSettersFloat[eProperty](pGroup, fValue);
            lua_pushboolean(luaVM, true);
            return 1;
        }
        case eObjectGroup::Modifiable::CAMERAAVOID:
        case eObjectGroup::Modifiable::EXPLOSION:
        case eObjectGroup::Modifiable::SPARKSONIMPACT:
        {
            bool bValue;
            argStream.ReadBool(bValue);
            if (argStream.HasErrors())
                break;

            g_GroupPropertiesSettersBool[eProperty](pGroup, bValue);
            lua_pushboolean(luaVM, true);
            return 1;
        }
        case eObjectGroup::Modifiable::FXOFFSET:
        case eObjectGroup::Modifiable::BREAKVELOCITY:
        {
            CVector vecValue;
            argStream.ReadVector3D(vecValue);
            if (argStream.HasErrors())
                break;

            g_GroupPropertiesSettersVector[eProperty](pGroup, vecValue);
            lua_pushboolean(luaVM, true);
            return 1;
        }
        case eObjectGroup::Modifiable::COLDAMAGEEFFECT:
        {
            eObjectGroup::DamageEffect eDamEffect;
            argStream.ReadEnumString(eDamEffect);
            if (argStream.HasErrors())
                break;

            pGroup->SetCollisionDamageEffect(eDamEffect);
            lua_pushboolean(luaVM, true);
            return 1;
        }
        case eObjectGroup::Modifiable::SPECIALCOLRESPONSE:
        {
            eObjectGroup::CollisionResponse eColRepsonse;
            argStream.ReadEnumString(eColRepsonse);
            if (argStream.HasErrors())
                break;

            pGroup->SetCollisionSpecialResponseCase(eColRepsonse);
            lua_pushboolean(luaVM, true);
            return 1;
        }
        case eObjectGroup::Modifiable::FXTYPE:
        {
            eObjectGroup::FxType eFxType;
            argStream.ReadEnumString(eFxType);
            if (argStream.HasErrors())
                break;

            pGroup->SetFxType(eFxType);
            lua_pushboolean(luaVM, true);
            return 1;
        }
        case eObjectGroup::Modifiable::BREAKMODE:
        {
            eObjectGroup::BreakMode eBreakMode;
            argStream.ReadEnumString(eBreakMode);
            if (argStream.HasErrors())
                break;

            pGroup->SetBreakMode(eBreakMode);
            lua_pushboolean(luaVM, true);
            return 1;
        }
        case eObjectGroup::Modifiable::FXSYSTEM:
        {
            if (argStream.NextIsBool())
            {
                bool bRemove;
                argStream.ReadBool(bRemove);
                if (!bRemove)
                {
                    pGroup->RemoveFxParticleSystem();
                    lua_pushboolean(luaVM, true);
                    return 1;
                }

                argStream.SetCustomError("Expected either false, or valid fx system name at argument 3");
                break;
            }
            else
            {
                SString sName;
                argStream.ReadString(sName);
                if (argStream.HasErrors())
                    break;

                if (!g_pGame->GetFxManager()->IsValidFxSystemBlueprintName(sName))
                {
                    argStream.SetCustomError("Expected valid fx system name at argument 3");
                    break;
                }

                CFxSystemBPSAInterface* pBlueprint = g_pGame->GetFxManager()->GetFxSystemBlueprintByName(sName);
                if (pGroup->SetFxParticleSystem(pBlueprint))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                else
                {
                    argStream.SetCustomError("Given fx system isn't supported");
                    break;
                }
            }
        }
    }

    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

std::unordered_map<eObjectGroup::Modifiable, std::function<float(CObjectGroupPhysicalProperties*)>> g_GroupPropertiesGettersFloat{
    {eObjectGroup::Modifiable::MASS,                [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetMass(); }},
    {eObjectGroup::Modifiable::TURNMASS,            [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetTurnMass(); }},
    {eObjectGroup::Modifiable::AIRRESISTANCE,       [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetAirResistance(); }},
    {eObjectGroup::Modifiable::ELASTICITY,          [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetElasticity(); }},
    {eObjectGroup::Modifiable::BUOYANCY,            [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetBuoyancy(); }},
    {eObjectGroup::Modifiable::UPROOTLIMIT,         [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetUprootLimit(); }},
    {eObjectGroup::Modifiable::COLDAMAGEMULTIPLIER, [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetCollisionDamageMultiplier(); }},
    {eObjectGroup::Modifiable::SMASHMULTIPLIER,     [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetSmashMultiplier(); }},
    {eObjectGroup::Modifiable::BREAKVELOCITYRAND,   [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetBreakVelocityRandomness(); }},
};
std::unordered_map<eObjectGroup::Modifiable, std::function<bool(CObjectGroupPhysicalProperties*)>> g_GroupPropertiesGettersBool{
    {eObjectGroup::Modifiable::CAMERAAVOID,    [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetCameraAvoidObject(); }},
    {eObjectGroup::Modifiable::EXPLOSION,      [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetCausesExplosion(); }},
    {eObjectGroup::Modifiable::SPARKSONIMPACT, [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetSparksOnImpact(); }},
};
std::unordered_map<eObjectGroup::Modifiable, std::function<CVector(CObjectGroupPhysicalProperties*)>> g_GroupPropertiesGettersVector{
    {eObjectGroup::Modifiable::FXOFFSET,      [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetFxOffset(); }},
    {eObjectGroup::Modifiable::BREAKVELOCITY, [](CObjectGroupPhysicalProperties* pGroup) { return pGroup->GetBreakVelocity(); }},
};
int CLuaEngineDefs::EngineGetObjectGroupPhysicalProperty(lua_State* luaVM)
{
    //  bool engineGetObjectGroupPhysicalProperty ( int groupID, string property )
    int                      iGivenGroup;
    eObjectGroup::Modifiable eProperty;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iGivenGroup);
    argStream.ReadEnumString(eProperty);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (iGivenGroup < 0 || iGivenGroup > 159)
    {
        argStream.SetCustomError("Expected group ID in range [0-159] at argument 1");
        return luaL_error(luaVM, argStream.GetFullErrorMessage());
    }

    auto pGroup = g_pGame->GetObjectGroupPhysicalProperties(iGivenGroup);
    if (!pGroup)
    {
        argStream.SetCustomError("Expected valid group ID at argument 1");
        return luaL_error(luaVM, argStream.GetFullErrorMessage());
    }

    switch (eProperty)
    {
        case eObjectGroup::Modifiable::MASS:
        case eObjectGroup::Modifiable::TURNMASS:
        case eObjectGroup::Modifiable::AIRRESISTANCE:
        case eObjectGroup::Modifiable::ELASTICITY:
        case eObjectGroup::Modifiable::BUOYANCY:
        case eObjectGroup::Modifiable::UPROOTLIMIT:
        case eObjectGroup::Modifiable::COLDAMAGEMULTIPLIER:
        case eObjectGroup::Modifiable::SMASHMULTIPLIER:
        case eObjectGroup::Modifiable::BREAKVELOCITYRAND:
        {
            float fValue = g_GroupPropertiesGettersFloat[eProperty](pGroup);
            lua_pushnumber(luaVM, fValue);
            return 1;
        }
        case eObjectGroup::Modifiable::CAMERAAVOID:
        case eObjectGroup::Modifiable::EXPLOSION:
        case eObjectGroup::Modifiable::SPARKSONIMPACT:
        {
            bool bValue = g_GroupPropertiesGettersBool[eProperty](pGroup);
            lua_pushboolean(luaVM, bValue);
            return 1;
        }
        case eObjectGroup::Modifiable::FXOFFSET:
        case eObjectGroup::Modifiable::BREAKVELOCITY:
        {
            CVector vecValue = g_GroupPropertiesGettersVector[eProperty](pGroup);
            lua_pushvector(luaVM, vecValue);
            return 1;
        }
        case eObjectGroup::Modifiable::COLDAMAGEEFFECT:
        {
            eObjectGroup::DamageEffect eDamEffect = pGroup->GetCollisionDamageEffect();
            if (!EnumValueValid(eDamEffect))
                break;

            lua_pushstring(luaVM, EnumToString(eDamEffect));
            return 1;
        }
        case eObjectGroup::Modifiable::SPECIALCOLRESPONSE:
        {
            eObjectGroup::CollisionResponse eColRepsonse = pGroup->GetCollisionSpecialResponseCase();
            if (!EnumValueValid(eColRepsonse))
                break;

            lua_pushstring(luaVM, EnumToString(eColRepsonse));
            return 1;
        }
        case eObjectGroup::Modifiable::FXTYPE:
        {
            eObjectGroup::FxType eFxType = pGroup->GetFxType();
            if (!EnumValueValid(eFxType))
                break;

            lua_pushstring(luaVM, EnumToString(eFxType));
            return 1;
        }
        case eObjectGroup::Modifiable::BREAKMODE:
        {
            eObjectGroup::BreakMode eBreakMode = pGroup->GetBreakMode();
            if (!EnumValueValid(eBreakMode))
                break;

            lua_pushstring(luaVM, EnumToString(eBreakMode));
            return 1;
        }
        case eObjectGroup::Modifiable::FXSYSTEM:
        {
            argStream.SetCustomError("Fx system name isn't possible to be extracted.");
            break;
        }
    }

    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaEngineDefs::EngineRestoreObjectGroupPhysicalProperties(lua_State* luaVM)
{
    //  bool engineRestoreObjectGroupPhysicalProperties ( int groupID )
    int iGivenGroup;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iGivenGroup);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (iGivenGroup < 0 || iGivenGroup > 159)
    {
        argStream.SetCustomError("Expected group ID in range [0-159] at argument 1");
        return luaL_error(luaVM, argStream.GetFullErrorMessage());
    }

    auto pGroup = g_pGame->GetObjectGroupPhysicalProperties(iGivenGroup);
    if (!pGroup)
    {
        argStream.SetCustomError("Expected valid group ID at argument 1");
        return luaL_error(luaVM, argStream.GetFullErrorMessage());
    }

    pGroup->RestoreDefault();
    lua_pushboolean(luaVM, true);
    return 1;
}

bool CLuaEngineDefs::EngineRestreamWorld(lua_State* const luaVM)
{
    g_pClientGame->RestreamWorld();
    return true;
}
