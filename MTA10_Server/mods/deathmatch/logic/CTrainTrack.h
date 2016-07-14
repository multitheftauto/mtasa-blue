/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTrainTrack.h
*  PURPOSE:     Header file for train node class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

class CTrainTrackManager;

#define NUM_RAILTRACKS                          4

typedef struct
{
    short sX;               // x coordinate times 8
    short sY;               // y coordinate times 8
    short sZ;               // z coordinate times 8
    float fRailDistance;    // on-rail distance ( leave the client to calculate this )
    void Init ( short sX, short sY, short sZ, unsigned short sRailDistance, WORD padding )
    {
        this->sX = sX;
        this->sY = sY;
        this->sZ = sZ;
        this->fRailDistance = sRailDistance;
    }
} SRailNode;

class CTrainTrack : public CElement
{
    friend class CTrainTrackManager;
public:
                    CTrainTrack                     ( unsigned char ucTrackID, CTrainTrackManager * pManager, CElement * pParent, CXMLNode * pNode );
                    ~CTrainTrack                    ( void );

private:
    void            Recalculate                     ( );
    void            Initialise                      ( void );
    bool            SetLength                       ( float fLength );
    float           GetLength                       ( void );
    void            Reset                           ( void );
public:

    bool            SetRailNodePosition             ( unsigned int uiNode, CVector vecPosition );
    bool            GetRailNodePosition             ( unsigned int uiNode, CVector& vecPosition );

    bool            SetTrackLength                  ( float fLength ); 
    float           GetTrackLength                  ( void );

    bool            SetNumberOfNodes                ( unsigned int uiNumberOfNodes );
    unsigned int    GetNumberOfNodes                ( void )                                            { return m_dwNumberOfNodes; };

    unsigned char   GetTrackID                      ( void )                                            { return m_ucTrackID; };

    unsigned int    GetRailNodeSize                 ( void )                                            { return m_RailNodes.size(); };

    void            Unlink                          ( void )                                            { };
    bool            ReadSpecialData                 ( void )                                            { return false; };

    void            SetLastNodesLinked              ( bool bLinked )                                    { m_bLinkLastNodes = bLinked; };
    bool            GetLastNodesLinked              ( void )                                            { return m_bLinkLastNodes; };


private:
    SRailNode *     GetRailNode             ( unsigned int uiNode );

    bool                            m_bInitialised;
    DWORD                           m_dwNumberOfNodes;
    unsigned char                   m_ucTrackID;
    CTrainTrackManager *            m_pManager;
    std::map < int, SRailNode * >   m_RailNodes;
    bool                            m_bLinkLastNodes;
};
