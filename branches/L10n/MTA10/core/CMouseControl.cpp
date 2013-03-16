/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMouseControl.cpp
*  PURPOSE:     Simulates the 'Fly with mouse' and 'Steer with mouse' controls
*  DEVELOPERS:  Talidan
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CMouseControl.h"
#include <game/CGame.h>

#define MOUSE_CONTROL_MULTIPLIER    35

extern CCore* g_pCore;


////////////////////////////////////////////////////
//
//  CMouseControl::CMouseControl
//
//
//
////////////////////////////////////////////////////
CMouseControl::CMouseControl ( void )
{
    m_usLeftStickX = 0;
    m_usLeftStickY = 0;
}


////////////////////////////////////////////////////
//
//  CMouseControl::~CMouseControl
//
//
//
////////////////////////////////////////////////////
CMouseControl::~CMouseControl ( void )
{
}


////////////////////////////////////////////////////
//
//  CMouseControl::ProcessMouseMove
//
//  Process a windows mouse movement message and turn it into control
//
////////////////////////////////////////////////////
bool CMouseControl::ProcessMouseMove ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if ( uMsg != WM_MOUSEMOVE )
        return false;

    if ( g_pCore->GetGame ()->GetSystemState () != 9 )
        return false;

    // HACK:  Grab our local player, and check his vehicle
    CPed* pPed = g_pCore->GetGame ()->GetPools ()->GetPedFromRef ( (DWORD)1 );
    if ( !pPed )
        return false;

    CVehicle* pVehicle = pPed->GetVehicle ();
    if ( !pVehicle )
        return false;

    CModelInfo* pModelInfo = g_pCore->GetGame ()->GetModelInfo( pVehicle->GetModelIndex() );

    bool bVar;
    CVARS_GET ( "fly_with_mouse", bVar );
    if ( pModelInfo->IsPlane() || pModelInfo->IsHeli() && !bVar ) // Are we in a plane, but not have mouse flight enabled?
        return false;

    CVARS_GET ( "steer_with_mouse", bVar );
    if ( !bVar )  // Are we in another type of vehicle, but not have mouse steering enabled?
        return false;


    // Let's calculate our mouse movement directions
    CVector2D resolution = g_pCore->GetGUI()->GetResolution();
    int iX = LOWORD ( lParam ), iY = HIWORD ( lParam );
    float fX = (iX - resolution.fX*0.5f)/resolution.fX;

    fX *= MOUSE_CONTROL_MULTIPLIER;

    float fMouseSensitivity = g_pCore->GetGame ( )->GetSettings()->GetMouseSensitivity ();
    fX *= fMouseSensitivity;

    m_usLeftStickX += fX*128;
    m_usLeftStickX  = Clamp < const short > ( -128, m_usLeftStickX, 128 );

    // Only process Y input if we're in a vehicle that requires it
    if ( pModelInfo->IsPlane() || pModelInfo->IsHeli() || pModelInfo->IsBike() || pModelInfo->IsBmx() || pModelInfo->IsQuadBike() )
    {
        float fY = (resolution.fY*0.5f - iY)/resolution.fY;
        fY *= MOUSE_CONTROL_MULTIPLIER;
        fY *= fMouseSensitivity;

        CVARS_GET ( "invert_mouse", bVar );
        fY = bVar ? -fY : fY;

        m_usLeftStickY += fY*128;
        m_usLeftStickY  = Clamp < const short > ( -128, m_usLeftStickY, 128 );
    }

    return true;
}


////////////////////////////////////////////////////
//
//  CMouseControl::Reset
//
// 
//
////////////////////////////////////////////////////
void CMouseControl::Reset ( void )
{
    m_usLeftStickX = 0;
    m_usLeftStickY = 0;
}


///////////////////////////////////////////////////////////////
//
// CMouseControl::ApplyAxes
//
// Map physical axes into CControllerState
//
///////////////////////////////////////////////////////////////
void CMouseControl::ApplyAxes ( CControllerState& cs )
{
    // If input was obtained from elsewhere, we reset
    if ( cs.LeftStickX || cs.LeftStickY )
    {
        Reset();
        return;
    }

    cs.LeftStickX = m_usLeftStickX;
    cs.LeftStickY = m_usLeftStickY;
}