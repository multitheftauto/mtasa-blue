/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/RenderWareD3D.h
 *  PURPOSE:     RenderWare-compatible definitions for
 *               Grand Theft Auto: San Andreas
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

// From SA::Render
struct RwD3D9Raster
{
    union
    {
        IDirect3DTexture9* texture;
        IDirect3DSurface9* surface;
    };
    std::uint8_t*       palette;
    std::uint8_t        alpha;
    std::uint8_t        cubeTextureFlags; /* 0x01 IS_CUBEMAP_TEX */
    std::uint8_t        textureFlags;     /* 0x10 IS_COMPRESSED */
    std::uint8_t        lockedLevel;
    IDirect3DSurface9*   lockedSurface;
    D3DLOCKED_RECT       lockedRect;
    D3DFORMAT            format;
    IDirect3DSwapChain9* swapChain;
    HWND*                hwnd;
};

// From gtamodding.com
struct NativeTexturePC_Header
{
    struct
    {
        std::uint32_t platformId;
        std::uint32_t filterMode : 8;
        std::uint32_t uAddressing : 4;
        std::uint32_t vAddressing : 4;
        std::uint32_t pad : 16;
        char         name[32];
        char         maskName[32];
    } TextureFormat;

    struct
    {
        std::uint32_t   rasterFormat;
        D3DFORMAT      d3dFormat;
        std::uint16_t width;
        std::uint16_t height;
        std::uint8_t  depth;
        std::uint8_t  numLevels;
        std::uint8_t  rasterType;
        std::uint8_t  alpha : 1;
        std::uint8_t  cubeTexture : 1;
        std::uint8_t  autoMipMaps : 1;
        std::uint8_t  compressed : 1;
        std::uint8_t  pad : 4;
    } RasterFormat;
};
