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
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"         /* get library error codes too */
#include "cderror.h"        /* get application-specific error codes */

// Custom error handler for libjpeg - allows recovery instead of exit()
struct JpegErrorManager
{
    struct jpeg_error_mgr pub;
    jmp_buf               setjmp_buffer;
};

static void JpegErrorExit(j_common_ptr cinfo)
{
    JpegErrorManager* myerr = reinterpret_cast<JpegErrorManager*>(cinfo->err);
    longjmp(myerr->setjmp_buffer, 1);
}

bool IsJpeg(const void* pData, uint uiDataSize)
{
    if (!pData || uiDataSize == 0)
        return false;

    static uchar JpegHeader[] = {0xFF, 0xD8, 0xFF};
    static uchar JpegTail[] = {0xFF, 0xD9};

    // Need header (3), segment size (2 at offset 4-5), and tail (2)
    if (uiDataSize >= 6)
    {
        if (memcmp(pData, JpegHeader, sizeof(JpegHeader)) == 0 && memcmp((BYTE*)pData + uiDataSize - sizeof(JpegTail), JpegTail, sizeof(JpegTail)) == 0)
        {
            // Size of first segment
            uint uiSeg1Size = *((BYTE*)pData + 4) * 256;
            uiSeg1Size += *((BYTE*)pData + 5);
            if (uiSeg1Size + 5 < uiDataSize)
            {
                // Check first byte of second segement
                uchar ucSeg2Marker = *((BYTE*)pData + 4 + uiSeg1Size);
                if (ucSeg2Marker == 0xFF)
                    return true;
            }
        }
    }
    return false;
}

// Decode JPEG to XRGB
bool JpegDecode(const void* pData, uint uiDataSize, CBuffer* pOutBuffer, uint& uiOutWidth, uint& uiOutHeight, std::string* pOutError)
{
    if (!pData || uiDataSize == 0)
    {
        if (pOutError)
            *pOutError = "Invalid JPEG input data";
        return false;
    }

    struct jpeg_decompress_struct cinfo;
    JpegErrorManager              jerr;

    memset(&cinfo, 0, sizeof(cinfo));

    // Set up error handling
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = JpegErrorExit;

    if (setjmp(jerr.setjmp_buffer))
    {
        if (pOutError)
            *pOutError = "JPEG decode error (libjpeg internal error)";
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (uchar*)pData, uiDataSize);

    int headerResult = jpeg_read_header(&cinfo, TRUE);
    if (headerResult != JPEG_HEADER_OK)
    {
        if (pOutError)
            *pOutError = "Failed to read JPEG header";
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    // Validate dimensions (libjpeg max: 65500)
    if (cinfo.image_width == 0 || cinfo.image_height == 0 ||
        cinfo.image_width > JPEG_MAX_DIMENSION || cinfo.image_height > JPEG_MAX_DIMENSION)
    {
        if (pOutError)
            *pOutError = "Invalid JPEG dimensions: " + std::to_string(cinfo.image_width) + "x" + std::to_string(cinfo.image_height) + " (max " + std::to_string(JPEG_MAX_DIMENSION) + ")";
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    // Return dimensions only if no output buffer
    if (!pOutBuffer)
    {
        uiOutWidth = cinfo.image_width;
        uiOutHeight = cinfo.image_height;

        jpeg_destroy_decompress(&cinfo);
        return true;
    }

    cinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress(&cinfo);

    // Verify RGB output
    if (cinfo.output_components != 3)
    {
        if (pOutError)
            *pOutError = "Unexpected JPEG output components: " + std::to_string(cinfo.output_components) + " (expected 3)";
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    JDIMENSION uiWidth = cinfo.output_width;
    JDIMENSION uiHeight = cinfo.output_height;

    uiOutWidth = uiWidth;
    uiOutHeight = uiHeight;

    uint64 uiRequiredSize = static_cast<uint64>(uiWidth) * uiHeight * 4;
    if (uiRequiredSize > UINT_MAX)
    {
        if (pOutError)
            *pOutError = "JPEG dimensions too large for buffer: " + std::to_string(uiWidth) + "x" + std::to_string(uiHeight) + " requires " + std::to_string(uiRequiredSize) + " bytes";
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    // Allocate buffers before decode loop (longjmp safety)
    try
    {
        pOutBuffer->SetSize(static_cast<uint>(uiRequiredSize));
    }
    catch (...)
    {
        if (pOutError)
            *pOutError = "Out of memory allocating decode buffer (" + std::to_string(static_cast<uint>(uiRequiredSize)) + " bytes)";
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    char* pOutData = pOutBuffer->GetData();

    CBuffer rowBuffer;
    try
    {
        rowBuffer.SetSize(uiWidth * 3);
    }
    catch (...)
    {
        if (pOutError)
            *pOutError = "Out of memory allocating row buffer (" + std::to_string(uiWidth * 3) + " bytes)";
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    char* pRowTemp = rowBuffer.GetData();

    JSAMPROW row_pointer[1];
    while (cinfo.output_scanline < cinfo.output_height)
    {
        uint64 uiRowOffset = static_cast<uint64>(cinfo.output_scanline) * uiWidth * 4;
        BYTE* pRowDest = (BYTE*)pOutData + static_cast<size_t>(uiRowOffset);
        row_pointer[0] = (JSAMPROW)pRowTemp;

        JDIMENSION num_read = jpeg_read_scanlines(&cinfo, row_pointer, 1);
        if (num_read != 1)
        {
            if (pOutError)
                *pOutError = "Failed to read JPEG scanline " + std::to_string(cinfo.output_scanline) + " of " + std::to_string(cinfo.output_height) + " (truncated image?)";
            jpeg_destroy_decompress(&cinfo);
            return false;
        }

        // Convert RGB to BGRA
        for (uint i = 0; i < uiWidth; i++)
        {
            pRowDest[i * 4 + 0] = pRowTemp[i * 3 + 2];  // B
            pRowDest[i * 4 + 1] = pRowTemp[i * 3 + 1];  // G
            pRowDest[i * 4 + 2] = pRowTemp[i * 3 + 0];  // R
            pRowDest[i * 4 + 3] = 255;                   // A
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    // Treat libjpeg warnings as errors
    if (jerr.pub.num_warnings)
    {
        if (pOutError)
            *pOutError = "JPEG decode completed with " + std::to_string(static_cast<uint>(jerr.pub.num_warnings)) + " warning(s)";
        return false;
    }

    return true;
}

// Encode XRGB to JPEG
bool JpegEncode(uint uiWidth, uint uiHeight, uint uiQuality, const void* pData, uint uiDataSize, CBuffer& outBuffer, std::string* pOutError)
{
    if (!pData || uiWidth == 0 || uiHeight == 0)
    {
        if (pOutError)
            *pOutError = "Invalid JPEG encode input parameters";
        return false;
    }

    // Validate dimensions (libjpeg max: 65500)
    if (uiWidth > JPEG_MAX_DIMENSION || uiHeight > JPEG_MAX_DIMENSION)
    {
        if (pOutError)
            *pOutError = "JPEG dimensions too large: " + std::to_string(uiWidth) + "x" + std::to_string(uiHeight) + " (max " + std::to_string(JPEG_MAX_DIMENSION) + ")";
        return false;
    }

    uint64 uiExpectedSize = static_cast<uint64>(uiWidth) * uiHeight * 4;
    if (uiDataSize == 0)
    {
        if (pOutError)
            *pOutError = "Input buffer size is zero";
        return false;
    }
    if (uiExpectedSize > UINT_MAX)
    {
        if (pOutError)
            *pOutError = "Image dimensions cause buffer overflow: " + std::to_string(uiWidth) + "x" + std::to_string(uiHeight);
        return false;
    }
    if (uiDataSize != static_cast<uint>(uiExpectedSize))
    {
        if (pOutError)
            *pOutError = "Input buffer size mismatch: expected " + std::to_string(static_cast<uint>(uiExpectedSize)) + " bytes, got " + std::to_string(uiDataSize);
        return false;
    }

    struct jpeg_compress_struct cinfo;
    JpegErrorManager            jerr;

    memset(&cinfo, 0, sizeof(cinfo));
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = JpegErrorExit;

    if (setjmp(jerr.setjmp_buffer))
    {
        if (pOutError)
            *pOutError = "JPEG encode error (libjpeg internal error)";
        jpeg_destroy_compress(&cinfo);
        return false;
    }

    jpeg_create_compress(&cinfo);

    unsigned char* membuffer_ptr = nullptr;
    size_t memlen_val = 0;
    jpeg_mem_dest(&cinfo, &membuffer_ptr, &memlen_val);

    cinfo.image_width = uiWidth;
    cinfo.image_height = uiHeight;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, uiQuality, TRUE);
    cinfo.dct_method = JDCT_IFAST;

    jpeg_start_compress(&cinfo, TRUE);

    // Allocate row buffer before encode loop (longjmp safety)
    CBuffer rowBuffer;
    try
    {
        rowBuffer.SetSize(uiWidth * 3);
    }
    catch (...)
    {
        if (pOutError)
            *pOutError = "Out of memory allocating encode row buffer (" + std::to_string(uiWidth * 3) + " bytes)";
        jpeg_destroy_compress(&cinfo);
        if (membuffer_ptr)
            free(membuffer_ptr);
        return false;
    }

    char* pRowTemp = rowBuffer.GetData();

    bool bSuccess = false;
    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height)
    {
        uint64 uiRowOffset = static_cast<uint64>(cinfo.next_scanline) * uiWidth * 4;
        BYTE* pRowSrc = (BYTE*)pData + static_cast<size_t>(uiRowOffset);

        // Convert BGRA to RGB
        for (uint i = 0; i < uiWidth; i++)
        {
            pRowTemp[i * 3 + 0] = pRowSrc[i * 4 + 2];  // R
            pRowTemp[i * 3 + 1] = pRowSrc[i * 4 + 1];  // G
            pRowTemp[i * 3 + 2] = pRowSrc[i * 4 + 0];  // B
        }
        row_pointer[0] = (JSAMPROW)pRowTemp;

        JDIMENSION num_written = jpeg_write_scanlines(&cinfo, row_pointer, 1);
        if (num_written != 1)
        {
            if (pOutError)
                *pOutError = "Failed to write JPEG scanline " + std::to_string(cinfo.next_scanline) + " of " + std::to_string(cinfo.image_height);
            jpeg_destroy_compress(&cinfo);
            return false;
        }
    }

    jpeg_finish_compress(&cinfo);

    if (membuffer_ptr && memlen_val > 0)
    {
        try
        {
            outBuffer = CBuffer(membuffer_ptr, memlen_val);
            bSuccess = true;
        }
        catch (...)
        {
            if (pOutError)
                *pOutError = "Out of memory copying JPEG output (" + std::to_string(memlen_val) + " bytes)";
            jpeg_destroy_compress(&cinfo);
            free(membuffer_ptr);
            return false;
        }
    }
    else if (pOutError)
    {
        *pOutError = "JPEG compression produced no output";
    }

    jpeg_destroy_compress(&cinfo);
    if (membuffer_ptr)
        free(membuffer_ptr);

    return bSuccess;
}

// Get JPEG dimensions without decoding
bool JpegGetDimensions(const void* pData, uint uiDataSize, uint& uiOutWidth, uint& uiOutHeight, std::string* pOutError)
{
    return JpegDecode(pData, uiDataSize, nullptr, uiOutWidth, uiOutHeight, pOutError);
}
