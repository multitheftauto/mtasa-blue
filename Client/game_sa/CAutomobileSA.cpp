/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAutomobileSA.cpp
 *  PURPOSE:     Automobile vehicle entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

/**
 * \todo Spawn automobiles with engine off
 */
CAutomobileSA::CAutomobileSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2) : CVehicleSA(dwModelID, ucVariation, ucVariation2)
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
    for (int i = 0; i < MAX_DOORS; i++)
        this->door[i] = new CDoorSA(&((CAutomobileSAInterface*)this->GetInterface())->m_doors[i]);
    /*}
     */
}

CAutomobileSA::CAutomobileSA(CAutomobileSAInterface* automobile)
{
    DEBUG_TRACE("CAutomobileSA::CAutomobileSA( CAutomobileSAInterface * automobile )");
    // just so it can be passed on to CVehicle
}

CAutomobileSA::~CAutomobileSA()
{
    for (int i = 0; i < MAX_DOORS; i++)
    {
        if (this->door[i])
            delete this->door[i];
    }
}

bool CAutomobileSA::BurstTyre(DWORD dwTyreID)
{
    DEBUG_TRACE("bool CAutomobileSA::BurstTyre ( DWORD dwTyreID )");

    // CAutomobile::BurstTyre
    return ((bool(__thiscall*)(CAutomobileSAInterface*, unsigned char, bool))FUNC_CAutomobile_BurstTyre)(GetAutomobileInterface(), dwTyreID, true);
}

bool CAutomobileSA::BreakTowLink()
{
    DEBUG_TRACE("bool CAutomobileSA::BreakTowLink ( void )");

    // CAutomobile::BreakTowLink
    return ((bool(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_BreakTowLink)(GetAutomobileInterface());
}

void CAutomobileSA::BlowUpCar(CEntity* pEntity)
{
    DEBUG_TRACE("void CAutomobileSA::BlowUpCar ( CEntity* pEntity )");

    // ?
    // CAutomobile::BlowUpCar
    ((void(__thiscall*)(CAutomobileSAInterface*, CEntity*))FUNC_CAutomobile_BlowUpCar)(GetAutomobileInterface(), pEntity);
}

void CAutomobileSA::BlowUpCarsInPath()
{
    DEBUG_TRACE("void CAutomobileSA::BlowUpCarsInPath ( void )");

    // CAutomobile::BlowUpCarsInPath
    ((void(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_BlowUpCarsInPath)(GetAutomobileInterface());
}

void CAutomobileSA::CloseAllDoors()
{
    DEBUG_TRACE("void CAutomobileSA::CloseAllDoors ( void )");

    // CAutomobile::CloseAllDoors
    ((void(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_CloseAllDoors)(GetAutomobileInterface());
}

void CAutomobileSA::CloseBoot()
{
    DEBUG_TRACE("void CAutomobileSA::CloseBoot ( void )");

    // CAutomobile::CloseBoot
    ((void(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_CloseBoot)(GetAutomobileInterface());
}

float CAutomobileSA::FindWheelWidth(bool bRear)
{
    DEBUG_TRACE("float CAutomobileSA::FindWheelWidth ( bool bUnknown )");

    // CAutomobile::FindWheelWidth
    return ((float(__thiscall*)(CAutomobileSAInterface*, bool))FUNC_CAutomobile_FindWheelWidth)(GetAutomobileInterface(), bRear);
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

void CAutomobileSA::FixDoor(int iCarNodeIndex, eDoorsSA Door)
{
    DEBUG_TRACE("void CAutomobileSA::FixDoor ( int iCarNodeIndex, eDoorsSA Door )");

    // CAutomobile::FixDoor
    ((void(__thiscall*)(CAutomobileSAInterface*, int, eDoorsSA))FUNC_CAutomobile_FixDoor)(GetAutomobileInterface(), iCarNodeIndex, Door);
}

int CAutomobileSA::FixPanel(int iCarNodeIndex, ePanelsSA Panel)
{
    DEBUG_TRACE("int CAutomobileSA::FixPanel ( int iCarNodeIndex, ePanelsSA Panel )");
 
    // CAutomobile::FixDoor
    return ((int(__thiscall*)(CAutomobileSAInterface*, int, ePanelsSA))FUNC_CAutomobile_FixPanel)(GetAutomobileInterface(), iCarNodeIndex, Panel);
}

bool CAutomobileSA::GetAllWheelsOffGround()
{
    DEBUG_TRACE("bool CAutomobileSA::GetAllWheelsOffGround ( void )");

    // CAutomobile::GetAllWheelsOffGround
    return ((bool(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_GetAllWheelsOffGround)(GetAutomobileInterface());
}

float CAutomobileSA::GetCarPitch()
{
    DEBUG_TRACE("float CAutomobileSA::GetCarPitch ( void )");

    // CAutomobile::GetCarPitch
    return ((float(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_GetCarPitch)(GetAutomobileInterface());
}

float CAutomobileSA::GetCarRoll()
{
    DEBUG_TRACE("float CAutomobileSA::GetCarRoll ( void )");

    // CAutomobile::GetCarRoll
    return ((float(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_GetCarRoll)(GetAutomobileInterface());
}

void CAutomobileSA::GetComponentWorldPosition(int iComponentID, CVector* pVector)
{
    DEBUG_TRACE("void CAutomobileSA::GetComponentWorldPosition ( int iComponentID, CVector* pVector)");

    // CAutomobile::GetComponentWorldPosition
    ((void(__thiscall*)(CAutomobileSAInterface*, int, CVector&))FUNC_CAutomobile_GetComponentWorldPosition)(GetAutomobileInterface(), iComponentID, *pVector);
}

/*float CAutomobileSA::GetHeightAboveRoad ( void )
{

}*/

DWORD CAutomobileSA::GetNumContactWheels()
{
    DEBUG_TRACE("DWORD CAutomobileSA::GetNumContactWheels ( void )");

    // CAutomobile::GetNumContactWheels
    return ((DWORD(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_GetNumContactWheels)(GetAutomobileInterface());
}

float CAutomobileSA::GetRearHeightAboveRoad()
{
    DEBUG_TRACE("float CAutomobileSA::GetRearHeightAboveRoad ( void )");

    // CAutomobile::GetRearHeightAboveRoad
    return ((float(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_GetRearHeightAboveRoad)(GetAutomobileInterface());
}

bool CAutomobileSA::IsComponentPresent(int iComponentID)
{
    DEBUG_TRACE("bool CAutomobileSA::IsComponentPresent ( int iComponentID )");

    // CAutomobile::IsComponentPresent
    return ((bool(__thiscall*)(CAutomobileSAInterface*, int))FUNC_CAutomobile_IsComponentPresent)(GetAutomobileInterface(), iComponentID);
}

bool CAutomobileSA::IsDoorClosed(eDoorsSA Door)
{
    DEBUG_TRACE("bool CAutomobileSA::IsDoorClosed ( eDoorsSA Door )");

    // CAutomobile::IsDoorClosed
    return ((bool(__thiscall*)(CAutomobileSAInterface*, eDoorsSA))FUNC_CAutomobile_IsDoorClosed)(GetAutomobileInterface(), Door);
}

bool CAutomobileSA::IsDoorFullyOpen(eDoorsSA Door)
{
    DEBUG_TRACE("bool CAutomobileSA::IsDoorFullyOpen ( eDoorsSA Door )");

    // CAutomobile::IsDoorFullyOpen
    return ((bool(__thiscall*)(CAutomobileSAInterface*, eDoorsSA))FUNC_CAutomobile_IsDoorFullyOpen)(GetAutomobileInterface(), Door);
}

bool CAutomobileSA::IsDoorMissing(eDoorsSA Door)
{
    DEBUG_TRACE("bool CAutomobileSA::IsDoorMissing ( eDoorsSA Door )");

    // CAutomobile::IsDoorMissing
    return ((bool(__thiscall*)(CAutomobileSAInterface*, eDoorsSA))FUNC_CAutomobile_IsDoorMissing)(GetAutomobileInterface(), Door);
}

bool CAutomobileSA::IsDoorReady(eDoorsSA Door)
{
    DEBUG_TRACE("bool CAutomobileSA::IsDoorReady ( eDoorsSA Door )");

    // CAutomobile::IsDoorReady
    return ((bool(__thiscall*)(CAutomobileSAInterface*, eDoorsSA))FUNC_CAutomobile_IsDoorReady)(GetAutomobileInterface(), Door);
}

bool CAutomobileSA::IsInAir()
{
    DEBUG_TRACE("bool CAutomobileSA::IsInAir ( void )");

    // CAutomobile::IsInAir
    return ((bool(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_IsInAir)(GetAutomobileInterface());
}

bool CAutomobileSA::IsOpenTopCar()
{
    DEBUG_TRACE("bool CAutomobileSA::IsOpenTopCar ( void )");

    // CAutomobile::IsOpenTopCar
    return ((bool(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_IsOpenTopCar)(GetAutomobileInterface());
}

void CAutomobileSA::PlayCarHorn()
{
    DEBUG_TRACE("void CAutomobileSA::PlayCarHorn ( void )");

    // CAutomobile::PlayCarHorn
    ((void(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_PlayCarHorn)(GetAutomobileInterface());
}

void CAutomobileSA::PopBoot()
{
    DEBUG_TRACE("void CAutomobileSA::PopBoot ( void )");

    // CAutomobile::PopBoot
    ((void(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_PopBoot)(GetAutomobileInterface());
}

void CAutomobileSA::PopBootUsingPhysics()
{
    DEBUG_TRACE("void CAutomobileSA::PopBootUsingPhysics ( void )");

    // CAutomobile::PopBootUsingPhysics
    ((void(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_PopBootUsingPhysics)(GetAutomobileInterface());
}

void CAutomobileSA::PopDoor(int iCarNodeIndex, eDoorsSA Door, bool bUnknown)
{
    DEBUG_TRACE("void CAutomobileSA::PopDoor ( int iCarNodeIndex, eDoorsSA Door, bool bUnknown )");

    // CAutomobile::PopDoor
    ((void(__thiscall*)(CAutomobileSAInterface*, int, eDoorsSA, bool))FUNC_CAutomobile_PopDoor)(GetAutomobileInterface(), iCarNodeIndex, Door,
                                                                                                            bUnknown);
}

void CAutomobileSA::PopPanel(int iCarNodeIndex, ePanelsSA Panel, bool bFallOffFast)
{
    DEBUG_TRACE("void CAutomobileSA::PopPanel ( int iCarNodeIndex, ePanelsSA Panel, bool bFallOffFast )");
    DWORD dwThis = (DWORD)GetInterface();

    // CAutomobile::PopPanel
    ((void(__thiscall*)(CAutomobileSAInterface*, int, ePanelsSA, bool))FUNC_CAutomobile_PopPanel)(GetAutomobileInterface(), iCarNodeIndex, Panel,
                                                                                                             bFallOffFast);
}

void CAutomobileSA::ResetSuspension()
{
    DEBUG_TRACE("void CAutomobileSA::ResetSuspension ( void )");

    // CAutomobile::ResetSuspension
    ((void(__thiscall*)(CAutomobileSAInterface*))FUNC_CAutomobile_ResetSuspension)(GetAutomobileInterface());
}

void CAutomobileSA::SetRandomDamage(bool bUnknown)
{
    DEBUG_TRACE("void CAutomobileSA::SetRandomDamage ( bool bUnknown )");

    // CAutomobile::SetRandomDamage
    ((void(__thiscall*)(CAutomobileSAInterface*, bool))FUNC_CAutomobile_SetRandomDamage)(GetAutomobileInterface(), bUnknown);
}

void CAutomobileSA::SetTaxiLight(bool bState)
{
    DEBUG_TRACE("void CAutomobileSA::SetTaxiLight ( bool bState )");

    // CAutomobile::SetTaxiLight
    ((void(__thiscall*)(CAutomobileSAInterface*, bool))FUNC_CAutomobile_SetTaxiLight)(GetAutomobileInterface(), bState);
}

void CAutomobileSA::SetTotalDamage(bool bUnknown)
{
    DEBUG_TRACE("void CAutomobileSA::SetTotalDamage (bool bUnknown )");

    // CAutomobile::SetTotalDamage
    ((void(__thiscall*)(CAutomobileSAInterface*, bool))FUNC_CAutomobile_SetTotalDamage)(GetAutomobileInterface(), bUnknown);
}

CPhysical* CAutomobileSA::SpawnFlyingComponent(int iCarNodeIndex, int iUnknown)
{
    DEBUG_TRACE("CPhysical* CAutomobileSA::SpawnFlyingComponent ( int iCarNodeIndex, int iUnknown )");

    // ?
    // CAutomobile::SpawnFlyingComponent
    return ((CPhysical*(__thiscall*)(CAutomobileSAInterface*, int, int))FUNC_CAutomobile_SpawnFlyingComponent)(GetAutomobileInterface(), iCarNodeIndex, iUnknown);
}

CDoor* CAutomobileSA::GetDoor(eDoors doorID)
{
    DEBUG_TRACE("CDoor * CAutomobileSA::GetDoor(eDoors doorID)");
    return this->door[doorID];
}
