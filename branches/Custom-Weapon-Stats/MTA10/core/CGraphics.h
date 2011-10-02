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
#include <CRenderItemManager.h>

class CTileBatcher;
class CLine3DBatcher;
struct IDirect3DDevice9;
struct IDirect3DSurface9;

class CGraphics : public CGraphicsInterface, public CSingleton < CGraphics >
{
    friend class CDirect3DEvents9;
public:
                        CGraphics               ( CLocalGUI* pGUI );
                        ~CGraphics              ( void );

    // DirectX misc. functions
    inline IDirect3DDevice9 *   GetDevice       ( void )            { return m_pDevice; };

    // Transformation functions
    void                CalcWorldCoors          ( CVector * vecScreen, CVector * vecWorld );
    void                CalcScreenCoors         ( CVector * vecWorld, CVector * vecScreen );

    // DirectX drawing functions
    void                DrawText                ( int iLeft, int iTop, int iRight, int iBottom, unsigned long dwColor, const char* wszText, float fScaleX, float fScaleY, unsigned long ulFormat, ID3DXFont * pDXFont = NULL );
    void                DrawText                ( int iX, int iY, unsigned long dwColor, float fScale, const char * szText, ... );
    void                DrawLine3D              ( const CVector& vecBegin, const CVector& vecEnd, unsigned long ulColor, float fWidth = 1.0f );
    void                DrawRectangle           ( float fX, float fY, float fWidth, float fHeight, unsigned long ulColor );

    unsigned int        GetViewportWidth        ( void );
    unsigned int        GetViewportHeight       ( void );

    // DirectX font functions
    ID3DXFont *         GetFont                 ( eFontType fontType = FONT_DEFAULT );
    eFontType           GetFontType             ( const char* szFontName );

    bool                LoadStandardDXFonts     ( void );
    bool                DestroyStandardDXFonts  ( void );

    bool                LoadAdditionalDXFont    ( std::string strFontPath, std::string strFontName, unsigned int uiHeight, bool bBold, ID3DXFont** ppD3DXFont );
    bool                DestroyAdditionalDXFont ( std::string strFontPath, ID3DXFont* pD3DXFont );

    float               GetDXFontHeight         ( float fScale = 1.0f, ID3DXFont * pDXFont = NULL );
    float               GetDXCharacterWidth     ( char c, float fScale = 1.0f, ID3DXFont * pDXFont = NULL );
    float               GetDXTextExtent         ( const char * szText, float fScale = 1.0f, ID3DXFont * pDXFont = NULL );

    // Textures
    void                DrawTexture             ( CTextureItem* texture, float fX, float fY, float fScaleX = 1.0f, float fScaleY = 1.0f, float fRotation = 0.0f, float fCenterX = 0.0f, float fCenterY = 0.0f, DWORD dwColor = 0xFFFFFFFF );

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

    void                DrawTextureQueued       ( float fX, float fY,
                                                  float fWidth, float fHeight,
                                                  float fU, float fV,
                                                  float fSizeU, float fSizeV, 
                                                  bool bRelativeUV,
                                                  CMaterialItem* pMaterial,
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

    bool                CanSetRenderTarget      ( void )                { return m_bSetRenderTargetEnabled; }
    void                EnableSetRenderTarget   ( bool bEnable );
    void                OnChangingRenderTarget  ( uint uiNewViewportSizeX, uint uiNewViewportSizeY );

    // Subsystems
    CRenderItemManagerInterface* GetRenderItemManager   ( void )        { return m_pRenderItemManager; }

    // To draw queued up drawings
    void                DrawPreGUIQueue         ( void );
    void                DrawPostGUIQueue        ( void );
    
private:
    void                OnDeviceCreate          ( IDirect3DDevice9 * pDevice );
    void                OnDeviceInvalidate      ( IDirect3DDevice9 * pDevice );
    void                OnDeviceRestore         ( IDirect3DDevice9 * pDevice );
    ID3DXFont*          MaybeGetBigFont         ( ID3DXFont* pDXFont, float& fScaleX, float& fScaleY );

    CLocalGUI*          m_pGUI;

    int                 m_iDebugQueueRefs;

    LPD3DXSPRITE        m_pDXSprite;
    IDirect3DTexture9 * m_pDXPixelTexture;

    IDirect3DDevice9 *  m_pDevice;

    CRenderItemManager* m_pRenderItemManager;
    CTileBatcher*       m_pTileBatcher;
    CLine3DBatcher*     m_pLine3DBatcherPreGUI;
    CLine3DBatcher*     m_pLine3DBatcherPostGUI;
    bool                m_bSetRenderTargetEnabled;

    // Fonts
    ID3DXFont*          m_pDXFonts [ NUM_FONTS ];
    ID3DXFont*          m_pBigDXFonts [ NUM_FONTS ];

    std::vector < SString > m_FontResourceNames;

    // ******* Drawing queue code ***********
    // Used by client scripts to draw DX stuff. Rather than drawing immediately,
    // we delay drawing so we can keep renderstates and perform drawing after
    // the GUI has been drawn.
    enum eDrawQueueType
    {
        QUEUE_LINE,
        QUEUE_TEXT,
        QUEUE_RECT,
        QUEUE_CIRCLE,
        QUEUE_TEXTURE,
        QUEUE_SHADER,
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
        CMaterialItem*  pMaterial;
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

    void                                HandleDrawQueueModeChange ( uint curMode, uint newMode );
    void                                AddQueueItem            ( const sDrawQueueItem& Item, bool bPostGUI );
    void                                DrawQueueItem           ( const sDrawQueueItem& Item );
    void                                DrawQueue               ( std::vector < sDrawQueueItem >& Queue );
    void                                ClearDrawQueue          ( std::vector < sDrawQueueItem >& Queue );
    void                                AddQueueRef             ( CRenderItem* pRenderItem );
    void                                RemoveQueueRef          ( CRenderItem* pRenderItem );
    void                                AddQueueRef             ( IUnknown* pUnknown );
    void                                RemoveQueueRef          ( IUnknown* pUnknown );

    // Drawing types
    struct ID3DXLine*                   m_pLineInterface;
};

#endif
