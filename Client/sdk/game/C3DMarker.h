/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/C3DMarker.h
 *  PURPOSE:     3D marker entity interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CVector;
struct RpClump;
class CMatrix;

/**
 * \todo fix SetColor/GetColor, the format is actually BGRA (strange)
 */
class C3DMarker
{
public:
    virtual ~C3DMarker(){};

    virtual void     GetMatrix(CMatrix* pMatrix) = 0;
    virtual void     SetMatrix(CMatrix* pMatrix) = 0;
    virtual void     SetPosition(CVector* vecPosition) = 0;
    virtual CVector* GetPosition() = 0;
    virtual DWORD    GetType() = 0;            // need enum?
    virtual BOOL     IsActive() = 0;
    virtual DWORD    GetIdentifier() = 0;
    virtual SColor   GetColor() = 0;
    virtual void     SetColor(const SColor color) = 0;
    virtual void     SetPulsePeriod(WORD wPulsePeriod) = 0;
    virtual void     SetPulseFraction(FLOAT fPulseFraction) = 0;
    virtual void     SetRotateRate(short RotateRate) = 0;
    virtual FLOAT    GetSize() = 0;
    virtual void     SetSize(FLOAT fSize) = 0;
    virtual FLOAT    GetBrightness() = 0;
    virtual void     SetBrightness(FLOAT fBrightness) = 0;
    virtual void     SetCameraRange(FLOAT fCameraRange) = 0;
    virtual FLOAT    GetPulseFraction() = 0;
    virtual void     Disable() = 0;
    virtual void     SetActive() = 0;
    virtual RpClump* GetRwObject() = 0;
};
