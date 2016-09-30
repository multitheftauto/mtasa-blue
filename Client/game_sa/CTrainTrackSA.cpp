/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTrainTrackSA.cpp
*  PURPOSE:     Train Node class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"
#include "CTrainTrackSA.h"

CTrainTrackSA::CTrainTrackSA(uint index, const std::vector<STrackNode>& nodes, bool linkLastNode, CTrainTrackManagerSA* pManager)
{
    m_Index = index;
    m_Nodes = nodes;
    m_LinkLastNodes = linkLastNode;
    m_pManager = pManager;
}

void CTrainTrackSA::SetLastNodesLinked(bool linked)
{
    // Store the updated status
    bool recalculate = linked != m_LinkLastNodes;
    m_LinkLastNodes = linked;

    if (recalculate)
        Recalculate();
}

STrackNode* CTrainTrackSA::AddNode(const CVector& position)
{
    // Prepare track node
    STrackNode node;
    node.SetPosition(position);

    // Add new node to the vector
    m_Nodes.push_back(node);

    // Recalculate lengths
    Recalculate();

    // Don't forget to update references in manager (as std::vector might have relocated the underlying data)
    m_pManager->UpdateTrackData(this);

    return &m_Nodes.back();
}

void CTrainTrackSA::Recalculate()
{
    // Default our distance to 0
    float distance = 0.0f;

    for (auto iter = m_Nodes.begin(); iter != m_Nodes.end() - 2; ++iter)
    {
        auto& node = *iter;
        auto& nextNode = *(iter + 1);

        auto position = node.GetPosition();
        auto nextPosition = nextNode.GetPosition();

        // Update rail distance
        node.SetDistance(distance);

        // Add to our distance
        distance += (nextPosition - position).Length(); // TODO: Check if this must be 2D
    }

    // Update overall length
    m_Length = distance;

    // Calculate distance from last node to the start again
    auto& startNode = *m_Nodes.begin();
    auto& endNode = *(m_Nodes.end() - 1);
    endNode.SetDistance((endNode.GetPosition() - endNode.GetPosition()).Length());
}
