/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CRenderItemManager.h
 *  PURPOSE:
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
    CRenderItemManager();
    ~CRenderItemManager();

    // CRenderItemManagerInterface
    virtual void          DoPulse();
    virtual CDxFontItem*  CreateDxFont(const SString& strFullFilePath, uint uiSize, bool bBold, DWORD ulQuality = DEFAULT_QUALITY);
    virtual CGuiFontItem* CreateGuiFont(const SString& strFullFilePath, const SString& strFontName, uint uiSize);
    virtual CTextureItem* CreateTexture(const SString& strFullFilePath, const CPixels* pPixels, bool bMipMaps = true, uint uiSizeX = RDEFAULT,
                                        uint uiSizeY = RDEFAULT, ERenderFormat format = RFORMAT_UNKNOWN, ETextureAddress textureAddress = TADDRESS_WRAP,
                                        ETextureType textureType = TTYPE_TEXTURE, uint uiVolumeDepth = 1);
    virtual CShaderItem*  CreateShader(const SString& strFile, const SString& strRootPath, bool bIsRawData, SString& strOutStatus, float fPriority, float fMaxDistance,
                                       bool bLayered, bool bDebug, int iTypeMask);
    virtual CRenderTargetItem* CreateRenderTarget(uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel, bool bForce = false);
    virtual CScreenSourceItem* CreateScreenSource(uint uiSizeX, uint uiSizeY);
    virtual CWebBrowserItem*   CreateWebBrowser(uint uiSizeX, uint uiSizeY);
    virtual bool               SetRenderTarget(CRenderTargetItem* pItem, bool bClear);
    virtual void               EnableSetRenderTargetOldVer(bool bEnable);
    virtual bool               IsSetRenderTargetEnabledOldVer();
    virtual bool               RestoreDefaultRenderTarget();
    virtual void               UpdateBackBufferCopy();
    virtual void               UpdateScreenSource(CScreenSourceItem* pScreenSourceItem, bool bResampleNow);
    virtual SShaderItemLayers* GetAppliedShaderForD3DData(CD3DDUMMY* pD3DData);
    virtual bool               ApplyShaderItemToWorldTexture(CShaderItem* pShaderItem, const SString& strTextureNameMatch, CClientEntityBase* pClientEntity,
                                                             bool bAppendLayers);
    virtual bool               RemoveShaderItemFromWorldTexture(CShaderItem* pShaderItem, const SString& strTextureNameMatch, CClientEntityBase* pClientEntity);
    virtual void               RemoveClientEntityRefs(CClientEntityBase* pClientEntity);
    virtual void               GetVisibleTextureNames(std::vector<SString>& outNameList, const SString& strTextureNameMatch, ushort usModelID);
    virtual eDxTestMode        GetTestMode() { return m_TestMode; }
    virtual void               SetTestMode(eDxTestMode testMode);
    virtual void               GetDxStatus(SDxStatus& outStatus);
    virtual CEffectCloner*     GetEffectCloner() { return m_pEffectCloner; }
    virtual void               PreDrawWorld();
    virtual void               SetDepthBufferFormat(ERenderFormat depthBufferFormat) { m_depthBufferFormat = depthBufferFormat; }
    virtual ERenderFormat      GetDepthBufferFormat() { return m_depthBufferFormat; }
    virtual void               SaveReadableDepthBuffer();
    virtual void               FlushNonAARenderTarget();
    virtual void HandleStretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect,
                                   int Filter);

    // CRenderItemManager
    void NotifyContructRenderItem(CRenderItem* pItem);
    void NotifyDestructRenderItem(CRenderItem* pItem);
    void OnDeviceCreate(IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY);
    void OnLostDevice();
    void OnResetDevice();
    void UpdateBackBufferCopySize();
    bool SaveDefaultRenderTarget();
    bool IsUsingDefaultRenderTarget();
    void ChangeRenderTarget(uint uiSizeX, uint uiSizeY, IDirect3DSurface9* pD3DRenderTarget, IDirect3DSurface9* pD3DZStencilSurface);
    void RemoveShaderItemFromWatchLists(CShaderItem* pShaderItem);
    void UpdateMemoryUsage();
    bool CanCreateRenderItem(ClassId classId);
    void NotifyShaderItemUsesDepthBuffer(CShaderItem* pShaderItem, bool bUsesDepthBuffer);
    void NotifyShaderItemUsesMultipleRenderTargets(CShaderItem* pShaderItem, bool bUsesMultipleRenderTargets);

    static int GetBitsPerPixel(D3DFORMAT Format);
    static int GetPitchDivisor(D3DFORMAT Format);
    static int CalcD3DResourceMemoryKBUsage(IDirect3DResource9* pD3DResource);
    static int CalcD3DSurfaceMemoryKBUsage(IDirect3DSurface9* pD3DSurface);
    static int CalcD3DTextureMemoryKBUsage(IDirect3DTexture9* pD3DTexture);
    static int CalcD3DVolumeTextureMemoryKBUsage(IDirect3DVolumeTexture9* pD3DVolumeTexture);
    static int CalcD3DCubeTextureMemoryKBUsage(IDirect3DCubeTexture9* pD3DCubeTexture);

    IDirect3DDevice9* m_pDevice;

protected:
    std::set<CRenderItem*>   m_CreatedItemList;
    IDirect3DSurface9*       m_pDefaultD3DRenderTarget = nullptr;
    IDirect3DSurface9*       m_pDefaultD3DZStencilSurface = nullptr;
    uint                     m_uiDefaultViewportSizeX = 0;
    uint                     m_uiDefaultViewportSizeY = 0;
    CRenderTargetItem*       m_pBackBufferCopy = nullptr;
    bool                     m_bBackBufferCopyMaybeNeedsResize = false;
    uint                     m_uiBackBufferCopyRevision = 0;
    CFastHashSet<CD3DDUMMY*> m_FrameTextureUsage;
    CFastHashSet<CD3DDUMMY*> m_PrevFrameTextureUsage;
    class CRenderWare*       m_pRenderWare = nullptr;
    CEffectCloner*           m_pEffectCloner = nullptr;
    eDxTestMode              m_TestMode = eDxTestMode::DX_TEST_MODE_NONE;
    SString                  m_strVideoCardName;
    int                      m_iVideoCardMemoryKBTotal = 0;
    int                      m_iVideoCardMemoryKBForMTATotal = 0;
    SString                  m_strVideoCardPSVersion;
    int                      m_iTextureMemoryKBUsed = 0;
    int                      m_iRenderTargetMemoryKBUsed = 0;
    int                      m_iFontMemoryKBUsed = 0;
    int                      m_iMemoryKBFreeForMTA = 0;
    bool                     m_bSetRenderTargetEnabledOldVer = false;
    bool                     m_bUsingReadableDepthBuffer = false;
    ERenderFormat            m_depthBufferFormat = ERenderFormat::RFORMAT_UNKNOWN;
    std::set<CShaderItem*>   m_ShadersUsingDepthBuffer;
    std::set<CShaderItem*>   m_ShadersUsingMultipleRenderTargets;
    IDirect3DSurface9*       m_pSavedSceneDepthSurface = nullptr;
    IDirect3DSurface9*       m_pSavedSceneRenderTargetAA = nullptr;
    IDirect3DSurface9*       m_pNonAADepthSurface2 = nullptr;
    IDirect3DSurface9*       m_pNonAARenderTarget = nullptr;
    IDirect3DTexture9*       m_pNonAARenderTargetTexture = nullptr;
    bool                     m_bIsSwiftShader = false;
};
