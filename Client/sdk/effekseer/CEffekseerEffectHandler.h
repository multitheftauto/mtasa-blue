/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/effekseer/CEffecseerEffectHandler.h
 *  PURPOSE:     Effekseer effect handler interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <CMatrix.h>
#include <cstdint>

class CEffekseerEffectHandler
{
public:
    virtual void    GetPosition(CVector& vecPosition) = 0;
    virtual void    GetRotation(CVector& vecPosition) = 0;
    virtual void    SetPosition(const CVector& vecPosition) = 0;
    virtual void    SetRotation(const CVector& vecRotation) = 0;
    virtual void    SetScale(const CVector& vecScale) = 0;
    virtual void    StopEffect() = 0;
    virtual void    StopRoot() = 0;
    virtual void    SetSpeed(float fSpeed) = 0;
    virtual float   GetSpeed() = 0;
    virtual void    GetMatrix(CMatrix& matrix) = 0;
    virtual void    SetMatrix(const CMatrix& matrix) = 0;
    virtual void    SetDynamicInput(int32_t index, float fValue) = 0;
    virtual float   GetDynamicInput(int32_t index) = 0;
    virtual void    SetRandomSeed(int32_t seed) = 0;
    virtual void    SetLayer(int32_t layer) = 0;
    virtual int32_t GetLayer() = 0;
    virtual bool    GetSpawnDisabled() = 0;
    virtual void    SetSpawnDisabled(bool status) = 0;
    virtual void    SetPaused(bool status) = 0;
    virtual bool    GetPaused() = 0;
    virtual void    SetShown(bool status) = 0;
    virtual bool    GetShown() = 0;
    virtual void    SendTrigger(int32_t index) = 0;
    virtual void    SetTargetLocation(const CVector& location) = 0;
    virtual void    SetAllColor(SColor color) = 0;
    virtual bool    Exists() = 0;
};
