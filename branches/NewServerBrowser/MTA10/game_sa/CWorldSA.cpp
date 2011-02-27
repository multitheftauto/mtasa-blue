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

void CWorldSA::Add ( CEntity * pEntity )
{
    DEBUG_TRACE("VOID CWorldSA::Add ( CEntity * pEntity )");

    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );

    if ( pEntitySA )
    {
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


void CWorldSA::Add ( CEntitySAInterface * entityInterface )
{
    DEBUG_TRACE("VOID CWorldSA::Add ( CEntitySAInterface * entityInterface )");
    DWORD dwFunction = FUNC_Add;
    _asm
    {
        push    entityInterface
        call    dwFunction
        add     esp, 4
    }
}

void CWorldSA::Remove ( CEntity * pEntity )
{
    DEBUG_TRACE("VOID CWorldSA::Remove ( CEntity * entity )");

    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );

    if ( pEntitySA )
    {
        DWORD dwEntity = (DWORD)pEntitySA->GetInterface();
        DWORD dwFunction = FUNC_Remove;
        _asm
        {
            push    dwEntity
            call    dwFunction
            add     esp, 4
        }
    }
}

void CWorldSA::Remove ( CEntitySAInterface * entityInterface )
{
    DEBUG_TRACE("VOID CWorldSA::Remove ( CEntitySAInterface * entityInterface )");
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

bool CWorldSA::ProcessLineOfSight(const CVector * vecStart, const CVector * vecEnd, CColPoint ** colCollision, 
                                  CEntity ** CollisionEntity, bool bCheckBuildings, 
                                  bool bCheckVehicles, bool bCheckPeds, 
                                  bool bCheckObjects, bool bCheckDummies , 
                                  bool bSeeThroughStuff, bool bIgnoreSomeObjectsForCamera, 
                                  bool bShootThroughStuff )
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

    _asm
    {
        push    bShootThroughStuff
        push    bIgnoreSomeObjectsForCamera
        push    bSeeThroughStuff
        push    bCheckDummies
        push    bCheckObjects
        push    bCheckPeds
        push    bCheckVehicles
        push    bCheckBuildings
        lea     eax, targetEntity
        push    eax
        push    pColPointSAInterface    
        push    vecEnd
        push    vecStart    
        call    dwFunc
        mov     bReturn, al
        add     esp, 0x30
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
        MemPut < DWORD > ( VAR_IgnoredEntity, (DWORD) pEntitySA->GetInterface () );  //         *(DWORD *)VAR_IgnoredEntity = (DWORD) pEntitySA->GetInterface ();
    else
        MemPut < DWORD > ( VAR_IgnoredEntity, 0 );  //         *(DWORD *)VAR_IgnoredEntity = 0;
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

bool CWorldSA::IsLineOfSightClear ( CVector * vecStart, CVector * vecEnd, bool bCheckBuildings,
                                   bool bCheckVehicles, bool bCheckPeds, bool bCheckObjects,
                                   bool bCheckDummies, bool bSeeThroughStuff, bool bIgnoreSomeObjectsForCamera )
{
    DWORD dwFunc = FUNC_IsLineOfSightClear;
    bool bReturn = false;
    // bool bCheckBuildings = true, bool bCheckVehicles = true, bool bCheckPeds = true, 
    // bool bCheckObjects = true, bool bCheckDummies = true, bool bSeeThroughStuff = false, 
    // bool bIgnoreSomeObjectsForCamera = false

    _asm
    {
        push    bIgnoreSomeObjectsForCamera
        push    bSeeThroughStuff
        push    bCheckDummies
        push    bCheckObjects
        push    bCheckPeds
        push    bCheckVehicles
        push    bCheckBuildings
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
    MemPut < DWORD > ( VAR_currArea, dwArea );  //     *(DWORD *)VAR_currArea = dwArea;

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
    MemPut < float > ( VAR_fJetpackMaxHeight, fHeight );  //     *(float *)(VAR_fJetpackMaxHeight) = fHeight;
}

float CWorldSA::GetJetpackMaxHeight ( void )
{
    return *(float *)(VAR_fJetpackMaxHeight);
}

void CWorldSA::SetWindVelocity ( float fX, float fY, float fZ )
{
    //Disable
    MemPut < WORD > ( ADDR_WindSpeedSetX, 0xD8DD );  //     *(WORD *)(ADDR_WindSpeedSetX) = 0xD8DD;
    MemPut < DWORD > ( ADDR_WindSpeedSetX + 2, 0x90909090 );  //     *(DWORD *)(ADDR_WindSpeedSetX + 2) = 0x90909090;
    MemPut < WORD > ( ADDR_WindSpeedSetY, 0xD8DD );  //     *(WORD *)(ADDR_WindSpeedSetY) = 0xD8DD;
    MemPut < DWORD > ( ADDR_WindSpeedSetY + 2, 0x90909090 );  //     *(DWORD *)(ADDR_WindSpeedSetY + 2) = 0x90909090;
    MemPut < WORD > ( ADDR_WindSpeedSetZ, 0xD8DD );  //     *(WORD *)(ADDR_WindSpeedSetZ) = 0xD8DD;
    MemPut < DWORD > ( ADDR_WindSpeedSetZ + 2, 0x90909090 );  //     *(DWORD *)(ADDR_WindSpeedSetZ + 2) = 0x90909090;

    MemPut < WORD > ( ADDR_WindSpeedSetX2, 0xD8DD );  //     *(WORD *)(ADDR_WindSpeedSetX2) = 0xD8DD;
    MemPut < DWORD > ( ADDR_WindSpeedSetX2 + 2, 0x90909090 );  //     *(DWORD *)(ADDR_WindSpeedSetX2 + 2) = 0x90909090;
    MemPut < WORD > ( ADDR_WindSpeedSetY2, 0xD8DD );  //     *(WORD *)(ADDR_WindSpeedSetY2) = 0xD8DD;
    MemPut < DWORD > ( ADDR_WindSpeedSetY2 + 2, 0x90909090 );  //     *(DWORD *)(ADDR_WindSpeedSetY2 + 2) = 0x90909090;
    MemPut < WORD > ( ADDR_WindSpeedSetZ2, 0xD8DD );  //     *(WORD *)(ADDR_WindSpeedSetZ2) = 0xD8DD;
    MemPut < DWORD > ( ADDR_WindSpeedSetZ2 + 2, 0x90909090 );  //     *(DWORD *)(ADDR_WindSpeedSetZ2 + 2) = 0x90909090;

    //Set
    MemPut < float > ( VAR_fWindSpeedX, fX );  //     *(float *)(VAR_fWindSpeedX) = fX;
    MemPut < float > ( VAR_fWindSpeedY, fY );  //     *(float *)(VAR_fWindSpeedY) = fY;
    MemPut < float > ( VAR_fWindSpeedZ, fZ );  //     *(float *)(VAR_fWindSpeedZ) = fZ;
}

void CWorldSA::GetWindVelocity ( float& fX, float& fY, float& fZ )
{
    fX = *(float *)(VAR_fWindSpeedX);
    fY = *(float *)(VAR_fWindSpeedY);
    fZ = *(float *)(VAR_fWindSpeedZ);
}

void CWorldSA::RestoreWindVelocity ( void )
{
    MemPut < WORD > ( ADDR_WindSpeedSetX, 0x1DD9 );  //     *(WORD *)(ADDR_WindSpeedSetX) = 0x1DD9;
    MemPut < DWORD > ( ADDR_WindSpeedSetX + 2, 0x00C813E0 );  //     *(DWORD *)(ADDR_WindSpeedSetX + 2) = 0x00C813E0;
    MemPut < WORD > ( ADDR_WindSpeedSetY, 0x1DD9 );  //     *(WORD *)(ADDR_WindSpeedSetY) = 0x1DD9;
    MemPut < DWORD > ( ADDR_WindSpeedSetY + 2, 0x00C813E4 );  //     *(DWORD *)(ADDR_WindSpeedSetY + 2) = 0x00C813E4;
    MemPut < WORD > ( ADDR_WindSpeedSetZ, 0x1DD9 );  //     *(WORD *)(ADDR_WindSpeedSetZ) = 0x1DD9;
    MemPut < DWORD > ( ADDR_WindSpeedSetZ + 2, 0x00C813E8 );  //     *(DWORD *)(ADDR_WindSpeedSetZ + 2) = 0x00C813E8;

    MemPut < WORD > ( ADDR_WindSpeedSetX2, 0x15D9 );  //     *(WORD *)(ADDR_WindSpeedSetX2) = 0x15D9;
    MemPut < DWORD > ( ADDR_WindSpeedSetX2 + 2, 0x00C813E0 );  //     *(DWORD *)(ADDR_WindSpeedSetX2 + 2) = 0x00C813E0;
    MemPut < WORD > ( ADDR_WindSpeedSetY2, 0x1DD9 );  //     *(WORD *)(ADDR_WindSpeedSetY2) = 0x1DD9;
    MemPut < DWORD > ( ADDR_WindSpeedSetY2 + 2, 0x00C813E4 );  //     *(DWORD *)(ADDR_WindSpeedSetY2 + 2) = 0x00C813E4;
    MemPut < WORD > ( ADDR_WindSpeedSetZ2, 0x1DD9 );  //     *(WORD *)(ADDR_WindSpeedSetZ2) = 0x1DD9;
    MemPut < DWORD > ( ADDR_WindSpeedSetZ2 + 2, 0x00C813E8 );  //     *(DWORD *)(ADDR_WindSpeedSetZ2 + 2) = 0x00C813E8;
}

float CWorldSA::GetFarClipDistance ( )
{
    return *(float *)(VAR_fFarClipDistance);
}

void CWorldSA::SetFarClipDistance ( float fDistance )
{
    MemPut < BYTE > ( 0x55FCC8, 0xDD );  //     *(BYTE *)0x55FCC8 = 0xDD;
    MemPut < BYTE > ( 0x55FCC9, 0xD8 );  //     *(BYTE *)0x55FCC9 = 0xD8;
    MemPut < BYTE > ( 0x55FCCA, 0x90 );  //     *(BYTE *)0x55FCCA = 0x90;

    MemPut < BYTE > ( 0x5613A3, 0xDD );  //     *(BYTE *)0x5613A3 = 0xDD;
    MemPut < BYTE > ( 0x5613A4, 0xD8 );  //     *(BYTE *)0x5613A4 = 0xD8;
    MemPut < BYTE > ( 0x5613A5, 0x90 );  //     *(BYTE *)0x5613A5 = 0x90;

    MemPut < BYTE > ( 0x560A23, 0xDD );  //     *(BYTE *)0x560A23 = 0xDD;
    MemPut < BYTE > ( 0x560A24, 0xD8 );  //     *(BYTE *)0x560A24 = 0xD8;
    MemPut < BYTE > ( 0x560A25, 0x90 );  //     *(BYTE *)0x560A25 = 0x90;

    MemPut < float > ( VAR_fFarClipDistance, fDistance );  //     *(float *)(VAR_fFarClipDistance) = fDistance;
}

void CWorldSA::RestoreFarClipDistance ( )
{
    BYTE originalFstp[3] = {0xD9, 0x5E, 0x50};

    MemCpy ( (LPVOID)0x55FCC8, &originalFstp, 3 );
    MemCpy ( (LPVOID)0x5613A3, &originalFstp, 3 );
    MemCpy ( (LPVOID)0x560A23, &originalFstp, 3 );
}

float CWorldSA::GetFogDistance ( )
{
    return *(float *)(VAR_fFogDistance);
}

void CWorldSA::SetFogDistance ( float fDistance )
{
    MemPut < BYTE > ( 0x55FCDB, 0xDD );  //     *(BYTE *)0x55FCDB = 0xDD;
    MemPut < BYTE > ( 0x55FCDC, 0xD8 );  //     *(BYTE *)0x55FCDC = 0xD8;
    MemPut < BYTE > ( 0x55FCDD, 0x90 );  //     *(BYTE *)0x55FCDD = 0x90;

    MemPut < float > ( VAR_fFogDistance, fDistance );  //     *(float *)(VAR_fFogDistance) = fDistance;
}

void CWorldSA::RestoreFogDistance ( )
{
    BYTE originalFstp[3] = {0xD9, 0x5E, 0x54};

    MemCpy ( (LPVOID)0x55FCDB, &originalFstp, 3 );
}

void CWorldSA::GetSunColor ( unsigned char& ucCoreRed, unsigned char& ucCoreGreen, unsigned char& ucCoreBlue, unsigned char& ucCoronaRed, unsigned char& ucCoronaGreen, unsigned char& ucCoronaBlue)
{
    ucCoreRed   = *(BYTE *)(VAR_ucSunCoreR);
    ucCoreGreen = *(BYTE *)(VAR_ucSunCoreG);
    ucCoreBlue  = *(BYTE *)(VAR_ucSunCoreB);

    ucCoronaRed   = *(BYTE *)(VAR_ucSunCoronaR);
    ucCoronaGreen = *(BYTE *)(VAR_ucSunCoronaG);
    ucCoronaBlue  = *(BYTE *)(VAR_ucSunCoronaB);
}

void CWorldSA::SetSunColor ( unsigned char ucCoreRed, unsigned char ucCoreGreen, unsigned char ucCoreBlue, unsigned char ucCoronaRed, unsigned char ucCoronaGreen, unsigned char ucCoronaBlue )
{
    MemSet ( (LPVOID)0x55F9B2, 0x90, 4 );
    MemSet ( (LPVOID)0x55F9DD, 0x90, 4 );
    MemSet ( (LPVOID)0x55FA08, 0x90, 4 );
    MemSet ( (LPVOID)0x55FA33, 0x90, 4 );
    MemSet ( (LPVOID)0x55FA5E, 0x90, 4 );
    MemSet ( (LPVOID)0x55FA8D, 0x90, 4 );

    MemPut < BYTE > ( VAR_ucSunCoreR, ucCoreRed );  //     *(BYTE *)(VAR_ucSunCoreR) = ucCoreRed;
    MemPut < BYTE > ( VAR_ucSunCoreG, ucCoreGreen );  //     *(BYTE *)(VAR_ucSunCoreG) = ucCoreGreen;
    MemPut < BYTE > ( VAR_ucSunCoreB, ucCoreBlue );  //     *(BYTE *)(VAR_ucSunCoreB) = ucCoreBlue;

    MemPut < BYTE > ( VAR_ucSunCoronaR, ucCoronaRed );  //     *(BYTE *)(VAR_ucSunCoronaR) = ucCoronaRed;
    MemPut < BYTE > ( VAR_ucSunCoronaG, ucCoronaGreen );  //     *(BYTE *)(VAR_ucSunCoronaG) = ucCoronaGreen;
    MemPut < BYTE > ( VAR_ucSunCoronaB, ucCoronaBlue );  //     *(BYTE *)(VAR_ucSunCoronaB) = ucCoronaBlue;
}

void CWorldSA::ResetSunColor ( )
{
    BYTE originalMov[3] = {0x66, 0x89, 0x46};

    MemCpy ( (LPVOID)0x55F9B2, &originalMov, 3 );
    MemPut < BYTE > ( 0x55F9B5, 0x30 );  //     *(BYTE *)0x55F9B5 = 0x30;
    MemCpy ( (LPVOID)0x55F9DD, &originalMov, 3 );
    MemPut < BYTE > ( 0x55F9E0, 0x32 );  //     *(BYTE *)0x55F9E0 = 0x32;
    MemCpy ( (LPVOID)0x55FA08, &originalMov, 3 );
    MemPut < BYTE > ( 0x55FA0B, 0x34 );  //     *(BYTE *)0x55FA0B = 0x34;

    MemCpy ( (LPVOID)0x55FA33, &originalMov, 3 );
    MemPut < BYTE > ( 0x55FA36, 0x36 );  //     *(BYTE *)0x55FA36 = 0x36;
    MemCpy ( (LPVOID)0x55FA5E, &originalMov, 3 );
    MemPut < BYTE > ( 0x55FA61, 0x38 );  //     *(BYTE *)0x55FA61 = 0x38;
    MemCpy ( (LPVOID)0x55FA8D, &originalMov, 3 );
    MemPut < BYTE > ( 0x55FA90, 0x3A );  //     *(BYTE *)0x55FA90 = 0x3A;
}

float CWorldSA::GetSunSize ( )
{
    return *(float *)(VAR_fSunSize) / 10;
}

void CWorldSA::SetSunSize ( float fSize )
{
    MemPut < BYTE > ( 0x55FA9D, 0xDD );  //     *(BYTE *)0x55FA9D = 0xDD;
    MemPut < BYTE > ( 0x55FA9E, 0xD8 );  //     *(BYTE *)0x55FA9E = 0xD8;
    MemPut < BYTE > ( 0x55FA9F, 0x90 );  //     *(BYTE *)0x55FA9F = 0x90;

    MemPut < float > ( VAR_fSunSize, fSize * 10 );  //     *(float *)VAR_fSunSize = fSize * 10;
}

void CWorldSA::ResetSunSize ( )
{
    MemPut < BYTE > ( 0x55FA9D, 0xD9 );  //     *(BYTE *)0x55FA9D = 0xD9;
    MemPut < BYTE > ( 0x55FA9E, 0x5E );  //     *(BYTE *)0x55FA9E = 0x5E;
    MemPut < BYTE > ( 0x55FA9F, 0x3C );  //     *(BYTE *)0x55FA9F = 0x3C;
}
