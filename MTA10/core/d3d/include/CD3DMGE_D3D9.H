/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/d3d/include/CD3DMGE_D3D9.h
*  PURPOSE:		Interface for rendering Direct3D 9 primitives/sprites
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CD3DSPRITE_H
#define __CD3DSPRITE_H

#define DIRECT3D_VERSION         0x0900

#include "d3d9.h"
#include "d3dx9.h"
#include "dxutil.h"

///////////////////////////////////////////////////////////////////////////////
// Defines used to calculate characters from font bitmap.
#define D3DFE_CHARACTERSPERROW		(16)
#define	D3DFE_CHARACTERSPERCOLUMN	(16)
#define D3DFE_MAXFONTWIDTHS			(D3DFE_CHARACTERSPERROW*D3DFE_CHARACTERSPERCOLUMN)

enum FONTSET	{ D3DFS_FONTSET1=0,
				  D3DFS_FONTSET2=128 };

enum FONTFLAGS	{ D3DFF_CENTERED=1,
				  D3DFF_RIGHT,
				  D3DFF_LEFT };

///////////////////////////////////////////////////////////////////////////////
// Texture FVFs
#define D3DFVF_SPRITEVERTEX2DTEX	(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_SPRITEVERTEX3DTEX	(D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE)
#define D3DFVF_FONTVERTEX2D			(D3DFVF_SPRITEVERTEX2DTEX)

///////////////////////////////////////////////////////////////////////////////
// Non-textured FVFs
#define D3DFVF_SPRITEVERTEX2D		(D3DFVF_SPRITEVERTEX2DTEX&~D3DFVF_TEX1)
#define D3DFVF_SPRITEVERTEX3D		(D3DFVF_SPRITEVERTEX3DTEX&~D3DFVF_TEX1)

///////////////////////////////////////////////////////////////////////////////
// Maximum number of vertices for our vbuffer.
#define D3DSPRITE_NUMVERTS 6*50

#define D3DFVF_XYZ              0x002
#define D3DFVF_RESERVED1        0x020
#define D3DFVF_DIFFUSE          0x040
#define D3DFVF_SPECULAR         0x080
#define D3DFVF_TEX1             0x100
#define D3DFVF_LVERTEX ( D3DFVF_XYZ | D3DFVF_RESERVED1 | D3DFVF_DIFFUSE | \
                         D3DFVF_SPECULAR | D3DFVF_TEX1 )

typedef float D3DVALUE, *LPD3DVALUE;

typedef struct D3DLVERTEX {
    union {
    D3DVALUE     x;             /* Homogeneous coordinates */
    D3DVALUE     dvX;
    };
    union {
    D3DVALUE     y;
    D3DVALUE     dvY;
    };
    union {
    D3DVALUE     z;
    D3DVALUE     dvZ;
    };
    DWORD            dwReserved;
    union {
    D3DCOLOR     color;         /* Vertex color */
    D3DCOLOR     dcColor;
    };
    union {
    D3DCOLOR     specular;      /* Specular component of vertex */
    D3DCOLOR     dcSpecular;
    };
    union {
    D3DVALUE     tu;            /* Texture coordinates */
    D3DVALUE     dvTU;
    };
    union {
    D3DVALUE     tv;
    D3DVALUE     dvTV;
    };
} D3DLVERTEX, *LPD3DLVERTEX;

typedef struct __SPRITEVERTEX2D
{
    FLOAT	fX, fY, fZ;     // Vertex position
	FLOAT	fRHW;
	DWORD	dwColor;
    FLOAT   tu, tv; // Vertex texture coordinates
} D3DSPRITEVERTEX2D, *PD3DSPRITEVERTEX2D;

typedef struct __SPRITEVERTEX3D
{
    FLOAT	fX, fY, fZ;     // Vertex position
	DWORD	dwColor;
    FLOAT   tu, tv; // Vertex texture coordinates
} D3DSPRITEVERTEX3D, *PD3DSPRITEVERTEX3D;

typedef struct __FONTCOORDS
{
	FLOAT	tx1, tx2, ty1, ty2;
	FLOAT	fWidth, fHeight;
} FONTCOORDS, *PFONTCOORDS;

class CD3DMGEng
{
public:

	CD3DMGEng ( LPDIRECT3DDEVICE9 pD3DDevice );
	~CD3DMGEng () {};

	VOID OnDeleteDevice ( void );
	VOID OnRestoreDevice ( void );
	VOID OnInvalidateDevice ( void );

	BOOL Render2DSpriteEx ( LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR2 *pScaling, D3DXVECTOR2 *pTranslation, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR );
	BOOL Render2DSprite ( LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR2 *pScaling, D3DXVECTOR2 *pTranslation, DWORD dwColor );
	BOOL Render3DSpriteEx ( LPDIRECT3DTEXTURE9 pTexture, FLOAT fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR );
	BOOL Render3DSprite ( LPDIRECT3DTEXTURE9 pTexture, FLOAT fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColor );
    
    BOOL Render3DTriangle ( LPDIRECT3DTEXTURE9 pTexture, FLOAT fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorB );

	inline VOID	SetRenderTarget ( LPDIRECT3DSURFACE9 pSurface )	{ m_pDevice->SetRenderTarget ( 0, pSurface ); };
	inline VOID GetRenderTarget ( LPDIRECT3DSURFACE9 pSurface )	{ m_pDevice->GetRenderTarget ( 0, &pSurface ); };

    BOOL BeginDrawing ();
	BOOL EndDrawing ();

	BOOL Render2DBox ( FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight, DWORD dwColor );
	BOOL Render2DBoxEx ( FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR );
	BOOL Render3DBox ( FLOAT x, FLOAT y, FLOAT z, FLOAT fL, FLOAT fW, FLOAT fH, DWORD dwColor, BOOL bWireframe = FALSE );

	BOOL LoadFontTextureFromMem ( PVOID pvMemory, DWORD dwSizeOfFile );
	BOOL LoadFontTextureFromFile ( TCHAR *wszFileName );
	BOOL LoadFontInfoFromMem ( PVOID pvMemory );
	BOOL LoadFontInfoFromFile ( TCHAR *wszFileName );
	BOOL LoadDefaultFontInfo ( VOID );
	BOOL UnloadFont ( VOID );

    BOOL DrawLine ( float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, DWORD dwColor );

    VOID DrawTextTTF ( int left, int top, int right, int bottom,  DWORD dwColor, const TCHAR *wszText, INT iTextHeight, INT iTextWeight, DWORD dwFormat, CHAR * szFaceName );
	BOOL DrawText2D ( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR *wszText,  FLOAT fTextSize = 1.0f, FONTFLAGS ffFlags = D3DFF_LEFT, FONTSET fsSet = D3DFS_FONTSET1 );
    BOOL DrawText3D ( FLOAT x, FLOAT y, FLOAT z, D3DMATRIX * pMatView, DWORD dwColor, const TCHAR *wszText,  FLOAT fTextSize = 1.0f, FONTFLAGS ffFlags = D3DFF_CENTERED, FONTSET fsSet = D3DFS_FONTSET1 );
	BOOL DrawLine3D ( FLOAT ax, FLOAT ay, FLOAT az, FLOAT bx, FLOAT by, FLOAT bz, FLOAT w, FLOAT h, LPDIRECT3DTEXTURE9 pTexture, DWORD dwColor );

	BOOL DrawPlane3D ( D3DXVECTOR3 a, D3DXVECTOR3 b, LPDIRECT3DTEXTURE9 pTexture, DWORD dwColor );

	BOOL UTIL_FormatText ( const TCHAR* wszInputText, TCHAR* wszOutputText, float fMaxWidth, FLOAT fTextSize = 1.0f, FONTSET fsSet = D3DFS_FONTSET1 );
	BOOL UTIL_GetTextExtent ( const TCHAR* strText, SIZE* pSize, FLOAT fTextSize = 1.0f, FONTSET fsSet = D3DFS_FONTSET1 );
    VOID UTIL_CaptureDeviceState ( );
    VOID UTIL_RestoreDeviceState ( );

private:

	//////////////////////////////////////////
	// Font engine related variables.
	LPDIRECT3DTEXTURE9		m_pFontTexture;	
	FLOAT					m_fFontTexWidth;
	FLOAT					m_fFontTexHeight;
	FONTCOORDS				m_aFontCoords[D3DFE_CHARACTERSPERROW*D3DFE_CHARACTERSPERCOLUMN];
	BOOL					m_bFontInfoLoaded;
    D3DXFONT_DESC           m_dLastFontDesc;
    LPD3DXFONT              m_pLastFont;

	//////////////////////////////////////////
	// Generic rendering variables.
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DDEVICE9		m_pDevice;
    LPDIRECT3DSTATEBLOCK9   m_pDeviceState;

};

#endif
