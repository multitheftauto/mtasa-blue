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

#define DUMMY_PROGRESS_INITIAL_DELAY        1000    // Game stall time before spinner is displayed
#define DUMMY_PROGRESS_MIN_DISPLAY_TIME     1000    // Minimum time spinner is drawn (to prevent flicker)
#define DUMMY_PROGRESS_ANIMATION_INTERVAL   100     // Animation speed

class CTileBatcher;
class CLine3DBatcher;
class CMaterialLine3DBatcher;
class CAspectRatioConverter;
struct IDirect3DDevice9;
struct IDirect3DSurface9;

namespace EDrawMode
{
    enum EDrawModeType
    {
        NONE,
        DX_LINE,
        DX_SPRITE,
        TILE_BATCHER,
    };
}
using EDrawMode::EDrawModeType;

struct SCustomScaleFontInfo
{
    SCustomScaleFontInfo( void ) : fScale( 0 ), fontType( FONT_DEFAULT ), pFont( NULL ) {}
    float fScale;
    eFontType fontType;
    ID3DXFont* pFont;
};

struct sFontInfo
{
    const char* szName;
    unsigned int uiHeight;
    unsigned int uiWeight;
};

class CGraphics : public CGraphicsInterface, public CSingleton < CGraphics >
{
    friend class CDirect3DEvents9;
public:
    ZERO_ON_NEW
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

    void                SetBlendMode            ( EBlendModeType blendMode );
    EBlendModeType      GetBlendMode            ( void );
    SColor              ModifyColorForBlendMode ( SColor color, EBlendModeType blendMode );
    void                BeginDrawBatch          ( EBlendModeType blendMode = EBlendMode::NONE );
    void                EndDrawBatch            ( void );
    void                SetBlendModeRenderStates ( EBlendModeType blendMode );

    unsigned int        GetViewportWidth        ( void );
    unsigned int        GetViewportHeight       ( void );

    void                SetAspectRatioAdjustmentEnabled     ( bool bEnabled, float fSourceRatio = 4/3.f );
    bool                IsAspectRatioAdjustmentEnabled      ( void );
    float               GetAspectRatioAdjustmentSourceRatio ( void );
    void                SetAspectRatioAdjustmentSuspended   ( bool bSuspended );
    float               ConvertPositionForAspectRatio       ( float fY );
    void                ConvertSideForAspectRatio           ( float* pfY, float* pfHeight );

    // DirectX font functions
    ID3DXFont *         GetFont                 ( eFontType fontType = FONT_DEFAULT, float* pfOutScaleUsed = NULL, float fRequestedScale = 1, const char* szCustomScaleUser = NULL );

    bool                LoadStandardDXFonts     ( void );
    bool                DestroyStandardDXFonts  ( void );
    bool                CreateStandardDXFontWithCustomScale ( eFontType fontType, float fScale, ID3DXFont** ppD3DXFont );

    bool                LoadAdditionalDXFont    ( std::string strFontPath, std::string strFontName, unsigned int uiHeight, bool bBold, ID3DXFont** ppD3DXFont );
    bool                DestroyAdditionalDXFont ( std::string strFontPath, ID3DXFont* pD3DXFont );

    float               GetDXFontHeight         ( float fScale = 1.0f, ID3DXFont * pDXFont = NULL );
    float               GetDXCharacterWidth     ( char c, float fScale = 1.0f, ID3DXFont * pDXFont = NULL );
    float               GetDXTextExtent         ( const char * szText, float fScale = 1.0f, ID3DXFont * pDXFont = NULL, bool bColorCoded = false );
    float               GetDXTextExtentW        ( const wchar_t* wszText, float fScale = 1.0f, LPD3DXFONT pDXFont = NULL );

    // Textures
    void                DrawTexture             ( CTextureItem* texture, float fX, float fY, float fScaleX = 1.0f, float fScaleY = 1.0f, float fRotation = 0.0f, float fCenterX = 0.0f, float fCenterY = 0.0f, DWORD dwColor = 0xFFFFFFFF, float fU = 0, float fV = 0, float fSizeU = 1, float fSizeV = 1, bool bRelativeUV = true );

    // Interface functions
    void                SetCursorPosition       ( int iX, int iY, DWORD Flags );

    // Queued up drawing funcs
    void                DrawLineQueued          ( float fX1, float fY1,
                                                  float fX2, float fY2,
                                                  float fWidth,
                                                  unsigned long ulColor,
                                                  bool bPostGUI );

    void                DrawLine3DQueued       ( const CVector& vecBegin,
                                                 const CVector& vecEnd,
                                                 float fWidth,
                                                 unsigned long ulColor,
                                                 bool bPostGUI );

    void                DrawMaterialLine3DQueued
                                               ( const CVector& vecBegin,
                                                 const CVector& vecEnd,
                                                 float fWidth,
                                                 unsigned long ulColor,
                                                 CMaterialItem* pMaterial,
                                                 float fU = 0, float fV = 0,
                                                 float fSizeU = 1, float fSizeV = 1, 
                                                 bool bRelativeUV = true,
                                                 bool bUseFaceToward = false,
                                                 const CVector& vecFaceToward = CVector () );

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

    void                DrawTextQueued          ( float iLeft, float iTop,
                                                  float iRight, float iBottom,
                                                  unsigned long dwColor,
                                                  const char* wszText,
                                                  float fScaleX,
                                                  float fScaleY,
                                                  unsigned long ulFormat,
                                                  ID3DXFont * pDXFont = NULL,
                                                  bool bPostGUI = false,
                                                  bool bColorCoded = false,
                                                  bool bSubPixelPositioning = false );

    void                OnChangingRenderTarget  ( uint uiNewViewportSizeX, uint uiNewViewportSizeY );

    // Subsystems
    CRenderItemManagerInterface* GetRenderItemManager   ( void )        { return m_pRenderItemManager; }
    CScreenGrabberInterface*     GetScreenGrabber       ( void )        { return m_pScreenGrabber; }
    CPixelsManagerInterface*     GetPixelsManager       ( void )        { return m_pPixelsManager; }

    // Transition between GTA and MTA controlled rendering
    virtual void                            EnteringMTARenderZone       ( void );
    virtual void                            LeavingMTARenderZone        ( void );
    virtual void                            MaybeEnteringMTARenderZone  ( void );
    virtual void                            MaybeLeavingMTARenderZone   ( void );
    void                                    SaveGTARenderStates         ( void );
    void                                    RestoreGTARenderStates      ( void );

    // To draw queued up drawings
    void                DrawPreGUIQueue         ( void );
    void                DrawPostGUIQueue        ( void );
    void                DrawMaterialLine3DQueue ( void );
    bool                HasMaterialLine3DQueueItems ( void );

    void                DidRenderScene          ( void );
    void                SetProgressMessage      ( const SString& strMessage );
    void                DrawProgressMessage     ( bool bPreserveBackbuffer = true );

private:
    void                OnDeviceCreate          ( IDirect3DDevice9 * pDevice );
    void                OnDeviceInvalidate      ( IDirect3DDevice9 * pDevice );
    void                OnDeviceRestore         ( IDirect3DDevice9 * pDevice );
    void                OnZBufferModified       ( void );
    ID3DXFont*          MaybeGetBigFont         ( ID3DXFont* pDXFont, float& fScaleX, float& fScaleY );
    void                CheckModes              ( EDrawModeType newDrawMode, EBlendModeType newBlendMode = EBlendMode::NONE );
    void                DrawColorCodedTextLine  ( float fLeft, float fRight, float fY, SColor& currentColor, const wchar_t* wszText,
                                                  float fScaleX, float fScaleY, unsigned long ulFormat, ID3DXFont* pDXFont, bool bPostGUI, bool bSubPixelPositioning );

    CLocalGUI*          m_pGUI;

    int                 m_iDebugQueueRefs;
    int                 m_iDrawBatchRefCount;
    EBlendModeType      m_ActiveBlendMode;
    EDrawModeType       m_CurDrawMode;
    EBlendModeType      m_CurBlendMode;

    LPD3DXSPRITE        m_pDXSprite;
    IDirect3DTexture9 * m_pDXPixelTexture;

    IDirect3DDevice9 *  m_pDevice;

    CRenderItemManager*         m_pRenderItemManager;
    CScreenGrabberInterface*    m_pScreenGrabber;
    CPixelsManagerInterface*    m_pPixelsManager;
    CTileBatcher*               m_pTileBatcher;
    CLine3DBatcher*             m_pLine3DBatcherPreGUI;
    CLine3DBatcher*             m_pLine3DBatcherPostGUI;
    CMaterialLine3DBatcher*     m_pMaterialLine3DBatcher;
    CAspectRatioConverter*      m_pAspectRatioConverter;

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
        float           fLeft;
        float           fTop;
        float           fRight;
        float           fBottom;
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
        EBlendModeType      blendMode;
        std::wstring        wstrText;

        // Queue item data based on the eType.
        union
        {
            sDrawQueueLine          Line;
            sDrawQueueText          Text;
            sDrawQueueRect          Rect;
            sDrawQueueTexture       Texture;
        };
    };

    // Drawing queues
    std::vector < sDrawQueueItem >      m_PreGUIQueue;
    std::vector < sDrawQueueItem >      m_PostGUIQueue;

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

    enum EMTARenderZone
    {
        MTA_RZONE_NONE,
        MTA_RZONE_MAIN,     // MTA rendering inside known areas.
        MTA_RZONE_OUTSIDE,  // MTA rendering outside known areas. i.e. During a keypress or GTA callback
    };

    EMTARenderZone                      m_MTARenderZone;
    int                                 m_iOutsideZoneCount;
    IDirect3DStateBlock9*               m_pSavedStateBlock;
    CElapsedTime                        m_LastRenderedSceneTimer;
    IDirect3DSurface9*                  m_pSavedFrontBufferData;
    CRenderTargetItem*                  m_pTempBackBufferData;
    CTextureItem*                       m_ProgressSpinnerTexture;
    SString                             m_strProgressMessage;
    CElapsedTime                        m_FirstDrawnProgressTimer;
    CElapsedTime                        m_LastDrawnProgressTimer;
    CElapsedTime                        m_LastLostDeviceTimer;
    bool                                m_bProgressVisible;
    CElapsedTime                        m_ProgressAnimTimer;
    uint                                m_uiProgressAnimFrame;
    std::map < SString, SCustomScaleFontInfo > m_CustomScaleFontMap;
};

#endif
