/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        MTA10_Server/mods/deathmatch/logic/CTrainTrack.h
*  PURPOSE:     Header file for train node class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once
class CTrainTrackManager;

struct STrackNode
{
    CVector position;
    float railDistance;

    STrackNode(std::int16_t x, std::int16_t y, std::int16_t z, std::uint16_t distance)
    {
        position.fX = 8.0f * x;
        position.fY = 8.0f * y;
        position.fZ = 8.0f * z;
        railDistance = 3.0f * distance; // TODO: Remove distance
    }

    STrackNode(const CVector& pos) : position(pos) {}
};

class CTrainTrack : public CElement
{
public:
    CTrainTrack(CTrainTrackManager* pManager, const std::vector<STrackNode>& nodes, bool linkLastNodes, CElement* pParent, CXMLNode* pNode);
    // TODO: Add move constructor
    virtual ~CTrainTrack();

    bool SetTrackNodePosition(uint nodeIndex, const CVector& position);
    bool GetTrackNodePosition(uint nodeIndex, CVector& position);

    inline const std::vector<STrackNode>& GetNodes() const { return m_Nodes; }
    inline std::size_t GetNumberOfNodes() const { return m_Nodes.size(); }

    inline void SetLastNodesLinked(bool link) { m_LinkLastNodes = link; }
    inline bool GetLastNodesLinked() { return m_LinkLastNodes; }

    // CClientEntity methods
    virtual void Unlink() override {}
    virtual bool ReadSpecialData() override { return false; }

private:
    CTrainTrackManager* m_pManager;

    std::vector<STrackNode> m_Nodes;
    bool m_LinkLastNodes;
};
