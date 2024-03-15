/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientRadarMarker.h
 *  PURPOSE:     Radar marker entity class header
 *
 *****************************************************************************/

class CClientRadarMarker;

#pragma once

#include "CClientCommon.h"
#include "CClientEntity.h"
#include "CClientRadarMarkerManager.h"
#include <gui/CGUI.h>

#include <game/CMarker.h>

#define RADAR_MARKER_LIMIT 63

class CClientRadarMarker final : public CClientEntity
{
    DECLARE_CLASS(CClientRadarMarker, CClientEntity)
    friend class CClientRadarMarkerManager;

public:
    CClientRadarMarker(class CClientManager* pManager, ElementID ID, short usOrdering = 0, std::uint16_t usVisibleDistance = 16383);
    ~CClientRadarMarker();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTRADARMARKER; };

    void DoPulse();

    void SetPosition(const CVector& vecPosition);
    void GetPosition(CVector& vecPosition) const;

    std::uint16_t GetScale() { return m_usScale; };
    void           SetScale(std::uint16_t usScale);

    SColor GetColor() const { return m_Color; }
    void   SetColor(const SColor color);

    std::uint32_t GetSprite() const { return m_ulSprite; };
    void          SetSprite(std::uint32_t ulSprite);

    bool IsVisible() const { return m_bIsVisible; };
    void SetVisible(bool bVisible);

    void SetDimension(std::uint16_t usDimension) override;
    void RelateDimension(std::uint16_t usDimension);

    short GetOrdering() { return m_sOrdering; }
    void  SetOrdering(short sOrdering);

    std::uint16_t GetVisibleDistance() { return m_usVisibleDistance; }
    void           SetVisibleDistance(std::uint16_t usVisibleDistance) { m_usVisibleDistance = usVisibleDistance; }

    bool IsInVisibleDistance();

private:
    bool Create();
    void InternalCreate();
    void Destroy();

    void CreateMarker();
    void DestroyMarker();

    class CClientRadarMarkerManager* m_pRadarMarkerManager;
    CMarker*                         m_pMarker;

    CVector        m_vecPosition;
    std::uint16_t m_usScale;
    SColor         m_Color;
    std::uint32_t  m_ulSprite;

    bool           m_bIsVisible;
    short          m_sOrdering;
    std::uint16_t m_usVisibleDistance;
};
