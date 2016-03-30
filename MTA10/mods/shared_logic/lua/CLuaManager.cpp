/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaManager.cpp
*  PURPOSE:     Lua virtual machine manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

static int DummyPreCall ( lua_CFunction f, lua_State* L )
{
    // Always allow functions
    return true;
}

CLuaManager::CLuaManager ( CClientGame* pClientGame )
{
    m_pEvents = pClientGame->GetEvents ();
    m_pGUIManager = pClientGame->GetGUIManager ();
    m_pRegisteredCommands = pClientGame->GetRegisteredCommands ();

    // Ensure lua was compiled with apichecks
    #ifdef NDEBUG
        #error "NDEBUG should not be defined"
    #endif
    assert( luaX_is_apicheck_enabled() );

    // Load the C functions
    LoadCFunctions ();
    lua_registerPreCallHook ( CLuaDefs::CanUseFunction );
    lua_registerUndumpHook ( CLuaMain::OnUndump );
}


CLuaManager::~CLuaManager ( void )
{
    // Delete all the VM's
    list<CLuaMain *>::iterator iter;
    for ( iter = m_virtualMachines.begin(); iter != m_virtualMachines.end(); iter++ )
    {
        delete (*iter);
    }

    // Clear the C functions
    CLuaCFunctions::RemoveAllFunctions ();
}

CLuaMain * CLuaManager::CreateVirtualMachine ( CResource* pResourceOwner, bool bEnableOOP )
{
    // Create it and add it to the list over VM's
    CLuaMain * pLuaMain = new CLuaMain ( this, pResourceOwner, bEnableOOP );
    m_virtualMachines.push_back ( pLuaMain );
    pLuaMain->InitVM ();
    return pLuaMain;
}

bool CLuaManager::RemoveVirtualMachine ( CLuaMain * pLuaMain )
{
    if ( pLuaMain )
    {
        // Remove all events registered by it
        m_pEvents->RemoveAllEvents ( pLuaMain );
        m_pRegisteredCommands->CleanUpForVM ( pLuaMain );

        // Delete it unless it is already
        if ( !pLuaMain->BeingDeleted () )
        {
            delete pLuaMain;
        }

        // Remove it from our list
        m_virtualMachines.remove ( pLuaMain );
        return true;
    }

    return false;
}


void CLuaManager::OnLuaMainOpenVM( CLuaMain* pLuaMain, lua_State* luaVM )
{
    MapSet( m_VirtualMachineMap, pLuaMain->GetVirtualMachine(), pLuaMain );
}


void CLuaManager::OnLuaMainCloseVM( CLuaMain* pLuaMain, lua_State* luaVM )
{
    MapRemove( m_VirtualMachineMap, pLuaMain->GetVirtualMachine() );
}


void CLuaManager::ProcessPendingDeleteList ( void )
{
    while ( !m_PendingDeleteList.empty () )
    {
        lua_State* luaVM = m_PendingDeleteList.front ();
        m_PendingDeleteList.pop_front ();
        CLuaFunctionRef::RemoveLuaFunctionRefsForVM( luaVM );
        lua_close( luaVM );
    }
}


void CLuaManager::DoPulse ( void )
{
    list<CLuaMain *>::iterator iter = m_virtualMachines.begin ();
    for ( ; iter != m_virtualMachines.end(); ++iter )
    {
        (*iter)->DoPulse ();
    }
}

CLuaMain* CLuaManager::GetVirtualMachine ( lua_State* luaVM )
{
    if ( !luaVM )
        return NULL;

    // Grab the main virtual state because the one we've got passed might be a coroutine state
    // and only the main state is in our list.
    lua_State* main = lua_getmainstate ( luaVM );
    if ( main )
    {
        luaVM = main;
    }

    // Find a matching VM in our map
    CLuaMain* pLuaMain = MapFindRef( m_VirtualMachineMap, luaVM );
    if ( pLuaMain )
        return pLuaMain;

    // Find a matching VM in our list (should not be needed)
    list < CLuaMain* >::const_iterator iter = m_virtualMachines.begin ();
    for ( ; iter != m_virtualMachines.end (); iter++ )
    {
        if ( luaVM == (*iter)->GetVirtualMachine () )
        {
            dassert( 0 );   // Why not in map?
            return *iter;
        }
    }

    // Doesn't exist
    return NULL;
}


void CLuaManager::LoadCFunctions ( void )
{
    // ** BACKWARDS COMPATIBILITY FUNCS. SHOULD BE REMOVED BEFORE FINAL RELEASE! **
    CLuaCFunctions::AddFunction ( "getPlayerRotation", CLuaPedDefs::GetPedRotation );
    CLuaCFunctions::AddFunction ( "canPlayerBeKnockedOffBike", CLuaPedDefs::CanPedBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "getPlayerContactElement", CLuaPedDefs::GetPedContactElement );
    CLuaCFunctions::AddFunction ( "isPlayerInVehicle", CLuaPedDefs::IsPedInVehicle );
    CLuaCFunctions::AddFunction ( "doesPlayerHaveJetPack", CLuaPedDefs::DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPlayerInWater", CLuaFunctionDefs::IsElementInWater );
    CLuaCFunctions::AddFunction ( "isPedInWater", CLuaFunctionDefs::IsElementInWater );
    CLuaCFunctions::AddFunction ( "isPedOnFire", CLuaPedDefs::IsPedOnFire );
    CLuaCFunctions::AddFunction ( "setPedOnFire", CLuaPedDefs::SetPedOnFire );
    CLuaCFunctions::AddFunction ( "isPlayerOnGround", CLuaPedDefs::IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPlayerTask", CLuaPedDefs::GetPedTask );
    CLuaCFunctions::AddFunction ( "getPlayerSimplestTask", CLuaPedDefs::GetPedSimplestTask );
    CLuaCFunctions::AddFunction ( "isPlayerDoingTask", CLuaPedDefs::IsPedDoingTask );
    CLuaCFunctions::AddFunction ( "getPlayerTarget", CLuaPedDefs::GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPlayerTargetStart", CLuaPedDefs::GetPedTargetStart );
    CLuaCFunctions::AddFunction ( "getPlayerTargetEnd", CLuaPedDefs::GetPedTargetEnd );
    CLuaCFunctions::AddFunction ( "getPlayerTargetRange", CLuaPedDefs::GetPedTargetRange );
    CLuaCFunctions::AddFunction ( "getPlayerTargetCollision", CLuaPedDefs::GetPedTargetCollision );
    CLuaCFunctions::AddFunction ( "getPlayerWeaponSlot", CLuaPedDefs::GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPlayerWeapon", CLuaPedDefs::GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPlayerAmmoInClip", CLuaPedDefs::GetPedAmmoInClip );
    CLuaCFunctions::AddFunction ( "getPlayerTotalAmmo", CLuaPedDefs::GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPedWeaponMuzzlePosition", CLuaPedDefs::GetPedWeaponMuzzlePosition );
    CLuaCFunctions::AddFunction ( "getPlayerOccupiedVehicle", CLuaPedDefs::GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPlayerArmor", CLuaPedDefs::GetPedArmor );
    CLuaCFunctions::AddFunction ( "getPlayerSkin", CLuaFunctionDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "isPlayerChoking", CLuaPedDefs::IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPlayerDucked", CLuaPedDefs::IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPlayerStat", CLuaPedDefs::GetPedStat );
    CLuaCFunctions::AddFunction ( "setPlayerWeaponSlot", CLuaPedDefs::SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "setPlayerSkin", CLuaFunctionDefs::SetElementModel );
    CLuaCFunctions::AddFunction ( "setPlayerRotation", CLuaPedDefs::SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPlayerCanBeKnockedOffBike", CLuaPedDefs::SetPedCanBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "setVehicleModel", CLuaFunctionDefs::SetElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleModel", CLuaFunctionDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "getPedSkin", CLuaFunctionDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "setPedSkin", CLuaFunctionDefs::SetElementModel );
    CLuaCFunctions::AddFunction ( "getObjectRotation", CLuaFunctionDefs::GetElementRotation );
    CLuaCFunctions::AddFunction ( "setObjectRotation", CLuaFunctionDefs::SetElementRotation );
    CLuaCFunctions::AddFunction ( "getVehicleIDFromName", CLuaVehicleDefs::GetVehicleModelFromName );    
    CLuaCFunctions::AddFunction ( "getVehicleID", CLuaFunctionDefs::GetElementModel );    
    CLuaCFunctions::AddFunction ( "getVehicleRotation", CLuaFunctionDefs::GetElementRotation );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromID", CLuaVehicleDefs::GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "setVehicleRotation", CLuaFunctionDefs::SetElementRotation );
    CLuaCFunctions::AddFunction ( "attachElementToElement", CLuaFunctionDefs::AttachElements );
    CLuaCFunctions::AddFunction ( "detachElementFromElement", CLuaFunctionDefs::DetachElements );
    CLuaCFunctions::AddFunction ( "xmlFindSubNode", CLuaFunctionDefs::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlNodeGetSubNodes", CLuaFunctionDefs::XMLNodeGetChildren );
    CLuaCFunctions::AddFunction ( "xmlNodeFindSubNode", CLuaFunctionDefs::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlCreateSubNode", CLuaFunctionDefs::XMLCreateChild );
    CLuaCFunctions::AddFunction ( "xmlNodeFindChild", CLuaFunctionDefs::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "isPlayerDead", CLuaPedDefs::IsPedDead );
    CLuaCFunctions::AddFunction ( "guiEditSetCaratIndex", CLuaFunctionDefs::GUIEditSetCaretIndex );
    CLuaCFunctions::AddFunction ( "guiMemoSetCaratIndex", CLuaFunctionDefs::GUIMemoSetCaretIndex );
    // ** END OF BACKWARDS COMPATIBILITY FUNCS. **

    // Resource funcs
    CLuaCFunctions::AddFunction ( "call", CLuaFunctionDefs::Call );
    CLuaCFunctions::AddFunction ( "getThisResource", CLuaFunctionDefs::GetThisResource );
    CLuaCFunctions::AddFunction ( "getResourceConfig", CLuaFunctionDefs::GetResourceConfig );
    CLuaCFunctions::AddFunction ( "getResourceName", CLuaFunctionDefs::GetResourceName );
    CLuaCFunctions::AddFunction ( "getResourceFromName", CLuaFunctionDefs::GetResourceFromName );
    CLuaCFunctions::AddFunction ( "getResourceRootElement", CLuaFunctionDefs::GetResourceRootElement );
    CLuaCFunctions::AddFunction ( "getResourceGUIElement", CLuaFunctionDefs::GetResourceGUIElement );
    CLuaCFunctions::AddFunction ( "getResourceDynamicElementRoot", CLuaFunctionDefs::GetResourceDynamicElementRoot );
    CLuaCFunctions::AddFunction ( "getResourceExportedFunctions", CLuaFunctionDefs::GetResourceExportedFunctions );
    CLuaCFunctions::AddFunction ( "getResourceState", CLuaFunctionDefs::GetResourceState );
    CLuaCFunctions::AddFunction ( "loadstring", CLuaFunctionDefs::LoadString );
    CLuaCFunctions::AddFunction ( "load", CLuaFunctionDefs::Load );

    // Event funcs
    CLuaCFunctions::AddFunction ( "addEvent", CLuaFunctionDefs::AddEvent );
    CLuaCFunctions::AddFunction ( "addEventHandler", CLuaFunctionDefs::AddEventHandler );
    CLuaCFunctions::AddFunction ( "removeEventHandler", CLuaFunctionDefs::RemoveEventHandler );
    CLuaCFunctions::AddFunction ( "getEventHandlers", CLuaFunctionDefs::GetEventHandlers );
    CLuaCFunctions::AddFunction ( "triggerEvent", CLuaFunctionDefs::TriggerEvent );
    CLuaCFunctions::AddFunction ( "triggerServerEvent", CLuaFunctionDefs::TriggerServerEvent );
    CLuaCFunctions::AddFunction ( "cancelEvent", CLuaFunctionDefs::CancelEvent );
    CLuaCFunctions::AddFunction ( "wasEventCancelled", CLuaFunctionDefs::WasEventCancelled );
    CLuaCFunctions::AddFunction ( "triggerLatentServerEvent", CLuaFunctionDefs::TriggerLatentServerEvent );
    CLuaCFunctions::AddFunction ( "getLatentEventHandles", CLuaFunctionDefs::GetLatentEventHandles );
    CLuaCFunctions::AddFunction ( "getLatentEventStatus", CLuaFunctionDefs::GetLatentEventStatus );
    CLuaCFunctions::AddFunction ( "cancelLatentEvent", CLuaFunctionDefs::CancelLatentEvent );

    // Output funcs
    CLuaCFunctions::AddFunction ( "outputConsole", CLuaFunctionDefs::OutputConsole );
    CLuaCFunctions::AddFunction ( "outputChatBox", CLuaFunctionDefs::OutputChatBox );
    CLuaCFunctions::AddFunction ( "showChat", CLuaFunctionDefs::ShowChat );
    CLuaCFunctions::AddFunction ( "isChatVisible", CLuaFunctionDefs::IsChatVisible );
    CLuaCFunctions::AddFunction ( "outputDebugString", CLuaFunctionDefs::OutputClientDebugString );
    CLuaCFunctions::AddFunction ( "setClipboard", CLuaFunctionDefs::SetClipboard );
    //CLuaCFunctions::AddFunction ( "getClipboard", CLuaFunctionDefs::GetClipboard );
    CLuaCFunctions::AddFunction ( "setWindowFlashing", CLuaFunctionDefs::SetWindowFlashing );

    // Element get funcs
    CLuaCFunctions::AddFunction ( "getRootElement", CLuaFunctionDefs::GetRootElement );
    CLuaCFunctions::AddFunction ( "isElement", CLuaFunctionDefs::IsElement );
    CLuaCFunctions::AddFunction ( "getElementByID", CLuaFunctionDefs::GetElementByID );
    CLuaCFunctions::AddFunction ( "getElementByIndex", CLuaFunctionDefs::GetElementByIndex );
    CLuaCFunctions::AddFunction ( "getElementData", CLuaFunctionDefs::GetElementData );
    CLuaCFunctions::AddFunction ( "getElementMatrix", CLuaFunctionDefs::GetElementMatrix );
    CLuaCFunctions::AddFunction ( "getElementPosition", CLuaFunctionDefs::GetElementPosition );
    CLuaCFunctions::AddFunction ( "getElementRotation", CLuaFunctionDefs::GetElementRotation );
    CLuaCFunctions::AddFunction ( "getElementVelocity", CLuaFunctionDefs::GetElementVelocity );
    CLuaCFunctions::AddFunction ( "getElementType", CLuaFunctionDefs::GetElementType );
    CLuaCFunctions::AddFunction ( "getElementChildren", CLuaFunctionDefs::GetElementChildren );
    CLuaCFunctions::AddFunction ( "getElementChild", CLuaFunctionDefs::GetElementChild );
    CLuaCFunctions::AddFunction ( "getElementChildrenCount", CLuaFunctionDefs::GetElementChildrenCount );
    CLuaCFunctions::AddFunction ( "getElementID", CLuaFunctionDefs::GetElementID );
    CLuaCFunctions::AddFunction ( "getElementParent", CLuaFunctionDefs::GetElementParent );
    CLuaCFunctions::AddFunction ( "getElementsByType", CLuaFunctionDefs::GetElementsByType );
    CLuaCFunctions::AddFunction ( "getElementInterior", CLuaFunctionDefs::GetElementInterior );
    CLuaCFunctions::AddFunction ( "isElementWithinColShape", CLuaFunctionDefs::IsElementWithinColShape );
    CLuaCFunctions::AddFunction ( "isElementWithinMarker", CLuaFunctionDefs::IsElementWithinMarker );
    CLuaCFunctions::AddFunction ( "getElementsWithinColShape", CLuaFunctionDefs::GetElementsWithinColShape );
    CLuaCFunctions::AddFunction ( "getElementDimension", CLuaFunctionDefs::GetElementDimension );
    CLuaCFunctions::AddFunction ( "getElementBoundingBox", CLuaFunctionDefs::GetElementBoundingBox );
    CLuaCFunctions::AddFunction ( "getElementRadius", CLuaFunctionDefs::GetElementRadius );
    CLuaCFunctions::AddFunction ( "isElementAttached", CLuaFunctionDefs::IsElementAttached );
    CLuaCFunctions::AddFunction ( "getElementAttachedTo", CLuaFunctionDefs::GetElementAttachedTo );
    CLuaCFunctions::AddFunction ( "getAttachedElements", CLuaFunctionDefs::GetAttachedElements );
    CLuaCFunctions::AddFunction ( "getElementDistanceFromCentreOfMassToBaseOfModel", CLuaFunctionDefs::GetElementDistanceFromCentreOfMassToBaseOfModel );
    CLuaCFunctions::AddFunction ( "isElementLocal", CLuaFunctionDefs::IsElementLocal );
    CLuaCFunctions::AddFunction ( "getElementAttachedOffsets", CLuaFunctionDefs::GetElementAttachedOffsets );
    CLuaCFunctions::AddFunction ( "getElementAlpha", CLuaFunctionDefs::GetElementAlpha );
    CLuaCFunctions::AddFunction ( "isElementOnScreen", CLuaFunctionDefs::IsElementOnScreen );
    CLuaCFunctions::AddFunction ( "getElementHealth", CLuaFunctionDefs::GetElementHealth );
    CLuaCFunctions::AddFunction ( "getElementModel", CLuaFunctionDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "isElementStreamedIn", CLuaFunctionDefs::IsElementStreamedIn );
    CLuaCFunctions::AddFunction ( "isElementStreamable", CLuaFunctionDefs::IsElementStreamable );
    CLuaCFunctions::AddFunction ( "getElementColShape", CLuaFunctionDefs::GetElementColShape );
    CLuaCFunctions::AddFunction ( "isElementInWater", CLuaFunctionDefs::IsElementInWater );
    CLuaCFunctions::AddFunction ( "isElementSyncer", CLuaFunctionDefs::IsElementSyncer );
    CLuaCFunctions::AddFunction ( "isElementCollidableWith", CLuaFunctionDefs::IsElementCollidableWith );
    CLuaCFunctions::AddFunction ( "isElementDoubleSided", CLuaFunctionDefs::IsElementDoubleSided );
    CLuaCFunctions::AddFunction ( "getElementCollisionsEnabled", CLuaFunctionDefs::GetElementCollisionsEnabled );
    CLuaCFunctions::AddFunction ( "isElementFrozen", CLuaFunctionDefs::IsElementFrozen );
    CLuaCFunctions::AddFunction ( "getLowLODElement", CLuaFunctionDefs::GetLowLodElement );
    CLuaCFunctions::AddFunction ( "isElementLowLOD", CLuaFunctionDefs::IsElementLowLod );
    CLuaCFunctions::AddFunction ( "isElementCallPropagationEnabled", CLuaFunctionDefs::IsElementCallPropagationEnabled );
    CLuaCFunctions::AddFunction ( "isElementWaitingForGroundToLoad", CLuaFunctionDefs::IsElementWaitingForGroundToLoad );

    // Element set funcs
    CLuaCFunctions::AddFunction ( "createElement", CLuaFunctionDefs::CreateElement );
    CLuaCFunctions::AddFunction ( "destroyElement", CLuaFunctionDefs::DestroyElement );
    CLuaCFunctions::AddFunction ( "setElementID", CLuaFunctionDefs::SetElementID );
    CLuaCFunctions::AddFunction ( "setElementParent", CLuaFunctionDefs::SetElementParent );
    CLuaCFunctions::AddFunction ( "setElementData", CLuaFunctionDefs::SetElementData );
    //CLuaCFunctions::AddFunction ( "removeElementData", CLuaFunctionDefs::RemoveElementData ); TODO Clientside
    CLuaCFunctions::AddFunction ( "setElementMatrix", CLuaFunctionDefs::SetElementMatrix );
    CLuaCFunctions::AddFunction ( "setElementPosition", CLuaFunctionDefs::SetElementPosition );
    CLuaCFunctions::AddFunction ( "setElementRotation", CLuaFunctionDefs::SetElementRotation );
    CLuaCFunctions::AddFunction ( "setElementVelocity", CLuaFunctionDefs::SetElementVelocity );
    CLuaCFunctions::AddFunction ( "setElementInterior", CLuaFunctionDefs::SetElementInterior );
    CLuaCFunctions::AddFunction ( "setElementDimension", CLuaFunctionDefs::SetElementDimension );
    CLuaCFunctions::AddFunction ( "attachElements", CLuaFunctionDefs::AttachElements );
    CLuaCFunctions::AddFunction ( "detachElements", CLuaFunctionDefs::DetachElements );
    CLuaCFunctions::AddFunction ( "setElementAttachedOffsets", CLuaFunctionDefs::SetElementAttachedOffsets );
    CLuaCFunctions::AddFunction ( "setElementAlpha", CLuaFunctionDefs::SetElementAlpha );
    CLuaCFunctions::AddFunction ( "setElementHealth", CLuaFunctionDefs::SetElementHealth );
    CLuaCFunctions::AddFunction ( "setElementModel", CLuaFunctionDefs::SetElementModel );
    CLuaCFunctions::AddFunction ( "setElementStreamable", CLuaFunctionDefs::SetElementStreamable );
    CLuaCFunctions::AddFunction ( "setElementCollisionsEnabled", CLuaFunctionDefs::SetElementCollisionsEnabled );
    CLuaCFunctions::AddFunction ( "setElementCollidableWith", CLuaFunctionDefs::SetElementCollidableWith );
    CLuaCFunctions::AddFunction ( "setElementDoubleSided", CLuaFunctionDefs::SetElementDoubleSided );
    CLuaCFunctions::AddFunction ( "setElementFrozen", CLuaFunctionDefs::SetElementFrozen );
    CLuaCFunctions::AddFunction ( "setLowLODElement", CLuaFunctionDefs::SetLowLodElement );
    CLuaCFunctions::AddFunction ( "setElementCallPropagationEnabled", CLuaFunctionDefs::SetElementCallPropagationEnabled );

    // Clothes and body functions
    CLuaCFunctions::AddFunction ( "getBodyPartName", CLuaFunctionDefs::GetBodyPartName );
    CLuaCFunctions::AddFunction ( "getClothesByTypeIndex", CLuaFunctionDefs::GetClothesByTypeIndex );
    CLuaCFunctions::AddFunction ( "getTypeIndexFromClothes", CLuaFunctionDefs::GetTypeIndexFromClothes );
    CLuaCFunctions::AddFunction ( "getClothesTypeName", CLuaFunctionDefs::GetClothesTypeName );

    // Explosion funcs
    CLuaCFunctions::AddFunction ( "createExplosion", CLuaFunctionDefs::CreateExplosion );

    // Fire funcs
    CLuaCFunctions::AddFunction ( "createFire", CLuaFunctionDefs::CreateFire );

    // Engine funcs
    CLuaCFunctions::AddFunction ( "engineLoadTXD", CLuaFunctionDefs::EngineLoadTXD );
    CLuaCFunctions::AddFunction ( "engineLoadCOL", CLuaFunctionDefs::EngineLoadCOL );
    CLuaCFunctions::AddFunction ( "engineLoadDFF", CLuaFunctionDefs::EngineLoadDFF );
    CLuaCFunctions::AddFunction ( "engineImportTXD", CLuaFunctionDefs::EngineImportTXD );
    CLuaCFunctions::AddFunction ( "engineReplaceCOL", CLuaFunctionDefs::EngineReplaceCOL );
    CLuaCFunctions::AddFunction ( "engineRestoreCOL", CLuaFunctionDefs::EngineRestoreCOL );
    CLuaCFunctions::AddFunction ( "engineReplaceModel", CLuaFunctionDefs::EngineReplaceModel );
    CLuaCFunctions::AddFunction ( "engineRestoreModel", CLuaFunctionDefs::EngineRestoreModel );
    CLuaCFunctions::AddFunction ( "engineGetModelLODDistance", CLuaFunctionDefs::EngineGetModelLODDistance );
    CLuaCFunctions::AddFunction ( "engineSetModelLODDistance", CLuaFunctionDefs::EngineSetModelLODDistance );
    CLuaCFunctions::AddFunction ( "engineSetAsynchronousLoading", CLuaFunctionDefs::EngineSetAsynchronousLoading );
    CLuaCFunctions::AddFunction ( "engineApplyShaderToWorldTexture", CLuaFunctionDefs::EngineApplyShaderToWorldTexture );
    CLuaCFunctions::AddFunction ( "engineRemoveShaderFromWorldTexture", CLuaFunctionDefs::EngineRemoveShaderFromWorldTexture );
    CLuaCFunctions::AddFunction ( "engineGetModelNameFromID", CLuaFunctionDefs::EngineGetModelNameFromID );
    CLuaCFunctions::AddFunction ( "engineGetModelIDFromName", CLuaFunctionDefs::EngineGetModelIDFromName );
    CLuaCFunctions::AddFunction ( "engineGetModelTextureNames", CLuaFunctionDefs::EngineGetModelTextureNames );
    CLuaCFunctions::AddFunction ( "engineGetVisibleTextureNames", CLuaFunctionDefs::EngineGetVisibleTextureNames );

    //CLuaCFunctions::AddFunction ( "engineReplaceMatchingAtomics", CLuaFunctionDefs::EngineReplaceMatchingAtomics );
    //CLuaCFunctions::AddFunction ( "engineReplaceWheelAtomics", CLuaFunctionDefs::EngineReplaceWheelAtomics );
    //CLuaCFunctions::AddFunction ( "enginePositionAtomic", CLuaFunctionDefs::EnginePositionAtomic );
    //CLuaCFunctions::AddFunction ( "enginePositionSeats", CLuaFunctionDefs::EnginePositionSeats );
    //CLuaCFunctions::AddFunction ( "engineAddAllAtomics", CLuaFunctionDefs::EngineAddAllAtomics );
    //CLuaCFunctions::AddFunction ( "engineReplaceVehiclePart", CLuaFunctionDefs::EngineReplaceVehiclePart );

    // Drawing funcs
    CLuaCFunctions::AddFunction ( "dxDrawLine", CLuaFunctionDefs::dxDrawLine );
    CLuaCFunctions::AddFunction ( "dxDrawMaterialLine3D", CLuaFunctionDefs::dxDrawMaterialLine3D );
    CLuaCFunctions::AddFunction ( "dxDrawMaterialSectionLine3D", CLuaFunctionDefs::dxDrawMaterialSectionLine3D );
    CLuaCFunctions::AddFunction ( "dxDrawLine3D", CLuaFunctionDefs::dxDrawLine3D );
    CLuaCFunctions::AddFunction ( "dxDrawText", CLuaFunctionDefs::dxDrawText );
    CLuaCFunctions::AddFunction ( "dxDrawRectangle", CLuaFunctionDefs::dxDrawRectangle );
    CLuaCFunctions::AddFunction ( "dxDrawImage", CLuaFunctionDefs::dxDrawImage );
    CLuaCFunctions::AddFunction ( "dxDrawImageSection", CLuaFunctionDefs::dxDrawImageSection );
    CLuaCFunctions::AddFunction ( "dxGetTextWidth", CLuaFunctionDefs::dxGetTextWidth );
    CLuaCFunctions::AddFunction ( "dxGetFontHeight", CLuaFunctionDefs::dxGetFontHeight );
    CLuaCFunctions::AddFunction ( "dxCreateFont", CLuaFunctionDefs::dxCreateFont );
    CLuaCFunctions::AddFunction ( "dxCreateTexture", CLuaFunctionDefs::dxCreateTexture );
    CLuaCFunctions::AddFunction ( "dxCreateShader", CLuaFunctionDefs::dxCreateShader );
    CLuaCFunctions::AddFunction ( "dxCreateRenderTarget", CLuaFunctionDefs::dxCreateRenderTarget );
    CLuaCFunctions::AddFunction ( "dxCreateScreenSource", CLuaFunctionDefs::dxCreateScreenSource );
    CLuaCFunctions::AddFunction ( "dxGetMaterialSize", CLuaFunctionDefs::dxGetMaterialSize );
    CLuaCFunctions::AddFunction ( "dxSetShaderValue", CLuaFunctionDefs::dxSetShaderValue );
    CLuaCFunctions::AddFunction ( "dxSetShaderTessellation", CLuaFunctionDefs::dxSetShaderTessellation );
    CLuaCFunctions::AddFunction ( "dxSetShaderTransform", CLuaFunctionDefs::dxSetShaderTransform );
    CLuaCFunctions::AddFunction ( "dxSetRenderTarget", CLuaFunctionDefs::dxSetRenderTarget );
    CLuaCFunctions::AddFunction ( "dxUpdateScreenSource", CLuaFunctionDefs::dxUpdateScreenSource );
    CLuaCFunctions::AddFunction ( "dxGetStatus", CLuaFunctionDefs::dxGetStatus );
    CLuaCFunctions::AddFunction ( "dxSetTestMode", CLuaFunctionDefs::dxSetTestMode );
    CLuaCFunctions::AddFunction ( "dxGetTexturePixels", CLuaFunctionDefs::dxGetTexturePixels );
    CLuaCFunctions::AddFunction ( "dxSetTexturePixels", CLuaFunctionDefs::dxSetTexturePixels );
    CLuaCFunctions::AddFunction ( "dxGetPixelsSize", CLuaFunctionDefs::dxGetPixelsSize );
    CLuaCFunctions::AddFunction ( "dxGetPixelsFormat", CLuaFunctionDefs::dxGetPixelsFormat );
    CLuaCFunctions::AddFunction ( "dxConvertPixels", CLuaFunctionDefs::dxConvertPixels );
    CLuaCFunctions::AddFunction ( "dxGetPixelColor", CLuaFunctionDefs::dxGetPixelColor );
    CLuaCFunctions::AddFunction ( "dxSetPixelColor", CLuaFunctionDefs::dxSetPixelColor );
    CLuaCFunctions::AddFunction ( "dxSetBlendMode", CLuaFunctionDefs::dxSetBlendMode );
    CLuaCFunctions::AddFunction ( "dxGetBlendMode", CLuaFunctionDefs::dxGetBlendMode );
    CLuaCFunctions::AddFunction ( "dxSetAspectRatioAdjustmentEnabled", CLuaFunctionDefs::dxSetAspectRatioAdjustmentEnabled );
    CLuaCFunctions::AddFunction ( "dxIsAspectRatioAdjustmentEnabled", CLuaFunctionDefs::dxIsAspectRatioAdjustmentEnabled );
    CLuaCFunctions::AddFunction ( "dxSetTextureEdge", CLuaFunctionDefs::dxSetTextureEdge );

    // Radar-area funcs
    CLuaCFunctions::AddFunction ( "createRadarArea", CLuaFunctionDefs::CreateRadarArea );
    CLuaCFunctions::AddFunction ( "getRadarAreaColor", CLuaFunctionDefs::GetRadarAreaColor  );
    CLuaCFunctions::AddFunction ( "getRadarAreaSize", CLuaFunctionDefs::GetRadarAreaSize );
    CLuaCFunctions::AddFunction ( "isRadarAreaFlashing", CLuaFunctionDefs::IsRadarAreaFlashing );
    CLuaCFunctions::AddFunction ( "setRadarAreaColor", CLuaFunctionDefs::SetRadarAreaColor );
    CLuaCFunctions::AddFunction ( "setRadarAreaFlashing", CLuaFunctionDefs::SetRadarAreaFlashing );
    CLuaCFunctions::AddFunction ( "setRadarAreaSize", CLuaFunctionDefs::SetRadarAreaSize );
    CLuaCFunctions::AddFunction ( "isInsideRadarArea", CLuaFunctionDefs::IsInsideRadarArea );

    // Pickup create/destroy funcs
    CLuaCFunctions::AddFunction ( "createPickup", CLuaFunctionDefs::CreatePickup );

    // Pickup get funcs
    CLuaCFunctions::AddFunction ( "getPickupType", CLuaFunctionDefs::GetPickupType );
    CLuaCFunctions::AddFunction ( "getPickupWeapon", CLuaFunctionDefs::GetPickupWeapon );
    CLuaCFunctions::AddFunction ( "getPickupAmount", CLuaFunctionDefs::GetPickupAmount );
    CLuaCFunctions::AddFunction ( "getPickupAmmo", CLuaFunctionDefs::GetPickupAmmo );

    // Pickup set funcs
    CLuaCFunctions::AddFunction ( "setPickupType", CLuaFunctionDefs::SetPickupType );

    // Cam get funcs
    CLuaCFunctions::AddFunction ( "getCamera", CLuaFunctionDefs::GetCamera );
    CLuaCFunctions::AddFunction ( "getCameraViewMode", CLuaFunctionDefs::GetCameraViewMode );
    CLuaCFunctions::AddFunction ( "getCameraMatrix", CLuaFunctionDefs::GetCameraMatrix );
    CLuaCFunctions::AddFunction ( "getCameraTarget", CLuaFunctionDefs::GetCameraTarget );
    CLuaCFunctions::AddFunction ( "getCameraInterior", CLuaFunctionDefs::GetCameraInterior );
    CLuaCFunctions::AddFunction ( "getCameraGoggleEffect", CLuaFunctionDefs::GetCameraGoggleEffect );
    CLuaCFunctions::AddFunction ( "getCameraShakeLevel", CLuaFunctionDefs::GetCameraShakeLevel );
    CLuaCFunctions::AddFunction ( "getCameraFieldOfView", CLuaFunctionDefs::GetCameraFieldOfView );

    // Cam set funcs
    CLuaCFunctions::AddFunction ( "setCameraMatrix", CLuaFunctionDefs::SetCameraMatrix );
    CLuaCFunctions::AddFunction ( "setCameraFieldOfView", CLuaFunctionDefs::SetCameraFieldOfView );
    CLuaCFunctions::AddFunction ( "setCameraTarget", CLuaFunctionDefs::SetCameraTarget );
    CLuaCFunctions::AddFunction ( "setCameraInterior", CLuaFunctionDefs::SetCameraInterior );
    CLuaCFunctions::AddFunction ( "fadeCamera", CLuaFunctionDefs::FadeCamera );
    CLuaCFunctions::AddFunction ( "setCameraClip", CLuaFunctionDefs::SetCameraClip );
    CLuaCFunctions::AddFunction ( "getCameraClip", CLuaFunctionDefs::GetCameraClip );
    CLuaCFunctions::AddFunction ( "setCameraViewMode", CLuaFunctionDefs::SetCameraViewMode );
    CLuaCFunctions::AddFunction ( "setCameraGoggleEffect", CLuaFunctionDefs::SetCameraGoggleEffect );
    CLuaCFunctions::AddFunction ( "setCameraShakeLevel", CLuaFunctionDefs::SetCameraShakeLevel );

    // Cursor funcs
    CLuaCFunctions::AddFunction ( "getCursorPosition", CLuaFunctionDefs::GetCursorPosition );
    CLuaCFunctions::AddFunction ( "setCursorPosition", CLuaFunctionDefs::SetCursorPosition );
    CLuaCFunctions::AddFunction ( "isCursorShowing", CLuaFunctionDefs::IsCursorShowing );
    CLuaCFunctions::AddFunction ( "showCursor", CLuaFunctionDefs::ShowCursor );
    CLuaCFunctions::AddFunction ( "getCursorAlpha", CLuaFunctionDefs::GetCursorAlpha );
    CLuaCFunctions::AddFunction ( "setCursorAlpha", CLuaFunctionDefs::SetCursorAlpha );

    // GUI funcs
    CLuaCFunctions::AddFunction ( "guiGetInputEnabled", CLuaFunctionDefs::GUIGetInputEnabled );
    CLuaCFunctions::AddFunction ( "guiSetInputEnabled", CLuaFunctionDefs::GUISetInputEnabled );
    CLuaCFunctions::AddFunction ( "guiGetInputMode", CLuaFunctionDefs::GUIGetInputMode );
    CLuaCFunctions::AddFunction ( "guiSetInputMode", CLuaFunctionDefs::GUISetInputMode );

    CLuaCFunctions::AddFunction ( "isChatBoxInputActive", CLuaFunctionDefs::GUIIsChatBoxInputActive );
    CLuaCFunctions::AddFunction ( "isConsoleActive", CLuaFunctionDefs::GUIIsConsoleActive );
    CLuaCFunctions::AddFunction ( "isDebugViewActive", CLuaFunctionDefs::GUIIsDebugViewActive );
    CLuaCFunctions::AddFunction ( "isMainMenuActive", CLuaFunctionDefs::GUIIsMainMenuActive );
    CLuaCFunctions::AddFunction ( "isMTAWindowActive", CLuaFunctionDefs::GUIIsMTAWindowActive );
    CLuaCFunctions::AddFunction ( "isTransferBoxActive", CLuaFunctionDefs::GUIIsTransferBoxActive );

    CLuaCFunctions::AddFunction ( "guiCreateWindow", CLuaFunctionDefs::GUICreateWindow );
    CLuaCFunctions::AddFunction ( "guiCreateLabel", CLuaFunctionDefs::GUICreateLabel );
    CLuaCFunctions::AddFunction ( "guiCreateButton", CLuaFunctionDefs::GUICreateButton );
    CLuaCFunctions::AddFunction ( "guiCreateEdit", CLuaFunctionDefs::GUICreateEdit );
    CLuaCFunctions::AddFunction ( "guiCreateMemo", CLuaFunctionDefs::GUICreateMemo );
    CLuaCFunctions::AddFunction ( "guiCreateGridList", CLuaFunctionDefs::GUICreateGridList );
    CLuaCFunctions::AddFunction ( "guiCreateScrollPane", CLuaFunctionDefs::GUICreateScrollPane );
    CLuaCFunctions::AddFunction ( "guiCreateScrollBar", CLuaFunctionDefs::GUICreateScrollBar );
    CLuaCFunctions::AddFunction ( "guiCreateTabPanel", CLuaFunctionDefs::GUICreateTabPanel );
    CLuaCFunctions::AddFunction ( "guiCreateTab", CLuaFunctionDefs::GUICreateTab );
    CLuaCFunctions::AddFunction ( "guiCreateProgressBar", CLuaFunctionDefs::GUICreateProgressBar );
    CLuaCFunctions::AddFunction ( "guiCreateCheckBox", CLuaFunctionDefs::GUICreateCheckBox );
    CLuaCFunctions::AddFunction ( "guiCreateRadioButton", CLuaFunctionDefs::GUICreateRadioButton );
    CLuaCFunctions::AddFunction ( "guiCreateStaticImage", CLuaFunctionDefs::GUICreateStaticImage );
    CLuaCFunctions::AddFunction ( "guiCreateComboBox", CLuaFunctionDefs::GUICreateComboBox );
    CLuaCFunctions::AddFunction ( "guiCreateFont", CLuaFunctionDefs::GUICreateFont );

    CLuaCFunctions::AddFunction ( "guiStaticImageLoadImage", CLuaFunctionDefs::GUIStaticImageLoadImage );
    CLuaCFunctions::AddFunction ( "guiStaticImageGetNativeSize", CLuaFunctionDefs::GUIStaticImageGetNativeSize );
    CLuaCFunctions::AddFunction ( "guiGetSelectedTab", CLuaFunctionDefs::GUIGetSelectedTab );
    CLuaCFunctions::AddFunction ( "guiSetSelectedTab", CLuaFunctionDefs::GUISetSelectedTab );
    CLuaCFunctions::AddFunction ( "guiDeleteTab", CLuaFunctionDefs::GUIDeleteTab );

    CLuaCFunctions::AddFunction ( "guiGridListSetSortingEnabled", CLuaFunctionDefs::GUIGridListSetSortingEnabled );
    CLuaCFunctions::AddFunction ( "guiGridListAddColumn", CLuaFunctionDefs::GUIGridListAddColumn );
    CLuaCFunctions::AddFunction ( "guiGridListRemoveColumn", CLuaFunctionDefs::GUIGridListRemoveColumn );
    CLuaCFunctions::AddFunction ( "guiGridListSetColumnWidth", CLuaFunctionDefs::GUIGridListSetColumnWidth );
    CLuaCFunctions::AddFunction ( "guiGridListGetColumnWidth", CLuaFunctionDefs::GUIGridListGetColumnWidth );
    CLuaCFunctions::AddFunction ( "guiGridListSetColumnTitle", CLuaFunctionDefs::GUIGridListSetColumnTitle );
    CLuaCFunctions::AddFunction ( "guiGridListGetColumnTitle", CLuaFunctionDefs::GUIGridListGetColumnTitle );
    CLuaCFunctions::AddFunction ( "guiGridListSetScrollBars", CLuaFunctionDefs::GUIGridListSetScrollBars );
    CLuaCFunctions::AddFunction ( "guiGridListGetRowCount", CLuaFunctionDefs::GUIGridListGetRowCount );
    CLuaCFunctions::AddFunction ( "guiGridListGetColumnCount", CLuaFunctionDefs::GUIGridListGetColumnCount );
    CLuaCFunctions::AddFunction ( "guiGridListAddRow", CLuaFunctionDefs::GUIGridListAddRow );
    CLuaCFunctions::AddFunction ( "guiGridListInsertRowAfter", CLuaFunctionDefs::GUIGridListInsertRowAfter );
    CLuaCFunctions::AddFunction ( "guiGridListRemoveRow", CLuaFunctionDefs::GUIGridListRemoveRow );
    CLuaCFunctions::AddFunction ( "guiGridListAutoSizeColumn", CLuaFunctionDefs::GUIGridListAutoSizeColumn );
    CLuaCFunctions::AddFunction ( "guiGridListClear", CLuaFunctionDefs::GUIGridListClear );
    CLuaCFunctions::AddFunction ( "guiGridListSetItemText", CLuaFunctionDefs::GUIGridListSetItemText );
    CLuaCFunctions::AddFunction ( "guiGridListGetItemText", CLuaFunctionDefs::GUIGridListGetItemText );
    CLuaCFunctions::AddFunction ( "guiGridListSetItemData", CLuaFunctionDefs::GUIGridListSetItemData );
    CLuaCFunctions::AddFunction ( "guiGridListGetItemData", CLuaFunctionDefs::GUIGridListGetItemData );
    CLuaCFunctions::AddFunction ( "guiGridListSetItemColor", CLuaFunctionDefs::GUIGridListSetItemColor );
    CLuaCFunctions::AddFunction ( "guiGridListGetItemColor", CLuaFunctionDefs::GUIGridListGetItemColor );
    CLuaCFunctions::AddFunction ( "guiGridListSetSelectionMode", CLuaFunctionDefs::GUIGridListSetSelectionMode );
    CLuaCFunctions::AddFunction ( "guiGridListGetSelectedItem", CLuaFunctionDefs::GUIGridListGetSelectedItem );
    CLuaCFunctions::AddFunction ( "guiGridListGetSelectedItems", CLuaFunctionDefs::GUIGridListGetSelectedItems );
    CLuaCFunctions::AddFunction ( "guiGridListGetSelectedCount", CLuaFunctionDefs::GUIGridListGetSelectedCount );
    CLuaCFunctions::AddFunction ( "guiGridListSetSelectedItem", CLuaFunctionDefs::GUIGridListSetSelectedItem );
    CLuaCFunctions::AddFunction ( "guiGridListSetHorizontalScrollPosition", CLuaFunctionDefs::GUIGridListSetHorizontalScrollPosition );
    CLuaCFunctions::AddFunction ( "guiGridListGetHorizontalScrollPosition", CLuaFunctionDefs::GUIGridListGetHorizontalScrollPosition );
    CLuaCFunctions::AddFunction ( "guiGridListSetVerticalScrollPosition", CLuaFunctionDefs::GUIGridListSetVerticalScrollPosition );
    CLuaCFunctions::AddFunction ( "guiGridListGetVerticalScrollPosition", CLuaFunctionDefs::GUIGridListGetVerticalScrollPosition );

    CLuaCFunctions::AddFunction ( "guiScrollPaneSetScrollBars", CLuaFunctionDefs::GUIScrollPaneSetScrollBars );
    CLuaCFunctions::AddFunction ( "guiScrollPaneSetHorizontalScrollPosition", CLuaFunctionDefs::GUIScrollPaneSetHorizontalScrollPosition );
    CLuaCFunctions::AddFunction ( "guiScrollPaneGetHorizontalScrollPosition", CLuaFunctionDefs::GUIScrollPaneGetHorizontalScrollPosition );
    CLuaCFunctions::AddFunction ( "guiScrollPaneSetVerticalScrollPosition", CLuaFunctionDefs::GUIScrollPaneSetVerticalScrollPosition );
    CLuaCFunctions::AddFunction ( "guiScrollPaneGetVerticalScrollPosition", CLuaFunctionDefs::GUIScrollPaneGetVerticalScrollPosition );

    CLuaCFunctions::AddFunction ( "guiScrollBarSetScrollPosition", CLuaFunctionDefs::GUIScrollBarSetScrollPosition );
    CLuaCFunctions::AddFunction ( "guiScrollBarGetScrollPosition", CLuaFunctionDefs::GUIScrollBarGetScrollPosition );

    CLuaCFunctions::AddFunction ( "guiSetEnabled", CLuaFunctionDefs::GUISetEnabled );
    CLuaCFunctions::AddFunction ( "guiSetProperty", CLuaFunctionDefs::GUISetProperty );
    CLuaCFunctions::AddFunction ( "guiSetAlpha", CLuaFunctionDefs::GUISetAlpha );
    CLuaCFunctions::AddFunction ( "guiSetText", CLuaFunctionDefs::GUISetText );
    CLuaCFunctions::AddFunction ( "guiSetFont", CLuaFunctionDefs::GUISetFont );
    CLuaCFunctions::AddFunction ( "guiSetSize", CLuaFunctionDefs::GUISetSize );
    CLuaCFunctions::AddFunction ( "guiSetPosition", CLuaFunctionDefs::GUISetPosition );
    CLuaCFunctions::AddFunction ( "guiSetVisible", CLuaFunctionDefs::GUISetVisible );

    CLuaCFunctions::AddFunction ( "guiBringToFront", CLuaFunctionDefs::GUIBringToFront );
    CLuaCFunctions::AddFunction ( "guiMoveToBack", CLuaFunctionDefs::GUIMoveToBack );

    CLuaCFunctions::AddFunction ( "guiCheckBoxSetSelected", CLuaFunctionDefs::GUICheckBoxSetSelected );
    CLuaCFunctions::AddFunction ( "guiRadioButtonSetSelected", CLuaFunctionDefs::GUIRadioButtonSetSelected );

    CLuaCFunctions::AddFunction ( "guiGetEnabled", CLuaFunctionDefs::GUIGetEnabled );
    CLuaCFunctions::AddFunction ( "guiGetProperty", CLuaFunctionDefs::GUIGetProperty );
    CLuaCFunctions::AddFunction ( "guiGetProperties", CLuaFunctionDefs::GUIGetProperties );
    CLuaCFunctions::AddFunction ( "guiGetAlpha", CLuaFunctionDefs::GUIGetAlpha );
    CLuaCFunctions::AddFunction ( "guiGetText", CLuaFunctionDefs::GUIGetText );
    CLuaCFunctions::AddFunction ( "guiGetFont", CLuaFunctionDefs::GUIGetFont );
    CLuaCFunctions::AddFunction ( "guiGetSize", CLuaFunctionDefs::GUIGetSize );
    CLuaCFunctions::AddFunction ( "guiGetPosition", CLuaFunctionDefs::GUIGetPosition );
    CLuaCFunctions::AddFunction ( "guiGetVisible", CLuaFunctionDefs::GUIGetVisible );

    CLuaCFunctions::AddFunction ( "guiCheckBoxGetSelected", CLuaFunctionDefs::GUICheckBoxGetSelected );
    CLuaCFunctions::AddFunction ( "guiRadioButtonGetSelected", CLuaFunctionDefs::GUIRadioButtonGetSelected );

    CLuaCFunctions::AddFunction ( "guiProgressBarSetProgress", CLuaFunctionDefs::GUIProgressBarSetProgress );
    CLuaCFunctions::AddFunction ( "guiProgressBarGetProgress", CLuaFunctionDefs::GUIProgressBarGetProgress );

    CLuaCFunctions::AddFunction ( "guiGetScreenSize", CLuaFunctionDefs::GUIGetScreenSize );

    CLuaCFunctions::AddFunction ( "guiEditSetCaretIndex", CLuaFunctionDefs::GUIEditSetCaretIndex );
    CLuaCFunctions::AddFunction ( "guiEditGetCaretIndex", CLuaFunctionDefs::GUIEditGetCaretIndex );
    CLuaCFunctions::AddFunction ( "guiEditSetMasked", CLuaFunctionDefs::GUIEditSetMasked );
    CLuaCFunctions::AddFunction ( "guiEditSetMaxLength", CLuaFunctionDefs::GUIEditSetMaxLength );
    CLuaCFunctions::AddFunction ( "guiEditSetReadOnly", CLuaFunctionDefs::GUIEditSetReadOnly );

    CLuaCFunctions::AddFunction ( "guiMemoSetCaretIndex", CLuaFunctionDefs::GUIMemoSetCaretIndex );
    CLuaCFunctions::AddFunction ( "guiMemoGetCaretIndex", CLuaFunctionDefs::GUIMemoGetCaretIndex );
    CLuaCFunctions::AddFunction ( "guiMemoSetReadOnly", CLuaFunctionDefs::GUIMemoSetReadOnly );

    CLuaCFunctions::AddFunction ( "guiLabelSetColor", CLuaFunctionDefs::GUILabelSetColor );
    CLuaCFunctions::AddFunction ( "guiLabelGetColor", CLuaFunctionDefs::GUILabelGetColor );
    CLuaCFunctions::AddFunction ( "guiLabelSetVerticalAlign", CLuaFunctionDefs::GUILabelSetVerticalAlign );
    CLuaCFunctions::AddFunction ( "guiLabelSetHorizontalAlign", CLuaFunctionDefs::GUILabelSetHorizontalAlign );

    CLuaCFunctions::AddFunction ( "guiLabelGetTextExtent", CLuaFunctionDefs::GUILabelGetTextExtent );
    CLuaCFunctions::AddFunction ( "guiLabelGetFontHeight", CLuaFunctionDefs::GUILabelGetFontHeight );

    CLuaCFunctions::AddFunction ( "guiWindowSetMovable", CLuaFunctionDefs::GUIWindowSetMovable );
    CLuaCFunctions::AddFunction ( "guiWindowSetSizable", CLuaFunctionDefs::GUIWindowSetSizable );

    CLuaCFunctions::AddFunction ( "getChatboxLayout", CLuaFunctionDefs::GUIGetChatboxLayout );

    CLuaCFunctions::AddFunction ( "guiComboBoxAddItem", CLuaFunctionDefs::GUIComboBoxAddItem );
    CLuaCFunctions::AddFunction ( "guiComboBoxRemoveItem", CLuaFunctionDefs::GUIComboBoxRemoveItem );
    CLuaCFunctions::AddFunction ( "guiComboBoxClear", CLuaFunctionDefs::GUIComboBoxClear );
    CLuaCFunctions::AddFunction ( "guiComboBoxGetSelected", CLuaFunctionDefs::GUIComboBoxGetSelected );
    CLuaCFunctions::AddFunction ( "guiComboBoxSetSelected", CLuaFunctionDefs::GUIComboBoxSetSelected );
    CLuaCFunctions::AddFunction ( "guiComboBoxGetItemText", CLuaFunctionDefs::GUIComboBoxGetItemText );
    CLuaCFunctions::AddFunction ( "guiComboBoxSetItemText", CLuaFunctionDefs::GUIComboBoxSetItemText );


    // Util functions
    CLuaCFunctions::AddFunction ( "gettok", CLuaFunctionDefs::GetTok );
    CLuaCFunctions::AddFunction ( "split", CLuaFunctionDefs::Split );
    CLuaCFunctions::AddFunction ( "setTimer", CLuaFunctionDefs::SetTimer );
    CLuaCFunctions::AddFunction ( "killTimer", CLuaFunctionDefs::KillTimer );
    CLuaCFunctions::AddFunction ( "resetTimer", CLuaFunctionDefs::ResetTimer );
    CLuaCFunctions::AddFunction ( "getTimers", CLuaFunctionDefs::GetTimers );
    CLuaCFunctions::AddFunction ( "isTimer", CLuaFunctionDefs::IsTimer );
    CLuaCFunctions::AddFunction ( "getTimerDetails", CLuaFunctionDefs::GetTimerDetails );
    CLuaCFunctions::AddFunction ( "getTickCount", CLuaFunctionDefs::GetTickCount_ );
    CLuaCFunctions::AddFunction ( "getRealTime", CLuaFunctionDefs::GetCTime );
    CLuaCFunctions::AddFunction ( "tocolor", CLuaFunctionDefs::tocolor );
    CLuaCFunctions::AddFunction ( "ref", CLuaFunctionDefs::Reference );
    CLuaCFunctions::AddFunction ( "deref", CLuaFunctionDefs::Dereference );
    CLuaCFunctions::AddFunction ( "getColorFromString", CLuaFunctionDefs::GetColorFromString );
    CLuaCFunctions::AddFunction ( "getValidPedModels", CLuaFunctionDefs::GetValidPedModels );
    CLuaCFunctions::AddFunction ( "downloadFile", CLuaFunctionDefs::DownloadFile );

    // World get functions
    CLuaCFunctions::AddFunction ( "getTime", CLuaFunctionDefs::GetTime_ );    
    CLuaCFunctions::AddFunction ( "getGroundPosition", CLuaFunctionDefs::GetGroundPosition );
    CLuaCFunctions::AddFunction ( "processLineOfSight", CLuaFunctionDefs::ProcessLineOfSight );
    CLuaCFunctions::AddFunction ( "isLineOfSightClear", CLuaFunctionDefs::IsLineOfSightClear );
    CLuaCFunctions::AddFunction ( "testLineAgainstWater", CLuaFunctionDefs::TestLineAgainstWater );
    CLuaCFunctions::AddFunction ( "getWaterLevel", CLuaFunctionDefs::GetWaterLevel );
    CLuaCFunctions::AddFunction ( "getWaterVertexPosition", CLuaFunctionDefs::GetWaterVertexPosition );
    CLuaCFunctions::AddFunction ( "getWorldFromScreenPosition", CLuaFunctionDefs::GetWorldFromScreenPosition );
    CLuaCFunctions::AddFunction ( "getScreenFromWorldPosition", CLuaFunctionDefs::GetScreenFromWorldPosition );
    CLuaCFunctions::AddFunction ( "getWeather", CLuaFunctionDefs::GetWeather );
    CLuaCFunctions::AddFunction ( "getZoneName", CLuaFunctionDefs::GetZoneName );
    CLuaCFunctions::AddFunction ( "getGravity", CLuaFunctionDefs::GetGravity );
    CLuaCFunctions::AddFunction ( "getGameSpeed", CLuaFunctionDefs::GetGameSpeed );
    CLuaCFunctions::AddFunction ( "getMinuteDuration", CLuaFunctionDefs::GetMinuteDuration );
    CLuaCFunctions::AddFunction ( "getWaveHeight", CLuaFunctionDefs::GetWaveHeight );
    CLuaCFunctions::AddFunction ( "isGarageOpen", CLuaFunctionDefs::IsGarageOpen );
    CLuaCFunctions::AddFunction ( "getGaragePosition", CLuaFunctionDefs::GetGaragePosition );
    CLuaCFunctions::AddFunction ( "getGarageSize", CLuaFunctionDefs::GetGarageSize );
    CLuaCFunctions::AddFunction ( "getGarageBoundingBox", CLuaFunctionDefs::GetGarageBoundingBox );
    CLuaCFunctions::AddFunction ( "isWorldSpecialPropertyEnabled", CLuaFunctionDefs::IsWorldSpecialPropertyEnabled );
    CLuaCFunctions::AddFunction ( "getBlurLevel", CLuaFunctionDefs::GetBlurLevel );
    CLuaCFunctions::AddFunction ( "getTrafficLightState", CLuaFunctionDefs::GetTrafficLightState );
    CLuaCFunctions::AddFunction ( "areTrafficLightsLocked", CLuaFunctionDefs::AreTrafficLightsLocked );
    CLuaCFunctions::AddFunction ( "getSkyGradient", CLuaFunctionDefs::GetSkyGradient );
    CLuaCFunctions::AddFunction ( "getHeatHaze", CLuaFunctionDefs::GetHeatHaze );
    CLuaCFunctions::AddFunction ( "getWaterColor", CLuaFunctionDefs::GetWaterColor );
    CLuaCFunctions::AddFunction ( "getJetpackMaxHeight", CLuaFunctionDefs::GetJetpackMaxHeight );
    CLuaCFunctions::AddFunction ( "getWindVelocity", CLuaFunctionDefs::GetWindVelocity );
    CLuaCFunctions::AddFunction ( "getInteriorSoundsEnabled", CLuaFunctionDefs::GetInteriorSoundsEnabled );
    CLuaCFunctions::AddFunction ( "getInteriorFurnitureEnabled", CLuaFunctionDefs::GetInteriorFurnitureEnabled );
    CLuaCFunctions::AddFunction ( "getFarClipDistance", CLuaFunctionDefs::GetFarClipDistance );
    CLuaCFunctions::AddFunction ( "getNearClipDistance", CLuaFunctionDefs::GetNearClipDistance );
    CLuaCFunctions::AddFunction ( "getFogDistance", CLuaFunctionDefs::GetFogDistance );
    CLuaCFunctions::AddFunction ( "getSunColor", CLuaFunctionDefs::GetSunColor );
    CLuaCFunctions::AddFunction ( "getSunSize", CLuaFunctionDefs::GetSunSize );
    CLuaCFunctions::AddFunction ( "getAircraftMaxHeight", CLuaFunctionDefs::GetAircraftMaxHeight );
    CLuaCFunctions::AddFunction ( "getAircraftMaxVelocity", CLuaFunctionDefs::GetAircraftMaxVelocity );
    CLuaCFunctions::AddFunction ( "getOcclusionsEnabled", CLuaFunctionDefs::GetOcclusionsEnabled );
    CLuaCFunctions::AddFunction ( "getCloudsEnabled", CLuaFunctionDefs::GetCloudsEnabled );
    CLuaCFunctions::AddFunction ( "getRainLevel", CLuaFunctionDefs::GetRainLevel );
    CLuaCFunctions::AddFunction ( "setMoonSize", CLuaFunctionDefs::SetMoonSize );
    CLuaCFunctions::AddFunction ( "getMoonSize", CLuaFunctionDefs::GetMoonSize );
    CLuaCFunctions::AddFunction ( "resetMoonSize", CLuaFunctionDefs::ResetMoonSize );
    CLuaCFunctions::AddFunction ( "setFPSLimit", CLuaFunctionDefs::SetFPSLimit );
    CLuaCFunctions::AddFunction ( "getFPSLimit", CLuaFunctionDefs::GetFPSLimit );
    CLuaCFunctions::AddFunction ( "fetchRemote", CLuaFunctionDefs::FetchRemote );

    // World set funcs
    CLuaCFunctions::AddFunction ( "setTime", CLuaFunctionDefs::SetTime );
    CLuaCFunctions::AddFunction ( "createWater", CLuaFunctionDefs::CreateWater );
    CLuaCFunctions::AddFunction ( "setWaterLevel", CLuaFunctionDefs::SetWaterLevel );
    CLuaCFunctions::AddFunction ( "resetWaterLevel", CLuaFunctionDefs::ResetWaterLevel );
    CLuaCFunctions::AddFunction ( "setWaterVertexPosition", CLuaFunctionDefs::SetWaterVertexPosition );
    CLuaCFunctions::AddFunction ( "setWaterDrawnLast", CLuaFunctionDefs::SetWaterDrawnLast );
    CLuaCFunctions::AddFunction ( "isWaterDrawnLast", CLuaFunctionDefs::IsWaterDrawnLast );
    CLuaCFunctions::AddFunction ( "setSkyGradient", CLuaFunctionDefs::SetSkyGradient );
    CLuaCFunctions::AddFunction ( "resetSkyGradient", CLuaFunctionDefs::ResetSkyGradient );
    CLuaCFunctions::AddFunction ( "setHeatHaze", CLuaFunctionDefs::SetHeatHaze );
    CLuaCFunctions::AddFunction ( "resetHeatHaze", CLuaFunctionDefs::ResetHeatHaze );
    CLuaCFunctions::AddFunction ( "setWaterColor", CLuaFunctionDefs::SetWaterColor );
    CLuaCFunctions::AddFunction ( "resetWaterColor", CLuaFunctionDefs::ResetWaterColor );
    CLuaCFunctions::AddFunction ( "setWeather", CLuaFunctionDefs::SetWeather );
    CLuaCFunctions::AddFunction ( "setWeatherBlended", CLuaFunctionDefs::SetWeatherBlended );
    CLuaCFunctions::AddFunction ( "setGravity", CLuaFunctionDefs::SetGravity );
    CLuaCFunctions::AddFunction ( "setGameSpeed", CLuaFunctionDefs::SetGameSpeed );
    CLuaCFunctions::AddFunction ( "setWaveHeight", CLuaFunctionDefs::SetWaveHeight );
    CLuaCFunctions::AddFunction ( "setMinuteDuration", CLuaFunctionDefs::SetMinuteDuration );
    CLuaCFunctions::AddFunction ( "setGarageOpen", CLuaFunctionDefs::SetGarageOpen );
    CLuaCFunctions::AddFunction ( "setWorldSpecialPropertyEnabled", CLuaFunctionDefs::SetWorldSpecialPropertyEnabled );
    CLuaCFunctions::AddFunction ( "setBlurLevel", CLuaFunctionDefs::SetBlurLevel );
    CLuaCFunctions::AddFunction ( "setJetpackMaxHeight", CLuaFunctionDefs::SetJetpackMaxHeight );
    CLuaCFunctions::AddFunction ( "setCloudsEnabled", CLuaFunctionDefs::SetCloudsEnabled );
    CLuaCFunctions::AddFunction ( "setTrafficLightState", CLuaFunctionDefs::SetTrafficLightState );
    CLuaCFunctions::AddFunction ( "setTrafficLightsLocked", CLuaFunctionDefs::SetTrafficLightsLocked );
    CLuaCFunctions::AddFunction ( "setWindVelocity", CLuaFunctionDefs::SetWindVelocity );
    CLuaCFunctions::AddFunction ( "resetWindVelocity", CLuaFunctionDefs::ResetWindVelocity );
    CLuaCFunctions::AddFunction ( "setInteriorSoundsEnabled", CLuaFunctionDefs::SetInteriorSoundsEnabled );
    CLuaCFunctions::AddFunction ( "setInteriorFurnitureEnabled", CLuaFunctionDefs::SetInteriorFurnitureEnabled );
    CLuaCFunctions::AddFunction ( "setRainLevel", CLuaFunctionDefs::SetRainLevel );
    CLuaCFunctions::AddFunction ( "resetRainLevel", CLuaFunctionDefs::ResetRainLevel );
    CLuaCFunctions::AddFunction ( "setFarClipDistance", CLuaFunctionDefs::SetFarClipDistance );
    CLuaCFunctions::AddFunction ( "resetFarClipDistance", CLuaFunctionDefs::ResetFarClipDistance );
    CLuaCFunctions::AddFunction ( "setNearClipDistance", CLuaFunctionDefs::SetNearClipDistance );
    CLuaCFunctions::AddFunction ( "resetNearClipDistance", CLuaFunctionDefs::ResetNearClipDistance );
    CLuaCFunctions::AddFunction ( "setFogDistance", CLuaFunctionDefs::SetFogDistance );
    CLuaCFunctions::AddFunction ( "resetFogDistance", CLuaFunctionDefs::ResetFogDistance );
    CLuaCFunctions::AddFunction ( "setSunColor", CLuaFunctionDefs::SetSunColor );
    CLuaCFunctions::AddFunction ( "resetSunColor", CLuaFunctionDefs::ResetSunColor );
    CLuaCFunctions::AddFunction ( "setSunSize", CLuaFunctionDefs::SetSunSize );
    CLuaCFunctions::AddFunction ( "resetSunSize", CLuaFunctionDefs::ResetSunSize );
    CLuaCFunctions::AddFunction ( "removeWorldModel", CLuaFunctionDefs::RemoveWorldBuilding );
    CLuaCFunctions::AddFunction ( "restoreAllWorldModels", CLuaFunctionDefs::RestoreWorldBuildings );
    CLuaCFunctions::AddFunction ( "restoreWorldModel", CLuaFunctionDefs::RestoreWorldBuilding );
    CLuaCFunctions::AddFunction ( "setAircraftMaxHeight", CLuaFunctionDefs::SetAircraftMaxHeight );
    CLuaCFunctions::AddFunction ( "setAircraftMaxVelocity", CLuaFunctionDefs::SetAircraftMaxVelocity );
    CLuaCFunctions::AddFunction ( "setOcclusionsEnabled", CLuaFunctionDefs::SetOcclusionsEnabled );
    CLuaCFunctions::AddFunction ( "createSWATRope", CLuaFunctionDefs::CreateSWATRope );
    CLuaCFunctions::AddFunction ( "setBirdsEnabled", CLuaFunctionDefs::SetBirdsEnabled );
    CLuaCFunctions::AddFunction ( "getBirdsEnabled", CLuaFunctionDefs::GetBirdsEnabled );
    CLuaCFunctions::AddFunction ( "setPedTargetingMarkerEnabled", CLuaFunctionDefs::SetPedTargetingMarkerEnabled );
    CLuaCFunctions::AddFunction ( "isPedTargetingMarkerEnabled", CLuaFunctionDefs::IsPedTargetingMarkerEnabled );

    // Input functions
    CLuaCFunctions::AddFunction ( "bindKey", CLuaFunctionDefs::BindKey );
    CLuaCFunctions::AddFunction ( "unbindKey", CLuaFunctionDefs::UnbindKey );
    CLuaCFunctions::AddFunction ( "getKeyState", CLuaFunctionDefs::GetKeyState );    
    CLuaCFunctions::AddFunction ( "getControlState", CLuaFunctionDefs::GetControlState );
    CLuaCFunctions::AddFunction ( "getAnalogControlState", CLuaFunctionDefs::GetAnalogControlState );
    CLuaCFunctions::AddFunction ( "setAnalogControlState", CLuaFunctionDefs::SetAnalogControlState );
    CLuaCFunctions::AddFunction ( "isControlEnabled", CLuaFunctionDefs::IsControlEnabled );
    CLuaCFunctions::AddFunction ( "getBoundKeys", CLuaFunctionDefs::GetBoundKeys );
    CLuaCFunctions::AddFunction ( "getFunctionsBoundToKey", CLuaFunctionDefs::GetFunctionsBoundToKey );
    CLuaCFunctions::AddFunction ( "getKeyBoundToFunction", CLuaFunctionDefs::GetKeyBoundToFunction );
    CLuaCFunctions::AddFunction ( "getCommandsBoundToKey", CLuaFunctionDefs::GetCommandsBoundToKey );
    CLuaCFunctions::AddFunction ( "getKeyBoundToCommand", CLuaFunctionDefs::GetKeyBoundToCommand );
    
    CLuaCFunctions::AddFunction ( "setControlState", CLuaFunctionDefs::SetControlState );
    CLuaCFunctions::AddFunction ( "toggleControl", CLuaFunctionDefs::ToggleControl );
    CLuaCFunctions::AddFunction ( "toggleAllControls", CLuaFunctionDefs::ToggleAllControls );

    // XML functions
    CLuaCFunctions::AddFunction ( "xmlFindChild", CLuaFunctionDefs::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlNodeGetChildren", CLuaFunctionDefs::XMLNodeGetChildren );
    CLuaCFunctions::AddFunction ( "xmlNodeGetValue", CLuaFunctionDefs::XMLNodeGetValue );
    CLuaCFunctions::AddFunction ( "xmlNodeSetValue", CLuaFunctionDefs::XMLNodeSetValue );
    CLuaCFunctions::AddFunction ( "xmlNodeGetAttributes", CLuaFunctionDefs::XMLNodeGetAttributes );
    CLuaCFunctions::AddFunction ( "xmlNodeGetAttribute", CLuaFunctionDefs::XMLNodeGetAttribute );
    CLuaCFunctions::AddFunction ( "xmlNodeSetAttribute", CLuaFunctionDefs::XMLNodeSetAttribute );
    CLuaCFunctions::AddFunction ( "xmlNodeGetParent", CLuaFunctionDefs::XMLNodeGetParent );
    CLuaCFunctions::AddFunction ( "xmlLoadFile", CLuaFunctionDefs::XMLLoadFile );
    CLuaCFunctions::AddFunction ( "xmlCreateFile", CLuaFunctionDefs::XMLCreateFile );
    CLuaCFunctions::AddFunction ( "xmlUnloadFile", CLuaFunctionDefs::XMLUnloadFile );
    CLuaCFunctions::AddFunction ( "xmlSaveFile", CLuaFunctionDefs::XMLSaveFile );

    CLuaCFunctions::AddFunction ( "xmlCreateChild", CLuaFunctionDefs::XMLCreateChild );
    CLuaCFunctions::AddFunction ( "xmlDestroyNode", CLuaFunctionDefs::XMLDestroyNode );
    CLuaCFunctions::AddFunction ( "xmlCopyFile", CLuaFunctionDefs::XMLCopyFile );

    CLuaCFunctions::AddFunction ( "xmlNodeGetName", CLuaFunctionDefs::XMLNodeGetName );
    CLuaCFunctions::AddFunction ( "xmlNodeSetName", CLuaFunctionDefs::XMLNodeSetName );

    // Projectile funcs
    CLuaCFunctions::AddFunction ( "createProjectile", CLuaFunctionDefs::CreateProjectile );
    CLuaCFunctions::AddFunction ( "getProjectileType", CLuaFunctionDefs::GetProjectileType );
    CLuaCFunctions::AddFunction ( "getProjectileTarget", CLuaFunctionDefs::GetProjectileTarget );
    CLuaCFunctions::AddFunction ( "getProjectileCreator", CLuaFunctionDefs::GetProjectileCreator );
    CLuaCFunctions::AddFunction ( "getProjectileForce", CLuaFunctionDefs::GetProjectileForce );
    CLuaCFunctions::AddFunction ( "setProjectileCounter", CLuaFunctionDefs::SetProjectileCounter );
    CLuaCFunctions::AddFunction ( "getProjectileCounter", CLuaFunctionDefs::GetProjectileCounter );

    // Shape create funcs
    CLuaCFunctions::AddFunction ( "createColCircle", CLuaFunctionDefs::CreateColCircle );
    CLuaCFunctions::AddFunction ( "createColCuboid", CLuaFunctionDefs::CreateColCuboid );
    CLuaCFunctions::AddFunction ( "createColSphere", CLuaFunctionDefs::CreateColSphere );
    CLuaCFunctions::AddFunction ( "createColRectangle", CLuaFunctionDefs::CreateColRectangle );
    CLuaCFunctions::AddFunction ( "createColPolygon", CLuaFunctionDefs::CreateColPolygon );
    CLuaCFunctions::AddFunction ( "createColTube", CLuaFunctionDefs::CreateColTube );

    // Team get funcs
    CLuaCFunctions::AddFunction ( "getTeamFromName", CLuaFunctionDefs::GetTeamFromName );
    CLuaCFunctions::AddFunction ( "getTeamName", CLuaFunctionDefs::GetTeamName );
    CLuaCFunctions::AddFunction ( "getTeamColor", CLuaFunctionDefs::GetTeamColor );
    CLuaCFunctions::AddFunction ( "getTeamFriendlyFire", CLuaFunctionDefs::GetTeamFriendlyFire );
    CLuaCFunctions::AddFunction ( "getPlayersInTeam", CLuaFunctionDefs::GetPlayersInTeam );
    CLuaCFunctions::AddFunction ( "countPlayersInTeam", CLuaFunctionDefs::CountPlayersInTeam );

    // Weapon funcs
    CLuaCFunctions::AddFunction ( "getWeaponNameFromID", CLuaFunctionDefs::GetWeaponNameFromID );
    CLuaCFunctions::AddFunction ( "getWeaponIDFromName", CLuaFunctionDefs::GetWeaponIDFromName );
    CLuaCFunctions::AddFunction ( "getSlotFromWeapon", CLuaFunctionDefs::GetSlotFromWeapon );
    CLuaCFunctions::AddFunction ( "createWeapon", CLuaFunctionDefs::CreateWeapon );
    CLuaCFunctions::AddFunction ( "setWeaponProperty", CLuaFunctionDefs::SetWeaponProperty );
    CLuaCFunctions::AddFunction ( "fireWeapon", CLuaFunctionDefs::FireWeapon );
    CLuaCFunctions::AddFunction ( "setWeaponState", CLuaFunctionDefs::SetWeaponState );
    CLuaCFunctions::AddFunction ( "getWeaponState", CLuaFunctionDefs::GetWeaponState );
    CLuaCFunctions::AddFunction ( "setWeaponTarget", CLuaFunctionDefs::SetWeaponTarget );
    CLuaCFunctions::AddFunction ( "getWeaponTarget", CLuaFunctionDefs::GetWeaponTarget );
    //CLuaCFunctions::AddFunction ( "setWeaponOwner", CLuaFunctionDefs::SetWeaponOwner );
    CLuaCFunctions::AddFunction ( "getWeaponOwner", CLuaFunctionDefs::GetWeaponOwner );
    CLuaCFunctions::AddFunction ( "setWeaponFlags", CLuaFunctionDefs::SetWeaponFlags );
    CLuaCFunctions::AddFunction ( "getWeaponFlags", CLuaFunctionDefs::GetWeaponFlags );
    CLuaCFunctions::AddFunction ( "setWeaponFiringRate", CLuaFunctionDefs::SetWeaponFiringRate );
    CLuaCFunctions::AddFunction ( "getWeaponFiringRate", CLuaFunctionDefs::GetWeaponFiringRate );
    CLuaCFunctions::AddFunction ( "resetWeaponFiringRate", CLuaFunctionDefs::ResetWeaponFiringRate );
    CLuaCFunctions::AddFunction ( "getWeaponAmmo", CLuaFunctionDefs::GetWeaponAmmo );
    CLuaCFunctions::AddFunction ( "getWeaponClipAmmo", CLuaFunctionDefs::GetWeaponClipAmmo );
    CLuaCFunctions::AddFunction ( "setWeaponAmmo", CLuaFunctionDefs::SetWeaponAmmo );
    CLuaCFunctions::AddFunction ( "setWeaponClipAmmo", CLuaFunctionDefs::SetWeaponClipAmmo );

    // Command funcs
    CLuaCFunctions::AddFunction ( "addCommandHandler", CLuaFunctionDefs::AddCommandHandler );
    CLuaCFunctions::AddFunction ( "removeCommandHandler", CLuaFunctionDefs::RemoveCommandHandler );
    CLuaCFunctions::AddFunction ( "executeCommandHandler", CLuaFunctionDefs::ExecuteCommandHandler );

    // Utility vector math functions
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints2D", CLuaFunctionDefs::GetDistanceBetweenPoints2D );
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints3D", CLuaFunctionDefs::GetDistanceBetweenPoints3D );
    CLuaCFunctions::AddFunction ( "getEasingValue", CLuaFunctionDefs::GetEasingValue );
    CLuaCFunctions::AddFunction ( "interpolateBetween", CLuaFunctionDefs::InterpolateBetween );

    // Utility
    CLuaCFunctions::AddFunction ( "md5", CLuaFunctionDefs::Md5 );
    CLuaCFunctions::AddFunction ( "sha256", CLuaFunctionDefs::Sha256 );
    CLuaCFunctions::AddFunction ( "hash", CLuaFunctionDefs::Hash );
    CLuaCFunctions::AddFunction ( "teaEncode", CLuaFunctionDefs::TeaEncode );
    CLuaCFunctions::AddFunction ( "teaDecode", CLuaFunctionDefs::TeaDecode );
    CLuaCFunctions::AddFunction ( "base64Encode", CLuaFunctionDefs::Base64encode );
    CLuaCFunctions::AddFunction ( "base64Decode", CLuaFunctionDefs::Base64decode );
    CLuaCFunctions::AddFunction ( "getNetworkUsageData", CLuaFunctionDefs::GetNetworkUsageData );
    CLuaCFunctions::AddFunction ( "getNetworkStats", CLuaFunctionDefs::GetNetworkStats );
    CLuaCFunctions::AddFunction ( "getPerformanceStats", CLuaFunctionDefs::GetPerformanceStats );
    CLuaCFunctions::AddFunction ( "setDevelopmentMode", CLuaFunctionDefs::SetDevelopmentMode );
    CLuaCFunctions::AddFunction ( "getDevelopmentMode", CLuaFunctionDefs::GetDevelopmentMode );
    CLuaCFunctions::AddFunction ( "addDebugHook", CLuaFunctionDefs::AddDebugHook );
    CLuaCFunctions::AddFunction ( "removeDebugHook", CLuaFunctionDefs::RemoveDebugHook );
    CLuaCFunctions::AddFunction ( "isOOPEnabled", CLuaFunctionDefs::IsOOPEnabled );

    // Version functions
    CLuaCFunctions::AddFunction ( "getVersion", CLuaFunctionDefs::GetVersion );

    // Localization functions
    CLuaCFunctions::AddFunction ( "getLocalization", CLuaFunctionDefs::GetLocalization );

    // UTF functions
    CLuaCFunctions::AddFunction ( "utfLen", CLuaFunctionDefs::UtfLen );
    CLuaCFunctions::AddFunction ( "utfSeek", CLuaFunctionDefs::UtfSeek );
    CLuaCFunctions::AddFunction ( "utfSub", CLuaFunctionDefs::UtfSub );
    CLuaCFunctions::AddFunction ( "utfChar", CLuaFunctionDefs::UtfChar );
    CLuaCFunctions::AddFunction ( "utfCode", CLuaFunctionDefs::UtfCode );

    // PCRE functions
    CLuaCFunctions::AddFunction ( "pregFind", CLuaFunctionDefs::PregFind );
    CLuaCFunctions::AddFunction ( "pregReplace", CLuaFunctionDefs::PregReplace );
    CLuaCFunctions::AddFunction ( "pregMatch", CLuaFunctionDefs::PregMatch );

    // Voice functions
    CLuaCFunctions::AddFunction ( "isVoiceEnabled", CLuaFunctionDefs::IsVoiceEnabled );

    // JSON funcs
    CLuaCFunctions::AddFunction ( "toJSON", CLuaFunctionDefs::toJSON );
    CLuaCFunctions::AddFunction ( "fromJSON", CLuaFunctionDefs::fromJSON );

    // Luadef definitions
    CLuaAudioDefs::LoadFunctions ();
    CLuaBitDefs::LoadFunctions ();
    CLuaBlipDefs::LoadFunctions ();
    CLuaBrowserDefs::LoadFunctions ();
    CLuaFileDefs::LoadFunctions ();
    CLuaFxDefs::LoadFunctions ();
    CLuaMarkerDefs::LoadFunctions ();
    CLuaObjectDefs::LoadFunctions ();
    CLuaPedDefs::LoadFunctions ();
    CLuaPlayerDefs::LoadFunctions ();
    CLuaPointLightDefs::LoadFunctions ();
    CLuaSearchLightDefs::LoadFunctions ();
    CLuaTaskDefs::LoadFunctions ();
    CLuaVehicleDefs::LoadFunctions ();
}
