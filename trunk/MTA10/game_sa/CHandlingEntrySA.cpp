/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CHandlingEntrySA.cpp
*  PURPOSE:		Vehicle handling data entry
*  DEVELOPERS:	Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CHandlingEntrySA::CHandlingEntrySA ( void )
{
    /*
    // Create a new interface and zero it
    m_pHandlingSA = new tHandlingDataSA;
    memset ( m_pHandlingSA, 0, sizeof ( tHandlingDataSA ) );
    m_bDeleteInterface = true;

    // We have no original data
    m_pOriginalData = NULL;
    */
}


CHandlingEntrySA::CHandlingEntrySA ( tHandlingDataSA* pDataSA, tHandlingDataSA* pOriginalUncalculatedData )
{
    /*
    // Store gta's pointer
    m_pHandlingSA = pDataSA;
    m_bDeleteInterface = false;
    m_pOriginalData = pOriginalUncalculatedData;
    */
}


CHandlingEntrySA::CHandlingEntrySA ( tHandlingDataSA* pOriginal )
{
    /*
    // Store gta's pointer
    m_pHandlingSA = NULL;
    m_pOriginalData = NULL;
    m_bDeleteInterface = false;
    memcpy ( &m_Handling, pOriginal, sizeof ( tHandlingDataSA ) );
    */
}


CHandlingEntrySA::~CHandlingEntrySA ( void )
{
    /*
    if ( m_bDeleteInterface )
    {
        delete m_pHandlingSA;
    }
    */
}


void CHandlingEntrySA::ApplyHandlingData ( CHandlingEntry* pData )
{
    /*
    // Copy the data
    CHandlingEntrySA* pEntrySA = static_cast < CHandlingEntrySA* > ( pData );
    m_Handling = pEntrySA->m_Handling;
    */
}


void CHandlingEntrySA::Recalculate ( void )
{
    /*
    // Real GTA class?
    if ( m_pHandlingSA )
    {
        // Copy our stored field to GTA's
        memcpy ( m_pHandlingSA, &m_Handling, sizeof ( m_Handling ) );

        // Call GTA's function that calculates the final values from the read values
        DWORD dwFunc = 0x6F5080;
        DWORD dwHandling = reinterpret_cast < DWORD > ( m_pHandlingSA );
        _asm
        {
            push        dwHandling
            call        dwFunc
        }
    }
    */
}


void CHandlingEntrySA::Restore ( void )
{
    /*
    // Real GTA class?
    if ( m_pOriginalData )
    {
        // Copy default stuff over gta's data
        memcpy ( &m_Handling, m_pOriginalData, sizeof ( tHandlingDataSA ) );

        // Recalculate the fields
        Recalculate ();
    }
    */
}
