/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        effekseer/EffekseerManager.cpp
 *  PURPOSE:     Effekseer manager file
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

//#include <StdInc.h>

#include "CEffekseerManager.h"

#define EFK_MAX_PARTICLES 8000

static EffekseerManager* pCurrentManager;

EffekseerManager::EffekseerManager()
{
    m_pDevice = nullptr;
}

EffekseerManager::~EffekseerManager()
{
}

void EffekseerManager::Init(IDirect3DDevice9* pDevice)
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

EffekseerManager* EffekseerManager::GetManager()
{
    if (pCurrentManager)
        return pCurrentManager;

    pCurrentManager = new EffekseerManager();

    return pCurrentManager;
}

void EffekseerManager::OnLostDevice()
{
    m_pRenderer->OnLostDevice();
}

void EffekseerManager::OnResetDevice()
{
    m_pRenderer->OnResetDevice();
}

void EffekseerManager::DrawEffects(D3DMATRIX matrixProj, D3DMATRIX matrixView)
{
    m_pRenderer->SetProjectionMatrix(ConvertD3DMatrix(matrixProj));
    m_pRenderer->SetCameraMatrix(ConvertD3DMatrix(matrixView));

    m_pInterface->Update();

    m_pRenderer->BeginRendering();

    m_pDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    m_pInterface->Draw();
    m_pRenderer->EndRendering();
}

// TODO use cast?
::Effekseer::Matrix44 EffekseerManager::ConvertD3DMatrix(D3DMATRIX matrix)
{
    ::Effekseer::Matrix44 effMatrix;

    for (int a = 0; a < 4; a++)
    {
        for (int b = 0; b < 4; b++)
        {
            effMatrix.Values[a][b] = matrix.m[a][b];
        }
    }

    return effMatrix;
}
