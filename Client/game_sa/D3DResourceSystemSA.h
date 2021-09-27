#pragma once

class D3DTextureBuffer {
public:
    unsigned int m_nFormat;
    unsigned int m_nWidth;
    int          m_nLevels;
    unsigned int m_nCapacity;
    unsigned int m_nNumTexturesInBuffer;
    unsigned int m_nSize;
    IDirect3DTexture9** m_apTextures;
};

class D3DIndexDataBuffer {
public:
    unsigned int m_nFormat;
    int field_4;
    unsigned int m_nCapcacity;
    unsigned int m_nNumDatasInBuffer;
    unsigned int m_nSize;
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
