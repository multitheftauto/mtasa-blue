#pragma once

#include <d3d9.h>
#include "TextureNative.h"

struct gtaBGRA { unsigned char b, g, r, a; };
struct gtaRGAB { unsigned char r, g, a, b; };

enum RasterFormat
{
    FORMATDEFAULT = 0,
    FORMAT1555 = 1,
    FORMAT565 = 2,
    FORMAT4444 = 3,
    FORMATLUM8 = 4,
    FORMAT8888 = 5,
    FORMAT888 = 6,
    FORMAT16 = 7,
    FORMAT24 = 8,
    FORMAT32 = 9,
    FORMAT555 = 10
};

enum _MYD3DFORMAT
{
    UNKNOWN0              =  0,

    R8G8B8               = 20,
    A8R8G8B8             = 21,
    X8R8G8B8             = 22,
    R5G6B5               = 23,
    X1R5G5B5             = 24,
    A1R5G5B5             = 25,
    A4R4G4B4             = 26,
    R3G3B2               = 27,
    A8                   = 28,
    A8R3G3B2             = 29,
    X4R4G4B4             = 30,
    A2B10G10R10          = 31,
    A8B8G8R8             = 32,
    X8B8G8R8             = 33,
    G16R16               = 34,
    A2R10G10B10          = 35,
    A16B16G16R16         = 36,

    A8P8                 = 40,
    P8                   = 41,

    L8                   = 50,
    A8L8                 = 51,
    A4L4                 = 52,

    V8U8                 = 60,
    L6V5U5               = 61,
    X8L8V8U8             = 62,
    Q8W8V8U8             = 63,
    V16U16               = 64,
    A2W10V10U10          = 67,

    UYVY                 = MAKEFOURCC('U', 'Y', 'V', 'Y'),
    R8G8_B8G8            = MAKEFOURCC('R', 'G', 'B', 'G'),
    YUY2                 = MAKEFOURCC('Y', 'U', 'Y', '2'),
    G8R8_G8B8            = MAKEFOURCC('G', 'R', 'G', 'B'),
    DXT1                 = MAKEFOURCC('D', 'X', 'T', '1'),
    DXT2                 = MAKEFOURCC('D', 'X', 'T', '2'),
    DXT3                 = MAKEFOURCC('D', 'X', 'T', '3'),
    DXT4                 = MAKEFOURCC('D', 'X', 'T', '4'),
    DXT5                 = MAKEFOURCC('D', 'X', 'T', '5'),

    D16_LOCKABLE         = 70,
    D32                  = 71,
    D15S1                = 73,
    D24S8                = 75,
    D24X8                = 77,
    D24X4S4              = 79,
    D16                  = 80,

    D32F_LOCKABLE        = 82,
    D24FS8               = 83,

/* D3D9Ex only -- */
#if !defined(D3D_DISABLE_9EX)

    /* Z-Stencil formats valid for CPU access */
    D32_LOCKABLE         = 84,
    S8_LOCKABLE          = 85,

#endif // !D3D_DISABLE_9EX
/* -- D3D9Ex only */


    L16                  = 81,

    VERTEXDATA           =100,
    INDEX16              =101,
    INDEX32              =102,

    Q16W16V16U16         =110,

    MULTI2_ARGB8         = MAKEFOURCC('M','E','T','1'),

    // Floating point surface formats

    // s10e5 formats (16-bits per channel)
    R16F                 = 111,
    G16R16F              = 112,
    A16B16G16R16F        = 113,

    // IEEE s23e8 formats (32-bits per channel)
    R32F                 = 114,
    G32R32F              = 115,
    A32B32G32R32F        = 116,

    CxV8U8               = 117,

/* D3D9Ex only -- */
#if !defined(D3D_DISABLE_9EX)

    // Monochrome 1 bit per pixel format
    A1                   = 118,

    // 2.8 biased fixed point
    A2B10G10R10_XR_BIAS  = 119,


    // Binary format indicating that the data has no inherent type
    BINARYBUFFER         = 199,
    
#endif // !D3D_DISABLE_9EX
/* -- D3D9Ex only */


    FORCE_DWORD          =0x7fffffff
};

#define D3DVal(x,y) case y: return #x;

extern char unknownformat[32];
 
const char* GetD3DFormatName(int nD3DFormat);

const char *GetRasterFormat(int format);

D3DFORMAT GetTextureFormat(gtaRwTextureNative *texture);

unsigned char GetFormatDepth(D3DFORMAT format);

bool GetFormatHasAlpha(D3DFORMAT format);

unsigned int GetFormatRwFormat(D3DFORMAT format);

bool GetFormatIsCompressed(D3DFORMAT format);

unsigned char GetFormatCompression(D3DFORMAT format);

struct _dxt1block
{
	unsigned short color_0; // 565 or 1555 (alpha)
	unsigned short color_1; // same
	unsigned int indices; // 2bits per pixel (0 - color_0, 1 - color_1, 2 - average, 3 - transparent)
};

// we will stick to this one instead of DXT1HasAlphaPixels() currently.
bool DXT1HasAlphaFormat(_dxt1block *blocks, unsigned int numBlocks);

bool DXT1HasAlphaPixels(_dxt1block *blocks, unsigned int numBlocks);