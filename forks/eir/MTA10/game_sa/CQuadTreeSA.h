/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CQuadTreeSA.h
*  PURPOSE:     Quad Tree world engine
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _QUAD_TREE_ENGINE_
#define _QUAD_TREE_ENGINE_

#include "CPtrNodeSA.h"

enum eQuadCheckType : unsigned int
{
    QUADC_TOP_LEFT,
    QUADC_TOP_RIGHT,
    QUADC_BOTTOM_LEFT,
    QUADC_BOTTOM_RIGHT,
    NUM_QUAD_TREE_SECTORS
};

template <typename storageType>
struct CQuadTreeNodeSAInterface //size: 40
{
    // In this node we store all allocations.
    typedef ptrNodeSingleContainer <storageType> ptrNodeContainer_t;
    typedef CPtrNodeSingleSA <storageType> ptrNode_t;

    /*=========================================================
        CQuadTreeNodeSAInterface::constructor

        Purpose:
            Constructs a quad tree node.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00552830
        Link(s) worth a read:
            http://gtaforums.com/topic/201875-map-objects-not-being-created-outside-boundaries/
    =========================================================*/
    CQuadTreeNodeSAInterface( const CBounds2D& bounds, unsigned int hierDepth ) : m_bounds( bounds )
    {
        m_bounds = bounds;
        m_hierDepth = hierDepth;
        
        for ( unsigned int n = 0; n < NUM_QUAD_TREE_SECTORS; n++ )
            m_sectors[n] = NULL;
    }

    /*=========================================================
        CQuadTreeNodeSAInterface::GetSectorIndex

        Arguments:
            pos - position to get the sector index of
        Purpose:
            Returns the sector index associated with the given
            position. If there cannot be a sector index, 
            0xFFFFFFFF is returned.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00552640
    =========================================================*/
    unsigned int __thiscall GetSectorIndex( const CVector2D& pos ) const
    {
        if ( m_hierDepth == 0 )
            return 0xFFFFFFFF;

        float midX = ( m_bounds.m_maxX + m_bounds.m_minX ) / 2;
        float midY = ( m_bounds.m_maxY + m_bounds.m_minY ) / 2;

        bool topRow = midY <= pos.fY;
        bool isRight = midX <= pos.fX;

        if ( isRight )
            return topRow ? QUADC_TOP_RIGHT : QUADC_BOTTOM_RIGHT;
        
        return topRow ? QUADC_TOP_LEFT : QUADC_BOTTOM_LEFT;
    }

    /*=========================================================
        CQuadTreeNodeSAInterface::ForEachIntersection

        Arguments:
            pos - position to call items for
            callback - function to call for every intersected item
        Purpose:
            Invokes the given callback for the allocations of
            every quad tree node sector that intersects with the
            given position.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x005529F0
    =========================================================*/
    typedef void (__cdecl*intersectCallback_t)( const CVector2D& pos, storageType *entity );

    void __thiscall ForEachIntersection( const CVector2D& pos, intersectCallback_t callback )
    {
        // Call for each data entry in this node.
        for ( ptrNode_t *iter = m_allocations.GetList(); iter != NULL; iter = iter->m_next )
        {
            callback( pos, iter->data );
        }

        // If we can select a sub node which contains pos,
        // we recurse into it.
        unsigned int sectorIndex = GetSectorIndex( pos );

        if ( sectorIndex != 0xFFFFFFFF )
        {
            CQuadTreeNodeSAInterface *subNode = m_sectors[sectorIndex];

            // Only call if the sub node has been allocated.
            if ( subNode )
                subNode->ForEachIntersection( pos, callback );
        }
    }

    /*=========================================================
        CQuadTreeNodeSAInterface::GetBounds (inlined)

        Arguments:
            bounds - pointer to save bounds into
            checkType - sector of the quad node to retrieve
        Purpose:
            Returns a partial of the complete sector of the quad
            tree node as denoted by checkType.
    =========================================================*/
    void __thiscall GetBounds( CBounds2D& bounds, eQuadCheckType checkType ) const
    {
        CBounds2D quadBounds( m_bounds );

        // Select the desired rectangle.
        // We can split it into four parts.
        // Or just leave to check against whole rectangle.
        switch( checkType )
        {
        case QUADC_TOP_LEFT:
            quadBounds.m_maxX = ( quadBounds.m_minX + quadBounds.m_maxX ) / 2;
            quadBounds.m_minY = ( quadBounds.m_maxY + quadBounds.m_minY ) / 2;
            break;
        case QUADC_TOP_RIGHT:
            quadBounds.m_minX = ( quadBounds.m_minX + quadBounds.m_maxX ) / 2;
            quadBounds.m_minY = ( quadBounds.m_maxY + quadBounds.m_minY ) / 2;
            break;
        case QUADC_BOTTOM_LEFT:
            quadBounds.m_maxX = ( quadBounds.m_minX + quadBounds.m_maxX ) / 2;
            quadBounds.m_maxY = ( quadBounds.m_minY + quadBounds.m_maxY ) / 2;
            break;
        case QUADC_BOTTOM_RIGHT:
            quadBounds.m_minX = ( quadBounds.m_minX + quadBounds.m_maxX ) / 2;
            quadBounds.m_maxY = ( quadBounds.m_minY + quadBounds.m_maxY ) / 2;
            break;
        }

        bounds = quadBounds;
    }

    /*=========================================================
        CQuadTreeNodeSAInterface::CheckBounds

        Arguments:
            bounds - 2D bounds to check against this node
            checkType - describes how to check
        Purpose:
            Returns whether the bounds validate against this
            quad tree node. Returns true if they do.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x005526A0
    =========================================================*/
    bool __thiscall CheckBounds( const CBounds2D& bounds, eQuadCheckType checkType ) const
    {
        CBounds2D quadBounds;

        // If we are the lowest sectorial node, we do not have sub sectors.
        // Hence return false.
        if ( m_hierDepth == 0 )
            return false;

        // Get the sector we want to check.
        GetBounds( quadBounds, checkType );

        // Return whether the section of the quadtree node is inside the given rectangle.
        return bounds.IsInside( quadBounds );
    }

    /*=========================================================
        CQuadTreeNodeSAInterface::LinkToEntity

        Arguments:
            entity - data entry to save
            bounds - boundary of the data entry
        Purpose:
            Saves entries of the storageType entity in every quad tree
            node which is encapsulated by bounds.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00552CD0
    =========================================================*/
    void __thiscall LinkToEntity( storageType *entity, const CBounds2D& bounds )
    {
        // If we are the lowest node of the tree, just allocate.
        if ( m_hierDepth == 0 )
        {
            m_allocations.AllocateEntry( entity );
            return;
        }

        // Go through all split sectors.
        for ( unsigned int n = 0; n < NUM_QUAD_TREE_SECTORS; n++ )
        {
            if ( !CheckBounds( bounds, (eQuadCheckType)n ) )
                continue;

            CQuadTreeNodeSAInterface *node = m_sectors[n];
            
            // If the sector has not been previously allocated,
            // do it now.
            if ( !node )
            {
                CBounds2D quadBounds;

                GetBounds( quadBounds, (eQuadCheckType)n );

                node = new CQuadTreeNodeSAInterface( quadBounds, m_hierDepth - 1 );

                m_sectors[n] = node;
            }

            // Try to link to the sub sector.
            node->LinkToEntity( entity, bounds );
        }
    }

    /*=========================================================
        CQuadTreeNodeSAInterface::RemoveItemFromHierarchy

        Arguments:
            sector - sector element to remove from all allocations
        Purpose:
            Scans the entire sector hierarchy of this quad tree
            node and deletes any allocation of the given sector
            found.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00552A40
    =========================================================*/
    void __thiscall RemoveItemFromHierarchy( storageType *sector )
    {
        // Check whether the item even exists.
        // NOTE: this logic may be redundant, but done by R* Games.
        for ( ptrNode_t *node = m_allocations.GetList(); node != NULL; node = node->m_next )
        {
            if ( node->data == sector )
                m_allocations.RemoveItem( sector );
        }

        // Do the same for all sub sectors.
        for ( unsigned int n = 0; n < NUM_QUAD_TREE_SECTORS; n++ )
        {
            CQuadTreeNodeSAInterface *quadNode = m_sectors[n];

            if ( quadNode )
                quadNode->RemoveItemFromHierarchy( sector );
        }
    }

    CBounds2D                   m_bounds;                           // 0, sector this node encorporates
    ptrNodeContainer_t          m_allocations;                      // 16, this field is only allocated if (m_hierDepth == 0)
    CQuadTreeNodeSAInterface*   m_sectors[NUM_QUAD_TREE_SECTORS];   // 20
    unsigned int                m_hierDepth;                        // 36, if zero, allocations are forced into this node

    // Allocations in the quad tree node pool all have to be the same size: 40 bytes.
    // Template definitions must not change this size.
    // This is a rule set by Rockstar Games.
    void* CQuadTreeNodeSAInterface::operator new ( size_t )
    {
        return (*ppQuadTreeNodePool)->Allocate();
    }

    void CQuadTreeNodeSAInterface::operator delete ( void *ptr )
    {
        (*ppQuadTreeNodePool)->Free( (CQuadTreeNodeSAInterface <void>*)ptr );
    }
};

// Module Initialization.
void QuadTree_Init( void );
void QuadTree_Shutdown( void );

#endif //_QUAD_TREE_ENGINE_