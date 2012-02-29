/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CRenderItemManagerInterface.h
*  PURPOSE:
*  DEVELOPERS:  idiot
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <CVector.h>

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
class CRenderItemManager;
class CD3DDUMMY;
class CSHADERDUMMY;
class CEffectCloner;
class CPixels;

#define RDEFAULT            ((uint) -1)

enum ERenderFormat
{
    RFORMAT_UNKNOWN,
    RFORMAT_ARGB                 = 21,          // D3DFMT_A8R8G8B8
    RFORMAT_DXT1                 = '1TXD',
    RFORMAT_DXT2                 = '2TXD',
    RFORMAT_DXT3                 = '3TXD',
    RFORMAT_DXT4                 = '4TXD',
    RFORMAT_DXT5                 = '5TXD',
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
    eDxTestMode     testMode;

    struct
    {
        SString         strName;
        int             iInstalledMemoryKB;
        SString         strPSVersion;
        uint            uiAvailableTextureMem;
    } videoCard;

    struct
    {
        int             iFreeForMTA;
        int             iUsedByFonts;
        int             iUsedByTextures;
        int             iUsedByRenderTargets;
    } videoMemoryKB;

    struct
    {
        bool            bWindowed;
        int             iFXQuality;
        int             iDrawDistance;
        bool            bVolumetricShadows;
        bool            bAllowScreenUpload;
        int             iStreamingMemory;
    } settings;
};


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
    virtual                     ~CRenderItemManagerInterface        ( void ) {}

    // CRenderItemManagerInterface
    virtual void                DoPulse                             ( void ) = 0;
    virtual CDxFontItem*        CreateDxFont                        ( const SString& strFullFilePath, uint uiSize, bool bBold ) = 0;
    virtual CGuiFontItem*       CreateGuiFont                       ( const SString& strFullFilePath, const SString& strFontName, uint uiSize ) = 0;
    virtual CTextureItem*       CreateTexture                       ( const SString& strFullFilePath, const CPixels* pPixels = NULL, bool bMipMaps = true, uint uiSizeX = RDEFAULT, uint uiSizeY = RDEFAULT, ERenderFormat format = RFORMAT_UNKNOWN ) = 0;
    virtual CShaderItem*        CreateShader                        ( const SString& strFullFilePath, const SString& strRootPath, SString& strOutStatus, float fPriority, float fMaxDistance, bool bDebug ) = 0;
    virtual CRenderTargetItem*  CreateRenderTarget                  ( uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel, bool bForce = false ) = 0;
    virtual CScreenSourceItem*  CreateScreenSource                  ( uint uiSizeX, uint uiSizeY ) = 0;
    virtual bool                SetRenderTarget                     ( CRenderTargetItem* pItem, bool bClear ) = 0;
    virtual bool                SaveDefaultRenderTarget             ( void ) = 0;
    virtual bool                RestoreDefaultRenderTarget          ( void ) = 0;
    virtual void                UpdateBackBufferCopy                ( void ) = 0;
    virtual void                UpdateScreenSource                  ( CScreenSourceItem* pScreenSourceItem, bool bResampleNow ) = 0;
    virtual CShaderItem*        GetAppliedShaderForD3DData          ( CD3DDUMMY* pD3DData ) = 0;
    virtual bool                ApplyShaderItemToWorldTexture       ( CShaderItem* pShaderItem, const SString& strTextureNameMatch ) = 0;
    virtual bool                RemoveShaderItemFromWorldTexture    ( CShaderItem* pShaderItem, const SString& strTextureNameMatch ) = 0;
    virtual void                GetVisibleTextureNames              ( std::vector < SString >& outNameList, const SString& strTextureNameMatch, ushort usModelID ) = 0;
    virtual eDxTestMode         GetTestMode                         ( void ) = 0;
    virtual void                SetTestMode                         ( eDxTestMode testMode ) = 0;
    virtual void                GetDxStatus                         ( SDxStatus& outStatus ) = 0;
    virtual CEffectCloner*      GetEffectCloner                     ( void ) = 0;
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
        bool bValue;
        float floatList[16];
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
    bool bRotCenOffsetOriginIsScreen;

    CVector vecPersCenOffset;
    bool bPersCenOffsetOriginIsScreen;
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
    CLASS_CEffectTemplate,
    CLASS_CShaderItem,
    CLASS_CShaderInstance,
    CLASS_CTextureItem,
    CLASS_CFileTextureItem,
    CLASS_CRenderTargetItem,
    CLASS_CScreenSourceItem,
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CRenderItem
//
class CRenderItem
{
    DECLARE_BASE_CLASS( CRenderItem )
                    CRenderItem             ( void ) : ClassInit ( this ) {}
    virtual         ~CRenderItem            ( void );
    virtual void    PostConstruct           ( CRenderItemManager* pManager );
    virtual void    PreDestruct             ( void );
    virtual void    Release                 ( void );
    virtual void    AddRef                  ( void );
    virtual bool    IsValid                 ( void ) = 0;
    virtual void    OnLostDevice            ( void ) = 0;
    virtual void    OnResetDevice           ( void ) = 0;
    int             GetVideoMemoryKBUsed    ( void ) { return m_iMemoryKBUsed; }
    int             GetRevision             ( void ) { return m_iRevision; }

    CRenderItemManager* m_pManager;
    IDirect3DDevice9*   m_pDevice;
    int                 m_iRefCount;
    int                 m_iMemoryKBUsed;
    int                 m_iRevision;
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CDxFontItem
//
class CDxFontItem : public CRenderItem
{
    DECLARE_CLASS( CDxFontItem, CRenderItem )
                    CDxFontItem             ( void ) : ClassInit ( this ) {}
    virtual void    PostConstruct           ( CRenderItemManager* pManager, const SString& strFullFilePath, uint uiSize, bool bBold );
    virtual void    PreDestruct             ( void );
    virtual bool    IsValid                 ( void );
    virtual void    OnLostDevice            ( void );
    virtual void    OnResetDevice           ( void );
    void            CreateUnderlyingData    ( uint uiSize, bool bBold );
    void            ReleaseUnderlyingData   ( void );

    SString             m_strFullFilePath;
    ID3DXFont*          m_pFntNormal;
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CGuiFontItem
//
class CGuiFontItem : public CRenderItem
{
    DECLARE_CLASS( CGuiFontItem, CRenderItem )
                    CGuiFontItem            ( void ) : ClassInit ( this ) {}
    virtual void    PostConstruct           ( CRenderItemManager* pManager, const SString& strFullFilePath, const SString& strFontName, uint uiSize );
    virtual void    PreDestruct             ( void );
    virtual bool    IsValid                 ( void );
    virtual void    OnLostDevice            ( void );
    virtual void    OnResetDevice           ( void );
    void            CreateUnderlyingData    ( const SString& strFullFilePath, const SString& strFontName, uint uiSize );
    void            ReleaseUnderlyingData   ( void );

    SString             m_strCEGUIFontName;
    CGUIFont*           m_pFntCEGUI;
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CEffectWrap - ID3DXEffect* wrapped with common handles
//
class CEffectWrap : public CRenderItem
{
    DECLARE_CLASS( CEffectWrap, CRenderItem )
                    CEffectWrap             ( void ) : ClassInit ( this ) {}
    virtual void    ApplyCommonHandles      ( void ) = 0;
    virtual void    ApplyMappedHandles      ( void ) = 0;
    virtual void    ReadParameterHandles    ( void ) = 0;

    ID3DXEffect*    m_pD3DEffect;
    D3DXHANDLE      hWorld, hView, hProjection, hWorldView, hWorldViewProj;
    D3DXHANDLE      hViewProj, hViewInv, hWorldInvTr, hViewInvTr;
    D3DXHANDLE      hCamPos, hCamDir;
    D3DXHANDLE      hTime;
    D3DXHANDLE      hLightAmbient, hLightDiffuse, hLightSpecular, hLightDirection;

    std::map < SString, D3DXHANDLE > m_texureHandleMap;
    std::map < SString, D3DXHANDLE > m_valueHandleMap;
    D3DXHANDLE      m_hFirstTexture;
    bool            m_bRequiresNormals;
    uint            m_uiSaveStateFlags;
    SString         m_strName;
};

CEffectWrap* NewEffectWrap ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug );

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CMaterialItem
//
class CMaterialItem : public CRenderItem
{
    DECLARE_CLASS( CMaterialItem, CRenderItem )
                    CMaterialItem           ( void ) : ClassInit ( this ) {}

    uint                m_uiSizeX;
    uint                m_uiSizeY;
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CShaderItem
//
class CShaderItem : public CMaterialItem
{
    DECLARE_CLASS( CShaderItem, CMaterialItem )
                    CShaderItem             ( void ) : ClassInit ( this ) {}
    virtual void    PostConstruct           ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, float fPriority, float fMaxDistance, bool bDebug );
    virtual void    PreDestruct             ( void );
    virtual bool    IsValid                 ( void );
    virtual void    OnLostDevice            ( void );
    virtual void    OnResetDevice           ( void );
    void            CreateUnderlyingData    ( const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug );
    void            ReleaseUnderlyingData   ( void );
    virtual bool    SetValue                ( const SString& strName, CTextureItem* pTextureItem );
    virtual bool    SetValue                ( const SString& strName, bool bValue );
    virtual bool    SetValue                ( const SString& strName, const float* pfValues, uint uiCount );
    void            MaybeRenewShaderInstance ( void );
    void            RenewShaderInstance     ( void );
    virtual void    SetTessellation         ( uint uiTessellationX, uint uiTessellationY );
    virtual void    SetTransform            ( const SShaderTransform& transform );

    CEffectWrap*        m_pEffectWrap;
    float               m_fPriority;
    float               m_fMaxDistanceSq;

    // This is used as the current render material
    // If the shader wants to change a parameter, and the instance is refed by something else, then the shader must clone a new instance for itself
    CShaderInstance*    m_pShaderInstance;
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CShaderInstance - Used to store a snapshot of parameter values for a shader
//
class CShaderInstance : public CMaterialItem
{
    DECLARE_CLASS( CShaderInstance, CMaterialItem )
                    CShaderInstance         ( void ) : ClassInit ( this ) {}
    virtual void    PostConstruct           ( CRenderItemManager* pManager, CShaderItem* pShaderItem );
    virtual void    PreDestruct             ( void );
    virtual bool    IsValid                 ( void );
    virtual void    OnLostDevice            ( void );
    virtual void    OnResetDevice           ( void );
    void            CreateUnderlyingData    ( CShaderItem* pShaderItem );
    void            ReleaseUnderlyingData   ( void );

    void            SetTextureValue         ( D3DXHANDLE hHandle, CTextureItem* pTextureItem );
    void            SetBoolValue            ( D3DXHANDLE hHandle, bool pTextureItem );
    void            SetFloatsValue          ( D3DXHANDLE hHandle, const float* pfValues, uint uiCount );
    bool            CmpTextureValue         ( D3DXHANDLE hHandle, CTextureItem* pTextureItem );
    bool            CmpBoolValue            ( D3DXHANDLE hHandle, bool pTextureItem );
    bool            CmpFloatsValue          ( D3DXHANDLE hHandle, const float* pfValues, uint uiCount );
    SShaderValue*   GetParam                ( D3DXHANDLE hHandle );
    void            ApplyShaderParameters   ( void );

    CEffectWrap*        m_pEffectWrap;
    uint                m_uiTessellationX;
    uint                m_uiTessellationY;
    SShaderTransform    m_Transform;
    bool                m_bHasModifiedTransform;
    std::map < D3DXHANDLE, SShaderValue > m_currentSetValues;
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CTextureItem
//
class CTextureItem : public CMaterialItem
{
    DECLARE_CLASS( CTextureItem, CMaterialItem )
                    CTextureItem            ( void ) : ClassInit ( this ) {}

    IDirect3DBaseTexture9*  m_pD3DTexture;
    uint                    m_uiSurfaceSizeX;
    uint                    m_uiSurfaceSizeY;
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CFileTextureItem - Texture loaded from a file
//
class CFileTextureItem : public CTextureItem
{
    DECLARE_CLASS( CFileTextureItem, CTextureItem )
                    CFileTextureItem        ( void ) : ClassInit ( this ) {}
    virtual void    PostConstruct           ( CRenderItemManager* pManager, const SString& strFilename, const CPixels* pPixels, bool bMipMaps = true, uint uiSizeX = RDEFAULT, uint uiSizeY = RDEFAULT, ERenderFormat format = RFORMAT_UNKNOWN );
    virtual void    PreDestruct             ( void );
    virtual bool    IsValid                 ( void );
    virtual void    OnLostDevice            ( void );
    virtual void    OnResetDevice           ( void );
    void            CreateUnderlyingData    ( const SString& strFilename, bool bMipMaps, uint uiSizeX, uint uiSizeY, ERenderFormat format );
    void            CreateUnderlyingData    ( const CPixels* pPixels, bool bMipMaps, ERenderFormat format );
    void            CreateUnderlyingData    ( bool bMipMaps, uint uiSizeX, uint uiSizeY, ERenderFormat format );
    void            ReleaseUnderlyingData   ( void );
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CRenderTargetItem - Render to texture
//
class CRenderTargetItem : public CTextureItem
{
    DECLARE_CLASS( CRenderTargetItem, CTextureItem )
                    CRenderTargetItem       ( void ) : ClassInit ( this ) {}
    virtual void    PostConstruct           ( CRenderItemManager* pManager, uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel );
    virtual void    PreDestruct             ( void );
    virtual bool    IsValid                 ( void );
    virtual void    OnLostDevice            ( void );
    virtual void    OnResetDevice           ( void );
    void            CreateUnderlyingData    ( void );
    void            ReleaseUnderlyingData   ( void );
    void            ReadPixels              ( CBuffer& outBuffer );

    bool                m_bWithAlphaChannel;
    IDirect3DSurface9*  m_pD3DRenderTargetSurface;
    IDirect3DSurface9*  m_pD3DZStencilSurface;
    IDirect3DSurface9*  m_pD3DReadSurface;
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CScreenSourceItem - Screen grab texture
//
class CScreenSourceItem : public CTextureItem
{
    DECLARE_CLASS( CScreenSourceItem, CTextureItem )
                    CScreenSourceItem       ( void ) : ClassInit ( this ) {}
    virtual void    PostConstruct           ( CRenderItemManager* pRenderItemManager, uint uiSizeX, uint uiSizeY );
    virtual void    PreDestruct             ( void );
    virtual bool    IsValid                 ( void );
    virtual void    OnLostDevice            ( void );
    virtual void    OnResetDevice           ( void );
    void            CreateUnderlyingData    ( void );
    void            ReleaseUnderlyingData   ( void );

    IDirect3DSurface9*  m_pD3DRenderTargetSurface;
    uint                m_uiRevision;
};
