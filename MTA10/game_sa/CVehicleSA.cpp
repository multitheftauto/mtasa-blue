/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleSA.cpp
*  PURPOSE:     Vehicle base entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGameSA* pGame;

#include "gamesa_renderware.h"
namespace
{
    RpAtomic* ClumpDumpCB (RpAtomic* pAtomic, void * data)
    {
        CVehicleSA * pVehicleSA = (CVehicleSA *)data;
        RwFrame* pFrame = RpGetFrame ( pAtomic );   
        pVehicleSA->AddComponent ( pFrame, false );
        //g_pCore->GetConsole()->Print ( SString ( "Atomic:%08x  Frame:%08x %s", pAtomic, pFrame, pFrame ? pFrame->szName : "" ) );
        //OutputDebugLine ( SString ( "Atomic:%08x  Frame:%08x %s", pAtomic, pFrame, pFrame ? pFrame->szName : "" ) );
        return pAtomic;
    }

    void ClumpDump ( RpClump* pClump, CVehicleSA * pVehicleSA )
    {
        // get the clump's frame
        RwFrame* pFrame = RpGetFrame ( pClump );
        //OutputDebugLine ( SStringX ( "--------------------------------" ) );
        //OutputDebugLine ( SString ( "Clump:%08x  Frame:%08x %s", pClump, pFrame, pFrame ? pFrame->szName : "" ) );
        //g_pCore->GetConsole()->Print ( SStringX ( "--------------------------------" ) );
        //g_pCore->GetConsole()->Print ( SString ( "Clump:%08x  Frame:%08x %s", pClump, pFrame, pFrame ? pFrame->szName : "" ) );
        // Do for all atomics
        RpClumpForAllAtomics ( pClump, ClumpDumpCB, pVehicleSA );
    }
    
    // Recursive RwFrame children searching function
    void RwFrameDump ( RwFrame * parent, CVehicleSA * pVehicleSA ) {
        RwFrame * ret = parent->child;
        while ( ret != NULL ) {
            // recurse into the child
            if ( ret->child != NULL ) {         
                RwFrameDump ( ret, pVehicleSA );
            }
            // don't re-add, check ret for validity, if it has an empty string at this point it isn't a variant or it's already added
            if ( pVehicleSA->IsComponentPresent ( ret->szName ) == false && ret->szName != "" )
            {
                pVehicleSA->AddComponent ( ret, true );
            }
            ret = ret->next;
        }
    }

    void VehicleDump ( CVehicleSA* pVehicleSA )
    {
        // This grabs 90% of the frames
        ClumpDump( pVehicleSA->GetInterface()->m_pRwObject, pVehicleSA );    
        // This grabs the rest which aren't always copied to the Model Info in the interface ( usually markers for things like NOS ) 
        RwFrameDump( RpGetFrame ( (RpClump *)pGame->GetModelInfo ( pVehicleSA->GetModelIndex ( ) )->GetRwObject ( ) ), pVehicleSA );
    }

    RwObject * __cdecl GetAllAtomicObjectCB(struct RwObject * object, void * data)
    {
        std::vector< RwObject* >& result = *((std::vector< RwObject* >*)data);
        result.push_back( object );
        return object;
    }

    // Get all atomics for this frame (even if they are invisible)
    void GetAllAtomicObjects( RwFrame * frame, std::vector< RwObject* >& result )
    {
        RwFrameForAllObjects( frame, ( void * ) GetAllAtomicObjectCB, &result );
    }
}


CVehicleSA::CVehicleSA ()
    : m_ucAlpha ( 255 ), m_bIsDerailable ( true ), m_vecGravity ( 0.0f, 0.0f, -1.0f ), m_HeadLightColor ( SColorRGBA ( 255, 255, 255, 255 ) )
{
    assert ( 0 );   // Never used ?
}

/**
 *\todo ASAP: Remove all the VC specific (SCM) function calls propperly
 */
CVehicleSA::CVehicleSA( eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2 )
    : m_ucAlpha ( 255 ), m_bIsDerailable ( true ), m_vecGravity ( 0.0f, 0.0f, -1.0f ), m_HeadLightColor ( SColorRGBA ( 255, 255, 255, 255 ) )
{
    DEBUG_TRACE("CVehicleSA::CVehicleSA( eVehicleTypes dwModelID )");
    // for SA, we can just call the following function and it should just work:
    // CCarCtrl::CreateCarForScript(int,class CVector,unsigned char)
    //                              ModelID, Position, IsMissionVehicle

    m_pHandlingData = NULL;
    m_pSuspensionLines = NULL;

    DWORD dwReturn = 0;

    // Set Variation 1 before creation.
    MemSetFast( (void *)VAR_CVehicle_Variation1, ucVariation, 1 );
    MemSetFast( (void *)VAR_CVehicle_Variation2, ucVariation2, 1 );
    
    DWORD dwFunc = FUNC_CCarCtrlCreateCarForScript;
    _asm
    {
        push    0           // its a mission vehicle
        push    0
        push    0
        push    0           // spawn at 0,0,0
        push    dwModelID   
        call    dwFunc
        add     esp, 0x14
        mov     dwReturn, eax
    }

    m_pInterface = reinterpret_cast < CEntitySAInterface* > ( dwReturn );
#if 0
    this->BeingDeleted = FALSE;

    m_pInterface->bStreamingDontDelete = true;
    m_pInterface->bDontStream = true;
    
    // store our CVehicleSA pointer in the vehicle's time of creation member (as it won't get modified later and as far as I know it isn't used for something important)
    GetVehicleInterface ()->m_pVehicle = this;

    // Unlock doors as they spawn randomly with locked doors
    LockDoors ( false );

    // Reset the car countss to 0 so that this vehicle doesn't affect the population vehicles
    for ( int i = 0; i < 5; i++ )
    {
        MemPutFast < DWORD > ( VARS_CarCounts + i * sizeof(DWORD), 0 );
    }

    // only applicable for CAutomobile based vehicles (i.e. not bikes or boats, but includes planes, helis etc)
    this->damageManager = new CDamageManagerSA ( m_pInterface, (CDamageManagerSAInterface *)((DWORD)this->GetInterface() + 1440));


    // Replace the handling interface with our own to prevent handlig.cfg cheats and allow custom handling stuff.
    // We don't use SA's array because we want one handling per vehicle type and also allow custom handlings
    // per car later.
    /*CHandlingEntry* pEntry = pGame->GetHandlingManager ()->CreateHandlingData ();
    //CHandlingEntry* pEntry = pGame->GetHandlingManager ()->GetHandlingData ( dwModelID );
    pEntry->ApplyHandlingData ( pGame->GetHandlingManager ()->GetHandlingData ( dwModelID ) );  // We need to do that so vehicle handling wont get corrupted
    SetHandlingData ( pEntry );
    pEntry->Recalculate ();*/

    GetVehicleInterface ()->m_nVehicleFlags.bVehicleCanBeTargetted = true;

    this->internalID = pGame->GetPools ()->GetVehicleRef ( (DWORD *)this->GetVehicleInterface () );
#else
    m_ucVariant = ucVariation;
    m_ucVariant2 = ucVariation2;
    Init ();    // Use common setup
#endif
}

CVehicleSA::CVehicleSA ( CVehicleSAInterface* pVehicleInterface )
{
    m_pInterface = pVehicleInterface;
    m_pHandlingData = NULL;
    m_pSuspensionLines = NULL;

#if 0
    m_pInterface->bStreamingDontDelete = true;
    m_pInterface->bDontStream = true;
    this->BeingDeleted = FALSE;

    // Store our CVehicleSA pointer in the vehicle's time of creation member (as it won't get modified later and as far as I know it isn't used for something important)
    GetVehicleInterface ()->m_pVehicle = this;

    // Reset the car countss to 0 so that this vehicle doesn't affect the population vehicles
    for ( int i = 0; i < 5; i++ )
    {
        MemPutFast < DWORD > ( VARS_CarCounts + i * sizeof(DWORD), 0 );
    }

    // only applicable for CAutomobile based vehicles (i.e. not bikes, trains or boats, but includes planes, helis etc)
    this->damageManager = new CDamageManagerSA( m_pInterface, (CDamageManagerSAInterface *)((DWORD)this->GetInterface() + 1440));

    this->internalID = pGame->GetPools ()->GetVehicleRef ( (DWORD *)this->GetVehicleInterface () );

    m_bIsDerailable = true;
    m_ucAlpha = 255;
#else
    Init ();    // Use common setup
#endif
}

void CVehicleSA::Init ( void )
{
    m_pInterface->bStreamingDontDelete = true;
    m_pInterface->bDontStream = true;
    this->BeingDeleted = FALSE;

    // Store our CVehicleSA pointer in the vehicle's time of creation member (as it won't get modified later and as far as I know it isn't used for something important)
    GetVehicleInterface ()->m_pVehicle = this;

    // Unlock doors as they spawn randomly with locked doors
    LockDoors ( false );

    // Reset the car countss to 0 so that this vehicle doesn't affect the population vehicles
    for ( int i = 0; i < 5; i++ )
    {
        MemPutFast < DWORD > ( VARS_CarCounts + i * sizeof(DWORD), 0 );
    }

    // only applicable for CAutomobile based vehicles (i.e. not bikes, trains or boats, but includes planes, helis etc)
    this->m_pDamageManager = new CDamageManagerSA( m_pInterface, (CDamageManagerSAInterface *)((DWORD)this->GetInterface() + 1440));

    // Replace the handling interface with our own to prevent handlig.cfg cheats and allow custom handling stuff.
    // We don't use SA's array because we want one handling per vehicle type and also allow custom handlings
    // per car later.
    /*CHandlingEntry* pEntry = pGame->GetHandlingManager ()->CreateHandlingData ();
    //CHandlingEntry* pEntry = pGame->GetHandlingManager ()->GetHandlingData ( dwModelID );
    pEntry->ApplyHandlingData ( pGame->GetHandlingManager ()->GetHandlingData ( dwModelID ) );  // We need to do that so vehicle handling wont get corrupted
    SetHandlingData ( pEntry );
    pEntry->Recalculate ();*/

    GetVehicleInterface ()->m_nVehicleFlags.bVehicleCanBeTargetted = true;

    this->internalID = pGame->GetPools ()->GetVehicleRef ( (DWORD *)this->GetVehicleInterface () );

    m_bIsDerailable = true;
    m_ucAlpha = 255;
    m_vecGravity = CVector ( 0.0f, 0.0f, -1.0f );
    m_HeadLightColor = SColorRGBA ( 255, 255, 255, 255 );

    m_RGBColors[0] = CVehicleColor::GetRGBFromPaletteIndex ( ((CVehicleSAInterface *)(this->GetInterface()))->m_colour1 );
    m_RGBColors[1] = CVehicleColor::GetRGBFromPaletteIndex ( ((CVehicleSAInterface *)(this->GetInterface()))->m_colour2 );
    m_RGBColors[2] = CVehicleColor::GetRGBFromPaletteIndex ( ((CVehicleSAInterface *)(this->GetInterface()))->m_colour3 );
    m_RGBColors[3] = CVehicleColor::GetRGBFromPaletteIndex ( ((CVehicleSAInterface *)(this->GetInterface()))->m_colour4 );

    // Initialize doors depending on the vtable.
    DWORD dwOffset;
    DWORD dwFunc = ((CVehicleSAInterfaceVTBL *)this->GetVehicleInterface()->vtbl)->GetDoorAngleOpenRatio_;
    if ( dwFunc == FUNC_CAutomobile__GetDoorAngleOpenRatio )
        dwOffset = 1464;
    else if ( dwFunc == FUNC_CTrain__GetDoorAngleOpenRatio )
        dwOffset = 1496;
    else
        dwOffset = 0; // Other vehicles don't have door information.

    if ( dwOffset != 0 )
    {
        for ( unsigned int i = 0; i < sizeof(m_doors)/sizeof(m_doors[0]); ++i )
        {
            DWORD dwInterface = (DWORD)GetInterface ();
            DWORD dwDoorAddress = dwInterface + 1464 + i*24;
            m_doors[i].SetInterface ( (CDoorSAInterface *)dwDoorAddress );
        }
    }
    m_bSwingingDoorsAllowed = false;

    CopyGlobalSuspensionLinesToPrivate ();
    m_tSirenInfo.m_bOverrideSirens = false;
    m_tSirenInfo.m_bSirenSilent = false;

    for ( unsigned int i = 0; i < 8; i++ )
    {
        m_tSirenInfo.m_tSirenInfo[i].m_dwMinSirenAlpha = 0;
    }
    // set our variant count
    m_ucVariantCount = 0;
    // clear our rw frames list
    m_ExtraFrames.clear ( ); 
    // dump the frames
    VehicleDump( this );
}

// DESTRUCTOR
CVehicleSA::~CVehicleSA()
{
    DEBUG_TRACE("CVehicleSA::~CVehicleSA()");
    if(!this->BeingDeleted)
    {
        if ( (DWORD)m_pInterface->vtbl != VTBL_CPlaceable )
        {
            GetVehicleInterface ()->m_pVehicle = NULL;

            if ( m_pDamageManager )
            {
                delete m_pDamageManager;
                m_pDamageManager = NULL;
            }

            if ( m_pSuspensionLines )
            {
                delete [] m_pSuspensionLines;
                m_pSuspensionLines = NULL;
            }
            
            DWORD dwThis = (DWORD) m_pInterface;
            DWORD dwFunc = 0x6D2460;        // CVehicle::ExtinguishCarFire
            _asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }

            CWorldSA* pWorld = (CWorldSA *)pGame->GetWorld();
            pWorld->Remove ( m_pInterface, CVehicle_Destructor );
            pWorld->RemoveReferencesToDeletedObject ( m_pInterface );

            
            dwFunc = m_pInterface->vtbl->SCALAR_DELETING_DESTRUCTOR; // we use the vtbl so we can be vehicle type independent
            _asm
            {
                mov     ecx, dwThis
                push    1           //delete too
                call    dwFunc
            }
        }
        this->BeingDeleted = true;
        ((CPoolsSA *)pGame->GetPools())->RemoveVehicle((CVehicle *)this);
    }
}

VOID CVehicleSA::SetMoveSpeed ( CVector* vecMoveSpeed )
{
    DWORD dwFunc = FUNC_GetMoveSpeed;
    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwReturn = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax
    }
    MemCpyFast ((void *)dwReturn, vecMoveSpeed, sizeof(CVector));

    // INACCURATE. Use Get/SetTrainSpeed instead of Get/SetMoveSpeed. (Causes issue #4829).
#if 0
    // In case of train: calculate on-rail speed
    WORD wModelID = GetModelIndex();
    if ( wModelID == 537 || wModelID == 538 || wModelID == 569 || wModelID == 570 || wModelID == 590 || wModelID == 449 )
    {
        if ( !IsDerailed () )
        {
            CVehicleSAInterface* pInterf = GetVehicleInterface ();

            // Find the rail node we are on
            DWORD dwNumNodes = ((DWORD *)ARRAY_NumRailTrackNodes) [ pInterf->m_ucRailTrackID ];
            SRailNodeSA* pNode = ( (SRailNodeSA **) ARRAY_RailTrackNodePointers ) [ pInterf->m_ucRailTrackID ];
            SRailNodeSA* pNodesEnd = &pNode [ dwNumNodes ];
            while ( (float)pNode->sRailDistance / 3.0f <= pInterf->m_fTrainRailDistance && pNode < pNodesEnd )
            {
                pNode++;
            }
            if ( pNode >= pNodesEnd )
                return;
            // Get the direction vector between the nodes the train is between
            CVector vecNode1 ( (float)(pNode - 1)->sX / 8.0f, (float)(pNode - 1)->sY / 8.0f, (float)(pNode - 1)->sZ / 8.0f );
            CVector vecNode2 ( (float)pNode->sX / 8.0f, (float)pNode->sY / 8.0f, (float)pNode->sZ / 8.0f );
            CVector vecDirection = vecNode2 - vecNode1;
            vecDirection.Normalize ();
            // Set the speed
            pInterf->m_fTrainSpeed = vecDirection.DotProduct ( vecMoveSpeed );
        }
    }
#endif
}

CVehicleSAInterface * CVehicleSA::GetNextCarriageInTrain ( void )
{
    return (CVehicleSAInterface *)*(DWORD *)((DWORD)this->GetInterface() + 1492);
}

CVehicle * CVehicleSA::GetNextTrainCarriage ( void )
{
    CVehicleSAInterface * pVehicle = GetNextCarriageInTrain();
    if ( pVehicle )
        return pGame->GetPools()->GetVehicle ( (DWORD *)pVehicle );
    else
        return NULL;
}

bool CVehicleSA::AddProjectile ( eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector * target, CEntity * targetEntity )
{
    return ((CProjectileInfoSA*)pGame->GetProjectileInfo())->AddProjectile ( (CEntitySA*)this, eWeapon, vecOrigin, fForce, target, targetEntity );
}

void CVehicleSA::SetNextTrainCarriage ( CVehicle * next )
{
    if ( next )
    {
        CVehicleSA * pNextVehicle = dynamic_cast < CVehicleSA* > ( next );
        if ( pNextVehicle )
        {
            MemPutFast < DWORD > ( (DWORD)this->GetInterface () + 1492, (DWORD)pNextVehicle->GetInterface() );
            if ( pNextVehicle->GetPreviousTrainCarriage () != this )
                pNextVehicle->SetPreviousTrainCarriage ( this );
        }
    }
    else
    {
        MemPutFast < DWORD > ( (DWORD)this->GetInterface() + 1492, NULL );
    }
}


CVehicleSAInterface * CVehicleSA::GetPreviousCarriageInTrain ( void )
{
    return (CVehicleSAInterface *)*(DWORD *)((DWORD)this->GetInterface() + 1488);
}

void CVehicleSA::SetPreviousTrainCarriage ( CVehicle * previous )
{
    if ( previous )
    {
        CVehicleSA * pPreviousVehicle = dynamic_cast < CVehicleSA* > ( previous );
        if ( pPreviousVehicle )
        {
            MemPutFast < DWORD > ( (DWORD)this->GetInterface () + 1488, (DWORD)pPreviousVehicle->GetInterface() );
            if ( pPreviousVehicle->GetNextTrainCarriage () != this )
                pPreviousVehicle->SetNextTrainCarriage ( this );
        }
    }
    else
    {
        MemPutFast < DWORD > ( (DWORD)this->GetInterface() + 1488, NULL );
    }
}


CVehicle * CVehicleSA::GetPreviousTrainCarriage ( void )
{
    CVehicleSAInterface * pVehicle = GetPreviousCarriageInTrain();
    if ( pVehicle )
        return pGame->GetPools()->GetVehicle ( (DWORD *)pVehicle );
    else
        return NULL;
}


void CVehicleSA::AttachTrainCarriage ( CVehicle* pCarriage )
{
    if ( !pCarriage )
        return;

    CVehicleSA* pCarriageSA = dynamic_cast < CVehicleSA* > ( pCarriage );
    if ( !pCarriageSA )
        return;

    // Link both vehicles
    SetNextTrainCarriage ( pCarriage );

    // Mark the carriage as non chain engine
    CVehicleSAInterface* pCarriageInterface = pCarriage->GetVehicleInterface ();
    pCarriageInterface->trainFlags.bIsTheChainEngine = false;
    //pNextInterface->trainFlags.bIsDrivenByBrownSteak = true;
    
    if ( pCarriageInterface->trainFlags.bDirection )
    {
        CBoundingBox* pBoundingBox = pGame->GetModelInfo ( pCarriage->GetModelIndex() )->GetBoundingBox ();
        pCarriageInterface->m_fDistanceToNextCarriage = -( pBoundingBox->vecBoundMax.fY - pBoundingBox->vecBoundMin.fY );
    }
    else
    {
        CBoundingBox* pBoundingBox = pGame->GetModelInfo ( this->GetModelIndex() )->GetBoundingBox ();
        pCarriageInterface->m_fDistanceToNextCarriage = pBoundingBox->vecBoundMax.fY - pBoundingBox->vecBoundMin.fY;
    }
}


void CVehicleSA::DetachTrainCarriage ( CVehicle* pCarriage )
{
    SetNextTrainCarriage ( NULL );
    
    if ( pCarriage )
    {
        pCarriage->SetPreviousTrainCarriage ( NULL );
        pCarriage->SetChainEngine ( true );
    }
}


bool CVehicleSA::IsChainEngine ( void )
{
    return GetVehicleInterface ()->trainFlags.bIsTheChainEngine;
}


void CVehicleSA::SetChainEngine ( bool bChainEngine )
{
    GetVehicleInterface ()->trainFlags.bIsTheChainEngine = bChainEngine;
}


bool CVehicleSA::IsDerailed ( void )
{
    CVehicleSAInterface* pInterface = GetVehicleInterface ();
    return pInterface->trainFlags.bIsDerailed;
}


void CVehicleSA::SetDerailed ( bool bDerailed )
{
    WORD wModelID = GetModelIndex();
    if ( wModelID == 537 || wModelID == 538 || wModelID == 569 || wModelID == 570 || wModelID == 590 || wModelID == 449 )
    {
        CVehicleSAInterface* pInterface = GetVehicleInterface ();
        DWORD dwThis = (DWORD)pInterface;
        if ( bDerailed != pInterface->trainFlags.bIsDerailed )
        {

            if ( bDerailed )
            {
                pInterface->trainFlags.bIsDerailed = true;
                MemAndFast < DWORD > ( dwThis + 64, ( DWORD ) 0xFFFDFFFB );
            }
            else
            {
                pInterface->trainFlags.bIsDerailed = false;
                MemOrFast < DWORD > ( dwThis + 64, ( DWORD ) 0x20004 );

                // Recalculate the on-rail distance from the start node (train position parameter, m_fTrainRailDistance)
                DWORD dwFunc = FUNC_CVehicle_RecalcOnRailDistance;
                _asm
                {
                    mov     ecx, dwThis
                        call    dwFunc
                }

                // Reset the speed
                GetVehicleInterface ()->m_fTrainSpeed = 0.0f;
            }
        }
    }
}

float CVehicleSA::GetTrainSpeed ()
{
    return GetVehicleInterface ()->m_fTrainSpeed;
}


void CVehicleSA::SetTrainSpeed ( float fSpeed )
{
    GetVehicleInterface ()->m_fTrainSpeed = fSpeed;
}


bool CVehicleSA::GetTrainDirection ()
{
    return ( *( (BYTE *)GetInterface () + 1464 ) & 0x40 ) != 0;
}


void CVehicleSA::SetTrainDirection ( bool bDirection )
{
    if ( bDirection )
    {
        MemOrFast < BYTE > ( (BYTE*)GetInterface () + 1464, 0x40 );
    }
    else
    {
        MemAndFast < BYTE > ( (BYTE*)GetInterface () + 1464, ~0x40 );
    }
}

BYTE CVehicleSA::GetRailTrack ()
{
    return GetVehicleInterface ()->m_ucRailTrackID;
}

void CVehicleSA::SetRailTrack ( BYTE ucTrackID )
{
    if ( ucTrackID >= NUM_RAILTRACKS )
        return;

    CVehicleSAInterface* pInterf = GetVehicleInterface ();
    if ( pInterf->m_ucRailTrackID != ucTrackID )
    {
        pInterf->m_ucRailTrackID = ucTrackID;
        if ( !IsDerailed () )
        {
            DWORD dwFunc = FUNC_CVehicle_RecalcOnRailDistance;
            _asm
            {
                mov ecx, pInterf
                call dwFunc
            }
        }
    }
}

float CVehicleSA::GetTrainPosition ()
{
    return GetVehicleInterface ()->m_fTrainRailDistance;
}


void CVehicleSA::SetTrainPosition ( float fPosition, bool bRecalcOnRailDistance )
{
    CVehicleSAInterface* pInterface = GetVehicleInterface ();
    if ( pInterface->m_fTrainRailDistance <= fPosition - 0.1 || pInterface->m_fTrainRailDistance >= fPosition + 0.1 )
    {
        pInterface->m_fTrainRailDistance = fPosition;
        if ( bRecalcOnRailDistance && !IsDerailed () )
        {
            DWORD dwFunc = FUNC_CVehicle_RecalcOnRailDistance;
            _asm
            {
                mov ecx, pInterface
                call dwFunc
            }
        }
    }
}

bool CVehicleSA::CanPedEnterCar ( void )
{
    DEBUG_TRACE("bool CVehicleSA::CanPedEnterCar ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_CanPedEnterCar;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


bool CVehicleSA::CanPedJumpOutCar ( CPed* pPed )
{
    DEBUG_TRACE("bool CVehicleSA::CanPedJumpOutCar ( CPed* pPed )");

    bool bReturn = false;

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );

    if ( pPedSA )
    {
        DWORD dwThis = (DWORD) m_pInterface;
        CPedSAInterface* pPedInt = pPedSA->GetPedInterface();
        DWORD dwFunc = FUNC_CVehicle_CanPedJumpOutCar;

        _asm
        {
            mov     ecx, dwThis
            push    pPedInt
            call    dwFunc
            mov     bReturn, al
        }
    }

    return bReturn;
}

CDoorSA* CVehicleSA::GetDoor ( unsigned char ucDoor )
{
    if ( ucDoor <= 5 )
        return & m_doors [ ucDoor ];
    return 0;
}

void CVehicleSA::OpenDoor ( unsigned char ucDoor, float fRatio, bool bMakeNoise )
{
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = ((CVehicleSAInterfaceVTBL*)GetVehicleInterface()->vtbl)->OpenDoor;

    // Grab the car node index for the given door id
    static int s_iCarNodeIndexes [6] = { 0x10, 0x11, 0x0A, 0x08, 0x0B, 0x09 };
    DWORD dwIdx = s_iCarNodeIndexes [ ucDoor ];
    DWORD dwDoor = ucDoor;
    DWORD dwMakeNoise = bMakeNoise;

    _asm
    {
        mov     ecx, dwThis
        push    dwMakeNoise
        push    fRatio
        push    dwDoor
        push    dwIdx
        push    0
        call    dwFunc
    }
}

void CVehicleSA::SetSwingingDoorsAllowed ( bool bAllowed )
{
    m_bSwingingDoorsAllowed = bAllowed;
}

bool CVehicleSA::AreSwingingDoorsAllowed () const
{
    return m_bSwingingDoorsAllowed;
}

bool CVehicleSA::AreDoorsLocked ( void )
{
    return ( GetVehicleInterface ()->ul_doorstate == 2 || GetVehicleInterface ()->ul_doorstate == 5 || 
             GetVehicleInterface ()->ul_doorstate == 4 || GetVehicleInterface ()->ul_doorstate == 7 || 
             GetVehicleInterface ()->ul_doorstate == 3 );
}

void CVehicleSA::LockDoors ( bool bLocked )
{
    bool bAreDoorsLocked = AreDoorsLocked ();
    bool bAreDoorsUndamageable = AreDoorsUndamageable ();

    if ( bLocked && !bAreDoorsLocked )
    {
        if ( bAreDoorsUndamageable )
            GetVehicleInterface ()->ul_doorstate = 7;
        else
            GetVehicleInterface ()->ul_doorstate = 2;
    }
    else if ( !bLocked && bAreDoorsLocked )
    {
        if ( bAreDoorsUndamageable )
            GetVehicleInterface ()->ul_doorstate = 1;
        else
            GetVehicleInterface ()->ul_doorstate = 0;
    }
}

bool CVehicleSA::AreDoorsUndamageable ( void )
{
    return ( GetVehicleInterface ()->ul_doorstate == 1 ||
             GetVehicleInterface ()->ul_doorstate == 7 );
}

void CVehicleSA::SetDoorsUndamageable ( bool bUndamageable )
{
    bool bAreDoorsLocked = AreDoorsLocked ();
    bool bAreDoorsUndamageable = AreDoorsUndamageable ();

    if ( bUndamageable && !bAreDoorsUndamageable )
    {
        if ( bAreDoorsLocked )
            GetVehicleInterface ()->ul_doorstate = 7;
        else
            GetVehicleInterface ()->ul_doorstate = 1;
    }
    else if ( !bUndamageable && bAreDoorsUndamageable )
    {
        if ( bAreDoorsLocked )
            GetVehicleInterface ()->ul_doorstate = 2;
        else
            GetVehicleInterface ()->ul_doorstate = 0;
    }
}

void CVehicleSA::AddVehicleUpgrade ( DWORD dwModelID )
{   
    DEBUG_TRACE("void CVehicleSA::AddVehicleUpgrade ( DWORD dwModelID )");
    if ( dwModelID >= 1000 && dwModelID <= 1193 )
    {
        DWORD dwThis = (DWORD) m_pInterface;

        DWORD dwFunc = FUNC_CVehicle_AddVehicleUpgrade;
        _asm
        {
            mov     ecx, dwThis
            push    dwModelID
            call    dwFunc
        }
    }
}

void CVehicleSA::RemoveVehicleUpgrade ( DWORD dwModelID )
{
    DEBUG_TRACE("void CVehicleSA::RemoveVehicleUpgrade ( DWORD dwModelID )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_RemoveVehicleUpgrade;

    _asm
    {
        mov     ecx, dwThis
        push    dwModelID
        call    dwFunc
    }
}

bool CVehicleSA::DoesSupportUpgrade ( SString strFrameName )
{
    SVehicleFrame Component;
    if ( GetVehicleComponent ( strFrameName, Component ) && Component.pFrame != NULL )
    {
        // Todo: enforce hierarchy
        return true;
    }

    return false;
}

bool CVehicleSA::CanPedLeanOut ( CPed* pPed )
{
    DEBUG_TRACE("bool CVehicleSA::CanPedLeanOut ( CPed* pPed )");

    bool bReturn = false;

    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( pPed );

    if ( pPedSA )
    {
        DWORD dwThis = (DWORD) m_pInterface;
        CPedSAInterface* pPedInt = pPedSA->GetPedInterface();
        DWORD dwFunc = FUNC_CVehicle_CanPedLeanOut;

        _asm
        {
            mov     ecx, dwThis
            push    pPedInt
            call    dwFunc
            mov     bReturn, al
        }
    }

    return bReturn;

}


bool CVehicleSA::CanPedStepOutCar ( bool bUnknown )
{
    DEBUG_TRACE("bool CVehicleSA::CanPedStepOutCar ( bool bUnknown )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwUnknown = (DWORD) bUnknown;
    DWORD dwFunc = FUNC_CVehicle_CanPedStepOutCar;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        push    dwUnknown
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


bool CVehicleSA::CarHasRoof ( void )
{
    DEBUG_TRACE("bool CVehicleSA::CarHasRoof ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_CarHasRoof;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


void CVehicleSA::ExtinguishCarFire ( void )
{
    DEBUG_TRACE("void CVehicleSA::ExtinguishCarFire ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_ExtinguishCarFire;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


DWORD CVehicleSA::GetBaseVehicleType ( void )
{
    DEBUG_TRACE("DWORD CVehicleSA::GetBaseVehicleType ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_GetBaseVehicleType;
    DWORD dwReturn = 0;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax

    }

    return dwReturn;
}


void CVehicleSA::SetBodyDirtLevel ( float fDirtLevel )
{
    DEBUG_TRACE("void CVehicleSA::SetBodyDirtLevel ( float fDirtLevel )");
    GetVehicleInterface ()->nBodyDirtLevel = fDirtLevel;
}


float CVehicleSA::GetBodyDirtLevel ( void )
{
    DEBUG_TRACE("float CVehicleSA::GetBodyDirtLevel ( void )");
    return GetVehicleInterface ()->nBodyDirtLevel;
}


unsigned char CVehicleSA::GetCurrentGear ( void )
{
    DEBUG_TRACE("unsigned char CVehicleSA::GetCurrentGear ( void )");
    return GetVehicleInterface ()->m_nCurrentGear;
}


float CVehicleSA::GetGasPedal ( void )
{
    DEBUG_TRACE("float CVehicleSA::GetGasPedal ( void )");
    return GetVehicleInterface ()->m_fGasPedal;
}


float CVehicleSA::GetHeightAboveRoad ( void )
{
    DEBUG_TRACE("float CVehicleSA::GetHeightAboveRoad ( void )");
    DWORD dwThis = (DWORD) GetVehicleInterface ();
    DWORD dwFunc = FUNC_CVehicle_GetHeightAboveRoad;
    float fReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fReturn
    }

    return fReturn;
}


float CVehicleSA::GetSteerAngle ( void )
{
    DEBUG_TRACE("float CVehicleSA::GetSteerAngle ( void )");
    return GetVehicleInterface ()->m_fSteerAngle;
}


bool CVehicleSA::GetTowBarPos ( CVector* pVector, CVehicle* pTrailer )
{
    DEBUG_TRACE("bool CVehicleSA::GetTowBarPos ( CVector* pVector )");
    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(m_pInterface->vtbl);
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = vehicleVTBL->GetTowbarPos;
    bool bReturn = false;

    DWORD dwTrailerInt = 0;
    CVehicleSA* pTrailerSA = dynamic_cast < CVehicleSA* > ( pTrailer );
    if ( pTrailerSA )
        dwTrailerInt = (DWORD) pTrailerSA->GetInterface ();

    _asm
    {
        mov     ecx, dwThis
        push    dwTrailerInt
        push    1
        push    pVector
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


bool CVehicleSA::GetTowHitchPos ( CVector* pVector )
{
    DEBUG_TRACE("bool CVehicleSA::GetTowHitchPos ( CVector* pVector )");
    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(m_pInterface->vtbl);
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = vehicleVTBL->GetTowHitchPos;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        push    0
        push    1
        push    pVector
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


bool CVehicleSA::IsOnItsSide ( void )
{
    DEBUG_TRACE("bool CVehicleSA::IsOnItsSide ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsOnItsSide;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}


bool CVehicleSA::IsLawEnforcementVehicle ( void )
{
    DEBUG_TRACE("bool CVehicleSA::IsLawEnforcementVehicle ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsLawEnforcementVehicle;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}


bool CVehicleSA::IsPassenger ( CPed* pPed )
{
    DEBUG_TRACE("bool CVehicleSA::IsPassenger ( CPed* pPed )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsPassenger;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        push    pPed
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}


bool CVehicleSA::IsSphereTouchingVehicle ( CVector * vecOrigin, float fRadius )
{
    DEBUG_TRACE("bool CVehicleSA::IsSphereTouchingVehicle ( CVector * vecOrigin, float fRadius )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsSphereTouchingVehicle;
    bool bReturn = false;

    _asm
    {
        push    eax

        mov     ecx, dwThis
        mov     eax, vecOrigin
        push    fRadius
        push    dword ptr [eax]
        push    dword ptr [eax + 4]
        push    dword ptr [eax + 8]
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}


bool CVehicleSA::IsUpsideDown ( void )
{
    DEBUG_TRACE("bool CVehicleSA::IsUpsideDown ( void )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsUpsideDown;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


void CVehicleSA::MakeDirty ( CColPoint* pPoint )
{
    DEBUG_TRACE("void CVehicleSA::MakeDirty ( CColPoint* pPoint )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_MakeDirty;

    _asm
    {
        mov     ecx, dwThis
        push    pPoint
        call    dwFunc
    }
}


void CVehicleSA::SetEngineOn ( bool bEngineOn )
{
    DEBUG_TRACE("void CVehicleSA::SetEngineOn ( bool bEngineOn )");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwEngineOn = (DWORD) bEngineOn;
    DWORD dwFunc = FUNC_CVehicle_SetEngineOn;

    _asm
    {
        mov     ecx, dwThis
        push    dwEngineOn
        call    dwFunc
    }
}


CPed* CVehicleSA::GetDriver ( void )
{
    DEBUG_TRACE("CPed* CVehicleSA::GetDriver ( void )");
    CPoolsSA* pPools  = (CPoolsSA *)pGame->GetPools();

    CPedSAInterface* pDriver = GetVehicleInterface ()->pDriver;
    if ( pDriver )
        return pPools->GetPed( (DWORD*) pDriver );
    else
        return NULL;
}


CPed* CVehicleSA::GetPassenger ( unsigned char ucSlot )
{
    DEBUG_TRACE("CPed* CVehicleSA::GetPassenger ( unsigned char ucSlot )");
    CPoolsSA* pPools  = (CPoolsSA *)pGame->GetPools();

    if ( ucSlot < 8 )
    {
        CPedSAInterface* pPassenger = GetVehicleInterface ()->pPassengers [ucSlot];
        if ( pPassenger )
            return pPools->GetPed( (DWORD*) pPassenger );
    }

    return NULL;
}


bool CVehicleSA::IsBeingDriven()
{
    DEBUG_TRACE("bool CVehicleSA::IsBeingDriven()");
    CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
    if ( GetVehicleInterface ()->pDriver != NULL )
        return TRUE;
    else
        return FALSE;
}


/**
 * \todo Implement for other vehicle types too and check
 */
void CVehicleSA::PlaceBikeOnRoadProperly()
{
    DEBUG_TRACE("void CVehicleSA::PlaceBikeOnRoadProperly()");
    DWORD dwFunc = FUNC_Bike_PlaceOnRoadProperly;
    DWORD dwBike = (DWORD)this->GetInterface();

    _asm
    {
        mov     ecx, dwBike
        call    dwFunc
    }
}

void CVehicleSA::PlaceAutomobileOnRoadProperly()
{
    DEBUG_TRACE("void CVehicleSA::PlaceAutomobileOnRoadProperly()");
    DWORD dwFunc = FUNC_Automobile_PlaceOnRoadProperly;
    DWORD dwAutomobile = (DWORD)this->GetInterface();

    _asm
    {
        mov     ecx, dwAutomobile
        call    dwFunc
    }
}

void CVehicleSA::SetColor ( SColor color1, SColor color2, SColor color3, SColor color4, int )
{
    m_RGBColors[0] = color1;
    m_RGBColors[1] = color2;
    m_RGBColors[2] = color3;
    m_RGBColors[3] = color4;
}

void CVehicleSA::GetColor ( SColor* color1, SColor* color2, SColor* color3, SColor* color4, int )
{
    *color1 = m_RGBColors[0];
    *color2 = m_RGBColors[1];
    *color3 = m_RGBColors[2];
    *color4 = m_RGBColors[3];
}

// works with firetrucks & tanks
void CVehicleSA::GetTurretRotation ( float * fHorizontal, float * fVertical )
{
    DEBUG_TRACE("void * CVehicleSA::GetTurretRotation ( float * fHorizontal, float * fVertical )");
    // This is coded in asm because for some reason it was failing to compile
    // correctly with normal c++.
    DWORD vehicleInterface = (DWORD)this->GetInterface();
    float fHoriz = 0.0f;
    float fVert = 0.0f;
    _asm
    {
        mov     eax, vehicleInterface
        add     eax, 0x94C
        fld     [eax]
        fstp    fHoriz
        add     eax, 4
        fld     [eax]
        fstp    fVert
    }
    *fHorizontal = fHoriz;
    *fVertical = fVert;
}

void CVehicleSA::SetTurretRotation ( float fHorizontal, float fVertical )
{
    DEBUG_TRACE("void * CVehicleSA::SetTurretRotation ( float fHorizontal, float fVertical )");
    //*(float *)(this->GetInterface() + 2380) = fHorizontal;
    //*(float *)(this->GetInterface() + 2384) = fVertical;
    DWORD vehicleInterface = (DWORD)this->GetInterface();
    _asm
    {
        mov     eax, vehicleInterface
        add     eax, 0x94C
        fld     fHorizontal
        fstp    [eax]
        add     eax, 4
        fld     fVertical
        fstp    [eax]
    }
}

bool CVehicleSA::IsSirenOrAlarmActive ( )
{
    return ((CVehicleSAInterface *)this->GetInterface())->m_nVehicleFlags.bSirenOrAlarm;
}

void CVehicleSA::SetSirenOrAlarmActive ( bool bActive )
{
   ((CVehicleSAInterface *)this->GetInterface())->m_nVehicleFlags.bSirenOrAlarm = bActive;
}

DWORD * CVehicleSA::GetMemoryValue ( DWORD dwOffset )
{
    if ( dwOffset <= SIZEOF_CHELI )
        return (DWORD *)((DWORD)(this->GetInterface()) + dwOffset);
    else
        return NULL;
}

/*

bool CVehicleSA::isInRect(FLOAT fX1, FLOAT fY1, FLOAT fX2, FLOAT fY2, bool bShowHotspot)
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_IN_RECT_STILL,&this->internalID,&fX1,&fY1,&fX2,&fY2,&bShowHotspot);
}

//-----------------------------------------------------------

bool CVehicleSA::isInCube(CVector * v3dCorner1, CVector * v3dCorner2, bool bShowHotspot)
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_IN_CUBE_STILL,&this->internalID,*v3dCorner1,*v3dCorner2,&bShowHotspot);
}

//-----------------------------------------------------------

bool CVehicleSA::isNearPoint(FLOAT fX, FLOAT fY, FLOAT fRadiusX, FLOAT fRadiusY, bool bShowHotspot, bool bStill)
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_NEAR_POINT,&this->internalID,&fX,&fY,&fRadiusX,&fRadiusY,&bShowHotspot,&bStill);
}

//-----------------------------------------------------------

bool CVehicleSA::isNearPoint3D(FLOAT fX, FLOAT fY, FLOAT fZ, FLOAT fRadiusX, FLOAT fRadiusY, FLOAT fRadiusZ, bool bShowHotspot, bool bStill)
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_NEAR_POINT_3D,&this->internalID,&fX,&fY,&fZ,&fRadiusX,&fRadiusY,&fRadiusZ,&bShowHotspot,&bStill);
}

//-----------------------------------------------------------

bool CVehicleSA::isStopped()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_STOPPED,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isWrecked()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_WRECKED,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isOnLand()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_ON_LAND,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isCrushed()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_CRUSHED,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isOnRoof()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_FLIPPED,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isAirbourne()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_AIRBORNE,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isUpsidedown()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_UPSIDEDOWN,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isStuck()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_STUCK,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isSunk()
{
    return (bool)CallScriptFunction(GTAVC_SCM_HAS_CAR_SUNK,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isBoundingSphereVisible()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_BOUNDING_SPHERE_VISIBLE,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isDamaged()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_DAMAGED,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isBombActive()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_BOMB_STATUS,&this->internalID);
}

//-----------------------------------------------------------

bool CVehicleSA::isPassengerSeatFree()
{
    return (bool)CallScriptFunction(GTAVC_SCM_IS_CAR_PASSENGER_SEAT_FREE,&this->internalID);
}

//-----------------------------------------------------------

DWORD CVehicleSA::passengerCount()
{
    return ((CVehicleSAInterface *)this->GetInterface())->PassengerCount;
}

//-----------------------------------------------------------

DWORD CVehicleSA::maxPassengers()
{
    DWORD dwMaxPassengers = 0;
    CallScriptFunction(GTAVC_SCM_GET_CAR_MAX_PASSENGERS,&this->internalID, &dwMaxPassengers);
    return dwMaxPassengers;
}

//-----------------------------------------------------------

void CVehicleSA::setDensity(DWORD dwDensity)
{
    CallScriptFunction(GTAVC_SCM_SET_CAR_DENSITY,&this->internalID,&dwDensity);
}

//-----------------------------------------------------------

void CVehicleSA::makeHeavy(bool bHeavy)
{
    CallScriptFunction(GTAVC_SCM_MAKE_CAR_HEAVY,&this->internalID,&bHeavy);
}
*/
//-----------------------------------------------------------
/**
 * \todo Make a eDoorState enum
 */
/*
void CVehicleSA::setDoorsStatus(DWORD dwStatus)
{
    CallScriptFunction(GTAVC_SCM_SET_CAR_DOOR_STATUS,&this->internalID,&dwStatus);
}

//-----------------------------------------------------------

void CVehicleSA::setTaxiLight(bool bOn)
{
    CallScriptFunction(GTAVC_SCM_SET_CAR_TAXIAVAILABLE,&this->internalID,&bOn);
}

//-----------------------------------------------------------

void CVehicleSA::setSpeed(FLOAT fSpeed)
{
    CallScriptFunction(GTAVC_SCM_SET_CAR_SPEED,&this->internalID,&fSpeed);
}

//-----------------------------------------------------------

FLOAT CVehicleSA::getSpeed()
{
    FLOAT fReturn;
    CallScriptFunction(GTAVC_SCM_GET_CAR_SPEED,&this->internalID, &fReturn);
    return fReturn;
}

//-----------------------------------------------------------

void CVehicleSA::setSpeedInstantly(FLOAT fSpeed)
{
    CallScriptFunction(GTAVC_SCM_SET_CAR_SPEED_INSTANTLY,&this->internalID,&fSpeed);
}
//-----------------------------------------------------------

void CVehicleSA::setImmunities(bool bBullet, bool bFire, bool bExplosion, bool bDamage, bool bUnknown)
{
    CallScriptFunction(GTAVC_SCM_SET_CAR_IMMUNITIES,&this->internalID,&bBullet,&bFire,&bExplosion,&bDamage,&bUnknown);
}
*/

//-----------------------------------------------------------
FLOAT CVehicleSA::GetHealth()
{
    CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
    return vehicle->m_nHealth;
}
//-----------------------------------------------------------
void CVehicleSA::SetHealth( FLOAT fHealth )
{
    CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
    vehicle->m_nHealth = fHealth;
    if ( fHealth >= 250.0f )
        vehicle->m_fBurningTime = 0.0f;
}

// SHould be plane funcs

void CVehicleSA::SetLandingGearDown ( bool bLandingGearDown )
{
    // This is the C code corresponding to the gta_sa
    // asm code at address 0x6CB39A.
    CVehicleSAInterface* pInterface = this->GetVehicleInterface ();
    DWORD dwThis = (DWORD)pInterface;
    float& fPosition = *(float *)( dwThis + 0x9CC );
    float& fTimeStep = *(float *)( 0xB7CB5C );
    float& flt_871904 = *(float *)( 0x871904 );
    
    if ( IsLandingGearDown () != bLandingGearDown )
    {
        // The following code toggles the landing gear direction
        if ( fPosition == 0.0f )
        {
            MemPutFast < DWORD > ( dwThis + 0x5A5, 0x02020202 );
            fPosition += ( fTimeStep * flt_871904 );
        }
        else
        {
            if ( fPosition != 1.0f )
                fPosition *= -1.0f;
            else
                fPosition = ( fTimeStep * flt_871904 ) - 1.0f;
        }
    }
}

float CVehicleSA::GetLandingGearPosition ( )
{
    DWORD dwThis = (DWORD)this->GetInterface();
    return *(float *)(dwThis + 2508);
}

void CVehicleSA::SetLandingGearPosition ( float fPosition )
{
    DWORD dwThis = (DWORD)this->GetInterface();
    MemPutFast < float > ( dwThis + 2508, fPosition );
}

bool CVehicleSA::IsLandingGearDown ( )
{
    DWORD dwThis = (DWORD)this->GetInterface();
    if ( *(float *)(dwThis + 2508) <= 0.0f )
        return true;
    else
        return false;
}

void CVehicleSA::Fix ( void )
{
    DEBUG_TRACE("void CVehicleSA::Fix ( void )");
    DWORD dwThis = (DWORD) GetInterface();

    DWORD dwFunc = 0;
    CModelInfo* pModelInfo = pGame->GetModelInfo ( this->GetModelIndex() );
    if ( pModelInfo )
    {
        if ( pModelInfo->IsCar() || pModelInfo->IsMonsterTruck() || pModelInfo->IsTrailer() )
            dwFunc = FUNC_CAutomobile__Fix;
        else if ( pModelInfo->IsPlane() )
            dwFunc = FUNC_CPlane__Fix;
        else if ( pModelInfo->IsHeli() )
            dwFunc = FUNC_CHeli__Fix;
        else if ( pModelInfo->IsBike() )
            dwFunc = FUNC_CBike_Fix;

        if ( dwFunc)
        {
            _asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }
        }
    }
}

CDamageManager * CVehicleSA::GetDamageManager()
{
    DEBUG_TRACE("CDamageManager * CVehicleSA::GetDamageManager()");
    return this->m_pDamageManager;
}

void CVehicleSA::BlowUp ( CEntity* pCreator, unsigned long ulUnknown )
{
    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(this->GetInterface()->vtbl);
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = vehicleVTBL->BlowUpCar;

    DWORD dwCreator = (DWORD)pCreator;

    _asm
    {
        push        ulUnknown
        push        dwCreator
        mov         ecx, dwThis
        call        dwFunc
    }
}


void CVehicleSA::BlowUpCutSceneNoExtras ( unsigned long ulUnknown1, unsigned long ulUnknown2, unsigned long ulUnknown3, unsigned long ulUnknown4 )
{
    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(this->GetInterface()->vtbl);
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = vehicleVTBL->BlowUpCarCutSceneNoExtras;

    _asm
    {
        push        ulUnknown1
        push        ulUnknown2
        push        ulUnknown3
        push        ulUnknown4
        mov         ecx, dwThis
        call        dwFunc
    }
}


void CVehicleSA::FadeOut ( bool bFadeOut )
{
    CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
    vehicle->bDistanceFade = bFadeOut;
    vehicle->m_nVehicleFlags.bFadeOut = bFadeOut;
}

bool CVehicleSA::IsFadingOut ( void )
{
    CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
    return vehicle->m_nVehicleFlags.bFadeOut;
}


unsigned char CVehicleSA::GetNumberGettingIn ( void )
{
    return GetVehicleInterface ()->m_nNumGettingIn;
}


unsigned char CVehicleSA::GetPassengerCount ( void )
{
    return GetVehicleInterface ()->m_nNumPassengers;
}


unsigned char CVehicleSA::GetMaxPassengerCount ( void )
{
    return GetVehicleInterface ()->m_nMaxPassengers;
}

void CVehicleSA::SetTowLink ( CVehicle* pVehicle )
{
    DEBUG_TRACE("void CVehicleSA::SetTowLink ( CVehicle* pVehicle )");
    // We can't use the vtable func, because it teleports the trailer parallel to the vehicle => make our own one (see #1655)

    CVehicleSA* pVehicleSA = dynamic_cast < CVehicleSA* > ( pVehicle );
    
    if ( pVehicleSA )
    {
        DWORD dwThis = (DWORD) GetInterface();
        DWORD dwVehicleInt = (DWORD) pVehicleSA->GetVehicleInterface();

        *(DWORD*)(dwThis + 1220) = dwVehicleInt;
        *(DWORD*)(dwVehicleInt + 1224) = dwThis;

        // Set the trailer's status to "remote controlled"
        SetEntityStatus ( eEntityStatus::STATUS_REMOTE_CONTROLLED );
    }
}

bool CVehicleSA::BreakTowLink ( void )
{
    DEBUG_TRACE("bool CVehicleSA::BreakTowLink ( void )");
    DWORD dwThis = (DWORD) GetInterface();

    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(this->GetInterface()->vtbl);
    DWORD dwFunc = vehicleVTBL->BreakTowLink;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis       
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

CVehicle * CVehicleSA::GetTowedVehicle ( void )
{
    DEBUG_TRACE("CVehicle * CVehicleSA::GetTowedVehicle ( void )");
    CVehicleSAInterface * pTowedVehicle = (CVehicleSAInterface *)*(DWORD *)((DWORD)this->GetInterface() + 1224);
    if ( pTowedVehicle )
        return pGame->GetPools()->GetVehicle ( (DWORD *)pTowedVehicle );
    return NULL;
}

CVehicle * CVehicleSA::GetTowedByVehicle ( void )
{
    DEBUG_TRACE("CVehicle * CVehicleSA::GetTowedVehicle ( void )");
    CVehicleSAInterface * pTowedVehicle = (CVehicleSAInterface *)*(DWORD *)((DWORD)this->GetInterface() + 1220);
    if ( pTowedVehicle )
        return pGame->GetPools()->GetVehicle ( (DWORD *)pTowedVehicle );
    return NULL;
}

void CVehicleSA::SetWinchType ( eWinchType winchType )
{
    if ( winchType < 4 && winchType != 2 )
    {
        GetVehicleInterface ()->WinchType = winchType;
    }
}

void CVehicleSA::PickupEntityWithWinch ( CEntity* pEntity )
{
    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );

    if ( pEntitySA )
    {
        DWORD dwFunc = FUNC_CVehicle_PickUpEntityWithWinch;
        DWORD dwThis = (DWORD) GetInterface();
        DWORD dwEntityInterface = (DWORD) pEntitySA->GetInterface ();

        _asm
        {
            push    dwEntityInterface
            mov     ecx, dwThis
            call    dwFunc
        }
    }
}

void CVehicleSA::ReleasePickedUpEntityWithWinch ( void )
{
    DWORD dwFunc = FUNC_CVehicle_ReleasePickedUpEntityWithWinch;
    DWORD dwThis = (DWORD) GetInterface();

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CVehicleSA::SetRopeHeightForHeli ( float fRopeHeight )
{
    DWORD dwFunc = FUNC_CVehicle_SetRopeHeightForHeli;
    DWORD dwThis = (DWORD) GetInterface();

    _asm
    {
        push    fRopeHeight
        mov     ecx, dwThis
        call    dwFunc
    }
}

CPhysical * CVehicleSA::QueryPickedUpEntityWithWinch ( )
{
    DWORD dwFunc = FUNC_CVehicle_QueryPickedUpEntityWithWinch;
    DWORD dwThis = (DWORD) GetInterface();

    CPhysicalSAInterface * phys;
    CPhysical * physRet;
    _asm
    {
        mov     ecx, dwThis       
        call    dwFunc
        mov     phys, eax
    }

    if ( phys )
    {
        CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
        switch(phys->nType)
        {
        case ENTITY_TYPE_PED:
            physRet = (CPhysical *)pPools->GetPed((DWORD *)phys);
            break;
        case ENTITY_TYPE_VEHICLE:
            physRet = (CPhysical *)pGame->GetPools()->GetVehicle ( (DWORD *)phys );
            break;
        case ENTITY_TYPE_OBJECT:
            physRet = (CPhysical *)pPools->GetObject( (DWORD *)phys );
            break;
        default:
            physRet = NULL;
        }
    }
    return physRet;
}

void CVehicleSA::SetRemap ( int iRemap )
{
    DWORD dwFunc = FUNC_CVehicle__SetRemap;
    DWORD dwThis = (DWORD) GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    iRemap
        call    dwFunc
    }
}


int CVehicleSA::GetRemapIndex ( void )
{
    DWORD dwFunc = FUNC_CVehicle__GetRemapIndex;
    DWORD dwThis = (DWORD) GetInterface();
    int iReturn = 0;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     iReturn, eax
    }
    return iReturn;
}

void CVehicleSA::SetRemapTexDictionary ( int iRemapTextureDictionary )
{
    DWORD dwFunc = FUNC_CVehicle__SetRemapTexDictionary;
    DWORD dwThis = (DWORD) GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    iRemapTextureDictionary
        call    dwFunc
    }
}

bool CVehicleSA::IsSmokeTrailEnabled ( void )
{
    return ( *(unsigned char*)((DWORD)this->GetInterface() + 2560) == 1 );
}

void CVehicleSA::SetSmokeTrailEnabled ( bool bEnabled )
{
    MemPutFast < unsigned char > ( (DWORD)this->GetInterface() + 2560, ( bEnabled ) ? 1 : 0 );
}


CHandlingEntry* CVehicleSA::GetHandlingData ( void )
{
    return m_pHandlingData;
}


void CVehicleSA::SetHandlingData ( CHandlingEntry* pHandling )
{
    // Store the handling and recalculate it
    m_pHandlingData = static_cast < CHandlingEntrySA* > ( pHandling );
    GetVehicleInterface ()->pHandlingData = m_pHandlingData->GetInterface ();
    RecalculateHandling ();
}


void CVehicleSA::RecalculateHandling ( void )
{
    if ( !m_pHandlingData )
        return;

    m_pHandlingData->Recalculate ( GetModelIndex ( ) );
    
    // Recalculate the suspension lines
    RecalculateSuspensionLines ();

    // Put it in our interface
    CVehicleSAInterface* pInt = GetVehicleInterface ();
    unsigned int uiHandlingFlags = m_pHandlingData->GetInterface ()->uiHandlingFlags;
    // user error correction - NOS_INST = NOS Installed t/f
    // if nos is installed we need the flag set
    if ( pInt->m_upgrades[0] && pInt->m_upgrades[0] >= 1008 && pInt->m_upgrades[0] <= 1010 )
    {
        // Flag not enabled?
        if ( uiHandlingFlags | HANDLING_NOS_Flag )
        {
            // Set zee flag
            uiHandlingFlags |= HANDLING_NOS_Flag;
            m_pHandlingData->SetHandlingFlags ( uiHandlingFlags );
        }
    }
    else
    {
        // Flag Enabled?
        if ( uiHandlingFlags & HANDLING_NOS_Flag )
        {
            // Unset the flag
            uiHandlingFlags &= ~HANDLING_NOS_Flag;
            m_pHandlingData->SetHandlingFlags ( uiHandlingFlags );
        }
    }
    // Hydraulics Flag fixing
    if ( pInt->m_upgrades[1] && pInt->m_upgrades[1] == 1087 )
    {
        // Flag not enabled?
        if ( uiHandlingFlags | HANDLING_Hydraulics_Flag )
        {
            // Set zee flag
            uiHandlingFlags |= HANDLING_Hydraulics_Flag;
            m_pHandlingData->SetHandlingFlags ( uiHandlingFlags );
        }
    }
    else
    {
        // Flag Enabled?
        if ( uiHandlingFlags & HANDLING_Hydraulics_Flag )
        {
            // Unset the flag
            uiHandlingFlags &= ~HANDLING_Hydraulics_Flag;
            m_pHandlingData->SetHandlingFlags ( uiHandlingFlags );
        }
    }
    pInt->dwHandlingFlags = uiHandlingFlags;
    pInt->m_fMass = m_pHandlingData->GetInterface ()->fMass;
    pInt->m_fTurnMass = m_pHandlingData->GetInterface ()->fTurnMass;// * pGame->GetHandlingManager()->GetTurnMassMultiplier();
    pInt->m_vecCenterOfMass = m_pHandlingData->GetInterface()->vecCenterOfMass;
    pInt->m_fBuoyancyConstant = m_pHandlingData->GetInterface()->fUnknown2;
    /*if (m_pHandlingData->GetInterface()->fDragCoeff >= pGame->GetHandlingManager()->GetBasicDragCoeff())
        GetVehicleInterface ()->fDragCoeff = pGame->GetHandlingManager()->GetBasicDragCoeff();
    else*/
        //pInt->fDragCoeff = m_pHandlingData->GetInterface()->fDragCoeff / 1000 * pGame->GetHandlingManager()->GetDragMultiplier();
}


void CVehicleSA::BurstTyre ( BYTE bTyre )
{
    CVehicleSAInterfaceVTBL * vehicleVTBL = (CVehicleSAInterfaceVTBL *)(m_pInterface->vtbl);
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = vehicleVTBL->BurstTyre;
    _asm
    {
        mov         ecx, dwThis
        push        1 // not used
        push        bTyre
        call        dwFunc
    }
}


BYTE CVehicleSA::GetBikeWheelStatus ( BYTE bWheel )
{
    if ( bWheel == 0 ) return * ( BYTE * ) ( (DWORD)this->GetInterface() + 0x65C );
    if ( bWheel == 1 ) return * ( BYTE * ) ( (DWORD)this->GetInterface() + 0x65D );
    return 0;
}


void CVehicleSA::SetBikeWheelStatus ( BYTE bWheel, BYTE bStatus )
{
    if ( bWheel == 0 ) * ( BYTE * ) ( (DWORD)this->GetInterface() + 0x65C ) = bStatus;
    else if ( bWheel == 1 ) * ( BYTE * ) ( (DWORD)this->GetInterface() + 0x65D ) = bStatus;
}

void CVehicleSA::SetTaxiLightOn ( bool bLightOn )
{
    DEBUG_TRACE("void CVehicleSA::SetTaxiLight ( bool bLightOn )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwState = (DWORD) bLightOn;
    DWORD dwFunc = FUNC_CAutomobile_SetTaxiLight;

    _asm
    {
        mov     ecx, dwThis
        push    dwState
        call    dwFunc
    }
}

void GetMatrixForGravity ( const CVector& vecGravity, CMatrix& mat )
{
    // Calculates a basis where the z axis is the inverse of the gravity
    if ( vecGravity.Length () > 0.0001f )
    {
        mat.vUp = -vecGravity;
        mat.vUp.Normalize ();
        if ( fabs(mat.vUp.fX) > 0.0001f || fabs(mat.vUp.fZ) > 0.0001f )
        {
            CVector y ( 0.0f, 1.0f, 0.0f );
            mat.vFront = vecGravity;
            mat.vFront.CrossProduct ( &y );
            mat.vFront.CrossProduct ( &vecGravity );
            mat.vFront.Normalize ();
        }
        else
        {
            mat.vFront = CVector ( 0.0f, 0.0f, vecGravity.fY );
        }
        mat.vRight = mat.vFront;
        mat.vRight.CrossProduct ( &mat.vUp );
    }
    else
    {
        // No gravity, use default axes
        mat.vRight = CVector ( 1.0f, 0.0f, 0.0f );
        mat.vFront = CVector ( 0.0f, 1.0f, 0.0f );
        mat.vUp    = CVector ( 0.0f, 0.0f, 1.0f );
    }
}


void CVehicleSA::SetGravity ( const CVector* pvecGravity )
{
    if ( pGame->GetPools ()->GetPedFromRef ( 1 )->GetVehicle () == this )
    {
        // If this is the local player's vehicle, adjust the camera's position history.
        // This is to keep the automatic camera settling (which happens when driving while not moving the mouse)
        // nice and consistent while the gravity changes.
        CCam* pCam = pGame->GetCamera ()->GetCam ( pGame->GetCamera ()->GetActiveCam () );

        CMatrix matOld, matNew;
        GetMatrixForGravity ( m_vecGravity, matOld );
        GetMatrixForGravity ( *pvecGravity, matNew );
        
        CVector* pvecPosition = &m_pInterface->Placeable.matrix->vPos;

        matOld.Invert ();
        pCam->GetTargetHistoryPos () [ 0 ] = matOld * (pCam->GetTargetHistoryPos () [ 0 ] - *pvecPosition);
        pCam->GetTargetHistoryPos () [ 0 ] = matNew * pCam->GetTargetHistoryPos () [ 0 ] + *pvecPosition;

        pCam->GetTargetHistoryPos () [ 1 ] = matOld * (pCam->GetTargetHistoryPos () [ 1 ] - *pvecPosition);
        pCam->GetTargetHistoryPos () [ 1 ] = matNew * pCam->GetTargetHistoryPos () [ 1 ] + *pvecPosition;
    }

    m_vecGravity = *pvecGravity;
}


CObject * CVehicleSA::SpawnFlyingComponent ( int i_1, unsigned int ui_2 )
{
    DWORD dwReturn;
    DWORD dwThis = ( DWORD ) GetInterface ();
    DWORD dwFunc = FUNC_CAutomobile__SpawnFlyingComponent;
    _asm
    {
        mov     ecx, dwThis
        push    ui_2
        push    i_1
        call    dwFunc
        mov     dwReturn, eax
    }

    CObject * pObject = NULL;
    if ( dwReturn ) pObject = pGame->GetPools ()->GetObject ( ( DWORD * ) dwReturn );
    return pObject;
}


void CVehicleSA::SetWheelVisibility ( eWheels wheel, bool bVisible )
{    
    CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
    RwFrame * pFrame = NULL;
    switch ( wheel )
    {        
        case FRONT_LEFT_WHEEL: pFrame = vehicle->pWheelFrontLeft; break;
        case REAR_LEFT_WHEEL: pFrame = vehicle->pWheelRearLeft; break;
        case FRONT_RIGHT_WHEEL: pFrame = vehicle->pWheelFrontRight; break;
        case REAR_RIGHT_WHEEL: pFrame = vehicle->pWheelRearRight; break;
        default: break;
    }

    if ( pFrame )
    {
        DWORD dw_GetCurrentAtomicObjectCB = 0x6a0750;
        RwObject * pObject = NULL;

        // Stop GetCurrentAtomicObjectCB from returning null for 'invisible' objects
        MemPutFast < BYTE > ( 0x6A0758, 0x90 );
        MemPutFast < BYTE > ( 0x6A0759, 0x90 );
        RwFrameForAllObjects ( pFrame, ( void * ) dw_GetCurrentAtomicObjectCB, &pObject );
        MemPutFast < BYTE > ( 0x6A0758, 0x74 );
        MemPutFast < BYTE > ( 0x6A0759, 0x06 );

        if ( pObject ) pObject->flags = ( bVisible ) ? 4 : 0;
    }
}


CVector CVehicleSA::GetWheelPosition ( eWheels wheel )
{
    switch (wheel)
    {
        case FRONT_LEFT_WHEEL:
            return GetVehicleInterface()->WheelFrontLeftColPoint.Position;
        case FRONT_RIGHT_WHEEL:
            return GetVehicleInterface()->WheelFrontRightColPoint.Position;
        case REAR_LEFT_WHEEL:
            return GetVehicleInterface()->WheelRearLeftColPoint.Position;
        case REAR_RIGHT_WHEEL:
            return GetVehicleInterface()->WheelRearRightColPoint.Position;
    }
    return CVector();
}


bool CVehicleSA::IsHeliSearchLightVisible ( void )
{
    // See CHeli::PreRender
    DWORD dwThis = ( DWORD ) GetInterface ();
    return * ( bool * ) ( dwThis + 2577 );
}


void CVehicleSA::SetHeliSearchLightVisible ( bool bVisible )
{
    // See CHeli::PreRender
    DWORD dwThis = ( DWORD ) GetInterface ();
    MemPutFast < bool > ( dwThis + 2577, bVisible );
}


CColModel * CVehicleSA::GetSpecialColModel ( void )
{
    CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
    if ( vehicle->m_nSpecialColModel != 0xFF )
    {
        CColModelSAInterface * pSpecialColModels = ( CColModelSAInterface * ) VAR_CVehicle_SpecialColModels;
        CColModelSAInterface * pColModelInterface = &pSpecialColModels [ vehicle->m_nSpecialColModel ];
        if ( pColModelInterface )
        {
            CColModel * pColModel = new CColModelSA ( pColModelInterface );
            return pColModel;
        }
    }
    return NULL;
}


bool CVehicleSA::UpdateMovingCollision ( float fAngle )
{
    // If we dont have a driver, use the local player for this function
    // It will check a few key-states which shouldn't make any difference as we've specified an angle.
    CVehicleSAInterface * vehicle = (CVehicleSAInterface *)this->GetInterface();
    CPedSAInterface * pDriver = vehicle->pDriver;    
    if ( !pDriver )
    {
        CPed * pLocalPed = pGame->GetPools ()->GetPedFromRef ( 1 );
        if ( pLocalPed ) vehicle->pDriver = ( CPedSAInterface * ) pLocalPed->GetInterface ();
    }

    bool bReturn;
    DWORD dwThis = ( DWORD ) GetInterface ();
    DWORD dwFunc = FUNC_CAutomobile__UpdateMovingCollision;
    _asm
    {
        mov     ecx, dwThis
        push    fAngle
        call    dwFunc
        mov     bReturn, al
    }

    // Restore our driver
    vehicle->pDriver = pDriver;

    return bReturn;
}

void* CVehicleSA::GetPrivateSuspensionLines ( void )
{
    if ( m_pSuspensionLines == NULL )
    {
        CModelInfo* pModelInfo = pGame->GetModelInfo ( GetModelIndex() );
        CColDataSA* pColData = pModelInfo->GetInterface ()->pColModel->pColData;
        if ( pModelInfo->IsMonsterTruck ( ) )
        {
            // Monster truck suspension data is 0x90 BYTES rather than 0x80 (some extra stuff I guess)
            m_pSuspensionLines = new BYTE[ 0x90 ];
        }
        else if ( pModelInfo->IsBike ( ) )
        {
            // Bike Suspension data is 0x80 BYTES rather than 0x40 (Some extra stuff I guess)
            m_pSuspensionLines = new BYTE[ 0x80 ];
        }
        else
        {
            // CAutomobile allocates wheels * 32 (0x20)
            m_pSuspensionLines = new BYTE[ pColData->ucNumWheels * 0x20 ];
        }
    }

    return m_pSuspensionLines;
}

void CVehicleSA::CopyGlobalSuspensionLinesToPrivate ( void )
{
    CModelInfo* pModelInfo = pGame->GetModelInfo ( GetModelIndex () );
    CColDataSA* pColData = pModelInfo->GetInterface ()->pColModel->pColData;
    if ( pModelInfo->IsMonsterTruck() )
    {
        // Monster trucks are 0x90 bytes not 0x80
        if ( pColData->pSuspensionLines )
            memcpy ( GetPrivateSuspensionLines (), pColData->pSuspensionLines, 0x90 );
    }
    else if ( pModelInfo->IsBike ( ) )
    {
        // Bikes are 0x80 bytes not 0x40
        if ( pColData->pSuspensionLines )
            memcpy ( GetPrivateSuspensionLines (), pColData->pSuspensionLines, 0x80 );
    }
    else
    {
        // CAutomobile allocates wheels * 32 (0x20)
        if ( pColData->pSuspensionLines )
            memcpy ( GetPrivateSuspensionLines (), pColData->pSuspensionLines, pColData->ucNumWheels * 0x20 );
    }
}

void CVehicleSA::RecalculateSuspensionLines ( void )
{
    CHandlingEntry * pHandlingEntry = GetHandlingData ( );
    // if suspension is master disabled or suspension hasn't changed return.
    //if ( g_pCore->GetMultiplayer ()->IsSuspensionEnabled () == false || pHandlingEntry->HasSuspensionChanged ( ) == false )
    //{
    //    return;
    //}

    DWORD dwModel = GetModelIndex ();
    CModelInfo* pModelInfo = pGame->GetModelInfo ( dwModel );
    if ( pModelInfo && pModelInfo->IsMonsterTruck() || pModelInfo->IsCar() )
    {
        CVehicleSAInterface* pInt = GetVehicleInterface ();
        // Trains (Their trailers do as well!)
        if ( pModelInfo->IsTrain () || dwModel == 571 || dwModel == 570 || dwModel == 569 || dwModel == 590 )
            return;

        CVehicleSAInterfaceVTBL* pVtbl = reinterpret_cast < CVehicleSAInterfaceVTBL* > ( pInt->vtbl );
        DWORD dwSetupSuspensionLines = pVtbl->SetupSuspensionLines;
        DWORD dwThis = (DWORD)pInt;
        _asm
        {
            mov ecx, dwThis
            call dwSetupSuspensionLines
        }

        CopyGlobalSuspensionLinesToPrivate ();
    }
}

void CVehicleSA::GiveVehicleSirens ( unsigned char ucSirenType, unsigned char ucSirenCount )
{
    m_tSirenInfo.m_bOverrideSirens = true;
    m_tSirenInfo.m_ucSirenType = ucSirenType;
    m_tSirenInfo.m_ucSirenCount = ucSirenCount;
}

void CVehicleSA::SetVehicleSirenPosition ( unsigned char ucSirenID, CVector vecPos )
{
    m_tSirenInfo.m_tSirenInfo[ucSirenID].m_vecSirenPositions = vecPos;
}

void CVehicleSA::GetVehicleSirenPosition ( unsigned char ucSirenID, CVector & vecPos )
{
    vecPos = m_tSirenInfo.m_tSirenInfo[ucSirenID].m_vecSirenPositions;
}

void CVehicleSA::SetVehicleFlags ( bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent )
{
     m_tSirenInfo.m_b360Flag = bEnable360; 
     m_tSirenInfo.m_bDoLOSCheck = bEnableLOSCheck; 
     m_tSirenInfo.m_bUseRandomiser = bEnableRandomiser;
     m_tSirenInfo.m_bSirenSilent = bEnableSilent;
}


void CVehicleSA::OnChangingPosition ( const CVector& vecNewPosition )
{
    // Only apply to CAutomobile and down
    if ( GetBaseVehicleType () == 0 )
    {
        CVector vecDelta = vecNewPosition - m_pInterface->Placeable.matrix->vPos;
        if ( vecDelta.LengthSquared () > 10 * 10 )
        {
            // Reposition colpoints for big moves to avoid random spinning
            CVehicleSAInterface* pInterface = GetVehicleInterface ();
            pInterface->WheelFrontLeftColPoint.Position += vecDelta;
            pInterface->WheelRearLeftColPoint.Position += vecDelta;
            pInterface->WheelFrontRightColPoint.Position += vecDelta;
            pInterface->WheelRearRightColPoint.Position += vecDelta;
        }
    }
}

namespace
{
    VOID _MatrixConvertFromEulerAngles ( CMatrix_Padded* matrixPadded, float fX, float fY, float fZ )
    {
        int iUnknown = 0;
        if ( matrixPadded )
        {
            DWORD dwFunc = FUNC_CMatrix__ConvertFromEulerAngles;
            _asm
            {
                push    iUnknown
                push    fZ
                push    fY
                push    fX
                mov     ecx, matrixPadded
                call    dwFunc
            }
        }
    }
    VOID _MatrixConvertToEulerAngles ( CMatrix_Padded* matrixPadded, float &fX, float &fY, float &fZ )
    {
        int iUnknown = 0;
        if ( matrixPadded )
        {
            DWORD dwFunc = FUNC_CMatrix__ConvertToEulerAngles;
            _asm
            {
                push    iUnknown
                push    fZ
                push    fY
                push    fX
                mov     ecx, matrixPadded
                call    dwFunc
            }
        }
    }
}

bool CVehicleSA::GetVehicleComponent ( SString vehicleComponent, SVehicleFrame &Frame )
{
    // find a match
    std::map < SString, SVehicleFrame >::iterator iter = m_ExtraFrames.find ( vehicleComponent );
    if ( iter != m_ExtraFrames.end ( ) )
    {
        Frame = iter->second;
        // did we find a match if so return it
        return true;
    }
    // return null
    return false;
}

bool CVehicleSA::SetComponentRotation ( SString vehicleComponent, CVector vecRotation )  
{ 
    SVehicleFrame Component;
    // Check validty
    if ( GetVehicleComponent ( vehicleComponent, Component ) && Component.pFrame != NULL && Component.bReadOnly == false )
    {
        RwFrame * pComponent = Component.pFrame;
        // call our convert from euler angles function to get a valid matrix
        CMatrix_Padded matrixPadded;
        _MatrixConvertFromEulerAngles ( &matrixPadded, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
        // copy it into the rotation field
        pComponent->modelling.right = (RwV3d&)matrixPadded.vRight;
        pComponent->modelling.up = (RwV3d&)matrixPadded.vFront;
        pComponent->modelling.at = (RwV3d&)matrixPadded.vUp;
        return true;
    }
    return false;
}

bool CVehicleSA::GetComponentRotation ( SString vehicleComponent, CVector &vecRotation )
{
    SVehicleFrame Component;
    // Check validty
    if ( GetVehicleComponent ( vehicleComponent, Component ) && Component.pFrame != NULL )
    {
        RwFrame * pComponent = Component.pFrame;
        // call our convert to euler angles function to get a valid rotation
        CMatrix_Padded matrixPadded;
        (RwV3d&)matrixPadded.vRight = pComponent->modelling.right;
        (RwV3d&)matrixPadded.vFront = pComponent->modelling.up;
        (RwV3d&)matrixPadded.vUp = pComponent->modelling.at;
        _MatrixConvertToEulerAngles ( &matrixPadded, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
        return true;
    }
    return false;
}

bool CVehicleSA::SetComponentPosition ( SString vehicleComponent, CVector vecPosition )  
{ 
    SVehicleFrame Component;
    // Check validty
    if ( GetVehicleComponent ( vehicleComponent, Component ) && Component.pFrame != NULL && Component.bReadOnly == false )
    {
        RwFrame * pComponent = Component.pFrame;
        // set our position (modelling is relative positions and ltm is world pos)
        pComponent->modelling.pos.x = vecPosition.fX;
        pComponent->modelling.pos.y = vecPosition.fY;
        pComponent->modelling.pos.z = vecPosition.fZ;
        return true;
    }
    return false;
}

bool CVehicleSA::GetComponentPosition ( SString vehicleComponent, CVector &vecPositionModelling )  
{ 
    SVehicleFrame Component;
    // Check validty
    if ( GetVehicleComponent ( vehicleComponent, Component ) && Component.pFrame != NULL )
    {
        RwFrame * pComponent = Component.pFrame;
        // get our position (modelling is relative positions and ltm is world pos)
        vecPositionModelling = CVector ( pComponent->modelling.pos.x, pComponent->modelling.pos.y, pComponent->modelling.pos.z );
        return true;
    }
    return false;
}

bool CVehicleSA::IsComponentPresent ( SString vehicleComponent )
{
    SVehicleFrame Frame;
    return GetVehicleComponent ( vehicleComponent, Frame );
}

bool CVehicleSA::GetComponentMatrix ( SString vehicleComponent, RwMatrix &ltm, RwMatrix &modelling )
{
    SVehicleFrame Component;
    // Check validty
    if ( GetVehicleComponent ( vehicleComponent, Component ) && Component.pFrame != NULL )
    {
        RwFrame * pFrame = Component.pFrame;
        // fill in our matricies
        ltm = pFrame->ltm;
        modelling = pFrame->modelling;
        return true;
    }
    return false;
}

bool CVehicleSA::SetComponentMatrix ( SString vehicleComponent, RwMatrix &ltm, RwMatrix &modelling )
{
    SVehicleFrame Component;
    // Check validty
    if ( GetVehicleComponent ( vehicleComponent, Component ) && Component.pFrame != NULL && Component.bReadOnly == false )
    {
        RwFrame * pFrame = Component.pFrame;
        // Copy vectors and leave flags for now.
        pFrame->ltm.at = ltm.at;
        pFrame->ltm.pos = ltm.pos;
        pFrame->ltm.right = ltm.right;
        pFrame->ltm.up = ltm.up;

        pFrame->modelling.at = modelling.at;
        pFrame->modelling.pos = modelling.pos;
        pFrame->modelling.right = modelling.right;
        pFrame->modelling.up = modelling.up;
        return true;
    }
    return false;
}

void CVehicleSA::AddComponent ( RwFrame * pFrame, bool bReadOnly )
{
    // if the frame is invalid we don't want to be here
    if ( !pFrame )
        return;
    // if the frame already exists ignore it
    if ( IsComponentPresent ( pFrame->szName ) || pFrame->szName == "" )
        return;

    SString strName = pFrame->szName;
    // variants have no name field.
    if ( strName == "" )
    {
        // name starts with extra 
        strName = "extra_";
        if ( m_ucVariantCount == 0 )
        {
            // variants are extra_a, extra_b and so on
            strName += ('a' - 1) + m_ucVariant;
        }
        if ( m_ucVariantCount == 1 )
        {
            // variants are extra_a, extra_b and so on
            strName += ('a' - 1) + m_ucVariant2;
        }
        // increment the variant count ( we assume that the first variant created is variant1 and the second is variant2 )
        m_ucVariantCount++;
    }
    SVehicleFrame frame = SVehicleFrame ( pFrame, bReadOnly );
    // insert our new frame
    m_ExtraFrames.insert ( std::pair < SString, SVehicleFrame > ( strName, frame ) );
}


bool CVehicleSA::SetComponentVisible ( SString vehicleComponent, bool bRequestVisible )  
{ 
    SVehicleFrame Component;
    // Check validty
    if ( GetVehicleComponent ( vehicleComponent, Component ) && Component.pFrame != NULL && Component.bReadOnly == false )
    {
        RwFrame * pComponent = Component.pFrame;
        // Get all atomics for this component - Usually one, or two if there is a damaged version
        std::vector< RwObject* > atomicList;
        GetAllAtomicObjects( pComponent, atomicList );

        // Count number currently visible
        uint uiNumAtomicsCurrentlyVisible = 0;
        for ( uint i = 0 ; i < atomicList.size() ; i++ )
            if ( atomicList[i]->flags & 0x04 )
                uiNumAtomicsCurrentlyVisible++;

        if ( bRequestVisible && uiNumAtomicsCurrentlyVisible == 0 )
        {
            // Make atomic (undamaged version) visible. TODO - Check if damaged version should be made visible instead
            for ( uint i = 0 ; i < atomicList.size() ; i++ )
            {
                RwObject* pAtomic = atomicList[i];
                int AtomicId = pGame->GetVisibilityPlugins()->GetAtomicId( pAtomic );

                if ( !( AtomicId & ATOMIC_ID_FLAG_TWO_VERSIONS_DAMAGED ) )
                {
                    // Either only one version, or two versions and this is the undamaged one
                    pAtomic->flags |= 0x04;
                }
            }
        }
        else
        if ( !bRequestVisible && uiNumAtomicsCurrentlyVisible > 0 )
        {
            // Make all atomics invisible
            for ( uint i = 0 ; i < atomicList.size() ; i++ )
                atomicList[i]->flags &= ~0x05;  // Mimic what GTA seems to do - Not sure what the bottom bit is for
        }
        return true;
    }
    return false;
}

bool CVehicleSA::GetComponentVisible ( SString vehicleComponent, bool &bOutVisible )  
{ 
    SVehicleFrame Component;
    // Check validty
    if ( GetVehicleComponent ( vehicleComponent, Component ) && Component.pFrame != NULL && Component.bReadOnly == false )
    {
        RwFrame * pComponent = Component.pFrame;
        // Get all atomics for this component - Usually one, or two if there is a damaged version
        std::vector< RwObject* > atomicList;
        GetAllAtomicObjects( pComponent, atomicList );

        // Count number currently visible
        uint uiNumAtomicsCurrentlyVisible = 0;
        for ( uint i = 0 ; i < atomicList.size() ; i++ )
            if ( atomicList[i]->flags & 0x04 )
                uiNumAtomicsCurrentlyVisible++;

        // Set result
        bOutVisible = ( uiNumAtomicsCurrentlyVisible > 0 );
        return true;
    }
    return false;
}

void CVehicleSA::SetNitroLevel ( float fLevel )
{
    DWORD dwThis = ( DWORD ) GetInterface ();
    MemPutFast < FLOAT > ( dwThis + 0x8A4, fLevel );
}

float CVehicleSA::GetNitroLevel ()
{
    DWORD dwThis = ( DWORD ) GetInterface ();
    float fLevel = *( FLOAT *)( dwThis + 0x8A4 );
    return fLevel;
}

// The following function is a reproduction of a part of
// CPlane::ProcessControlInputs (006CADD0). This allows 
// the usage of setVehicleLandingGearDown on non-occupied
// planes. (Issue 0007608)
void CVehicleSA::UpdateLandingGearPosition ()
{
    DWORD dwThis = ( DWORD ) GetInterface ();
    CVehicleSAInterface* pVehicle = (CVehicleSAInterface*) (dwThis);
    float& fGearPosition = *(float *)( dwThis + 0x9CC );
    const float& fTimeStep = *(float *)( 0xB7CB5C );

    // Guide to fGearPosition
    //  1.0f = Landing gear is pulled in
    //  0.0f = Landing gear is pulled out
    // -1.0f to 0.0f -> Landing gear is being pulled out
    //  0.0f to 1.0f -> Landing gear is being pulled in

    if(fGearPosition < 0.0f)
    {
        fGearPosition = fTimeStep * 0.02f + fGearPosition;
        
        if(fGearPosition >= 0.0f)
        {
            // Set the position to 0.0f
            fGearPosition = 0.0f;

            // Remove Wheels
            m_pDamageManager->SetWheelStatus(FRONT_LEFT_WHEEL, 0);
            m_pDamageManager->SetWheelStatus(FRONT_RIGHT_WHEEL,0);
            m_pDamageManager->SetWheelStatus(REAR_LEFT_WHEEL,  0);
            m_pDamageManager->SetWheelStatus(REAR_RIGHT_WHEEL, 0);
            
            // Update Air Resistance
            float fDragCoeff = GetHandlingData()->GetDragCoeff();
            pVehicle->m_fAirResistance = fDragCoeff / 1000.0f * 0.5f;
        }
    }
    else if(fGearPosition > 0.0f && fGearPosition < 1.0f)
    {
        // Pull in
        fGearPosition = fTimeStep * 0.02f + fGearPosition;
        if(fGearPosition >= 1.0f)
        {
            // Set the position to 1.0f
            fGearPosition = 1.0f;
               
            // C++ Representaion of CPlane::SetLandingGearDown (006CAC20)
            
            // Recreate Wheels
            m_pDamageManager->SetWheelStatus(FRONT_LEFT_WHEEL, 2);
            m_pDamageManager->SetWheelStatus(FRONT_RIGHT_WHEEL,2);
            m_pDamageManager->SetWheelStatus(REAR_LEFT_WHEEL,  2);
            m_pDamageManager->SetWheelStatus(REAR_RIGHT_WHEEL, 2);
        
            // Update Air Resistance
            float fDragCoeff = GetHandlingData()->GetDragCoeff();
                
            const float& fFlyingHandlingGearUpR = pVehicle->pFlyingHandlingData->GearUpR;
            pVehicle->m_fAirResistance = fDragCoeff / 1000.0f * 0.5f * fFlyingHandlingGearUpR;
        }
    }
}
// Change plate text of existing vehicle
bool CVehicleSA::SetPlateText( const SString& strText )
{
    CModelInfo* pModelInfo = pGame->GetModelInfo ( this->GetModelIndex() );
    if ( !pModelInfo )
        return false;
    CVehicleModelInfoSAInterface* pVehicleModelInfo = (CVehicleModelInfoSAInterface*)pModelInfo->GetInterface();

    // Copy text
    strncpy ( pVehicleModelInfo->plateText, *strText, 8 );

    // Check if changeable
    if ( !pVehicleModelInfo->pPlateMaterial )
        return false;

    // Release texture ref, if was custom before
    RwTexture*& pOldTexture = GetVehicleInterface ()->m_pCustomPlateTexture;
    if ( pOldTexture && pOldTexture != pVehicleModelInfo->pPlateMaterial->texture )
    {
        RwTextureDestroy( pOldTexture );
        pOldTexture = NULL;
    }

    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_CustomCarPlate_TextureCreate;
    bool bReturn = false;
    _asm
    {
        mov     ecx, dwThis
        push    pVehicleModelInfo
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}
