/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPathFindSA.h
*  PURPOSE:     Header file for game entity path finding class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PATHFIND
#define __CGAMESA_PATHFIND

#include <game/CPathFind.h>

#define PATH_SECTOR_LENGTH                      750
#define PATH_SECTORS_ON_LINE                    ((WORLD_BOUNDS * 2) / PATH_SECTOR_LENGTH)   // built in: 8
#define REAL_MAX_PATH_SECTORS                   (PATH_SECTORS_ON_LINE * PATH_SECTORS_ON_LINE)
#define MAX_PATH_SECTORS                        (REAL_MAX_PATH_SECTORS + PATH_SECTORS_ON_LINE)

#define MAX_PATH_MAP_AREAS                      640

#ifdef _DEBUG
// R* flag: Comment this out if path nodes should not be debugged.
#define PERFORM_PATHNODE_DEBUG
#endif //_DEBUG

#define FUNC_FindNthNodeClosestToCoors          0x44F8C0
#define FUNC_FindNodePointer                    0x420AC0
#define FUNC_SwitchRoadsOffInArea               0x452C80
#define FUNC_SwitchPedRoadsOffInArea            0x452F00

#define FUNC_CPathNode_GetCoors                 0x420A10

#define CLASS_CPathFind                         0x96F050

#define VAR_PedDensityMultiplier                0x8D2530
#define VAR_m_AllRandomPedsThisType             0x8D2534
#define VAR_MaxNumberOfPedsInUse                0x8D2538
#define VAR_NumberOfPedsInUseInterior           0x8D253C
#define VAR_CarDensityMultiplier                0x8A5B20
#define VAR_MaxNumberOfCarsInUse                0x8A5B24

// http://gta.wikia.com/Paths_(GTA_SA)
// http://www.gtamodding.com/index.php?title=Paths_(GTA_SA)
namespace PathFind
{
    // The_GTA: I use namespaces to seperate internal logic from the main logic.
    // The structures in namespaces often cannot be directly exported to higher level modules.
    // They are not classes because they are very dynamically allocated containers (see comments)
    // or too simple (naviNodeLink).

    // GTA:SA internal data.
    struct dynamicLink
    {
        unsigned short          m_areaID : 16;          // 0
        unsigned short          m_nodeID : 16;          // 2
    };
    
    struct pathDynamicLinkContainer
    {
        // from here this structure extends by CPathFindSAInterface::m_numLinks[pathSectorID] * 4
        inline dynamicLink&     GetLink( unsigned int index )
        {
            return *( (dynamicLink*)this + index );
        }

        BYTE                    m_pad[0x300];           // ?, some sort of filler data
    };

    struct naviNodeLink
    {
        unsigned short          m_nodeID : 10;
        unsigned short          m_areaID : 6;
    };

    struct nodeLinkLength
    {
        // from here this structure extends by CPathFindSAInterface::m_numLinks[pathSectorID] * 1

        BYTE                    m_pad[0xC0];            // 0
    };

    struct unknownData
    {
        // from here this structure extends by CPathFindSAInterface::m_numLinks[pathSectorID] * 1

        BYTE                    m_pad[0xC0];            // 0
    };

    struct pathNode //size: 28
    {
        // Bugfix: provide a constructor for empty nodes.
        pathNode( void )
        {
            m_memAddress = 0;
            m_unknown = 0;
            m_posX = 0;
            m_posY = 0;
            m_posZ = 0;
            m_unknown2 = 0x7FFE;
            m_linkID = 0;
            m_areaID = 0;
            m_nodeID = 0;
            m_pathWidth = 0;
            m_nodeType = 0;
            m_flags = 0;
        }

        // Built-in pathnode functions.
        void __thiscall             GetPosition( CVector& outPos ) const;

        unsigned int                m_memAddress;           // 0, unused pointers used by path compiler
        unsigned int                m_unknown;              // 4
        short                       m_posX;                 // 8, packed floating-point position (turn to double, then / 8)
        short                       m_posY;                 // 10
        short                       m_posZ;                 // 12
        short                       m_unknown2;             // 14
        unsigned short              m_linkID;               // 16, starting index inside of PathFind::pathDynamicLinkContainer
        unsigned short              m_areaID;               // 18, should match the pathnode sector index
        unsigned short              m_nodeID;               // 20
        unsigned char               m_pathWidth;            // 22, packed path width
        unsigned char               m_nodeType;             // 23, defines the node group (car, boat, ped, ...)

        union
        {
            unsigned int            m_flags;                // 24

            struct
            {
                unsigned char       m_linkCount : 4;                // 24
                unsigned char       m_trafficLevel : 2;
                bool                m_roadBlocks : 1;
                bool                m_boats : 1;

                bool                m_emergencyVehiclesOnly : 1;    // 25
                bool                m_unknownFlag4 : 1;
                bool                m_unknownFlag5 : 1;
                bool                m_unknownFlag1 : 1;
                bool                m_isRural : 1;
                bool                m_isHighway : 1;
                bool                m_unknownFlag2 : 1;
                bool                m_unknownFlag3 : 1;

                unsigned char       m_spawnProbability : 4;         // 26
                unsigned char       m_usageType : 4;
            };

            struct
            {
                unsigned char       m_pad : 4;
                bool                m_trafficPrimary : 1;
                bool                m_trafficSecondary : 1;
            };
        };
    };

#pragma pack(push)
#pragma pack(1)
    struct naviNode
    {
        short                       m_posX;                 // 0
        short                       m_posY;                 // 2
        unsigned short              m_areaID;               // 4
        unsigned short              m_nodeID;               // 6
        char                        m_dirX;                 // 8
        char                        m_dirY;                 // 9
        unsigned int                m_flags;                // 10
    };

    struct mapArea  //size: 28
    {
        float                       m_minX, m_maxX;         // 0
        float                       m_minY, m_maxY;         // 8
        float                       m_minZ, m_maxZ;         // 16
        bool                        m_emergencyOnly;        // 24
        bool                        m_vehiclesOnly;         // 25
        BYTE                        m_pad[2];               // 26
    };
#pragma pack(pop)
};

class CPathFindSAInterface
{
public:
    // Internal GTA:SA functions.
    bool __thiscall             IsNodeMobile( PathFind::pathNode *node ) const;
    unsigned int __thiscall     GetNodeNumberOfMobileLinks( PathFind::pathNode *node ) const;

    void __thiscall GetNodeRoadLinkedNodes(
        PathFind::pathNode *node, PathFind::pathNode*& primaryNode, PathFind::pathNode **secondaryNode, bool emergencyVehiclesOnly, bool useNodeConfig
    );

    inline PathFind::pathDynamicLinkContainer& __thiscall GetNodeLinkContainer( PathFind::pathNode *node ) const
    {
        return *m_links[node->m_areaID];
    }

    inline PathFind::pathNode* __thiscall GetLinkedNode( PathFind::pathNode *node, unsigned int index ) const
    {
        // Get the link container of our node.
        const PathFind::dynamicLink& dynLink = GetNodeLinkContainer( node ).GetLink( index + node->m_linkID );

        // Obtain the area the link points to.
        PathFind::pathNode *sector = m_sectors[dynLink.m_areaID];

        // The sector which the link points to does not have to be loaded.
        // Only return the linked node if the sector is loaded.
        return ( sector != NULL ) ? ( sector + dynLink.m_nodeID ) : NULL;
    }

    void __thiscall CheckRoadValidityInSector(
        float x1, float x2, float y1, float y2, float z1, float z2, bool emergencyVehiclesOnly, bool vehicleNodesOnly, unsigned int sectorID, bool useNodeConfig
    );

    void __thiscall             ReadContainer( RwStream *stream, unsigned int pathNodeID );
    void __thiscall             FreeContainer( unsigned int pathNodeID );

    bool __thiscall             IsSectorCarNodeInRadius( float x, float y, float radius ) const;

    // The engine actually has support for 72 pathnode sectors.
    // Due to limitations while storing the navi links, the actual limit is 64.
    BYTE                                    m_pad[2052];                            // 0

    PathFind::pathNode*                     m_sectors[MAX_PATH_SECTORS];            // 2052, arrays of path nodes for every sector
    PathFind::naviNode*                     m_naviSectors[MAX_PATH_SECTORS];        // 2340
    PathFind::pathDynamicLinkContainer*     m_links[MAX_PATH_SECTORS];              // 2628
    PathFind::nodeLinkLength*               m_linkLengths[MAX_PATH_SECTORS];        // 2916
    PathFind::unknownData*                  m_unknownData[MAX_PATH_SECTORS];        // 3204
    PathFind::naviNodeLink*                 m_naviLinks[MAX_PATH_SECTORS];          // 3492

    BYTE                                    m_pad3[224];                            // 3780

    unsigned int                            m_numNodes[MAX_PATH_SECTORS];           // 4004
    unsigned int                            m_numVehicleNodes[MAX_PATH_SECTORS];    // 4292
    unsigned int                            m_numPedNodes[MAX_PATH_SECTORS];        // 4580
    unsigned int                            m_numNaviNodes[MAX_PATH_SECTORS];       // 4868
    unsigned int                            m_numLinks[MAX_PATH_SECTORS];           // 5156

    BYTE                                    m_pad2[8228];                           // 5444

    unsigned int                            m_mapAreaCount;                         // 13672
    PathFind::mapArea                       m_mapAreas[MAX_PATH_MAP_AREAS];         // 13676
};

namespace PathFind
{
    inline CPathFindSAInterface&    GetInterface( void )
    {
        return *(CPathFindSAInterface*)CLASS_CPathFind;
    }
};

class CPathFindSA : public CPathFind
{
public:
    CNodeAddress *  FindNthNodeClosestToCoors ( CVector * vecCoors, int iNodeNumber, int iType, CNodeAddress * pNodeAddress, float fDistance );
    CPathNode *     GetPathNode ( CNodeAddress * node );
    CVector *       GetNodePosition ( CPathNode * pNode, CVector * pPosition);
    CVector *       GetNodePosition ( CNodeAddress * pNode, CVector * pPosition );
    void            SwitchRoadsOffInArea ( CVector * vecAreaCorner1, CVector * vecAreaCorner2, bool bEnable = false );
    void            SwitchPedRoadsOffInArea ( CVector * vecAreaCorner1, CVector * vecAreaCorner2, bool bEnable = false );

    // Belong elsewhere - cpopulation and ccarctrl
    void            SetPedDensity ( float fPedDensity );
    void            SetVehicleDensity ( float fVehicleDensity );
    void            SetMaxPeds ( int iMaxPeds );
    void            SetMaxPedsInterior ( int iMaxPeds );
    void            SetMaxVehicles ( int iMaxVehicles );
    void            SetAllRandomPedsThisType ( int iType );
};

#endif
