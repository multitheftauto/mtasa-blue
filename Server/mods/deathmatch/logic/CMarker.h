/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMarker.h
 *  PURPOSE:     Marker entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include "CColCallback.h"
#include "CColManager.h"
#include "CColShape.h"
#include "CEvents.h"
#include "CPerPlayerEntity.h"

class CMarker : public CPerPlayerEntity, private CColCallback
{
    friend class CMarkerManager;

public:
    enum
    {
        TYPE_CHECKPOINT,
        TYPE_RING,
        TYPE_CYLINDER,
        TYPE_ARROW,
        TYPE_CORONA,
        TYPE_INVALID = 0xFF,
    };

    enum
    {
        ICON_NONE,
        ICON_ARROW,
        ICON_FINISH,
        ICON_INVALID,
    };

public:
    CMarker(class CMarkerManager* pMarkerManager, CColManager* pColManager, CElement* pParent);
    ~CMarker();
    CElement* Clone(bool* bAddEntity, CResource* pResource) override;

    void Unlink();

    bool           HasTarget() { return m_bHasTarget; };
    const CVector& GetTarget() { return m_vecTarget; };
    unsigned char  GetMarkerType() { return m_ucType; };
    float          GetSize() { return m_fSize; };
    SColor         GetColor() const { return m_Color; };
    unsigned char  GetIcon() { return m_ucIcon; }

    void SetPosition(const CVector& vecPosition);
    void SetTarget(const CVector* pTargetVector);
    void SetMarkerType(unsigned char ucType);
    void SetSize(float fSize);
    void SetColor(const SColor color);

    void SetIcon(unsigned char ucIcon);

    CColShape* GetColShape() { return m_pCollision; }

    virtual CSphere GetWorldBoundingSphere();

    void SetIgnoreAlphaLimits(bool ignore) noexcept { m_ignoreAlphaLimits = ignore; };
    bool AreAlphaLimitsIgnored() const noexcept { return m_ignoreAlphaLimits; };

    SColor GetTargetArrowColor() const noexcept { return m_TargetArrowColor; };
    float  GetTargetArrowSize() const noexcept { return m_TargetArrowSize; };
    void SetTargetArrowProperties(const SColor color, float size) noexcept;

protected:
    bool ReadSpecialData(const int iLine) override;

private:
    void Callback_OnCollision(CColShape& Shape, CElement& Element);
    void Callback_OnLeave(CColShape& Shape, CElement& Element);
    void Callback_OnCollisionDestroy(CColShape* pShape);

    void UpdateCollisionObject(unsigned char ucOldType);

    class CMarkerManager* m_pMarkerManager;
    CColManager*          m_pColManager;
    bool                  m_bHasTarget;
    CVector               m_vecTarget;
    unsigned char         m_ucType;
    float                 m_fSize;
    SColor                m_Color;
    unsigned char         m_ucIcon;
    bool                  m_ignoreAlphaLimits;
    SColor                m_TargetArrowColor;
    float                 m_TargetArrowSize;

    CColShape* m_pCollision;
};
