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
                                CRenderItemManager          ( void );
                                ~CRenderItemManager         ( void );

    // CRenderItemManagerInterface
    virtual CFontItem*          CreateFont                  ( const SString& strFullFilePath, const SString& strFontName, uint uiSize, bool bBold );
    virtual CTextureItem*       CreateTexture               ( const SString& strFullFilePath );
    virtual CShaderItem*        CreateShader                ( const SString& strFullFilePath, SString& strOutStatus );
    virtual CRenderTargetItem*  CreateRenderTarget          ( uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel );
    virtual CScreenSourceItem*  CreateScreenSource          ( uint uiSizeX, uint uiSizeY );
    virtual void                ReleaseRenderItem           ( CRenderItem* pItem );
    virtual bool                SetShaderValue              ( CShaderItem* pItem, const SString& strName, CTextureItem* pTextureItem );
    virtual bool                SetShaderValue              ( CShaderItem* pItem, const SString& strName, bool bValue );
    virtual bool                SetShaderValue              ( CShaderItem* pItem, const SString& strName, const float* pfValues, uint uiCount );
    virtual bool                SetRenderTarget             ( CRenderTargetItem* pItem, bool bClear );
    virtual bool                RestoreDefaultRenderTarget  ( void );
    virtual void                UpdateBackBufferCopy        ( void );
    virtual void                UpdateScreenSource          ( CScreenSourceItem* pScreenSourceItem );

    // CRenderItemManager
    void                        NotifyContructRenderItem    ( CRenderItem* pItem );
    void                        NotifyDestructRenderItem    ( CRenderItem* pItem );
    void                        OnDeviceCreate              ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY );
    void                        OnLostDevice                ( void );
    void                        OnResetDevice               ( void );
    void                        UpdateBackBufferCopySize    ( void );
    void                        ChangeRenderTarget          ( uint uiSizeX, uint uiSizeY, IDirect3DSurface9* pD3DRenderTarget, IDirect3DSurface9* pD3DZStencilSurface );

    IDirect3DDevice9*           m_pDevice;
protected:
    std::set < CRenderItem* >   m_CreatedItemList;
    IDirect3DSurface9*          m_pDefaultD3DRenderTarget;
    IDirect3DSurface9*          m_pDefaultD3DZStencilSurface;
    uint                        m_uiDefaultViewportSizeX;
    uint                        m_uiDefaultViewportSizeY;
    CRenderTargetItem*          m_pBackBufferCopy;
    bool                        m_bBackBufferCopyMaybeNeedsResize;
    uint                        m_uiBackBufferCopyRevision;
};
