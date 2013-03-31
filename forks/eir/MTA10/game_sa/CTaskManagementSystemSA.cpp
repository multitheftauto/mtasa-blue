/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTaskManagementSystemSA.cpp
*  PURPOSE:     Task management system
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

//CMTATaskPool *mtaTaskPool;
CTaskSA *mtaTasks[MAX_TASKS];

// This is an example how game pools should be managed.
// Stop using dynamic lists and make use the simplicity of game pools:
// * They are preallocated memory and slot-based.
// Create an array of pointers and set the MTA interface into the slot which is handled by MTA.
// This way we ca distinguish MTA handled game classes from GTA:SA classes fairly easy.
// Looking up MTA classes takes very low execution time, as it will become an array offset operation.
// (no more scaling depending on class count)

// In order to create the same system for the vehicle interfaces, they should be seperated out properly.
// The same goes for ped and object interfaces.

void __cdecl HOOK_CTask_Operator_Delete( CTaskSAInterface *task )
{
    unsigned int id = (*ppTaskPool)->GetIndex( task );

    if ( CTaskSA *task = mtaTasks[id] )
    {
        task->DestroyJustThis();
        mtaTasks[id] = NULL;
    }

    // Free it from the pool
    (*ppTaskPool)->Free( id );
}

CTaskManagementSystemSA::CTaskManagementSystemSA ( void )
{
    // Install our hook used to delete our tasks when GTA does
    HookInstall ( FUNC_CTask_Operator_Delete, (DWORD)HOOK_CTask_Operator_Delete, 6 );

#ifdef MTA_INTERFACES_WITH_POOLS
    // Initiate our task pool
    mtaTaskPool = new CMTATaskPool;
#endif //MTA_INTERFACES_WITH_POOLS

    // Zero is all out
    memset( mtaTasks, 0, sizeof(mtaTasks) );
}

CTaskManagementSystemSA::~CTaskManagementSystemSA ( void )
{
#ifdef MTA_INTERFACES_WITH_POOLS
    delete mtaTaskPool;
#endif //MTA_INTERFACES_WITH_POOLS
}


CTask * CTaskManagementSystemSA::AddTask ( CTaskSA * pTask )
{
    if ( !pTask )
        return NULL;

    mtaTasks[(*ppTaskPool)->GetIndex( pTask->GetInterface() )] = pTask;
    return pTask;
}

CTask * CTaskManagementSystemSA::GetTask ( CTaskSAInterface * pTaskInterface )
{
    // Return NULL if we got passed NULL
    if ( pTaskInterface == 0 ) return NULL;

    if ( CTaskSA *task = mtaTasks[ (*ppTaskPool)->GetIndex( pTaskInterface ) ] )
        return task;

    // its not existed before, lets create the task
    // First, we create a temp task
    int iTaskType = pTaskInterface->GetTaskType();

    // Create it and add it to our list
    CTaskSA *pTask = dynamic_cast < CTaskSA* > ( CreateAppropriateTask ( pTaskInterface, iTaskType ) );

    AddTask( pTask );
    return pTask;
}

CTask * CTaskManagementSystemSA::CreateAppropriateTask ( CTaskSAInterface * pTaskInterface, int iTaskType )
{
    CTaskSA* pTaskSA = NULL;

    switch ( iTaskType )
    {
        // Attack
        case TASK_SIMPLE_GANG_DRIVEBY:
            pTaskSA = new CTaskSimpleGangDriveBySA;
            break;
        case TASK_SIMPLE_USE_GUN:
            pTaskSA = new CTaskSimpleUseGunSA;
            break;
        case TASK_SIMPLE_FIGHT:
            pTaskSA = new CTaskSimpleFightSA;
            break;
        
        // Basic
        case TASK_COMPLEX_USE_MOBILE_PHONE:
            pTaskSA = new CTaskComplexUseMobilePhoneSA;
            break;
        case TASK_SIMPLE_ANIM:
            pTaskSA = new CTaskSimpleRunAnimSA;
            break;
        case TASK_SIMPLE_NAMED_ANIM:
            pTaskSA = new CTaskSimpleRunNamedAnimSA;
            break;
        case TASK_COMPLEX_DIE:
            pTaskSA = new CTaskComplexDieSA;
            break;
        case TASK_SIMPLE_STEALTH_KILL:
            pTaskSA = new CTaskSimpleStealthKillSA;
            break;
        case TASK_COMPLEX_SUNBATHE:
            pTaskSA = new CTaskComplexSunbatheSA;
            break;
    
        // Car accessories
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
            pTaskSA = new CTaskSimpleCarSetPedInAsPassengerSA;
            break;
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
            pTaskSA = new CTaskSimpleCarSetPedInAsDriverSA;
            break;
        case TASK_SIMPLE_CAR_SET_PED_OUT:
            pTaskSA = new CTaskSimpleCarSetPedOutSA;
            break;

        // Car
        case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
            pTaskSA = new CTaskComplexEnterCarAsDriverSA;
            break;
        case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
            pTaskSA = new CTaskComplexEnterCarAsPassengerSA;
            break;
        case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
            pTaskSA = new CTaskComplexEnterBoatAsDriverSA;
            break;
        case TASK_COMPLEX_LEAVE_CAR:
            pTaskSA = new CTaskComplexLeaveCarSA;
            break;

        // GoTo
        case TASK_COMPLEX_WANDER:
            pTaskSA = new CTaskComplexWanderStandardSA;
            break;

        // IK
        case TASK_SIMPLE_IK_CHAIN:
            pTaskSA = new CTaskSimpleIKChainSA;
            break;
        case TASK_SIMPLE_IK_LOOK_AT:
            pTaskSA = new CTaskSimpleIKLookAtSA;
            break;
        case TASK_SIMPLE_IK_MANAGER:
            pTaskSA = new CTaskSimpleIKManagerSA;
            break;

        // JumpFall
        case TASK_SIMPLE_CLIMB:
            pTaskSA = new CTaskSimpleClimbSA;
            break;
        case TASK_SIMPLE_JETPACK:
            pTaskSA = new CTaskSimpleJetPackSA;  
            break;

        // Physical response
        case TASK_SIMPLE_CHOKING:
            pTaskSA = new CTaskSimpleChokingSA;
            break;

        // Secondary
        case TASK_SIMPLE_DUCK:
            pTaskSA = new CTaskSimpleDuckSA;
            break;    

        // Just create the baseclass
        default:
            pTaskSA = new CTaskSA;
            break;
    }

    assert ( pTaskSA && !pTaskSA->GetInterface () );

    // Set the internal interface
    pTaskSA->SetInterface ( pTaskInterface );
    return pTaskSA;
}