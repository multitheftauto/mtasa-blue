/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CRenderItemManager.h
*  PURPOSE:
*  DEVELOPERS:  idiot
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


//
// CRenderItemManager
//
// Make/delete/fiddle with render items
//
class CRenderItemManager : public CRenderItemManagerInterface
{
public:
    ZERO_ON_NEW
                                CRenderItemManager                  ( void );
                                ~CRenderItemManager                 ( void );

    // CRenderItemManagerInterface
    virtual void                DoPulse                             ( void );
    virtual CDxFontItem*        CreateDxFont                        ( const SString& strFullFilePath, uint uiSize, bool bBold );
    virtual CGuiFontItem*       CreateGuiFont                       ( const SString& strFullFilePath, const SString& strFontName, uint uiSize );
    virtual CTextureItem*       CreateTexture                       ( const SString& strFullFilePath, const CPixels* pPixels, bool bMipMaps = true, uint uiSizeX = RDEFAULT, uint uiSizeY = RDEFAULT, ERenderFormat format = RFORMAT_UNKNOWN, ETextureAddress textureAddress = TADDRESS_WRAP, ETextureType textureType = TTYPE_TEXTURE, uint uiVolumeDepth = 1 );
    virtual CShaderItem*        CreateShader                        ( const SString& strFullFilePath, const SString& strRootPath, SString& strOutStatus, float fPriority, float fMaxDistance, bool bDebug );
    virtual CRenderTargetItem*  CreateRenderTarget                  ( uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel, bool bForce = false );
    virtual CScreenSourceItem*  CreateScreenSource                  ( uint uiSizeX, uint uiSizeY );
    virtual bool                SetRenderTarget                     ( CRenderTargetItem* pItem, bool bClear );
    virtual bool                SaveDefaultRenderTarget             ( void );
    virtual bool                RestoreDefaultRenderTarget          ( void );
    virtual void                UpdateBackBufferCopy                ( void );
    virtual void                UpdateScreenSource                  ( CScreenSourceItem* pScreenSourceItem, bool bResampleNow );
    virtual CShaderItem*        GetAppliedShaderForD3DData          ( CD3DDUMMY* pD3DData );
    virtual bool                ApplyShaderItemToWorldTexture       ( CShaderItem* pShaderItem, const SString& strTextureNameMatch, CClientEntityBase* pClientEntity );
    virtual bool                RemoveShaderItemFromWorldTexture    ( CShaderItem* pShaderItem, const SString& strTextureNameMatch, CClientEntityBase* pClientEntity );
    virtual void                RemoveClientEntityRefs              ( CClientEntityBase* pClientEntity );
    virtual void                GetVisibleTextureNames              ( std::vector < SString >& outNameList, const SString& strTextureNameMatch, ushort usModelID );
    virtual eDxTestMode         GetTestMode                         ( void )                    { return m_TestMode; }
    virtual void                SetTestMode                         ( eDxTestMode testMode );
    virtual void                GetDxStatus                         ( SDxStatus& outStatus );
    virtual CEffectCloner*      GetEffectCloner                     ( void )                    { return m_pEffectCloner; }

    // CRenderItemManager
    void                        NotifyContructRenderItem            ( CRenderItem* pItem );
    void                        NotifyDestructRenderItem            ( CRenderItem* pItem );
    void                        OnDeviceCreate                      ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY );
    void                        OnLostDevice                        ( void );
    void                        OnResetDevice                       ( void );
    void                        UpdateBackBufferCopySize            ( void );
    void                        ChangeRenderTarget                  ( uint uiSizeX, uint uiSizeY, IDirect3DSurface9* pD3DRenderTarget, IDirect3DSurface9* pD3DZStencilSurface );
    void                        RemoveShaderItemFromWatchLists      ( CShaderItem* pShaderItem );
    void                        UpdateMemoryUsage                   ( void );
    bool                        CanCreateRenderItem                 ( ClassId classId );

    static int                  GetBitsPerPixel                     ( D3DFORMAT Format );
    static int                  CalcD3DResourceMemoryKBUsage        ( IDirect3DResource9* pD3DResource );
    static int                  CalcD3DSurfaceMemoryKBUsage         ( IDirect3DSurface9* pD3DSurface );
    static int                  CalcD3DTextureMemoryKBUsage         ( IDirect3DTexture9* pD3DTexture );
    static int                  CalcD3DVolumeTextureMemoryKBUsage   ( IDirect3DVolumeTexture9* pD3DVolumeTexture );
    static int                  CalcD3DCubeTextureMemoryKBUsage     ( IDirect3DCubeTexture9* pD3DCubeTexture );

    IDirect3DDevice9*                           m_pDevice;
protected:
    std::set < CRenderItem* >                   m_CreatedItemList;
    IDirect3DSurface9*                          m_pDefaultD3DRenderTarget;
    IDirect3DSurface9*                          m_pDefaultD3DZStencilSurface;
    uint                                        m_uiDefaultViewportSizeX;
    uint                                        m_uiDefaultViewportSizeY;
    CRenderTargetItem*                          m_pBackBufferCopy;
    bool                                        m_bBackBufferCopyMaybeNeedsResize;
    uint                                        m_uiBackBufferCopyRevision;
    std::set < CD3DDUMMY* >                     m_FrameTextureUsage;
    std::set < CD3DDUMMY* >                     m_PrevFrameTextureUsage;
    class CRenderWare*                          m_pRenderWare;
    CEffectCloner*                              m_pEffectCloner;
    eDxTestMode                                 m_TestMode;
    SString                                     m_strVideoCardName;
    int                                         m_iVideoCardMemoryKBTotal;
    int                                         m_iVideoCardMemoryKBForMTATotal;
    SString                                     m_strVideoCardPSVersion;
    int                                         m_iTextureMemoryKBUsed;
    int                                         m_iRenderTargetMemoryKBUsed;
    int                                         m_iFontMemoryKBUsed;
    int                                         m_iMemoryKBFreeForMTA;
};
