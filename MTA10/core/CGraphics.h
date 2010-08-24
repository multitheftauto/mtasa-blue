/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CGraphics.h
*  PURPOSE:     Header file for general graphics subsystem class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CGraphics;

#ifndef __CGRAPHICS_H
#define __CGRAPHICS_H

#include <core/CGraphicsInterface.h>
#include <gui/CGUI.h>
#include "CGUI.h"
#include "CSingleton.h"

struct IDirect3DDevice9;
struct IDirect3DSurface9;

struct SCachedTextureInfo
{
    IDirect3DTexture9*  d3dTexture;
    unsigned long       ulTimeLastUsed;
    unsigned int        uiSurfaceWidth;
    unsigned int        uiSurfaceHeight;
    unsigned int        uiFileWidth;
    unsigned int        uiFileHeight;
};

class CGraphics : public CGraphicsInterface, public CSingleton < CGraphics >
{
    friend class CDirect3DEvents9;
public:
                        CGraphics               ( CLocalGUI* pGUI );
                        ~CGraphics              ( void );

    // DirectX misc. functions
    void                BeginDrawing            ( void );
    void                EndDrawing              ( void );

    inline void         SetRenderTarget         ( IDirect3DSurface9 * pSurface )    { m_pRenderTarget = pSurface; }

    inline IDirect3DDevice9 *   GetDevice       ( void )            { return m_pDevice; };

    void                BeginSingleDrawing      ( void );
    void                EndSingleDrawing        ( void );

    void                SetCEGUIUsed            ( bool bCEGUI )     { m_bCEGUI = bCEGUI; };
    bool                GetCEGUIUsed            ( void )            { return m_bCEGUI; };

    // Transformation functions
    void                CalcWorldCoors          ( CVector * vecScreen, CVector * vecWorld );
    void                CalcScreenCoors         ( CVector * vecWorld, CVector * vecScreen );

    // DirectX drawing functions
    void                Draw3DBox               ( float fX, float fY, float fZ, float fL, float fW, float fH, DWORD dwColor, bool bWireframe = false );
    void                DrawText                ( int iLeft, int iTop, int iRight, int iBottom, unsigned long dwColor, const char* wszText, float fScaleX, float fScaleY, unsigned long ulFormat, ID3DXFont * pDXFont = NULL );
    void                DrawText                ( int iX, int iY, unsigned long dwColor, float fScale, const char * szText, ... );
    void                DrawTextCEGUI           ( int uiLeft, int uiTop, int uiRight, int uiBottom, unsigned long ulColor, const char* szText, float fScale, unsigned long ulFormat );
    void                DrawText2DA             ( int uiX, int uiY, unsigned long ulColor, float fScale, const char* szDisplayText, ... );
    void                DrawText3DA             ( float fX, float fY, float fZ, unsigned long ulColor, float fScale, const char* szDisplayText, ... );
    void                DrawLine                ( float fX1, float fY1, float fX2, float fY2, unsigned long ulColor );
    void                DrawLine3D              ( const CVector& vecBegin, const CVector& vecEnd, unsigned long ulColor, float fWidth = 1.0f );
    void                DrawRectangle           ( float fX, float fY, float fWidth, float fHeight, unsigned long ulColor );

    void                Render3DSprite          ( float fX, float fY, float fZ, float fScale, unsigned long ulColor );

    unsigned int        GetViewportWidth        ( void );
    unsigned int        GetViewportHeight       ( void );

    // DirectX font functions
    ID3DXFont *         GetFont                 ( eFontType fontType = FONT_DEFAULT );
    eFontType           GetFontType             ( const char* szFontName );

    bool                LoadFonts               ( void );
    bool                DestroyFonts            ( void );

    float               GetDXFontHeight         ( float fScale = 1.0f, ID3DXFont * pDXFont = NULL );
    float               GetDXCharacterWidth     ( char c, float fScale = 1.0f, ID3DXFont * pDXFont = NULL );
    float               GetDXTextExtent         ( const char * szText, float fScale = 1.0f, ID3DXFont * pDXFont = NULL );

    int                 GetTextExtent           ( const char* szText, float fScale );
    int                 GetTextHeight           ( float fScale );

    // Textures
    IDirect3DTexture9*  CreateTexture           ( DWORD* dwBitMap, unsigned int uiWidth, unsigned int uiHeight );
    IDirect3DTexture9*  LoadTexture             ( const char* szFile );
    IDirect3DTexture9*  LoadTexture             ( const char* szFile, unsigned int uiWidth, unsigned int uiHeight );
    void                DrawTexture             ( IDirect3DTexture9* texture, float fX, float fY, float fScaleX = 1.0f, float fScaleY = 1.0f, float fRotation = 0.0f, float fCenterX = 0.0f, float fCenterY = 0.0f, unsigned char ucAlpha = 255 );

    // Interface functions
    void                SetCursorPosition       ( int iX, int iY, DWORD Flags );

    // Queued up drawing funcs
    void                DrawLineQueued          ( float fX1, float fY1,
                                                  float fX2, float fY2,
                                                  float fWidth,
                                                  unsigned long ulColor,
                                                  bool bPostGUI );

    void                DrawLine3DQueued        ( const CVector& vecBegin,
                                                  const CVector& vecEnd,
                                                  float fWidth,
                                                  unsigned long ulColor,
                                                  bool bPostGUI );

    void                DrawRectQueued          ( float fX, float fY,
                                                  float fWidth, float fHeight,
                                                  unsigned long ulColor,
                                                  bool bPostGUI );

    bool                DrawTextureQueued       ( float fX, float fY,
                                                  float fWidth, float fHeight,
                                                  float fU, float fV,
                                                  float fSizeU, float fSizeV, 
                                                  bool bRelativeUV,
                                                  const std::string& strFilename,
                                                  float fRotation,
                                                  float fRotCenOffX,
                                                  float fRotCenOffY,
                                                  unsigned long ulColor,
                                                  bool bPostGUI );

    void                DrawTextQueued          ( int iLeft, int iTop,
                                                  int iRight, int iBottom,
                                                  unsigned long dwColor,
                                                  const char* wszText,
                                                  float fScaleX,
                                                  float fScaleY,
                                                  unsigned long ulFormat,
                                                  ID3DXFont * pDXFont = NULL,
                                                  bool bPostGUI = false );

    // To draw queued up drawings
    void                DrawPreGUIQueue         ( void );
    void                DrawPostGUIQueue        ( void );
    
private:
    void                OnDeviceCreate          ( IDirect3DDevice9 * pDevice );
    void                OnDeviceInvalidate      ( IDirect3DDevice9 * pDevice );
    void                OnDeviceRestore         ( IDirect3DDevice9 * pDevice );
    SCachedTextureInfo& CacheTexture            ( const std::string& strFilename );
    void                ExpireCachedTextures    ( bool bExpireAll = false );
    ID3DXFont*          GetBigFont              ( ID3DXFont* pDXFont );

    CLocalGUI*          m_pGUI;
    CGUIFont*           m_pFont;

    bool                m_bIsDrawing;
    bool                m_bCEGUI;

    LPD3DXSPRITE        m_pDXSprite;
    IDirect3DTexture9 * m_pDXPixelTexture;

    IDirect3DSurface9 * m_pRenderTarget;
    IDirect3DSurface9 * m_pOriginalTarget;

    IDirect3DDevice9 *  m_pDevice;

    // Fonts
    ID3DXFont*          m_pDXFonts [ NUM_FONTS ];
    ID3DXFont*          m_pBigDXFonts [ NUM_FONTS ];

    /*
    ID3DXFont *         m_pDXDefaultFont;
    ID3DXFont *         m_pDXDefaultBoldFont;
    ID3DXFont *         m_pDXClearFont;
    ID3DXFont *         m_pDXArialFont;
    ID3DXFont *         m_pDXSansFont;
    ID3DXFont *         m_pDXPricedownFont;
    ID3DXFont *         m_pDXBankGothicFont;
    ID3DXFont *         m_pDXDiplomaFont;
    ID3DXFont *         m_pDXBeckettFont;

    ID3DXFont *         m_pDXBigDefaultFont;
    */

    // ******* Drawing queue code ***********
    // Used by client scripts to draw DX stuff. Rather than drawing immediately,
    // we delay drawing so we can keep renderstates and perform drawing after
    // the GUI has been drawn.
    enum eDrawQueueType
    {
        QUEUE_LINE,
        QUEUE_LINE3D,
        QUEUE_TEXT,
        QUEUE_RECT,
        QUEUE_CIRCLE,
        QUEUE_TEXTURE,
    };

    struct sDrawQueueLine
    {
        float           fX1;
        float           fY1;
        float           fX2;
        float           fY2;
        float           fWidth;
        unsigned long   ulColor;
    };

    struct sDrawQueueLine3D
    {
        float           fX1;
        float           fY1;
        float           fZ1;
        float           fX2;
        float           fY2;
        float           fZ2;
        float           fWidth;
        unsigned long   ulColor;
    };

    struct sDrawQueueText
    {
        int             iLeft;
        int             iTop;
        int             iRight;
        int             iBottom;
        unsigned long   ulColor;
        float           fScaleX;
        float           fScaleY;
        unsigned long   ulFormat;
        ID3DXFont*      pDXFont;
    };

    struct sDrawQueueRect
    {
        float           fX;
        float           fY;
        float           fWidth;
        float           fHeight;
        unsigned long   ulColor;
    };

    struct sDrawQueueCircle
    {
        float           fX;
        float           fY;
        float           fRadius;
        unsigned long   ulColor;
    };

    struct sDrawQueueTexture
    {
        SCachedTextureInfo info;
        float           fX;
        float           fY;
        float           fWidth;
        float           fHeight;
        float           fU;
        float           fV;
        float           fSizeU;
        float           fSizeV;
        float           fRotation;
        float           fRotCenOffX;
        float           fRotCenOffY;
        unsigned long   ulColor;
        bool            bRelativeUV;
    };

    struct sDrawQueueItem
    {
        eDrawQueueType      eType;
        std::wstring        strText;

        // Queue item data based on the eType.
        union
        {
            sDrawQueueLine          Line;
            sDrawQueueLine3D        Line3D;
            sDrawQueueText          Text;
            sDrawQueueRect          Rect;
            sDrawQueueCircle        Circle;
            sDrawQueueTexture       Texture;
        };
    };


    struct sFontInfo
    {
        const char* szName;
        unsigned int uiHeight;
        unsigned int uiWeight;
    };

    // Drawing queues
    std::vector < sDrawQueueItem >      m_PreGUIQueue;
    std::vector < sDrawQueueItem >      m_PostGUIQueue;

    bool                                IsDrawQueueItemSprite   ( const sDrawQueueItem& Item );
    void                                AddQueueItem            ( const sDrawQueueItem& Item, bool bPostGUI );
    void                                DrawQueueItem           ( const sDrawQueueItem& Item );
    void                                DrawQueue               ( std::vector < sDrawQueueItem >& Queue );

    // Drawing types
    struct ID3DXLine*                   m_pLineInterface;

    std::map < std::string, SCachedTextureInfo > m_CachedTextureInfoMap;
};

#endif
