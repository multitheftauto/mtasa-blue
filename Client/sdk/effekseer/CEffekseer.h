/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/effekseer/CEffecseer.h
 *  PURPOSE:     Effekseer module interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CEffekseerEffect.h"

class CEffekseer
{
public:
    virtual CEffekseerEffect* Create(const char* path) = 0;
    virtual void              Remove(CEffekseerEffect* effect) = 0;
    virtual void              OnLostDevice() = 0;
    virtual void              OnResetDevice() = 0;
    virtual void              DrawEffects(D3DMATRIX& mProection, D3DMATRIX& mView) = 0;
};
