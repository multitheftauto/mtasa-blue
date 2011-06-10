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
    virtual void                ReleaseRenderItem           ( SRenderItem* pItem );

    // CRenderItemManager
    void                        OnDeviceCreate              ( IDirect3DDevice9* pDirect3DDevice9 );
    void                        DestroyFont                 ( SFontItem* pFontItem );
    void                        DestroyTexture              ( STextureItem* pTextureItem );

protected:
    IDirect3DDevice9*           m_pD3DDevice;
    std::set < SRenderItem* >   m_CreatedItemList;
};
