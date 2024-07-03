/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientMarker.h
 *  PURPOSE:     Marker entity class
 *
 *****************************************************************************/

#pragma once

#include "CClientStreamElement.h"
#include "CClientMarkerCommon.h"
#include "CClient3DMarker.h"
#include "CClientCheckpoint.h"
#include "CClientCorona.h"
#include "CClientColShape.h"
#include "CClientColCallback.h"

class CClientMarkerManager;

class CClientMarker final : public CClientStreamElement, private CClientColCallback
{
    DECLARE_CLASS(CClientMarker, CClientStreamElement)
    friend class CClientMarkerManager;
    friend class CClientColShape;

public:
    enum eMarkerType
    {
        MARKER_CHECKPOINT,
        MARKER_RING,
        MARKER_CYLINDER,
        MARKER_ARROW,
        MARKER_CORONA,
        MARKER_INVALID = 0xFF,
    };

    CClientMarker(class CClientManager* pManager, ElementID ID, int iMarkerType);
    ~CClientMarker();

    void Unlink();

    void GetPosition(CVector& vecPosition) const;
    void SetPosition(const CVector& vecPosition);
    bool SetMatrix(const CMatrix& matrix);

    void AttachTo(CClientEntity* pEntity) override;
    void SetAttachedOffsets(CVector& vecPosition, CVector& vecRotation) override;

    void DoPulse();

    eClientEntityType GetType() const { return CCLIENTMARKER; }

    CClientMarker::eMarkerType GetMarkerType() const;
    void                       SetMarkerType(CClientMarker::eMarkerType eType);

    class CClient3DMarker*   Get3DMarker();
    class CClientCheckpoint* GetCheckpoint();
    class CClientCorona*     GetCorona();

    bool IsHit(const CVector& vecPosition) const;
    bool IsHit(CClientEntity* pEntity) const;

    bool IsVisible() const;
    void SetVisible(bool bVisible);

    SColor GetColor() const;
    void   SetColor(const SColor color);

    float GetSize() const;
    void  SetSize(float fSize);

    static int  StringToType(const char* szString);
    static bool TypeToString(unsigned int uiType, SString& strOutString);

    static bool IsLimitReached();

    CClientColShape* GetColShape() { return m_pCollision; }

    void Callback_OnCollision(CClientColShape& Shape, CClientEntity& Entity);
    void Callback_OnLeave(CClientColShape& Shape, CClientEntity& Entity);

    virtual CSphere GetWorldBoundingSphere();

    void SetIgnoreAlphaLimits(bool ignore);
    bool AreAlphaLimitsIgnored() const noexcept { return m_pMarker->AreAlphaLimitsIgnored(); };

protected:
    void StreamIn(bool bInstantly);
    void StreamOut();

private:
    void CreateOfType(int iType);

    CClientMarkerManager* m_pMarkerManager;
    CClientMarkerCommon*  m_pMarker;

    CVector             m_vecPosition;
    static unsigned int m_uiStreamedInMarkers;

    CClientColShape* m_pCollision;
};
