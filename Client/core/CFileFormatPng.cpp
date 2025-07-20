/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <libpng/png.h>

///////////////////////////////////////////////////////////////
//
// IsPng
//
// Check if probably png
//
///////////////////////////////////////////////////////////////
bool IsPng(const void* pData, uint uiDataSize)
{
    static uchar PngHeader[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    static uchar PngTail[] = {0xAE, 0x42, 0x60, 0x82};

    if (uiDataSize >= sizeof(PngHeader))
    {
        if (memcmp(pData, PngHeader, sizeof(PngHeader)) == 0 && memcmp((BYTE*)pData + uiDataSize - sizeof(PngTail), PngTail, sizeof(PngTail)) == 0)
        {
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////
//
// Callback for PngDecode
//
//
///////////////////////////////////////////////////////////////
void ParseRGBA(CBuffer& outImage, const png_structp& png_ptr, const png_infop& info_ptr, const uint width, const uint height)
{
    SColor* pData = (SColor*)outImage.GetData();

    const png_uint_32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
    byte*             rowData = new byte[bytesPerRow];

    // read single row at a time
    for (uint rowIdx = 0; rowIdx < height; ++rowIdx)
    {
        png_read_row(png_ptr, (png_bytep)rowData, NULL);

        const uint rowOffset = rowIdx * width;

        uint byteIndex = 0;
        for (uint colIdx = 0; colIdx < width; ++colIdx)
        {
            pData->R = rowData[byteIndex++];
            pData->G = rowData[byteIndex++];
            pData->B = rowData[byteIndex++];
            pData->A = rowData[byteIndex++];
            pData++;
        }
        assert(byteIndex == bytesPerRow);
    }

    delete[] rowData;
}

///////////////////////////////////////////////////////////////
//
// Callback for PngDecode
//
//
///////////////////////////////////////////////////////////////
void ParseRGB(CBuffer& outImage, const png_structp& png_ptr, const png_infop& info_ptr, const uint width, const uint height)
{
    SColor* pData = (SColor*)outImage.GetData();

    const png_uint_32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
    byte*             rowData = new byte[bytesPerRow];

    // read single row at a time
    for (uint rowIdx = 0; rowIdx < height; ++rowIdx)
    {
        png_read_row(png_ptr, (png_bytep)rowData, NULL);

        const uint rowOffset = rowIdx * width;

        uint byteIndex = 0;
        for (uint colIdx = 0; colIdx < width; ++colIdx)
        {
            pData->R = rowData[byteIndex++];
            pData->G = rowData[byteIndex++];
            pData->B = rowData[byteIndex++];
            pData->A = 255;
            pData++;
        }
        assert(byteIndex == bytesPerRow);
    }

    delete[] rowData;
}

///////////////////////////////////////////////////////////////
//
// Callback for PngDecode
//
//
///////////////////////////////////////////////////////////////
void ReadDataFromInputStream(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
{
    if (!png_ptr)
        return;

    void* ioPtr = png_get_io_ptr(png_ptr);

    if (!ioPtr)
        return;

    CBufferReadStream& stream = *(CBufferReadStream*)ioPtr;
    stream.ReadBytes((byte*)outBytes, (size_t)byteCountToRead);
}

///////////////////////////////////////////////////////////////
//
// PngDecode
//
// png to XRGB
//
///////////////////////////////////////////////////////////////
bool PngDecode(const void* pData, uint uiDataSize, CBuffer* pOutBuffer, uint& uiOutWidth, uint& uiOutHeight)
{
    CBuffer           inBuffer(pData, uiDataSize);
    CBufferReadStream stream(inBuffer);

    enum
    {
        kPngSignatureLength = 8
    };
    byte pngSignature[kPngSignatureLength];

    stream.ReadBytes(pngSignature, kPngSignatureLength);

    if (!png_check_sig(pngSignature, kPngSignatureLength))
        return false;

    ///////////////////////////////////////////////////
    // get PNG file info struct (memory is allocated by libpng)
    png_structp png_ptr = NULL;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL)
        return false;

    ///////////////////////////////////////////////////
    // get PNG image data info struct (memory is allocated by libpng)
    png_infop info_ptr = NULL;
    info_ptr = png_create_info_struct(png_ptr);

    if (info_ptr == NULL)
    {
        // libpng must free file info struct memory before we bail
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;
    }

    png_set_read_fn(png_ptr, &stream, ReadDataFromInputStream);

    // tell libpng we already read the signature
    png_set_sig_bytes(png_ptr, kPngSignatureLength);

    ///////////////////////////////////////////////////
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width = 0;
    png_uint_32 height = 0;
    int         bitDepth = 0;
    int         colorType = -1;
    png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);

    if (retval != 1)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    ///////////////////////////////////////////////////
    uiOutWidth = width;
    uiOutHeight = height;

    if (pOutBuffer)
    {
        pOutBuffer->SetSize(width * height * 4);

        switch (colorType)
        {
            case PNG_COLOR_TYPE_RGB:
                ParseRGB(*pOutBuffer, png_ptr, info_ptr, width, height);
                break;

            case PNG_COLOR_TYPE_RGB_ALPHA:
                ParseRGBA(*pOutBuffer, png_ptr, info_ptr, width, height);
                break;

            default:
                png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                return false;
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return true;
}

///////////////////////////////////////////////////////////////
//
// Callback for PngEncode
//
//
///////////////////////////////////////////////////////////////
void my_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
    if (!png_ptr)
        return;

    void* io_ptr = png_get_io_ptr(png_ptr);

    if (!io_ptr)
        return;

    CBufferWriteStream* stream = (CBufferWriteStream*)io_ptr;
    stream->WriteBytes(data, length);
}


///////////////////////////////////////////////////////////////
//
// PngEncode
//
// XRGB to png
//
///////////////////////////////////////////////////////////////
bool PngEncode(uint uiWidth, uint uiHeight, const void* pData, uint uiDataSize, CBuffer& outBuffer)
{
    // Create the screen data buffer
    BYTE** ppScreenData = NULL;
    ppScreenData = new BYTE*[uiHeight];
    for (unsigned short y = 0; y < uiHeight; y++)
    {
        ppScreenData[y] = new BYTE[uiWidth * 4];
    }

    // Copy the surface data into a row-based buffer for libpng
    unsigned long ulLineWidth = uiWidth * 4;
    for (unsigned int i = 0; i < uiHeight; i++)
    {
        memcpy(ppScreenData[i], (BYTE*)pData + i * ulLineWidth, ulLineWidth);
    }

    CBufferWriteStream stream(outBuffer);

    png_struct* png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_info*   info_ptr = png_create_info_struct(png_ptr);
    png_set_write_fn(png_ptr, &stream, my_png_write_data, NULL);
    png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
    png_set_compression_level(png_ptr, 1);
    png_set_IHDR(png_ptr, info_ptr, uiWidth, uiHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_rows(png_ptr, info_ptr, ppScreenData);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR /*| PNG_TRANSFORM_STRIP_ALPHA*/, NULL);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    // Clean up the screen data buffer
    if (ppScreenData)
    {
        for (unsigned short y = 0; y < uiHeight; y++)
        {
            delete[] ppScreenData[y];
        }
        delete[] ppScreenData;
    }

    return true;
}

///////////////////////////////////////////////////////////////
//
// PngGetDimensions
//
//
//
///////////////////////////////////////////////////////////////
bool PngGetDimensions(const void* pData, uint uiDataSize, uint& uiOutWidth, uint& uiOutHeight)
{
    return PngDecode(pData, uiDataSize, nullptr, uiOutWidth, uiOutHeight);
}
