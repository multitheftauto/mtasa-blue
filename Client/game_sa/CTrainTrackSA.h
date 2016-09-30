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
#pragma once

struct STrackNode;
class CTrainTrackManagerSA;

//006F6BD0  int GetTrainNodeNearPoint(float x, float y, float z, int* pTrackID) places track ID in *pTrackID and returns node ID
#define FUNC_GetTrainNodeNearPoint              0x6F6BD0

class CTrainTrackSA : public CTrainTrack
{
    friend class CTrainTrackManagerSA;

public:
    CTrainTrackSA                   (uint index, bool linkedLastNode, CTrainTrackManagerSA* pManager);

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

    inline STrackNode* GetTrackNodes() { return m_TrackNodes; }

    void            SetLastNodesLinked              ( bool bLinked );

private:
    STrackNode*   GetRailNode             ( unsigned int uiNode );

    uint                    m_Index;
    bool                    m_LinkLastNodes;
    STrackNode*             m_TrackNodes;
    DWORD                   m_dwNumberOfNodes;
    CTrainTrackManagerSA *  m_pManager;
};
