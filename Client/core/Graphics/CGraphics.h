/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CGraphics.h
 *  PURPOSE:     Header file for general graphics subsystem class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CGraphics;

#pragma once

#include <core/CGraphicsInterface.h>
#include <gui/CGUI.h>
#include "CGUI.h"
#include "CSingleton.h"
#include "CRenderItemManager.h"
#include <effekseer/CEffekseer.h>

#define DUMMY_PROGRESS_INITIAL_DELAY        1000    // Game stall time before spinner is displayed
#define DUMMY_PROGRESS_MIN_DISPLAY_TIME     1000    // Minimum time spinner is drawn (to prevent flicker)
#define DUMMY_PROGRESS_ANIMATION_INTERVAL   100     // Animation speed

class CTileBatcher;
class CLine3DBatcher;
class CMaterialLine3DBatcher;
class CPrimitiveBatcher;
class CPrimitiveMaterialBatcher;
class CPrimitive3DBatcher;
class CMaterialPrimitive3DBatcher;
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
        PRIMITIVE,
        PRIMITIVE_MATERIAL
    };
}
using EDrawMode::EDrawModeType;

struct SCustomScaleFontInfo
{
    SCustomScaleFontInfo() : fScale(0), fontType(FONT_DEFAULT), pFont(NULL) {}
    float      fScale;
    eFontType  fontType;
    ID3DXFont* pFont;
};

struct sFontInfo
{
    const char*  szName;
    unsigned int uiHeight;
    unsigned int uiWeight;
};

class CGraphics : public CGraphicsInterface, public CSingleton<CGraphics>
{
    friend class CDirect3DEvents9;
    friend CPrimitiveMaterialBatcher;
    friend CMaterialPrimitive3DBatcher;

public:
    ZERO_ON_NEW
    CGraphics(CLocalGUI* pGUI);
    ~CGraphics();

    // DirectX misc. functions
    IDirect3DDevice9* GetDevice() { return m_pDevice; };

    // Transformation functions
    void CalcWorldCoors(CVector* vecScreen, CVector* vecWorld);
    void CalcScreenCoors(CVector* vecWorld, CVector* vecScreen);

    // DirectX drawing functions
    void DrawString(int iLeft, int iTop, int iRight, int iBottom, unsigned long dwColor, const char* wszText, float fScaleX, float fScaleY,
                    unsigned long ulFormat, ID3DXFont* pDXFont = NULL, bool bOutline = false);
    void DrawString(int iX, int iY, unsigned long dwColor, float fScale, const char* szText, ...);
    void DrawLine3D(const CVector& vecBegin, const CVector& vecEnd, unsigned long ulColor, float fWidth = 1.0f);
    void DrawRectangle(float fX, float fY, float fWidth, float fHeight, unsigned long ulColor, bool bSubPixelPositioning = false);
    void DrawStringOutline(const RECT& rect, unsigned long ulColor, const wchar_t* szText, unsigned long ulFormat, LPD3DXFONT pDXFont);

    void           SetBlendMode(EBlendModeType blendMode);
    EBlendModeType GetBlendMode();
    SColor         ModifyColorForBlendMode(SColor color, EBlendModeType blendMode);
    void           BeginDrawBatch(EBlendModeType blendMode = EBlendMode::NONE);
    void           EndDrawBatch();
    void           SetBlendModeRenderStates(EBlendModeType blendMode);

    unsigned int GetViewportWidth();
    unsigned int GetViewportHeight();

    void  SetAspectRatioAdjustmentEnabled(bool bEnabled, float fSourceRatio = 4 / 3.f);
    bool  IsAspectRatioAdjustmentEnabled();
    float GetAspectRatioAdjustmentSourceRatio();
    void  SetAspectRatioAdjustmentSuspended(bool bSuspended);
    float ConvertPositionForAspectRatio(float fY);
    void  ConvertSideForAspectRatio(float* pfY, float* pfHeight);

    // DirectX font functions
    ID3DXFont* GetFont(eFontType fontType = FONT_DEFAULT, float* pfOutScaleUsed = NULL, float fRequestedScale = 1, const char* szCustomScaleUser = NULL);

    bool LoadStandardDXFonts();
    bool DestroyStandardDXFonts();
    bool CreateStandardDXFontWithCustomScale(eFontType fontType, float fScale, ID3DXFont** ppD3DXFont);

    bool LoadAdditionalDXFont(std::string strFontPath, std::string strFontName, unsigned int uiHeight, bool bBold, ID3DXFont** ppD3DXFont);
    bool LoadAdditionalDXFont(std::string strFontPath, std::string strFontName, unsigned int uiHeight, bool bBold, DWORD ulQuality, ID3DXFont** ppD3DXFont);
    bool DestroyAdditionalDXFont(std::string strFontPath, ID3DXFont* pD3DXFont);

    float GetDXFontHeight(float fScale = 1.0f, ID3DXFont* pDXFont = NULL);
    float GetDXCharacterWidth(char c, float fScale = 1.0f, ID3DXFont* pDXFont = NULL);
    float GetDXTextExtent(const char* szText, float fScale = 1.0f, ID3DXFont* pDXFont = NULL, bool bColorCoded = false);
    float GetDXTextExtentW(const wchar_t* wszText, float fScale = 1.0f, LPD3DXFONT pDXFont = NULL);
    void  GetDXTextSize(CVector2D& vecSize, const char* szText, float fWidth = 0, float fScaleX = 1.0f, float fScaleY = 1.0f, LPD3DXFONT pDXFont = nullptr,
                        bool bWordBreak = false, bool bColorCoded = false);

    // Textures
    void DrawTexture(CTextureItem* texture, float fX, float fY, float fScaleX = 1.0f, float fScaleY = 1.0f, float fRotation = 0.0f, float fCenterX = 0.0f,
                     float fCenterY = 0.0f, DWORD dwColor = 0xFFFFFFFF, float fU = 0, float fV = 0, float fSizeU = 1, float fSizeV = 1,
                     bool bRelativeUV = true);

    // Interface functions
    void SetCursorPosition(int iX, int iY, DWORD Flags);

    // Queued up drawing funcs
    void DrawLineQueued(float fX1, float fY1, float fX2, float fY2, float fWidth, unsigned long ulColor, bool bPostGUI);

    void DrawLine3DQueued(const CVector& vecBegin, const CVector& vecEnd, float fWidth, unsigned long ulColor, bool bPostGUI);

    void DrawMaterialLine3DQueued(const CVector& vecBegin, const CVector& vecEnd, float fWidth, unsigned long ulColor, CMaterialItem* pMaterial, float fU = 0,
                                  float fV = 0, float fSizeU = 1, float fSizeV = 1, bool bRelativeUV = true, bool bFlipUV = false, bool bUseFaceToward = false,
                                  const CVector& vecFaceToward = CVector(), bool bPostGUI = false) override;

    void DrawRectQueued(float fX, float fY, float fWidth, float fHeight, unsigned long ulColor, bool bPostGUI, bool bSubPixelPositioning = false);

    void DrawTextureQueued(float fX, float fY, float fWidth, float fHeight, float fU, float fV, float fSizeU, float fSizeV, bool bRelativeUV,
                           CMaterialItem* pMaterial, float fRotation, float fRotCenOffX, float fRotCenOffY, unsigned long ulColor, bool bPostGUI);

    void DrawStringQueued(float iLeft, float iTop, float iRight, float iBottom, unsigned long dwColor, const char* wszText, float fScaleX, float fScaleY,
                          unsigned long ulFormat, ID3DXFont* pDXFont = NULL, bool bPostGUI = false, bool bColorCoded = false, bool bSubPixelPositioning = false,
                          float fRotation = 0, float fRotationCenterX = 0, float fRotationCenterY = 0, float fLineHeight = 0);

    void DrawPrimitiveQueued(std::vector<PrimitiveVertice>* pVecVertices, D3DPRIMITIVETYPE eType, bool bPostGUI = false);
    void DrawMaterialPrimitiveQueued(std::vector<PrimitiveMaterialVertice>* vertices, D3DPRIMITIVETYPE type, CMaterialItem* pMaterial, bool bPostGUI);

    void DrawPrimitive3DQueued(std::vector<PrimitiveVertice>* pVecVertices, D3DPRIMITIVETYPE eType, bool bPostGUI);
    void DrawMaterialPrimitive3DQueued(std::vector<PrimitiveMaterialVertice>* pVecVertices, D3DPRIMITIVETYPE eType, CMaterialItem* pMaterial, bool bPostGUI);

    void DrawCircleQueued(float fX, float fY, float fRadius, float fStartAngle, float fStopAngle, unsigned long ulColor, unsigned long ulColorCenter,
                          short siSegments, float fRatio, bool bPostGUI);

    void DrawWiredSphere(CVector vecPosition, float radius, SColor color, float fLineWidth, int iterations);

    bool IsValidPrimitiveSize(int iNumVertives, D3DPRIMITIVETYPE eType);

    void OnChangingRenderTarget(uint uiNewViewportSizeX, uint uiNewViewportSizeY);

    // Subsystems
    CRenderItemManagerInterface* GetRenderItemManager() { return m_pRenderItemManager; }
    CScreenGrabberInterface*     GetScreenGrabber() { return m_pScreenGrabber; }
    CPixelsManagerInterface*     GetPixelsManager() { return m_pPixelsManager; }

    // Transition between GTA and MTA controlled rendering
    virtual void EnteringMTARenderZone();
    virtual void LeavingMTARenderZone();
    virtual void MaybeEnteringMTARenderZone();
    virtual void MaybeLeavingMTARenderZone();
    void         SaveGTARenderStates();
    void         RestoreGTARenderStates();

    // Texture data manipulation
    bool ResizeTextureData(const void* pData, uint uiDataPitch, uint uiWidth, uint uiHeight, uint d3dFormat, uint uiNewWidth, uint uiNewHeight,
                           CBuffer& outBuffer);
    bool CopyDataToSurface(IDirect3DSurface9* pSurface, const uchar* pPixelsData, uint uiDataPitch);
    bool CopyDataFromSurface(IDirect3DSurface9* pSurface, CBuffer& outBuffer);

    // To draw queued up drawings
    void DrawPreGUIQueue(void);
    void DrawPostGUIQueue(void);
    void DrawLine3DPreGUIQueue(void);
    bool HasLine3DPreGUIQueueItems(void);
    void DrawPrimitive3DPreGUIQueue(void);
    bool HasPrimitive3DPreGUIQueueItems(void);
    void DrawEffekseerEffects();

    void DidRenderScene();
    void SetProgressMessage(const SString& strMessage);
    void DrawProgressMessage(bool bPreserveBackbuffer = true);
    void DrawRectangleInternal(float fX, float fY, float fWidth, float fHeight, unsigned long ulColor, bool bSubPixelPositioning);

private:
    void       OnDeviceCreate(IDirect3DDevice9* pDevice);
    void       OnDeviceInvalidate(IDirect3DDevice9* pDevice);
    void       OnDeviceRestore(IDirect3DDevice9* pDevice);
    void       OnZBufferModified();
    ID3DXFont* MaybeGetBigFont(ID3DXFont* pDXFont, float& fScaleX, float& fScaleY);
    void       CheckModes(EDrawModeType newDrawMode, EBlendModeType newBlendMode = EBlendMode::NONE);
    void       DrawColorCodedTextLine(float fLeft, float fRight, float fY, SColor& currentColor, const wchar_t* wszText, float fScaleX, float fScaleY,
                                      unsigned long ulFormat, ID3DXFont* pDXFont, bool bPostGUI, bool bSubPixelPositioning, float fRotation, float fRotationCenterX,
                                      float fRotationCenterY);
    int        GetTrailingSpacesWidth(ID3DXFont* pDXFont, WString& strText);

    CLocalGUI* m_pGUI;

    int            m_iDebugQueueRefs;
    int            m_iDrawBatchRefCount;
    EBlendModeType m_ActiveBlendMode;
    EDrawModeType  m_CurDrawMode;
    EBlendModeType m_CurBlendMode;

    LPD3DXSPRITE m_pDXSprite = nullptr;

    IDirect3DDevice9* m_pDevice = nullptr;

    CRenderItemManager*          m_pRenderItemManager = nullptr;
    CScreenGrabberInterface*     m_pScreenGrabber = nullptr;
    CPixelsManagerInterface*     m_pPixelsManager = nullptr;
    CTileBatcher*                m_pTileBatcher = nullptr;
    CLine3DBatcher*              m_pLine3DBatcherPreGUI = nullptr;
    CLine3DBatcher*              m_pLine3DBatcherPostGUI = nullptr;
    CMaterialLine3DBatcher*      m_pMaterialLine3DBatcherPreGUI = nullptr;
    CMaterialLine3DBatcher*      m_pMaterialLine3DBatcherPostGUI = nullptr;
    CPrimitiveBatcher*           m_pPrimitiveBatcher = nullptr;
    CPrimitiveMaterialBatcher*   m_pPrimitiveMaterialBatcher = nullptr;
    CPrimitive3DBatcher*         m_pPrimitive3DBatcherPreGUI = nullptr;
    CPrimitive3DBatcher*         m_pPrimitive3DBatcherPostGUI = nullptr;
    CMaterialPrimitive3DBatcher* m_pMaterialPrimitive3DBatcherPreGUI = nullptr;
    CMaterialPrimitive3DBatcher* m_pMaterialPrimitive3DBatcherPostGUI = nullptr;
    CAspectRatioConverter*       m_pAspectRatioConverter = nullptr;

    // Fonts
    ID3DXFont* m_pDXFonts[NUM_FONTS];
    ID3DXFont* m_pBigDXFonts[NUM_FONTS];

    std::vector<SString> m_FontResourceNames;

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
        QUEUE_PRIMITIVE,
        QUEUE_PRIMITIVEMATERIAL,
    };

    struct sDrawQueueLine
    {
        float         fX1;
        float         fY1;
        float         fX2;
        float         fY2;
        float         fWidth;
        unsigned long ulColor;
    };

    struct sDrawQueueText
    {
        float         fLeft;
        float         fTop;
        float         fRight;
        float         fBottom;
        unsigned long ulColor;
        float         fScaleX;
        float         fScaleY;
        unsigned long ulFormat;
        ID3DXFont*    pDXFont;
        float         fRotation;
        float         fRotationCenterX;
        float         fRotationCenterY;
    };

    struct sDrawQueueRect
    {
        float         fX;
        float         fY;
        float         fWidth;
        float         fHeight;
        unsigned long ulColor;
        bool          bSubPixelPositioning;
    };

    struct sDrawQueueTexture
    {
        CMaterialItem* pMaterial;
        float          fX;
        float          fY;
        float          fWidth;
        float          fHeight;
        float          fU;
        float          fV;
        float          fSizeU;
        float          fSizeV;
        float          fRotation;
        float          fRotCenOffX;
        float          fRotCenOffY;
        unsigned long  ulColor;
        bool           bRelativeUV;
    };

    struct sDrawQueuePrimitive
    {
        D3DPRIMITIVETYPE               eType;
        std::vector<PrimitiveVertice>* pVecVertices;
    };

    struct sDrawQueuePrimitiveMaterial
    {
        D3DPRIMITIVETYPE                       eType;
        CMaterialItem*                         pMaterial;
        std::vector<PrimitiveMaterialVertice>* pVecVertices;
    };

    struct sDrawQueueItem
    {
        eDrawQueueType eType;
        EBlendModeType blendMode;
        std::wstring   wstrText;

        // Queue item data based on the eType.
        union
        {
            sDrawQueueLine              Line;
            sDrawQueueText              Text;
            sDrawQueueRect              Rect;
            sDrawQueueTexture           Texture;
            sDrawQueuePrimitive         Primitive;
            sDrawQueuePrimitiveMaterial PrimitiveMaterial;
        };
    };

    // Drawing queues
    std::vector<sDrawQueueItem> m_PreGUIQueue;
    std::vector<sDrawQueueItem> m_PostGUIQueue;

    void AddQueueItem(const sDrawQueueItem& Item, bool bPostGUI);
    void DrawQueueItem(const sDrawQueueItem& Item);
    void DrawQueue(std::vector<sDrawQueueItem>& Queue);
    void ClearDrawQueue(std::vector<sDrawQueueItem>& Queue);
    void AddQueueRef(CRenderItem* pRenderItem);
    void RemoveQueueRef(CRenderItem* pRenderItem);
    void AddQueueRef(IUnknown* pUnknown);
    void RemoveQueueRef(IUnknown* pUnknown);

    // Drawing types
    struct ID3DXLine* m_pLineInterface = nullptr;

    enum EMTARenderZone
    {
        MTA_RZONE_NONE,
        MTA_RZONE_MAIN,               // MTA rendering inside known areas.
        MTA_RZONE_OUTSIDE,            // MTA rendering outside known areas. i.e. During a keypress or GTA callback
    };

    EMTARenderZone                          m_MTARenderZone;
    int                                     m_iOutsideZoneCount = 0;
    IDirect3DStateBlock9*                   m_pSavedStateBlock = nullptr;
    CElapsedTime                            m_LastRenderedSceneTimer;
    IDirect3DSurface9*                      m_pSavedFrontBufferData = nullptr;
    CRenderTargetItem*                      m_pTempBackBufferData = nullptr;
    CTextureItem*                           m_ProgressSpinnerTexture = nullptr;
    CTextureItem*                           m_RectangleEdgeTexture = nullptr;
    SString                                 m_strProgressMessage;
    CElapsedTime                            m_FirstDrawnProgressTimer;
    CElapsedTime                            m_LastDrawnProgressTimer;
    CElapsedTime                            m_LastLostDeviceTimer;
    bool                                    m_bProgressVisible;
    CElapsedTime                            m_ProgressAnimTimer;
    uint                                    m_uiProgressAnimFrame;
    std::map<SString, SCustomScaleFontInfo> m_CustomScaleFontMap;
};
