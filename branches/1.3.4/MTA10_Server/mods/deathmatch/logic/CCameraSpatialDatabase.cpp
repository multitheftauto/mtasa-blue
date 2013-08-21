/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CCameraSpatialDatabase.cpp
*  PURPOSE:     
*  DEVELOPERS:  A dynamic_cast of thousands
*
*****************************************************************************/

#include "StdInc.h"
#include "RTree.h"

namespace
{
    // Define our tree type
    typedef RTree < CPlayerCamera*, float, 2 > CItemTree;

    //
    // SItemInfo used by CCameraSpatialDatabaseImpl
    //
    struct SItemInfo
    {
        CBox box;
    };
}

///////////////////////////////////////////////////////////////
//
// CCameraSpatialDatabaseImpl
//
///////////////////////////////////////////////////////////////
class CCameraSpatialDatabaseImpl : public CCameraSpatialDatabase
{
public:
    // CCameraSpatialDatabase interface
    virtual void        UpdateItem          ( CPlayerCamera* pCamera );
    virtual void        RemoveItem          ( CPlayerCamera* pCamera );
    virtual bool        IsItemPresent       ( CPlayerCamera* pCamera );
    virtual void        SphereQuery         ( CCameraQueryResult& outResult, const CSphere& sphere );
    virtual void        AllQuery            ( CCameraQueryResult& outResult );

    // CCameraSpatialDatabaseImpl functions
    void                FlushUpdateQueue    ( void );
    bool                IsValidSphere       ( const CSphere& sphere );

    CItemTree                                   m_Tree;
    std::map < CPlayerCamera*, SItemInfo >      m_InfoMap;
    std::map < CPlayerCamera*, int >            m_UpdateQueue;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CCameraSpatialDatabaseImpl* g_pCameraSpatialDatabaseImp = NULL;

CCameraSpatialDatabase* GetCameraSpatialDatabase ()
{
    if ( !g_pCameraSpatialDatabaseImp )
        g_pCameraSpatialDatabaseImp = new CCameraSpatialDatabaseImpl ();
    return g_pCameraSpatialDatabaseImp;
}


///////////////////////////////////////////////////////////////
//
// CCameraSpatialDatabaseImpl::UpdateItem
//
//
//
///////////////////////////////////////////////////////////////
void CCameraSpatialDatabaseImpl::UpdateItem ( CPlayerCamera* pCamera )
{
    // Add the item to a list of pending updates
    m_UpdateQueue[ pCamera ] = 1;
}


///////////////////////////////////////////////////////////////
//
// CCameraSpatialDatabaseImpl::RemoveItem
//
// Remove an item from the database
//
///////////////////////////////////////////////////////////////
void CCameraSpatialDatabaseImpl::RemoveItem ( CPlayerCamera* pCamera )
{
    // Remove from the tree and info map
    SItemInfo* pInfo = MapFind ( m_InfoMap, pCamera );
    if ( pInfo )
    {
        m_Tree.Remove ( &pInfo->box.vecMin.fX, &pInfo->box.vecMax.fX, pCamera );
        MapRemove ( m_InfoMap, pCamera );
    }
    // Remove from the update queue
    MapRemove ( m_UpdateQueue, pCamera );
}


///////////////////////////////////////////////////////////////
//
// CCameraSpatialDatabaseImpl::IsItemPresent
//
// Check if an item is in the database
//
///////////////////////////////////////////////////////////////
bool CCameraSpatialDatabaseImpl::IsItemPresent ( CPlayerCamera* pCamera )
{
    return MapFind ( m_InfoMap, pCamera ) != NULL || MapFind ( m_UpdateQueue, pCamera ) != NULL;
}


///////////////////////////////////////////////////////////////
//
// CCameraSpatialDatabaseImpl::SphereQuery
//
// Return the list of entities that intersect the sphere
//
///////////////////////////////////////////////////////////////
void CCameraSpatialDatabaseImpl::SphereQuery ( CCameraQueryResult& outResult, const CSphere& sphere )
{
    // Do any pending updates first
    FlushUpdateQueue ();

    if ( !IsValidSphere ( sphere ) )
        return;

    // Make a box from the sphere
    CBox box ( sphere.vecPosition, fabsf ( sphere.fRadius ) );
    // Make everything 2D for now
    box.vecMin.fZ = SPATIAL_2D_Z;
    box.vecMax.fZ = SPATIAL_2D_Z;

    // Find all entiites which overlap the box
    m_Tree.Search( &box.vecMin.fX, &box.vecMax.fX, outResult );
}


///////////////////////////////////////////////////////////////
//
// CCameraSpatialDatabaseImpl::AllQuery
//
// Return the list of all entities
//
///////////////////////////////////////////////////////////////
void CCameraSpatialDatabaseImpl::AllQuery ( CCameraQueryResult& outResult )
{
    // Do any pending updates first
    FlushUpdateQueue ();

    // Copy results from map to output
    outResult.clear ();
    for ( std::map < CPlayerCamera*, SItemInfo >::iterator it = m_InfoMap.begin (); it != m_InfoMap.end (); ++it )
        outResult.push_back ( it->first );
}


///////////////////////////////////////////////////////////////
//
// CCameraSpatialDatabaseImpl::FlushUpdateQueue
//
// Process all entities that have changed since the last call
//
///////////////////////////////////////////////////////////////
void CCameraSpatialDatabaseImpl::FlushUpdateQueue ( void )
{
    std::map < CPlayerCamera*, int > updateQueueCopy = m_UpdateQueue;
    m_UpdateQueue.clear ();
    for ( std::map < CPlayerCamera*, int >::iterator it = updateQueueCopy.begin (); it != updateQueueCopy.end (); ++it )
    {
        CPlayerCamera* pCamera = it->first;

        // Get the new bounding box
        SItemInfo newInfo;
        CSphere sphere ( pCamera->GetPosition (), 0 );
        newInfo.box = CBox ( sphere.vecPosition, fabsf ( sphere.fRadius ) );
        // Make everything 2D for now
        newInfo.box.vecMin.fZ = SPATIAL_2D_Z;
        newInfo.box.vecMax.fZ = SPATIAL_2D_Z;

        // Get previous info
        if ( SItemInfo* pOldInfo = MapFind ( m_InfoMap, pCamera ) )
        {
            // Don't update if bounding box is the same
            if ( pOldInfo->box == newInfo.box )
                continue;

            // Remove old bounding box from tree
            m_Tree.Remove ( &pOldInfo->box.vecMin.fX, &pOldInfo->box.vecMax.fX, pCamera );
        }

        if ( !IsValidSphere ( sphere ) )
            continue;

        // Add new bounding box
        m_Tree.Insert( &newInfo.box.vecMin.fX, &newInfo.box.vecMax.fX, pCamera );

        // Update info map
        MapSet ( m_InfoMap, pCamera, newInfo );
    }
}


///////////////////////////////////////////////////////////////
//
// CCameraSpatialDatabaseImpl::IsValidSphere
//
// Is the sphere valid for use in this class
//
///////////////////////////////////////////////////////////////
bool CCameraSpatialDatabaseImpl::IsValidSphere ( const CSphere& sphere )
{
    // Check for nan
    if ( _isnan ( sphere.fRadius + sphere.vecPosition.fX + sphere.vecPosition.fY + sphere.vecPosition.fZ ) )
        return false;

    // Check radius within limits
    if ( sphere.fRadius < -12000 || sphere.fRadius > 12000 )        // radius = sqrt(worldlimits*worldlimits + worldlimits*worldlimits)
        return false;

    // Check position within limits
    float fDistSquared2D = sphere.vecPosition.fX * sphere.vecPosition.fX + sphere.vecPosition.fY * sphere.vecPosition.fY;
    if ( fDistSquared2D > 12000 * 12000 )
        return false;

    return true;
}