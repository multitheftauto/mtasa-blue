/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C3DMarkerSA.h
 *  PURPOSE:     Header file for 3D Marker entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/C3DMarker.h>
#include "interfaces/C3DMarkerSAInterface.h"

class C3DMarkerSA : public C3DMarker
{
private:
    C3DMarkerSAInterface* internalInterface;

public:
    C3DMarkerSA(C3DMarkerSAInterface* markerInterface) { internalInterface = markerInterface; };

    C3DMarkerSAInterface* GetInterface() { return internalInterface; }
    C3DMarkerSAInterface* GetInterface() const { return internalInterface; }

    void               GetMatrix(CMatrix* pMatrix);
    void               SetMatrix(CMatrix* pMatrix);
    void               SetPosition(CVector* vecPosition);
    CVector*           GetPosition();
    e3DMarkerType      GetType() const override;
    void               SetType(e3DMarkerType type);            // doesn't work propperly (not virtualed)
    bool               IsActive();
    DWORD              GetIdentifier();
    void               SetColor(const SharedUtil::SColor color);            // actually BGRA
    void               SetPulsePeriod(WORD wPulsePeriod);
    void               SetRotateRate(short RotateRate);
    float              GetSize();
    void               SetSize(float fSize);
    float              GetBrightness();
    void               SetBrightness(float fBrightness);
    void               SetCameraRange(float fCameraRange);
    void               SetPulseFraction(float fPulseFraction);            // doesn't work propperly (not virtualed)
    float              GetPulseFraction();
    void               Disable();
    void               Reset();
    void               SetActive() { internalInterface->m_bIsUsed = true; }
    RpClump*           GetRwObject() { return reinterpret_cast<RpClump*>(internalInterface->m_pRwObject); }
};
