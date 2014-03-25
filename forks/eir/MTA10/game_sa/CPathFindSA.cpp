/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPathFindSA.cpp
*  PURPOSE:     Game entity path finding logic
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

using namespace PathFind;

/*=========================================================
    CPathFindSAInterface::IsNodeMobile

    Arguments:
        node - the node to check
    Purpose:
        Returns whether the given node is of mobile nature.
        A static node is either a parking node or part of a
        immobile road-block.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0044D3E0
=========================================================*/
bool __thiscall CPathFindSAInterface::IsNodeMobile( PathFind::pathNode *node ) const
{
    return ( node->m_usageType != 1 ) && ( node->m_usageType != 2 );
}

/*=========================================================
    CPathFindSAInterface::GetNodeNumberOfMobileLinks

    Arguments:
        node - the node to check
    Purpose:
        Returns the number of mobile links attached to the
        given node.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004504F0
=========================================================*/
unsigned int __thiscall CPathFindSAInterface::GetNodeNumberOfMobileLinks( PathFind::pathNode *node ) const
{
    unsigned int numLinks = node->m_linkCount;
    unsigned int mobileCount = 0;

    // Iterate through all linked nodes.
    for ( unsigned int n = 0; n < numLinks; n++ )
    {
        PathFind::pathNode *linkedNode = GetLinkedNode( node, n );

        // Increase the count if we found a mobile node.
        if ( linkedNode && IsNodeMobile( linkedNode ) )
            mobileCount++;
    }

    return mobileCount;
}

/*=========================================================
    CPathFindSAInterface::GetNodeRoadLinkedNodes

    Arguments:
        node - the node to get the road connections of
        primaryNode - first node of the road link
        secondaryNode - second node of the road link
        emergencyVehiclesOnly - boolean whether to exclude emergency vehicle roads
                                if true, road links are not returned that are used
                                by emergency vehicles
        useNodeConfig - if true, then doNotAllowEmergency is ignored and the node
                        configuration is used instead
    Purpose:
        Scans the linked nodes of the given node and attempts to
        return two nodes that make up a road. If the given node is
        not a simple road, no nodes are returned.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00452160
=========================================================*/
inline bool FilterEmergencyNode( PathFind::pathNode *node, bool emergencyVehiclesOnly, bool useNodeConfig )
{
    return IsNotNeutral( node->m_trafficSecondary, ( useNodeConfig ? node->m_emergencyVehiclesOnly : emergencyVehiclesOnly ) );
}

void __thiscall CPathFindSAInterface::GetNodeRoadLinkedNodes( pathNode *node, pathNode*& primaryNode, pathNode **secondaryNode, bool emergencyVehiclesOnly, bool useNodeConfig )
{
    // Make sure booleans do not mismatch.
    if ( FilterEmergencyNode( node, emergencyVehiclesOnly, useNodeConfig ) )
        node->m_trafficSecondary = !node->m_trafficSecondary;

    // Set up the node returns
    primaryNode = NULL;

    if ( secondaryNode )
        *secondaryNode = NULL;

    // If the node is part of a junction, we cannot perform.
    if ( GetNodeNumberOfMobileLinks( node ) >= 3 )
        return;

    for ( unsigned int n = 0; n < node->m_linkCount; n++ )
    {
        PathFind::pathNode *linkedNode = GetLinkedNode( node, n );

        // We must have a node and it has to be mobile.
        if ( linkedNode && IsNodeMobile( linkedNode ) )
        {
            // Only perform through emergency vehicle nodes or not.
            if ( FilterEmergencyNode( linkedNode, emergencyVehiclesOnly, useNodeConfig ) )
            {
                // The linked node has to be a road, not a junction.
                if ( GetNodeNumberOfMobileLinks( linkedNode ) < 3 )
                {
                    // Return the node by writing it to the output pointers.
                    // This code will crash if secondaryNode is NULL, even
                    // though code above acknoledges it (R* bug?).
                    if ( primaryNode == NULL )
                        primaryNode = linkedNode;
                    else
                        *secondaryNode = linkedNode;
                }
            }
        }
    }
}

/*=========================================================
    PathFind::pathNode::GetPosition

    Arguments:
        outPos - vector pointer to write the position into
    Purpose:
        Writes the unpacked node position into curPos vector.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00420A10
    Note:
        The node position is encoded using short ints. By dividing
        them through 8, you get the floating point value. This limits
        the maximum coord value to 4096, making path nodes outside
        of (-4096,-4096,4096,4096) quad impossible.
=========================================================*/
void __thiscall PathFind::pathNode::GetPosition( CVector& outPos ) const
{
    outPos = CVector( (float)m_posX / 8, (float)m_posY / 8, (float)m_posZ / 8 );
}

/*=========================================================
    CPathFindSAInterface::CheckRoadValidityInSector (DEBUG)

    Arguments:
        x1, x2, y1, y2, z1, z2 - position coordinates that form an area
        emergencyVehiclesOnly - boolean deciding whether only roads
                                marked for emergency vehicles are
                                affected
                                (only used if useNodeConfig is true)
        vehicleNodesOnly - if true, only vehicle nodes are scanned;
                           otherwise ped nodes are scanned only
        sectorID - index of the sector to scan nodes of
        useNodeConfig - switch to either use node settings or the
                        emergencyVehiclesOnly parameter
    Purpose:
        Goes through all the nodes in the given area and checks their
        road connections for validity. If an invalid road is stumbled
        upon, this function may crash or infinite loop. Most likely,
        this function is a left-over debug entity used by R* when
        they were crafting paths.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00420A10
    Note:
        This function does not have to be called at all.
=========================================================*/
void __thiscall CPathFindSAInterface::CheckRoadValidityInSector(
    float x1, float x2, float y1, float y2, float z1, float z2, bool emergencyVehiclesOnly, bool vehicleNodesOnly, unsigned int sectorID, bool useNodeConfig
)
{
    PathFind::pathNode *sector = m_sectors[sectorID];

    if ( !sector )
        return;

    unsigned int n, max;

    // Decide which nodes to go through.
    if ( vehicleNodesOnly )
    {
        n = 0;
        max = m_numVehicleNodes[sectorID];
    }
    else
    {
        n = m_numVehicleNodes[sectorID];
        max = m_numNodes[sectorID];
    }

    // Loop through the nodes in the selected sector.
    for ( ; n < max; n++ )
    {
        PathFind::pathNode *node = sector + n;

        // Check that the node is in our given sector.
        CVector nodePos;

        node->GetPosition( nodePos );

        if ( nodePos[0] < x1 || nodePos[0] > x2 ||
             nodePos[1] < y1 || nodePos[1] > y2 ||
             nodePos[2] < z1 || nodePos[2] > z2 )
            continue;

        // We require mobile nodes.
        if ( !IsNodeMobile( node ) )
            continue;

        // Filter emergency vehicle roads or not.
        if ( FilterEmergencyNode( node, emergencyVehiclesOnly, useNodeConfig ) )
        {
            // Get the road connections of this node.
            PathFind::pathNode *primary;
            PathFind::pathNode *secondary;

            GetNodeRoadLinkedNodes( node, primary, &secondary, emergencyVehiclesOnly, useNodeConfig );

            // Check the validity of both nodes as roads.
            while ( primary )
                GetNodeRoadLinkedNodes( primary, primary, NULL, emergencyVehiclesOnly, useNodeConfig );

            while ( secondary )
                GetNodeRoadLinkedNodes( secondary, secondary, NULL, emergencyVehiclesOnly, useNodeConfig );
        }
    }
}

/*=========================================================
    CPathFindSAInterface::ReadContainer

    Arguments:
        stream - RenderWare stream containing the pathnode file
        pathNodeID - slot to load the pathnode data into
    Purpose:
        Reads a pathnode sector from a RenderWare stream. There
        are 64 sectors each designating a 750*750 square, since
        the world ranges from -3000,-3000 to 3000,3000.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004529F0
=========================================================*/
struct _pathFileHeader  //size: 20 bytes
{
    unsigned int numNodes;              // 0
    unsigned int numVehicleNodes;       // 4
    unsigned int numPedNodes;           // 8
    unsigned int numNaviNodes;          // 12
    unsigned int numLinks;              // 16
};

__forceinline bool AttemptDataStreamRead( RwStream *stream, void *dst, size_t dataSize )
{
    if ( dst )
    {
        return RwStreamReadBlocks( stream, dst, dataSize ) == dataSize;
    }

    RwStreamSkip( stream, dataSize );
    return true;
}

void __thiscall CPathFindSAInterface::ReadContainer( RwStream *stream, unsigned int pathNodeID )
{
    // The_GTA: I have optimized the structure of this function.
    // Instead of calling the buffer reading function multiple times,
    // I read an entire header at once.
    // Invalid path files are detected and not loaded.
    _pathFileHeader header;

    // We have to read a complete header.
    if ( RwStreamReadBlocks( stream, &header, sizeof(header) ) != sizeof(header) )
        return;

    // Addition: If the node count of every type is not equal to the total amount,
    // the pathnode file is invalid.
    if ( header.numNodes != ( header.numVehicleNodes + header.numPedNodes ) )
        return;

    // Allocate the requested amount of nodes.
    PathFind::pathNode *nodes;

    if ( header.numNodes != 0 )
    {
        // Allocate the path node structures
        size_t nodeSize = sizeof(*nodes) * header.numNodes;

        nodes = (PathFind::pathNode*)RwMalloc( nodeSize, 0 );

        // Bugfix: if we do not have enough memory, return here.
        if ( !nodes )
            return;

        // Read the data from our stream.
        if ( RwStreamReadBlocks( stream, nodes, nodeSize ) != nodeSize )
        {
            RwFree( nodes );
            return;
        }
    }
    else
    {
        // We allocate an empty structure to hopefully save the engine!
        nodes = new (RwMalloc( sizeof(*nodes), 0 )) PathFind::pathNode();

        // Bugfix: if we do not have enough memory, return here.
        if ( !nodes )
            return;
    }

    // Read the navi nodes; they are not required.
    PathFind::naviNode *naviNodes;

    if ( header.numNaviNodes != 0 )
    {
        // Allocate the navi node structures for our sector.
        size_t nodeSize = sizeof(*naviNodes) * header.numNaviNodes;

        naviNodes = (PathFind::naviNode*)RwMalloc( nodeSize, 0 );

        // If the allocation succeeded, we read the node information
        // from our stream.
        // Only save the node information if the read was successful.
        // We skip the bytes if we failed to allocate navi nodes.
        if ( !AttemptDataStreamRead( stream, naviNodes, nodeSize ) )
        {
            RwFree( naviNodes );

            naviNodes = NULL;
        }
    }
    else
        naviNodes = NULL;

    // Read linking information.
    PathFind::pathDynamicLinkContainer *dynamicLinks;
    PathFind::naviNodeLink *naviNodeLink;
    PathFind::nodeLinkLength *nodeLinkLength;
    PathFind::unknownData *unknown;
    
    if ( header.numLinks != 0 )
    {
        // Allocate the structures.
        size_t dynamicLinkSize = sizeof(*dynamicLinks) + header.numLinks * sizeof(unsigned int);
        size_t naviNodeLinkSize = sizeof(*naviNodeLink) * header.numLinks;
        size_t nodeLinkLengthSize = sizeof(*nodeLinkLength) + header.numLinks;
        size_t unknownDataSize = sizeof(*unknown) + header.numLinks;

        dynamicLinks = (PathFind::pathDynamicLinkContainer*)RwMalloc( dynamicLinkSize, 0 );
        naviNodeLink = (PathFind::naviNodeLink*)RwMalloc( naviNodeLinkSize, 0 );
        nodeLinkLength = (PathFind::nodeLinkLength*)RwMalloc( nodeLinkLengthSize, 0 );
        unknown = (PathFind::unknownData*)RwMalloc( unknownDataSize, 0 );

        // Read from our stream.
        // The_GTA: only save the readed bytes if allocation of the structures succeeded.
        // We can use RwStreamSkip if allocation failed.
        AttemptDataStreamRead( stream, dynamicLinks, dynamicLinkSize );
        AttemptDataStreamRead( stream, naviNodeLink, naviNodeLinkSize );
        AttemptDataStreamRead( stream, nodeLinkLength, nodeLinkLengthSize );
        AttemptDataStreamRead( stream, unknown, unknownDataSize );
    }
    else
    {
        // We do not need these structures if they do not exist.
        dynamicLinks = NULL;
        naviNodeLink = NULL;
        nodeLinkLength = NULL;
        unknown = NULL;
    }

    // Loop through all nodes of our sector and manipulate flags.
    for ( unsigned int n = 0; n < header.numNodes; n++ )
    {
        PathFind::pathNode *node = nodes + n;

        // Make sure flags do not contradict themselves.
        if ( IsNotNeutral( node->m_emergencyVehiclesOnly, node->m_trafficSecondary ) )
            node->m_emergencyVehiclesOnly = !node->m_emergencyVehiclesOnly;
    }

    // Once everything succeeded, we set the internal structure.
    // The_GTA: in the original code, node data was applied at time of retrieval.
    // This is a safer approach, since we include detection of invalid
    // nodes.
    m_numNodes[pathNodeID] = header.numNodes;
    m_numVehicleNodes[pathNodeID] = header.numVehicleNodes;
    m_numPedNodes[pathNodeID] = header.numPedNodes;
    m_numNaviNodes[pathNodeID] = header.numNaviNodes;
    m_numLinks[pathNodeID] = header.numLinks;
    m_sectors[pathNodeID] = nodes;
    m_naviSectors[pathNodeID] = naviNodes;
    m_links[pathNodeID] = dynamicLinks;
    m_naviLinks[pathNodeID] = naviNodeLink;
    m_linkLengths[pathNodeID] = nodeLinkLength;
    m_unknownData[pathNodeID] = unknown;

#ifdef PERFORM_PATHNODE_DEBUG
    // Check the validity of all roads in map sectors.
    // We must check the validity, because nodes will be heavily used in-game.
    for ( unsigned int n = 0; n < m_mapAreaCount; n++ )
    {
        const PathFind::mapArea& area = m_mapAreas[n];

        // Go through all the roads and if any of them is invalid,
        // either crash or infinite loop. Leave it to the developer
        // to give feedback about the pathnode issue.
        CheckRoadValidityInSector(
            area.m_minX, area.m_maxX, area.m_minY, area.m_maxY, area.m_minZ, area.m_maxZ, area.m_emergencyOnly, area.m_vehiclesOnly, pathNodeID, false
        );
    }
#endif //PERFORM_PATHNODE_DEBUG
}

/*=========================================================
    CPathFindSAInterface::FreeContainer

    Arguments:
        pathNodeID - slot which contains valid pathnode data
    Purpose:
        Frees all resources from a previously loaded pathnode
        sector.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0044D0F0
=========================================================*/
void __thiscall CPathFindSAInterface::FreeContainer( unsigned int pathNodeID )
{
    // Free memory.
    if ( PathFind::pathNode *sector = m_sectors[pathNodeID] )
        RwFree( sector );

    if ( PathFind::naviNode *sector = m_naviSectors[pathNodeID] )
        RwFree( sector );

    if ( PathFind::pathDynamicLinkContainer *dynLinks = m_links[pathNodeID] )
        RwFree( dynLinks );

    if ( PathFind::naviNodeLink *naviLinks = m_naviLinks[pathNodeID] )
        RwFree( naviLinks );

    if ( PathFind::nodeLinkLength *linkLengths = m_linkLengths[pathNodeID] )
        RwFree( linkLengths );

    if ( PathFind::unknownData *unk = m_unknownData[pathNodeID] )
        RwFree( unk );

    // Zero out all data.
    m_sectors[pathNodeID] = NULL;
    m_naviSectors[pathNodeID] = NULL;
    m_links[pathNodeID] = NULL;
    m_naviLinks[pathNodeID] = NULL;
    m_linkLengths[pathNodeID] = NULL;
    m_unknownData[pathNodeID] = NULL;
}

/*=========================================================
    CPathFindSAInterface::IsSectorCarNodeInRadius

    Arguments:
        x, y - coordinates of the collision circle
        radius - radius of the collision circle
    Purpose:
        Goes through all car nodes of all map sectors and
        and returns whether one is inside of the given
        circle/radius.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00450DE0
=========================================================*/
bool __thiscall CPathFindSAInterface::IsSectorCarNodeInRadius( float x, float y, float radius ) const
{
    // Go through all map sectors that contain vehicle and ped nodes.
    // You might aswell call them "real" or "main".
    for ( unsigned int n = 0; n < REAL_MAX_PATH_SECTORS; n++ )
    {
        PathFind::pathNode *sector = m_sectors[n];

        if ( !sector )
            continue;

        // Execute for all vehicles nodes, which come before the ped nodes.
        unsigned int max = m_numVehicleNodes[n];

        for ( unsigned int i = 0; i < max; i++ )
        {
            PathFind::pathNode *node = sector + i;

            // Do not execute for boats.
            if ( node->m_boats )
                continue;

            // Return the first car node that resides in our circle.
            CVector nodePos;
            node->GetPosition( nodePos );

            if ( ( CVector2D( x, y ) - CVector2D( nodePos[0], nodePos[1] ) ).Length() < radius )
                return true;
        }
    }

    return false;
}

/*=========================================================
    CPathFindSAInterface::GetPathNodeFromAddress

    Arguments:
        address - struct that holds node information
    Purpose:
        Returns the pointer to the node that is specified by
        the given address struct. Returns NULL if no such
        node is valid.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00420AC0
=========================================================*/
PathFind::pathNode* __thiscall CPathFindSAInterface::GetPathNodeFromAddress( const CNodeAddress address )
{
    return m_sectors[ address.sRegion ] + address.sIndex;
}

CNodeAddress * CPathFindSA::FindNthNodeClosestToCoors ( CVector * vecCoors, int iNodeNumber, int iType, CNodeAddress * pNodeAddress, float fDistance )
{
    DWORD dwFunc = FUNC_FindNthNodeClosestToCoors;
    
    CNodeAddress node;
    CNodeAddress * nodeptr; 

    _asm
    {
        lea     eax, node
        push    eax
        push    0
        push    iType  // Type : 0 - cars, 1 - boats
        push    iNodeNumber // Nth
        push    0
        push    0
        push    fDistance
        push    0
        mov     eax, vecCoors
        push    [eax+8]
        push    [eax+4]
        push    [eax]
        lea     eax, nodeptr
        push    eax
        mov     ecx, CLASS_CPathFind
        call    dwFunc
    }

    *pNodeAddress = node;
    return pNodeAddress;
}

CPathNode * CPathFindSA::GetPathNode ( CNodeAddress * node )
{
    DWORD dwFunc = FUNC_FindNodePointer;
    if ( node->sRegion >= 0 && node->sIndex >= 0 )
    {
        // Here the "*" stands for: resolve internal class to virtual one.
        return *PathFind::GetInterface().GetPathNodeFromAddress( *node );
    }
    return NULL;
}

CVector * CPathFindSA::GetNodePosition ( CPathNode * pNode, CVector * pPosition)
{
    // *pNode stands for: resolve virtual class name to internal one.
    PathFind::pathNode *node = *pNode;

    node->GetPosition( *pPosition );

    return pPosition;
}

CVector * CPathFindSA::GetNodePosition ( CNodeAddress * pNode, CVector * pPosition )
{
    CPathNode * pPathNode = GetPathNode ( pNode );
    return GetNodePosition ( pPathNode, pPosition );
}

void CPathFindSA::SwitchRoadsOffInArea ( CVector * vecAreaCorner1, CVector * vecAreaCorner2, bool bEnable )
{
    float fX1 = vecAreaCorner1->fX;
    float fY1 = vecAreaCorner1->fY;
    float fZ1 = vecAreaCorner1->fZ;

    float fX2 = vecAreaCorner2->fX;
    float fY2 = vecAreaCorner2->fY;
    float fZ2 = vecAreaCorner2->fZ;

    DWORD dwEnable = !bEnable;

    DWORD dwFunc = FUNC_SwitchRoadsOffInArea;

    _asm
    {
        mov     ecx, CLASS_CPathFind
        push    0
        push    1
        push    dwEnable
        push    fZ2
        push    fZ1
        push    fY2
        push    fY1
        push    fX2
        push    fX1
        call    dwFunc
    }
}

void CPathFindSA::SwitchPedRoadsOffInArea ( CVector * vecAreaCorner1, CVector * vecAreaCorner2, bool bEnable )
{
    float fX1 = vecAreaCorner1->fX;
    float fY1 = vecAreaCorner1->fY;
    float fZ1 = vecAreaCorner1->fZ;

    float fX2 = vecAreaCorner2->fX;
    float fY2 = vecAreaCorner2->fY;
    float fZ2 = vecAreaCorner2->fZ;

    DWORD dwEnable = !bEnable;

    DWORD dwFunc = FUNC_SwitchPedRoadsOffInArea;

    _asm
    {
        mov     ecx, CLASS_CPathFind
        push    0
        push    dwEnable
        push    fZ2
        push    fZ1
        push    fY2
        push    fY1
        push    fX2
        push    fX1
        call    dwFunc
    }
}

void CPathFindSA::SetPedDensity ( float fPedDensity )
{
    MemPutFast < float > ( VAR_PedDensityMultiplier, fPedDensity );
}

void CPathFindSA::SetVehicleDensity ( float fVehicleDensity )
{
    MemPutFast < float > ( VAR_CarDensityMultiplier, fVehicleDensity );
}

void CPathFindSA::SetMaxPeds ( int iMaxPeds )
{
    MemPutFast < int > ( VAR_MaxNumberOfPedsInUse, iMaxPeds );
}

void CPathFindSA::SetMaxPedsInterior ( int iMaxPeds )
{
     MemPutFast < int > ( VAR_NumberOfPedsInUseInterior, iMaxPeds );
}

void CPathFindSA::SetMaxVehicles ( int iMaxVehicles )
{
    MemPutFast < int > ( VAR_MaxNumberOfCarsInUse, iMaxVehicles );
}

void CPathFindSA::SetAllRandomPedsThisType ( int iType )
{
    MemPutFast < int > ( VAR_m_AllRandomPedsThisType, iType );
}
