/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        effekseer/EffekseerManager.h
 *  PURPOSE:     Effekseer manager file
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class EffekseerManager;

#pragma once

#include <effekseer/CEffekseerManager.h>
//#include "Effekseer.h"
#include "EffekseerRendererDX9.h"

class EffekseerManager : public CEffekseerManagerInterface
{
private:
    EffekseerManager();
    ~EffekseerManager();

    static ::Effekseer::Matrix44 ConvertD3DMatrix(D3DMATRIX matrix);
public:
    static EffekseerManager* GetManager();

    void Init(IDirect3DDevice9* pDevice);

    Effekseer::ManagerRef GetInterface() { return m_pInterface; };

    void OnLostDevice();
    void OnResetDevice();

    void DrawEffects(D3DMATRIX mProection, D3DMATRIX mView);
private:
    Effekseer::ManagerRef m_pInterface;

    EffekseerRendererDX9::RendererRef m_pRenderer;
    IDirect3DDevice9*                 m_pDevice;
};
