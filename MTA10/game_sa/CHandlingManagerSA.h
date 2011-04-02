/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CHandlingManagerSA.h
*  PURPOSE:     Header file for vehicle handling manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
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
    void                        RemoveFromVeh                   ( CVehicle* pVeh );

    CHandlingEntry*             GetHandlingData                 ( eVehicleTypes eModel );
    const CHandlingEntry*       GetOriginalHandlingData         ( eVehicleTypes eModel );
    CHandlingEntry*             GetOriginalHandlingTable        ( eHandlingTypes eHandling );
    CHandlingEntry*             GetPreviousHandlingTable        ( eHandlingTypes eHandling );
    float                       GetDragMultiplier               ( void );
    float                       GetBasicDragCoeff               ( void );
    eHandlingTypes              GetHandlingID                   ( eVehicleTypes eModel );

private:
    void                        InitializeDefaultHandlings      ( void );

    static DWORD                m_dwStore_LoadHandlingCfg;

    static void                 LoadHandlingCfg                 ( void );
    static void                 Hook_LoadHandlingCfg            ( void );

    // Original handling data unaffected by handling.cfg changes
    static tHandlingDataSA      m_OriginalHandlingData [HT_MAX];

    // Our wrapper classes for the classes GTA use and the original data
    static CHandlingEntrySA*    m_pEntries [HT_MAX];
    static CHandlingEntrySA*    m_pOriginalEntries [HT_MAX];

    // These are the entries GTA use
    static tHandlingDataSA      m_RealHandlingData [HT_MAX];

    // Additional entries are saved here
    std::list < CHandlingEntrySA* > m_HandlingList;
};

#endif
