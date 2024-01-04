
#pragma once

#include "CEffekseerEffect.h"

class CEffekseerManagerInterface
{
public:
    virtual CEffekseerEffect* Create(const char* path) = 0;
    virtual void              Remove(CEffekseerEffect* effect) = 0;
    virtual void              OnLostDevice() = 0;
    virtual void              OnResetDevice() = 0;
    virtual void              DrawEffects(D3DMATRIX& mProection, D3DMATRIX& mView) = 0;
};
