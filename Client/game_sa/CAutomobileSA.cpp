/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAutomobileSA.cpp
*  PURPOSE:     Automobile vehicle entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

/**
 * \todo Spawn automobiles with engine off
 */
CAutomobileSA::CAutomobileSA( eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2 ):CVehicleSA( dwModelID, ucVariation, ucVariation2 )
{   
    DEBUG_TRACE("CAutomobileSA::CAutomobileSA( eVehicleTypes dwModelID ):CVehicleSA( dwModelID )");
/*  if(this->internalInterface)
    {
        // create the actual vehicle
        DWORD dwFunc = FUNC_CAutomobileContructor;
        DWORD dwThis = (DWORD)this->internalInterface;
        _asm
        {
            mov     ecx, dwThis
            push    MISSION_VEHICLE
            push    dwModelID
            call    dwFunc
        }

        this->SetEntityStatus(STATUS_ABANDONED); // so it actually shows up in the world

        pGame->GetWorld()->Add((CEntitySA *)this);
*/
        // create our mirror classes
        for(int i =0;i<MAX_DOORS;i++)
            this->door[i] = new CDoorSA(&((CAutomobileSAInterface *)this->GetInterface())->doors[i]);
    /*}
    else
        _asm int 3*/
}


CAutomobileSA::CAutomobileSA( CAutomobileSAInterface * automobile )
{
    DEBUG_TRACE("CAutomobileSA::CAutomobileSA( CAutomobileSAInterface * automobile )");
    // just so it can be passed on to CVehicle
}

CAutomobileSA::~CAutomobileSA( void )
{
    for(int i =0;i<MAX_DOORS;i++)
    {
        if ( this->door[i] ) delete this->door[i];
    }
}

bool CAutomobileSA::BurstTyre ( DWORD dwTyreID )
{
    DEBUG_TRACE("bool CAutomobileSA::BurstTyre ( DWORD dwTyreID )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_BurstTyre;
    bool bReturn;

    _asm
    {
        mov     ecx, dwThis
        push    dwTyreID
        call    dwFunc;
        mov     bReturn, al
    }

    return bReturn;
}


bool CAutomobileSA::BreakTowLink ( void )
{
    DEBUG_TRACE("bool CAutomobileSA::BreakTowLink ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_BreakTowLink;
    bool bReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


void CAutomobileSA::BlowUpCar ( CEntity* pEntity )
{
    DEBUG_TRACE("void CAutomobileSA::BlowUpCar ( CEntity* pEntity )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_BlowUpCar;

    _asm
    {
        mov     ecx, dwThis
        push    pEntity
        call    dwFunc
    }
}


void CAutomobileSA::BlowUpCarsInPath ( void )
{
    DEBUG_TRACE("void CAutomobileSA::BlowUpCarsInPath ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_BlowUpCarsInPath;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CAutomobileSA::CloseAllDoors ( void )
{
    DEBUG_TRACE("void CAutomobileSA::CloseAllDoors ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_CloseAllDoors;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CAutomobileSA::CloseBoot ( void )
{
    DEBUG_TRACE("void CAutomobileSA::CloseBoot ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_CloseBoot;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

float CAutomobileSA::FindWheelWidth ( bool bUnknown )
{
    DEBUG_TRACE("float CAutomobileSA::FindWheelWidth ( bool bUnknown )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwUnknown = (DWORD) bUnknown;
    DWORD dwFunc = FUNC_CAutomobile_FindWheelWidth;
    float fReturn;

    _asm
    {
        mov     ecx, dwThis
        push    dwUnknown
        call    dwFunc
        fstp    fReturn;
    }

    return fReturn;
}

/*
void CAutomobileSA::Fix ( void )
{
    DEBUG_TRACE("void CAutomobileSA::Fix ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_Fix;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}*/


void CAutomobileSA::FixDoor ( int iCarNodeIndex, eDoorsSA Door )
{
    DEBUG_TRACE("void CAutomobileSA::FixDoor ( int iCarNodeIndex, eDoorsSA Door )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_FixDoor;

    _asm
    {
        mov     ecx, dwThis
        push    Door
        push    iCarNodeIndex
        call    dwFunc
    }
}


int CAutomobileSA::FixPanel ( int iCarNodeIndex, ePanelsSA Panel )
{
    DEBUG_TRACE("int CAutomobileSA::FixPanel ( int iCarNodeIndex, ePanelsSA Panel )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_FixPanel;
    int iReturn;

    _asm
    {
        mov     ecx, dwThis
        push    Panel
        push    iCarNodeIndex
        call    dwFunc
        mov     iReturn, eax
    }
    
    return iReturn;
}


bool CAutomobileSA::GetAllWheelsOffGround ( void )
{
    DEBUG_TRACE("bool CAutomobileSA::GetAllWheelsOffGround ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_GetAllWheelsOffGround;
    bool bReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


float CAutomobileSA::GetCarPitch ( void )
{
    DEBUG_TRACE("float CAutomobileSA::GetCarPitch ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_GetCarPitch;
    float fReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fReturn
    }

    return fReturn;
}


float CAutomobileSA::GetCarRoll ( void )
{
    DEBUG_TRACE("float CAutomobileSA::GetCarRoll ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_GetCarRoll;
    float fReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fReturn
    }

    return fReturn;
}


void CAutomobileSA::GetComponentWorldPosition ( int iComponentID, CVector* pVector)
{
    DEBUG_TRACE("void CAutomobileSA::GetComponentWorldPosition ( int iComponentID, CVector* pVector)");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_GetComponentWorldPosition;

    _asm
    {
        mov     ecx, dwThis
        push    pVector
        push    iComponentID
        call    dwFunc
    }
}


/*float CAutomobileSA::GetHeightAboveRoad ( void )
{

}*/


DWORD CAutomobileSA::GetNumContactWheels ( void )
{
    DEBUG_TRACE("DWORD CAutomobileSA::GetNumContactWheels ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_GetNumContactWheels;
    DWORD dwReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwReturn, eax
    }

    return dwReturn;
}


float CAutomobileSA::GetRearHeightAboveRoad ( void )
{
    DEBUG_TRACE("float CAutomobileSA::GetRearHeightAboveRoad ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_GetRearHeightAboveRoad;
    float fReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        fstp    fReturn
    }

    return fReturn;
}

bool CAutomobileSA::IsComponentPresent ( int iComponentID )
{
    DEBUG_TRACE("bool CAutomobileSA::IsComponentPresent ( int iComponentID )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_IsComponentPresent;
    bool bReturn;

    _asm
    {
        mov     ecx, dwThis
        push    iComponentID
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


bool CAutomobileSA::IsDoorClosed ( eDoorsSA Door )
{
    DEBUG_TRACE("bool CAutomobileSA::IsDoorClosed ( eDoorsSA Door )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_IsDoorClosed;
    bool bReturn;

    _asm
    {
        mov     ecx, dwThis
        push    Door
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


bool CAutomobileSA::IsDoorFullyOpen ( eDoorsSA Door )
{
    DEBUG_TRACE("bool CAutomobileSA::IsDoorFullyOpen ( eDoorsSA Door )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_IsDoorFullyOpen;
    bool bReturn;

    _asm
    {
        mov     ecx, dwThis
        push    Door
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


bool CAutomobileSA::IsDoorMissing ( eDoorsSA Door )
{
    DEBUG_TRACE("bool CAutomobileSA::IsDoorMissing ( eDoorsSA Door )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_IsDoorMissing;
    bool bReturn;

    _asm
    {
        mov     ecx, dwThis
        push    Door
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


bool CAutomobileSA::IsDoorReady ( eDoorsSA Door )
{
    DEBUG_TRACE("bool CAutomobileSA::IsDoorReady ( eDoorsSA Door )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_IsDoorReady;
    bool bReturn;

    _asm
    {
        mov     ecx, dwThis
        push    Door
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


bool CAutomobileSA::IsInAir ( void )
{
    DEBUG_TRACE("bool CAutomobileSA::IsInAir ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_IsInAir;
    bool bReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


bool CAutomobileSA::IsOpenTopCar ( void )
{
    DEBUG_TRACE("bool CAutomobileSA::IsOpenTopCar ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_IsOpenTopCar;
    bool bReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}


void CAutomobileSA::PlayCarHorn ( void )
{
    DEBUG_TRACE("void CAutomobileSA::PlayCarHorn ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_PlayCarHorn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CAutomobileSA::PopBoot ( void )
{
    DEBUG_TRACE("void CAutomobileSA::PopBoot ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_PopBoot;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CAutomobileSA::PopBootUsingPhysics ( void )
{
    DEBUG_TRACE("void CAutomobileSA::PopBootUsingPhysics ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_PopBootUsingPhysics;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CAutomobileSA::PopDoor ( int iCarNodeIndex, eDoorsSA Door, bool bUnknown )
{
    DEBUG_TRACE("void CAutomobileSA::PopDoor ( int iCarNodeIndex, eDoorsSA Door, bool bUnknown )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwUnknown = (DWORD) bUnknown;
    DWORD dwFunc = FUNC_CAutomobile_PopDoor;

    _asm
    {
        mov     ecx, dwThis
        push    dwUnknown
        push    Door
        push    iCarNodeIndex
        call    dwFunc
    }
}


void CAutomobileSA::PopPanel ( int iCarNodeIndex, ePanelsSA Panel, bool bFallOffFast )
{
    DEBUG_TRACE("void CAutomobileSA::PopPanel ( int iCarNodeIndex, ePanelsSA Panel, bool bFallOffFast )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFallOffFast = (DWORD) bFallOffFast;
    DWORD dwFunc = FUNC_CAutomobile_PopPanel;

    _asm
    {
        mov     ecx, dwThis
        push    dwFallOffFast
        push    Panel
        push    iCarNodeIndex
        call    dwFunc
    }
}


void CAutomobileSA::ResetSuspension ( void )
{
    DEBUG_TRACE("void CAutomobileSA::ResetSuspension ( void )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_ResetSuspension;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}


void CAutomobileSA::SetRandomDamage ( bool bUnknown )
{
    DEBUG_TRACE("void CAutomobileSA::SetRandomDamage ( bool bUnknown )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwUnknown = (DWORD) bUnknown;
    DWORD dwFunc = FUNC_CAutomobile_SetRandomDamage;

    _asm
    {
        mov     ecx, dwThis
        push    dwUnknown
        call    dwFunc
    }
}


void CAutomobileSA::SetTaxiLight ( bool bState )
{
    DEBUG_TRACE("void CAutomobileSA::SetTaxiLight ( bool bState )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwState = (DWORD) bState;
    DWORD dwFunc = FUNC_CAutomobile_SetTaxiLight;

    _asm
    {
        mov     ecx, dwThis
        push    dwState
        call    dwFunc
    }
}


void CAutomobileSA::SetTotalDamage (bool bUnknown )
{
    DEBUG_TRACE("void CAutomobileSA::SetTotalDamage (bool bUnknown )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwUnknown = (DWORD) bUnknown;
    DWORD dwFunc = FUNC_CAutomobile_SetTotalDamage;

    _asm
    {
        mov     ecx, dwThis
        push    dwUnknown
        call    dwFunc
    }
}

CPhysical* CAutomobileSA::SpawnFlyingComponent ( int iCarNodeIndex, int iUnknown )
{
    DEBUG_TRACE("CPhysical* CAutomobileSA::SpawnFlyingComponent ( int iCarNodeIndex, int iUnknown )");
    DWORD dwThis = (DWORD) GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_SpawnFlyingComponent;
    CPhysical* pReturn;

    _asm
    {
        mov     ecx, dwThis
        push    iUnknown
        push    iCarNodeIndex
        call    dwFunc
        mov     pReturn, eax
    }

    return pReturn;
}

CDoor * CAutomobileSA::GetDoor(eDoors doorID)
{
    DEBUG_TRACE("CDoor * CAutomobileSA::GetDoor(eDoors doorID)");
    return this->door[doorID];
}
