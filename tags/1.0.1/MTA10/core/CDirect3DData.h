/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirect3DData.h
*  PURPOSE:     Header file for Direct3D data class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDIRECT3DDATA_H
#define __CDIRECT3DDATA_H

#include <d3d9.h>
#include "CSingleton.h"

class CDirect3DData : public CSingleton < CDirect3DData >
{
public:
	            CDirect3DData       ( void );
	           ~CDirect3DData       ( void );

	void        StoreTransform      ( D3DTRANSFORMSTATETYPE dwMatrixToStore, const D3DMATRIX* pMatrix );
	void        GetTransform        ( D3DTRANSFORMSTATETYPE dwRequestedMatrix, D3DMATRIX * pMatrixOut );

    inline HWND GetDeviceWindow     ( void )                                                    { return m_hDeviceWindow; };
    inline void StoreDeviceWindow   ( HWND hDeviceWindow )                                      { m_hDeviceWindow = hDeviceWindow; };

	void        StoreViewport       ( DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight );

	DWORD       GetViewportX        ( void );
	DWORD       GetViewportY        ( void );
	DWORD       GetViewportWidth    ( void );
	DWORD       GetViewportHeight   ( void );

private:
	D3DMATRIX   m_mViewMatrix;
    D3DMATRIX   m_mProjMatrix;
    D3DMATRIX   m_mWorldMatrix;

    HWND        m_hDeviceWindow;

	DWORD       m_dwViewportX;
	DWORD       m_dwViewportY;
	DWORD       m_dwViewportWidth;
	DWORD       m_dwViewportHeight;
};

#endif
