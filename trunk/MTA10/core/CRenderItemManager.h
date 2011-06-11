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
    virtual void                ReleaseRenderItem           ( SRenderItem* pItem );
    virtual bool                SetShaderValue              ( SShaderItem* pItem, const SString& strName, STextureItem* pTextureItem );
    virtual bool                SetShaderValue              ( SShaderItem* pItem, const SString& strName, bool bValue );
    virtual bool                SetShaderValue              ( SShaderItem* pItem, const SString& strName, const float* pfValues, uint uiCount );

    // CRenderItemManager
    void                        OnDeviceCreate              ( IDirect3DDevice9* pDevice );
    void                        DestroyFont                 ( SFontItem* pFontItem );
    void                        DestroyTexture              ( STextureItem* pTextureItem );
    void                        DestroyShader               ( SShaderItem* pTextureItem );

protected:
    IDirect3DDevice9*           m_pDevice;
    std::set < SRenderItem* >   m_CreatedItemList;
};
