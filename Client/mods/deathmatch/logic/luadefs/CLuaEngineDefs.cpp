/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaEngineDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include "../Client/game_sa/CModelInfoSA.h"
#include "../Client/game_sa/CColModelSA.h"

CBaseModelInfoSAInterface** ppModelInfo = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;

void CLuaEngineDefs::LoadFunctions()
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
        {"engineGetModelCollisionProperties", EngineGetModelCollisionProperties},
        {"engineGetModelCollisionData", EngineGetModelCollisionData},
        {"engineSetModelCollisionData", EngineSetModelCollisionData},
        {"engineUpdateModelCollisionBoundingBox", EngineUpdateModelCollisionBoundingBox},
        {"isModelCollisionLoaded", IsModelCollisionLoaded},
        {"engineGetSurfaceProperties", EngineGetSurfaceProperties},
        {"engineSetSurfaceProperties", EngineSetSurfaceProperties},
        {"engineResetSurfaceProperties", EngineResetSurfaceProperties},

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

                        // Calculate amount of vertices for future operations
                        pCol->UpdateVerticesCount();

                        // Let user update bounding box if default is invalid.
                        pCol->SetCollisionHasChanged(true);

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
                bool    bIsRawData = CIFPEngine::IsIFPData(strFile);
                SString strPath;
                // Is this a legal filepath?
                if (bIsRawData || CResourceManager::ParseResourcePathInput(strFile, pResource, &strPath))
                {
                    std::shared_ptr<CClientIFP> pIFP =
                        CIFPEngine::EngineLoadIFP(pResource, m_pManager, bIsRawData ? strFile : strPath, bIsRawData, strBlockName);
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
    CClientColModel* pCol = nullptr;
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
    CClientTXD*      pTXD = nullptr;
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
    CClientColModel* pCol = nullptr;
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
                if (pCol)
                {
                    lua_pushnumber(luaVM, pCol->GetVerticesCount());
                }
                else
                {
                    lua_pushnumber(luaVM, pColData->getNumVertices());
                }
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
    ushort           usModel;
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
    CClientColModel* pCol = nullptr;
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
            ushort           usIndex;
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
                                return 1;
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
                                lua_pushtablevalue(luaVM, 1, static_cast<float>(pTriangle.vertex[0] + 1));
                                lua_pushtablevalue(luaVM, 2, static_cast<float>(pTriangle.vertex[1] + 1));
                                lua_pushtablevalue(luaVM, 3, static_cast<float>(pTriangle.vertex[2] + 1));
                                lua_pushtablevalue(luaVM, 4, static_cast<float>(pTriangle.material));
                                lua_pushtablevalue(luaVM, 5, static_cast<float>(pTriangle.lighting.day));
                                lua_pushtablevalue(luaVM, 6, static_cast<float>(pTriangle.lighting.night));
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
                                lua_pushtablevalue(luaVM, 1, static_cast<float>(pTriangle.vertex[0] + 1));
                                lua_pushtablevalue(luaVM, 2, static_cast<float>(pTriangle.vertex[1] + 1));
                                lua_pushtablevalue(luaVM, 3, static_cast<float>(pTriangle.vertex[2] + 1));
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
                            unsigned short usNumVertices;
                            if (pCol)
                            {
                                usNumVertices = pCol->GetVerticesCount();
                            }
                            else
                            {
                                usNumVertices = pColData->getNumVertices();
                            }

                            if (pColData->isValidIndex(COLLISION_VERTEX, usIndex, usNumVertices))
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
                            unsigned short usNumVertices;
                            if (pCol)
                            {
                                usNumVertices = pCol->GetVerticesCount();
                            }
                            else
                            {
                                usNumVertices = pColData->getNumVertices();
                            }

                            CVector          vec;
                            CompressedVector pVertex;

                            lua_newtable(luaVM);
                            for (ushort i = 0; i < usNumVertices; i++)
                            {
                                lua_pushnumber(luaVM, i + 1);
                                lua_newtable(luaVM);
                                pVertex = pColData->pVertices[i];
                                vec = pVertex.getVector();
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
    CClientColModel* pCol = nullptr;
    CVector          vecCenter;
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
            if (!pCol->HasChanged())
            {
                lua_pushboolean(luaVM, true);            // bounding box already updated.
                return 1;
            }
            CColDataSA*     pColData = pColModelSAInterface->pColData;
            CBoundingBoxSA* pBoundingBox = &pColModelSAInterface->boundingBox;
            CVector         minVec(0, 0, 0);
            CVector         maxVec(0, 0, 0);
            CVector         pBoxMinVec, pBoxMaxVec;

            CColSphereSA pSphere;
            CColBoxSA    pBox;
            for (uint i = 0; pColData->numColBoxes > i; i++)
            {
                pBox = pColData->pColBoxes[i];
                CClientColModel::AlignVector(maxVec, minVec, pBox.min);
                CClientColModel::AlignVector(maxVec, minVec, pBox.max);
            }

            for (uint i = 0; pColData->numColSpheres > i; i++)
            {
                pSphere = pColData->pColSpheres[i];
                pBoxMaxVec = pSphere.vecCenter;
                pBoxMaxVec.fX += pSphere.fRadius;
                pBoxMaxVec.fY += pSphere.fRadius;
                pBoxMaxVec.fZ += pSphere.fRadius;
                CClientColModel::AlignVector(maxVec, minVec, pBoxMaxVec);

                pBoxMaxVec.fX -= pSphere.fRadius * 2;
                pBoxMaxVec.fY -= pSphere.fRadius * 2;
                pBoxMaxVec.fZ -= pSphere.fRadius * 2;
                CClientColModel::AlignVector(maxVec, minVec, pBoxMaxVec);
            }

            std::map<ushort, CompressedVector>           vecVertices = pColData->getAllVertices();
            std::map<ushort, CompressedVector>::iterator it;
            for (it = vecVertices.begin(); it != vecVertices.end(); it++)
            {
                CClientColModel::AlignVector(maxVec, minVec, it->second.getVector());
            }

            pBoundingBox->fRadius = std::max(DistanceBetweenPoints3D(maxVec, vecCenter), DistanceBetweenPoints3D(minVec, vecCenter));
            pBoundingBox->vecOffset = vecCenter;
            pBoundingBox->vecMax = maxVec;
            pBoundingBox->vecMin = minVec;
            pCol->SetCollisionHasChanged(false);
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEngineDefs::EngineSetModelCollisionData(lua_State* luaVM)
{
    CClientColModel*  pCol = nullptr;
    std::vector<uint> vecUncheckedShapeId, vecShapeId, vecFailed;
    eCollisionShapes  eCollisionShape;
    eCollisionKey     eCollisionKey;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pCol);
    argStream.ReadEnumString(eCollisionShape);
    argStream.ReadEnumString(eCollisionKey);
    if (argStream.NextIsNumber())
    {
        uint usShapeId;
        argStream.ReadNumber(usShapeId);
        vecUncheckedShapeId.push_back(usShapeId);
    }
    else
        argStream.ReadNumberTable(vecUncheckedShapeId);

    CColModelSAInterface* pColModelSAInterface;
    if (!argStream.HasErrors())
    {
        if (pCol)
            pColModelSAInterface = pCol->GetColModelInterface();

        if (pColModelSAInterface)
        {
            CColDataSA* pColData = pColModelSAInterface->pColData;
            ushort      numVertices = pCol->GetVerticesCount();
            for (ushort i = 0; i != vecUncheckedShapeId.size(); i++)
            {
                if (pColData->isValidIndex(eCollisionShape, vecUncheckedShapeId[i] - 1, numVertices))
                    vecShapeId.push_back(vecUncheckedShapeId[i] - 1);
                else
                    vecFailed.push_back(vecUncheckedShapeId[i] - 1);
            }

            std::sort(vecShapeId.begin(), vecShapeId.end());
            vecShapeId.erase(unique(vecShapeId.begin(), vecShapeId.end()), vecShapeId.end());

            if (vecFailed.size() == 0)
            {
                CVector vec1, vec2;

                switch (eCollisionShape)
                {
                    case COLLISION_BOX:
                        CColBoxSA* pBox;
                        switch (eCollisionKey)
                        {
                            case COLLISION_KEY_MATERIAL:
                                uint cSurface;
                                argStream.ReadNumber(cSurface);
                                if (!argStream.HasErrors())
                                {
                                    if (cSurface >= EColSurfaceValue::DEFAULT && cSurface <= EColSurfaceValue::RAILTRACK)
                                    {
                                        for (unsigned int it : vecShapeId)
                                        {
                                            pBox = &pColData->pColBoxes[it];
                                            pBox->material = cSurface;
                                        }
                                    }
                                    else
                                        argStream.SetCustomError("Invalid material id.");
                                }
                                break;
                            case COLLISION_KEY_SIZE:
                                if (argStream.NextIsBool())
                                {
                                    bool bMinVector;
                                    argStream.ReadBool(bMinVector);            // true = min, false = max
                                    argStream.ReadVector3D(vec1);
                                    if (!argStream.HasErrors())
                                    {
                                        if (CClientColModel::CheckVector(vec1))
                                        {
                                            if (bMinVector)
                                            {
                                                for (unsigned int it : vecShapeId)
                                                {
                                                    pBox = &pColData->pColBoxes[it];
                                                    if (!CClientColModel::CompareVector(vec1, pBox->max))
                                                        vecFailed.push_back(it);
                                                }
                                                if (vecFailed.size() == 0)
                                                {
                                                    for (unsigned int it : vecShapeId)
                                                    {
                                                        pBox = &pColData->pColBoxes[it];
                                                        pBox->min = vec1;
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                for (unsigned int it : vecShapeId)
                                                {
                                                    pBox = &pColData->pColBoxes[it];
                                                    if (!CClientColModel::CompareVector(pBox->min, vec1))
                                                        vecFailed.push_back(it);
                                                }
                                                if (vecFailed.size() == 0)
                                                {
                                                    for (unsigned int it : vecShapeId)
                                                    {
                                                        pBox = &pColData->pColBoxes[it];
                                                        pBox->max = vec1;
                                                    }
                                                }
                                            }
                                        }
                                        else
                                            argStream.SetCustomError("Position is out of bounding box.");
                                    }
                                }
                                else
                                {
                                    argStream.ReadVector3D(vec1);
                                    argStream.ReadVector3D(vec2);
                                    if (!argStream.HasErrors())
                                    {
                                        if (CClientColModel::CheckVector(vec1))
                                        {
                                            if (CClientColModel::CheckVector(vec2))
                                            {
                                                if (CClientColModel::CompareVector(vec1, vec2))
                                                {
                                                    for (unsigned int it : vecShapeId)
                                                    {
                                                        pBox = &pColData->pColBoxes[it];
                                                        pBox->min = vec1;
                                                        pBox->max = vec2;
                                                    }
                                                }
                                                else
                                                    argStream.SetCustomError("Second position need to be greater than first.");
                                            }
                                            else
                                                argStream.SetCustomError("Second position is out of bounding box.");
                                        }
                                        else
                                            argStream.SetCustomError("First position is out of bounding box.");
                                    }
                                }
                                break;
                            case COLLISION_KEY_POSITION:
                                argStream.ReadVector3D(vec1);
                                if (!argStream.HasErrors())
                                {
                                    for (unsigned int it : vecShapeId)
                                    {
                                        pBox = &pColData->pColBoxes[it];
                                        vec2 = CVector(pBox->min.fX + pBox->max.fX, pBox->min.fY + pBox->max.fY, pBox->min.fZ + pBox->max.fZ) / 2;
                                        if (!CClientColModel::CheckVector(pBox->min + (vec1 - vec2)) ||
                                            !CClientColModel::CheckVector(pBox->max + (vec1 - vec2)))
                                            vecFailed.push_back(it);
                                    }
                                    if (vecFailed.size())
                                    {
                                        for (unsigned int it : vecShapeId)
                                        {
                                            pBox = &pColData->pColBoxes[it];
                                            vec2 = CVector(pBox->min.fX + pBox->max.fX, pBox->min.fY + pBox->max.fY, pBox->min.fZ + pBox->max.fZ) / 2;
                                            pBox->min += (vec1 - vec2);
                                            pBox->max += (vec1 - vec2);
                                        }
                                    }
                                }
                                break;
                            case COLLISION_KEY_MOVE:
                                argStream.ReadVector3D(vec1);
                                if (!argStream.HasErrors())
                                {
                                    for (unsigned int it : vecShapeId)
                                    {
                                        pBox = &pColData->pColBoxes[it];
                                        if (!(CClientColModel::CheckVector(pBox->min + vec1) && CClientColModel::CheckVector(pBox->max + vec1)))
                                            vecFailed.push_back(it);
                                    }
                                    if (vecFailed.size() == 0)
                                    {
                                        for (unsigned int it : vecShapeId)
                                        {
                                            pBox = &pColData->pColBoxes[it];
                                            pBox->min += vec1;
                                            pBox->max += vec1;
                                        }
                                    }
                                }
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
                                        for (unsigned int it : vecShapeId)
                                        {
                                            pSphere = &pColData->pColSpheres[it];
                                            if (!CClientColModel::CheckVector(vec1, pSphere->fRadius))
                                            {
                                                vecFailed.push_back(it);
                                            }
                                        }

                                        if (vecFailed.size() == 0)
                                        {
                                            for (unsigned int it : vecShapeId)
                                            {
                                                pSphere = &pColData->pColSpheres[it];
                                                pSphere->vecCenter = vec1;
                                            }
                                        }
                                    }
                                    else
                                        argStream.SetCustomError("First position is out of bounding box.");
                                }
                                break;
                            case COLLISION_KEY_MOVE:
                                argStream.ReadVector3D(vec1);
                                if (!argStream.HasErrors())
                                {
                                    if (CClientColModel::CheckMoveVector(vec1))
                                    {
                                        for (unsigned int it : vecShapeId)
                                        {
                                            pSphere = &pColData->pColSpheres[it];
                                            if (!CClientColModel::CheckVector(pSphere->vecCenter + vec1, pSphere->fRadius))
                                                vecFailed.push_back(it);
                                        }
                                        if (vecFailed.size() == 0)
                                        {
                                            for (unsigned int it : vecShapeId)
                                            {
                                                pSphere = &pColData->pColSpheres[it];
                                                pSphere->vecCenter += vec1;
                                            }
                                        }
                                    }
                                    else
                                        argStream.SetCustomError("You can't move furher than 256 units in each axis.");
                                }
                                break;
                            case COLLISION_KEY_RADIUS:
                                float fRadius;
                                argStream.ReadNumber(fRadius);
                                if (!argStream.HasErrors())
                                {
                                    if (fRadius >= 0 && fRadius <= 256)            // bigger than 256 are sure that are outside bounding
                                    {
                                        for (unsigned int it : vecShapeId)
                                        {
                                            pSphere = &pColData->pColSpheres[it];
                                            if (!CClientColModel::CheckVector(pSphere->vecCenter, fRadius))
                                                vecFailed.push_back(it);
                                        }
                                        if (vecFailed.size() == 0)
                                        {
                                            for (unsigned int it : vecShapeId)
                                            {
                                                pSphere = &pColData->pColSpheres[it];
                                                pSphere->fRadius = fRadius;
                                            }
                                        }
                                    }
                                    else
                                        argStream.SetCustomError("Radius must be between 0 and 256.");
                                }
                                break;
                            case COLLISION_KEY_MATERIAL:
                                uint cSurface;
                                argStream.ReadNumber(cSurface);
                                if (!argStream.HasErrors())
                                {
                                    if (cSurface >= EColSurfaceValue::DEFAULT && cSurface <= EColSurfaceValue::RAILTRACK)
                                    {
                                        for (unsigned int it : vecShapeId)
                                        {
                                            pSphere = &pColData->pColSpheres[it];
                                            pSphere->material = cSurface;
                                        }
                                    }
                                    else
                                        argStream.SetCustomError("Invalid material id.");
                                }
                        }
                        break;
                    case COLLISION_TRIANGLE:
                        CColTriangleSA* pTriangle;
                        switch (eCollisionKey)
                        {
                            case COLLISION_KEY_MATERIAL:
                                uint cSurface;
                                argStream.ReadNumber(cSurface);
                                if (!argStream.HasErrors())
                                {
                                    if (cSurface >= EColSurfaceValue::DEFAULT && cSurface <= EColSurfaceValue::RAILTRACK)
                                    {
                                        for (unsigned int it : vecShapeId)
                                        {
                                            pTriangle = &pColData->pColTriangles[it];
                                            pTriangle->material = cSurface;
                                        }
                                    }
                                    else
                                        argStream.SetCustomError("Invalid material id.");
                                }
                            case COLLISION_KEY_LIGHTING:
                                uint cDay, cNight;
                                argStream.ReadNumber(cDay);
                                argStream.ReadNumber(cNight);
                                if (!argStream.HasErrors())
                                {
                                    if (cDay >= 0 && cDay <= 15)
                                    {
                                        if (cNight >= 0 && cNight <= 15)
                                        {
                                            for (unsigned int it : vecShapeId)
                                            {
                                                pTriangle = &pColData->pColTriangles[it];
                                                pTriangle->lighting.day = cDay;
                                                pTriangle->lighting.night = cNight;
                                            }
                                        }
                                        else
                                            argStream.SetCustomError("Night light must be in range 0 up to 15.");
                                    }
                                    else
                                        argStream.SetCustomError("Day light must be in range 0 up to 15.");
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
                                        for (unsigned int it : vecShapeId)
                                        {
                                            pVertex = &pColData->pVertices[it];
                                            pVertex->setVector(vec1);
                                        }
                                    }
                                    else
                                        argStream.SetCustomError("Position is out of bounding box.");
                                }
                                break;
                            case COLLISION_KEY_MOVE:
                                argStream.ReadVector3D(vec1);
                                if (!argStream.HasErrors())
                                {
                                    for (unsigned int it : vecShapeId)
                                    {
                                        pVertex = &pColData->pVertices[it];
                                        if (!CClientColModel::CheckVector(vec1 + pVertex->getVector()))
                                            vecFailed.push_back(it);
                                    }
                                    if (vecFailed.size() == 0)
                                    {
                                        for (unsigned int it : vecShapeId)
                                        {
                                            pVertex = &pColData->pVertices[it];
                                            pVertex->setVector(vec1 + pVertex->getVector());
                                        }
                                    }
                                }
                        }
                }
            }
        }
    }
    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
    else
    {
        if (vecFailed.size() > 0)
        {
            unsigned short i = 1;
            lua_pushboolean(luaVM, false);
            lua_newtable(luaVM);
            for (unsigned int it : vecFailed)
            {
                lua_pushtablevalue(luaVM, i++, it + 1);
            }
            return 2;
        }
        else
        {
            if (pColModelSAInterface != nullptr)
            {
                pCol->SetCollisionHasChanged(true);
            }
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
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
