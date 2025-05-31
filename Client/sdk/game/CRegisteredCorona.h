/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CRegisteredCorona.h
 *  PURPOSE:     Registered corona entity interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "enums/CoronaType.h"

class CVector;
struct RwTexture;

class CRegisteredCorona
{
public:
    virtual CVector* GetPosition() = 0;
    virtual void     SetPosition(CVector* vector) = 0;
    virtual float    GetSize() = 0;
    virtual void     SetSize(float fSize) = 0;
    virtual float    GetRange() = 0;
    virtual void     SetRange(float fRange) = 0;
    virtual float    GetPullTowardsCamera() = 0;
    virtual void     SetPullTowardsCamera(float fPullTowardsCamera) = 0;
    virtual void     SetColor(BYTE Red, BYTE Green, BYTE Blue, BYTE Alpha) = 0;
    virtual void     SetTexture(RwTexture* texture) = 0;
    virtual void     SetTexture(CoronaType texture) = 0;
    virtual BYTE     GetFlareType() = 0;
    virtual void     SetFlareType(BYTE fFlareType) = 0;
    virtual void     SetReflectionType(BYTE reflectionType) = 0;
    virtual DWORD    GetIdentifier() = 0;
    virtual DWORD    GetID() = 0;
    virtual void     Refresh() = 0;
    virtual void     Disable() = 0;
};
