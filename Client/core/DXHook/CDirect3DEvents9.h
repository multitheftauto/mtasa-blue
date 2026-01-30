/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CDirect3DEvents9.h
 *  PURPOSE:     Header file for Direct3D 9 events class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <d3d9.h>

class CDirect3DEvents9
{
public:
    static void    OnDirect3DDeviceCreate(IDirect3DDevice9* pDevice);
    static void    OnDirect3DDeviceDestroy(IDirect3DDevice9* pDevice);
    static void    OnPresent(IDirect3DDevice9* pDevice);
    static void    OnBeginScene(IDirect3DDevice9* pDevice);
    static bool    OnEndScene(IDirect3DDevice9* pDevice);
    static void    OnInvalidate(IDirect3DDevice9* pDevice);
    static void    OnRestore(IDirect3DDevice9* pDevice);
    static HRESULT OnDrawPrimitive(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
    static HRESULT DrawPrimitiveShader(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount,
                                       CShaderItem* pShaderItem, bool bIsLayer);
    static HRESULT OnDrawIndexedPrimitive(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices,
                                          UINT startIndex, UINT primCount);
    static HRESULT DrawIndexedPrimitiveShader(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex,
                                              UINT NumVertices, UINT startIndex, UINT primCount, CShaderItem* pShaderItem, bool bIsLayer,
                                              bool bCanBecomeActiveShader);
    static HRESULT DrawPrimitiveGuarded(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
    static HRESULT DrawIndexedPrimitiveGuarded(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex,
                                               UINT NumVertices, UINT startIndex, UINT primCount);
    static HRESULT DrawPrimitiveUPGuarded(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData,
                                          UINT VertexStreamZeroStride);
    static HRESULT DrawIndexedPrimitiveUPGuarded(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices,
                                                 UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData,
                                                 UINT VertexStreamZeroStride);
    static HRESULT DrawRectPatchGuarded(IDirect3DDevice9* pDevice, UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo);
    static HRESULT DrawTriPatchGuarded(IDirect3DDevice9* pDevice, UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo);
    static HRESULT ProcessVerticesGuarded(IDirect3DDevice9* pDevice, UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer,
                                          IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags);
    static HRESULT ClearGuarded(IDirect3DDevice9* pDevice, DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
    static HRESULT ColorFillGuarded(IDirect3DDevice9* pDevice, IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color);
    static HRESULT UpdateSurfaceGuarded(IDirect3DDevice9* pDevice, IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect,
                                        IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint);
    static HRESULT UpdateTextureGuarded(IDirect3DDevice9* pDevice, IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture);
    static HRESULT GetRenderTargetDataGuarded(IDirect3DDevice9* pDevice, IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface);
    static HRESULT GetFrontBufferDataGuarded(IDirect3DDevice9* pDevice, UINT iSwapChain, IDirect3DSurface9* pDestSurface);
    static HRESULT SetRenderTargetGuarded(IDirect3DDevice9* pDevice, DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget);
    static HRESULT SetDepthStencilSurfaceGuarded(IDirect3DDevice9* pDevice, IDirect3DSurface9* pNewZStencil);
    static HRESULT CreateAdditionalSwapChainGuarded(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters,
                                                    IDirect3DSwapChain9** pSwapChain);
    static HRESULT CreateVolumeTextureGuarded(IDirect3DDevice9* pDevice, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format,
                                              D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle);
    static HRESULT CreateCubeTextureGuarded(IDirect3DDevice9* pDevice, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                            IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle);
    static HRESULT CreateRenderTargetGuarded(IDirect3DDevice9* pDevice, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
                                             DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
    static HRESULT CreateDepthStencilSurfaceGuarded(IDirect3DDevice9* pDevice, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
                                                    DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
    static HRESULT CreateOffscreenPlainSurfaceGuarded(IDirect3DDevice9* pDevice, UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool,
                                                      IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
    static HRESULT PresentGuarded(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride,
                                  CONST RGNDATA* pDirtyRegion);
    static bool    IsDeviceOperational(IDirect3DDevice9* pDevice, bool* pbTemporarilyLost = nullptr, HRESULT* pHrCooperativeLevel = nullptr);
    static HRESULT CreateVertexBuffer(IDirect3DDevice9* pDevice, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer,
                                      HANDLE* pSharedHandle);
    static HRESULT CreateIndexBuffer(IDirect3DDevice9* pDevice, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer,
                                     HANDLE* pSharedHandle);
    static HRESULT CreateTexture(IDirect3DDevice9* pDevice, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                 IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle);
    static HRESULT SetStreamSource(IDirect3DDevice9* pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride);
    static HRESULT CreateVertexDeclaration(IDirect3DDevice9* pDevice, CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl);
    static HRESULT SetVertexDeclaration(IDirect3DDevice9* pDevice, IDirect3DVertexDeclaration9* pDecl);
    static void    CheckForScreenShot();
    static ERenderFormat DiscoverReadableDepthFormat(IDirect3DDevice9* pDevice, D3DMULTISAMPLE_TYPE multisampleType, bool bWindowed);
    static void          CloseActiveShader(bool bDeviceOperational = true);

    static IDirect3DVertexBuffer9* GetRealVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer);
    static IDirect3DIndexBuffer9*  GetRealIndexBuffer(IDirect3DIndexBuffer9* pIndexBuffer);
    static IDirect3DBaseTexture9*  GetRealTexture(IDirect3DBaseTexture9* pTexture);
};
