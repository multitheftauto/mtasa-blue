/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CRenderItemManagerInterface.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <CVector.h>
#include <CVector2D.h>

struct ID3DXFont;
struct IDirect3DBaseTexture9;
struct IDirect3DSurface9;
struct IDirect3DDevice9;
struct ID3DXEffect;
typedef LPCSTR D3DXHANDLE;
class CGUIFont;
class CRenderItem;
class CMaterialItem;
class CDxFontItem;
class CGuiFontItem;
class CTextureItem;
class CShaderItem;
class CShaderInstance;
class CRenderTargetItem;
class CScreenSourceItem;
class CWebBrowserItem;
class CRenderItemManager;
class CD3DDUMMY;
class CEffectCloner;
class CPixels;
class CClientEntityBase;
struct SShaderItemLayers;
typedef CShaderItem CSHADERDUMMY;
enum eAspectRatio;
class CWebViewInterface;
class CEffectTemplate;
class CVectorGraphicItem;

#define RDEFAULT ((uint)-1)

enum ERenderFormat
{
    RFORMAT_UNKNOWN,
    RFORMAT_ARGB = 21,            // D3DFMT_A8R8G8B8
    RFORMAT_DXT1 = '1TXD',
    RFORMAT_DXT2 = '2TXD',
    RFORMAT_DXT3 = '3TXD',
    RFORMAT_DXT4 = '4TXD',
    RFORMAT_DXT5 = '5TXD',
    RFORMAT_INTZ = 'ZTNI',
    RFORMAT_DF24 = '42FD',
    RFORMAT_DF16 = '61FD',
    RFORMAT_RAWZ = 'ZWAR',
};

enum ETextureType
{
    TTYPE_TEXTURE = 3,
    TTYPE_VOLUMETEXTURE = 4,
    TTYPE_CUBETEXTURE = 5,
};

enum ETextureAddress
{
    TADDRESS_WRAP = 1,
    TADDRESS_MIRROR = 2,
    TADDRESS_CLAMP = 3,
    TADDRESS_BORDER = 4,
    TADDRESS_MIRRORONCE = 5,
};

enum eDxTestMode
{
    DX_TEST_MODE_NONE,
    DX_TEST_MODE_NO_MEM,
    DX_TEST_MODE_LOW_MEM,
    DX_TEST_MODE_NO_SHADER,
};

struct SDxStatus
{
    eDxTestMode testMode;

    struct
    {
        SString       strName;
        int           iInstalledMemoryKB;
        SString       strPSVersion;
        int           iMaxAnisotropy;
        ERenderFormat depthBufferFormat;
        int           iNumSimultaneousRTs;
    } videoCard;

    struct
    {
        int iNumShadersUsingReadableDepthBuffer;
    } state;

    struct
    {
        int iFreeForMTA;
        int iUsedByFonts;
        int iUsedByTextures;
        int iUsedByRenderTargets;
    } videoMemoryKB;

    struct
    {
        bool         bWindowed;
        int          iFullScreenStyle;
        int          iFXQuality;
        int          iDrawDistance;
        bool         bVolumetricShadows;
        bool         bAllowScreenUpload;
        int          iStreamingMemory;
        bool         b32BitColor;
        bool         bGrassEffect;
        bool         bHeatHaze;
        int          iAnisotropicFiltering;
        int          iAntiAliasing;
        eAspectRatio aspectRatio;
        bool         bHUDMatchAspectRatio;
        float        fFieldOfView;
        bool         bHighDetailVehicles;
        bool         bHighDetailPeds;
        bool         bBlur;
        bool         bCoronaReflections;
        bool         bDynamicPedShadows;
    } settings;
};

using EffectMacroList = std::vector<std::pair<SString, SString>>;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CRenderItemManagerInterface
//
// Interface for client module
//
class CRenderItemManagerInterface
{
public:
    virtual ~CRenderItemManagerInterface() {}

    // CRenderItemManagerInterface
    virtual void                DoPulse() = 0;
    virtual CDxFontItem*        CreateDxFont(const SString& strFullFilePath, uint uiSize, bool bBold, DWORD ulQuality = DEFAULT_QUALITY) = 0;
    virtual CGuiFontItem*       CreateGuiFont(const SString& strFullFilePath, const SString& strFontName, uint uiSize) = 0;
    virtual CTextureItem*       CreateTexture(const SString& strFullFilePath, const CPixels* pPixels = NULL, bool bMipMaps = true, uint uiSizeX = RDEFAULT,
                                              uint uiSizeY = RDEFAULT, ERenderFormat format = RFORMAT_UNKNOWN, ETextureAddress textureAddress = TADDRESS_WRAP,
                                              ETextureType textureType = TTYPE_TEXTURE, uint uiVolumeDepth = 1) = 0;
    virtual CShaderItem*        CreateShader(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus, float fPriority,
                                             float fMaxDistance, bool bLayered, bool bDebug, int iTypeMask, const EffectMacroList& macros) = 0;
    virtual CRenderTargetItem*  CreateRenderTarget(uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel, bool bForce = false) = 0;
    virtual CScreenSourceItem*  CreateScreenSource(uint uiSizeX, uint uiSizeY) = 0;
    virtual CWebBrowserItem*    CreateWebBrowser(uint uiSizeX, uint uiSizeY) = 0;
    virtual CVectorGraphicItem* CreateVectorGraphic(uint uiSizeX, uint uiSizeY) = 0;
    virtual bool                SetRenderTarget(CRenderTargetItem* pItem, bool bClear) = 0;
    virtual void                EnableSetRenderTargetOldVer(bool bEnable) = 0;
    virtual bool                IsSetRenderTargetEnabledOldVer() = 0;
    virtual bool                RestoreDefaultRenderTarget() = 0;
    virtual void                UpdateBackBufferCopy() = 0;
    virtual void                UpdateScreenSource(CScreenSourceItem* pScreenSourceItem, bool bResampleNow) = 0;
    virtual SShaderItemLayers*  GetAppliedShaderForD3DData(CD3DDUMMY* pD3DData) = 0;
    virtual bool                ApplyShaderItemToWorldTexture(CShaderItem* pShaderItem, const SString& strTextureNameMatch, CClientEntityBase* pClientEntity,
                                                              bool bAppendLayers) = 0;
    virtual bool           RemoveShaderItemFromWorldTexture(CShaderItem* pShaderItem, const SString& strTextureNameMatch, CClientEntityBase* pClientEntity) = 0;
    virtual void           RemoveClientEntityRefs(CClientEntityBase* pClientEntity) = 0;
    virtual void           GetVisibleTextureNames(std::vector<SString>& outNameList, const SString& strTextureNameMatch, ushort usModelID) = 0;
    virtual eDxTestMode    GetTestMode() = 0;
    virtual void           SetTestMode(eDxTestMode testMode) = 0;
    virtual void           GetDxStatus(SDxStatus& outStatus) = 0;
    virtual CEffectCloner* GetEffectCloner() = 0;
    virtual void           PreDrawWorld() = 0;
    virtual void           SetDepthBufferFormat(ERenderFormat depthBufferFormat) = 0;
    virtual ERenderFormat  GetDepthBufferFormat() = 0;
    virtual void           SaveReadableDepthBuffer() = 0;
    virtual void           FlushNonAARenderTarget() = 0;
    virtual void           HandleStretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect,
                                             int Filter) = 0;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// SShaderValue - Used in CShaderInstance to store parameter values
//
struct SShaderValue
{
    char cType, cCount;
    union
    {
        CTextureItem* pTextureItem;
        bool          bValue;
        float         floatList[16];
    };
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// SShaderTransform
//
struct SShaderTransform
{
    CVector vecRot;
    CVector vecRotCenOffset;
    bool    bRotCenOffsetOriginIsScreen;

    CVector2D vecPersCenOffset;
    bool      bPersCenOffsetOriginIsScreen;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CRenderItem + derived class
//
// Render item is the core's version of something renderable
//
enum eRenderItemClassTypes
{
    CLASS_CRenderItem,
    CLASS_CDxFontItem,
    CLASS_CGuiFontItem,
    CLASS_CMaterialItem,
    CLASS_CEffectWrap,
    CLASS_CEffectParameters,
    CLASS_CEffectTemplate,
    CLASS_CShaderItem,
    CLASS_CShaderInstance,
    CLASS_CTextureItem,
    CLASS_CVectorGraphicItem,
    CLASS_CFileTextureItem,
    CLASS_CRenderTargetItem,
    CLASS_CScreenSourceItem,
    CLASS_CWebBrowserItem,
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CRenderItem
//
class CRenderItem
{
    DECLARE_BASE_CLASS(CRenderItem)
    CRenderItem() : ClassInit(this) {}
    virtual ~CRenderItem();
    virtual void PostConstruct(CRenderItemManager* pManager, bool bIncludeInMemoryStats = true);
    virtual void PreDestruct();
    virtual void Release();
    virtual void AddRef();
    virtual bool IsValid() = 0;
    virtual void OnLostDevice() = 0;
    virtual void OnResetDevice() = 0;
    int          GetVideoMemoryKBUsed() { return m_iMemoryKBUsed; }
    int          GetRevision() { return m_iRevision; }
    bool         GetIncludeInMemoryStats() { return m_bIncludeInMemoryStats; }

    CRenderItemManager* m_pManager;
    IDirect3DDevice9*   m_pDevice;
    int                 m_iRefCount;
    int                 m_iMemoryKBUsed;
    int                 m_iRevision;
    bool                m_bIncludeInMemoryStats;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CDxFontItem
//
class CDxFontItem : public CRenderItem
{
    DECLARE_CLASS(CDxFontItem, CRenderItem)
    CDxFontItem() : ClassInit(this) {}
    virtual void PostConstruct(CRenderItemManager* pManager, const SString& strFullFilePath, uint uiSize, bool bBold, DWORD ulQuality = DEFAULT_QUALITY);
    virtual void PreDestruct();
    virtual bool IsValid();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    void         CreateUnderlyingData(uint uiSize, bool bBold, DWORD ulQuality = DEFAULT_QUALITY);
    void         ReleaseUnderlyingData();

    SString    m_strFullFilePath;
    ID3DXFont* m_pFntNormal;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CGuiFontItem
//
class CGuiFontItem : public CRenderItem
{
    DECLARE_CLASS(CGuiFontItem, CRenderItem)
    CGuiFontItem() : ClassInit(this) {}
    virtual void PostConstruct(CRenderItemManager* pManager, const SString& strFullFilePath, const SString& strFontName, uint uiSize);
    virtual void PreDestruct();
    virtual bool IsValid();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    void         CreateUnderlyingData(const SString& strFullFilePath, const SString& strFontName, uint uiSize);
    void         ReleaseUnderlyingData();

    SString   m_strCEGUIFontName;
    CGUIFont* m_pFntCEGUI;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CEffectWrap - ID3DXEffect* wrapped with common handles
//
class CEffectWrap : public CRenderItem
{
    DECLARE_CLASS(CEffectWrap, CRenderItem)
    CEffectWrap() : ClassInit(this) {}
    virtual void PostConstruct(CRenderItemManager* pManager, CEffectTemplate* pEffectTemplate);
    virtual void PreDestruct();
    virtual bool IsValid();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    HRESULT      Begin(UINT* pPasses, DWORD Flags, bool bWorldRender = true);
    HRESULT      End();
    bool         ApplyCommonHandles();
    bool         ApplyMappedHandles();

    CEffectTemplate* m_pEffectTemplate;
    ID3DXEffect*     m_pD3DEffect;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CMaterialItem
//
class CMaterialItem : public CRenderItem
{
    DECLARE_CLASS(CMaterialItem, CRenderItem)
    CMaterialItem() : ClassInit(this), m_TextureAddress(TADDRESS_WRAP), m_uiBorderColor(0) {}

    uint            m_uiSizeX;
    uint            m_uiSizeY;
    ETextureAddress m_TextureAddress;
    uint            m_uiBorderColor;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CShaderItem
//
class CShaderItem : public CMaterialItem
{
    DECLARE_CLASS(CShaderItem, CMaterialItem)
    CShaderItem() : ClassInit(this) {}
    virtual void PostConstruct(CRenderItemManager* pManager, const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus,
                               float fPriority, float fMaxDistance, bool bLayered, bool bDebug, int iTypeMask, const EffectMacroList& macros);
    virtual void PreDestruct();
    virtual bool IsValid();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    void         CreateUnderlyingData(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus, bool bDebug,
                                      const EffectMacroList& macros);
    void         ReleaseUnderlyingData();
    virtual bool SetValue(const SString& strName, CTextureItem* pTextureItem);
    virtual bool SetValue(const SString& strName, bool bValue);
    virtual bool SetValue(const SString& strName, const float* pfValues, uint uiCount);
    void         MaybeRenewShaderInstance();
    void         RenewShaderInstance();
    virtual void SetTessellation(uint uiTessellationX, uint uiTessellationY);
    virtual void SetTransform(const SShaderTransform& transform);
    virtual bool GetUsesVertexShader();

    CEffectWrap* m_pEffectWrap;
    float        m_fPriority;
    uint         m_uiCreateTime;
    static uint  ms_uiCreateTimeCounter;
    float        m_fMaxDistanceSq;
    bool         m_bLayered;
    int          m_iTypeMask;

    // This is used as the current render material
    // If the shader wants to change a parameter, and the instance is refed by something else, then the shader must clone a new instance for itself
    CShaderInstance* m_pShaderInstance;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CShaderInstance - Used to store a snapshot of parameter values for a shader
//
class CShaderInstance : public CMaterialItem
{
    DECLARE_CLASS(CShaderInstance, CMaterialItem)
    CShaderInstance() : ClassInit(this) {}
    virtual void PostConstruct(CRenderItemManager* pManager, CShaderItem* pShaderItem);
    virtual void PreDestruct();
    virtual bool IsValid();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    void         CreateUnderlyingData(CShaderItem* pShaderItem);
    void         ReleaseUnderlyingData();

    void          SetTextureValue(D3DXHANDLE hHandle, CTextureItem* pTextureItem);
    void          SetBoolValue(D3DXHANDLE hHandle, bool pTextureItem);
    void          SetFloatsValue(D3DXHANDLE hHandle, const float* pfValues, uint uiCount);
    bool          CmpTextureValue(D3DXHANDLE hHandle, CTextureItem* pTextureItem);
    bool          CmpBoolValue(D3DXHANDLE hHandle, bool pTextureItem);
    bool          CmpFloatsValue(D3DXHANDLE hHandle, const float* pfValues, uint uiCount);
    SShaderValue* GetParam(D3DXHANDLE hHandle);
    void          ApplyShaderParameters();

    CEffectWrap*                       m_pEffectWrap;
    uint                               m_uiTessellationX;
    uint                               m_uiTessellationY;
    SShaderTransform                   m_Transform;
    bool                               m_bHasModifiedTransform;
    std::map<D3DXHANDLE, SShaderValue> m_currentSetValues;
    uint                               m_uiModifiedParametersRevision;
    std::vector<D3DXHANDLE>            m_requiredDefaultValuesList;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CTextureItem
//
class CTextureItem : public CMaterialItem
{
    DECLARE_CLASS(CTextureItem, CMaterialItem)
    CTextureItem() : ClassInit(this) {}

    IDirect3DBaseTexture9* m_pD3DTexture;
    uint                   m_uiSurfaceSizeX;
    uint                   m_uiSurfaceSizeY;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CFileTextureItem - Texture loaded from a file
//
class CFileTextureItem : public CTextureItem
{
    DECLARE_CLASS(CFileTextureItem, CTextureItem)
    CFileTextureItem() : ClassInit(this) {}
    virtual void PostConstruct(CRenderItemManager* pManager, const SString& strFilename, const CPixels* pPixels, bool bMipMaps = true, uint uiSizeX = RDEFAULT,
                               uint uiSizeY = RDEFAULT, ERenderFormat format = RFORMAT_UNKNOWN, ETextureAddress textureAddress = TADDRESS_WRAP,
                               ETextureType textureType = TTYPE_TEXTURE, uint uiVolumeDepth = 1);
    virtual void PreDestruct();
    virtual bool IsValid();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    void         CreateUnderlyingData(const SString& strFilename, bool bMipMaps, uint uiSizeX, uint uiSizeY, ERenderFormat format);
    void         CreateUnderlyingData(const CPixels* pPixels, bool bMipMaps, ERenderFormat format);
    void         CreateUnderlyingData(bool bMipMaps, uint uiSizeX, uint uiSizeY, ERenderFormat format, ETextureType textureType, uint uiVolumeDepth);
    void         ReleaseUnderlyingData();

    uint         m_uiVolumeDepth;
    ETextureType m_TextureType;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CVectorGraphicItem - CClientVectorGraphic to texture
//
class CVectorGraphicItem : public CTextureItem
{
    DECLARE_CLASS(CVectorGraphicItem, CTextureItem)
    CVectorGraphicItem() : ClassInit(this) {}
    virtual void PostConstruct(CRenderItemManager* pRenderItemManager, uint width, uint height);
    virtual void PreDestruct();
    virtual bool IsValid();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    void         CreateUnderlyingData();
    void         ReleaseUnderlyingData();
    void         UpdateTexture();
    virtual void Resize(const CVector2D& size);

    IDirect3DSurface9* m_pD3DRenderTargetSurface;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CRenderTargetItem - Render to texture
//
class CRenderTargetItem : public CTextureItem
{
    DECLARE_CLASS(CRenderTargetItem, CTextureItem)
    CRenderTargetItem() : ClassInit(this) {}
    virtual void PostConstruct(CRenderItemManager* pManager, uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel, bool bIncludeInMemoryStats);
    virtual void PreDestruct();
    virtual bool IsValid();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    void         CreateUnderlyingData();
    void         ReleaseUnderlyingData();
    bool         ReadPixels(CBuffer& outBuffer, SString& strOutError);

    bool               m_bWithAlphaChannel;
    IDirect3DSurface9* m_pD3DRenderTargetSurface;
    IDirect3DSurface9* m_pD3DZStencilSurface;
    IDirect3DSurface9* m_pD3DReadSurface;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CScreenSourceItem - Screen grab texture
//
class CScreenSourceItem : public CTextureItem
{
    DECLARE_CLASS(CScreenSourceItem, CTextureItem)
    CScreenSourceItem() : ClassInit(this) {}
    virtual void PostConstruct(CRenderItemManager* pRenderItemManager, uint uiSizeX, uint uiSizeY);
    virtual void PreDestruct();
    virtual bool IsValid();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    void         CreateUnderlyingData();
    void         ReleaseUnderlyingData();

    IDirect3DSurface9* m_pD3DRenderTargetSurface;
    uint               m_uiRevision;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CWebBrowserItem - webbrowser texture
//
class CWebBrowserItem : public CTextureItem
{
    DECLARE_CLASS(CWebBrowserItem, CTextureItem)
    CWebBrowserItem() : ClassInit(this) {}
    virtual void PostConstruct(CRenderItemManager* pRenderItemManager, uint uiSizeX, uint uiSizeY);
    virtual void PreDestruct();
    virtual bool IsValid();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    void         CreateUnderlyingData();
    void         ReleaseUnderlyingData();
    virtual void Resize(const CVector2D& size);

    IDirect3DSurface9* m_pD3DRenderTargetSurface;
};
