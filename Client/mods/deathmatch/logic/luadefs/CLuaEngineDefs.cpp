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

#include "../Client/game_sa/CModelInfoSA.h"
#include "../Client/game_sa/CColModelSA.h"
CBaseModelInfoSAInterface** ppModelInfo = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;

void CLuaEngineDefs::LoadFunctions(void)
{
    std::map<const char*, lua_CFunction> functions{
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
        {"engineGetModelLODDistance", EngineGetModelLODDistance},
        {"engineSetModelLODDistance", EngineSetModelLODDistance},
        {"engineSetAsynchronousLoading", EngineSetAsynchronousLoading},
        {"engineApplyShaderToWorldTexture", EngineApplyShaderToWorldTexture},
        {"engineRemoveShaderFromWorldTexture", EngineRemoveShaderFromWorldTexture},
        {"engineGetModelNameFromID", EngineGetModelNameFromID},
        {"engineGetModelIDFromName", EngineGetModelIDFromName},
        {"engineGetModelTextureNames", EngineGetModelTextureNames},
        {"engineGetVisibleTextureNames", EngineGetVisibleTextureNames},
        {"engineGetModelCollisionProperties", EngineGetModelCollisionProperties },
        {"engineGetModelCollisionData", EngineGetModelCollisionData},
        {"engineSetModelCollisionData", EngineSetModelCollisionData},
        {"engineUpdateModelCollisionBoundingBox", EngineUpdateModelCollisionBoundingBox },
        {"isModelCollisionLoaded", IsModelCollisionLoaded },

        // CLuaCFunctions::AddFunction ( "engineReplaceMatchingAtomics", EngineReplaceMatchingAtomics );
        // CLuaCFunctions::AddFunction ( "engineReplaceWheelAtomics", EngineReplaceWheelAtomics );
        // CLuaCFunctions::AddFunction ( "enginePositionAtomic", EnginePositionAtomic );
        // CLuaCFunctions::AddFunction ( "enginePositionSeats", EnginePositionSeats );
        // CLuaCFunctions::AddFunction ( "engineAddAllAtomics", EngineAddAllAtomics );
        // CLuaCFunctions::AddFunction ( "engineReplaceVehiclePart", EngineReplaceVehiclePart );
    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
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

CColModelSAInterface* GetModelCollisionInterface(ushort usModel)
{
    if (CClientObjectManager::IsValidModel(usModel))
    {
        CBaseModelInfoSAInterface* pModelInfo = ppModelInfo[usModel];
        if (pModelInfo != nullptr)
        {
            CColModelSAInterface* pColModelInterface = pModelInfo->pColModel;
            if (pColModelInterface)
            {
                return pColModelInterface;
            }
        }
    }
    return false;
}

int CLuaEngineDefs::EngineGetModelCollisionProperties(lua_State* luaVM)
{
    CClientColModel* pCol = NULL;
    unsigned short   usModel = 0;
    CScriptArgReader argStream(luaVM);
    // Grab the COL or model ID
    // If COL element then read custom collision, otherwise read original collision
    if( argStream.NextIsNumber())
        argStream.ReadNumber(usModel);
    else
        argStream.ReadUserData(pCol);

    if (!argStream.HasErrors())
    {
        CColModelSAInterface* pColModelSAInterface;
        if (pCol)
            pColModelSAInterface = pCol->GetColModelInterface();
        else
            pColModelSAInterface = GetModelCollisionInterface(usModel);

        if (pColModelSAInterface)
        {
            lua_newtable(luaVM);
            lua_pushstring(luaVM, "boundingBox");
            lua_newtable(luaVM);
            lua_pushstring(luaVM, "vecMin");
            lua_newtable(luaVM);
            lua_pushtablevalue(luaVM, 1, pColModelSAInterface->boundingBox.vecMin.fX);
            lua_pushtablevalue(luaVM, 2, pColModelSAInterface->boundingBox.vecMin.fY);
            lua_pushtablevalue(luaVM, 3, pColModelSAInterface->boundingBox.vecMin.fZ);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "vecMax");
            lua_newtable(luaVM);
            lua_pushtablevalue(luaVM, 1, pColModelSAInterface->boundingBox.vecMax.fX);
            lua_pushtablevalue(luaVM, 2, pColModelSAInterface->boundingBox.vecMax.fY);
            lua_pushtablevalue(luaVM, 3, pColModelSAInterface->boundingBox.vecMax.fZ);
            lua_settable(luaVM, -3);
            lua_pushstring(luaVM, "vecOffset");
            lua_newtable(luaVM);
            lua_pushtablevalue(luaVM, 1, pColModelSAInterface->boundingBox.vecOffset.fX);
            lua_pushtablevalue(luaVM, 2, pColModelSAInterface->boundingBox.vecOffset.fY);
            lua_pushtablevalue(luaVM, 3, pColModelSAInterface->boundingBox.vecOffset.fZ);
            lua_settable(luaVM, -3);
            lua_pushstring(luaVM, "radius");
            lua_pushnumber(luaVM, pColModelSAInterface->boundingBox.fRadius);
            lua_settable(luaVM, -3);

            CColDataSA* pColData = pColModelSAInterface->pColData;
            if (pColData != nullptr)
            {
                lua_settable(luaVM, -3);
                lua_pushstring(luaVM, "colBoxes");
                lua_pushnumber(luaVM, pColData->numColBoxes);
                lua_settable(luaVM, -3);
                lua_pushstring(luaVM, "colSpheres");
                lua_pushnumber(luaVM, pColData->numColSpheres);
                lua_settable(luaVM, -3);
                lua_pushstring(luaVM, "colTriangles");
                lua_pushnumber(luaVM, pColData->numColTriangles);
                lua_settable(luaVM, -3);
                lua_pushstring(luaVM, "colVertices");
                lua_pushnumber(luaVM, pColData->getNumVertices());
                lua_settable(luaVM, -3);
            }

            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::IsModelCollisionLoaded(lua_State* luaVM)
{
    ushort usModel;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usModel);
    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, GetModelCollisionInterface(usModel) != nullptr);
        return 1;
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaEngineDefs::EngineGetModelCollisionData(lua_State* luaVM)
{
    CClientColModel* pCol = NULL;
    unsigned short   usModel = 0;
    CScriptArgReader argStream(luaVM);
    // Grab the COL or model ID
    // If COL element then read custom collision, otherwise read original collision
    if (argStream.NextIsNumber())
        argStream.ReadNumber(usModel);
    else
        argStream.ReadUserData(pCol);

    if (!argStream.HasErrors())
    {
        CColModelSAInterface* pColModelSAInterface;
        if (pCol)
            pColModelSAInterface = pCol->GetColModelInterface();
        else
            pColModelSAInterface = GetModelCollisionInterface(usModel);

        if (pColModelSAInterface)
        {

            ushort usIndex;
            eCollisionShapes eCollisionShape;
            argStream.ReadEnumString(eCollisionShape);
            argStream.ReadNumber(usIndex, 0);
            CColDataSA* pColData = pColModelSAInterface->pColData;
            if (pColData)
            {
                switch (eCollisionShape)
                {
                case COLLISION_BOX:
                    if (usIndex > 0)
                    {
                        usIndex--;
                        if (pColData->isValidIndex(COLLISION_BOX, usIndex))
                        {
                            CColBoxSA pBox = pColData->pColBoxes[usIndex];
                            lua_newtable(luaVM);
                            lua_pushtablevalue(luaVM, 1, pBox.min.fX);
                            lua_pushtablevalue(luaVM, 2, pBox.min.fY);
                            lua_pushtablevalue(luaVM, 3, pBox.min.fZ);
                            lua_pushtablevalue(luaVM, 4, pBox.max.fX);
                            lua_pushtablevalue(luaVM, 5, pBox.max.fY);
                            lua_pushtablevalue(luaVM, 6, pBox.max.fZ);
                            lua_pushtablevalue(luaVM, 7, pBox.material);
                            lua_pushtablevalue(luaVM, 8, pBox.lighting.day);
                            lua_pushtablevalue(luaVM, 9, pBox.lighting.night);
                            lua_settable(luaVM, -3);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("Invalid box index.");
                            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                            lua_pushboolean(luaVM, false);
                        }
                    }
                    else
                    {
                        lua_newtable(luaVM);
                        for (uint i = 0; pColData->numColBoxes > i; i++)
                        {
                            lua_pushnumber(luaVM, i + 1);
                            CColBoxSA pBox = pColData->pColBoxes[i];
                            lua_newtable(luaVM);
                            lua_pushtablevalue(luaVM, 1, pBox.min.fX);
                            lua_pushtablevalue(luaVM, 2, pBox.min.fY);
                            lua_pushtablevalue(luaVM, 3, pBox.min.fZ);
                            lua_pushtablevalue(luaVM, 4, pBox.max.fX);
                            lua_pushtablevalue(luaVM, 5, pBox.max.fY);
                            lua_pushtablevalue(luaVM, 6, pBox.max.fZ);
                            lua_pushtablevalue(luaVM, 7, pBox.material);
                            lua_pushtablevalue(luaVM, 8, pBox.lighting.day);
                            lua_pushtablevalue(luaVM, 9, pBox.lighting.night);
                            lua_settable(luaVM, -3);
                        }
                        return 1;
                    }
                    break;
                case COLLISION_SPHERE:
                    if (usIndex > 0)
                    {
                        usIndex--;
                        if (pColData->isValidIndex(COLLISION_SPHERE, usIndex))
                        {
                            CColSphereSA pSphere = pColData->pColSpheres[usIndex];
                            lua_newtable(luaVM);
                            lua_pushtablevalue(luaVM, 1, pSphere.vecCenter.fX);
                            lua_pushtablevalue(luaVM, 2, pSphere.vecCenter.fY);
                            lua_pushtablevalue(luaVM, 3, pSphere.vecCenter.fZ);
                            lua_pushtablevalue(luaVM, 4, pSphere.fRadius);
                            lua_pushtablevalue(luaVM, 5, pSphere.material);
                            lua_pushtablevalue(luaVM, 6, pSphere.lighting.day);
                            lua_pushtablevalue(luaVM, 7, pSphere.lighting.night);
                            lua_settable(luaVM, -3);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("Invalid sphere index.");
                            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                    }
                    else
                    {
                        lua_newtable(luaVM);
                        CColSphereSA pSphere;
                        for (uint i = 0; pColData->numColSpheres > i; i++)
                        {
                            lua_pushnumber(luaVM, i + 1);
                            pSphere = pColData->pColSpheres[i];
                            lua_newtable(luaVM);
                            lua_pushtablevalue(luaVM, 1, pSphere.vecCenter.fX);
                            lua_pushtablevalue(luaVM, 2, pSphere.vecCenter.fY);
                            lua_pushtablevalue(luaVM, 3, pSphere.vecCenter.fZ);
                            lua_pushtablevalue(luaVM, 4, pSphere.fRadius);
                            lua_pushtablevalue(luaVM, 5, pSphere.material);
                            lua_pushtablevalue(luaVM, 6, pSphere.lighting.day);
                            lua_pushtablevalue(luaVM, 7, pSphere.lighting.night);
                            lua_settable(luaVM, -3);
                        }
                        return 1;
                    }
                    break;
                case COLLISION_TRIANGLE:
                    if (usIndex > 0)
                    {
                        usIndex--;
                        if (pColData->isValidIndex(COLLISION_TRIANGLE, usIndex))
                        {
                            CColTriangleSA pTriangle = pColData->pColTriangles[usIndex];
                            lua_newtable(luaVM);
                            lua_pushtablevalue(luaVM, 1, pTriangle.vertex[0] + 1);
                            lua_pushtablevalue(luaVM, 2, pTriangle.vertex[1] + 1);
                            lua_pushtablevalue(luaVM, 3, pTriangle.vertex[2] + 1);
                            lua_pushtablevalue(luaVM, 4, pTriangle.material);
                            lua_pushtablevalue(luaVM, 5, pTriangle.lighting.day);
                            lua_pushtablevalue(luaVM, 6, pTriangle.lighting.night);
                            lua_settable(luaVM, -3);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("Invalid triangle index.");
                            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                    }
                    else
                    {
                        lua_newtable(luaVM);
                        CColTriangleSA pTriangle;
                        for (uint i = 0; pColData->numColTriangles > i; i++)
                        {
                            lua_pushnumber(luaVM, i + 1);
                            pTriangle = pColData->pColTriangles[i];
                            lua_newtable(luaVM);
                            lua_pushtablevalue(luaVM, 1, pTriangle.vertex[0] + 1);
                            lua_pushtablevalue(luaVM, 2, pTriangle.vertex[1] + 1);
                            lua_pushtablevalue(luaVM, 3, pTriangle.vertex[2] + 1);
                            lua_pushtablevalue(luaVM, 4, pTriangle.material);
                            lua_pushtablevalue(luaVM, 5, pTriangle.lighting.day);
                            lua_pushtablevalue(luaVM, 6, pTriangle.lighting.night);
                            lua_settable(luaVM, -3);
                        }
                        return 1;
                    }
                    break;
                case COLLISION_VERTEX:
                    if (usIndex > 0)
                    {
                        usIndex--;
                        if (pColData->isValidIndex(COLLISION_VERTEX, usIndex))
                        {
                            CompressedVector pTriangle = pColData->pVertices[usIndex];
                            lua_newtable(luaVM);
                            CVector vec = pTriangle.getVector();
                            lua_pushtablevalue(luaVM, 1, vec.fX);
                            lua_pushtablevalue(luaVM, 2, vec.fY);
                            lua_pushtablevalue(luaVM, 3, vec.fZ);
                            lua_settable(luaVM, -3);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("Invalid vertex index.");
                            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }
                    }
                    else
                    {
                        std::map<ushort, CompressedVector> vecVertices = pColData->getAllVertices();
                        lua_newtable(luaVM);

                        std::map<ushort, CompressedVector>::iterator it;
                        CVector vec;
                        for (it = vecVertices.begin(); it != vecVertices.end(); it++)
                        {
                            lua_pushnumber(luaVM, it->first + 1);
                            lua_newtable(luaVM);
                            vec = it->second.getVector();
                            lua_pushtablevalue(luaVM, 1, vec.fX);
                            lua_pushtablevalue(luaVM, 2, vec.fY);
                            lua_pushtablevalue(luaVM, 3, vec.fZ);
                            lua_settable(luaVM, -3);
                        }
                        return 1;
                    }
                    break;
                }
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineUpdateModelCollisionBoundingBox(lua_State* luaVM)
{
    CClientColModel* pCol = NULL;
    CVector vecCenter;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pCol);
    argStream.ReadVector3D(vecCenter, CVector(0, 0, 0));

    if (!argStream.HasErrors())
    {
        CColModelSAInterface* pColModelSAInterface;
        if (pCol)
            pColModelSAInterface = pCol->GetColModelInterface();

        if (pColModelSAInterface)
        {
            CColDataSA* pColData = pColModelSAInterface->pColData;
            CBoundingBoxSA* pBoundingBox = &pColModelSAInterface->boundingBox;
            float fRadius = 0;
            CVector minVec(0, 0, 0);
            CVector maxVec(0, 0, 0);
            CVector pBoxMinVec, pBoxMaxVec;

            CColSphereSA pSphere;
            CColBoxSA pBox;
            float fDis;
            for (uint i = 0; pColData->numColBoxes > i; i++)
            {
                pBox = pColData->pColBoxes[i];
                pBoxMaxVec = pBox.max;
                pBoxMinVec = pBox.min;
                fRadius = std::max(DistanceBetweenPoints3D(pBoxMaxVec, vecCenter), fRadius);

                CClientColModel::AlignVector(maxVec, minVec, pBoxMinVec);
                CClientColModel::AlignVector(maxVec, minVec, pBoxMaxVec);
            }

            for (uint i = 0; pColData->numColSpheres > i; i++)
            {
                pSphere = pColData->pColSpheres[i];
                pBoxMaxVec = pSphere.vecCenter;
                CClientColModel::AlignVector(maxVec, minVec, pBoxMaxVec);
                pBoxMaxVec.fX += pSphere.fRadius;
                pBoxMaxVec.fY += pSphere.fRadius;
                pBoxMaxVec.fZ += pSphere.fRadius;
                CClientColModel::AlignVector(maxVec, minVec, pBoxMaxVec);
                fRadius = std::max(DistanceBetweenPoints3D(pBoxMaxVec, vecCenter), fRadius);

                pBoxMaxVec.fX -= pSphere.fRadius * 2;
                pBoxMaxVec.fY -= pSphere.fRadius * 2;
                pBoxMaxVec.fZ -= pSphere.fRadius * 2;
                CClientColModel::AlignVector(maxVec, minVec, pBoxMaxVec);
                fRadius = std::max(DistanceBetweenPoints3D(pBoxMaxVec, vecCenter), fRadius);
            }

            std::map<ushort, CompressedVector> vecVertices = pColData->getAllVertices();
            std::map<ushort, CompressedVector>::iterator it;
            for (it = vecVertices.begin(); it != vecVertices.end(); it++)
            {
                pBoxMaxVec = it->second.getVector();
                CClientColModel::AlignVector(maxVec, minVec, pBoxMaxVec);
                fRadius = std::max(DistanceBetweenPoints3D(pBoxMaxVec, vecCenter), fRadius);
            }
            pBoundingBox->fRadius = fRadius;
            pBoundingBox->vecOffset = vecCenter;
            pBoundingBox->vecMax = maxVec;
            pBoundingBox->vecMin = minVec;
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    lua_pushboolean(luaVM, false);
    return 1;
}
int CLuaEngineDefs::EngineSetModelCollisionData(lua_State* luaVM)
{
    CClientColModel* pCol = NULL;
    ushort usShapeId;
    std::vector<ushort> vecShapeId;
    eCollisionShapes eCollisionShape;
    eCollisionKey eCollisionKey;
    CVector vec1, vec2, vec3;
    uchar cSurface;
    float fNumber;
    bool bBool;
    uchar cDay,cNight;
    ushort sVertex[3];
    float fPosition[3];
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pCol);
    argStream.ReadEnumString(eCollisionShape);
    argStream.ReadEnumString(eCollisionKey);
    if (argStream.NextIsNumber())
    {
        argStream.ReadNumber(usShapeId);
        vecShapeId.push_back(usShapeId);
    }
    else
        argStream.ReadNumberTable(vecShapeId);


    if (!argStream.HasErrors())
    {
        CColModelSAInterface* pColModelSAInterface;
        if (pCol)
            pColModelSAInterface = pCol->GetColModelInterface();

        if (pColModelSAInterface)
        {
            CColDataSA* pColData = pColModelSAInterface->pColData;
            ushort numVertices = pColData->getNumVertices();
            for (int i = 0; i != vecShapeId.size(); i++) {
                vecShapeId[i]--;
                if (!pColData->isValidIndex(eCollisionShape, vecShapeId[i], numVertices))
                {
                    lua_pushboolean(luaVM, false);
                    lua_pushnumber(luaVM, vecShapeId[i]); // return which is invalid
                    return 2;
                }
            }

            switch (eCollisionShape)
            {
                case COLLISION_BOX:
                    CColBoxSA* pBox;
                    switch (eCollisionKey)
                    {
                    case COLLISION_KEY_MATERIAL:
                        argStream.ReadNumber(cSurface);
                        if (!argStream.HasErrors())
                        {
                            if (cSurface >= EColSurfaceValue::DEFAULT && cSurface <= EColSurfaceValue::RAILTRACK)
                            {
                                for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                {
                                    pBox = &pColData->pColBoxes[*it];
                                    pBox->material = cSurface;
                                }
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                        }
                    case COLLISION_KEY_SIZE:
                        if (argStream.NextIsBool())
                        {
                            argStream.ReadBool(bBool); // true = min, false = max
                            argStream.ReadVector3D(vec1);
                            if (!argStream.HasErrors())
                            {
                                if (CClientColModel::CheckVector(vec1))
                                {
                                    if (bBool)
                                    {
                                        for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                        {
                                            pBox = &pColData->pColBoxes[*it];
                                            if (CClientColModel::CompareVector(vec1, pBox->max))
                                            {
                                                pBox->min = vec1;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                        {
                                            pBox = &pColData->pColBoxes[*it];
                                            if (CClientColModel::CompareVector(pBox->min, vec1))
                                            {
                                                pBox->max = vec1;
                                            }
                                        }
                                    }
                                    lua_pushboolean(luaVM, true);
                                    return 1;
                                }
                            }
                        }
                        else
                        {
                            argStream.ReadVector3D(vec1);
                            argStream.ReadVector3D(vec2);
                            if (!argStream.HasErrors())
                            {
                                if (CClientColModel::CheckVector(vec1) && CClientColModel::CheckVector(vec2))
                                {
                                    if (CClientColModel::CompareVector(vec1, vec2))
                                    {
                                        for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                        {
                                            pBox = &pColData->pColBoxes[*it];
                                            pBox->min = vec1;
                                            pBox->max = vec2;
                                        }
                                        lua_pushboolean(luaVM, true);
                                        return 1;
                                    }
                                }
                            }
                        }
                    case COLLISION_KEY_POSITION:
                        argStream.ReadVector3D(vec1);
                        if (!argStream.HasErrors())
                        {
                            for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                            {
                                pBox = &pColData->pColBoxes[*it];
                                vec2 = CVector(pBox->min.fX + pBox->max.fX, pBox->min.fY + pBox->max.fY, pBox->min.fZ + pBox->max.fZ) / 2;
                                vec3 = pBox->min + (vec1 - vec2);
                                if (CClientColModel::CheckVector(vec3))
                                {
                                    if (CClientColModel::CheckVector(pBox->max + (vec1 - vec2)))
                                    {
                                        pBox->min = vec3;
                                        pBox->max += (vec1 - vec2);
                                    }
                                }
                            }
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        break;
                    case COLLISION_KEY_MOVE:
                        argStream.ReadVector3D(vec1);
                        if (!argStream.HasErrors())
                        {
                            for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                            {
                                pBox = &pColData->pColBoxes[*it];
                                if (CClientColModel::CheckVector(pBox->min + vec1) && CClientColModel::CheckVector(pBox->max + vec1))
                                {
                                    pBox->min += vec1;
                                    pBox->max += vec1;
                                }
                            }
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        break;

                    }
                break;
                case COLLISION_SPHERE:
                    CColSphereSA* pSphere;
                    switch (eCollisionKey)
                    {
                    case COLLISION_KEY_POSITION:
                        argStream.ReadVector3D(vec1);
                        if (!argStream.HasErrors())
                        {
                            if (CClientColModel::CheckVector(vec1))
                            {
                                for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                {
                                    ushort val = *it;
                                    pSphere = &pColData->pColSpheres[*it];
                                    if (CClientColModel::CheckVector(vec1, pSphere->fRadius))
                                    {
                                        pSphere->vecCenter = vec1;
                                    }
                                }
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                        }
                    case COLLISION_KEY_MOVE:
                        argStream.ReadVector3D(vec1);
                        if (!argStream.HasErrors())
                        {
                            for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                            {
                                pSphere = &pColData->pColSpheres[*it];
                                if (CClientColModel::CheckVector(pSphere->vecCenter + vec1, pSphere->fRadius))
                                    pSphere->vecCenter += vec1;
                            }
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    case COLLISION_KEY_RADIUS:
                        argStream.ReadNumber(fNumber);
                        if (!argStream.HasErrors())
                        {
                            if (fNumber >= 0 && fNumber <= 256) // bigger than 256 are sure that are outside bounding
                            {
                                for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                {
                                    pSphere = &pColData->pColSpheres[*it];
                                    if (CClientColModel::CheckVector(pSphere->vecCenter, fNumber))
                                        pSphere->fRadius = fNumber;
                                }
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                        }

                        break;
                    case COLLISION_KEY_MATERIAL:
                        argStream.ReadNumber(cSurface);
                        if (!argStream.HasErrors())
                        {
                            if (cSurface >= EColSurfaceValue::DEFAULT && cSurface <= EColSurfaceValue::RAILTRACK)
                            {
                                for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                {
                                    pSphere = &pColData->pColSpheres[*it];
                                    pSphere->material = cSurface;
                                }
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                        }
                    }
                break;
                case COLLISION_TRIANGLE:
                    CColTriangleSA* pTriangle;
                    switch (eCollisionKey)
                    {
                    case COLLISION_KEY_MATERIAL:
                        argStream.ReadNumber(cSurface);
                        if (!argStream.HasErrors())
                        {
                            if (cSurface >= EColSurfaceValue::DEFAULT && cSurface <= EColSurfaceValue::RAILTRACK)
                            {
                                for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                {
                                    pTriangle = &pColData->pColTriangles[*it];
                                    pTriangle->material = cSurface;
                                }
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                        }
                    case COLLISION_KEY_LIGHTING:
                        argStream.ReadNumber(cDay);
                        argStream.ReadNumber(cNight);
                        if (!argStream.HasErrors())
                        {
                            if (cDay >= 0 && cDay <= 15 && cNight >= 0 && cNight <= 15)
                            {
                                for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                {
                                    pTriangle = &pColData->pColTriangles[*it];
                                    pTriangle->lighting.day = cDay;
                                    pTriangle->lighting.night = cNight;
                                }
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                        }
                        break;
                    case COLLISION_KEY_VERTICES:
                        argStream.ReadNumber(sVertex[0]);
                        argStream.ReadNumber(sVertex[1]);
                        if (argStream.NextIsNumber())
                        {
                            argStream.ReadNumber(sVertex[2]);
                            if (!argStream.HasErrors())
                            {
                                for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                {
                                    pTriangle = &pColData->pColTriangles[*it];
                                    pTriangle->vertex[0] = sVertex[0];
                                    pTriangle->vertex[1] = sVertex[1];
                                    pTriangle->vertex[2] = sVertex[2];
                                }
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                        }
                        else
                        {
                            if (!argStream.HasErrors())
                            {
                                if (sVertex[0] > 0 && sVertex[0] < 3)
                                {
                                    for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                    {
                                        pTriangle = &pColData->pColTriangles[*it];
                                        pTriangle->vertex[sVertex[0]] = sVertex[1];
                                    }
                                    lua_pushboolean(luaVM, true);
                                    return 1;
                                }
                            }
                        }
                    }
                break;
                case COLLISION_VERTEX:
                    CompressedVector* pVertex;
                    switch (eCollisionKey)
                    {
                    case COLLISION_KEY_POSITION:
                        argStream.ReadVector3D(vec1);
                        if (!argStream.HasErrors())
                        {
                            if (CClientColModel::CheckVector(vec1))
                            {
                                for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                {
                                    pVertex = &pColData->pVertices[*it];
                                    pVertex->setVector(vec1);
                                }
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                        }
                    case COLLISION_KEY_MOVE:
                        argStream.ReadVector3D(vec1);
                        if (!argStream.HasErrors())
                        {
                            if (CClientColModel::CheckVector(vec1))
                            {
                                for (std::vector<ushort>::iterator it = vecShapeId.begin(); it != vecShapeId.end(); ++it)
                                {
                                    pVertex = &pColData->pVertices[*it];
                                    pVertex->setVector(vec1 + pVertex->getVector());
                                }
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                        }

                    }
                break;
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
