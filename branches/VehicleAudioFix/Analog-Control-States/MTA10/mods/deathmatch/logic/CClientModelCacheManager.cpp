/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#ifdef MTA_DEBUG
    #define WITH_MODEL_CACHE_STATS 1
#endif

#define PED_STREAM_IN_DISTANCE              (250)
#define VEHICLE_STREAM_IN_DISTANCE          (250)
#define STREAMER_STREAM_OUT_EXTRA_DISTANCE  (50)

#define PED_MAX_STREAM_DISTANCE             ( PED_STREAM_IN_DISTANCE + STREAMER_STREAM_OUT_EXTRA_DISTANCE )
#define PED_MAX_STREAM_DISTANCE_SQ          ( PED_MAX_STREAM_DISTANCE * PED_MAX_STREAM_DISTANCE )

#define VEHICLE_MAX_STREAM_DISTANCE         ( VEHICLE_STREAM_IN_DISTANCE + STREAMER_STREAM_OUT_EXTRA_DISTANCE )
#define VEHICLE_MAX_STREAM_DISTANCE_SQ      ( VEHICLE_MAX_STREAM_DISTANCE * VEHICLE_MAX_STREAM_DISTANCE )

#define PED_MAX_VELOCITY                    (10)
#define VEHICLE_MAX_VELOCITY                (10)


namespace
{
    struct SModelCacheInfo
    {
        SModelCacheInfo ( void ) : fClosestDistSq ( 0 ), bIsModelCachedHere ( false ), bIsModelLoadedByGame ( false ) {}
        CTickCount lastNeeded;
        CTickCount firstNeeded;
        float fClosestDistSq;
        bool bIsModelCachedHere;
        bool bIsModelLoadedByGame;
    };
}


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
    virtual void                GetStats                            ( std::vector < SModelCacheStatItem >& outList );
    virtual void                DrawStats                           ( void );

    // CClientModelCacheManagerImpl methods
                                CClientModelCacheManagerImpl        ( void );
                                ~CClientModelCacheManagerImpl       ( void );

    void                        PreLoad                             ( void );
    void                        DoPulsePedModels                    ( void );
    void                        DoPulseVehicleModels                ( void );
    void                        ProcessPlayerList                   ( std::map < ushort, float >& outNeedCacheList, const std::vector < CClientPlayer* >& playerList, float fMaxStreamDistanceSq );
    void                        ProcessPedList                      ( std::map < ushort, float >& outNeedCacheList, const std::vector < CClientPed* >& pedList, float fMaxStreamDistanceSq );
    void                        ProcessVehicleList                  ( std::map < ushort, float >& outNeedCacheList, const std::vector < CClientVehicle* >& vehicleList, float fMaxStreamDistanceSq );
    void                        RemoveCacheRefs                     ( std::map < ushort, SModelCacheInfo >& currentCacheInfoMap );
    void                        UpdateModelCaching                  ( const std::map < ushort, float >& newNeededList, std::map < ushort, SModelCacheInfo >& currentCacheInfoMap, uint uiMaxCachedAllowed );
    int                         GetModelRefCount                    ( ushort usModelId );
    void                        AddModelRefCount                    ( ushort usModelId );
    void                        SubModelRefCount                    ( ushort usModelId );
    void                        InsertIntoNeedCacheList             ( std::map < ushort, float >& outNeedCacheList, ushort usModelId, float fDistSq );
    void                        ClearStats                          ( void );
    void                        AddProcessStat                      ( const char* szTag, bool bCache, ePuresyncType syncType, ushort usModelId, const CVector& vecStartPos, const CVector& vecEndPos );

protected:
    int                         m_iFrameCounter;
    CTickCount                  m_TickCountNow;
    bool                        m_bDonePreLoad;
    CVector                     m_vecCameraPos;
    CTickCount                  m_LastTimeMs;
    float                       m_fSmoothCameraSpeed;
    CClientPlayer*              m_pLocalPlayer;
    float                       m_fGameFps;
    uint                        m_uiMaxCachedPedModels;
    uint                        m_uiMaxCachedVehicleModels;
    std::map < ushort, SModelCacheInfo >    m_PedModelCacheInfoMap;
    std::map < ushort, SModelCacheInfo >    m_VehicleModelCacheInfoMap;
    std::vector < SModelCacheStatItem >     m_StatsList;
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
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::~CClientModelCacheManagerImpl
//
///////////////////////////////////////////////////////////////
CClientModelCacheManagerImpl::~CClientModelCacheManagerImpl ( void )
{
    // Remove all extra refs applied here
    RemoveCacheRefs ( m_PedModelCacheInfoMap ) ;
    RemoveCacheRefs ( m_VehicleModelCacheInfoMap ) ;
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::PreLoad
//
// Cache all weapons and upgrades
//
// Peds KB:                64,832 KB         7-312     306  296 valid, 10 not so valid               219   KB/model             4.45/MB
// Weapons KB:             470               321-372   52   39 valid, 3 invalid(329,332,340)         470   KB all weapons
// Upgrades KB:            2,716             1000-1193 194  all valid                              2,716   KB all upgrades
// Vehicles(400-499) KB:   14,622                                                                    140   KB/model             7/MB
// Vehicles(500-599) KB:   14,888
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::PreLoad ( void )
{
    if ( m_bDonePreLoad )
        return;

    m_bDonePreLoad = true;

    CTickCount startTicks = CTickCount::Now ();
#if 0
    for ( uint i = 321 ; i <= 372 ; i++ )
    {
        AddModelRefCount ( i );
    }
    g_pGame->GetStreaming()->LoadAllRequestedModels ( false );
#endif
    for ( uint i = 1000 ; i <= 1193 ; i++ )
    {
        AddModelRefCount ( i );
    }
    g_pGame->GetStreaming()->LoadAllRequestedModels ( false );

    CTickCount deltaTicks = CTickCount::Now () - startTicks;
    OutputDebugLine ( SString ( "CClientModelCacheManagerImpl::PreLoad completed in %d ms", deltaTicks.ToInt () ) );
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

    PreLoad ();
    m_fGameFps = g_pGame->GetFPS ();


    // Adjust cache numbers depending on amount of streaming memory allocated
    //
    //  64MB streaming = 2+1 MB for peds & vehicles          9 peds + 7 veh
    //  96MB streaming = 4+4 MB for peds & vehicles         18 peds + 28 veh
    //  128MB streaming = 8+8 MB for peds & vehicles        36 peds + 56 veh
    //  256MB streaming = 16+8 MB for peds & vehicles       72 peds + 56 veh
    //
    int iStreamingMemoryAvailableKB             = *(int*)0x08A5A80;

    SSamplePoint < float > pedPoints[] = { {65536, 9},  {98304, 18},   {131072, 36},   {262144, 72} };
    SSamplePoint < float > vehPoints[] = { {65536, 7},  {98304, 28},   {131072, 56},   {262144, 56} };

    m_uiMaxCachedPedModels = (int)EvalSamplePosition < float > ( pedPoints, NUMELMS ( pedPoints ), (float)iStreamingMemoryAvailableKB );
    m_uiMaxCachedVehicleModels = (int)EvalSamplePosition < float > ( vehPoints, NUMELMS ( vehPoints ), (float)iStreamingMemoryAvailableKB );


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
    UpdateModelCaching ( newNeedCacheList, m_PedModelCacheInfoMap, m_uiMaxCachedPedModels );
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
    UpdateModelCaching ( newNeedCacheList, m_VehicleModelCacheInfoMap, m_uiMaxCachedVehicleModels );
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
                //MapInsert ( outNeedCacheList, usModelId );
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
                    //MapInsert ( outNeedCacheList, usModelId );
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
// CClientModelCacheManagerImpl::RemoveCacheRefs
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::RemoveCacheRefs ( std::map < ushort, SModelCacheInfo >& currentCacheInfoMap )
{
    for ( std::map < ushort, SModelCacheInfo >::iterator iter = currentCacheInfoMap.begin () ; iter != currentCacheInfoMap.end () ; ++iter )
    {
        const ushort usModelId = iter->first;
        SModelCacheInfo& info = iter->second;

        if ( info.bIsModelCachedHere )
        {
            SubModelRefCount ( usModelId );
            info.bIsModelCachedHere = false;
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::UpdateModelCaching
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::UpdateModelCaching ( const std::map < ushort, float >& newNeedCacheList, std::map < ushort, SModelCacheInfo >& currentCacheInfoMap, uint uiMaxCachedAllowed )
{
    // Update some flags and remove info for uncached and unneeded
    for ( std::map < ushort, SModelCacheInfo >::iterator iter = currentCacheInfoMap.begin () ; iter != currentCacheInfoMap.end () ; )
    {
        const ushort usModelId = iter->first;
        SModelCacheInfo& info = iter->second;

        info.bIsModelLoadedByGame = GetModelRefCount ( usModelId ) > ( info.bIsModelCachedHere ? 1 : 0 );

        if ( !info.bIsModelLoadedByGame && !info.bIsModelCachedHere )
        {
            if ( ( m_TickCountNow - info.lastNeeded ).ToInt () > 1000 )
            {
                // Not loaded, cached or needed for 1 second, so remove info
                currentCacheInfoMap.erase ( iter++ );
                continue;
            }
        }
        ++iter;
    }


    // Update current from new needed 
    for ( std::map < ushort, float >::const_iterator iter = newNeedCacheList.begin () ; iter != newNeedCacheList.end () ; ++iter )
    {
        SModelCacheInfo& info = MapGet ( currentCacheInfoMap, iter->first );
        info.fClosestDistSq = iter->second;
        info.lastNeeded = m_TickCountNow;
        if ( info.firstNeeded.ToInt () == 0 )
            info.firstNeeded = m_TickCountNow;
    }


    uint uiNumModelsCachedHereOnly = 0;

    std::map < uint, ushort > maybeUncacheUnneededList;
    std::map < uint, ushort > maybeUncacheNeededList;
    std::map < uint, ushort > maybeCacheList;

    // Update active 
    for ( std::map < ushort, SModelCacheInfo >::iterator iter = currentCacheInfoMap.begin () ; iter != currentCacheInfoMap.end () ; ++iter )
    {
        const ushort usModelId = iter->first;
        SModelCacheInfo& info = iter->second;

        if ( info.bIsModelLoadedByGame )
        {
            info.lastNeeded = m_TickCountNow;

            // Add cache ref here so when game tries to unload the model, we can keep it loaded
            if ( !info.bIsModelCachedHere )
            {
                AddModelRefCount ( usModelId );
                info.bIsModelCachedHere = true;
            }
        }
        else
        {
            if ( info.bIsModelCachedHere )
            {
                uiNumModelsCachedHereOnly++;
                // Update cached models that could be uncached
                uint uiTicksSinceLastNeeded = ( m_TickCountNow - info.lastNeeded ).ToInt ();
                if ( uiTicksSinceLastNeeded > 0 )
                    MapSet ( maybeUncacheUnneededList, uiTicksSinceLastNeeded, usModelId );
                else
                    MapSet ( maybeUncacheNeededList, (int)info.fClosestDistSq, usModelId );
            }
            else
            {
                if ( info.lastNeeded == m_TickCountNow )
                {
                    // Update uncached models that could be cached
                    uint uiTicksSinceFirstNeeded = ( m_TickCountNow - info.firstNeeded ).ToInt ();
                    MapSet ( maybeCacheList, uiTicksSinceFirstNeeded, usModelId );
                }
            }
        }
    }

    // If at or above cache limit, try to uncache unneeded first
    if ( uiNumModelsCachedHereOnly >= uiMaxCachedAllowed && !maybeUncacheUnneededList.empty () )
    {
            const ushort usModelId = maybeUncacheUnneededList.rbegin ()->second;
            SModelCacheInfo* pInfo = MapFind ( currentCacheInfoMap, usModelId );
            assert ( pInfo );
            assert ( pInfo->bIsModelCachedHere );
            SubModelRefCount ( usModelId );
            pInfo->bIsModelCachedHere = false;
            MapRemove ( currentCacheInfoMap, usModelId );
            OutputDebugLine ( SString ( "[Cache] End caching model %d", usModelId ) );
    }
    else
    if ( uiNumModelsCachedHereOnly > uiMaxCachedAllowed && !maybeUncacheNeededList.empty () )
    {
        // Only uncache from the needed list if above limit

        // Uncache furthest away model
        const ushort usModelId = maybeUncacheNeededList.rbegin ()->second;
        SModelCacheInfo* pInfo = MapFind ( currentCacheInfoMap, usModelId );
        assert ( pInfo );
        assert ( pInfo->bIsModelCachedHere );
        SubModelRefCount ( usModelId );
        pInfo->bIsModelCachedHere = false;
        MapRemove ( currentCacheInfoMap, usModelId );
        OutputDebugLine ( SString ( "[Cache] End caching model %d", usModelId ) );
    }

    // Cache if room
    if ( !maybeCacheList.empty () && uiNumModelsCachedHereOnly < uiMaxCachedAllowed )
    {
        // Cache one which has been waiting the longest
        const ushort usModelId = maybeCacheList.rbegin ()->second;
        SModelCacheInfo* pInfo = MapFind ( currentCacheInfoMap, usModelId );
        assert ( pInfo );
        assert ( !pInfo->bIsModelCachedHere );
        AddModelRefCount ( usModelId );
        pInfo->bIsModelCachedHere = true;
        OutputDebugLine ( SString ( "[Cache] Start caching model %d", usModelId ) );
    }
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::GetModelRefCount
//
///////////////////////////////////////////////////////////////
int CClientModelCacheManagerImpl::GetModelRefCount ( ushort usModelId )
{
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModelId );
    if ( pModelInfo )
        return pModelInfo->GetRefCount ();
    return 0;
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::AddModelRefCount
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::AddModelRefCount ( ushort usModelId )
{
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModelId );
    if ( pModelInfo )
        pModelInfo->ModelAddRef ( NON_BLOCKING, "cache" );
    else
        pModelInfo = pModelInfo;
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::SubModelRefCount
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::SubModelRefCount ( ushort usModelId )
{
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModelId );
    if ( pModelInfo )
        pModelInfo->RemoveRef ();
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
    m_StatsList.clear ();
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
// CClientModelCacheManagerImpl::GetStats
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::GetStats ( std::vector < SModelCacheStatItem >& outList )
{
    outList = m_StatsList;
}


///////////////////////////////////////////////////////////////
//
// CClientModelCacheManagerImpl::DrawStats
//
//
//
///////////////////////////////////////////////////////////////
void CClientModelCacheManagerImpl::DrawStats ( void )
{
}
