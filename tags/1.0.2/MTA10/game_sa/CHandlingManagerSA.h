/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CHandlingManagerSA.h
*  PURPOSE:		Header file for vehicle handling manager class
*  DEVELOPERS:	Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CHANDLINGMANAGER
#define __CGAME_CHANDLINGMANAGER

#include <game/CHandlingManager.h>
#include "CHandlingEntrySA.h"

class CHandlingManagerSA: public IHandlingManager
{
public:
                                CHandlingManagerSA              ( void );
                                ~CHandlingManagerSA             ( void );

    void                        LoadDefaultHandlings            ( void );

    CHandlingEntry*             CreateHandlingData              ( void );
    bool                        ApplyHandlingData               ( enum eVehicleTypes eModel, CHandlingEntry* pEntry );

    CHandlingEntry*             GetHandlingData                 ( eVehicleTypes eModel );
    const CHandlingEntry*       GetOriginalHandlingData         ( eVehicleTypes eModel );

private:
    void                        InitializeDefaultHandlings      ( void );

    static DWORD                m_dwStore_LoadHandlingCfg;

    static void                 LoadHandlingCfg                 ( void );
    static void                 Hook_LoadHandlingCfg            ( void );

    // Original handling data unaffected by handling.cfg changes
    static tHandlingDataSA      m_OriginalHandlingData [212];

    // Our wrapper classes for the classes GTA use and the original data
    static CHandlingEntrySA*    m_pEntries [212];
    static CHandlingEntrySA*    m_pOriginalEntries [212];

    // These are the entries GTA use
    static tHandlingDataSA      m_RealHandlingData [212];
};

#endif
