/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirect3DEvents9.h
*  PURPOSE:     Header file for Direct3D 9 events class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDIRECT3DEVENTS9_H
#define __CDIRECT3DEVENTS9_H

#include <d3d9.h>

class CDirect3DEvents9
{
    public:

    static void         OnDirect3DDeviceCreate      ( IDirect3DDevice9 *pDevice );
    static void         OnDirect3DDeviceDestroy     ( IDirect3DDevice9 *pDevice );
    static void         OnPresent                   ( IDirect3DDevice9 *pDevice );
    static void         OnBeginScene                ( IDirect3DDevice9 *pDevice ); 
    static bool         OnEndScene                  ( IDirect3DDevice9 *pDevice );
    static void         OnInvalidate                ( IDirect3DDevice9 *pDevice );
    static void         OnRestore                   ( IDirect3DDevice9 *pDevice );
    static HRESULT      OnDrawPrimitive             ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount );
    static HRESULT      DrawPrimitiveShader         ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount, CShaderItem* pShaderItem, bool bIsLayer  );
    static HRESULT      OnDrawIndexedPrimitive      ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount );
    static HRESULT      DrawIndexedPrimitiveShader  ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, CShaderItem* pShaderItem, bool bIsLayer, bool bCanBecomeActiveShader );
    static HRESULT      DrawPrimitiveGuarded        ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount );
    static HRESULT      DrawIndexedPrimitiveGuarded ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount );
    static HRESULT      CreateVertexBuffer          ( IDirect3DDevice9 *pDevice, UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle );
    static HRESULT      CreateIndexBuffer           ( IDirect3DDevice9 *pDevice, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle );
    static HRESULT      CreateTexture               ( IDirect3DDevice9 *pDevice, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle );
    static HRESULT      SetStreamSource             ( IDirect3DDevice9 *pDevice, UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride );
    static HRESULT      CreateVertexDeclaration     ( IDirect3DDevice9 *pDevice, CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl );
    static HRESULT      SetVertexDeclaration        ( IDirect3DDevice9 *pDevice, IDirect3DVertexDeclaration9* pDecl );
    static void         CheckForScreenShot          ( void );
    static ERenderFormat DiscoverReadableDepthFormat ( IDirect3DDevice9 *pDevice, D3DMULTISAMPLE_TYPE multisampleType, bool bWindowed );
    static void         CloseActiveShader           ( void );

    static IDirect3DVertexBuffer9*  GetRealVertexBuffer ( IDirect3DVertexBuffer9* pVertexBuffer );
    static IDirect3DIndexBuffer9*   GetRealIndexBuffer  ( IDirect3DIndexBuffer9* pIndexBuffer );
    static IDirect3DBaseTexture9*   GetRealTexture      ( IDirect3DBaseTexture9* pTexture );
};

#endif
