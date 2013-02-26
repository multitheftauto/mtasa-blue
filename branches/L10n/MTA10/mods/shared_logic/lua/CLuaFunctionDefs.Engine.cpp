/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Engine.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::EngineLoadCOL ( lua_State* luaVM )
{
    // Grab the lua main and the resource belonging to this script
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Get the resource we belong to
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            // Grab the filename
            SString strFile = ( lua_istype ( luaVM, 1, LUA_TSTRING ) ? lua_tostring ( luaVM, 1 ) : "" );
            
            SString strPath;
            // Is this a legal filepath?
            if ( CResourceManager::ParseResourcePathInput( strFile, pResource, strPath ) )
            {
                // Grab the resource root entity
                CClientEntity* pRoot = pResource->GetResourceCOLModelRoot ();

                // Create the col model
                CClientColModel* pCol = new CClientColModel ( m_pManager, INVALID_ELEMENT_ID );

                // Attempt loading the file
                if ( pCol->LoadCol ( strPath ) )
                {
                    // Success. Make it a child of the resource collision root
                    pCol->SetParent ( pRoot );

                    // Return the created col model
                    lua_pushelement ( luaVM, pCol );
                    return 1;
                }
                else
                {
                    // Delete it again. We failed
                    delete pCol;
                    m_pScriptDebugging->LogCustom ( luaVM, SString ( "Load error @ '%s' [Unable to load '%s']", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *strFile ) );
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM );
        }
    }

    // We failed for some reason
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineLoadDFF ( lua_State* luaVM )
{
    // Grab our virtual machine and grab our resource from that.
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Get this resource
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            // Grab the filename
            SString strFile = ( lua_istype ( luaVM, 1, LUA_TSTRING ) ? lua_tostring ( luaVM, 1 ) : "" );
            
            SString strPath;
            // Is this a legal filepath?
            if ( CResourceManager::ParseResourcePathInput( strFile, pResource, strPath ) )
            {
                // Grab the resource root entity
                CClientEntity* pRoot = pResource->GetResourceDFFRoot ();

                // Create a DFF element
                CClientDFF* pDFF = new CClientDFF ( m_pManager, INVALID_ELEMENT_ID );

                // Try to load the DFF file
                if ( pDFF->LoadDFF ( strPath ) )
                {
                    // Success loading the file. Set parent to DFF root
                    pDFF->SetParent ( pRoot );

                    // Return the DFF
                    lua_pushelement ( luaVM, pDFF );
                    return 1;
                }
                else
                {
                    // Delete it again
                    delete pDFF;
                    m_pScriptDebugging->LogCustom ( luaVM, SString ( "Load error @ '%s' [Unable to load '%s']", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *strFile ) );
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "string", 1 );
        }
    }

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineLoadTXD ( lua_State* luaVM )
{
    // Grab our virtual machine and grab our resource from that.
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Grab this resource
        CResource* pResource = pLuaMain->GetResource ();
        if ( pResource )
        {
            bool bFilteringEnabled = true;

            if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
                bFilteringEnabled = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

            // Grab the filename
            SString strFile = ( lua_istype ( luaVM, 1, LUA_TSTRING ) ? lua_tostring ( luaVM, 1 ) : "" );
            
            SString strPath;
            // Is this a legal filepath?
            if ( CResourceManager::ParseResourcePathInput( strFile, pResource, strPath ) )
            {
                // Grab the resource root entity
                CClientEntity* pRoot = pResource->GetResourceTXDRoot ();

                // Create a TXD element
                CClientTXD* pTXD = new CClientTXD ( m_pManager, INVALID_ELEMENT_ID );

                // Try to load the TXD file
                if ( pTXD->LoadTXD ( strPath, bFilteringEnabled ) )
                {
                    // Success loading the file. Set parent to TXD root
                    pTXD->SetParent ( pRoot );

                    // Return the TXD
                    lua_pushelement ( luaVM, pTXD );
                    return 1;
                }
                else
                {
                    // Delete it again
                    delete pTXD;
                    m_pScriptDebugging->LogCustom ( luaVM, SString ( "Load error @ '%s' [Unable to load '%s']", "engineLoadTXD", *strFile ) );
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "string", 1 );
        }
    }

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineReplaceCOL ( lua_State* luaVM )
{
    // Grab the DFF and model ID
    CClientColModel* pCol = ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) ? lua_tocolmodel ( luaVM, 1 ) : NULL );
    unsigned short usModel = CModelNames::ResolveModelID ( lua_tostring ( luaVM, 2 ) );

    // Valid collision model?
    if ( pCol )
    {
        // Valid client DFF and model?
        if ( CClientColModelManager::IsReplacableModel ( usModel ) )
        {
            // Replace the colmodel
            if ( pCol->Replace ( usModel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "number", 2 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "col", 1 );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineRestoreCOL ( lua_State* luaVM )
{
    // Grab the model ID we're going to restore
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned short usModelID = CModelNames::ResolveModelID ( lua_tostring ( luaVM, 1 ) );

        if ( m_pColModelManager->RestoreModel ( usModelID ) )
        {
            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }  
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    // Failed.
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineImportTXD ( lua_State* luaVM )
{
    // Grab the TXD and the model ID
    CClientTXD* pTXD = ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) ? lua_totxd ( luaVM, 1 ) : NULL );
    unsigned short usModelID = CModelNames::ResolveModelID ( lua_tostring ( luaVM, 2 ) );

    // Valid txd?
    if ( pTXD )
    {
        // Valid importable model?
        if ( CClientTXD::IsImportableModel ( usModelID ) )
        {
            // Try to import
            if ( pTXD->Import ( usModelID ) )
            {
                // Success
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "number", 2 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "txd", 1 );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineReplaceModel ( lua_State* luaVM )
{
    CClientDFF* pDFF;
    SString strModelName;
    bool bAlphaTransparency;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pDFF );
    argStream.ReadString ( strModelName );
    argStream.ReadBool ( bAlphaTransparency, false );

    if ( !argStream.HasErrors () )
    {
        ushort usModelID = CModelNames::ResolveModelID ( strModelName );
        if ( usModelID != INVALID_MODEL_ID )
        {
            if ( pDFF->ReplaceModel ( usModelID, bAlphaTransparency ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
            else
                argStream.SetCustomError( SString( "Model ID %d replace failed", usModelID ) );
        }
        else
            argStream.SetCustomError( "Expected valid model ID or name at argument 2" );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineRestoreModel ( lua_State* luaVM )
{
    // Grab the model ID
    unsigned short usModelID = CModelNames::ResolveModelID ( lua_tostring ( luaVM, 1 ) );

    // Valid client DFF and model?
    if ( CClientDFFManager::IsReplacableModel ( usModelID )  )
    {
        // Restore the model
        if ( m_pDFFManager->RestoreModel ( usModelID ) )
        {
            // Success
            lua_pushboolean ( luaVM, true );
            return true;
        }
    }
    else
    {
        m_pScriptDebugging->LogBadType ( luaVM );
    }

    // Failure
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineGetModelLODDistance ( lua_State* luaVM )
{
// float engineGetModelLODDistance ( int/string modelID )
    SString strModelId;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strModelId );

    if ( !argStream.HasErrors () )
    {
        ushort usModelID = CModelNames::ResolveModelID ( strModelId );
        CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModelID );
        if ( pModelInfo )
        {
            float fDistance = pModelInfo->GetLODDistance ();
            lua_pushnumber ( luaVM, fDistance );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineSetModelLODDistance ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        unsigned short usModelID = CModelNames::ResolveModelID ( lua_tostring ( luaVM, 1 )  );
        float fDistance = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModelID );
        if ( pModelInfo && fDistance > 0.0f )
        {
            pModelInfo->SetLODDistance ( fDistance );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineSetAsynchronousLoading ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TBOOLEAN ) &&
        ( iArgument2 == LUA_TBOOLEAN || iArgument2 == LUA_TNONE ) )
    {
        bool bEnabled = lua_toboolean ( luaVM, 1 ) ? true : false;
        bool bForced = iArgument2 == LUA_TBOOLEAN && lua_toboolean ( luaVM, 2 );
        g_pGame->SetAsyncLoadingFromScript ( bEnabled, bForced );
        lua_pushboolean ( luaVM, true );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


// TODO: int CLuaFunctionDefs::EngineReplaceMatchingAtomics ( lua_State* luaVM )
int CLuaFunctionDefs::EngineReplaceMatchingAtomics ( lua_State* luaVM )
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

    lua_pushboolean ( luaVM, false );
    return 1;
}


// TODO: int CLuaFunctionDefs::EngineReplaceWheelAtomics ( lua_State* luaVM )
int CLuaFunctionDefs::EngineReplaceWheelAtomics ( lua_State* luaVM )
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

    lua_pushboolean ( luaVM, false );
    return 1;
}


// TODO: int CLuaFunctionDefs::EnginePositionAtomic ( lua_State* luaVM )
int CLuaFunctionDefs::EnginePositionAtomic ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
    return 1;
}


// TODO: int CLuaFunctionDefs::EnginePositionSeats ( lua_State* luaVM )
int CLuaFunctionDefs::EnginePositionSeats ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
    return 1;
}


// TODO: int CLuaFunctionDefs::EngineAddAllAtomics ( lua_State* luaVM )
int CLuaFunctionDefs::EngineAddAllAtomics ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
    return 1;
}


// TODO: int CLuaFunctionDefs::EngineReplaceVehiclePart ( lua_State* luaVM )
int CLuaFunctionDefs::EngineReplaceVehiclePart ( lua_State* luaVM )
{
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineApplyShaderToWorldTexture ( lua_State* luaVM )
{
//  bool engineApplyShaderToWorldTexture ( element shader, string textureName, [ element targetElement, bool appendLayers ] )
    CClientShader* pShader; SString strTextureNameMatch; CClientEntity* pElement; bool bAppendLayers;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pShader );
    argStream.ReadString ( strTextureNameMatch );
    argStream.ReadUserData ( pElement, NULL );
    argStream.ReadBool ( bAppendLayers, true );

    if ( !argStream.HasErrors () )
    {
        bool bResult = g_pCore->GetGraphics ()->GetRenderItemManager ()->ApplyShaderItemToWorldTexture ( pShader->GetShaderItem (), strTextureNameMatch, pElement, bAppendLayers );
        lua_pushboolean ( luaVM, bResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineRemoveShaderFromWorldTexture ( lua_State* luaVM )
{
//  bool engineRemoveShaderFromWorldTexture ( element shader, string textureName, [ element targetElement ] )
    CClientShader* pShader; SString strTextureNameMatch; CClientEntity* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pShader );
    argStream.ReadString ( strTextureNameMatch );
    argStream.ReadUserData ( pElement, NULL );

    if ( !argStream.HasErrors () )
    {
        bool bResult = g_pCore->GetGraphics ()->GetRenderItemManager ()->RemoveShaderItemFromWorldTexture ( pShader->GetShaderItem (), strTextureNameMatch, pElement );
        lua_pushboolean ( luaVM, bResult );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineGetModelNameFromID ( lua_State* luaVM )
{
//  string engineGetModelNameFromID ( int modelID )
    int iModelID;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iModelID );

    if ( !argStream.HasErrors () )
    {
        SString strModelName = CModelNames::GetModelName ( iModelID );
        if ( !strModelName.empty () )
        {
            lua_pushstring ( luaVM, strModelName );
            return 1;
        }
        argStream.SetCustomError( "Expected valid model ID at argument 1" );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineGetModelIDFromName ( lua_State* luaVM )
{
    //  int engineGetModelIDFromName ( string modelName )
    SString strModelName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strModelName );

    if ( !argStream.HasErrors () )
    {
        int iModelID = CModelNames::GetModelID ( strModelName );
        if ( iModelID != INVALID_MODEL_ID )
        {
            lua_pushnumber ( luaVM, iModelID );
            return 1;
        }
        argStream.SetCustomError( "Expected valid model name at argument 1" );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

// Only works if the model is loaded. EngineGetVisibleTextureNames should be used instead
int CLuaFunctionDefs::EngineGetModelTextureNames ( lua_State* luaVM )
{
//  table engineGetModelTextureNames ( string modelName )
    SString strModelName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strModelName, "" );

    if ( !argStream.HasErrors () )
    {
        ushort usModelID = CModelNames::ResolveModelID ( strModelName );
        if ( usModelID != INVALID_MODEL_ID )
        {
            std::vector < SString > nameList;
            g_pGame->GetRenderWare ()->GetModelTextureNames ( nameList, usModelID );

            lua_newtable ( luaVM );
            for ( uint i = 0 ; i < nameList.size () ; i++ )
            {                
                lua_pushnumber ( luaVM, i + 1 );
                lua_pushstring ( luaVM, nameList [ i ] );
                lua_settable ( luaVM, -3 );
            }
            return 1;
        }
        argStream.SetCustomError( "Expected valid model ID or name at argument 1" );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineGetVisibleTextureNames ( lua_State* luaVM )
{
//  table engineGetVisibleTextureNames ( string wildcardMatch = "*" [, string modelName )
    SString strTextureNameMatch; SString strModelName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strTextureNameMatch, "*" );
    argStream.ReadString ( strModelName, "" );

    if ( !argStream.HasErrors () )
    {
        ushort usModelID = CModelNames::ResolveModelID ( strModelName );
        if ( usModelID != INVALID_MODEL_ID || strModelName == "" )
        {
            std::vector < SString > nameList;
            g_pCore->GetGraphics ()->GetRenderItemManager ()->GetVisibleTextureNames ( nameList, strTextureNameMatch, usModelID );

            lua_newtable ( luaVM );
            for ( uint i = 0 ; i < nameList.size () ; i++ )
            {                
                lua_pushnumber ( luaVM, i + 1 );
                lua_pushstring ( luaVM, nameList [ i ] );
                lua_settable ( luaVM, -3 );
            }
            return 1;
        }
        argStream.SetCustomError( "Expected valid model ID or name at argument 1" );
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}