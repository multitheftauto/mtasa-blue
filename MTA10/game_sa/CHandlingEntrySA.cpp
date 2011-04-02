/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CHandlingEntrySA.cpp
*  PURPOSE:     Vehicle handling data entry
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               The_GTA <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define FUNC_HandlingDataMgr_ConvertDataToGameUnits 0x6F5080

CHandlingEntrySA::CHandlingEntrySA ( void )
{
    // Create a new interface and zero it
    m_pHandlingSA = new tHandlingDataSA;
    MemSetFast ( m_pHandlingSA, 0, sizeof ( tHandlingDataSA ) );
    m_bDeleteInterface = true;

    // We have no original data
    m_pOriginalData = NULL;
}


CHandlingEntrySA::CHandlingEntrySA ( tHandlingDataSA* pDataSA, tHandlingDataSA* pOriginalUncalculatedData )
{
    // Store gta's pointer
    m_pHandlingSA = pDataSA;
    m_bDeleteInterface = false;
    m_pOriginalData = pOriginalUncalculatedData;
}


CHandlingEntrySA::CHandlingEntrySA ( tHandlingDataSA* pOriginal )
{
    // Store gta's pointer
    m_pHandlingSA = NULL;
    m_pOriginalData = NULL;
    m_bDeleteInterface = false;
    MemCpyFast ( &m_Handling, pOriginal, sizeof ( tHandlingDataSA ) );
}


CHandlingEntrySA::~CHandlingEntrySA ( void )
{
    if ( m_bDeleteInterface )
    {
        delete m_pHandlingSA;
    }
}

// Does vehicle entry already exist?
bool CHandlingEntrySA::IsVehicleAdded ( CVehicle* pVeh )
{
//  std::list < CVehicleSA* > ::iterator iter = m_VehicleList.begin ();
//  for ( ; iter != m_VehicleList.begin (); iter++ )
//  {
//      if (*iter == pVeh)
//          return true;
//  }
    return false;
}

// Adds a vehicle to list
void CHandlingEntrySA::AddVehicle ( CVehicle* pVeh )
{
//  if ( IsVehicleAdded ( pVeh ) )
//      return;
//  m_VehicleList.push_front ( dynamic_cast < CVehicleSA* > ( pVeh ) );
}


// Apply the handlingdata from another data
void CHandlingEntrySA::Assign ( const CHandlingEntry* pData )
{
    // Copy the data
    const CHandlingEntrySA* pEntrySA = static_cast < const CHandlingEntrySA* > ( pData );
    m_Handling = pEntrySA->m_Handling;
}


// Remove a vehicle from list
void CHandlingEntrySA::RemoveVehicle ( CVehicle* pVeh )
{
//  m_VehicleList.remove ( dynamic_cast < CVehicleSA* > ( pVeh ) );
}


void CHandlingEntrySA::Recalculate ( void )
{
    // Real GTA class?
    if ( m_pHandlingSA )
    {
        // Copy our stored field to GTA's
        MemCpyFast ( m_pHandlingSA, &m_Handling, sizeof ( m_Handling ) );

        // Call GTA's function that calculates the final values from the read values
        ( (void (_stdcall *)(tHandlingDataSA*))FUNC_HandlingDataMgr_ConvertDataToGameUnits )( m_pHandlingSA );
    }
}


void CHandlingEntrySA::Restore ( void )
{
    // Real GTA class?
    if ( m_pOriginalData )
    {
        // Copy default stuff over gta's data
        MemCpyFast ( &m_Handling, m_pOriginalData, sizeof ( tHandlingDataSA ) );

        // Recalculate the fields
        Recalculate ();
    }
}
