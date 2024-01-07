/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/effekseer/CEffecseerEffect.h
 *  PURPOSE:     Effekseer effect interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>

#include "CEffekseerEffectHandler.h"

class CEffekseerEffect
{
public:
    virtual CEffekseerEffectHandler* Play(const CVector &pos) = 0;
};
