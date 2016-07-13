/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTrainTrackSA.h
*  PURPOSE:     Header file for train node class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TRAINNODESA
#define __CGAMESA_TRAINNODESA

class CTrainTrackManagerSA;

//006F6BD0  int GetTrainNodeNearPoint(float x, float y, float z, int* pTrackID) places track ID in *pTrackID and returns node ID
#define FUNC_GetTrainNodeNearPoint              0x6F6BD0


#define NUM_RAILTRACKS                          4
#define ARRAY_NumRailTrackNodes                 0xC38014    // NUM_RAILTRACKS dwords
#define ARRAY_RailTrackNodePointers             0xC38024    // NUM_RAILTRACKS pointers to arrays of SRailNode

typedef struct
{
    short sX;               // x coordinate times 8
    short sY;               // y coordinate times 8
    short sZ;               // z coordinate times 8
    WORD  sRailDistance;    // on-rail distance times 3
    WORD  padding;
    void Init ( short sX, short sY, short sZ, WORD sRailDistance, WORD padding )
    {
        this->sX = sX;
        this->sY = sY;
        this->sZ = sZ;
        this->sRailDistance = sRailDistance;
        this->padding = padding;
    }
} SRailNodeSA;
C_ASSERT ( sizeof ( SRailNodeSA ) == 0xA );

class CTrainTrackSA : public CTrainTrack
{
    friend class CTrainTrackManagerSA;
public:
                    CTrainTrackSA                   ( unsigned char ucTrackID, bool bLinkedLastNode, CTrainTrackManagerSA * pManager );

private:
    void            Recalculate                     ( );
    void            Initialise                      ( void );
    bool            SetLength                       ( float fLength );
    float           GetLength                       ( void );
    void            DumpTrainData                   ( unsigned char ucTrackID );
    void            Reset                           ( void );
public:

    bool            SetRailNodePosition             ( unsigned int uiNode, CVector vecPosition );
    bool            GetRailNodePosition             ( unsigned int uiNode, CVector& vecPosition );
    bool            GetRailNodeDistance             ( unsigned int uiNode, float& fDistance );

    bool            SetTrackLength                  ( float fLength ); 
    float           GetTrackLength                  ( void );

    bool            SetNumberOfNodes                ( unsigned int uiNumberOfNodes );
    unsigned int    GetNumberOfNodes                ( void )                                            { return m_dwNumberOfNodes; };

    unsigned char   GetTrackID                      ( void )                                            { return m_ucTrackID; };

    void            SetLastNodesLinked              ( bool bLinked );

private:
    SRailNodeSA *   GetRailNode             ( unsigned int uiNode );

    bool                    m_bLinkLastNodes;
    bool                    m_bInitialised;
    SRailNodeSA *           m_pRailNodes;
    DWORD                   m_dwNumberOfNodes;
    unsigned char           m_ucTrackID;
    CTrainTrackManagerSA *  m_pManager;
};

#endif
