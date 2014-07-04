/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#define XRGB_BYTES_PER_PIXEL (4)
#define SIZEOF_PLAIN_TAIL (4)

///////////////////////////////////////////////////////////////
//
// CPixelsManager
//
//
///////////////////////////////////////////////////////////////
class CPixelsManager : public CPixelsManagerInterface
{
public:
    ZERO_ON_NEW
                                    CPixelsManager                      ( void );
                                    ~CPixelsManager                     ( void );

    // CPixelsManagerInterface
    virtual void                    OnDeviceCreate                      ( IDirect3DDevice9* pDevice );
    virtual bool                    IsPixels                            ( const CPixels& pixels );
    virtual bool                    GetTexturePixels                    ( IDirect3DBaseTexture9* pD3DBaseTexture, CPixels& outPixels, const RECT* pRect = NULL, uint uiSurfaceIndex = 0 );
    virtual bool                    SetTexturePixels                    ( IDirect3DBaseTexture9* pD3DBaseTexture, const CPixels& pixels, const RECT* pRect = NULL, uint uiSurfaceIndex = 0 );
    virtual bool                    GetPixelsSize                       ( const CPixels& pixels, uint& uiOutWidth, uint& uiOutHeight );
    virtual EPixelsFormatType       GetPixelsFormat                     ( const CPixels& pixels );
    virtual bool                    ChangePixelsFormat                  ( const CPixels& oldPixels, CPixels& newPixels, EPixelsFormatType newFormat, int uiQuality = 0 );
    virtual bool                    GetPixelColor                       ( const CPixels& pixels, uint uiPosX, uint uiPosY, SColor& outColor );
    virtual bool                    SetPixelColor                       ( CPixels& pixels, uint uiPosX, uint uiPosY, const SColor color );

    bool                            GetVolumeTexturePixels              ( IDirect3DVolumeTexture9* pD3DVolumeTexture, CPixels& outPixels, const RECT* pRect, uint uiSlice );
    bool                            SetVolumeTexturePixels              ( IDirect3DVolumeTexture9* pD3DVolumeTexture, const CPixels& pixels, const RECT* pRect, uint uiSlice );

    // CPixelsManager
    IDirect3DSurface9*              GetRTLockableSurface                ( IDirect3DSurface9* pRTSurface );
    bool                            GetMatchingOffscreenSurface         ( IDirect3DSurface9* pD3DSurface, IDirect3DSurface9*& pOffscreenSurface, D3DFORMAT ReqFormat = D3DFMT_UNKNOWN );
    bool                            GetSurfacePixels                    ( IDirect3DSurface9* pD3DSurface, CPixels& outPixels, const RECT* pRect );
    bool                            SetSurfacePixels                    ( IDirect3DSurface9* pD3DSurface, const CPixels& pixels, const RECT* pRect );
    bool                            SetPlainDimensions                  ( CPixels& pixels, uint uiWidth, uint uiHeight);
    bool                            GetPlainDimensions                  ( const CPixels& pixels, uint& uiOutWidth, uint& uiOutHeight );

    bool                            GetVolumePixels                     ( IDirect3DVolume9* pD3DVolume, CPixels& outPixels, const RECT* pRect, uint uiSlice );
    bool                            SetVolumePixels                     ( IDirect3DVolume9* pD3DVolume, const CPixels& pixels, const RECT* pRect, uint uiSlice );

    // Util
    static int                      GetRectWidth                        ( const RECT& rc );
    static int                      GetRectHeight                       ( const RECT& rc );
    static bool                     ClipRects                           ( const POINT& SrcSize, RECT& SrcRect, const POINT& DestSize, RECT& DestRect );
    static HRESULT                  LockSurfaceRect                     ( IDirect3DSurface9* pD3DSurface, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags );
    static HRESULT                  LockVolumeRect                      ( IDirect3DVolume9* pD3DVolume, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags, uint uiSlice );

protected:
    IDirect3DDevice9*               m_pDevice;
    IDirect3DSurface9*              m_pTempOffscreenSurface;
};
