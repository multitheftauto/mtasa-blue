/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/d3d/include/cd3dmge.h
*  PURPOSE:     Interface for rendering Direct3D primitives/sprites
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CD3DSPRITE_H
#define __CD3DSPRITE_H

#include "d3d8.h"
#include "d3dx8.h"
#include "dxutil.h"

///////////////////////////////////////////////////////////////////////////////
// Defines used to calculate characters from font bitmap.
#define D3DFE_CHARACTERSPERROW      (16)
#define D3DFE_CHARACTERSPERCOLUMN   (16)
#define D3DFE_MAXFONTWIDTHS         (D3DFE_CHARACTERSPERROW*D3DFE_CHARACTERSPERCOLUMN)

enum FONTSET    { D3DFS_FONTSET1=0,
                  D3DFS_FONTSET2=128 };

enum FONTFLAGS  { D3DFF_CENTERED=1,
                  D3DFF_RIGHT,
                  D3DFF_LEFT };

///////////////////////////////////////////////////////////////////////////////
// Texture FVFs
#define D3DFVF_SPRITEVERTEX2DTEX    (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_SPRITEVERTEX3DTEX    (D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE)
#define D3DFVF_FONTVERTEX2D         (D3DFVF_SPRITEVERTEX2DTEX)

///////////////////////////////////////////////////////////////////////////////
// Non-textured FVFs
#define D3DFVF_SPRITEVERTEX2D       (D3DFVF_SPRITEVERTEX2DTEX&~D3DFVF_TEX1)
#define D3DFVF_SPRITEVERTEX3D       (D3DFVF_SPRITEVERTEX3DTEX&~D3DFVF_TEX1)

///////////////////////////////////////////////////////////////////////////////
// Maximum number of vertices for our vbuffer.
#define D3DSPRITE_NUMVERTS 6*50

typedef struct __SPRITEVERTEX2D
{
    FLOAT   fX, fY, fZ;     // Vertex position
    FLOAT   fRHW;
    DWORD   dwColor;
    FLOAT   tu, tv; // Vertex texture coordinates
} D3DSPRITEVERTEX2D, *PD3DSPRITEVERTEX2D;

typedef struct __SPRITEVERTEX3D
{
    FLOAT   fX, fY, fZ;     // Vertex position
    DWORD   dwColor;
    FLOAT   tu, tv; // Vertex texture coordinates
} D3DSPRITEVERTEX3D, *PD3DSPRITEVERTEX3D;

typedef struct __FONTCOORDS
{
    FLOAT   tx1, tx2, ty1, ty2;
    FLOAT   fWidth, fHeight;
} FONTCOORDS, *PFONTCOORDS;

class CD3DMGEng
{
public:

    CD3DMGEng ( LPDIRECT3DDEVICE8 pD3DDevice );
    ~CD3DMGEng () {};

    VOID OnDeleteDevice ( void );
    VOID OnRestoreDevice ( void );
    VOID OnInvalidateDevice ( void );

    BOOL Render2DSpriteEx ( LPDIRECT3DTEXTURE8 pTexture, D3DXVECTOR2 *pScaling, D3DXVECTOR2 *pTranslation, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR );
    BOOL Render2DSprite ( LPDIRECT3DTEXTURE8 pTexture, D3DXVECTOR2 *pScaling, D3DXVECTOR2 *pTranslation, DWORD dwColor );
    BOOL Render3DSpriteEx ( LPDIRECT3DTEXTURE8 pTexture, FLOAT fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR, BOOL bIgnoreSetup );
    BOOL Render3DSprite ( LPDIRECT3DTEXTURE8 pTexture, FLOAT fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColor, BOOL bIgnoreSetup);

   // BOOL Render3DTriangle ( LPDIRECT3DTEXTURE8 pTexture, FLOAT fScale, D3DXVECTOR3 *pTranslation, D3DXMATRIX *pMatView, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorB );

    BOOL BeginDrawing ();
    BOOL EndDrawing ();

    BOOL Render2DBox ( FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight, DWORD dwColor );
    BOOL Render2DBoxEx ( FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight, DWORD dwColorTL, DWORD dwColorTR, DWORD dwColorBL, DWORD dwColorBR );
    BOOL Render3DBox ( FLOAT x, FLOAT y, FLOAT z, FLOAT fLWH, DWORD dwColor );

    BOOL LoadFontTextureFromMem ( PVOID pvMemory, DWORD dwSizeOfFile );
    BOOL LoadFontTextureFromFile ( TCHAR *wszFileName );
    BOOL LoadFontInfoFromMem ( PVOID pvMemory );
    BOOL LoadFontInfoFromFile ( TCHAR *wszFileName );
    BOOL LoadDefaultFontInfo ( VOID );
    BOOL UnloadFont ( VOID );

    BOOL DrawText2D ( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR *wszText,  FLOAT fTextSize = 0.0f, FONTFLAGS ffFlags = D3DFF_LEFT, FONTSET fsSet = D3DFS_FONTSET1 );

    BOOL UTIL_FormatText ( const TCHAR* wszInputText, TCHAR* wszOutputText, float fMaxWidth, FLOAT fTextSize = 0.0f, FONTSET fsSet = D3DFS_FONTSET1 );
    BOOL UTIL_GetTextExtent ( const TCHAR* strText, SIZE* pSize, FLOAT fTextSize = 0.0f, FONTSET fsSet = D3DFS_FONTSET1 );

private:

    //////////////////////////////////////////
    // Font engine related variables.
    LPDIRECT3DTEXTURE8      m_pFontTexture; 
    FLOAT                   m_fFontTexWidth;
    FLOAT                   m_fFontTexHeight;
    FONTCOORDS              m_aFontCoords[D3DFE_CHARACTERSPERROW*D3DFE_CHARACTERSPERCOLUMN];
    BOOL                    m_bFontInfoLoaded;

    //////////////////////////////////////////
    // Generic rendering variables.
    LPDIRECT3DVERTEXBUFFER8 m_pVB;
    LPDIRECT3DDEVICE8       m_pDevice;
    DWORD                   m_dwSavedStateBlock;
    DWORD                   m_dwRenderStateBlock;

};

#endif
