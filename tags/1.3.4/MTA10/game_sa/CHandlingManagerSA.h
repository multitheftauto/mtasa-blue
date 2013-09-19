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

class CHandlingManagerSA: public CHandlingManager
{
public:
                                CHandlingManagerSA              ( void );
                                ~CHandlingManagerSA             ( void );

    CHandlingEntry*             CreateHandlingData              ( void );

    const CHandlingEntry*       GetOriginalHandlingData         ( eVehicleTypes eModel );

    eHandlingTypes              GetHandlingID                   ( eVehicleTypes eModel );

    eHandlingProperty           GetPropertyEnumFromName         ( std::string strName );

    void                        CheckSuspensionChanges          ( CHandlingEntry* pEntry );
    void                        RemoveChangedVehicle            ( void );

private:
    void                        InitializeDefaultHandlings      ( void );

    static DWORD                m_dwStore_LoadHandlingCfg;

    // Original handling data unaffected by handling.cfg changes
    static tHandlingDataSA      m_OriginalHandlingData [HT_MAX];
    static CHandlingEntrySA*    m_pOriginalEntries [HT_MAX];

    std::map < std::string, eHandlingProperty > m_HandlingNames;
    int iChangedVehicles;
};

#endif
