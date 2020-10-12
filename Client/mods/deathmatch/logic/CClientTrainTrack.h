/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTrainTrack.h
*  PURPOSE:     Track Track class header
*  DEVELOPERS:  Cazomino05 <>
*
*****************************************************************************/
#pragma once

class CClientEntity;

class CClientTrainTrack : public CClientEntity
{
public:
    CClientTrainTrack(ElementID ID, const std::vector<STrackNode>& trackNodes, bool linkLastNode, bool isDefault);
    virtual ~CClientTrainTrack();

    virtual eClientEntityType GetType() const { return CCLIENTTRAINTRACK; }
    CTrainTrack* GetTrainTrack() { return m_pTrainTrack; }

    bool SetNodePosition(uint nodeInex, const CVector& position);
    bool GetNodePosition(uint nodeInex, CVector& position);

    float GetLength();
    uint GetTrackIndex();

    size_t GetNumberOfNodes();

    void SetLastNodesLinked(bool linked);
    bool IsDefault() { return m_defaultTrack; }

    // Sorta a hack that these are required by CClientEntity
    void Unlink () {}
    void GetPosition(CVector& vecPosition) const {}
    void SetPosition(const CVector& vecPosition) {}

private:
    CTrainTrack* m_pTrainTrack;
    bool         m_defaultTrack = false;
};
