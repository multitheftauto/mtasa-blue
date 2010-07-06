/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientHandlingManager.h
*  PURPOSE:     Vehicle handling entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

class CClientHandlingManager;

#ifndef __CCLIENTHANDLINGMANAGER_H
#define __CCLIENTHANDLINGMANAGER_H

#include "CClientEntity.h"
#include <list>
#include <vector>

class CClientManager;
class CClientHandling;
enum eVehicleTypes;

class CClientHandlingManager
{
    friend CClientManager;
    friend CClientHandling;

public:
    // Manage/handle handling list
    inline size_t                   Count                           ( void )                { return m_List.size (); };
    static CClientHandling*         Get                             ( ElementID ID );

    void                            DeleteAll                   ( void );

    // Default handling. This allows adding CClientHandling classes as the default handling class for
    // a vehicle model.
    void                            AddDefaultHandling              ( eVehicleTypes ID, CClientHandling* pHandling );
    void                            GetDefaultHandlings             ( eVehicleTypes ID, std::vector < CClientHandling* >& List );
    void                            UpdateDefaultHandlings          ( eVehicleTypes ID );

    // Use the following functions to get the value that will be used in first->last priority
    float                           GetMass                         ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetTurnMass                     ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetDragCoeff                    ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    const CVector&                  GetCenterOfMass                 ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );

    unsigned int                    GetPercentSubmerged             ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetTractionMultiplier           ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );

    CHandlingEntry::eDriveType      GetDriveType                    ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    CHandlingEntry::eEngineType     GetEngineType                   ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    unsigned char                   GetNumberOfGears                ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );

    float                           GetEngineAccelleration          ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetEngineInertia                ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetMaxVelocity                  ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );

    float                           GetBrakeDecelleration           ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetBrakeBias                    ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    bool                            GetABS                          ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );

    float                           GetSteeringLock                 ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetTractionLoss                 ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetTractionBias                 ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );

    float                           GetSuspensionForceLevel         ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetSuspensionDamping            ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetSuspensionHighSpeedDamping   ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetSuspensionUpperLimit         ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetSuspensionLowerLimit         ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetSuspensionFrontRearBias      ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetSuspensionAntidiveMultiplier ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );

    float                           GetCollisionDamageMultiplier    ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );

    unsigned int                    GetHandlingFlags                ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    unsigned int                    GetModelFlags                   ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    float                           GetSeatOffsetDistance           ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );

    CHandlingEntry::eLightType      GetHeadLight                    ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    CHandlingEntry::eLightType      GetTailLight                    ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );
    unsigned char                   GetAnimGroup                    ( const std::vector < CClientHandling* >& List, eVehicleTypes DefaultValueID );

private:
                                    CClientHandlingManager      ( class CClientManager* pManager );
                                    ~CClientHandlingManager     ( void );

    inline void                     AddToList                   ( CClientHandling* pHandling )   { m_List.push_back ( pHandling ); };
    void                            RemoveFromList              ( CClientHandling* pHandling );

    class CClientManager*           m_pManager;
    bool                            m_bCanRemoveFromList;
    std::list < CClientHandling* >  m_List;
};

#endif
