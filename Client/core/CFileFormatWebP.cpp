/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CFileFormatWebP.cpp
 *  PURPOSE:     WebP image format helpers (decode only).
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <webp/decode.h>

///////////////////////////////////////////////////////////////
//
// IsWebP
//
// Check for the RIFF....WEBP magic.
//
///////////////////////////////////////////////////////////////
bool IsWebP(const void* pData, uint uiDataSize)
{
    if (uiDataSize < 12)
        return false;

    const unsigned char* p = static_cast<const unsigned char*>(pData);
    return p[0] == 'R' && p[1] == 'I' && p[2] == 'F' && p[3] == 'F' && p[8] == 'W' && p[9] == 'E' && p[10] == 'B' && p[11] == 'P';
}

///////////////////////////////////////////////////////////////
//
// WebPGetDimensions
//
///////////////////////////////////////////////////////////////
bool WebPGetDimensions(const void* pData, uint uiDataSize, uint& uiOutWidth, uint& uiOutHeight)
{
    int width = 0;
    int height = 0;
    if (!WebPGetInfo(static_cast<const uint8_t*>(pData), uiDataSize, &width, &height))
        return false;

    if (width <= 0 || height <= 0)
        return false;

    uiOutWidth = static_cast<uint>(width);
    uiOutHeight = static_cast<uint>(height);
    return true;
}

///////////////////////////////////////////////////////////////
//
// WebPDecode
//
// Decodes into BGRA byte order to match SColor / D3DFMT_A8R8G8B8 memory layout
//
///////////////////////////////////////////////////////////////
bool WebPDecode(const void* pData, uint uiDataSize, CBuffer* pOutBuffer, uint& uiOutWidth, uint& uiOutHeight)
{
    if (!pOutBuffer)
        return false;

    int width = 0;
    int height = 0;
    if (!WebPGetInfo(static_cast<const uint8_t*>(pData), uiDataSize, &width, &height))
        return false;

    if (width <= 0 || height <= 0)
        return false;

    const size_t stride = static_cast<size_t>(width) * 4;
    const size_t total = stride * static_cast<size_t>(height);

    pOutBuffer->SetSize(static_cast<uint>(total));

    uint8_t* pDst = reinterpret_cast<uint8_t*>(pOutBuffer->GetData());
    if (!WebPDecodeBGRAInto(static_cast<const uint8_t*>(pData), uiDataSize, pDst, static_cast<size_t>(total), static_cast<int>(stride)))
    {
        return false;
    }

    uiOutWidth = static_cast<uint>(width);
    uiOutHeight = static_cast<uint>(height);
    return true;
}
