/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        effekseer/CEffekseerManager.h
 *  PURPOSE:     Effekseer manager file
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CEffekseerManager;

#pragma once

#include <effekseer/CEffekseer.h>
#include "EffekseerRendererDX9.h"

class CEffekseerManager : public CEffekseer
{
public:
    CEffekseerManager();
    ~CEffekseerManager();

    CEffekseerEffect* Create(const char* path) override;
    void              Remove(CEffekseerEffect* effect) override;
    void              Init(IDirect3DDevice9* pDevice);

    void OnLostDevice() override;
    void OnResetDevice() override;

    void DrawEffects(D3DMATRIX &mProection, D3DMATRIX &mView);
private:
    Effekseer::ManagerRef m_pInterface;

    EffekseerRendererDX9::RendererRef m_pRenderer;
    IDirect3DDevice9*                 m_pDevice;
};
