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
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "engineLoadCOL" );
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
            if ( lua_istype ( luaVM, 2, LUA_TNUMBER ) && CResourceManager::ParseResourcePathInput( strFile, pResource, strPath ) )
            {
                // Check the model ID
                unsigned short usModelID = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
                if ( usModelID == 0 || CClientDFFManager::IsReplacableModel ( usModelID ) )
                {
                    // Grab the resource root entity
                    CClientEntity* pRoot = pResource->GetResourceDFFRoot ();

                    // Create a DFF element
                    CClientDFF* pDFF = new CClientDFF ( m_pManager, INVALID_ELEMENT_ID );

                    // Try to load the DFF file
                    if ( pDFF->LoadDFF ( strPath, usModelID ) )
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
                    }
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "engineLoadDFF", "number", 2 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "engineLoadDFF", "string", 1 );
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
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "engineLoadTXD", "string", 1 );
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
    unsigned short usModel = ( lua_istype ( luaVM, 2, LUA_TNUMBER ) ? ( static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) ) ) : 0 );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "engineReplaceCOL", "number", 2 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "engineReplaceCOL", "col", 1 );

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
        unsigned short usModelID = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );

        if ( m_pColModelManager->RestoreModel ( usModelID ) )
        {
            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }  
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "engineRestoreCOL" );

    // Failed.
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineImportTXD ( lua_State* luaVM )
{
    // Grab the TXD and the model ID
    CClientTXD* pTXD = ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) ? lua_totxd ( luaVM, 1 ) : NULL );
    unsigned short usModelID = ( lua_istype ( luaVM, 2, LUA_TNUMBER ) ? ( static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) ) ) : 0 );

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
            m_pScriptDebugging->LogBadPointer ( luaVM, "engineImportTXD", "number", 2 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "engineImportTXD", "txd", 1 );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineReplaceModel ( lua_State* luaVM )
{
    // Grab the DFF and model ID
    CClientDFF* pDFF = ( lua_istype ( luaVM, 1, LUA_TLIGHTUSERDATA ) ? lua_todff ( luaVM, 1 ) : NULL );
    unsigned short usModelID = ( lua_istype ( luaVM, 2, LUA_TNUMBER ) ? ( static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) ) ) : 0 );

    // Valid client DFF?
    if ( pDFF )
    {
        // Valid model?
        if ( CClientDFFManager::IsReplacableModel ( usModelID ) )
        {
            // Replace the model
            pDFF->ReplaceModel ( usModelID );

            // Success
            lua_pushboolean ( luaVM, true );
            return true;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "engineReplaceModel", "number", 2 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "engineReplaceModel", "dff", 1 );

    // Failure
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::EngineRestoreModel ( lua_State* luaVM )
{
    // Grab the model ID
    unsigned short usModelID = ( lua_istype ( luaVM, 1, LUA_TNUMBER ) ? ( static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) ) ) : 0 );

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
        m_pScriptDebugging->LogBadType ( luaVM, "engineRestoreModel" );
    }

    // Failure
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
        unsigned short usModelID = static_cast < unsigned short > ( lua_tonumber ( luaVM, 1 ) );
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
    m_pScriptDebugging->LogBadType ( luaVM, "engineReplaceMatchingAtomics" );
    }
    }

    if ( pEntityClump && pClump ) {
    uiAtomics = m_pRenderWare->LoadAtomics ( pClump, &Atomics[0] );
    m_pRenderWare->ReplaceAllAtomicsInClump ( pEntityClump, &Atomics[0], uiAtomics );

    lua_pushboolean ( luaVM, true );
    } else {
    m_pScriptDebugging->LogBadType ( luaVM, "engineReplaceMatchingAtomics" );
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
    m_pScriptDebugging->LogBadType ( luaVM, "engineReplaceWheelAtomics" );
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


