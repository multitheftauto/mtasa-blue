/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.iplfixes.cpp
*  PURPOSE:     IPL sector streaming
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

// Compatibility variables.
static CGameSA *pGameInterface = NULL;

// Things that need to be fixed.
#define HOOKPOS_AddBuildingInstancesToWorld_CWorldAdd             0x5B5348
DWORD JMP_CWorld_Add_AddBuildingInstancesToWorld_CALL_CWorldAdd = 0x563220;
DWORD RETURN_AddBuildingInstancesToWorld_CWorldAdd =              0x5B534D;

#define HOOKPOS_CWorld_Remove_CPopulation_ConvertToDummyObject    0x6146F8

#define HOOKPOS_CWorld_ADD_CPopulation_ConvertToRealObject              0x6145C7
DWORD JMP_CWorld_Add_CPopulation_ConvertToRealObject_Retn =             0x6145CC;
DWORD JMP_CWorld_Add_CPopulation_ConvertToRealObject_CallCWorldAdd =    0x563220;

#define HOOKPOS_ConvertToObject_CPopulationManageDummy            0x616091
DWORD CALL_Convert_To_Real_Object_CPopulation_ManageDummy = 0x614580;
DWORD JMP_RETN_Called_CPopulation_ManageDummy = 0x616097;
DWORD JMP_RETN_Cancel_CPopulation_ManageDummy = 0x616098;

#define HOOKPOS_CWorld_ADD_CPopulation_ConvertToDummyObject       0x61470C
DWORD CALL_CWorld_Add_CPopulation_ConvertToDummyObject = 0x563220;
DWORD JMP_RETN_Called_CPopulation_ConvertToDummyObject = 0x614712;
DWORD JMP_RETN_Cancelled_CPopulation_ConvertToDummyObject = 0x614715;

#define HOOKPOS_CBuilding_DTR                                     0x404180
DWORD JMP_CBuilding_DTR   =                                       0x535E90;

#define HOOKPOS_CDummy_DTR                                        0x532566
DWORD JMP_CDummy_DTR   =                                          0x535E90;

#define HOOKPOS_CObject_DTR                                       0x59F680
DWORD JMP_CObject_DTR  =                                          0x59F686;

// Variables
SIPLInst* pEntityWorldAdd = NULL;
CEntitySAInterface * pLODInterface = NULL; 
bool bNextHookSetModel = false;
bool bCodePathCheck = false;

bool CheckRemovedModelNoSet ( )
{
    // Init our variables
    bNextHookSetModel = false;
    bCodePathCheck = bNextHookSetModel;
    pLODInterface = NULL;
    CWorld* pWorld = pGameInterface->GetWorld();
    // You never know.
    if ( pWorld )
    {
        // Is the model in question even removed?
        if ( pWorld->IsModelRemoved ( pEntityWorldAdd->m_nModelIndex ) )
        {
            // is the replaced model in the spherical radius of any building removal
            if ( pGameInterface->GetWorld ( )->IsRemovedModelInRadius ( pEntityWorldAdd ) )
            {
                // if it is next hook remove it from the world
                return true;
            }
        }
        return false;
    }
    return false;
}
// Binary
bool CheckRemovedModel ( )
{
    TIMING_CHECKPOINT( "+CheckRemovedModel" );
    bNextHookSetModel = CheckRemovedModelNoSet ( );
    TIMING_CHECKPOINT( "-CheckRemovedModel" );
    bCodePathCheck = true;
    return bNextHookSetModel;
}

// Binary
// Hook 1
void __cdecl HOOK_LoadIPLInstance ( SIPLInst *pAddInst )
{
    pEntityWorldAdd = pAddInst;
    if ( pEntityWorldAdd )
    {
        CheckRemovedModel ( );
    }
}
static bool bTest = false;
// Binary
void HideEntitySomehow ( )
{
    TIMING_CHECKPOINT( "+HideEntitySomehow" );
    // Did we get instructed to set the model
    if ( bNextHookSetModel && pLODInterface )
    {
        // Init pInterface with the Initial model
        CEntitySAInterface * pInterface = pLODInterface;
        // Grab the removal for the interface
        SBuildingRemoval* pBuildingRemoval = pGameInterface->GetWorld ( )->GetBuildingRemoval ( pInterface );
        // Remove down the LOD tree
        if ( pBuildingRemoval && pInterface && pInterface != NULL && pInterface->bIsProcObject == 0 && ( pInterface->nType == ENTITY_TYPE_BUILDING || pInterface->nType == ENTITY_TYPE_DUMMY ) )
        {
            // Add the LOD to the list
            pBuildingRemoval->AddBinaryBuilding ( pInterface );
            // Remove the model from the world
            pGameInterface->GetWorld ( )->Remove ( pInterface, BuildingRemoval );
            // Get next LOD ( LOD's can have LOD's so we keep checking pInterface )
            //pInterface = pInterface->m_pLod;
        }
    }
    else if ( bCodePathCheck && pLODInterface )
    {
        // Init pInterface with the Initial model
        CEntitySAInterface * pInterface = pLODInterface;
        if ( pInterface && pInterface != NULL && pInterface->bIsProcObject == 0 && ( pInterface->nType == ENTITY_TYPE_BUILDING || pInterface->nType == ENTITY_TYPE_DUMMY ) )
        {
            pGameInterface->GetWorld()->AddBinaryBuilding ( pInterface );
        }
    }
    // Reset our next hook variable
    bNextHookSetModel = false;
    bCodePathCheck = bNextHookSetModel;
    TIMING_CHECKPOINT( "-HideEntitySomehow" );
}
// Binary
// Hook 2
void __cdecl HOOK_CWorld_LOD_SETUP ( CEntitySAInterface *pLodEntity )
{
    pLODInterface = pLodEntity;
    HideEntitySomehow ( );
}

CEntitySAInterface * pBuildingAdd = NULL;
void StorePointerToBuilding ( )
{
    if ( pBuildingAdd != NULL )
    {
        pGameInterface->GetWorld ( )->AddDataBuilding ( pBuildingAdd );
    }
}

// Called when a data entity is added to the world (this happens once when the game loads so we just dump those in a list and we can sift through when someone tries to remove.)
void _declspec(naked) Hook_AddBuildingInstancesToWorld ( )
{
    _asm
    {
        pushad
        mov pBuildingAdd, edx
    }
    StorePointerToBuilding ( );
    _asm
    {
        popad
        jmp JMP_CWorld_Add_AddBuildingInstancesToWorld_CALL_CWorldAdd
    }
}

bool CheckForRemoval ( )
{
    // Did we get instructed to set the model
    if ( pLODInterface )
    {
        // Init pInterface with the Initial model
        CEntitySAInterface * pInterface = pLODInterface;
        // Remove down the LOD tree
        if ( pGameInterface->GetWorld ( )->IsObjectRemoved ( pInterface ) )
        {
            return true;
        }
    }
    return false;
}

// Call to CWorld::Add in CPopulation::ConvertToRealObject we just use this to get a list of pointers to valid objects for instant removal
void _declspec(naked) Hook_CWorld_ADD_CPopulation_ConvertToRealObject ( )
{
    _asm
    {
        pushad
        mov pBuildingAdd, esi
        mov pLODInterface, esi
    }
    StorePointerToBuilding ( );
    _asm
    {
        popad
        jmp JMP_CWorld_Add_CPopulation_ConvertToRealObject_CallCWorldAdd
        jmp JMP_CWorld_Add_CPopulation_ConvertToRealObject_Retn
    }
}

void RemoveObjectIfNeeded ( )
{
    TIMING_CHECKPOINT( "+RemoveObjectIfNeeded" );
    SBuildingRemoval* pBuildingRemoval = pGameInterface->GetWorld ( )->GetBuildingRemoval ( pLODInterface );
    if ( pBuildingRemoval != NULL )
    {
        if ( *(DWORD*)(pBuildingAdd) != VTBL_CPlaceable )
        {
            pBuildingRemoval->AddDataBuilding ( pBuildingAdd );
            pGameInterface->GetWorld ( )->Remove( pBuildingAdd, BuildingRemoval3 );
        }

        if ( *(DWORD*)(pLODInterface) != VTBL_CPlaceable )
        {
            pBuildingRemoval->AddDataBuilding ( pLODInterface );
            pGameInterface->GetWorld ( )->Remove( pLODInterface, BuildingRemoval4 );
        }
    }
    TIMING_CHECKPOINT( "-RemoveObjectIfNeeded" );
}

// on stream in -> create and remove it from the world just after so we can restore easily
void _declspec (naked) HOOK_ConvertToObject_CPopulationManageDummy ( )
{
    _asm
    {
        pushad
        mov pBuildingAdd, edx
        mov pLODInterface, edx
    }
    _asm
    {
        popad
        push edx
        call CALL_Convert_To_Real_Object_CPopulation_ManageDummy
        pop ecx
        mov pLODInterface, ecx
        pushad
    }
    RemoveObjectIfNeeded ( );
    _asm
    {
        popad
        jmp JMP_RETN_Cancel_CPopulation_ManageDummy
    }
}

CEntitySAInterface * pBuildingRemove = NULL;
void RemovePointerToBuilding ( )
{
    if ( pBuildingRemove->nType == ENTITY_TYPE_BUILDING || pBuildingRemove->nType == ENTITY_TYPE_DUMMY || pBuildingRemove->nType == ENTITY_TYPE_OBJECT )
    {
        pGameInterface->GetWorld ( )->RemoveWorldBuildingFromLists ( pBuildingRemove );
    }
}

DWORD dwCWorldRemove = 0x563280;
// Call to CWorld::Remove in CPopulation::ConvertToDummyObject this is called just before deleting a CObject so we remove the CObject while we are there and remove the new dummy if we need to do so before returning
void _declspec(naked) HOOK_CWorld_Remove_CPopulation_ConvertToDummyObject ( )
{
    _asm
    {
        pushad
        mov pBuildingRemove, esi
        mov pBuildingAdd, edi
        mov pLODInterface, edi
    }
    TIMING_CHECKPOINT( "+RemovePointerToBuilding" );
    RemovePointerToBuilding ( );
    StorePointerToBuilding ( );
    RemoveObjectIfNeeded ( );
    TIMING_CHECKPOINT( "-RemovePointerToBuilding" );
    _asm
    {
        popad
        jmp dwCWorldRemove
    }
}
// if it's replaced get rid of it
// ***- is this even used?
void RemoveDummyIfReplaced ( )
{
    SBuildingRemoval* pBuildingRemoval = pGameInterface->GetWorld ( )->GetBuildingRemoval ( pLODInterface );
    if ( pBuildingRemoval != NULL )
    {
        if ( *(DWORD*)(pBuildingAdd) != VTBL_CPlaceable )
        {
            pBuildingRemoval->AddDataBuilding ( pBuildingAdd );
            pGameInterface->GetWorld ( )->Remove( pBuildingAdd, BuildingRemoval5 );
        }
    }
}

// Function that handles dummy -> object so we can cancel this process if need be
void _declspec(naked) HOOK_CWorld_Add_CPopulation_ConvertToDummyObject ( )
{
    _asm
    {
        pushad
        mov pLODInterface, edi
        mov pBuildingAdd, edi
    }

    TIMING_CHECKPOINT( "+CheckForRemoval" );
    StorePointerToBuilding ( );
    if ( CheckForRemoval ( ) )
    {
        TIMING_CHECKPOINT( "-CheckForRemoval" );
        _asm
        {
            popad
            jmp JMP_RETN_Cancelled_CPopulation_ConvertToDummyObject
        }
    }
    else
    {
        TIMING_CHECKPOINT( "-CheckForRemoval" );
        _asm
        {
            popad
            push edi
            call CALL_CWorld_Add_CPopulation_ConvertToDummyObject
            jmp JMP_RETN_Called_CPopulation_ConvertToDummyObject
        }
    }
}

// Destructors to catch element deletion so we can delete their entries
void _declspec(naked) Hook_CBuilding_DTR ( )
{
    _asm
    {
        pushad
        mov pBuildingRemove, ecx
    }
    RemovePointerToBuilding ( );
    _asm
    {
        popad
        jmp JMP_CBuilding_DTR
    }
}

void _declspec(naked) Hook_CDummy_DTR ( )
{
    _asm
    {
        pushad
        mov pBuildingRemove, ecx
    }
    RemovePointerToBuilding ( );
    _asm
    {
        popad
        jmp JMP_CDummy_DTR
    }
}

DWORD dwObjectVtbl = 0x866F60;
void _declspec(naked) Hook_CObject_DTR ( )
{
    _asm
    {
        pushad
        mov pBuildingRemove, esi
    }
    RemovePointerToBuilding ( );
    _asm
    {
        popad
        mov dword ptr [esi], offset dwObjectVtbl
        jmp JMP_CObject_DTR
    }
}

// Module initialization.
void StreamingIPLFIXES_Init( void )
{
    pGameInterface = pGame;

    // Start of Building removal hooks
    HookInstall ( HOOKPOS_CBuilding_DTR, (DWORD)Hook_CBuilding_DTR, 5 );

    HookInstall ( HOOKPOS_CDummy_DTR, (DWORD)Hook_CDummy_DTR, 5 );

    HookInstall ( HOOKPOS_CObject_DTR, (DWORD)Hook_CObject_DTR, 6 );

    HookInstallCall ( HOOKPOS_AddBuildingInstancesToWorld_CWorldAdd, (DWORD)Hook_AddBuildingInstancesToWorld );

    HookInstallCall( HOOKPOS_CWorld_ADD_CPopulation_ConvertToRealObject, (DWORD)Hook_CWorld_ADD_CPopulation_ConvertToRealObject );

    HookInstallCall( HOOKPOS_CWorld_Remove_CPopulation_ConvertToDummyObject, (DWORD)HOOK_CWorld_Remove_CPopulation_ConvertToDummyObject );

    HookInstall ( HOOKPOS_CWorld_ADD_CPopulation_ConvertToDummyObject, (DWORD)HOOK_CWorld_Add_CPopulation_ConvertToDummyObject, 6 );
    
    HookInstall ( HOOKPOS_ConvertToObject_CPopulationManageDummy, (DWORD)HOOK_ConvertToObject_CPopulationManageDummy, 6 );
    // End of building removal hooks
}

void StreamingIPLFIXES_Shutdown( void )
{

}