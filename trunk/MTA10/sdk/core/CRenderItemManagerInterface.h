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

struct ID3DXFont;
struct IDirect3DTexture9;
struct IDirect3DSurface9;
struct IDirect3DDevice9;
struct ID3DXEffect;
typedef LPCSTR D3DXHANDLE;
class CGUIFont;
class CRenderItem;
class CMaterialItem;
class CFontItem;
class CTextureItem;
class CShaderItem;
class CShaderInstance;
class CRenderTargetItem;
class CScreenSourceItem;
class CRenderItemManager;


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
    virtual                     ~CRenderItemManagerInterface    ( void ) {}

    // CRenderItemManagerInterface
    virtual CFontItem*          CreateFont                  ( const SString& strFullFilePath, const SString& strFontName, uint uiSize, bool bBold ) = 0;
    virtual CTextureItem*       CreateTexture               ( const SString& strFullFilePath ) = 0;
    virtual CShaderItem*        CreateShader                ( const SString& strFullFilePath, SString& strOutStatus ) = 0;
    virtual CRenderTargetItem*  CreateRenderTarget          ( uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel ) = 0;
    virtual CScreenSourceItem*  CreateScreenSource          ( uint uiSizeX, uint uiSizeY ) = 0;
    virtual void                ReleaseRenderItem           ( CRenderItem* pItem ) = 0;
    virtual bool                SetShaderValue              ( CShaderItem* pItem, const SString& strName, CTextureItem* pTextureItem ) = 0;
    virtual bool                SetShaderValue              ( CShaderItem* pItem, const SString& strName, bool bValue ) = 0;
    virtual bool                SetShaderValue              ( CShaderItem* pItem, const SString& strName, const float* pfValues, uint uiCount ) = 0;
    virtual bool                SetRenderTarget             ( CRenderTargetItem* pItem, bool bClear ) = 0;
    virtual bool                RestoreDefaultRenderTarget  ( void ) = 0;
    virtual void                UpdateBackBufferCopy        ( void ) = 0;
    virtual void                UpdateScreenSource          ( CScreenSourceItem* pScreenSourceItem ) = 0;
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
// CRenderItem + derived class
//
// Render item is the core's version of something renderable
//
enum eRenderItemClassTypes
{
    CLASS_CRenderItem,
    CLASS_CFontItem,
    CLASS_CMaterialItem,
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
    void            Release                 ( void );
    void            AddRef                  ( void );
    virtual bool    IsValid                 ( void ) = 0;
    virtual void    OnLostDevice            ( void ) = 0;
    virtual void    OnResetDevice           ( void ) = 0;

    CRenderItemManager* m_pManager;
    IDirect3DDevice9*   m_pDevice;
    int                 m_iRefCount;
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CFontItem
//
class CFontItem : public CRenderItem
{
    DECLARE_CLASS( CFontItem, CRenderItem )
                    CFontItem               ( void ) : ClassInit ( this ) {}
    virtual void    PostConstruct           ( CRenderItemManager* pManager, const SString& strFullFilePath, const SString& strFontName, uint uiSize, bool bBold );
    virtual void    PreDestruct             ( void );
    virtual bool    IsValid                 ( void );
    virtual void    OnLostDevice            ( void );
    virtual void    OnResetDevice           ( void );
    void            CreateUnderlyingData    ( const SString& strFontName, uint uiSize, bool bBold );
    void            ReleaseUnderlyingData   ( void );

    SString             m_strCEGUIFontName;
    CGUIFont*           m_pFntCEGUI;
    SString             m_strFullFilePath;
    ID3DXFont*          m_pFntNormal;
    ID3DXFont*          m_pFntBig;
};


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
    virtual void    PostConstruct           ( CRenderItemManager* pManager, const SString& strFilename, SString& strOutStatus );
    virtual void    PreDestruct             ( void );
    virtual bool    IsValid                 ( void );
    virtual void    OnLostDevice            ( void );
    virtual void    OnResetDevice           ( void );
    void            CreateUnderlyingData    ( const SString& strFilename, SString& strOutStatus );
    void            ReleaseUnderlyingData   ( void );
    bool            SetValue                ( const SString& strName, CTextureItem* pTextureItem );
    bool            SetValue                ( const SString& strName, bool bValue );
    bool            SetValue                ( const SString& strName, const float* pfValues, uint uiCount );
    void            MaybeRenewShaderInstance ( void );
    void            RenewShaderInstance     ( void );

    ID3DXEffect*        m_pD3DEffect;
    std::map < SString, D3DXHANDLE > m_texureHandleMap;
    std::map < SString, D3DXHANDLE > m_valueHandleMap;
    D3DXHANDLE          m_hWorld, m_hView, m_hProjection, m_hAll, m_hTime;
    D3DXHANDLE          m_hFirstTexture;

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

    ID3DXEffect*        m_pD3DEffect;
    D3DXHANDLE          m_hWorld, m_hView, m_hProjection, m_hAll, m_hTime;
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

    IDirect3DTexture9*  m_pD3DTexture;
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
    virtual void    PostConstruct           ( CRenderItemManager* pManager, const SString& strFilename );
    virtual void    PreDestruct             ( void );
    virtual bool    IsValid                 ( void );
    virtual void    OnLostDevice            ( void );
    virtual void    OnResetDevice           ( void );
    void            CreateUnderlyingData    ( const SString& strFilename );
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

    bool                m_bWithAlphaChannel;
    IDirect3DSurface9*  m_pD3DRenderTargetSurface;
    IDirect3DSurface9*  m_pD3DZStencilSurface;
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
