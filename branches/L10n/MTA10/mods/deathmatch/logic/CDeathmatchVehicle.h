/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDeathmatchVehicle.h
*  PURPOSE:     Header for deathmatch vehicle class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDEATHMATCHVEHICLE_H
#define __CDEATHMATCHVEHICLE_H

#include "CClientVehicle.h"

class CDeathmatchVehicle : public CClientVehicle
{
    DECLARE_CLASS( CDeathmatchVehicle, CClientVehicle )
public:
                                    CDeathmatchVehicle              ( CClientManager* pManager, class CUnoccupiedVehicleSync* pUnoccupiedVehicleSync, ElementID ID, unsigned short usVehicleModel, unsigned char ucVariant, unsigned char ucVariant2 );
                                    ~CDeathmatchVehicle             ( void );

    inline bool                     IsSyncing                       ( void )                    { return m_bIsSyncing; };
    void                            SetIsSyncing                    ( bool bIsSyncing );

    bool                            SyncDamageModel                 ( void );
    void                            ResetDamageModelSync            ( void );

private:
    class CUnoccupiedVehicleSync*   m_pUnoccupiedVehicleSync;
    bool                            m_bIsSyncing;

    SFixedArray < unsigned char, MAX_DOORS >    m_ucLastDoorStates;
    SFixedArray < unsigned char, MAX_WHEELS >   m_ucLastWheelStates;
    SFixedArray < unsigned char, MAX_PANELS >   m_ucLastPanelStates;
    SFixedArray < unsigned char, MAX_LIGHTS >   m_ucLastLightStates;
};

#endif
