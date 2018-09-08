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

void CLuaEngineDefs::LoadFunctions(void)
{
    CLuaCFunctions::AddFunction("engineLoadTXD", EngineLoadTXD);
    CLuaCFunctions::AddFunction("engineLoadCOL", EngineLoadCOL);
    CLuaCFunctions::AddFunction("engineLoadDFF", EngineLoadDFF);
    CLuaCFunctions::AddFunction("engineLoadIFP", EngineLoadIFP);
    CLuaCFunctions::AddFunction("engineImportTXD", EngineImportTXD);
    CLuaCFunctions::AddFunction("engineReplaceCOL", EngineReplaceCOL);
    CLuaCFunctions::AddFunction("engineRestoreCOL", EngineRestoreCOL);
    CLuaCFunctions::AddFunction("engineReplaceModel", EngineReplaceModel);
    CLuaCFunctions::AddFunction("engineRestoreModel", EngineRestoreModel);
    CLuaCFunctions::AddFunction("engineReplaceAnimation", EngineReplaceAnimation);
    CLuaCFunctions::AddFunction("engineRestoreAnimation", EngineRestoreAnimation);
    CLuaCFunctions::AddFunction("engineGetModelLODDistance", EngineGetModelLODDistance);
    CLuaCFunctions::AddFunction("engineSetModelLODDistance", EngineSetModelLODDistance);
    CLuaCFunctions::AddFunction("engineSetAsynchronousLoading", EngineSetAsynchronousLoading);
    CLuaCFunctions::AddFunction("engineApplyShaderToWorldTexture", EngineApplyShaderToWorldTexture);
    CLuaCFunctions::AddFunction("engineRemoveShaderFromWorldTexture", EngineRemoveShaderFromWorldTexture);
    CLuaCFunctions::AddFunction("engineGetModelNameFromID", EngineGetModelNameFromID);
    CLuaCFunctions::AddFunction("engineGetModelIDFromName", EngineGetModelIDFromName);
    CLuaCFunctions::AddFunction("engineGetModelTextureNames", EngineGetModelTextureNames);
    CLuaCFunctions::AddFunction("engineGetVisibleTextureNames", EngineGetVisibleTextureNames);
    CLuaCFunctions::AddFunction("engineGetSurfaceProperties", EngineGetSurfaceProperties);
    CLuaCFunctions::AddFunction("engineSetSurfaceProperties", EngineSetSurfaceProperties);

    // CLuaCFunctions::AddFunction ( "engineReplaceMatchingAtomics", EngineReplaceMatchingAtomics );
    // CLuaCFunctions::AddFunction ( "engineReplaceWheelAtomics", EngineReplaceWheelAtomics );
    // CLuaCFunctions::AddFunction ( "enginePositionAtomic", EnginePositionAtomic );
    // CLuaCFunctions::AddFunction ( "enginePositionSeats", EnginePositionSeats );
    // CLuaCFunctions::AddFunction ( "engineAddAllAtomics", EngineAddAllAtomics );
    // CLuaCFunctions::AddFunction ( "engineReplaceVehiclePart", EngineReplaceVehiclePart );
}

void CLuaEngineDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "restoreCOL", "engineRestoreCOL");
    lua_classfunction(luaVM, "restoreModel", "engineRestoreModel");
    lua_classfunction(luaVM, "setAsynchronousLoading", "engineSetAsynchronousLoading");
    lua_classfunction(luaVM, "setModelLODDistance", "engineSetModelLODDistance");

    lua_classfunction(luaVM, "getVisibleTextureNames", "engineGetVisibleTextureNames");
    lua_classfunction(luaVM, "getModelLODDistance", "engineGetModelLODDistance");
    lua_classfunction(luaVM, "getModelTextureNames", "engineGetModelTextureNames");
    lua_classfunction(luaVM, "getModelIDFromName", "engineGetModelIDFromName");
    lua_classfunction(luaVM, "getModelNameFromID", "engineGetModelNameFromID");

    //  lua_classvariable ( luaVM, "modelLODDistance", "engineSetModelLODDistance", "engineGetModelLODDistance" ); .modelLODDistance[model] = distance
    //  lua_classvariable ( luaVM, "modelNameFromID", NULL, "engineGetModelNameFromID" ); .modelNameFromID[id] = "name"
    //  lua_classvariable ( luaVM, "modelIDFromName", NULL, "engineGetModelIDFromName" ); .modelIDFromName["name"] = id
    //  lua_classvariable ( luaVM, "modelTextureNames", NULL, "engineGetModelTextureNames" ); .modelTextureNames[mode] = {names}

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
    SString          strFile = "";
    CScriptArgReader argStream(luaVM);
    // Grab the COL filename or data
    argStream.ReadString(strFile);

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
                bool    bIsRawData = CClientColModel::IsCOLData(strFile);
                SString strPath;
                // Is this a legal filepath?
                if (bIsRawData || CResourceManager::ParseResourcePathInput(strFile, pResource, &strPath))
                {
                    // Grab the resource root entity
                    CClientEntity* pRoot = pResource->GetResourceCOLModelRoot();

                    // Create the col model
                    CClientColModel* pCol = new CClientColModel(m_pManager, INVALID_ELEMENT_ID);

                    // Attempt loading the file
                    if (pCol->LoadCol(bIsRawData ? strFile : strPath, bIsRawData))
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
                        argStream.SetCustomError(bIsRawData ? "raw data" : strFile, "Error loading COL");
                    }
                }
                else
                    argStream.SetCustomError(bIsRawData ? "raw data" : strFile, "Bad file path");
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // We failed for some reason
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineLoadDFF(lua_State* luaVM)
{
    SString          strFile = "";
    CScriptArgReader argStream(luaVM);
    // Grab the DFF filename or data (model ID ignored after 1.3.1)
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
                bool    bIsRawData = CClientDFF::IsDFFData(strFile);
                SString strPath;
                // Is this a legal filepath?
                if (bIsRawData || CResourceManager::ParseResourcePathInput(strFile, pResource, &strPath))
                {
                    // Grab the resource root entity
                    CClientEntity* pRoot = pResource->GetResourceDFFRoot();

                    // Create a DFF element
                    CClientDFF* pDFF = new CClientDFF(m_pManager, INVALID_ELEMENT_ID);

                    // Try to load the DFF file
                    if (pDFF->LoadDFF(bIsRawData ? strFile : strPath, bIsRawData))
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
                        argStream.SetCustomError(bIsRawData ? "raw data" : strFile, "Error loading DFF");
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

int CLuaEngineDefs::EngineLoadTXD(lua_State* luaVM)
{
    SString          strFile = "";
    bool             bFilteringEnabled = true;
    CScriptArgReader argStream(luaVM);
    // Grab the TXD filename or data
    argStream.ReadString(strFile);
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
                bool    bIsRawData = CClientTXD::IsTXDData(strFile);
                SString strPath;
                // Is this a legal filepath?
                if (bIsRawData || CResourceManager::ParseResourcePathInput(strFile, pResource, &strPath))
                {
                    // Grab the resource root entity
                    CClientEntity* pRoot = pResource->GetResourceTXDRoot();

                    // Create a TXD element
                    CClientTXD* pTXD = new CClientTXD(m_pManager, INVALID_ELEMENT_ID);

                    // Try to load the TXD file
                    if (pTXD->LoadTXD(bIsRawData ? strFile : strPath, bFilteringEnabled, bIsRawData))
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
                        argStream.SetCustomError(bIsRawData ? "raw data" : strFile, "Error loading TXD");
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

int CLuaEngineDefs::EngineLoadIFP(lua_State* luaVM)
{
    SString strFile = "";
    SString strBlockName = "";

    CScriptArgReader argStream(luaVM);
    // Grab the IFP filename or data
    argStream.ReadString(strFile);
    argStream.ReadString(strBlockName);

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
                bool bIsRawData = CIFPEngine::IsIFPData(strFile);
                SString strPath;
                // Is this a legal filepath?
                if (bIsRawData || CResourceManager::ParseResourcePathInput(strFile, pResource, &strPath))
                {
                    std::shared_ptr<CClientIFP> pIFP = CIFPEngine::EngineLoadIFP(pResource, m_pManager, bIsRawData ? strFile : strPath, bIsRawData, strBlockName);
                    if (pIFP != nullptr)
                    {
                        // Return the IFP element
                        lua_pushelement(luaVM, pIFP.get());
                        return 1;
                    }
                    else
                        argStream.SetCustomError(bIsRawData ? "raw data" : strFile, "Error loading IFP");
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
    SString strModelId;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strModelId);

    if (!argStream.HasErrors())
    {
        ushort      usModelID = CModelNames::ResolveModelID(strModelId);
        CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModelID);
        if (pModelInfo)
        {
            float fDistance = pModelInfo->GetLODDistance();
            lua_pushnumber(luaVM, fDistance);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineSetModelLODDistance(lua_State* luaVM)
{
    SString          strModel = "";
    float            fDistance = 0.0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strModel);
    argStream.ReadNumber(fDistance);

    if (!argStream.HasErrors())
    {
        unsigned short usModelID = CModelNames::ResolveModelID(strModel);
        CModelInfo*    pModelInfo = g_pGame->GetModelInfo(usModelID);
        if (pModelInfo && fDistance > 0.0f)
        {
            pModelInfo->SetLODDistance(fDistance);
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineSetSurfaceProperties(lua_State* luaVM)
{
    int                 iSurfaceID;
    eSurfaceProperties  eType;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iSurfaceID);
    argStream.ReadEnumString(eType);

    if (!argStream.HasErrors())
    {
        if (iSurfaceID >= 0 && iSurfaceID <= 179)
        {
            CSurfaceType* pSurfaceInfo = m_pClientGame->GetSurfaceInfo();
            SurfaceInfo_c* pSurface = &pSurfaceInfo->surfType[iSurfaceID];
            
            bool bEnabled;
            switch (eType)
            {
            case SURFACE_PROPERTY_AUDIO:
                eSurfaceAudio eAudio;
                argStream.ReadEnumString(eAudio);
                if (!argStream.HasErrors())
                {
                    pSurface->setFlagEnabled(SURFACE_AUDIO_CONCRETE, false, 8);
                    if (eAudio != SURFACE_AUDIO_DISABLED)
                        pSurface->setFlagEnabled(1, eAudio, true);

                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                break;
            case SURFACE_PROPERTY_STEPEFFECT:
                eSurfaceStepEffect eStepEffect;
                argStream.ReadEnumString(eStepEffect);
                if (!argStream.HasErrors())
                {
                    pSurface->setFlagEnabled(0, SURFACE_STEP_EFFECT_SAND, false, 2);

                    if (eStepEffect != SURFACE_STEP_EFFECT_DISABLED)
                        pSurface->setFlagEnabled(0, eStepEffect, true);

                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                break;
            case SURFACE_PROPERTY_BULLETEFFECT:
                eSurfaceBulletEffect eBulletEffect;
                argStream.ReadEnumString(eBulletEffect);
                if (!argStream.HasErrors())
                {

                    if (eBulletEffect == SURFACE_BULLET_EFFECT_DISABLED)
                        pSurface->m_bulletFx = 0;
                    else
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

            case SURFACE_PROPERTY_SKIDMARKTYPE:
                short sSkidMarkType;
                argStream.ReadNumber(sSkidMarkType);
                if (!argStream.HasErrors() && sSkidMarkType >= 0 && sSkidMarkType < 4)
                {
                    pSurface->m_skidmarkType = sSkidMarkType;
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                break;
            case SURFACE_PROPERTY_FRACTIONEFFECT:
                short sFractionEffect;
                argStream.ReadNumber(sFractionEffect);
                if (!argStream.HasErrors() && sFractionEffect > 0 && sFractionEffect < 8)
                {
                    pSurface->m_frictionEffect = sFractionEffect;
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                break;

            case SURFACE_PROPERTY_TYREGRIP:
                uint uiTyreGrip;
                argStream.ReadNumber(uiTyreGrip);
                if (!argStream.HasErrors() && uiTyreGrip >= 0 && uiTyreGrip < 10000)
                {
                    pSurface->m_tyreGrip = uiTyreGrip;
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                break;
            case SURFACE_PROPERTY_WETGRIP:
                uint uiWetGrip;
                argStream.ReadNumber(uiWetGrip);
                if (!argStream.HasErrors() && uiWetGrip >= 0 && uiWetGrip < 10000)
                {
                    pSurface->m_wetGrip = uiWetGrip;
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                break;
            case SURFACE_PROPERTY_ADHESIONGROUP:
                int fAdhesionGroup;
                argStream.ReadNumber(fAdhesionGroup);
                if (!argStream.HasErrors() && fAdhesionGroup >= 0 && fAdhesionGroup < 8)
                {
                    pSurface->m_adhesionGroup = fAdhesionGroup;
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                break;
            case SURFACE_PROPERTY_CLIMBING:
                argStream.ReadBool(bEnabled);
                if (!argStream.HasErrors())
                {
                    pSurface->setFlagEnabled(0, 9, bEnabled);
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                break;
            case SURFACE_PROPERTY_WHEELEFFECT:
                eSurfaceWheelEffect eWheelEffect;
                argStream.ReadEnumString(eWheelEffect);
                if (!argStream.HasErrors())
                {
                    pSurface->setFlagEnabled(1, 1, false, 4);
                    //surface->setFlag2Enabled(6, false); -- sand
                    //surface->setFlag2Enabled(7, false); -- spray
                    if (eWheelEffect != SURFACE_WHEEL_EFFECT_DISABLED)
                        pSurface->setFlagEnabled(1, 1, eWheelEffect, true);

                    lua_pushboolean(luaVM, true);
                    return 1;
                }
                break;
            }


        }
        else
            argStream.SetCustomError("Expected valid surface ID ( 0 - 179 ) at argument 1");
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, true);
    return 1;
}

const char* cSurfaceAudio[8] = { "concrete", "grass", "sand", "gravel", "wood", "water", "metal", "longGrass" };
const char* cSurfaceStepEffect[2] = { "sand", "water" };
const char* cSurfaceBulletEffect[4] = { "metal", "concrete", "sand", "wood" };

int CLuaEngineDefs::EngineGetSurfaceProperties(lua_State* luaVM)
{
    int                 iSurfaceID;
    eSurfaceProperties  eType;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iSurfaceID);
    argStream.ReadEnumString(eType);

    if (!argStream.HasErrors())
    {
        if (iSurfaceID >= 0 && iSurfaceID <= 179)
        {
            CSurfaceType* pSurfaceInfo = m_pClientGame->GetSurfaceInfo();
            SurfaceInfo_c* pSurface = &pSurfaceInfo->surfType[iSurfaceID];
            switch (eType)
            {
            case SURFACE_PROPERTY_AUDIO:
                for (char cFlag = SURFACE_AUDIO_CONCRETE; cFlag < SURFACE_AUDIO_DISABLED; cFlag++)
                {
                    if (pSurface->getFlagEnabled(1, cFlag))
                    {
                        lua_pushstring(luaVM, cSurfaceAudio[cFlag - SURFACE_AUDIO_CONCRETE]);
                        return 1;
                    }
                }
                lua_pushstring(luaVM, "disabled");
                return 1;
                break;
            case SURFACE_PROPERTY_STEPEFFECT:
                for (char cFlag = SURFACE_STEP_EFFECT_SAND; cFlag < SURFACE_STEP_EFFECT_DISABLED; cFlag++)
                {
                    if (pSurface->getFlagEnabled(0, cFlag))
                    {
                        lua_pushstring(luaVM, cSurfaceStepEffect[cFlag - SURFACE_STEP_EFFECT_SAND]);
                        return 1;
                    }
                }
                lua_pushstring(luaVM, "disabled");
                return 1;
                break;
            case SURFACE_PROPERTY_BULLETEFFECT:
                if (pSurface->m_bulletFx == 0)
                {
                    lua_pushstring(luaVM, "disabled");
                    return 1;
                }
                else
                {
                    lua_pushstring(luaVM, cSurfaceBulletEffect[pSurface->m_bulletFx]);
                    return 1;
                }
                break;
            case SURFACE_PROPERTY_SHOOTTHROUGH:
                lua_pushboolean(luaVM, pSurface->getFlagEnabled(0, 14));
                return 1;
                break;
            case SURFACE_PROPERTY_SEETHROUGH:
                lua_pushboolean(luaVM, pSurface->getFlagEnabled(0, 13));
                return 1;
                break;
            case SURFACE_PROPERTY_SKIDMARKTYPE:
                lua_pushnumber(luaVM, 99999);
                return 1;
                break;
            case SURFACE_PROPERTY_FRACTIONEFFECT:
                lua_pushnumber(luaVM, 99999);
                return 1;
                break;
            case SURFACE_PROPERTY_TYREGRIP:
                lua_pushnumber(luaVM, pSurface->m_tyreGrip);
                return 1;
                break;
            case SURFACE_PROPERTY_WETGRIP:
                lua_pushnumber(luaVM, pSurface->m_wetGrip);
                return 1;
                break;
            case SURFACE_PROPERTY_ADHESIONGROUP:
                lua_pushnumber(luaVM, pSurface->m_adhesionGroup);
                return 1;
                break;
            case SURFACE_PROPERTY_CLIMBING:
                lua_pushboolean(luaVM, pSurface->getFlagEnabled(1, 10));
                return 1;
                break;
            case SURFACE_PROPERTY_WHEELEFFECT:
                lua_pushnumber(luaVM, 99999);
                return 1;
                break;
            }

        }
        else
            argStream.SetCustomError("Expected valid surface ID ( 0 - 179 ) at argument 1");
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    
    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaEngineDefs::EngineResetSurfaceProperties(lua_State* luaVM)
{

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
