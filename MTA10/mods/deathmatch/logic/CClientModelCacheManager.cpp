/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "../../../core/CModelCacheManager.h"

#define PED_STREAM_IN_DISTANCE              (250)
#define VEHICLE_STREAM_IN_DISTANCE          (250)
#define STREAMER_STREAM_OUT_EXTRA_DISTANCE  (50)

#define PED_MAX_STREAM_DISTANCE             ( PED_STREAM_IN_DISTANCE + STREAMER_STREAM_OUT_EXTRA_DISTANCE )
#define PED_MAX_STREAM_DISTANCE_SQ          ( PED_MAX_STREAM_DISTANCE * PED_MAX_STREAM_DISTANCE )

#define VEHICLE_MAX_STREAM_DISTANCE         ( VEHICLE_STREAM_IN_DISTANCE + STREAMER_STREAM_OUT_EXTRA_DISTANCE )
#define VEHICLE_MAX_STREAM_DISTANCE_SQ      ( VEHICLE_MAX_STREAM_DISTANCE * VEHICLE_MAX_STREAM_DISTANCE )

#define PED_MAX_VELOCITY                    (10)
#define VEHICLE_MAX_VELOCITY                (10)


///////////////////////////////////////////////////////////////
//
//
// CClientModelCacheManager
//
//
///////////////////////////////////////////////////////////////
class CClientModelCacheManagerImpl : public CClientModelCacheManager
{
public:
    ZERO_ON_NEW

    // CClientModelCacheManager interface
    virtual void                DoPulse                             ( void );
    virtual void                OnRestreamModel                     ( ushort usModelId );

    // CClientModelCacheManagerImpl methods
                                CClientModelCacheManagerImpl        ( void );
                                ~CClientModelCacheManagerImpl       ( void );

    void                        DoPulsePedModels                    ( void );
    void                        DoPulseVehicleModels                ( void );
    void                        ProcessPlayerList                   ( std::map < ushort, float >& outNeedCacheList, const std::vector < CClientPlayer* >& playerList, float fMaxStreamDistanceSq );
    void                        ProcessPedList                      ( std::map < ushort, float >& outNeedCacheList, const std::vector < CClientPed* >& pedList, float fMaxStreamDistanceSq );
    void                        ProcessVehicleList                  ( std::map < ushort, float >& outNeedCacheList, const std::vector < CClientVehicle* >& vehicleList, float fMaxStreamDistanceSq );
    void                        InsertIntoNeedCacheList             ( std::map < ushort, float >& outNeedCacheList, ushort usModelId, float fDistSq );
    void                        ClearStats                          ( void );
    void                        AddProcessStat                      ( const char* szTag, bool bCache, ePuresyncType syncType, ushort usModelId, const CVector& vecStartPos, const CVector& vecEndPos );

protected:
    int                         m_iFrameCounter;
    CTickCount                  m_TickCountNow;
    CVector                     m_vecCameraPos;
    CTickCount                  m_LastTimeMs;
    float                       m_fSmoothCameraSpeed;
    CClientPlayer*              m_pLocalPlayer;
    float                       m_fGameFps;
    CModelCacheManager*         m_pCoreModelCacheManager;
};


///////////////////////////////////////////////////////////////
//
// Global new
//
//
///////////////////////////////////////////////////////////////
CClientModelCacheManager* NewClientModelCacheManager ()
{
    return new CClientModelCacheManagerImpl ();
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::CClientModelCacheManagerImpl
//
///////////////////////////////////////////////////////////////
CClientModelCacheManagerImpl::CClientModelCacheManagerImpl ( void )
{
    m_pCoreModelCacheManager = g_pCore->GetModelCacheManager ();
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::~CClientModelCacheManagerImpl
//
///////////////////////////////////////////////////////////////
CClientModelCacheManagerImpl::~CClientModelCacheManagerImpl ( void )
{
    m_pCoreModelCacheManager->OnClientClose ();
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::DoPulse
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::DoPulse ( void )
{
    m_TickCountNow = CTickCount::Now ();
    ClearStats ();

    CClientPlayer* m_pLocalPlayer = g_pClientGame->GetLocalPlayer ();
    if ( !m_pLocalPlayer )
        return;

    m_fGameFps = g_pGame->GetFPS ();


    // Assess which models will be needed in the next 2 seconds

    // Update frame time
    int iDeltaTimeMs = Clamp ( 10, ( m_TickCountNow - m_LastTimeMs ).ToInt (), 100 );
    m_LastTimeMs = m_TickCountNow;

    // Get camera position
    CVector vecOldCameraPos = m_vecCameraPos;
    CClientCamera* pCamera = g_pClientGame->GetManager ()->GetCamera ();
    if ( pCamera->IsInFixedMode () )        
        pCamera->GetPosition ( m_vecCameraPos );
    else
        g_pClientGame->GetLocalPlayer ()->GetPosition ( m_vecCameraPos );

    // Calculate camera velocity
    CVector vecDif = m_vecCameraPos - vecOldCameraPos;
    float fCameraSpeed = vecDif.Length () / ( iDeltaTimeMs * 0.001f );
    float a = m_fSmoothCameraSpeed;
    m_fSmoothCameraSpeed = Lerp ( m_fSmoothCameraSpeed, 0.25f, fCameraSpeed );

    // Spread updating over 4 frames
    m_iFrameCounter = ( m_iFrameCounter + 1 ) % 4;

    if ( m_iFrameCounter == 1 )
        DoPulsePedModels ();
    else
    if ( m_iFrameCounter == 3 )
        DoPulseVehicleModels ();
}

    
///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::DoPulsePedModels
//
// Pulse caching system for ped models
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::DoPulsePedModels ( void )
{
    // Scale up query radius to compensate for the camera speed and possible ped speeds
    float fPedQueryRadius = PED_STREAM_IN_DISTANCE + STREAMER_STREAM_OUT_EXTRA_DISTANCE + m_fSmoothCameraSpeed * 2 + PED_MAX_VELOCITY * 2;

    // Get all entities within range
    CClientEntityResult result;
    GetClientSpatialDatabase()->SphereQuery ( result, CSphere ( m_vecCameraPos, fPedQueryRadius ) );
 
    std::vector < CClientPed* > pedList;
    std::vector < CClientPlayer* > playerList;

    // For each entity found
    for ( CClientEntityResult::const_iterator iter = result.begin () ; iter != result.end (); ++iter )
    {
        switch ( (*iter)->GetType () )
        {
            case CCLIENTPED:
                pedList.push_back ( (CClientPed*)*iter );
                break;

            case CCLIENTPLAYER:
                playerList.push_back ( (CClientPlayer*)*iter );
                break;
        }
    }

    // Compile a list of ped models which should be cached
    std::map < ushort, float > newNeedCacheList;
    ProcessPlayerList ( newNeedCacheList, playerList, Square ( PED_STREAM_IN_DISTANCE + STREAMER_STREAM_OUT_EXTRA_DISTANCE + m_fSmoothCameraSpeed * 2 ) );
    ProcessPedList ( newNeedCacheList, pedList, Square ( PED_STREAM_IN_DISTANCE + STREAMER_STREAM_OUT_EXTRA_DISTANCE + m_fSmoothCameraSpeed * 2 ) );

    // Apply desired caching
    m_pCoreModelCacheManager->UpdatePedModelCaching ( newNeedCacheList );
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::DoPulseVehicleModels
//
// Pulse caching system for vehicle models
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::DoPulseVehicleModels ( void )
{
    // Scale up query radius to compensate for the camera speed and possible vehicle speeds
    float fVehicleQueryRadius = VEHICLE_STREAM_IN_DISTANCE + STREAMER_STREAM_OUT_EXTRA_DISTANCE + m_fSmoothCameraSpeed * 2 + VEHICLE_MAX_VELOCITY * 2;

    // Get all entities within range
    CClientEntityResult result;
    GetClientSpatialDatabase()->SphereQuery ( result, CSphere ( m_vecCameraPos, fVehicleQueryRadius ) );
 
    std::vector < CClientVehicle* > vehicleList;

    // For each entity found
    for ( CClientEntityResult::const_iterator iter = result.begin () ; iter != result.end (); ++iter )
    {
        switch ( (*iter)->GetType () )
        {
            case CCLIENTVEHICLE:
                vehicleList.push_back ( (CClientVehicle*)*iter );
                break;
        }
    }

    // Compile a list of vehicle models which should be cached
    std::map < ushort, float > newNeedCacheList;
    ProcessVehicleList ( newNeedCacheList, vehicleList, Square ( VEHICLE_STREAM_IN_DISTANCE + STREAMER_STREAM_OUT_EXTRA_DISTANCE + m_fSmoothCameraSpeed * 2 ) );

    // Apply desired caching
    m_pCoreModelCacheManager->UpdateVehicleModelCaching ( newNeedCacheList );
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::ProcessPlayerList
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::ProcessPlayerList ( std::map < ushort, float >& outNeedCacheList, const std::vector < CClientPlayer* >& playerList, float fMaxStreamDistanceSq )
{
    const ulong ulTimeNow = CClientTime::GetTime ();
    for (  std::vector < CClientPlayer* >::const_iterator iter = playerList.begin () ; iter != playerList.end (); ++iter )
    {
        CClientPlayer* pPlayer = *iter;
        ushort usModelId = (ushort)pPlayer->GetModel ();

        if ( usModelId < 7 || usModelId > 312 )
            continue;

        // Check if currently within distance
        {
            // Check distance
            CVector vecPosition;
            pPlayer->GetPosition ( vecPosition );
            float fDistSq = ( m_vecCameraPos - vecPosition ).LengthSquared ();
            if ( fDistSq < fMaxStreamDistanceSq )
            {
                // Add model to needed list
                InsertIntoNeedCacheList ( outNeedCacheList, usModelId, fDistSq );
                AddProcessStat ( "p", true, PURESYNC_TYPE_NONE, usModelId, vecPosition, vecPosition );
                continue;
            }
        }

        // Check if will be within distance soon
        ePuresyncType syncType = pPlayer->GetLastPuresyncType ();
        if ( syncType == PURESYNC_TYPE_PURESYNC || syncType == PURESYNC_TYPE_LIGHTSYNC )
        {
            ulong ulSyncAge = ulTimeNow - pPlayer->GetLastPuresyncTime ();
            if ( ulSyncAge < 8000 )
            {
                // Get velocity from somewhere
                CVector vecVelocity;
                CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
                if ( syncType == PURESYNC_TYPE_LIGHTSYNC )
                {
                    vecVelocity = pPlayer->GetLightsyncCalcedVelocity ();
                }
                else
                {
                    if ( pVehicle )
                        pVehicle->GetMoveSpeed ( vecVelocity );
                    else
                        pPlayer->GetMoveSpeed ( vecVelocity );
                    vecVelocity *= m_fGameFps;
                }

                // Extrapolate position
                float fSecondsToAdd = Min ( 6000UL, ulSyncAge + 2000 ) * 0.001f;
                CVector vecPosition;
                pPlayer->GetPosition ( vecPosition );
                CVector vecNewPosition = vecPosition + vecVelocity * fSecondsToAdd;

                // Check distance
                float fDistSq = ( m_vecCameraPos - vecNewPosition ).LengthSquared ();
                if ( fDistSq < fMaxStreamDistanceSq )
                {
                    // Add model to needed list
                    InsertIntoNeedCacheList ( outNeedCacheList, usModelId, fDistSq );
                    AddProcessStat ( "l", true, syncType, usModelId, vecPosition, vecNewPosition );
                    continue;
                }
                AddProcessStat ( "n", false, syncType, usModelId, vecPosition, vecNewPosition );
                continue;
            }
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::ProcessPedList
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::ProcessPedList ( std::map < ushort, float >& outNeedCacheList, const std::vector < CClientPed* >& pedList, float fMaxStreamDistanceSq )
{
    const ulong ulTimeNow = CClientTime::GetTime ();
    for (  std::vector < CClientPed* >::const_iterator iter = pedList.begin () ; iter != pedList.end (); ++iter )
    {
        CClientPed* pPed = *iter;
        const ushort usModelId = (ushort)pPed->GetModel ();

        if ( usModelId < 7 || usModelId > 312 )
            continue;

        // Check if currently within distance
        {
            // Check distance
            CVector vecPosition;
            pPed->GetPosition ( vecPosition );
            float fDistSq = ( m_vecCameraPos - vecPosition ).LengthSquared ();
            if ( fDistSq < fMaxStreamDistanceSq )
            {
                // Add model to needed list
                InsertIntoNeedCacheList ( outNeedCacheList, usModelId, fDistSq );
                AddProcessStat ( "p", true, PURESYNC_TYPE_NONE, usModelId, vecPosition, vecPosition );
                continue;
            }
        }

        // Check if will be within distance soon
        {
             // Extrapolate position for 2 seconds time
            ulong ulSyncAge = 0;
            if ( ulSyncAge < 8000 )
            {
                // Get velocity
                CVector vecVelocity;
                pPed->GetMoveSpeed ( vecVelocity );
                vecVelocity *= m_fGameFps;

                // Extrapolate position for 2 seconds time
                float fSecondsToAdd = Min ( 6000UL, ulSyncAge + 2000 ) * 0.001f;
                CVector vecPosition;
                pPed->GetPosition ( vecPosition );
                CVector vecNewPosition = vecPosition + vecVelocity * fSecondsToAdd;

                // Check distance
                float fDistSq = ( m_vecCameraPos - vecNewPosition ).LengthSquared ();
                if ( fDistSq < fMaxStreamDistanceSq )
                {
                    // Add model to needed list
                    InsertIntoNeedCacheList ( outNeedCacheList, usModelId, fDistSq );
                    AddProcessStat ( "l", true, PURESYNC_TYPE_NONE, usModelId, vecPosition, vecNewPosition );
                    continue;
                }
                AddProcessStat ( "n", false, PURESYNC_TYPE_NONE, usModelId, vecPosition, vecNewPosition );
                continue;
            }
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::ProcessVehicleList
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::ProcessVehicleList ( std::map < ushort, float >& outNeedCacheList, const std::vector < CClientVehicle* >& vehicleList, float fMaxStreamDistanceSq )
{
    const ulong ulTimeNow = CClientTime::GetTime ();
    for (  std::vector < CClientVehicle* >::const_iterator iter = vehicleList.begin () ; iter != vehicleList.end (); ++iter )
    {
        CClientVehicle* pVehicle = *iter;
        const ushort usModelId = pVehicle->GetModel ();

        if ( usModelId < 400 || usModelId > 611 )
            continue;

        // Check if currently within distance
        {
            // Check distance
            CVector vecPosition;
            pVehicle->GetPosition ( vecPosition );
            float fDistSq = ( m_vecCameraPos - vecPosition ).LengthSquared ();
            if ( fDistSq < fMaxStreamDistanceSq )
            {
                // Add model to needed list
                InsertIntoNeedCacheList ( outNeedCacheList, usModelId, fDistSq );
                AddProcessStat ( "p", true, PURESYNC_TYPE_NONE, usModelId, vecPosition, vecPosition );
                continue;
            }
        }

        CClientPlayer* pDriver = DynamicCast < CClientPlayer > ( pVehicle->GetControllingPlayer () );
        if ( !pDriver )
            continue;

        // Check if will be within distance soon
        ePuresyncType syncType = pDriver->GetLastPuresyncType ();
        if ( syncType == PURESYNC_TYPE_PURESYNC || syncType == PURESYNC_TYPE_LIGHTSYNC )
        {
            ulong ulSyncAge = ulTimeNow - pDriver->GetLastPuresyncTime ();
            if ( ulSyncAge < 8000 )
            {
                // Get velocity from somewhere
                CVector vecVelocity;
                if ( syncType == PURESYNC_TYPE_LIGHTSYNC )
                    vecVelocity = pDriver->GetLightsyncCalcedVelocity ();
                else
                {
                    pVehicle->GetMoveSpeed ( vecVelocity );
                    vecVelocity *= m_fGameFps;
                }

                // Extrapolate position for 2 seconds time
                float fSecondsToAdd = Min ( 6000UL, ulSyncAge + 2000 ) * 0.001f;
                CVector vecPosition;
                pVehicle->GetPosition ( vecPosition );
                CVector vecNewPosition = vecPosition + vecVelocity * fSecondsToAdd;

                // Check distance
                float fDistSq = ( m_vecCameraPos - vecNewPosition ).LengthSquared ();
                if ( fDistSq < fMaxStreamDistanceSq )
                {
                    // Add model to needed list
                    InsertIntoNeedCacheList ( outNeedCacheList, usModelId, fDistSq );
                    AddProcessStat ( "l", true, syncType, usModelId, vecPosition, vecNewPosition );
                    continue;
                }
                AddProcessStat ( "n", false, syncType, usModelId, vecPosition, vecNewPosition );
                continue;
            }
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::InsertIntoNeedCacheList
//
// Update model id closest distance
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::InsertIntoNeedCacheList ( std::map < ushort, float >& outNeedCacheList, ushort usModelId, float fDistSq )
{
    float* pfDistSqCurrent = MapFind ( outNeedCacheList, usModelId );
    if ( !pfDistSqCurrent )
    {
        MapSet ( outNeedCacheList, usModelId, fDistSq );
        return;
    }
    if ( fDistSq < *pfDistSqCurrent )
        *pfDistSqCurrent = fDistSq;
}



//
//
//
//
// Stats
//
//
//
//

///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::ClearStats
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::ClearStats ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::AddProcessStat
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::AddProcessStat ( const char* szTag, bool bCache, ePuresyncType syncType, ushort usModelId, const CVector& vecStartPos, const CVector& vecEndPos )
{
#ifdef WITH_MODEL_CACHE_STATS
    SModelCacheStatItem item;
    item.strTag = szTag;
    item.bCache = bCache;
    item.syncType = syncType;
    item.usModelId = usModelId;
    item.vecStart = vecStartPos;
    item.vecEnd = vecEndPos;
    m_StatsList.push_back ( item );
#endif
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::OnRestreamModel
//
// Uncache here, now.
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::OnRestreamModel ( ushort usModelId )
{
    m_pCoreModelCacheManager->OnRestreamModel ( usModelId );
}
