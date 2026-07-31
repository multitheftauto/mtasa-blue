/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTrainTrack.h
 *  PURPOSE:     Train track entity class header
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"

class CClientTrainTrackManager;

class CClientTrainTrack final : public CClientEntity
{
    DECLARE_CLASS(CClientTrainTrack, CClientEntity)
    friend class CClientTrainTrackManager;

public:
    CClientTrainTrack(class CClientManager* pManager, ElementID ID, const std::vector<CVector>& nodePositions);
    ~CClientTrainTrack();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTTRAINTRACK; }

    void GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; }
    void SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; }

    // The byte-sized track ID the game's native train code understands; 0xFF if we ran out of slots
    std::uint8_t GetGameTrackID() const noexcept;

private:
    class CClientManager*     m_pManager;
    CClientTrainTrackManager* m_pTrainTrackManager;

    std::vector<CVector> m_NodePositions;
    CVector              m_vecPosition;
    class CTrainTrack*   m_pGameTrainTrack;
};
