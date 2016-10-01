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

    // Recalculate lengths
    Recalculate();

    // Don't forget to update references in manager (as std::vector might have relocated the underlying data)
    m_pManager->UpdateTrackData(this);
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

bool CTrainTrackSA::SetNodePosition(uint nodeIndex, const CVector& position)
{
    if (nodeIndex >= m_Nodes.size())
        return false;

    auto& node = m_Nodes[nodeIndex];
    node.SetPosition(position);
    return true;
}

bool CTrainTrackSA::GetNodePosition(uint nodeIndex, CVector& position)
{
    if (nodeIndex >= m_Nodes.size())
        return false;

    auto& node = m_Nodes[nodeIndex];
    position = node.GetPosition();
    return true;
}

void CTrainTrackSA::Recalculate()
{
    // Default our distance to 0
    float distance = 0.0f;

    // Set distance of first node to 0
    m_Nodes.front().SetDistance(0.0f);

    for (auto iter = m_Nodes.begin(); iter != m_Nodes.end() - 1; ++iter)
    {
        auto& node = *iter;
        auto& nextNode = *(iter + 1);
        auto position = node.GetPosition();
        auto nextPosition = nextNode.GetPosition();

        // Add to our distance
        distance += (nextPosition - position).Length(); // TODO: Check if this must be 2D. Yes, it must! Maybe not

        // Update rail distance
        nextNode.SetDistance(distance);
    }

    // Update overall length
    m_Length = distance;

    // Calculate distance from last node to the start again
    /*if (m_LinkLastNodes)
    {
        m_Length += (m_Nodes.front().GetPosition() - m_Nodes.back().GetPosition()).Length();
    }*/
    //auto& startNode = *m_Nodes.begin();
    //auto& endNode = m_Nodes.back();
    //endNode.SetDistance((endNode.GetPosition() - startNode.GetPosition()).Length());
}
