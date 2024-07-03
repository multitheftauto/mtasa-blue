/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientMarkerCommon.h
 *  PURPOSE:     Marker entity base class
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"

class CClientMarkerCommon
{
public:
    enum
    {
        CLASS_CHECKPOINT,
        CLASS_3DMARKER,
        CLASS_CORONA,
    };

    enum
    {
        ICON_NONE,
        ICON_ARROW,
        ICON_FINISH,
        ICON_INVALID,
    };

    CClientMarkerCommon();
    virtual ~CClientMarkerCommon();

    virtual void         DoPulse();
    virtual unsigned int GetMarkerType() const = 0;

    virtual bool IsHit(const CVector& vecPosition) const = 0;
    bool         IsHit(CClientEntity* pEntity) const;

    virtual bool IsVisible() const = 0;
    virtual void SetVisible(bool bVisible) = 0;

    virtual SColor GetColor() const = 0;
    virtual void   SetColor(const SColor& color) = 0;

    virtual void GetPosition(CVector& vecPosition) const = 0;
    virtual void SetPosition(const CVector& vecPosition) = 0;

    virtual void GetMatrix(CMatrix& matrix) = 0;
    virtual void SetMatrix(CMatrix& matrix) = 0;

    virtual float GetSize() const = 0;
    virtual void  SetSize(float fSize) = 0;

    virtual void StreamIn() = 0;
    virtual void StreamOut() = 0;

    virtual void SetIgnoreAlphaLimits(bool ignore) noexcept = 0;
    virtual bool AreAlphaLimitsIgnored() const noexcept = 0;
};
