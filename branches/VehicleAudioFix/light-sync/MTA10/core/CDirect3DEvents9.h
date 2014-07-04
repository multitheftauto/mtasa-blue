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
typedef IUnknown IDirect3DDevice8;

class CDirect3DEvents9
{
    public:

    static void         OnDirect3DDeviceCreate      ( IDirect3DDevice9 *pDevice );
    static void         OnDirect3DDeviceDestroy     ( IDirect3DDevice9 *pDevice );
    static void         OnPresent                   ( IDirect3DDevice9 *pDevice );
    static void         OnBeginScene                ( IDirect3DDevice9 *pDevice ); 
    static bool         OnEndScene                  ( IDirect3DDevice9 *pDevice );
    static void         OnInvalidate                ( IDirect3DDevice9 *pDevice );
    static void         OnRestore                       ( IDirect3DDevice9 *pDevice );
    static HRESULT      OnDrawPrimitive             ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount );
    static HRESULT      OnDrawIndexedPrimitive      ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount );
    static HRESULT      DrawPrimitiveGuarded        ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount );
    static HRESULT      DrawIndexedPrimitiveGuarded ( IDirect3DDevice9 *pDevice, D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount );
    static HRESULT      CreateVertexBuffer          ( IDirect3DDevice9 *pDevice, UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle );
    static HRESULT      SetStreamSource             ( IDirect3DDevice9 *pDevice, UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride );
};

#endif