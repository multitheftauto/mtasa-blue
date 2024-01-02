
#pragma once

#include "CEffekseerEffect.h"

class CEffekseerManagerInterface
{
public:
    virtual CEffekseerEffect* Create(const char* path) = 0;
};
