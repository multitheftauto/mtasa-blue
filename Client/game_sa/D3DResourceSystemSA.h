#pragma once

#include <d3d9.h>

class D3DTextureBuffer
{
public:
    std::uint32_t        m_nFormat;
    std::uint32_t        m_nWidth;
    int                 m_nLevels;
    std::uint32_t        m_nCapacity;
    std::uint32_t        m_nNumTexturesInBuffer;
    std::uint32_t        m_nSize;
    IDirect3DTexture9** m_apTextures;
};

class D3DIndexDataBuffer
{
public:
    std::uint32_t            m_nFormat;
    int                     field_4;
    std::uint32_t            m_nCapcacity;
    std::uint32_t            m_nNumDatasInBuffer;
    std::uint32_t            m_nSize;
    IDirect3DIndexBuffer9** m_apIndexData;
};

class D3DResourceSystemSA
{
public:
    D3DResourceSystemSA();
    static void StaticSetHooks();
};

void D3DResourceSystem_Init();
void D3DResourceSystem_SetUseD3DResourceBuffering(char bUse);
