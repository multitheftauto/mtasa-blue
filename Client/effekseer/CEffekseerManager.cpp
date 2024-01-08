/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        effekseer/CEffekseerManager.cpp
 *  PURPOSE:     Effekseer manager file
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include "CEffekseerManager.h"
#include "CEffekseerEffectImpl.h"
#include <codecvt>
#include <chrono>

constexpr int32_t EFK_MAX_PARTICLES = 8000;

CEffekseerManager::CEffekseerManager()
{
    m_pDevice = nullptr;
}

CEffekseerManager::~CEffekseerManager()
{
}

CEffekseerEffect* CEffekseerManager::Create(const char* path)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
    std::u16string                                                    wstr = utf16conv.from_bytes(path);

    auto pEfkEffect = Effekseer::Effect::Create(m_pInterface, wstr.c_str());

    if (pEfkEffect == nullptr)
        return nullptr;

    auto pEffect = new CEffekseerEffectImpl(m_pInterface, pEfkEffect);

    return pEffect;
}

void CEffekseerManager::Remove(CEffekseerEffect* effect)
{
    // Effekseer types with Ref, such as Effekseer::ManagerRef and Effekseer::EffectRef, are managed by smart pointers.
    // These are automatically released when there are no more variables using the object.
    delete static_cast<CEffekseerEffectImpl*>(effect);
}

void CEffekseerManager::Init(IDirect3DDevice9* pDevice)
{
    m_pDevice = pDevice;

    m_pInterface = ::Effekseer::Manager::Create(EFK_MAX_PARTICLES);

    m_pRenderer = ::EffekseerRendererDX9::Renderer::Create(m_pDevice, EFK_MAX_PARTICLES);
    m_pRenderer->SetRestorationOfStatesFlag(false);

    m_pInterface->SetSpriteRenderer(m_pRenderer->CreateSpriteRenderer());
    m_pInterface->SetRibbonRenderer(m_pRenderer->CreateRibbonRenderer());
    m_pInterface->SetRingRenderer(m_pRenderer->CreateRingRenderer());
    m_pInterface->SetTrackRenderer(m_pRenderer->CreateTrackRenderer());
    m_pInterface->SetModelRenderer(m_pRenderer->CreateModelRenderer());

    // Create loaders
    m_pInterface->SetTextureLoader(m_pRenderer->CreateTextureLoader());
    m_pInterface->SetModelLoader(m_pRenderer->CreateModelLoader());
    m_pInterface->SetMaterialLoader(m_pRenderer->CreateMaterialLoader());
    m_pInterface->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
}

void CEffekseerManager::OnLostDevice()
{
    m_pRenderer->OnLostDevice();
}

void CEffekseerManager::OnResetDevice()
{
    m_pRenderer->OnResetDevice();
}

constexpr float NANO = 1.0e-9f;
static auto     lastFrame = std::chrono::steady_clock::now();

void CEffekseerManager::DrawEffects(D3DMATRIX &matrixProj, D3DMATRIX &matrixView)
{
    m_pRenderer->SetProjectionMatrix(reinterpret_cast<::Effekseer::Matrix44&>(matrixProj));
    m_pRenderer->SetCameraMatrix(reinterpret_cast<::Effekseer::Matrix44&>(matrixView));

    auto now = std::chrono::steady_clock::now();
    float dt = ((now - lastFrame).count() * 60) * NANO;

    lastFrame = now;

    m_pRenderer->SetTime(now.time_since_epoch().count() * NANO);

    m_pInterface->Update(dt);

    m_pRenderer->BeginRendering();

    m_pDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    m_pInterface->Draw();
    m_pRenderer->EndRendering();
}
