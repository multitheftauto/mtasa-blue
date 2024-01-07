
#pragma once

#include <effekseer/CEffekseerEffectHandler.h>

#include <Effekseer.h>

class CEffekseerEffectHandlerImpl : public CEffekseerEffectHandler
{
public:
    CEffekseerEffectHandlerImpl(Effekseer::ManagerRef manager, Effekseer::Handle handle) : m_pInterface(manager), m_Handle(handle){};
    ~CEffekseerEffectHandlerImpl() = default;

    // Inherited via CEffekseerEffectHandler
    void    GetPosition(CVector& vecPosition) override;
    void    GetRotation(CVector& vecPosition) override;
    void    SetPosition(const CVector& vecPosition) override;
    void    SetRotation(const CVector& vecRotation) override;
    void    GetMatrix(CMatrix& matrix) override;
    void    SetMatrix(const CMatrix& matrix) override;
    void    SetScale(const CVector& vecScale) override;
    void    StopEffect() override;
    void    StopRoot() override;
    void    SetSpeed(float fSpeed) override;
    float   GetSpeed() override;
    void    SetDynamicInput(int32_t index, float fValue) override;
    float   GetDynamicInput(int32_t index) override;
    void    SetRandomSeed(int32_t seed) override;
    void    SetLayer(int32_t layer) override;
    int32_t GetLayer() override;
    bool    GetSpawnDisabled() override;
    void    SetSpawnDisabled(bool status) override;
    void    SetPaused(bool status) override;
    bool    GetPaused() override;
    void    SetShown(bool status) override;
    bool    GetShown() override;
    void    SendTrigger(int32_t index) override;
    void    SetTargetLocation(const CVector& location) override;
    void    SetAllColor(SColor color) override;
    bool    Exists() override;

private:
    Effekseer::Handle     m_Handle;
    Effekseer::ManagerRef m_pInterface;
};
