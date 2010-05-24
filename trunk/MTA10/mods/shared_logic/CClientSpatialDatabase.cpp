/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSpatialDatabase.cpp
*  PURPOSE:     
*  DEVELOPERS:  A dynamic_cast of thousands
*
*****************************************************************************/

#include "StdInc.h"
#include "RTree.h"

// Define our tree type
typedef RTree < CClientEntity*, float, 3 > CClientEntityTree;

//
// SEntityInfo used by CClientSpatialDatabaseImpl
//
struct SEntityInfo
{
    CBox box;
};

///////////////////////////////////////////////////////////////
//
// CClientSpatialDatabaseImpl
//
///////////////////////////////////////////////////////////////
class CClientSpatialDatabaseImpl : public CClientSpatialDatabase
{
public:
    // CClientSpatialDatabase interface
    virtual void        UpdateEntity        ( CClientEntity* pEntity );
    virtual void        RemoveEntity        ( CClientEntity* pEntity );
    virtual bool        IsEntityPresent     ( CClientEntity* pEntity );
    virtual void        SphereQuery         ( CClientEntityResult& outResult, const CSphere& sphere );
    virtual void        AllQuery            ( CClientEntityResult& outResult );

    // CClientSpatialDatabaseImpl functions
    void                FlushUpdateQueue    ( void );

    CClientEntityTree                           m_Tree;
    std::map < CClientEntity*, SEntityInfo >    m_InfoMap;
    std::map < CClientEntity*, int >            m_UpdateQueue;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CClientSpatialDatabaseImpl* g_pClientSpatialDatabaseImp = NULL;

CClientSpatialDatabase* GetClientSpatialDatabase ()
{
    if ( !g_pClientSpatialDatabaseImp )
        g_pClientSpatialDatabaseImp = new CClientSpatialDatabaseImpl ();
    return g_pClientSpatialDatabaseImp;
}


///////////////////////////////////////////////////////////////
//
// CClientSpatialDatabaseImpl::UpdateEntity
//
//
//
///////////////////////////////////////////////////////////////
void CClientSpatialDatabaseImpl::UpdateEntity ( CClientEntity* pEntity )
{
    // Add the entity to a list of pending updates
    m_UpdateQueue[ pEntity ] = 1;
}


///////////////////////////////////////////////////////////////
//
// CClientSpatialDatabaseImpl::RemoveEntity
//
// Remove an entity from the database
//
///////////////////////////////////////////////////////////////
void CClientSpatialDatabaseImpl::RemoveEntity ( CClientEntity* pEntity )
{
    // Remove from the tree and info map
    SEntityInfo* pInfo = MapFind ( m_InfoMap, pEntity );
    if ( pInfo )
    {
        m_Tree.Remove ( &pInfo->box.vecMin.fX, &pInfo->box.vecMax.fX, pEntity );
        MapRemove ( m_InfoMap, pEntity );
    }
    // Remove from the update queue
    MapRemove ( m_UpdateQueue, pEntity );
}


///////////////////////////////////////////////////////////////
//
// CClientSpatialDatabaseImpl::IsEntityPresent
//
// Check if an entity is in the database
//
///////////////////////////////////////////////////////////////
bool CClientSpatialDatabaseImpl::IsEntityPresent ( CClientEntity* pEntity )
{
    return MapFind ( m_InfoMap, pEntity ) != NULL || MapFind ( m_UpdateQueue, pEntity ) != NULL;
}


///////////////////////////////////////////////////////////////
//
// CClientSpatialDatabaseImpl::SphereQuery
//
// Return the list of entities that intersect the sphere
//
///////////////////////////////////////////////////////////////
void CClientSpatialDatabaseImpl::SphereQuery ( CClientEntityResult& outResult, const CSphere& sphere )
{
    // Do any pending updates first
    FlushUpdateQueue ();

    // Make a box from the sphere
    CBox box ( sphere.vecPosition, fabsf ( sphere.fRadius ) );
    // Make everything 2D for now
    box.vecMin.fZ = SPATIAL_2D_Z;
    box.vecMax.fZ = SPATIAL_2D_Z;

    // Find all entiites which overlap the box
    m_Tree.Search( &box.vecMin.fX, &box.vecMax.fX, outResult );

    #ifdef SPATIAL_DATABASE_DEBUG_OUTPUTA
        OutputDebugLine ( SString ( "SpatialDatabase::SphereQuery %d results for %2.0f,%2.0f,%2.0f  %2.2f"
                                            ,outResult.size ()
                                            ,sphere.vecPosition.fX
                                            ,sphere.vecPosition.fY
                                            ,sphere.vecPosition.fZ
                                            ,sphere.fRadius
                                            ) );
    #endif
}


///////////////////////////////////////////////////////////////
//
// CClientSpatialDatabaseImpl::AllQuery
//
// Return the list of all entities
//
///////////////////////////////////////////////////////////////
void CClientSpatialDatabaseImpl::AllQuery ( CClientEntityResult& outResult )
{
    // Do any pending updates first
    FlushUpdateQueue ();

    // Copy results from map to output
    outResult.clear ();
    for ( std::map < CClientEntity*, SEntityInfo >::iterator it = m_InfoMap.begin (); it != m_InfoMap.end (); ++it )
        outResult.push_back ( it->first );
}


///////////////////////////////////////////////////////////////
//
// CClientSpatialDatabaseImpl::FlushUpdateQueue
//
// Process all entities that have changed since the last call
//
///////////////////////////////////////////////////////////////
void CClientSpatialDatabaseImpl::FlushUpdateQueue ( void )
{
    int iTotalToUpdate = m_UpdateQueue.size ();
    int iTotalUpdated = 0;

    for ( std::map < CClientEntity*, int >::iterator it = m_UpdateQueue.begin (); it != m_UpdateQueue.end (); ++it )
    {
        CClientEntity* pEntity = it->first;

        // Get the new bounding box
        SEntityInfo newInfo;
        CSphere sphere = pEntity->GetWorldBoundingSphere ();
        newInfo.box = CBox ( sphere.vecPosition, fabsf ( sphere.fRadius ) );
        // Make everything 2D for now
        newInfo.box.vecMin.fZ = SPATIAL_2D_Z;
        newInfo.box.vecMax.fZ = SPATIAL_2D_Z;

        // Get previous info
        if ( SEntityInfo* pOldInfo = MapFind ( m_InfoMap, pEntity ) )
        {
            // Don't update if bounding box is the same
            if ( pOldInfo->box == newInfo.box )
                continue;

            // Remove old bounding box from tree
            m_Tree.Remove ( &pOldInfo->box.vecMin.fX, &pOldInfo->box.vecMax.fX, pEntity );
        }

        // Add new bounding box
        m_Tree.Insert( &newInfo.box.vecMin.fX, &newInfo.box.vecMax.fX, pEntity );

        // Update info map
        MapSet ( m_InfoMap, pEntity, newInfo );
        iTotalUpdated++;
        #ifdef SPATIAL_DATABASE_DEBUG_OUTPUTA
            OutputDebugLine ( SString ( "SpatialDatabase::UpdateEntity %08x  %2.0f,%2.0f,%2.0f   %2.0f,%2.0f,%2.0f"
                                                ,pEntity
                                                ,info.box.vecMin.fX
                                                ,info.box.vecMin.fY
                                                ,info.box.vecMin.fZ
                                                ,info.box.vecMax.fX
                                                ,info.box.vecMax.fY
                                                ,info.box.vecMax.fZ
                                                ) );
        #endif
    }
    m_UpdateQueue.clear ();

    #ifdef SPATIAL_DATABASE_DEBUG_OUTPUTB
        if ( iTotalToUpdate )
            OutputDebugLine ( SString ( "SpatialDatabase::FlushUpdateQueue  TotalToUpdate: %d   TotalUpdated: %d  m_InfoMap: %d    tree: %d  ", iTotalToUpdate, iTotalUpdated, m_InfoMap.size (), m_Tree.Count () ) );
    #endif
}
