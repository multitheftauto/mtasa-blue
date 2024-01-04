
#pragma once

#include <effekseer/CEffekseerEffectHandler.h>

#include <Effekseer.h>

class CEffekseerEffectHandlerImpl : public CEffekseerEffectHandler
{
public:
    CEffekseerEffectHandlerImpl(Effekseer::ManagerRef manager, Effekseer::Handle handle) : m_pInterface(manager), m_Handle(handle){};
    ~CEffekseerEffectHandlerImpl() = default;

    // Inherited via CEffekseerEffectHandler
    void  GetPosition(CVector& vecPosition) override;
    void  GetRotation(CVector& vecPosition) override;
    void  SetPosition(const CVector& vecPosition) override;
    void  SetRotation(const CVector& vecRotation) override;
    void  SetScale(const CVector& vecScale) override;
    void  StopEffect() override;
    void  StopRoot() override;
    void  SetSpeed(float fSpeed) override;
    float GetSpeed() override;
    void  SetDynamicInput(int32_t index, float fValue) override;
    float GetDynamicInput(int32_t index) override;

private:
    Effekseer::Handle     m_Handle;
    Effekseer::ManagerRef m_pInterface;
};
