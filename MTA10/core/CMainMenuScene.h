/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMainMenuScene.h
*  PURPOSE:     Header file for Direct3D main menu rendering class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CMainMenuScene;

#ifndef __CMAINMENUSCENE_H
#define __CMAINMENUSCENE_H

#include "CCore.h"
#include "CSingleton.h"
#include "CVideoRenderer.h"

#include <dshow.h>
#include <vmr9.h>

class CMainMenuScene : public CSingleton < CMainMenuScene >
{
	friend class CMainMenu;

public:
						CMainMenuScene					( CMainMenu * pMainMenu );
						~CMainMenuScene					( void );

	void				OnInvalidate					( IDirect3DDevice9 * pDevice );
	void				OnRestore						( IDirect3DDevice9 * pDevice, CVector2D vecScreenSize, IDirect3DTexture9 * pRenderTarget );

	bool				ProcessMessage					( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

protected:
	bool				Init3DScene						( IDirect3DTexture9 * pRenderTarget, CVector2D vecScreenSize );
	void				Destroy3DScene					( void );
	void				Draw3DScene						( void );

	void				PlayVideo						( void );
	void				StopVideo						( void );

	void				PreStartCredits					( void );
	void				StartCredits					( void );
	void				StopCredits						( void );
	bool				IsCreditsPlaying				( void );

	bool				SetVideoEnabled					( bool bEnabled );
	void				SetPostProcessingEnabled		( bool bEnabled );

private:
	void				DrawCredit						( unsigned char ucIndex, short sStateRequired, D3DXVECTOR3 vecEyePt, float fDistanceThreshold, bool bIgnore = false );
	void				DrawRoll						( void );
	
	bool				InitShaders						( void );
	bool				InitRenderTargets				( void );
	bool				InitCreditSprites				( void );

	void				DestroyShaders					( void );
	void				DestroyRenderTargets			( void );

	bool				InitVideo						( void );

	void				CreateCylinder					( IDirect3DDevice9 * pDevice );

	CD3DMGEng *			m_pGFX;

	CVideoRenderer *	m_pVideoRenderer;

	CVector2D			m_vecScreenSize;

	IDirect3DDevice9 *	m_pDevice;

	CGraphics *			m_pGraphics;
	
	CMainMenu *			m_pMainMenu;

    std::vector < D3DMATERIAL9 >         pMeshMaterials;
    std::vector < LPDIRECT3DTEXTURE9 >   pMeshTextures;
    std::vector < bool >                 pbMeshTextures;
};

#endif
