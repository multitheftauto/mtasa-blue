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
    float	fX, fY, fZ;     // Vertex position
	float	fRHW;
	DWORD	dwColor;
    float   tu, tv; // Vertex texture coordinates
} D3DSPRITEVERTEX2D, *PD3DSPRITEVERTEX2D;

typedef struct __SPRITEVERTEX3D
{
    float	fX, fY, fZ;     // Vertex position
	DWORD	dwColor;
    float   tu, tv; // Vertex texture coordinates
} D3DSPRITEVERTEX3D, *PD3DSPRITEVERTEX3D;

typedef struct __FONTCOORDS
{
	float	tx1, tx2, ty1, ty2;
	float	fWidth, fHeight;
} FONTCOORDS, *PFONTCOORDS;

class CD3DMGEng
{
public:

	        CD3DMGEng                   ( LPDIRECT3DDEVICE9 pD3DDevice );
	        ~CD3DMGEng                  () {}

	void    OnDeleteDevice              ( void );
	void    OnRestoreDevice             ( void );
	void    OnInvalidateDevice          ( void );

    void	SetRenderTarget             ( LPDIRECT3DSURFACE9 pSurface )	{ m_pDevice->SetRenderTarget ( 0, pSurface ); };
	void    GetRenderTarget             ( LPDIRECT3DSURFACE9 pSurface )	{ m_pDevice->GetRenderTarget ( 0, &pSurface ); };

    bool    BeginDrawing                ();
	bool    EndDrawing                  ();

	bool    Render2DSpriteEx            ( LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR2 *pScaling, D3DXVECTOR2 *pTranslation, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR );
	bool    Render2DSprite              ( LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR2 *pScaling, D3DXVECTOR2 *pTranslation, DWORD dwColor );
	bool    Render3DSpriteEx            ( LPDIRECT3DTEXTURE9 pTexture, float fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR );
	bool    Render3DSprite              ( LPDIRECT3DTEXTURE9 pTexture, float fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColor );
    
    bool    Render3DTriangle            ( LPDIRECT3DTEXTURE9 pTexture, float fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorB );

	bool    Render2DBox                 ( float x, float y, float fWidth, float fHeight, DWORD dwColor );
	bool    Render2DBoxEx               ( float x, float y, float fWidth, float fHeight, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR );
	bool    Render3DBox                 ( float x, float y, float z, float fL, float fW, float fH, DWORD dwColor, bool bWireframe = FALSE );

    bool    DrawLine                    ( const D3DXVECTOR3& a, const D3DXVECTOR3& b, DWORD dwColor );
    bool    DrawLine3D                  ( const D3DXVECTOR3& a, const D3DXVECTOR3& b, float fWidth, LPDIRECT3DTEXTURE9 pTexture, DWORD dwColor );

	bool    DrawPlane3D                 ( const D3DXVECTOR3& a, const D3DXVECTOR3& b, LPDIRECT3DTEXTURE9 pTexture, DWORD dwColor );

    void    DrawTextTTF                 ( int left, int top, int right, int bottom,  DWORD dwColor, const TCHAR *wszText, INT iTextHeight, INT iTextWeight, DWORD dwFormat, CHAR * szFaceName );
	bool    DrawText2D                  ( float x, float y, DWORD dwColor, const TCHAR *wszText,  float fTextSize = 1.0f, FONTFLAGS ffFlags = D3DFF_LEFT, FONTSET fsSet = D3DFS_FONTSET1 );
    bool    DrawText3D                  ( float x, float y, float z, D3DMATRIX * pMatView, DWORD dwColor, const TCHAR *wszText,  float fTextSize = 1.0f, FONTFLAGS ffFlags = D3DFF_CENTERED, FONTSET fsSet = D3DFS_FONTSET1 );

    bool    LoadFontTextureFromMem      ( PVOID pvMemory, DWORD dwSizeOfFile );
	bool    LoadFontTextureFromFile     ( TCHAR *wszFileName );
	bool    LoadFontInfoFromMem         ( PVOID pvMemory );
	bool    LoadFontInfoFromFile        ( TCHAR *wszFileName );
	bool    LoadDefaultFontInfo         ( void );
	bool    UnloadFont                  ( void );

	bool    UTIL_FormatText             ( const TCHAR* wszInputText, TCHAR* wszOutputText, float fMaxWidth, float fTextSize = 1.0f, FONTSET fsSet = D3DFS_FONTSET1 );
	bool    UTIL_GetTextExtent          ( const TCHAR* strText, SIZE* pSize, float fTextSize = 1.0f, FONTSET fsSet = D3DFS_FONTSET1 );
    void    UTIL_CaptureDeviceState     ();
    void    UTIL_RestoreDeviceState     ();

private:

	//////////////////////////////////////////
	// Font engine related variables.
	LPDIRECT3DTEXTURE9		m_pFontTexture;	
	float					m_fFontTexWidth;
	float					m_fFontTexHeight;
	FONTCOORDS				m_aFontCoords[D3DFE_CHARACTERSPERROW*D3DFE_CHARACTERSPERCOLUMN];
	bool					m_bFontInfoLoaded;
    D3DXFONT_DESC           m_dLastFontDesc;
    LPD3DXFONT              m_pLastFont;

	//////////////////////////////////////////
	// Generic rendering variables.
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DDEVICE9		m_pDevice;
    LPDIRECT3DSTATEBLOCK9   m_pDeviceState;

};

#endif
