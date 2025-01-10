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
    CClientRadarMarker(class CClientManager* pManager, ElementID ID, short usOrdering = 0, unsigned short usVisibleDistance = 16383);
    ~CClientRadarMarker();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTRADARMARKER; };

    void DoPulse();

    void SetPosition(const CVector& vecPosition);
    void GetPosition(CVector& vecPosition) const;

    unsigned short GetScale() const noexcept { return m_usScale; }
    void           SetScale(unsigned short usScale);

    SColor GetColor() const noexcept { return m_Color; }
    void   SetColor(const SColor color);

    unsigned long GetSprite() const noexcept { return m_ulSprite; }
    void          SetSprite(unsigned long ulSprite);

    bool IsVisible() const { return m_bIsVisible; };
    void SetVisible(bool bVisible);

    void SetDimension(unsigned short usDimension) override;
    void RelateDimension(unsigned short usDimension);

    short GetOrdering() const noexcept { return m_sOrdering; }
    void  SetOrdering(short sOrdering);

    unsigned short GetVisibleDistance() const noexcept { return m_usVisibleDistance; }
    void           SetVisibleDistance(unsigned short usVisibleDistance) { m_usVisibleDistance = usVisibleDistance; }

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
    unsigned short m_usScale;
    SColor         m_Color;
    unsigned long  m_ulSprite;

    bool           m_bIsVisible;
    short          m_sOrdering;
    unsigned short m_usVisibleDistance;
};
