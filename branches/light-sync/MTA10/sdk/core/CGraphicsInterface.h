/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CGraphicsInterface.h
*  PURPOSE:     Graphics subsystem interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGRAPHICSINTERFACE_H
#define __CGRAPHICSINTERFACE_H

#include "CVector.h"

struct ID3DXFont;
struct IDirect3DDevice9;
struct IDirect3DTexture9;

enum eFontType
{
    FONT_DEFAULT = 0,
    FONT_DEFAULT_BOLD,
    FONT_CLEAR,
    FONT_ARIAL,
    FONT_SANS,
    FONT_PRICEDOWN,
    FONT_BANKGOTHIC,
    FONT_DIPLOMA,
    FONT_BECKETT,
    FONT_UNIFONT,
    NUM_FONTS
};

class CGraphicsInterface
{
public:
    virtual void                    BeginDrawing        ( void ) = 0;
    virtual void                    EndDrawing          ( void ) = 0;

    virtual IDirect3DDevice9 *      GetDevice           ( void ) = 0;

    virtual void                    Draw3DBox           ( float fX, float fY, float fZ, float fL, float fW, float fH, DWORD dwColor, bool bWireframe ) = 0;
    virtual void                    CalcWorldCoors      ( CVector * vecScreen, CVector * vecWorld ) = 0;
    virtual void                    CalcScreenCoors     ( CVector * vecWorld, CVector * vecScreen ) = 0;

    virtual void                    DrawText            ( int iLeft, int iTop, int iRight, int iBottom, unsigned long dwColor, const char* wszText, float fScaleX, float fScaleY, unsigned long ulFormat, ID3DXFont * pDXFont = NULL ) = 0;
    virtual void                    DrawText            ( int iX, int iY, unsigned long dwColor, float fScale, const char * szText, ... ) = 0;

    virtual void                    DrawText2DA         ( int iX, int iY, unsigned long ulColor, float fScale, const char* szDisplayText, ... ) = 0;
    virtual void                    DrawText3DA         ( float fX, float fY, float fZ, unsigned long ulColor, float fScale, const char* szDisplayText, ... ) = 0;
    virtual void                    DrawLine            ( float fX1, float fY1, float fX2, float fY2, unsigned long ulColor ) = 0;
    virtual void                    DrawLine3D          ( const CVector& vecBegin, const CVector& vecEnd, unsigned long ulColor, float fWidth = 1.0f ) = 0;
    virtual void                    DrawRectangle       ( float fX, float fY, float fWidth, float fHeight, unsigned long ulColor ) = 0;

    virtual void                    Render3DSprite      ( float fX, float fY, float fZ, float fScale, unsigned long ulColor ) = 0;
//    virtual void                    Render3DTriangle    ( float fX, float fY, float fZ, float fScale, unsigned long ulColorTopLeft, unsigned long ulColorTopRight, unsigned long ulColorBottom )=0;

    virtual unsigned int            GetViewportWidth    ( void ) = 0;
    virtual unsigned int            GetViewportHeight   ( void ) = 0;

    virtual float                   GetDXFontHeight     ( float fScale = 1.0f, ID3DXFont * pDXFont = NULL ) = 0;
    virtual float                   GetDXCharacterWidth ( char c, float fScale = 1.0f, ID3DXFont * pDXFont = NULL ) = 0;
    virtual float                   GetDXTextExtent     ( const char * szText, float fScale = 1.0f, ID3DXFont * pDXFont = NULL ) = 0;

    virtual bool                    LoadAdditionalDXFont    ( std::string strFontPath, std::string strFontName, unsigned int uiHeight, bool bBold, ID3DXFont** ppD3DXFont ) = 0;
    virtual bool                    DestroyAdditionalDXFont ( std::string strFontPath, ID3DXFont* pD3DXFont ) = 0;

    virtual ID3DXFont *             GetFont             ( eFontType fontType = FONT_DEFAULT ) = 0;
    virtual eFontType               GetFontType         ( const char* szFontName ) = 0;

    virtual IDirect3DTexture9*      LoadTexture         ( const char* szFile ) = 0;
    virtual IDirect3DTexture9*      LoadTexture         ( const char* szFile, unsigned int uiWidth, unsigned int uiHeight ) = 0;
    virtual void                    DrawTexture         ( IDirect3DTexture9* texture, float fX, float fY, float fScaleX = 1.0f, float fScaleY = 1.0f, float fRotation = 0.0f, float fCenterX = 0.0f, float fCenterY = 0.0f, DWORD dwColor = 0xFFFFFFFF ) = 0;

    // Queued up drawing
    virtual void                    DrawLineQueued      ( float fX1, float fY1,
                                                          float fX2, float fY2,
                                                          float fWidth,
                                                          unsigned long ulColor,
                                                          bool bPostGUI ) = 0;
    virtual void                    DrawLine3DQueued    ( const CVector& vecBegin,
                                                          const CVector& vecEnd,
                                                          float fWidth,
                                                          unsigned long ulColor,
                                                          bool bPostGUI ) = 0;

    virtual void                    DrawRectQueued      ( float fX, float fY,
                                                          float fWidth, float fHeight,
                                                          unsigned long ulColor,
                                                          bool bPostGUI ) = 0;

    virtual void                    DrawTextureQueued   ( float fX, float fY,
                                                          float fWidth, float fHeight,
                                                          float fU, float fV,
                                                          float fSizeU, float fSizeV, 
                                                          bool bRelativeUV,
                                                          CMaterialItem* pMaterial,
                                                          float fRotation,
                                                          float fRotCenOffX,
                                                          float fRotCenOffY,
                                                          unsigned long ulColor,
                                                          bool bPostGUI ) = 0;

    virtual void                    DrawTextQueued      ( int iLeft, int iTop,
                                                          int iRight, int iBottom,
                                                          unsigned long dwColor,
                                                          const char* wszText,
                                                          float fScaleX,
                                                          float fScaleY,
                                                          unsigned long ulFormat,
                                                          ID3DXFont * pDXFont,
                                                          bool bPostGUI ) = 0;

    virtual void                    EnableSetRenderTarget       ( bool bEnable ) = 0;
    // Subsystems
    virtual CRenderItemManagerInterface*   GetRenderItemManager  ( void ) = 0;
};

#endif
