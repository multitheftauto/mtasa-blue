/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWorldSA.cpp
*  PURPOSE:     Game world/entity logic
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
CWorldSA::CWorldSA ( )
{
    m_pBuildingRemovals = new std::multimap< unsigned short, SBuildingRemoval* >;
    m_pDataBuildings = new std::multimap < unsigned short, sDataBuildingRemovalItem* >;
    m_pBinaryBuildings = new std::multimap < unsigned short, sBuildingRemovalItem* >;
}

void CWorldSA::Add ( CEntity * pEntity, eDebugCaller CallerId )
{
    DEBUG_TRACE("VOID CWorldSA::Add ( CEntity * pEntity )");

    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );

    if ( pEntitySA )
    {
        CEntitySAInterface * pInterface = pEntitySA->GetInterface();
        if ( (DWORD)pInterface->vtbl == VTBL_CPlaceable )
        {
            SString strMessage ( "Caller: %i ", CallerId );
            LogEvent ( 506, "CWorld::Add ( CEntity * ) Crash", "", strMessage );
        }
        DWORD dwEntity = (DWORD) pEntitySA->GetInterface();
        DWORD dwFunction = FUNC_Add;
        _asm
        {
            push    dwEntity
            call    dwFunction
            add     esp, 4
        }
    }
}


void CWorldSA::Add ( CEntitySAInterface * entityInterface, eDebugCaller CallerId )
{
    DEBUG_TRACE("VOID CWorldSA::Add ( CEntitySAInterface * entityInterface )");
    DWORD dwFunction = FUNC_Add;
    if ( (DWORD)entityInterface->vtbl == VTBL_CPlaceable )
    {
        SString strMessage ( "Caller: %i ", CallerId );
        LogEvent ( 506, "CWorld::Add ( CEntitySAInterface * ) Crash", "", strMessage );
    }
    _asm
    {
        push    entityInterface
        call    dwFunction
        add     esp, 4
    }
}

void CWorldSA::Remove ( CEntity * pEntity, eDebugCaller CallerId )
{
    DEBUG_TRACE("VOID CWorldSA::Remove ( CEntity * entity )");

    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );

    if ( pEntitySA )
    {
        CEntitySAInterface * pInterface = pEntitySA->GetInterface();
        if ( (DWORD)pInterface->vtbl == VTBL_CPlaceable )
        {
            SString strMessage ( "Caller: %i ", CallerId );
            LogEvent ( 507, "CWorld::Remove ( CEntity * ) Crash", "", strMessage );
        }
        DWORD dwEntity = (DWORD)pInterface;
        DWORD dwFunction = FUNC_Remove;
        _asm
        {
            push    dwEntity
            call    dwFunction
            add     esp, 4
        }
    }
}

void CWorldSA::Remove ( CEntitySAInterface * entityInterface, eDebugCaller CallerId )
{
    DEBUG_TRACE("VOID CWorldSA::Remove ( CEntitySAInterface * entityInterface )");
    if ( (DWORD)entityInterface->vtbl == VTBL_CPlaceable )
    {
        SString strMessage ( "Caller: %i ", CallerId );
        LogEvent ( 507, "CWorld::Remove ( CEntitySAInterface * ) Crash", "", strMessage );
    }
    DWORD dwFunction = FUNC_Remove;
    _asm
    {
        push    entityInterface
        call    dwFunction
        add     esp, 4

    /*  mov     ecx, entityInterface
        mov     esi, [ecx]
        push    1
        call    dword ptr [esi+8]*/             
    }
}

void CWorldSA::RemoveReferencesToDeletedObject ( CEntitySAInterface * entity )
{
    DWORD dwFunc = FUNC_RemoveReferencesToDeletedObject;
    DWORD dwEntity = (DWORD)entity;
    _asm
    {
        push    dwEntity
        call    dwFunc
        add     esp, 4
    }
}

bool CWorldSA::TestLineSphere(CVector * vecStart, CVector * vecEnd, CVector * vecSphereCenter, float fSphereRadius, CColPoint ** colCollision )
{
    // THIS FUNCTION IS INCOMPLETE AND SHOULD NOT BE USED
    // Create a CColLine for us
    DWORD dwFunc = FUNC_CColLine_Constructor;
    DWORD dwCColLine[10]; // I don't know how big CColLine is, so we'll just be safe
    _asm
    {
        lea     ecx, dwCColLine
        push    vecEnd
        push    vecStart
        call    dwFunc
    }

    // Now, lets make a CColSphere
    BYTE byteColSphere[18]; // looks like its 18 bytes { vecPos, fSize, byteUnk, byteUnk, byteUnk }
    dwFunc = FUNC_CColSphere_Set;
    _asm
    {
        lea     ecx, byteColSphere
        push    255
        push    0
        push    0
        push    vecSphereCenter
        push    fSphereRadius
        call    dwFunc
    }
}


void ConvertMatrixToEulerAngles ( const CMatrix_Padded& matrixPadded, float& fX, float& fY, float& fZ )
{
    // Convert the given matrix to a padded matrix
    //CMatrix_Padded matrixPadded ( Matrix );

    // Grab its pointer and call gta's func
    const CMatrix_Padded* pMatrixPadded = &matrixPadded;
    DWORD dwFunc = FUNC_CMatrix__ConvertToEulerAngles;

    float* pfX = &fX;
    float* pfY = &fY;
    float* pfZ = &fZ;
    int iUnknown = 21;
    _asm
    {
        push    iUnknown
            push    pfZ
            push    pfY
            push    pfX
            mov     ecx, pMatrixPadded
            call    dwFunc
    }
}


bool CWorldSA::ProcessLineOfSight(const CVector * vecStart, const CVector * vecEnd, CColPoint ** colCollision, 
                                  CEntity ** CollisionEntity,
                                  const SLineOfSightFlags flags,
                                  SLineOfSightBuildingResult* pBuildingResult )
{
    DEBUG_TRACE("VOID CWorldSA::ProcessLineOfSight(CVector * vecStart, CVector * vecEnd, CColPoint * colCollision, CEntity * CollisionEntity)");
    DWORD dwPadding[100]; // stops the function missbehaving and overwriting the return address
    dwPadding [0] = 0;  // prevent the warning and eventual compiler optimizations from removing it

    CColPointSA * pColPointSA = new CColPointSA();
    CColPointSAInterface * pColPointSAInterface = pColPointSA->GetInterface();  

    //DWORD targetEntity;
    CEntitySAInterface * targetEntity = NULL;
    bool bReturn = false;

    DWORD dwFunc = FUNC_ProcessLineOfSight;
    // bool bCheckBuildings = true,                 bool bCheckVehicles = true,     bool bCheckPeds = true, 
    // bool bCheckObjects = true,                   bool bCheckDummies = true,      bool bSeeThroughStuff = false, 
    // bool bIgnoreSomeObjectsForCamera = false,    bool bShootThroughStuff = false
    MemPutFast < BYTE > ( VAR_CWorld_bIncludeCarTires, flags.bCheckCarTires );

    _asm
    {
        push    flags.bShootThroughStuff
        push    flags.bIgnoreSomeObjectsForCamera
        push    flags.bSeeThroughStuff
        push    flags.bCheckDummies
        push    flags.bCheckObjects
        push    flags.bCheckPeds
        push    flags.bCheckVehicles
        push    flags.bCheckBuildings
        lea     eax, targetEntity
        push    eax
        push    pColPointSAInterface    
        push    vecEnd
        push    vecStart    
        call    dwFunc
        mov     bReturn, al
        add     esp, 0x30
    }

    MemPutFast < BYTE > ( VAR_CWorld_bIncludeCarTires, 0 );

    // Building info needed?
    if ( pBuildingResult )
    {
        CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
        if ( pPools )
        {
            if ( targetEntity && targetEntity->nType == ENTITY_TYPE_BUILDING )
            {
                pBuildingResult->bValid = true;
                pBuildingResult->usModelID = targetEntity->m_nModelIndex;
                if ( targetEntity->m_pLod )
                    pBuildingResult->usLODModelID = targetEntity->m_pLod->m_nModelIndex;
                else
                    pBuildingResult->usLODModelID = 0;

                pBuildingResult->pInterface = targetEntity;
                pBuildingResult->vecPosition = targetEntity->Placeable.m_transform.m_translate;
                if ( targetEntity->Placeable.matrix )
                {
                    CVector& vecRotation = pBuildingResult->vecRotation;
                    ConvertMatrixToEulerAngles ( *targetEntity->Placeable.matrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
                    vecRotation = -vecRotation;
                }
            }
            if ( targetEntity && targetEntity->nType == ENTITY_TYPE_OBJECT )
            {
                pBuildingResult->bValid = true;
                pBuildingResult->usModelID = targetEntity->m_nModelIndex;
                if ( targetEntity->m_pLod )
                    pBuildingResult->usLODModelID = targetEntity->m_pLod->m_nModelIndex;
                else
                    pBuildingResult->usLODModelID = 0;

                pBuildingResult->pInterface = targetEntity;
            }
        }
    }


    if ( CollisionEntity )
    {
        CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
        if(pPools)
        {
            if(targetEntity)
            {
                switch (targetEntity->nType)
                {
                    case ENTITY_TYPE_PED:
                        *CollisionEntity = pPools->GetPed((DWORD *)targetEntity);
                        break;
                    case ENTITY_TYPE_OBJECT:
                        *CollisionEntity = pPools->GetObject((DWORD *)targetEntity);
                        break;
                    case ENTITY_TYPE_VEHICLE:
                        *CollisionEntity = pPools->GetVehicle((DWORD *)targetEntity);
                        break;
                }

                /*CEntitySA * entity = new CEntitySA();
                entity->SetInterface((CEntitySAInterface *)targetEntity);
                eEntityType EntityType = entity->GetEntityType();
                delete entity;
                switch(EntityType)
                {
                case ENTITY_TYPE_PED:
                case ENTITY_TYPE_OBJECT:
                    *CollisionEntity = pPools->GetPed((DWORD *)targetEntity);
                    if ( *CollisionEntity )
                        break;
                    *CollisionEntity = pPools->GetObject((CObjectSAInterface *)targetEntity);
                    break;
                case ENTITY_TYPE_VEHICLE:
                    *CollisionEntity = pPools->GetVehicle((CVehicleSAInterface *)targetEntity);
                    break;

                }*/
            }
        }
    }
    if ( colCollision ) *colCollision = pColPointSA;
    else pColPointSA->Destroy ();

    return bReturn;
}


void CWorldSA::IgnoreEntity(CEntity * pEntity)
{
    DEBUG_TRACE("VOID CWorldSA::IgnoreEntity(CEntity * entity)");

    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );

    if ( pEntitySA )
        MemPutFast < DWORD > ( VAR_IgnoredEntity, (DWORD) pEntitySA->GetInterface () );
    else
        MemPutFast < DWORD > ( VAR_IgnoredEntity, 0 );
}

// technically this is in CTheZones
BYTE CWorldSA::GetLevelFromPosition(CVector * vecPosition)
{
    DEBUG_TRACE("BYTE CWorldSA::GetLevelFromPosition(CVector * vecPosition)");
    DWORD dwFunc = FUNC_GetLevelFromPosition;
    BYTE bReturn = 0;
    _asm
    {
        push    vecPosition
        call    dwFunc
        mov     bReturn, al
        pop     eax
    }
    return bReturn;
}

float CWorldSA::FindGroundZForPosition(float fX, float fY)
{
    DEBUG_TRACE("FLOAT CWorldSA::FindGroundZForPosition(FLOAT fX, FLOAT fY)");
    DWORD dwFunc = FUNC_FindGroundZFor3DCoord;
    FLOAT fReturn = 0;
    _asm
    {
        push    fY
        push    fX
        call    dwFunc
        fstp    fReturn
        add     esp, 8
    }
    return fReturn;
}

float CWorldSA::FindGroundZFor3DPosition(CVector * vecPosition)
{
    DEBUG_TRACE("FLOAT CWorldSA::FindGroundZFor3DPosition(CVector * vecPosition)");
    DWORD dwFunc = FUNC_FindGroundZFor3DCoord;
    FLOAT fReturn = 0;
    FLOAT fX = vecPosition->fX;
    FLOAT fY = vecPosition->fY;
    FLOAT fZ = vecPosition->fZ;
    _asm
    {
        push    0
        push    0
        push    fZ
        push    fY
        push    fX
        call    dwFunc
        fstp    fReturn
        add     esp, 0x14
    }
    return fReturn;
}

void CWorldSA::LoadMapAroundPoint(CVector * vecPosition, FLOAT fRadius)
{
    DEBUG_TRACE("VOID CWorldSA::LoadMapAroundPoint(CVector * vecPosition, FLOAT fRadius)");
    DWORD dwFunc = FUNC_CTimer_Stop;
    _asm
    {
        call    dwFunc
    }

    dwFunc = FUNC_CRenderer_RequestObjectsInDirection;
    _asm
    {
        push    32
        push    fRadius
        push    vecPosition
        call    dwFunc
        add     esp, 12
    }


    dwFunc = FUNC_CStreaming_LoadScene;
    _asm
    {
        push    vecPosition
        call    dwFunc
        add     esp, 4
    }

    dwFunc = FUNC_CTimer_Update;
    _asm
    {
        call    dwFunc
    }

}


bool CWorldSA::IsLineOfSightClear ( const CVector * vecStart, const CVector * vecEnd, const SLineOfSightFlags flags )
{
    DWORD dwFunc = FUNC_IsLineOfSightClear;
    bool bReturn = false;
    // bool bCheckBuildings = true, bool bCheckVehicles = true, bool bCheckPeds = true, 
    // bool bCheckObjects = true, bool bCheckDummies = true, bool bSeeThroughStuff = false, 
    // bool bIgnoreSomeObjectsForCamera = false

    _asm
    {
        push    flags.bIgnoreSomeObjectsForCamera
        push    flags.bSeeThroughStuff
        push    flags.bCheckDummies
        push    flags.bCheckObjects
        push    flags.bCheckPeds
        push    flags.bCheckVehicles
        push    flags.bCheckBuildings
        push    vecEnd
        push    vecStart    
        call    dwFunc
        mov     bReturn, al
        add     esp, 0x24
    }
    return bReturn;
}

bool CWorldSA::HasCollisionBeenLoaded ( CVector * vecPosition )
{
    DWORD dwFunc = FUNC_HasCollisionBeenLoaded;
    bool bRet = false;
    _asm
    {
        push    0
        push    vecPosition
        call    dwFunc
        mov     bRet, al
        add     esp, 8
    } 
    return bRet;
}

DWORD CWorldSA::GetCurrentArea ( void )
{
    return *(DWORD *)VAR_currArea;
}

void CWorldSA::SetCurrentArea ( DWORD dwArea )
{
    MemPutFast < DWORD > ( VAR_currArea, dwArea );

    DWORD dwFunc = FUNC_RemoveBuildingsNotInArea;
    _asm
    {
        push    dwArea
        call    dwFunc
        add     esp, 4
    }
}

void CWorldSA::SetJetpackMaxHeight ( float fHeight )
{
    MemPut < float > ( VAR_fJetpackMaxHeight, fHeight );
}

float CWorldSA::GetJetpackMaxHeight ( void )
{
    return *(float *)(VAR_fJetpackMaxHeight);
}

void CWorldSA::SetAircraftMaxHeight ( float fHeight )
{
    g_pCore->GetMultiplayer ( )->SetAircraftMaxHeight ( fHeight );
}

float CWorldSA::GetAircraftMaxHeight ( void )
{
    return g_pCore->GetMultiplayer ( )->GetAircraftMaxHeight ( );
}

void CWorldSA::SetAircraftMaxVelocity ( float fVelocity )
{
    g_pCore->GetMultiplayer ( )->SetAircraftMaxVelocity ( fVelocity );
}

float CWorldSA::GetAircraftMaxVelocity ( void )
{
    return g_pCore->GetMultiplayer ( )->GetAircraftMaxVelocity ( );
}

void CWorldSA::SetOcclusionsEnabled ( bool bEnabled )
{
    if ( !bEnabled )
    {
        MemPut < BYTE > ( FUNC_COcclusion_ProcessBeforeRendering, 0xC3 );   // retn
        MemPutFast < int > ( VAR_COcclusion_NumActiveOccluders, 0 );
    }
    else
    {
        MemPut < BYTE > ( FUNC_COcclusion_ProcessBeforeRendering, 0x51 );   // Standard value
    }
}

bool CWorldSA::GetOcclusionsEnabled ( void )
{
    if ( *(BYTE*)FUNC_COcclusion_ProcessBeforeRendering == 0x51 )           // Is standard value ?
        return true;
    return false;
}

void CWorldSA::RemoveBuilding ( unsigned short usModelToRemove, float fRange, float fX, float fY, float fZ, char cInterior )
{    
    // New building Removal
    SBuildingRemoval* pRemoval = new SBuildingRemoval();
    pRemoval->m_usModel = usModelToRemove;
    pRemoval->m_vecPos.fX = fX;
    pRemoval->m_vecPos.fY = fY;
    pRemoval->m_vecPos.fZ = fZ;
    pRemoval->m_fRadius = fRange;
    pRemoval->m_cInterior = cInterior;
    // Push it to the back of the removal list
    m_pBuildingRemovals->insert ( std::pair<unsigned short, SBuildingRemoval*> ( usModelToRemove, pRemoval ) );

    bool bFound = false;
    // Init loop variables
    std::pair < std::multimap < unsigned short, sDataBuildingRemovalItem*>::iterator, std::multimap < unsigned short, sDataBuildingRemovalItem* >::iterator> iterators = m_pDataBuildings->equal_range ( usModelToRemove );
    std::multimap < unsigned short, sDataBuildingRemovalItem* > ::const_iterator iter = iterators.first;
    for ( ; iter != iterators.second; ++iter )
    {
        sDataBuildingRemovalItem * pFind = (*iter).second;
        if ( pFind )
        {
            // if the count is <= 0 and the interface is valid check the distance in case we found a removal (count is used to store if we have already removed this once)
            if ( pFind->m_iCount <= 0 && pFind->m_pInterface )
            {
                // Grab distances across each axis
                float fDistanceX = fX - pFind->m_pInterface->Placeable.m_transform.m_translate.fX;
                float fDistanceY = fY - pFind->m_pInterface->Placeable.m_transform.m_translate.fY;
                float fDistanceZ = fZ - pFind->m_pInterface->Placeable.m_transform.m_translate.fZ;

                if ( pFind->m_pInterface->Placeable.matrix != NULL )
                {
                    fDistanceX = fX - pFind->m_pInterface->Placeable.matrix->vPos.fX;
                    fDistanceY = fY - pFind->m_pInterface->Placeable.matrix->vPos.fY;
                    fDistanceZ = fZ - pFind->m_pInterface->Placeable.matrix->vPos.fZ;
                }

                // Square root 'em
                float fDistance = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );

                // Is it in range
                if ( fDistance <= fRange && ( pFind->m_pInterface->m_areaCode == cInterior || cInterior == -1 ) )
                {
                    CEntitySAInterface * pInterface = pFind->m_pInterface;
                    //while ( pInterface && pInterface != NULL )
                    // if the interface is valid
                    if ( pInterface && pInterface != NULL )
                    {
                        // if the building type is dummy or building and it's not already being removed
                        if ( ( pInterface->nType == ENTITY_TYPE_BUILDING || pInterface->nType == ENTITY_TYPE_DUMMY || pInterface->nType == ENTITY_TYPE_OBJECT ) && pInterface->bRemoveFromWorld != 1 )
                        {
                            if ( (DWORD)(pInterface->vtbl) != VTBL_CPlaceable )
                            {
                                // Add the Data Building to the list
                                pRemoval->AddDataBuilding ( pInterface );
                                // Remove the model from the world
                                Remove ( pInterface, BuildingRemoval2 );
                                bFound = true;

                            }
                        }
                        // Get next LOD ( LOD's can have LOD's so we keep checking pInterface )
                        //pInterface = pInterface->m_pLod;
                    }
                    // Set the count.
                    pFind->m_iCount = 1;
                }
            }
        }
    }
    
    std::pair < std::multimap < unsigned short, sBuildingRemovalItem*>::iterator, std::multimap < unsigned short, sBuildingRemovalItem* >::iterator> iteratorsBinary = m_pBinaryBuildings->equal_range ( usModelToRemove );
    std::multimap < unsigned short, sBuildingRemovalItem* > ::const_iterator iterBinary = iteratorsBinary.first;
    for ( ; iterBinary != iteratorsBinary.second; ++iterBinary )
    {
        sBuildingRemovalItem * pFindBinary = (*iterBinary).second;
        if ( pFindBinary )
        {
            // if the count is <= 0 and the interface is valid check the distance in case we found a removal (count is used to store if we have already removed this once)
            if ( pFindBinary->m_iCount <= 0 && pFindBinary->m_pInterface )
            {
                // Grab distances across each axis
                float fDistanceX = fX - pFindBinary->m_pInterface->Placeable.m_transform.m_translate.fX;
                float fDistanceY = fY - pFindBinary->m_pInterface->Placeable.m_transform.m_translate.fY;
                float fDistanceZ = fZ - pFindBinary->m_pInterface->Placeable.m_transform.m_translate.fZ;

                if ( pFindBinary->m_pInterface->Placeable.matrix != NULL )
                {
                    fDistanceX = fX - pFindBinary->m_pInterface->Placeable.matrix->vPos.fX;
                    fDistanceY = fY - pFindBinary->m_pInterface->Placeable.matrix->vPos.fY;
                    fDistanceZ = fZ - pFindBinary->m_pInterface->Placeable.matrix->vPos.fZ;
                }

                // Square root 'em
                float fDistance = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );

                // Is it in range
                if ( fDistance <= fRange && ( pFindBinary->m_pInterface->m_areaCode == cInterior || cInterior == -1 ) )
                {
                    CEntitySAInterface * pInterface = pFindBinary->m_pInterface;
                    //while ( pInterface && pInterface != NULL )
                    // if the interface is valid
                    if ( pInterface && pInterface != NULL )
                    {
                        // if the building type is dummy or building and it's not already being removed
                        if ( ( pInterface->nType == ENTITY_TYPE_BUILDING || pInterface->nType == ENTITY_TYPE_DUMMY || pInterface->nType == ENTITY_TYPE_OBJECT ) && pInterface->bRemoveFromWorld != 1 )
                        {
                            if ( (DWORD)(pInterface->vtbl) != VTBL_CPlaceable )
                            {
                                // Add the Data Building to the list
                                pRemoval->AddBinaryBuilding ( pInterface );
                                // Remove the model from the world
                                Remove ( pInterface, BuildingRemoval2 );
                                bFound = true;
                            }
                        }
                        // Get next LOD ( LOD's can have LOD's so we keep checking pInterface )
                        //pInterface = pInterface->m_pLod;
                    }
                    // Set the count.
                    pFindBinary->m_iCount = 1;
                }
            }
        }
    }
    if ( bFound )
        pGame->GetModelInfo ( usModelToRemove )->RestreamIPL ();
}

bool CWorldSA::RestoreBuilding ( unsigned short usModelToRestore, float fRange, float fX, float fY, float fZ, char cInterior )
{        
    bool bSuccess = false;
    // Init some variables
    std::pair < std::multimap < unsigned short, SBuildingRemoval* >::iterator, std::multimap < unsigned short, SBuildingRemoval* >::iterator> iterators = m_pBuildingRemovals->equal_range ( usModelToRestore );
    std::multimap < unsigned short, SBuildingRemoval* > ::const_iterator iter = iterators.first;
    // Loop through the buildings list
    for ( ; iter != iterators.second;  )
    {
        SBuildingRemoval * pFind = (*iter).second;
        // if pFind is valid and the model is the same
        if ( pFind )
        {
            // Grab distances across each axis
            float fDistanceX = fX - pFind->m_vecPos.fX;
            float fDistanceY = fY - pFind->m_vecPos.fY;
            float fDistanceZ = fZ - pFind->m_vecPos.fZ;

            // Square root 'em
            float fDistance = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );
            if ( fDistance <= pFind->m_fRadius && ( cInterior == -1 || pFind->m_cInterior == cInterior ) )
            {
                // Init some variables
                CEntitySAInterface * pEntity = NULL;
                std::list < CEntitySAInterface* > ::const_iterator entityIter = pFind->m_pBinaryRemoveList->begin ( );
                if ( pFind->m_pBinaryRemoveList->empty ( ) == false ) 
                {
                    // Loop through the Binary object list
                    for ( ; entityIter != pFind->m_pBinaryRemoveList->end (); )
                    {
                        // Grab the pEntity
                        pEntity = (*entityIter);
                        // if it's valid re-add it to the world.
                        if ( pEntity != NULL )
                        {
                            // if the building type is dummy or building and it's not already being removed
                            if ( ( pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT ) && pEntity->bRemoveFromWorld != 1 )
                            {
                                // Don't call this on entities being removed.
                                if ( (DWORD)(pEntity->vtbl) != VTBL_CPlaceable )
                                {
                                    Add ( pEntity, Building_Restore );
                                }
                            }
                            // Remove it from the binary list
                            pFind->m_pBinaryRemoveList->erase ( entityIter++ );

                            // If the building isn't streamed in, we won't find the building in the list (because the hook wasn't called) -> removeWorldModel doesn't work next time
                            AddBinaryBuilding ( pEntity );
                        }
                        else
                            ++entityIter;
                    }
                }
                // Start the iterator with the data remove list first item
                entityIter = pFind->m_pDataRemoveList->begin ( );
                if ( pFind->m_pDataRemoveList->empty ( ) == false )
                {
                    // Loop through the Data list
                    for ( ; entityIter != pFind->m_pDataRemoveList->end (); )
                    {
                        // Grab the pEntity
                        pEntity = (*entityIter);
                        // if it's valid re-add it to the world.
                        if ( pEntity != NULL )
                        {
                            // if the building type is dummy or building and it's not already being removed
                            if ( ( pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT ) && pEntity->bRemoveFromWorld != 1 )
                            {
                                if ( (DWORD)(pEntity->vtbl) != VTBL_CPlaceable )
                                {
                                    Add ( pEntity, Building_Restore2 );
                                }
                            }
                            pFind->m_pDataRemoveList->erase ( entityIter++ );
                        }
                        else
                        {
                            ++entityIter;
                        }
                    }
                }
                // Remove the building from the list
                m_pBuildingRemovals->erase ( iter++ );
                delete pFind;
                // Success! don't return incase there are any others to delete
                bSuccess = true;
            }
            else
                iter++;
        }
        else
            iter++;
    }
    std::pair < std::multimap < unsigned short, sDataBuildingRemovalItem*>::iterator, std::multimap < unsigned short, sDataBuildingRemovalItem* >::iterator> dataBuildingIterators = m_pDataBuildings->equal_range ( usModelToRestore );
    std::multimap < unsigned short, sDataBuildingRemovalItem* > ::const_iterator iterator = dataBuildingIterators.first;
    for ( ; iterator != dataBuildingIterators.second; ++iterator )
    {
        sDataBuildingRemovalItem * pFound = (*iterator).second;
        if ( pFound )
        {
            // Grab distances across each axis
            float fDistanceX = fX - pFound->m_pInterface->Placeable.m_transform.m_translate.fX;
            float fDistanceY = fY - pFound->m_pInterface->Placeable.m_transform.m_translate.fY;
            float fDistanceZ = fZ - pFound->m_pInterface->Placeable.m_transform.m_translate.fZ;

            if ( pFound->m_pInterface->Placeable.matrix != NULL )
            {
                fDistanceX = fX - pFound->m_pInterface->Placeable.matrix->vPos.fX;
                fDistanceY = fY - pFound->m_pInterface->Placeable.matrix->vPos.fY;
                fDistanceZ = fZ - pFound->m_pInterface->Placeable.matrix->vPos.fZ;
            }

            // Square root 'em
            float fDistance = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );
            if ( fDistance <= fRange && ( cInterior == -1 || pFound->m_pInterface->m_areaCode == cInterior ) )
            {
                // Fix the removed count.
                pFound->m_iCount--;
                if ( pFound->m_iCount < 0 )
                    pFound->m_iCount = 0;
            }
        }
    }

    std::pair < std::multimap < unsigned short, sBuildingRemovalItem*>::iterator, std::multimap < unsigned short, sBuildingRemovalItem* >::iterator> binaryBuildingIterators = m_pBinaryBuildings->equal_range ( usModelToRestore );
    std::multimap < unsigned short, sBuildingRemovalItem* > ::const_iterator iteratorBinary = binaryBuildingIterators.first;
    for ( ; iteratorBinary != binaryBuildingIterators.second; ++iteratorBinary )
    {
        sBuildingRemovalItem * pFoundBinary = (*iteratorBinary).second;
        if ( pFoundBinary )
        {
            // Grab distances across each axis
            float fDistanceX = fX - pFoundBinary->m_pInterface->Placeable.m_transform.m_translate.fX;
            float fDistanceY = fY - pFoundBinary->m_pInterface->Placeable.m_transform.m_translate.fY;
            float fDistanceZ = fZ - pFoundBinary->m_pInterface->Placeable.m_transform.m_translate.fZ;

            if ( pFoundBinary->m_pInterface->Placeable.matrix != NULL )
            {
                fDistanceX = fX - pFoundBinary->m_pInterface->Placeable.matrix->vPos.fX;
                fDistanceY = fY - pFoundBinary->m_pInterface->Placeable.matrix->vPos.fY;
                fDistanceZ = fZ - pFoundBinary->m_pInterface->Placeable.matrix->vPos.fZ;
            }

            // Square root 'em
            float fDistance = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );
            if ( fDistance <= fRange && ( cInterior == -1 || pFoundBinary->m_pInterface->m_areaCode == cInterior ) )
            {
                // Fix the removed count.
                pFoundBinary->m_iCount--;
                if ( pFoundBinary->m_iCount < 0 )
                    pFoundBinary->m_iCount = 0;
            }
        }
    }
    return bSuccess;
}

// Check Distance to see if the model being requested is in the radius
bool CWorldSA::IsRemovedModelInRadius ( SIPLInst* pInst )
{
    // Init some variables
    std::pair < std::multimap < unsigned short, SBuildingRemoval* >::iterator, std::multimap < unsigned short, SBuildingRemoval* >::iterator> iterators = m_pBuildingRemovals->equal_range ( pInst->m_nModelIndex );
    std::multimap < unsigned short, SBuildingRemoval* > ::const_iterator iter = iterators.first;
    // Loop through the buildings list
    for ( ; iter != iterators.second; ++iter )
    {
        SBuildingRemoval * pFind = (*iter).second;
        // if pFind is valid and the model is the same
        if ( pFind )
        {
            // Grab the distance
            float fDistanceX = pFind->m_vecPos.fX - pInst->m_pPosition.fX;
            float fDistanceY = pFind->m_vecPos.fY - pInst->m_pPosition.fY;
            float fDistanceZ = pFind->m_vecPos.fZ - pInst->m_pPosition.fZ;

            float fDistance = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );
            // is it in the removal spheres radius if so return else keep looking
            if ( fDistance <=  pFind->m_fRadius && ( pFind->m_cInterior == -1 || pFind->m_cInterior == pInst->m_nInterior ) )
            {
                return true;
            }
        }
    }
    return false;
}

// Check Distance to see if the model being requested is in the radius
bool CWorldSA::IsObjectRemoved ( CEntitySAInterface* pInterface )
{
    // Init some variables
    std::pair < std::multimap < unsigned short, SBuildingRemoval* >::iterator, std::multimap < unsigned short, SBuildingRemoval* >::iterator> iterators = m_pBuildingRemovals->equal_range ( pInterface->m_nModelIndex );
    std::multimap < unsigned short, SBuildingRemoval* > ::const_iterator iter = iterators.first;
    // Loop through the buildings list
    for ( ; iter != iterators.second; ++iter )
    {
        SBuildingRemoval * pFind = (*iter).second;
        // if pFind is valid and the model is the same
        if ( pFind )
        {
            // Are we using the interior param?? if so check for a match
            if ( pFind->m_cInterior == -1 || pFind->m_cInterior == pInterface->m_areaCode )
            {
                // Grab the distance
                float fDistanceX = pFind->m_vecPos.fX - pInterface->Placeable.m_transform.m_translate.fX;
                float fDistanceY = pFind->m_vecPos.fY - pInterface->Placeable.m_transform.m_translate.fY;
                float fDistanceZ = pFind->m_vecPos.fZ - pInterface->Placeable.m_transform.m_translate.fZ;

                if ( pInterface->Placeable.matrix != NULL )
                {
                    fDistanceX = pFind->m_vecPos.fX - pInterface->Placeable.matrix->vPos.fX;
                    fDistanceY = pFind->m_vecPos.fY - pInterface->Placeable.matrix->vPos.fY;
                    fDistanceZ = pFind->m_vecPos.fZ - pInterface->Placeable.matrix->vPos.fZ;
                }

                float fDistance = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );
                // is it in the removal spheres radius if so return else keep looking
                if ( fDistance <=  pFind->m_fRadius )
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// Check if a given model is replaced
bool CWorldSA::IsModelRemoved ( unsigned short usModelID )
{
    return m_pBuildingRemovals->count ( usModelID ) > 0;
}

// Check if a given model is replaced
bool CWorldSA::IsDataModelRemoved ( unsigned short usModelID )
{
    return m_pBuildingRemovals->count ( usModelID ) > 0;
}


// Resets deleted list
void CWorldSA::ClearRemovedBuildingLists ( )
{
    // Ensure no memory leaks by deleting items.
    std::multimap < unsigned short, SBuildingRemoval* > ::const_iterator iter = m_pBuildingRemovals->begin ( );
    for ( ; iter != m_pBuildingRemovals->end ( ); )
    {
        SBuildingRemoval * pFind = (*iter).second;
        if ( pFind )
        {
            // Init some variables
            CEntitySAInterface * pEntity = NULL;
            std::list < CEntitySAInterface* > ::const_iterator entityIter = pFind->m_pBinaryRemoveList->begin ( );
            if ( pFind->m_pBinaryRemoveList->empty ( ) == false )
            {
                // Loop through the Binary remove list
                for ( ; entityIter != pFind->m_pBinaryRemoveList->end (); ++entityIter )
                {
                    // Grab the pEntity
                    pEntity = (*entityIter);
                    // if it's valid re-add it to the world.
                    if ( pEntity && pEntity != NULL )
                    {
                        // if the building type is dummy or building and it's not already being removed
                        if ( ( pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT ) && pEntity->bRemoveFromWorld != 1 )
                        {
                            // Don't call this on entities being removed.
                            if ( (DWORD)(pEntity->vtbl) != VTBL_CPlaceable )
                            {
                                Add ( pEntity, BuildingRemovalReset );
                            }
                        }
                    }
                }
            }
            entityIter = pFind->m_pDataRemoveList->begin ( );
            if ( pFind->m_pDataRemoveList->empty ( ) == false )
            {
                // Loop through the Data list
                for ( ; entityIter != pFind->m_pDataRemoveList->end (); ++entityIter )
                {
                    // Grab the pEntity
                    pEntity = (*entityIter);
                    // if it's valid re-add it to the world.
                    if ( pEntity && pEntity != NULL )
                    {
                        // if the building type is dummy or building and it's not already being removed
                        if ( ( pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT ) && pEntity->bRemoveFromWorld != 1 )
                        {
                            // Don't call this on entities being removed.
                            if ( (DWORD)(pEntity->vtbl) != VTBL_CPlaceable )
                            {
                                Add ( pEntity, BuildingRemovalReset2 );
                            }
                        }
                    }
                }
            }
            m_pBuildingRemovals->erase ( iter++ );
        }
        else
            iter++;
    }
    // Init some variables
    std::multimap < unsigned short, sDataBuildingRemovalItem* > ::const_iterator iterator = m_pDataBuildings->begin ( );
    // Loop through the data building list
    for ( ; iterator != m_pDataBuildings->end ( ); ++iterator )
    {
        sDataBuildingRemovalItem * pFound = (*iterator).second;
        if ( pFound )
        {
            // Set the count to 0 so we can remove it again
            pFound->m_iCount = 0;
        }
    }
    // Init some variables 
    std::multimap < unsigned short, sBuildingRemovalItem* > ::const_iterator iteratorBinary = m_pBinaryBuildings->begin ( );
    // Loop through the data building list
    for ( ; iteratorBinary != m_pBinaryBuildings->end ( ); ++iteratorBinary )
    {
        sBuildingRemovalItem * pFoundBinary = (*iteratorBinary).second;
        if ( pFoundBinary )
        {
            // Set the count to 0 so we can remove it again
            pFoundBinary->m_iCount = 0;
        }
    }
    // Delete old building lists
    delete m_pBuildingRemovals;
    // Create new
    m_pBuildingRemovals = new std::multimap< unsigned short, SBuildingRemoval* >;
}


// Resets deleted list
SBuildingRemoval* CWorldSA::GetBuildingRemoval ( CEntitySAInterface * pInterface )
{
    // Init some variables
    std::pair < std::multimap < unsigned short, SBuildingRemoval* >::iterator, std::multimap < unsigned short, SBuildingRemoval* >::iterator> iterators = m_pBuildingRemovals->equal_range ( pInterface->m_nModelIndex );
    std::multimap < unsigned short, SBuildingRemoval* > ::const_iterator iter = iterators.first;
    // Loop through the buildings list
    for ( ; iter != iterators.second; ++iter )
    {
        SBuildingRemoval * pFind = (*iter).second;
        // if pFind is valid and the model is the same
        if ( pFind )
        {
            // Grab the distance
            float fDistanceX = pFind->m_vecPos.fX - pInterface->Placeable.m_transform.m_translate.fX;
            float fDistanceY = pFind->m_vecPos.fY - pInterface->Placeable.m_transform.m_translate.fY;
            float fDistanceZ = pFind->m_vecPos.fZ - pInterface->Placeable.m_transform.m_translate.fZ;

            if ( pInterface->Placeable.matrix != NULL )
            {
                fDistanceX = pFind->m_vecPos.fX - pInterface->Placeable.matrix->vPos.fX;
                fDistanceY = pFind->m_vecPos.fY - pInterface->Placeable.matrix->vPos.fY;
                fDistanceZ = pFind->m_vecPos.fZ - pInterface->Placeable.matrix->vPos.fZ;
            }

            float fDistance = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );
            // is it in the removal spheres radius if so return else keep looking
            if ( fDistance <=  pFind->m_fRadius && ( pFind->m_cInterior == -1 || pFind->m_cInterior == pInterface->m_areaCode ) )
            {
                return pFind;
            }
        }
    }
    return NULL;
}

void CWorldSA::AddDataBuilding ( CEntitySAInterface * pInterface )
{
    // Create a new building removal
    sDataBuildingRemovalItem * pBuildingRemoval = new sDataBuildingRemovalItem ( pInterface, true );
    // Insert it with the model index so we can fast lookup
    m_pDataBuildings->insert ( std::pair<unsigned short, sDataBuildingRemovalItem*> ( (unsigned short)pInterface->m_nModelIndex, pBuildingRemoval ) );
}

void CWorldSA::AddBinaryBuilding ( CEntitySAInterface * pInterface )
{
    // Create a new building removal
    sBuildingRemovalItem * pBuildingRemoval = new sBuildingRemovalItem ( pInterface, false );
    // Insert it with the model index so we can fast lookup
    m_pBinaryBuildings->insert ( std::pair<unsigned short, sBuildingRemovalItem*> ( (unsigned short)pInterface->m_nModelIndex, pBuildingRemoval ) );
}

void CWorldSA::RemoveWorldBuildingFromLists ( CEntitySAInterface * pInterface )
{
    std::pair < std::multimap < unsigned short, SBuildingRemoval* >::iterator, std::multimap < unsigned short, SBuildingRemoval* >::iterator> iterators = m_pBuildingRemovals->equal_range ( pInterface->m_nModelIndex );
    std::multimap < unsigned short, SBuildingRemoval* > ::const_iterator iter = iterators.first;

    // Loop through the buildings list
    for ( ; iter != iterators.second; ++iter )
    {
        SBuildingRemoval * pFind = (*iter).second;
        // if pFind is valid and the model is the same
        if ( pFind )
        {
            CEntitySAInterface * pEntity = NULL;
            // if the binary remove list is empty don't continue
            if ( pFind->m_pBinaryRemoveList->empty ( ) == false )
            {
                // grab the beginning
                std::list < CEntitySAInterface* > ::const_iterator entityIter = pFind->m_pBinaryRemoveList->begin ( );
                // Loop through the binary remove list
                for ( ; entityIter != pFind->m_pBinaryRemoveList->end (); )
                {
                    pEntity = (*entityIter);
                    // is the pointer the same as the one being deleted
                    if ( (DWORD)pEntity == (DWORD)pInterface )
                    {
                        // remove it from the binary removed list for this removal
                        pFind->m_pBinaryRemoveList->erase ( entityIter++ );
                    }
                    else
                        entityIter++;
                }
            }
            if ( pFind->m_pDataRemoveList->empty ( ) == false )
            {
                std::list < CEntitySAInterface* > ::const_iterator entityIter = pFind->m_pDataRemoveList->begin ( );
                // Loop through the Data list
                for ( ; entityIter != pFind->m_pDataRemoveList->end (); )
                {
                    // Grab the pEntity
                    pEntity = (*entityIter);
                    if ( pEntity )
                    {
                        // is the pointer the same as the one being deleted
                        if ( (DWORD)pEntity == (DWORD)pInterface )
                        {
                            // remove it from the data removed list for this removal
                            pFind->m_pDataRemoveList->erase ( entityIter++ );
                        }
                        else
                            entityIter++;
                    }
                    else
                        entityIter++;
                }
            }
        }
    }

    // Init some variables
    std::pair < std::multimap < unsigned short, sDataBuildingRemovalItem* >::iterator, std::multimap < unsigned short, sDataBuildingRemovalItem* >::iterator> dataIterators = m_pDataBuildings->equal_range ( pInterface->m_nModelIndex );
    std::multimap < unsigned short, sDataBuildingRemovalItem* > ::const_iterator iterator = dataIterators.first;
    for ( ; iterator != dataIterators.second; )
    {
        sDataBuildingRemovalItem * pFound = (*iterator).second;
        if ( pFound )
        {
            // is the pointer the same as the one being deleted
            if ( (DWORD)pFound->m_pInterface == (DWORD)pInterface )
            {
                // remove it from the data buildings list so we don't try and remove or add it again.
                m_pDataBuildings->erase ( iterator++ );
            }
            else
                iterator++;
        }
        else
            iterator++;
    }
    // Init some variables
    std::pair < std::multimap < unsigned short, sBuildingRemovalItem* >::iterator, std::multimap < unsigned short, sBuildingRemovalItem* >::iterator> binaryIterators = m_pBinaryBuildings->equal_range ( pInterface->m_nModelIndex );
    std::multimap < unsigned short, sBuildingRemovalItem* > ::const_iterator iteratorBinary = binaryIterators.first;
    for ( ; iteratorBinary != binaryIterators.second; )
    {
        sBuildingRemovalItem * pFound = (*iteratorBinary).second;
        if ( pFound )
        {
            // is the pointer the same as the one being deleted
            if ( (DWORD)pFound->m_pInterface == (DWORD)pInterface )
            {
                // remove it from the data buildings list so we don't try and remove or add it again.
                m_pBinaryBuildings->erase ( iteratorBinary++ );
            }
            else
                iteratorBinary++;
        }
        else
            iteratorBinary++;
    }

}