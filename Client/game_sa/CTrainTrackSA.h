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
public:
    CTrainTrackSA(uint index, const std::vector<STrackNode>& nodes, bool linkLastNode, CTrainTrackManagerSA* pManager);

    inline uint GetTrackIndex() { return m_Index; }
    inline STrackNode* GetTrackNodes() { return m_Nodes.data(); }
    inline float GetLength() { return m_Length; }
    inline std::size_t GetNumberOfNodes() { return m_Nodes.size(); }
    
    void SetLastNodesLinked(bool linked);

    STrackNode* AddNode(const CVector& position);

    void Recalculate();

private:
    uint m_Index;
    bool m_LinkLastNodes;
    CTrainTrackManagerSA* m_pManager;

    std::vector<STrackNode> m_Nodes;
    float m_Length = 0.0f;
};

struct STrackNode
{
    std::int16_t x, y, z; // coordinate times 8
    std::int16_t railDistance; // on-rail distance times

    inline void SetPosition(const CVector& position)
    {
        x = static_cast<std::int16_t>(position.fX);
        y = static_cast<std::int16_t>(position.fY);
        z = static_cast<std::int16_t>(position.fZ);
    }

    inline CVector GetPosition()
    {
        return CVector(x / 8.0f, y / 8.0f, z / 8.0f);
    }

    inline void SetDistance(float distance)
    {
        railDistance = static_cast<std::uint16_t>(distance * 3);
    }

private:
    std::int32_t pad;
};
static_assert(sizeof(STrackNode) == 12, "Size mismatch");
