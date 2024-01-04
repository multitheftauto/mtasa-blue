
#pragma once

#include <CVector.h>

#include "CEffekseerEffectHandler.h"

class CEffekseerEffect
{
public:
    virtual CEffekseerEffectHandler* Play(const CVector &pos) = 0;
};
