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

    CHandlingEntry*             CreateHandlingData              ( void );

    const CHandlingEntry*       GetOriginalHandlingData         ( eVehicleTypes eModel );
    float                       GetDragMultiplier               ( void );
    float                       GetBasicDragCoeff               ( void );
    eHandlingTypes              GetHandlingID                   ( eVehicleTypes eModel );

private:
    void                        InitializeDefaultHandlings      ( void );

    static DWORD                m_dwStore_LoadHandlingCfg;

    // Original handling data unaffected by handling.cfg changes
    static tHandlingDataSA      m_OriginalHandlingData [HT_MAX];
    static CHandlingEntrySA*    m_pOriginalEntries [HT_MAX];

    // Additional entries are saved here
    std::list < CHandlingEntrySA* > m_HandlingList;
};

#endif
