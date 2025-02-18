/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

namespace EPixelsFormat
{
    enum EPixelsFormatType
    {
        UNKNOWN,
        PLAIN,
        JPEG,
        PNG,
        DDS
    };
}
using EPixelsFormat::EPixelsFormatType;

//
// CPixels
//
// Pixels from a texture
//
class CPixels
{
public:
    ~CPixels() { dassert(externalData.pData == NULL || buffer.GetSize() == 0); }

    char*       GetData() { return externalData.pData ? externalData.pData : buffer.GetData(); }
    const char* GetData() const { return externalData.pData ? externalData.pData : buffer.GetData(); }
    uint        GetSize() const { return externalData.pData ? externalData.uiSize : buffer.GetSize(); }

    void SetSize(uint uiSize)
    {
        dassert(externalData.pData == NULL || buffer.GetSize() == 0);
        if (externalData.pData)
        {
            buffer = CBuffer(externalData.pData, externalData.uiSize);
            externalData = SCharStringRef();
        }
        buffer.SetSize(uiSize);
    }

    // Only one can be in use at a time
    CBuffer        buffer;
    SCharStringRef externalData;
};

///////////////////////////////////////////////////////////////
//
// CPixelsManagerInterface
//
//
///////////////////////////////////////////////////////////////
class CPixelsManagerInterface
{
public:
    virtual ~CPixelsManagerInterface() {}
    virtual void OnDeviceCreate(IDirect3DDevice9* pDevice) = 0;
    virtual bool IsPixels(const CPixels& pixels) = 0;
    virtual bool GetTexturePixels(IDirect3DBaseTexture9* pD3DTexture, CPixels& outPixels, EPixelsFormatType pixelsFormat = EPixelsFormat::PLAIN,
                                  ERenderFormat renderFormat = RFORMAT_UNKNOWN, bool bMipMaps = true, const RECT* pRect = NULL, uint uiSurfaceIndex = 0) = 0;
    virtual bool SetTexturePixels(IDirect3DBaseTexture9* pD3DTexture, const CPixels& pixels, const RECT* pRect = NULL, uint uiSurfaceIndex = 0) = 0;
    virtual bool GetPixelsSize(const CPixels& pixels, uint& uiOutWidth, uint& uiOutHeight) = 0;
    virtual EPixelsFormatType GetPixelsFormat(const CPixels& pixels) = 0;
    virtual bool              ChangePixelsFormat(const CPixels& oldPixels, CPixels& newPixels, EPixelsFormatType newFormat, int uiQuality = 0) = 0;
    virtual bool              GetPixelColor(const CPixels& pixels, uint uiPosX, uint uiPosY, SColor& outColor) = 0;
    virtual bool              SetPixelColor(CPixels& pixels, uint uiPosX, uint uiPosY, const SColor color) = 0;
};

CPixelsManagerInterface* NewPixelsManager();
