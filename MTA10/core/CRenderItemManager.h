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
    virtual CDxFontItem*        CreateDxFont                        ( const SString& strFullFilePath, uint uiSize, bool bBold );
    virtual CGuiFontItem*       CreateGuiFont                       ( const SString& strFullFilePath, const SString& strFontName, uint uiSize );
    virtual CTextureItem*       CreateTexture                       ( const SString& strFullFilePath );
    virtual CShaderItem*        CreateShader                        ( const SString& strFullFilePath, const SString& strRootPath, SString& strOutStatus, bool bDebug );
    virtual CRenderTargetItem*  CreateRenderTarget                  ( uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel );
    virtual CScreenSourceItem*  CreateScreenSource                  ( uint uiSizeX, uint uiSizeY );
    virtual void                ReleaseRenderItem                   ( CRenderItem* pItem );
    virtual bool                SetShaderValue                      ( CShaderItem* pItem, const SString& strName, CTextureItem* pTextureItem );
    virtual bool                SetShaderValue                      ( CShaderItem* pItem, const SString& strName, bool bValue );
    virtual bool                SetShaderValue                      ( CShaderItem* pItem, const SString& strName, const float* pfValues, uint uiCount );
    virtual bool                SetRenderTarget                     ( CRenderTargetItem* pItem, bool bClear );
    virtual bool                SaveDefaultRenderTarget             ( void );
    virtual bool                RestoreDefaultRenderTarget          ( void );
    virtual void                UpdateBackBufferCopy                ( void );
    virtual void                UpdateScreenSource                  ( CScreenSourceItem* pScreenSourceItem );
    virtual CShaderItem*        GetAppliedShaderForD3DData          ( CD3DDUMMY* pD3DData );
    virtual bool                ApplyShaderItemToWorldTexture       ( CShaderItem* pShaderItem, const SString& strTextureNameMatch );
    virtual bool                RemoveShaderItemFromWorldTexture    ( CShaderItem* pShaderItem, const SString& strTextureNameMatch );
    virtual void                GetVisibleTextureNames              ( std::vector < SString >& outNameList, const SString& strTextureNameMatch, ushort usModelID );

    // CRenderItemManager
    void                        NotifyContructRenderItem            ( CRenderItem* pItem );
    void                        NotifyDestructRenderItem            ( CRenderItem* pItem );
    void                        OnDeviceCreate                      ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY );
    void                        OnLostDevice                        ( void );
    void                        OnResetDevice                       ( void );
    void                        UpdateBackBufferCopySize            ( void );
    void                        ChangeRenderTarget                  ( uint uiSizeX, uint uiSizeY, IDirect3DSurface9* pD3DRenderTarget, IDirect3DSurface9* pD3DZStencilSurface );
    void                        RemoveShaderItemFromWatchLists      ( CShaderItem* pShaderItem );
    void                        WatchCallback                       ( CSHADERDUMMY* pShaderItem, CD3DDUMMY* pD3DDataNew, CD3DDUMMY* pD3DDataOld );
    static void                 StaticWatchCallback                 ( CSHADERDUMMY* pShaderItem, CD3DDUMMY* pD3DDataNew, CD3DDUMMY* pD3DDataOld );

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

    // Shaders applied to world textures
    std::map < CD3DDUMMY*, CSHADERDUMMY* >      m_D3DDataShaderMap;
    class CRenderWare*                          m_pRenderWare;
};
