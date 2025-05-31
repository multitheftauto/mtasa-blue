/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/D3DResourceSystemSA.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "D3DResourceSystemSA.h"

static bool&               D3DResourceSystem_UseD3DResourceBuffering = *(bool*)0x8D6084;
static unsigned int&       D3DResourceSystem_FreeTextureBufferIndex = *(unsigned int*)0xC87C60;
static D3DTextureBuffer&   D3DResourceSystem_TextureBuffer = *(D3DTextureBuffer*)0xC87C68;
static D3DIndexDataBuffer& D3DResourceSystem_IndexDataBuffer = *(D3DIndexDataBuffer*)0xC87E48;

D3DResourceSystemSA::D3DResourceSystemSA()
{
}

void D3DResourceSystemSA::StaticSetHooks()
{
    // Fix #1275: Txd texture memory leak
    // Keep resource buffering disabled to save memory.
    HookInstall(0x730830, (DWORD)D3DResourceSystem_Init, 5);
    HookInstall(0x730AC0, (DWORD)D3DResourceSystem_SetUseD3DResourceBuffering, 5);
}

void D3DResourceSystem_Init()
{
    auto D3DTextureBuffer_Setup = (void(__thiscall*)(D3DTextureBuffer * pThis, int format, int width, int bOneLevel, int capacity))0x72FE80;
    auto D3DIndexDataBuffer_Setup = (void(__thiscall*)(D3DIndexDataBuffer * pThis, int format, int a3, int capacity))0x730190;

    D3DResourceSystem_UseD3DResourceBuffering = false;
    D3DResourceSystem_FreeTextureBufferIndex = 0;
    D3DTextureBuffer_Setup(&D3DResourceSystem_TextureBuffer, 0, 0, -1, 16);
    D3DIndexDataBuffer_Setup(&D3DResourceSystem_IndexDataBuffer, D3DFMT_INDEX16, 0, 16);
}

void D3DResourceSystem_SetUseD3DResourceBuffering(char bUse)
{
    D3DResourceSystem_UseD3DResourceBuffering = false;
}
