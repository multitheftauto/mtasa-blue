
#pragma once

#include <CVector.h>
#include <CMatrix.h>
#include <cstdint>

class CEffekseerEffectHandler
{
public:
    virtual void  GetPosition(CVector& vecPosition) = 0;
    virtual void  GetRotation(CVector& vecPosition) = 0;
    virtual void  SetPosition(const CVector& vecPosition) = 0;
    virtual void  SetRotation(const CVector& vecRotation) = 0;
    virtual void  SetScale(const CVector& vecScale) = 0;
    virtual void  StopEffect() = 0;
    virtual void  StopRoot() = 0;
    virtual void  SetSpeed(float fSpeed) = 0;
    virtual float GetSpeed() = 0;
    virtual void  GetMatrix(CMatrix &matrix) = 0;
    virtual void  SetMatrix(const CMatrix &matrix) = 0;
    virtual void  SetDynamicInput(int32_t index, float fValue) = 0;
    virtual float GetDynamicInput(int32_t index) = 0;
};
