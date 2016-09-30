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
#include <game/CTrainTrack.h>

struct STrackNode;
class CTrainTrackManagerSA;

class CTrainTrackSA : public CTrainTrack
{
public:
    CTrainTrackSA(uint index, const std::vector<STrackNode>& nodes, bool linkLastNode, CTrainTrackManagerSA* pManager);

    virtual uint GetIndex() override { return m_Index; }

    inline STrackNode* GetNodesData() { return m_Nodes.data(); }
    virtual inline float GetLength() override { return m_Length; }
    inline std::size_t GetNumberOfNodes() { return m_Nodes.size(); }
    
    virtual void SetLastNodesLinked(bool linked) override;

    virtual STrackNode* AddNode(const CVector& position) override;
    virtual const std::vector<STrackNode>& GetNodes() const override { return m_Nodes; }

    virtual bool SetNodePosition(uint nodeIndex, const CVector& position) override;
    virtual bool GetNodePosition(uint nodeIndex, CVector& position) override;

    void Recalculate();

private:
    uint m_Index;
    bool m_LinkLastNodes;
    CTrainTrackManagerSA* m_pManager;

    std::vector<STrackNode> m_Nodes;
    float m_Length = 0.0f;
};

