/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CModelCacheManager.h"

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
// CModelCacheManager
//
//
///////////////////////////////////////////////////////////////
class CModelCacheManagerImpl : public CModelCacheManager
{
public:
    ZERO_ON_NEW

    // CModelCacheManager interface
    virtual void                DoPulse                             ( void );
    virtual void                GetStats                            ( SModelCacheStats& outStats );
    virtual void                OnRestreamModel                     ( ushort usModelId );
    virtual void                OnClientClose                       ( void );
    virtual void                UpdatePedModelCaching               ( const std::map < ushort, float >& newNeedCacheList );
    virtual void                UpdateVehicleModelCaching           ( const std::map < ushort, float >& newNeedCacheList );

    // CModelCacheManagerImpl methods
                                CModelCacheManagerImpl              ( void );
                                ~CModelCacheManagerImpl             ( void );

    void                        PreLoad                             ( void );
    void                        RemoveCacheRefs                     ( std::map < ushort, SModelCacheInfo >& currentCacheInfoMap );
    void                        UpdateModelCaching                  ( const std::map < ushort, float >& newNeededList, std::map < ushort, SModelCacheInfo >& currentCacheInfoMap, uint uiMaxCachedAllowed );
    int                         GetModelRefCount                    ( ushort usModelId );
    void                        AddModelRefCount                    ( ushort usModelId );
    void                        SubModelRefCount                    ( ushort usModelId );

protected:
    CGame*                      m_pGame;
    int                         m_iFrameCounter;
    CTickCount                  m_TickCountNow;
    bool                        m_bDonePreLoad;
    uint                        m_uiMaxCachedPedModels;
    uint                        m_uiMaxCachedVehicleModels;
    std::map < ushort, SModelCacheInfo >    m_PedModelCacheInfoMap;
    std::map < ushort, SModelCacheInfo >    m_VehicleModelCacheInfoMap;
};


///////////////////////////////////////////////////////////////
//
// Global new
//
//
///////////////////////////////////////////////////////////////
CModelCacheManager* NewModelCacheManager ()
{
    return new CModelCacheManagerImpl ();
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::CModelCacheManagerImpl
//
///////////////////////////////////////////////////////////////
CModelCacheManagerImpl::CModelCacheManagerImpl ( void )
{
    m_pGame = CCore::GetSingleton ().GetGame ();
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::~CModelCacheManagerImpl
//
// Clean up when quitting
//
///////////////////////////////////////////////////////////////
CModelCacheManagerImpl::~CModelCacheManagerImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::OnClientClose
//
// Clean up when client.dll unloads
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::OnClientClose ( void )
{
    // Remove all extra refs applied here
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::PreLoad
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
void CModelCacheManagerImpl::PreLoad ( void )
{
    if ( m_bDonePreLoad )
        return;

    m_bDonePreLoad = true;

    CTickCount startTicks = CTickCount::Now ();
#if 0
    for ( uint i = 321 ; i <= 372 ; i++ )
    {
        if ( CClientPedManager::IsValidWeaponModel ( i ) )
            AddModelRefCount ( i );
    }
    m_pGame->GetStreaming()->LoadAllRequestedModels ( false );
#endif
    for ( uint i = 1000 ; i <= 1193 ; i++ )
    {
        AddModelRefCount ( i );
    }
    m_pGame->GetStreaming()->LoadAllRequestedModels ( false );

    CTickCount deltaTicks = CTickCount::Now () - startTicks;
    OutputDebugLine ( SString ( "CModelCacheManagerImpl::PreLoad completed in %d ms", deltaTicks.ToInt () ) );
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::GetStats
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::GetStats ( SModelCacheStats& outStats )
{
    outStats.uiMaxNumPedModels = m_uiMaxCachedPedModels;
    outStats.uiMaxNumVehicleModels = m_uiMaxCachedVehicleModels;
    outStats.uiNumPedModels = 0;
    outStats.uiNumVehicleModels = 0;

    for ( std::map < ushort, SModelCacheInfo >::const_iterator iter = m_PedModelCacheInfoMap.begin () ; iter != m_PedModelCacheInfoMap.end () ; ++iter )
        if ( iter->second.bIsModelCachedHere )
            outStats.uiNumPedModels++;

    for ( std::map < ushort, SModelCacheInfo >::const_iterator iter = m_VehicleModelCacheInfoMap.begin () ; iter != m_VehicleModelCacheInfoMap.end () ; ++iter )
        if ( iter->second.bIsModelCachedHere )
            outStats.uiNumVehicleModels++;
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::DoPulse
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::DoPulse ( void )
{
    m_TickCountNow = CTickCount::Now ();

    PreLoad ();

    int iVal;
    CVARS_GET ( "model_cache", iVal );

    if ( iVal == 0 )    // Off
    {
        m_uiMaxCachedPedModels = 0;
        m_uiMaxCachedVehicleModels = 0;
    }
    else
    if ( iVal == 1 )    // Default
    {
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
    }
    else
    if ( iVal == 2 )    // Max
    {
        m_uiMaxCachedPedModels = 350;
        m_uiMaxCachedVehicleModels = 250;
    }
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::UpdatePedModelCaching
//
//
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::UpdatePedModelCaching ( const std::map < ushort, float >& newNeedCacheList )
{
    DoPulse ();
    UpdateModelCaching ( newNeedCacheList, m_PedModelCacheInfoMap, m_uiMaxCachedPedModels );
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::UpdateVehicleModelCaching
//
//
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::UpdateVehicleModelCaching ( const std::map < ushort, float >& newNeedCacheList )
{
    DoPulse ();
    UpdateModelCaching ( newNeedCacheList, m_VehicleModelCacheInfoMap, m_uiMaxCachedVehicleModels );
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::RemoveCacheRefs
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::RemoveCacheRefs ( std::map < ushort, SModelCacheInfo >& currentCacheInfoMap )
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
// CModelCacheManagerImpl::UpdateModelCaching
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::UpdateModelCaching ( const std::map < ushort, float >& newNeedCacheList, std::map < ushort, SModelCacheInfo >& currentCacheInfoMap, uint uiMaxCachedAllowed )
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
// CModelCacheManagerImpl::GetModelRefCount
//
///////////////////////////////////////////////////////////////
int CModelCacheManagerImpl::GetModelRefCount ( ushort usModelId )
{
    CModelInfo* pModelInfo = m_pGame->GetModelInfo ( usModelId );
    if ( pModelInfo )
        return pModelInfo->GetRefCount ();
    return 0;
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::AddModelRefCount
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::AddModelRefCount ( ushort usModelId )
{
    CModelInfo* pModelInfo = m_pGame->GetModelInfo ( usModelId );
    if ( pModelInfo )
        pModelInfo->ModelAddRef ( NON_BLOCKING, "cache" );
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::SubModelRefCount
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::SubModelRefCount ( ushort usModelId )
{
    CModelInfo* pModelInfo = m_pGame->GetModelInfo ( usModelId );
    if ( pModelInfo )
        pModelInfo->RemoveRef ();
}


///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::OnRestreamModel
//
// Uncache here, now.
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::OnRestreamModel ( ushort usModelId )
{
    std::map < ushort, SModelCacheInfo >* mapList[] = { &m_PedModelCacheInfoMap, &m_VehicleModelCacheInfoMap };

    for ( uint i = 0 ; i < NUMELMS( mapList ) ; i++ )
    {
        std::map < ushort, SModelCacheInfo >& cacheInfoMap = *mapList[i];

        SModelCacheInfo* pInfo = MapFind ( cacheInfoMap, usModelId );
        if ( pInfo )
        {
            if ( pInfo->bIsModelCachedHere )
            {
                SubModelRefCount ( usModelId );
                pInfo->bIsModelCachedHere = false;
                MapRemove ( cacheInfoMap, usModelId );
                OutputDebugLine ( SString ( "[Cache] End caching model %d", usModelId ) );
            }
        }
    }
}
