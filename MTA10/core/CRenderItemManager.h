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
                                CRenderItemManager          ( void );
                                ~CRenderItemManager         ( void );

    // CRenderItemManagerInterface
    virtual SFontItem*          CreateFont                  ( const SString& strFullFilePath, const SString& strFontName, uint uiSize, bool bBold );
    virtual STextureItem*       CreateTexture               ( const SString& strFullFilePath );
    virtual SShaderItem*        CreateShader                ( const SString& strFullFilePath, SString& strOutStatus );
    virtual SRenderTargetItem*  CreateRenderTarget          ( uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel );
    virtual SScreenSourceItem*  CreateScreenSource          ( uint uiSizeX, uint uiSizeY );
    virtual void                ReleaseRenderItem           ( SRenderItem* pItem );
    virtual bool                SetShaderValue              ( SShaderItem* pItem, const SString& strName, STextureItem* pTextureItem );
    virtual bool                SetShaderValue              ( SShaderItem* pItem, const SString& strName, bool bValue );
    virtual bool                SetShaderValue              ( SShaderItem* pItem, const SString& strName, const float* pfValues, uint uiCount );
    virtual bool                SetRenderTarget             ( SRenderTargetItem* pItem, bool bClear );
    virtual bool                RestoreDefaultRenderTarget  ( void );
    virtual void                UpdateBackBufferCopy        ( void );
    virtual void                UpdateScreenSource          ( SScreenSourceItem* pScreenSourceItem );

    // CRenderItemManager
    void                        OnDeviceCreate              ( IDirect3DDevice9* pDevice, float fViewportSizeX, float fViewportSizeY );
    void                        UpdateBackBufferCopySize    ( void );
    void                        ReleaseFontData             ( SFontItem* pFontItem );
    void                        ReleaseTextureData          ( STextureItem* pTextureItem );
    void                        ReleaseShaderData           ( SShaderItem* pTextureItem );
    void                        ReleaseRenderTargetData     ( SRenderTargetItem* pRenderTargetItem );
    void                        ReleaseScreenSourceData     ( SScreenSourceItem* pScreenSourceItem );
    void                        ChangeRenderTarget          ( uint uiSizeX, uint uiSizeY, IDirect3DSurface9* pD3DRenderTarget, IDirect3DSurface9* pD3DZStencilSurface );

protected:
    IDirect3DDevice9*           m_pDevice;
    std::set < SRenderItem* >   m_CreatedItemList;
    IDirect3DSurface9*          m_pDefaultD3DRenderTarget;
    IDirect3DSurface9*          m_pDefaultD3DZStencilSurface;
    uint                        m_uiDefaultViewportSizeX;
    uint                        m_uiDefaultViewportSizeY;
    SRenderTargetItem*          m_pBackBufferCopy;
    bool                        m_bBackBufferCopyMaybeNeedsResize;
    uint                        m_uiBackBufferCopyRevision;
};
